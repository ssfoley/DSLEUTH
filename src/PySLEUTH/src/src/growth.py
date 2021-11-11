from igrid import IGrid
from processing import Processing
from globals import Globals
from scenario import Scenario
from utilities import Utilities
from logger import Logger
from spread import Spread
from stats import Stats
from imageIO import ImageIO
from color import Color
from coeff import Coeff
from landClass import LandClass
from pgrid import PGrid
from transition import Transition
from delta import Deltatron
from grid import Grid
from input import Input
from output import Output
import os
import sys


class Grow:
    @staticmethod
    def grow(z, land1):
        deltatron = PGrid.get_deltatron()
        avg_slope = 0

        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            Processing.set_current_year(Scenario.get_scen_value('prediction_start_date'))
        else:
            Processing.set_current_year(IGrid.igrid.get_urban_year(0))

        Utilities.init_grid(z.gridData)
        # print(z.gridData)
        if len(Scenario.get_scen_value('landuse_data_file')) > 0:
            Grow.landuse_init(deltatron.gridData, land1.gridData)

        seed = IGrid.igrid.get_urban_grid(0)
        Utilities.condition_gif(seed, z.gridData)

        if Scenario.get_scen_value('echo'):
            print("******************************************")
            if Processing.get_processing_type() == Globals.mode_enum['calibrate']:
                c_run = Processing.get_current_run()
                t_run = Processing.get_total_runs()
                print(f"Run = {c_run} of {t_run}"
                      f" ({100 * c_run / t_run:8.1f} percent complete)")

            print(f"Monte Carlo = {int(Processing.get_current_monte()) + 1} of "
                  f"{Scenario.get_scen_value('monte_carlo_iterations')}")
            print(f"Processing.current_year = {Processing.get_current_year()}")
            print(f"Processing.stop_year = {Processing.get_stop_year()}")

        if Scenario.get_scen_value('logging') and int(Scenario.get_scen_value('log_processing_status')) > 0:
            Grow.completion_status()

        while Processing.get_current_year() < Processing.get_stop_year():
            # Increment Current Year
            Processing.increment_current_year()

            cur_yr = Processing.get_current_year()
            if Scenario.get_scen_value('echo'):
                print(f" {cur_yr}", end='')
                sys.stdout.flush()
                if (cur_yr + 1) % 10 == 0 or cur_yr == Processing.get_stop_year():
                    print()

            if Scenario.get_scen_value('logging'):
                Logger.log(f" {cur_yr}")
                if (cur_yr + 1) % 10 == 0 or cur_yr == Processing.get_stop_year():
                    Logger.log("")

            # Apply the Cellular Automaton Rules for this Year
            avg_slope, num_growth_pix, sng, sdc, og, rt, pop = Spread.spread(z, avg_slope)
            #print(f"rt: {rt}")
            sdg = 0  # this isn't passed into spread, but I don't know why then it's here
            Stats.set_sng(sng)
            Stats.set_sdg(sdc)
            #Stats.set_sdc(sdc)
            Stats.set_og(og)
            Stats.set_rt(rt)
            Stats.set_pop(pop)

            if Scenario.get_scen_value('view_growth_types'):
                if IGrid.using_gif:
                    filename = f"{Scenario.get_scen_value('output_dir')}z_growth_types" \
                               f"_{Processing.get_current_run()}_{Processing.get_current_monte()}_" \
                               f"{Processing.get_current_year()}.gif"
                else:
                    filename = f"{Scenario.get_scen_value('output_dir')}z_growth_types" \
                               f"_{Processing.get_current_run()}_{Processing.get_current_monte()}_" \
                               f"{Processing.get_current_year()}.tif"

                date = str(Processing.get_current_year())
                ImageIO.write_gif(z, Color.get_growth_table(), filename, date, IGrid.nrows, IGrid.ncols)

            if len(Scenario.get_scen_value('landuse_data_file')) > 0:
                Grow.grow_landuse(land1, num_growth_pix)
            else:
                Grow.grow_non_landuse(z.gridData)

            seed = IGrid.igrid.get_urban_grid(0)
            Utilities.condition_gif(seed, z.gridData)

            # do Statistics
            Stats.update(num_growth_pix)

            # do Self Modification
            Coeff.self_modify(Stats.get_growth_rate(), Stats.get_percent_urban())
            Coeff.write_current_coeff(Processing.get_current_run(), Processing.get_current_monte(), Processing.get_current_year())

    @staticmethod
    def landuse_init(deltatron, land1):
        total_pixels = IGrid.nrows * IGrid.ncols

        # Initialize Deltatron Grid to Zero
        for pixel in deltatron:
            pixel = 0

        if Processing.get_processing_type() == Globals.mode_enum['predict']:
            landuse = IGrid.igrid.get_landuse_igrid(1)
            for i in range(total_pixels):
                land1[i] = landuse[i]

        else:
            landuse = IGrid.igrid.get_landuse_igrid(0)
            for i in range(total_pixels):
                land1[i] = landuse[i]

    @staticmethod
    def grow_landuse(land1, num_growth_pix):
        nrows = IGrid.nrows
        ncols = IGrid.ncols
        ticktock = Processing.get_current_year()
        landuse0_year = IGrid.igrid.get_landuse_year(0)
        landuse1_year = IGrid.igrid.get_landuse_year(1)
        urban_code = LandClass.get_urban_code()
        new_indices = LandClass.get_new_indices()
        landuse_classes = LandClass.get_landclasses()
        class_indices = LandClass.get_reduced_classes()
        background = IGrid.igrid.get_background()
        slope = IGrid.igrid.get_slope()
        deltatron = PGrid.get_deltatron()
        z = PGrid.get_z()
        land2 = PGrid.get_land2()
        class_slope = Transition.get_class_slope()
        ftransition = Transition.get_ftransition()

        if ticktock >= landuse0_year:
            # Place the New Urban Simulation into the Land Use Image
            Utilities.condition_gt_gif(z.gridData, 0, land1.gridData, urban_code)
            Deltatron.deltatron(new_indices, landuse_classes, class_indices, deltatron, land1, land2,
                                slope, num_growth_pix, class_slope, ftransition)

            # Switch the old to the new
            for i in range(len(land2.gridData)):
                land1.gridData[i] = land2.gridData[i]

        if Processing.get_processing_type() == Globals.mode_enum['predict'] or \
            (Processing.get_processing_type() == Globals.mode_enum['test'] and
             Processing.get_current_monte() == Processing.get_last_monte()):
            #Write land1 to file
            if IGrid.using_gif:
                filename = f"{Scenario.get_scen_value('output_dir')}{IGrid.igrid.location}_land_n_urban" \
                           f".{Processing.get_current_year()}.gif"
            else:
                filename = f"{Scenario.get_scen_value('output_dir')}{IGrid.igrid.location}_land_n_urban" \
                           f".{Processing.get_current_year()}.tif"
                IGrid.echo_meta(f"{Scenario.get_scen_value('output_dir')}{IGrid.igrid.location}_land_n_urban."
                                f"{Processing.get_current_year()}.tfw", "landuse")

            date = f"{Processing.get_current_year()}"
            ImageIO.write_gif(land1, Color.get_landuse_table(), filename, date, nrows, ncols)

        # Compute final match statistics for landuse
        Utilities.condition_gt_gif(z.gridData, 0, land1.gridData, urban_code)

    @staticmethod
    def grow_non_landuse(z):
        num_monte = int(Scenario.get_scen_value('monte_carlo_iterations'))
        cumulate_monte_carlo = Grid()
        filename = f"{Scenario.get_scen_value('output_dir')}cumulate_monte_carlo.year_{Processing.get_current_year()}"

        if Processing.get_processing_type() != Globals.mode_enum['calibrate']:
            if Processing.get_current_monte() == 0:
                # Zero out accumulation grid
                cumulate_monte_carlo.init_grid_data(IGrid.total_pixels)
            else:
                Input.read_file_to_grid(filename, cumulate_monte_carlo)

            # Accumulate Z over monte carlos
            for i in range(IGrid.total_pixels):
                if z[i] > 0:
                    cumulate_monte_carlo.gridData[i] += 1

            if Processing.get_current_monte() == num_monte - 1:
                if Processing.get_processing_type() == Globals.mode_enum['test']:
                    Utilities.condition_gt_gif(z, 0, cumulate_monte_carlo.gridData, 100)
                else:
                    # Normalize Accumulated grid
                    for i in range(IGrid.total_pixels):
                        cumulate_monte_carlo.gridData[i] = 100 * cumulate_monte_carlo.gridData[i] / num_monte

                Utilities.write_z_prob_grid(cumulate_monte_carlo, "_urban_")
                if Processing.get_current_monte() != 0:
                    os.remove(filename)
            else:
                # Dump accumulated grid to disk
                Output.write_grid_to_file(filename, cumulate_monte_carlo)

    @staticmethod
    def completion_status():
        mc_iters = int(Scenario.get_scen_value('monte_carlo_iterations'))
        cur_mc = int(Processing.get_current_monte())
        total_runs = int(Processing.get_total_runs())

        total_mc = (mc_iters * total_runs) / int(Globals.npes)
        total_mc_executed = mc_iters * Processing.get_num_runs_exec_this_cpu() + cur_mc
        complete = min(total_mc_executed / total_mc, 1.0)

        Logger.log(f"Run= {Processing.get_current_run()} of {total_runs} MC= {cur_mc} of {mc_iters}")





