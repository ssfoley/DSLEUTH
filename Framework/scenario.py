import os

class Scenario:
    """
    Data container for storing the values for a specific scenario file
    """

    outputDir = None

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

    def print_me(self):
        print "diffusion: {} {} {}".format(self.diffStart, self.diffStep, self.diffStop)
        print "breed:  {} {} {}".format(self.breedStart, self.breedStep, self.breedStop)
        print "spread:  {} {} {}".format(self.spreadStart, self.spreadStep, self.spreadStop)
        print "slope:  {} {} {}".format(self.slopeStart, self.slopeStep, self.slopeStop)
        print "road:  {} {} {}".format(self.roadStart, self.roadStep, self.roadStop)
        print "output dir: {}".format(self.outputDir)



    def copy(self, to_copy):
        self.outputDir = to_copy.outputDir
        
        self.diffStart = to_copy.diffStart 
        self.diffStep = to_copy.diffStep 
        self.diffStop = to_copy.diffStop 
        self.diffNum = to_copy.diffNum
        
        self.breedStart = to_copy.breedStart
        self.breedStep = to_copy.breedStep
        self.breedStop = to_copy.breedStop
        self.breedNum = to_copy.breedNum
        
        self.spreadStart = to_copy.spreadStart
        self.spreadStep = to_copy.spreadStep
        self.spreadStop = to_copy.spreadStop
        self.spreadNum = to_copy.spreadNum
        
        self.slopeStart = to_copy.slopeStart
        self.slopeStep = to_copy.slopeStep
        self.slopeStop = to_copy.slopeStop
        self.slopeNum = to_copy.slopeNum
        
        self.roadStart = to_copy.roadStart
        self.roadStep = to_copy.roadStep
        self.roadStop = to_copy.roadStop
        self.roadNum = to_copy.roadNum
        




    def read_file(self, src_file):
        sf = open(src_file, "r")
        lines = sf.readlines()

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
                elif self.outputDirPattern in line:
                    # found output dir
                    self.outputDir = line.split("=")[1].strip()



    def write_file(self, orig_file, dest_file, fnum):
        sf = open(orig_file, "r")
        lines = sf.readlines()
        df = open(dest_file, "w")

        for line in lines:
            line = line.strip()
            if line is not "" and line[0] != '#':
                if self.diffusionStartPattern in line:
                    # found diffusion start
                    df.write(self.diffusionStartPattern + str(self.diffStart) + "\n")
                elif self.diffusionStepPattern in line:
                    # found diffusion step
                    df.write(self.diffusionStepPattern + str(self.diffStep) + "\n")
                elif self.diffusionStopPattern in line:
                    # found diffusion stop 
                    df.write(self.diffusionStopPattern + str(self.diffStop) + "\n")
                elif self.breedStartPattern in line:
                    # found breed start 
                    df.write(self.breedStartPattern + str(self.breedStart) + "\n")
                elif self.breedStepPattern in line:
                    # found breed step  
                    df.write(self.breedStepPattern + str(self.breedStep) + "\n")
                elif self.breedStopPattern in line:
                    # found breed stop 
                    df.write(self.breedStopPattern + str(self.breedStop) + "\n")
                elif self.spreadStartPattern in line:
                    # found spread start  
                    df.write(self.spreadStartPattern + str(self.spreadStart) + "\n")
                elif self.spreadStepPattern in line:
                    # found spread step 
                    df.write(self.spreadStepPattern + str(self.spreadStep) + "\n")
                elif self.spreadStopPattern in line:
                    # found spread stop 
                    df.write(self.spreadStopPattern + str(self.spreadStop) + "\n")
                elif self.slopeStartPattern in line:
                    # found slope start 
                    df.write(self.slopeStartPattern + str(self.slopeStart) + "\n")
                elif self.slopeStepPattern in line:
                    # found slope step  
                    df.write(self.slopeStepPattern + str(self.slopeStep) + "\n")
                elif self.slopeStopPattern in line:
                    # found slope stop  
                    df.write(self.slopeStopPattern + str(self.slopeStop) + "\n")
                elif self.roadStartPattern in line:
                    # found road start 
                    df.write(self.roadStartPattern + str(self.roadStart) + "\n")
                elif self.roadStepPattern in line:
                    # found road step 
                    df.write(self.roadStepPattern + str(self.roadStep) + "\n")
                elif self.roadStopPattern in line:
                    # found road stop 
                    df.write(self.roadStopPattern + str(self.roadStop) + "\n")
                elif self.outputDirPattern in line:
                    # found output dir
                    newdir = self.outputDir + fnum
                    #try:
                    print "attempting to create new output dir for {}: {}".format(fnum, newdir)
                    os.mkdir(newdir)
                    #except OSError:
                    #    print "WARNING: file path for output directory already exists, old files may be overwritten"

                    df.write(self.outputDirPattern + newdir + "/ \n")
                else:
                    df.write(line + "\n")
            else:
                df.write(line + "\n")


        
