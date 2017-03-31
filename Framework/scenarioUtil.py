from tempfile import mkstemp
from shutil import move
from os import close
import subprocess
import scenario
import os

class ScenarioUtil:
    ############### replace some attributes with new value and write in new files ######################
    def replace(self, file_path, scenario, destination_path):
        #Create temp file
        fh, abs_path = mkstemp()
        with open(abs_path,'w') as new_file:
            with open(file_path) as old_file:
                for line in old_file:
                    if line.find(scenario.diffusionStartPattern) != -1:
                        line = scenario.diffusionStart + '\n'
                        new_file.write(line)
                    elif line.find(scenario.diffusionStepPattern) != -1:
                        line = scenario.diffusionStep + '\n'
                        new_file.write(line)
                    elif line.find(scenario.diffusionStopPattern) != -1:
                        line = scenario.diffusionStop + '\n'
                        new_file.write(line)

                    elif line.find(scenario.breedStartPattern) != -1:
                        line = scenario.breedStart + '\n'
                        new_file.write(line)
                    elif line.find(scenario.breedStepPattern) != -1:
                        line = scenario.breedStep + '\n'
                        new_file.write(line)
                    elif line.find(scenario.breedStopPattern) != -1:
                        line = scenario.breedStop + '\n'
                        new_file.write(line)

                    elif line.find(scenario.spreadStartPattern) != -1:
                        line = scenario.spreadStart + '\n'
                        new_file.write(line)
                    elif line.find(scenario.spreadStepPattern) != -1:
                        line = scenario.spreadStep + '\n'
                        new_file.write(line)
                    elif line.find(scenario.spreadStopPattern) != -1:
                        line = scenario.spreadStop + '\n'
                        new_file.write(line)

                    elif line.find(scenario.slopeStartPattern) != -1:
                        line = scenario.slopeStart + '\n'
                        new_file.write(line)
                    elif line.find(scenario.slopeStepPattern) != -1:
                        line = scenario.slopeStep + '\n'
                        new_file.write(line)
                    elif line.find(scenario.slopeStopPattern) != -1:
                        line = scenario.slopeStop + '\n'
                        new_file.write(line)

                    elif line.find(scenario.roadStartPattern) != -1:
                        line = scenario.roadStart + '\n'
                        new_file.write(line)
                    elif line.find(scenario.roadStepPattern) != -1:
                        line = scenario.roadStep + '\n'
                        new_file.write(line)
                    elif line.find(scenario.roadStopPattern) != -1:
                        line = scenario.roadStop + '\n'
                        new_file.write(line)

                    elif line.find(scenario.outputDirPattern) != -1:
                        line = scenario.outputDir + '\n'
                        new_file.write(line)

                    else:
                        new_file.write(line)
        close(fh)
        move(abs_path, destination_path)
        subprocess.call(['chmod','777', destination_path])

    ########################## get the given Attribute's int value #################################
    def find(self, pattern, file_path):
        fo = open(file_path)
        for line in fo:
            if line.find(pattern)!= -1:
                str = line.split('=')
                return int(str[1])

    ########################## get the given Attribute's String value #################################
    def getAttribute(self, pattern, file_path):
        fo = open(file_path)
        for line in fo:
            if line.find(pattern)!= -1:
                str = line.split('=')
                return str[1]

    ########################## get the given OUTPUT_DIR #################################
    def getOutputDir(self, file_path):
        return getAttribute("OUTPUT_DIR=", file_path)


    def makeOutputDir(self, file_path):
        os.makedirs(file_path)
    
    ############################ generate scenario files according to the specified numbers ######### 
    def generatingBySplitDiffusionAndNum(self, file_path, destination_path, num):
        #the name of generated config
        FILE_NUM = 1

        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP

        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 1
        
        numOfDiffusionValues = (CALIBRATION_DIFFUSION_STOP - CALIBRATION_DIFFUSION_START) / CALIBRATION_DIFFUSION_STEP + 1
        numOfBreedValues = (CALIBRATION_BREED_STOP - CALIBRATION_BREED_START)/CALIBRATION_BREED_STEP + 1
        numOfSpreadValues = (CALIBRATION_SPREAD_STOP - CALIBRATION_SPREAD_START)/CALIBRATION_SPREAD_STEP + 1
        numOfSlopeValues = (CALIBRATION_SLOPE_STOP - CALIBRATION_SLOPE_START)/CALIBRATION_SLOPE_STEP + 1
        numOfRoadValues = (CALIBRATION_ROAD_STOP - CALIBRATION_ROAD_START)/CALIBRATION_ROAD_STEP + 1
        
        if numOfDiffusionValues > num:
            averageNumOfJobsPerFile = numOfJobs / num
            gapsPerJob = CALIBRATION_DIFFUSION_STEP
            remainder = numOfJobs % num

            while DIFFUSION_START <= CALIBRATION_DIFFUSION_STOP:
                
                scen = scenario.Scenario()
                scen.diffusionStart += str(DIFFUSION_START)
                scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP) 
                stop = DIFFUSION_START
                if remainder > 0 :
                    stop = stop + averageNumOfJobsPerFile * gapsPerJob
                    remainder = remainder - 1
                else:
                    stop = stop + (averageNumOfJobsPerFile - 1) * gapsPerJob
                DIFFUSION_START = stop + gapsPerJob
                scen.diffusionStop += str(stop)

                scen.breedStart += str(BREED_START)
                scen.breedStep += str(CALIBRATION_BREED_STEP)
                scen.breedStop += str(CALIBRATION_BREED_STOP)

                scen.spreadStart += str(SPREAD_START)
                scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
                scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

                scen.slopeStart += str(SLOPE_START)
                scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
                scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

                scen.roadStart += str(ROAD_START)
                scen.roadStep += str(CALIBRATION_ROAD_STEP)
                scen.roadStop += str(CALIBRATION_ROAD_STOP)

                scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

                print scen.outputDir
                self.replace(file_path, scen, destination_path + str(FILE_NUM))

                FILE_NUM = FILE_NUM + 1
                print FILE_NUM
                #add the last step
                i = i + 1

        elif numOfDiffusionValues * numOfBreedValues > num:
             while DIFFUSION_START <= CALIBRATION_DIFFUSION_STOP:
                 while BREED_START <= CALIBRATION_BREED_STOP:
                    scen = scenario.Scenario()
                    scen.diffusionStart += str(DIFFUSION_START)
                    scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
                    scen.diffusionStop += str(DIFFUSION_START)

                    scen.breedStart += str(BREED_START)
                    scen.breedStep += str(CALIBRATION_BREED_STEP)
                    scen.breedStop += str(BREED_START)

                    scen.spreadStart += str(SPREAD_START)
                    scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
                    scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

                    scen.slopeStart += str(SLOPE_START)
                    scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
                    scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

                    scen.roadStart += str(ROAD_START)
                    scen.roadStep += str(CALIBRATION_ROAD_STEP)
                    scen.roadStop += str(CALIBRATION_ROAD_STOP)

                    scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

                    print scen.outputDir
                    self.replace(file_path, scen, destination_path + str(FILE_NUM))

                    FILE_NUM = FILE_NUM + 1
                    print FILE_NUM
                    i = i + 1

                    BREED_START = BREED_START + CALIBRATION_BREED_STEP

                DIFFUSION_START = DIFFUSION_START + CALIBRATION_DIFFUSION_STEP
                BREED_START = CALIBRATION_BREED_START
        else:
             while DIFFUSION_START <= CALIBRATION_DIFFUSION_STOP:
                 while BREED_START <= CALIBRATION_BREED_STOP:
                     while SPREAD_START <= CALIBRATION_SPREAD_STOP:
                        scen = scenario.Scenario()
                        scen.diffusionStart += str(DIFFUSION_START)
                        scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
                        scen.diffusionStop += str(DIFFUSION_START)

                        scen.breedStart += str(BREED_START)
                        scen.breedStep += str(CALIBRATION_BREED_STEP)
                        scen.breedStop += str(BREED_START)

                        scen.spreadStart += str(SPREAD_START)
                        scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
                        scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

                        scen.slopeStart += str(SLOPE_START)
                        scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
                        scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

                        scen.roadStart += str(ROAD_START)
                        scen.roadStep += str(CALIBRATION_ROAD_STEP)
                        scen.roadStop += str(CALIBRATION_ROAD_STOP)

                        scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

                        print scen.outputDir
                        self.replace(file_path, scen, destination_path + str(FILE_NUM))

                        FILE_NUM = FILE_NUM + 1
                        print FILE_NUM
                        i = i + 1

                        SPREAD_START = SPREAD_START + CALIBRATION_SPREAD_STEP

                    BREED_START = BREED_START + CALIBRATION_BREED_STEP
                    SPREAD_START = CALIBRATION_SPREAD_START
                DIFFUSION_START = DIFFUSION_START + CALIBRATION_DIFFUSION_STEP
                BREED_START = CALIBRATION_BREED_START

        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM

