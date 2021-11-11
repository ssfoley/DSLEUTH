from scenario import Scenario
from logger import Logger


class Output:
    @staticmethod
    def write_restart_data(filename, diffusion_coeff, breed_coeff, spread_coeff,
                           slope_resist_coeff, road_grav_coeff, count, counter):
        if Scenario.get_scen_value('logging') and Scenario.get_scen_value('log_writes'):
            Logger.log(f"Writing restart data to file: {filename}")

        restart_file = open(filename, "w")
        restart_file.write(f"{diffusion_coeff} {breed_coeff} {spread_coeff} {slope_resist_coeff} {road_grav_coeff} {count} {counter}")
        restart_file.close()

    @staticmethod
    def write_grid_to_file(filename, grid):
        file = open(filename, "w")
        for data in grid.gridData:
            file.write(f"{data} ")
        file.close()

    @staticmethod
    def write_list_to_file(filename, metalist):
        file = open(filename, "w")
        for data in metalist:
            file.write(f"{data}")
        file.close()
