import sys
import random
from processing import Processing
from scenario import Scenario
from landClass import LandClass, LanduseMeta
from input import Input
from coeff import Coeff
from igrid import IGrid
from globals import Globals
from color import Color
from transition import Transition
from logger import Logger
from stats import Stats
from driver import Driver
from pgrid import PGrid
from ugm_defines import UGMDefines
from output import Output
from timer import TimerUtility
import traceback


def main():
    TimerUtility.start_timer('total_time')
    valid_modes = ["predict", "restart", "test", "calibrate"]

    Globals.mype = 0
    Globals.npes = 1
    packing = False
    restart_run = 0

    # Parse command line

    if len(sys.argv) != 3:
        __print_usage(sys.argv[0])
        sys.exit(1)

    if len(sys.argv) != 3 or sys.argv[1] not in valid_modes:
        __print_usage(sys.argv[0])
        sys.exit(1)

    Processing.set_processing_type(Globals.mode_enum[sys.argv[1]])

    if Processing.get_processing_type() == Globals.mode_enum['restart']:
        Processing.set_restart_flag(True)

    Scenario.init(sys.argv[2], Processing.get_restart_flag())

    try:

        log_it = Scenario.get_scen_value("logging")
        random_seed = Scenario.get_scen_value("random_seed")
        random.seed(random_seed)

        landuse_class_info = Scenario.get_scen_value("landuse_class_info")
        LandClass.num_landclasses = len(landuse_class_info)
        # filling in the class array in Land_Class
        for i, landuse_class in enumerate(landuse_class_info):
            # num, class_id, name, idx, hexColor
            landuse_class_meta = LanduseMeta(landuse_class.grayscale, landuse_class.type, landuse_class.name, i,
                                             landuse_class.color[2:])
            LandClass.landuse_classes.append(landuse_class_meta)

        # Set up Coefficients
        if sys.argv[1] == 'restart':
            if log_it:
                print("Implement log here")

            diffusion, breed, spread, slope_resistance, road_gravity, random_seed, restart_run = \
                Input.read_restart_file(Scenario.get_scen_value("output_dir"))
            Processing.set_current_run(restart_run)

        else:
            Processing.set_current_run(0)

        Coeff.set_start_coeff(Scenario.get_scen_value("calibration_diffusion_start"),
                              Scenario.get_scen_value("calibration_spread_start"),
                              Scenario.get_scen_value("calibration_breed_start"),
                              Scenario.get_scen_value("calibration_slope_start"),
                              Scenario.get_scen_value("calibration_road_start"))
        Coeff.set_stop_coeff(Scenario.get_scen_value("calibration_diffusion_stop"),
                             Scenario.get_scen_value("calibration_spread_stop"),
                             Scenario.get_scen_value("calibration_breed_stop"),
                             Scenario.get_scen_value("calibration_slope_stop"),
                             Scenario.get_scen_value("calibration_road_stop"))
        Coeff.set_step_coeff(Scenario.get_scen_value("calibration_diffusion_step"),
                             Scenario.get_scen_value("calibration_spread_step"),
                             Scenario.get_scen_value("calibration_breed_step"),
                             Scenario.get_scen_value("calibration_slope_step"),
                             Scenario.get_scen_value("calibration_road_step"))
        Coeff.set_best_fit_coeff(Scenario.get_scen_value("prediction_diffusion_best_fit"),
                                 Scenario.get_scen_value("prediction_spread_best_fit"),
                                 Scenario.get_scen_value("prediction_breed_best_fit"),
                                 Scenario.get_scen_value("prediction_slope_best_fit"),
                                 Scenario.get_scen_value("prediction_road_best_fit"))

        # Initial IGrid
        IGrid.init(packing, Processing.get_processing_type())

        '''
        Skipped memory and logging stuff for now, don't know if I'll need it
        If there is a problem, I can go back and implement
        '''

        # Initialize Landuse
        if len(Scenario.get_scen_value("landuse_data_file")) > 0:
            LandClass.init()
            if Scenario.get_scen_value("log_landclass_summary"):
                if log_it:
                    # this is where we would log
                    Logger.log("Test log")

        # Initialize Colortables
        Color.init(IGrid.ncols)

        # Read and validate input
        IGrid.read_input_files(packing, Scenario.get_scen_value("echo_image_files"),
                               Scenario.get_scen_value("output_dir"))
        IGrid.validate_grids(log_it)

        # Normalize Roads
        IGrid.normalize_roads()

        landuse_flag = len(Scenario.get_scen_value("landuse_data_file")) != 0
        IGrid.verify_inputs(log_it, landuse_flag)

        # Initialize PGRID Grids
        PGrid.init(IGrid.get_total_pixels())

        if log_it and Scenario.get_scen_value("log_colortables"):
            Color.log_colors()

        # Count the Number of Runs
        Processing.set_total_runs()
        Processing.set_last_monte(int(Scenario.get_scen_value("monte_carlo_iterations")) - 1)
        if log_it:
            if Processing.get_processing_type() == Globals.mode_enum["calibrate"]:
                Logger.log(f"Total Number of Runs = {Processing.get_total_runs()}")

        # Compute Transition Matrix
        if len(Scenario.get_scen_value("landuse_data_file")) > 0:
            Transition.create_matrix()
            if log_it and Scenario.get_scen_value("log_transition_matrix"):
                Transition.log_transition()

        # Compute the Base Statistics against which the calibration will take place
        Stats.set_base_stats()
        if log_it and Scenario.get_scen_value("log_base_statistics"):
            Stats.log_base_stats()

        if log_it and Scenario.get_scen_value("log_debug"):
            IGrid.debug("main.py")

        Processing.set_num_runs_exec_this_cpu(0)
        if Processing.get_current_run() == 0 and Globals.mype == 0:
            output_dir = Scenario.get_scen_value("output_dir")
            if Processing.get_processing_type() != Globals.mode_enum["predict"]:
                filename = f"{output_dir}control_stats.log"
                Stats.create_control_file(filename)

            if Scenario.get_scen_value("write_std_dev_file"):
                filename = f"{output_dir}std_dev.log"
                Stats.create_stats_val_file(filename)

            if Scenario.get_scen_value("write_avg_file"):
                filename = f"{output_dir}avg.log"
                Stats.create_stats_val_file(filename)

        if Scenario.get_scen_value("write_coeff_file"):
            output_dir = Scenario.get_scen_value("output_dir")
            filename = f"{output_dir}coeff.log"
            Coeff.create_coeff_file(filename, True)

        if Processing.get_processing_type() == Globals.mode_enum["predict"]:
            # Prediction Runs
            Processing.set_stop_year(Scenario.get_scen_value("prediction_stop_date"))
            Coeff.set_current_coeff(Coeff.get_best_diffusion(), Coeff.get_best_spread(),
                                    Coeff.get_best_breed(), Coeff.get_best_slope_resistance(),
                                    Coeff.get_best_road_gravity())
            if Globals.mype == 0:
                Driver.driver()
                Processing.increment_num_runs_exec_this_cpu()

            # Timing stuff
            if log_it and int(Scenario.get_scen_value('log_timings')) > 1:
                TimerUtility.log_timers()

        else:
            # Calibration and Test Runs
            Processing.set_stop_year(IGrid.igrid.get_urban_year(IGrid.igrid.get_num_urban() - 1))

            output_dir = Scenario.get_scen_value('output_dir')
            d_start, d_step, d_stop = Coeff.get_start_step_stop_diffusion()
            for diffusion_coeff in range(d_start, d_stop + 1, d_step):
                b_start, b_step, b_stop = Coeff.get_start_step_stop_breed()
                for breed_coeff in range(b_start, b_stop + 1, b_step):
                    s_start, s_step, s_stop = Coeff.get_start_step_stop_spread()
                    for spread_coeff in range(s_start, s_stop + 1, s_step):
                        sr_start, sr_step, sr_stop = Coeff.get_start_step_stop_slope_resistance()
                        for slope_resist_coeff in range(sr_start, sr_stop + 1, sr_step):
                            rg_start, rg_step, rg_stop = Coeff.get_start_step_stop_road_gravity()
                            for road_grav_coeff in range(rg_start, rg_stop + 1, rg_step):
                                filename = f"{output_dir}{UGMDefines.RESTART_FILE}{Globals.mype}"
                                Output.write_restart_data(filename, diffusion_coeff, breed_coeff, spread_coeff, slope_resist_coeff,
                                                          road_grav_coeff, Scenario.get_scen_value('random_seed'), restart_run)

                                restart_run += 1
                                Coeff.set_current_coeff(diffusion_coeff, spread_coeff, breed_coeff, slope_resist_coeff, road_grav_coeff)
                                Driver.driver()
                                Processing.increment_num_runs_exec_this_cpu()
                                # Timing Logs
                                if log_it and int(Scenario.get_scen_value('log_timings')) > 1:
                                    TimerUtility.log_timers()

                                Processing.increment_current_run()

                                if Processing.get_processing_type() == Globals.mode_enum['test']:
                                    TimerUtility.stop_timer('total_time')
                                    if log_it and int(Scenario.get_scen_value('log_timings')) > 0:
                                        TimerUtility.log_timers()
                                    Logger.close()
                                    sys.exit(0)

        # Stop timer
        TimerUtility.stop_timer('total_time')
        if log_it and int(Scenario.get_scen_value('log_timings')) > 0:
            TimerUtility.log_timers()
        # Close Logger
        Logger.close()

    except KeyError as err:
        traceback.print_exc()
        print("{0} is not set. Please set it in your scenario file".format(str(err).upper()))
        Logger.log("Something went wrong")
        Logger.close()
        sys.exit(1)
    except FileNotFoundError as err:
        traceback.print_exc()
        print(err)
        Logger.log("Something went wrong")
        Logger.close()
        sys.exit(1)
    except Exception:
        traceback.print_exc()
        Logger.log("Something went wrong")
        Logger.close()
        sys.exit(1)


def __print_usage(binary):
    print("Usage: \n")
    print(f"{binary} <mode> <scenario file>\n")
    print("Allowable modes are:\n")
    print("  calibrate\n")
    print("  restart\n")
    print("  test\n")
    print("  predict\n")


if __name__ == '__main__':
    main()