########################## generating the scenario files by split diffusion gap ########################
    def generatingBySplitDiffusion(self, file_path, destination_path):
        #the name of generated config
        FILE_NUM = 1

        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP

        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 0
        while DIFFUSION_START+ i * DIFFUSION_DISTANCE <= CALIBRATION_DIFFUSION_STOP:
        	
            scen = scenario.Scenario()
            scen.diffusionStart += str(DIFFUSION_START+ i * DIFFUSION_DISTANCE)
            scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP) 
            scen.diffusionStop += str(DIFFUSION_START+ i * DIFFUSION_DISTANCE)

            scen.breedStart += str(BREED_START)
            scen.breedStep += str(CALIBRATION_BREED_STEP)
            scen.breedStop += str(CALIBRATION_BREED_STOP)

            scen.spreadStart += str(SPREAD_START)
            scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
            scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

            scen.slopeStart += str(SLOPE_START)
            scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
            scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

            scen.roadStart += str(ROAD_START)
            scen.roadStep += str(CALIBRATION_ROAD_STEP)
            scen.roadStop += str(CALIBRATION_ROAD_STOP)

            scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

            print scen.outputDir
            self.replace(file_path, scen, destination_path + str(FILE_NUM))

            FILE_NUM = FILE_NUM + 1
            print FILE_NUM
            #add the last step
            i = i + 1

        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM

