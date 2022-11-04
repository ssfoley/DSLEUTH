from globals import Globals
from logger import Logger
import sys
import os


class Scenario:
    scenario = {}

    @staticmethod
    def init(filename, restart_flag):
        # read file
        success = Scenario.__scen_read_file(filename)
        # Scenario.__print_scen_dict(Scenario.scenario)
        # initiate log
        Scenario.__init_log(restart_flag)

    @staticmethod
    def get_scen_value(key):
        return Scenario.scenario[key]

    @staticmethod
    def __scen_read_file(filename):
        
        urban_data_file = []
        road_data_file = []
        landuse_data_file = []
        probability_color = []
        landuse_class_info = []
        deltatron_color = []
        
        scenario_info_dict = {}

        Scenario.add_defaults(scenario_info_dict)
        
        with open(filename, 'r') as file: # Use file to refer to the file object
            for line in file:
                line = line.rstrip()
                
                # skip over comments and empty lines
                if line.startswith("#") or not line:
                    continue

                # skip over empty pairs of values
                # TODO: is this wise?
                dict_pair = line.split("=")
                if len(dict_pair) < 2:
                    continue

                
                key = dict_pair[0].lower().split("(")[0]
                value = dict_pair[1].replace(" ", "")
                # get rid of comments in line
                value = value.split("#")[0]

                if key == 'urban_data':
                    urban_data_file.append(value)
                elif key == 'road_data':
                    road_data_file.append(value)
                elif key == 'landuse_data':
                    landuse_data_file.append(value)
                elif key == 'probability_color':
                    # lower, upper, color
                    value_params = value.split(",")
                    if len(value_params) != 4:
                        return False
                    hex_color = Scenario.__process_color(value_params[2])
                    value = ProbColorInfo(value_params[0], value_params[1], hex_color)
                    probability_color.append(value)
                elif key == 'landuse_class':
                    # pix, name, flag, hex/rgb
                    value_params = value.split(",")
                    if len(value_params) != 4:
                        return False
                    hex_color = Scenario.__process_color(value_params[3])
                    value = LanduseClassInfo(value_params[0], value_params[1], value_params[2], hex_color)
                    landuse_class_info.append(value)
                elif key == 'deltatron_color':
                    deltatron_color.append(Scenario.__process_color(value))
                elif key == 'input_dir' or key == 'output_dir':
                    #we don't want to lowercase the input/output path
                    scenario_info_dict[key] = value
                else:
                    value = value.lower()
                    # Need to check if it is supposed to be a bool
                    if value == 'yes':
                        scenario_info_dict[key] = True
                    elif value == 'no':
                        scenario_info_dict[key] = False
                    elif "color" in key:
                        scenario_info_dict[key] = Scenario.__process_color(value)
                    else:
                        scenario_info_dict[key] = value

            # out of the loop
            scenario_info_dict['urban_data_file'] = urban_data_file
            scenario_info_dict['road_data_file'] = road_data_file
            scenario_info_dict['landuse_data_file'] = landuse_data_file
            if len(probability_color) > 0:
                scenario_info_dict['probability_color'] = probability_color
            if len(landuse_class_info) > 0:
                scenario_info_dict['landuse_class_info'] = landuse_class_info
            if len(deltatron_color) > 0:
                scenario_info_dict['deltatron_color'] = deltatron_color

        Scenario.scenario = scenario_info_dict
        return True

    @staticmethod
    def __process_color(color):
        rgb_values = color.split(",")
        if len(rgb_values) >= 3:
            # in RGB, convert to Hex
            return '0x{:02x}{:02x}{:02x}'.format(int(rgb_values[0]), int(rgb_values[1]), int(rgb_values[2]))
        
        return color

    @staticmethod
    def __print_scen_dict(scenario_info_dict):
        for x, y in scenario_info_dict.items():
            if isinstance(y, list):
                print(f"{x}: ")
                for listItem in y:
                    print(f"       {listItem}")
                continue
            
            print(f"{x}: {y}")

    @staticmethod
    def add_defaults(dict):
        dict["date_color"] = "0xffffff"
        dict["seed_color"] = "0xf9d16e"
        dict["water_color"] = "0x1434d6"
        dict["phase0g_growth_color"] = "0xff0000"
        dict["phase1g_growth_color"] = "0x00ff00"
        dict["phase2g_growth_color"] = "0x0000ff"
        dict["phase3g_growth_color"] = "0xffff00"
        dict["phase4g_growth_color"] = "0xffffff"
        dict["phase5g_growth_color"] = "0x00ffff"

        probability_color = [Scenario.add_default_prob_color(0, 1, ""),
                             Scenario.add_default_prob_color(1, 10, "0x00ff33")]
        prob_color_list = ["0x00cc33", "0x009933", "0x006666", "0x003366", "0x000066", "0xff6a6a", "0xff7f00", "0xff3e96", "0xff0033"]
        for i in range(1, 10):
            probability_color.append(Scenario.add_default_prob_color(i * 10, (i + 1) * 10, prob_color_list[i - 1]))

        dict['probability_color'] = probability_color

        landuse_data = [
            LanduseClassInfo(0, "Unclass", "UNC", "0x000000"),
            LanduseClassInfo(1, "Urban", "URB", "0x8b2323"),
            LanduseClassInfo(2, "Agric", "", "0xffec8b"),
            LanduseClassInfo(3, "Range", "", "0xee9a49"),
            LanduseClassInfo(4, "Forest", "", "0x006400"),
            LanduseClassInfo(5, "Water", "EXC", "0x104e8b"),
            LanduseClassInfo(6, "Wetland", "", "0x483d8b"),
            LanduseClassInfo(7, "Barren", "", "0xeec591"),
        ]
        dict['landuse_class_info'] = landuse_data

        dict['deltatron_color'] = ["0x000000", "0x00ff00", "0x00d200", "0x00aa00", "0x008200", "0x005a00"]

    @staticmethod
    def add_default_prob_color(lower, upper, hex_color):
        hex_color = Scenario.__process_color(hex_color)
        return ProbColorInfo(lower, upper, hex_color)

    @staticmethod
    def add_default_landclass(pix, name, flag, hex_color):
        hex_color = Scenario.__process_color(hex_color)
        return LanduseClassInfo(pix, name, flag, hex_color)

    @staticmethod
    def __init_log(restart_flag):
        try:
            if Scenario.scenario["logging"]:
                if Logger.log_opened:
                    print("log file already open")
                    sys.exit(1)

                output = Scenario.scenario["output_dir"]
                log_filename = output + "LOG_" + str(Globals.mype)
                if not restart_flag:
                    '''file = open(log_filename, 'w')
                    file.close()'''
                    if os.path.isdir(output):
                        Logger.init(log_filename)
                        Scenario.scenario['log_filename'] = log_filename
                    else:
                        # output dir doesn't exist, ask user to create output file
                        print(f"Error: Output directory defined in Scenario File does not exist, please change \n "
                              f"output directory in Scenario file or create directory {output}")
                        sys.exit(1)
            else:
                Scenario.scenario['log_filename'] = None
        except KeyError as err:
            print("{0} is not set. Please set it in your scenario file".format(str(err).upper()))
            sys.exit()


class ProbColorInfo:
    def __init__(self, lower, upper, color):
        self.lower_bound = int(lower)
        self.upper_bound = int(upper)
        self.color = color
    
    def __str__(self):
        return f"{self.lower_bound} {self.upper_bound} {self.color}"


class LanduseClassInfo:
    def __init__(self, grayscale, name, var_type, color):
        self.name = name
        self.type = var_type
        self.color = color
        self.grayscale = grayscale
    
    def __str__(self):
        return f"{self.name} {self.type} {self.color} {self.grayscale}"