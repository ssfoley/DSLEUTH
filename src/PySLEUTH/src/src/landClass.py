import sys
import os
from copy import copy
from scenario import Scenario
from globals import Globals
from logger import Logger


class LandClass:
    class_indices = []  # type Landuse_Meta
    new_indices = []  # type int
    landuse_classes = []  # type Landuse_Meta
    urban_code = -1
    filename = ""
    max_landclass_num = -1
    num_reduced_classes = -1
    annual_prob_filename = ""
    annual_prob = []
    ugm_read = False

    @staticmethod
    def init():
        LandClass.set_max_land_class_num()
        LandClass.map_land_class_num()
        LandClass.create_reduced_classes()
        LandClass.set_urban_code()
        LandClass.ugm_read = True

    @staticmethod
    def get_urban_code():
        return LandClass.urban_code

    @staticmethod
    def set_urban_code():
        for landuse_class in LandClass.landuse_classes:
            if landuse_class.id == "URB":
                LandClass.urban_code = landuse_class.num

        if LandClass.urban_code <= 0:
            print("Error with land class, no urban code")
            sys.exit(1)

    @staticmethod
    def get_num_landclasses():
        return len(LandClass.landuse_classes)

    @staticmethod
    def get_landclasses():
        return LandClass.landuse_classes

    @staticmethod
    def get_new_indices():
        return LandClass.new_indices

    @staticmethod
    def get_reduced_classes():
        return LandClass.class_indices

    @staticmethod
    def set_max_land_class_num():
        max_class_meta = max(LandClass.landuse_classes, key=lambda l: l.num)
        max_num = max_class_meta.num
        if max_num >= Constants.MAX_NEW_INDICES:
            print(f"The maximum class number = {max_num} in file: {LandClass.filename}")
            print("exceeds MAX_NEW_INDICES. Increase the value of ")
            print("MAX_NEW_INDICES and recompile or reduce the landuse class number")
            sys.exit(1)

        LandClass.max_landclass_num = max_num

    @staticmethod
    def map_land_class_num():
        LandClass.new_indices = list(range(Constants.MAX_NEW_INDICES))

        for i, landuse_class in enumerate(LandClass.landuse_classes):
            new_index = landuse_class.num
            LandClass.new_indices[new_index] = i

    @staticmethod
    def create_reduced_classes():
        for landuse_class in LandClass.landuse_classes:
            if landuse_class.id != "EXC" and landuse_class.id != "URB" and landuse_class.id != "UNC":
                LandClass.class_indices.append(copy(landuse_class))

    @staticmethod
    def init_annual_prob(total_pixels):
        LandClass.annual_prob_filename = f"{Scenario.get_scen_value('output_dir')}annual_class_probabilities_{Globals.mype}"
        annual_prob_file = open(LandClass.annual_prob_filename, "w")
        num_pixels = total_pixels * LandClass.get_num_landclasses()

        for i in range(LandClass.get_num_landclasses()):
            temp = [0] * total_pixels
            LandClass.annual_prob.append(copy(temp))

        for i in range(num_pixels):
            annual_prob_file.write(str(0))

        if Scenario.get_scen_value("logging") and Scenario.get_scen_value("log_writes"):
            Logger.log(f"{num_pixels} zeroes written to {LandClass.annual_prob_filename}")

        annual_prob_file.close()

    @staticmethod
    def update_annual_prob(land1, total_pixels):
        if len(Scenario.get_scen_value("landuse_data_file")) < 1:
            return

        if Scenario.get_scen_value("logging") and Scenario.get_scen_value("log_writes"):
            Logger.log(f"Updating file {LandClass.annual_prob_filename}")

        for i in range(LandClass.get_num_landclasses()):
            cur_annual_prob = LandClass.annual_prob[i]

            for j in range(total_pixels):
                if i == LandClass.new_indices[land1[j]]:
                    cur_annual_prob[j] += 1

    @staticmethod
    def update_annual_prob_file():
        annual_prob_file = open(LandClass.annual_prob_filename, "w")
        for landclass in LandClass.annual_prob:
            for pix in landclass:
                annual_prob_file.write(str(pix))

        annual_prob_file.close()

    @staticmethod
    def build_prob_image(total_pixels):
        num_landclasses = len(LandClass.landuse_classes)
        cum_prob = [0] * total_pixels
        cum_uncert = [0] * total_pixels

        # K=0 Landclass data
        max_grid = copy(LandClass.annual_prob[0])

        # Initialize sum_grid
        sum_grid = copy(max_grid)
        for i in range(1, num_landclasses):
            # Look for the max of the max and the sum
            input_grid = copy(LandClass.annual_prob[i])
            for j in range(total_pixels):
                if input_grid[j] > max_grid[j]:
                    max_grid[j] = input_grid[j]
                    cum_prob[j] = i
                sum_grid[j] += input_grid[j]

        os.remove(LandClass.annual_prob_filename)

        for i in range(total_pixels):
            if sum_grid[i] != 0:
                cum_uncert[i] = 100 - (100 * max_grid[i]) / sum_grid[i]
            else:
                msg = f"Divide by zero: sum_grid[{i}] = {sum_grid[i]}"
                Logger.log(msg)
                print(msg)
                sys.exit(1)

        return cum_prob, cum_uncert


class Constants:
    MAX_NUM_CLASSES = 20
    MAX_NEW_INDICES = 256
    MAX_LINE_LEN = 256


class LanduseMeta:
    def __init__(self, num, class_id, name, idx, hex_color):
        self.num = int(num)
        self.id = class_id
        self.name = name
        self.idx = idx
        self.hex_color = hex_color
        (self.red, self.green, self.blue) = tuple(int(hex_color[i:i + 2], 16) for i in (0, 2, 4))
        self.EXC = class_id == "EXC"
        self.trans = not self.EXC

    def __str__(self):
        return f"{self.num} {self.id} {self.name} {self.idx} [{self.red}, {self.green}, {self.blue} -> {self.hex_color}] {self.EXC} {self.trans}"

    def __copy__(self):
        return type(self)(self.num, self.id, self.name, self.idx, self.hex_color)
