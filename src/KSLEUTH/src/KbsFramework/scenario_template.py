import os
from os import path
from configparser import ConfigParser


class Scenario:
    # outputDir: the FINAL output dir
    outputDir = "/SLEUTH/KbsFramework/splitOutput/"
    origin_scenario_file = None

    diffStart = -1
    diffStep = -1
    diffStop = -1
    diffNum = -1

    breedStart = -1
    breedStep = -1
    breedStop = -1
    breedNum = -1

    spreadStart = -1
    spreadStep = -1
    spreadStop = -1
    spreadNum = -1

    slopeStart = -1
    slopeStep = -1
    slopeStop = -1
    slopeNum = -1

    roadStart = -1
    roadStep = -1
    roadStop = -1
    roadNum = -1

    diffusionStartPattern = "CALIBRATION_DIFFUSION_START="
    diffusionStepPattern = "CALIBRATION_DIFFUSION_STEP="
    diffusionStopPattern = "CALIBRATION_DIFFUSION_STOP="

    breedStartPattern = "CALIBRATION_BREED_START="
    breedStepPattern = "CALIBRATION_BREED_STEP="
    breedStopPattern = "CALIBRATION_BREED_STOP="

    spreadStartPattern = "CALIBRATION_SPREAD_START="
    spreadStepPattern = "CALIBRATION_SPREAD_STEP="
    spreadStopPattern = "CALIBRATION_SPREAD_STOP="

    slopeStartPattern = "CALIBRATION_SLOPE_START="
    slopeStepPattern = "CALIBRATION_SLOPE_STEP="
    slopeStopPattern = "CALIBRATION_SLOPE_STOP="

    roadStartPattern = "CALIBRATION_ROAD_START="
    roadStepPattern = "CALIBRATION_ROAD_STEP="
    roadStopPattern = "CALIBRATION_ROAD_STOP="

    outputDirPattern = "OUTPUT_DIR="

    def __init__(self, sf: str):
        """
        init all parameter
        :param sf:
        """
        if isinstance(sf, str):
            try:
                cp = ConfigParser()
                cp.read(path.join(os.getenv('HOME'), ".ksleuth_config"))
                self.origin_scenario_file = path.join(cp['path']['originalScenario'], sf)
                self.split_scenario_file = cp['path']['splitScenario']
            except Exception as e:
                print("in scenario_template, __init__: " + str(e))

            lines = open(self.origin_scenario_file, "r").readlines()
            for line in lines:
                line = line.strip()
                if line is not "" and line[0] != '#':
                    if self.diffusionStartPattern in line:
                        # found diffusion start
                        self.diffStart = int(line.split("=")[1].strip())
                    elif self.diffusionStepPattern in line:
                        # found diffusion step
                        self.diffStep = int(line.split("=")[1].strip())
                    elif self.diffusionStopPattern in line:
                        # found diffusion stop
                        self.diffStop = int(line.split("=")[1].strip())
                    elif self.spreadStartPattern in line:
                        # found spread start
                        self.spreadStart = int(line.split("=")[1].strip())
                    elif self.spreadStepPattern in line:
                        # found spread step
                        self.spreadStep = int(line.split("=")[1].strip())
                    elif self.spreadStopPattern in line:
                        # found spread stop
                        self.spreadStop = int(line.split("=")[1].strip())
                    elif self.slopeStartPattern in line:
                        # found slope start
                        self.slopeStart = int(line.split("=")[1].strip())
                    elif self.slopeStepPattern in line:
                        # found slope step
                        self.slopeStep = int(line.split("=")[1].strip())
                    elif self.slopeStopPattern in line:
                        # found slope stop
                        self.slopeStop = int(line.split("=")[1].strip())
                    elif self.breedStartPattern in line:
                        # found breed start
                        self.breedStart = int(line.split("=")[1].strip())
                    elif self.breedStepPattern in line:
                        # found breed step
                        self.breedStep = int(line.split("=")[1].strip())
                    elif self.breedStopPattern in line:
                        # found breed stop
                        self.breedStop = int(line.split("=")[1].strip())
                    elif self.roadStartPattern in line:
                        # found road start
                        self.roadStart = int(line.split("=")[1].strip())
                    elif self.roadStepPattern in line:
                        # found road step
                        self.roadStep = int(line.split("=")[1].strip())
                    elif self.roadStopPattern in line:
                        # found road stop
                        self.roadStop = int(line.split("=")[1].strip())

            self.diffNum = int((self.diffStop - self.diffStart) / self.diffStep)+1
            self.spreadNum = int((self.spreadStop - self.spreadStart) / self.spreadStep)+1
            self.slopeNum = int((self.slopeStop - self.slopeStart) / self.slopeStep)+1
            self.breedNum = int((self.breedStop - self.breedStart) / self.breedStep)+1
            self.roadNum = int((self.roadStop - self.roadStart) / self.roadStep)+1

        else:
            return

    def save_split_scenario(self, name: str) -> str:

        """
        store the split_scenario_file into host machine
        :param name: str the name of split_scenario_file
        :return: str return the name of split_scenario_file if file store successful
        """

        scenario_file = open(self.origin_scenario_file, "r")
        split_scenario_file = open(self.split_scenario_file+name, "w")
        lines = scenario_file.readlines()

        for line in lines:
            line = line.strip()
            if line is not "" and line[0] != '#':
                if self.diffusionStartPattern in line:
                    # found diffusion start
                    split_scenario_file.write(self.diffusionStartPattern + str(self.diffStart) + "\n")
                elif self.diffusionStepPattern in line:
                    # found diffusion step
                    split_scenario_file.write(self.diffusionStepPattern + str(self.diffStep) + "\n")
                elif self.diffusionStopPattern in line:
                    # found diffusion stop
                    split_scenario_file.write(self.diffusionStopPattern + str(self.diffStop) + "\n")
                elif self.breedStartPattern in line:
                    # found breed start
                    split_scenario_file.write(self.breedStartPattern + str(self.breedStart) + "\n")
                elif self.breedStepPattern in line:
                    # found breed step
                    split_scenario_file.write(self.breedStepPattern + str(self.breedStep) + "\n")
                elif self.breedStopPattern in line:
                    # found breed stop
                    split_scenario_file.write(self.breedStopPattern + str(self.breedStop) + "\n")
                elif self.spreadStartPattern in line:
                    # found spread start
                    split_scenario_file.write(self.spreadStartPattern + str(self.spreadStart) + "\n")
                elif self.spreadStepPattern in line:
                    # found spread step
                    split_scenario_file.write(self.spreadStepPattern + str(self.spreadStep) + "\n")
                elif self.spreadStopPattern in line:
                    # found spread stop
                    split_scenario_file.write(self.spreadStopPattern + str(self.spreadStop) + "\n")
                elif self.slopeStartPattern in line:
                    # found slope start
                    split_scenario_file.write(self.slopeStartPattern + str(self.slopeStart) + "\n")
                elif self.slopeStepPattern in line:
                    # found slope step
                    split_scenario_file.write(self.slopeStepPattern + str(self.slopeStep) + "\n")
                elif self.slopeStopPattern in line:
                    # found slope stop
                    split_scenario_file.write(self.slopeStopPattern + str(self.slopeStop) + "\n")
                elif self.roadStartPattern in line:
                    # found road start
                    split_scenario_file.write(self.roadStartPattern + str(self.roadStart) + "\n")
                elif self.roadStepPattern in line:
                    # found road step
                    split_scenario_file.write(self.roadStepPattern + str(self.roadStep) + "\n")
                elif self.roadStopPattern in line:
                    # found road stop
                    split_scenario_file.write(self.roadStopPattern + str(self.roadStop) + "\n")
                elif self.outputDirPattern in line:
                    # found output dir
                    split_scenario_file.write(self.outputDirPattern + self.outputDir + name + "/ \n")
                else:
                    split_scenario_file.write(line + "\n")
            else:
                split_scenario_file.write(line + "\n")
        return name