########################## generating scenario file by spliting the Breed gap ##########################
    def generatingBySplitBreed(self, file_path, destination_path):
        FILE_NUM = 1
        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP

        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 0
        while BREED_START+ i * BREED_DISTANCE <= CALIBRATION_BREED_STOP:
        	
            scen = scenario.Scenario()
            scen.diffusionStart += str(DIFFUSION_START)
            scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
            scen.diffusionStop += str(CALIBRATION_DIFFUSION_STOP)

            scen.breedStart += str(BREED_START + i * BREED_DISTANCE)
            scen.breedStep += str(CALIBRATION_BREED_STEP)
            scen.breedStop += str(BREED_START + i * BREED_DISTANCE)

            scen.spreadStart += str(SPREAD_START)
            scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
            scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

            scen.slopeStart += str(SLOPE_START)
            scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
            scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

            scen.roadStart += str(ROAD_START)
            scen.roadStep += str(CALIBRATION_ROAD_STEP)
            scen.roadStop += str(CALIBRATION_ROAD_STOP)

            scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

            print scen.outputDir
            self.replace(file_path, scen, destination_path + str(FILE_NUM))

            FILE_NUM = FILE_NUM + 1
            print FILE_NUM
            #add the last step
            i = i + 1

        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM

########################## generating scenario file by spliting the Spread gap ##########################
    def generatingBySplitSpread(self, file_path, destination_path):
        FILE_NUM = 1

        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP

        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 0
        while SPREAD_START+ i * SPREAD_DISTANCE <= CALIBRATION_SPREAD_STOP:
        	
            scen = scenario.Scenario()
            scen.diffusionStart += str(DIFFUSION_START)
            scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
            scen.diffusionStop += str(CALIBRATION_DIFFUSION_STOP)

            scen.breedStart += str(BREED_START)
            scen.breedStep += str(CALIBRATION_BREED_STEP)
            scen.breedStop += str(CALIBRATION_BREED_STOP)

            scen.spreadStart += str(SPREAD_START + i * SPREAD_DISTANCE)
            scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
            scen.spreadStop += str(SPREAD_START + i * SPREAD_DISTANCE)

            scen.slopeStart += str(SLOPE_START)
            scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
            scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

            scen.roadStart += str(ROAD_START)
            scen.roadStep += str(CALIBRATION_ROAD_STEP)
            scen.roadStop += str(CALIBRATION_ROAD_STOP)

            scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

            print scen.outputDir
            self.replace(file_path, scen, destination_path + str(FILE_NUM))

            FILE_NUM = FILE_NUM + 1
            print FILE_NUM
            #add the last step
            i = i + 1

        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM

########################## generating scenario file by spliting the slope gap ##########################
    def generatingBySplitSlope(self, file_path, destination_path):
        FILE_NUM = 1

        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP

        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 0
        while SLOPE_START+ i * SLOPE_DISTANCE <= CALIBRATION_SLOPE_STOP:
        	
            scen = scenario.Scenario()
            scen.diffusionStart += str(DIFFUSION_START)
            scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
            scen.diffusionStop += str(CALIBRATION_DIFFUSION_STOP)

            scen.breedStart += str(BREED_START)
            scen.breedStep += str(CALIBRATION_BREED_STEP)
            scen.breedStop += str(CALIBRATION_BREED_STOP)

            scen.spreadStart += str(SPREAD_START)
            scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
            scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

            scen.slopeStart += str(SLOPE_START + i * SLOPE_DISTANCE)
            scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
            scen.slopeStop += str(SLOPE_START + i * SLOPE_DISTANCE)

            scen.roadStart += str(ROAD_START)
            scen.roadStep += str(CALIBRATION_ROAD_STEP)
            scen.roadStop += str(CALIBRATION_ROAD_STOP)

            scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

            print scen.outputDir
            self.replace(file_path, scen, destination_path + str(FILE_NUM))

            FILE_NUM = FILE_NUM + 1
            print FILE_NUM
            #add the last step
            i = i + 1
        
        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM

