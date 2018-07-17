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
        
    def write_file(self, orig_file, dest_file):
        
        
