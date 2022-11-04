from processing import Processing
from globals import Globals
from scenario import Scenario
from landClass import LandClass
from igrid import IGrid
from pgrid import PGrid
from coeff import Coeff
from stats import Stats
from growth import Grow
from color import Color
from imageIO import ImageIO
from utilities import Utilities
from timer import TimerUtility


class Driver:
    @staticmethod
    def driver():
        TimerUtility.start_timer('drv_driver')
        name = "_cumcolor_urban_"
        output_dir = Scenario.get_scen_value("output_dir")
        landuse_flag = len(Scenario.get_scen_value("landuse_data_file")) > 0
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        total_pixels = IGrid.get_total_pixels()
        z_cumulate = PGrid.get_cumulate()
        sim_landuse = PGrid.get_land1()

        # Create Annual Landuse Probability File
        if Processing.get_processing_type() == Globals.mode_enum["predict"]:
            if landuse_flag:
                LandClass.init_annual_prob(total_pixels)

        # Monte Carlo Simulation
        Driver.monte_carlo(z_cumulate, sim_landuse)

        if Processing.get_processing_type() == Globals.mode_enum["predict"]:
            # Output Urban Images
            if IGrid.using_gif:
                filename = f"{output_dir}cumulate_urban.gif"
            else:
                filename = f"{output_dir}cumulate_urban.tif"
                IGrid.echo_meta(f"{output_dir}cumulate_urban.tfw", "urban")
            colortable = Color.get_grayscale_table()

            ImageIO.write_gif(z_cumulate, colortable, filename, "",
                              nrows, ncols)
            Utilities.write_z_prob_grid(z_cumulate.gridData, name)

            if landuse_flag:
                cum_prob, cum_uncert = LandClass.build_prob_image(total_pixels)
                #print(cum_prob)

                # Output Cumulative Prob Image
                if IGrid.using_gif:
                    filename = f"{output_dir}cumcolor_landuse.gif"
                else:
                    filename = f"{output_dir}cumcolor_landuse.tif"
                    IGrid.echo_meta(f"{output_dir}cumcolor_landuse.tfw", "landuse")
                cum_prob_grid = IGrid.wrap_list(cum_prob)
                ImageIO.write_gif(cum_prob_grid, Color.get_landuse_table(), filename, "", nrows,
                                  ncols)

                # Output Cumulative Uncertainty Image
                if IGrid.using_gif:
                    filename = f"{output_dir}uncertainty.landuse.gif"
                else:
                    filename = f"{output_dir}uncertainty.landuse.tif"
                    IGrid.echo_meta(f"{output_dir}uncertainty.landuse.tfw", "landuse")
                cum_uncert_grid = IGrid.wrap_list(cum_uncert)
                ImageIO.write_gif(cum_uncert_grid, Color.get_grayscale_table(), filename, "", nrows,
                                  ncols)

        if not landuse_flag or Processing.get_processing_type() == Globals.mode_enum['predict']:
            fmatch = 0.0
        else:
            landuse1 = IGrid.igrid.get_landuse_igrid(1)
            fmatch = Driver.fmatch(sim_landuse, landuse1, landuse_flag, total_pixels)

        Stats.analyze(fmatch)
        TimerUtility.stop_timer('drv_driver')

        # Need to call Total_Time timer in main.c to stop from overflowing
        #TimerUtility.stop_timer('total_time')
        #TimerUtility.start_timer('total_time')

    @staticmethod
    def monte_carlo(cumulate, land1):
        log_it = Scenario.get_scen_value("logging")

        z = PGrid.get_z()
        total_pixels = IGrid.get_total_pixels()
        num_monte_carlo = int(Scenario.get_scen_value("monte_carlo_iterations"))

        for imc in range(num_monte_carlo):
            Processing.set_current_monte(imc)

            '''print("--------Saved-------")
            print(Coeff.get_saved_diffusion())
            print(Coeff.get_saved_spread())
            print(Coeff.get_saved_breed())
            print(Coeff.get_saved_slope_resistance())
            print(Coeff.get_saved_road_gravity())
            print("--------------------")'''

            # Reset the Parameters
            # use the getters and setters for dealing with corner cases
            Coeff.set_current_diffusion(Coeff.get_saved_diffusion())
            Coeff.set_current_spread(Coeff.get_saved_spread())
            Coeff.set_current_breed(Coeff.get_saved_breed())
            Coeff.set_current_slope_resistance(Coeff.get_saved_slope_resistance())
            Coeff.set_current_road_gravity(Coeff.get_saved_road_gravity())

            if log_it and Scenario.get_scen_value("log_initial_coefficients"):
                Coeff.log_current()

            # Run Simulation
            Stats.init_urbanization_attempts()
            TimerUtility.start_timer('grw_growth')
            Grow.grow(z, land1)
            TimerUtility.stop_timer('grw_growth')

            if log_it and Scenario.get_scen_value("log_urbanization_attempts"):
                Stats.log_urbanization_attempts()

            # Update Cumulate Grid
            for i in range(total_pixels):
                if z.gridData[i] > 0:
                    cumulate.gridData[i] += 1

            # Update Annual Land Class Probabilities
            if Processing.get_processing_type() == Globals.mode_enum["predict"]:
                LandClass.update_annual_prob(land1.gridData, total_pixels)

        # Normalize Cumulative Urban Image
        for i in range(total_pixels):
            cumulate.gridData[i] = (100 * cumulate.gridData[i]) / num_monte_carlo

    @staticmethod
    def fmatch(cum_probability, landuse1, landuse_flag, total_pixels):
        if not landuse_flag:
            fmatch = 1.0
        else:
            match_count = Utilities.img_intersection(cum_probability.gridData, landuse1)
            trans_count = total_pixels - match_count

            if match_count == 0 and trans_count == 0:
                fmatch = 0.0
            else:
                fmatch = match_count / (match_count + trans_count)

        return fmatch