########################## generating scenario file by spliting the road gap ##########################
    def generatingBySplitRoad(self, file_path, destination_path):
        FILE_NUM = 1

        CALIBRATION_DIFFUSION_START = self.find('CALIBRATION_DIFFUSION_START=', file_path)
        CALIBRATION_DIFFUSION_STEP = self.find('CALIBRATION_DIFFUSION_STEP=', file_path)
        CALIBRATION_DIFFUSION_STOP = self.find('CALIBRATION_DIFFUSION_STOP=', file_path)

        DIFFUSION_START = CALIBRATION_DIFFUSION_START
        DIFFUSION_DISTANCE = CALIBRATION_DIFFUSION_STEP

        CALIBRATION_BREED_START = self.find('CALIBRATION_BREED_START=', file_path)
        CALIBRATION_BREED_STEP = self.find('CALIBRATION_BREED_STEP=', file_path)
        CALIBRATION_BREED_STOP = self.find('CALIBRATION_BREED_STOP=', file_path)

        BREED_START = CALIBRATION_BREED_START
        BREED_DISTANCE = CALIBRATION_BREED_STEP

        CALIBRATION_SPREAD_START = self.find('CALIBRATION_SPREAD_START=', file_path)
        CALIBRATION_SPREAD_STEP = self.find('CALIBRATION_SPREAD_STEP=', file_path)
        CALIBRATION_SPREAD_STOP = self.find('CALIBRATION_SPREAD_STOP=', file_path)

        SPREAD_START = CALIBRATION_SPREAD_START
        SPREAD_DISTANCE = CALIBRATION_SPREAD_STEP

        CALIBRATION_SLOPE_START = self.find('CALIBRATION_SLOPE_START=', file_path)
        CALIBRATION_SLOPE_STEP = self.find('CALIBRATION_SLOPE_STEP=', file_path)
        CALIBRATION_SLOPE_STOP = self.find('CALIBRATION_SLOPE_STOP=', file_path)

        SLOPE_START = CALIBRATION_SLOPE_START
        SLOPE_DISTANCE = CALIBRATION_SLOPE_STEP

        CALIBRATION_ROAD_START = self.find('CALIBRATION_ROAD_START=', file_path)
        CALIBRATION_ROAD_STEP = self.find('CALIBRATION_ROAD_STEP=', file_path)
        CALIBRATION_ROAD_STOP = self.find('CALIBRATION_ROAD_STOP=', file_path)

        ROAD_START = CALIBRATION_ROAD_START
        ROAD_DISTANCE = CALIBRATION_ROAD_STEP
        
        OUTPUT_DIR = self.getAttribute("OUTPUT_DIR=", file_path)

        i = 0
        while ROAD_START+ i * ROAD_DISTANCE <= CALIBRATION_ROAD_STOP:
        	
            scen = scenario.Scenario()
            scen.diffusionStart += str(DIFFUSION_START)
            scen.diffusionStep += str(CALIBRATION_DIFFUSION_STEP)
            scen.diffusionStop += str(CALIBRATION_DIFFUSION_STOP)

            scen.breedStart += str(BREED_START)
            scen.breedStep += str(CALIBRATION_BREED_STEP)
            scen.breedStop += str(CALIBRATION_BREED_STOP)

            scen.spreadStart += str(SPREAD_START)
            scen.spreadStep += str(CALIBRATION_SPREAD_STEP)
            scen.spreadStop += str(CALIBRATION_SPREAD_STOP)

            scen.slopeStart += str(SLOPE_START)
            scen.slopeStep += str(CALIBRATION_SLOPE_STEP)
            scen.slopeStop += str(CALIBRATION_SLOPE_STOP)

            scen.roadStart += str(ROAD_START + i * ROAD_DISTANCE)
            scen.roadStep += str(CALIBRATION_ROAD_STEP)
            scen.roadStop += str(ROAD_START + i * ROAD_DISTANCE)

            scen.outputDir += OUTPUT_DIR.replace("\n", "") + str(FILE_NUM) + "/"

            print scen.outputDir
            self.replace(file_path, scen, destination_path + str(FILE_NUM))

            FILE_NUM = FILE_NUM + 1
            print FILE_NUM
            #add the last step
            i = i + 1

        for i in range(1, FILE_NUM):
            if not os.path.exists(OUTPUT_DIR.replace("\n", "") + str(i) + "/"):
                self.makeOutputDir(OUTPUT_DIR.replace("\n", "") + str(i) + "/")
        return FILE_NUM