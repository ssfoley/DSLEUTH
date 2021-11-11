from coeff import Coeff


class Processing:
    type_of_processing = -1
    total_runs = -1
    total_runs_exec_this_cpu = -1
    last_run = -1
    last_mc = -1
    current_run = 0
    current_monte_carlo = -1
    current_year = 0
    stop_year = -1
    restart_flag = False
    last_run_flag = False
    last_mc_flag = False

    @staticmethod
    def get_total_runs():
        return Processing.total_runs

    @staticmethod
    def set_total_runs():
        diffusion = ((Coeff.stop_coeff.diffusion - Coeff.start_coeff.diffusion) / Coeff.step_coeff.diffusion) + 1
        breed = ((Coeff.stop_coeff.breed - Coeff.start_coeff.breed) / Coeff.step_coeff.breed) + 1
        spread = ((Coeff.stop_coeff.spread - Coeff.start_coeff.spread) / Coeff.step_coeff.spread) + 1
        slope_resist = ((Coeff.stop_coeff.slope_resistance - Coeff.start_coeff.slope_resistance) / Coeff.step_coeff.slope_resistance) + 1
        road_gravity = ((Coeff.stop_coeff.road_gravity - Coeff.start_coeff.road_gravity) / Coeff.step_coeff.road_gravity) + 1

        Processing.total_runs = int(diffusion * breed * spread * slope_resist * road_gravity)
        Processing.last_run_flag = False
        Processing.last_mc_flag = False
        Processing.last_run = Processing.total_runs - 1

    @staticmethod
    def get_current_run():
        return Processing.current_run

    @staticmethod
    def set_current_run(run):
        Processing.current_run = int(run)

    @staticmethod
    def increment_current_run():
        Processing.current_run += 1

    @staticmethod
    def get_current_monte():
        return Processing.current_monte_carlo

    @staticmethod
    def set_current_monte(monte):
        Processing.current_monte_carlo = monte

    @staticmethod
    def set_last_monte(val):
        Processing.last_mc = val

    @staticmethod
    def get_last_monte():
        return Processing.last_mc

    @staticmethod
    def get_num_runs_exec_this_cpu():
        return Processing.total_runs_exec_this_cpu

    @staticmethod
    def set_num_runs_exec_this_cpu(val):
        Processing.total_runs_exec_this_cpu = val

    @staticmethod
    def increment_num_runs_exec_this_cpu():
        Processing.total_runs_exec_this_cpu += 1

    @staticmethod
    def get_current_year():
        return Processing.current_year

    @staticmethod
    def set_current_year(year):
        Processing.current_year = int(year)

    @staticmethod
    def increment_current_year():
        Processing.current_year += 1

    @staticmethod
    def get_stop_year():
        return Processing.stop_year

    @staticmethod
    def set_stop_year(year):
        Processing.stop_year = int(year)

    @staticmethod
    def get_processing_type():
        return Processing.type_of_processing

    @staticmethod
    def set_processing_type(type_of_processing):
        Processing.type_of_processing = type_of_processing

    @staticmethod
    def get_restart_flag():
        return Processing.restart_flag

    @staticmethod
    def set_restart_flag(flag):
        Processing.restart_flag = flag
