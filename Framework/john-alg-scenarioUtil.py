from tempfile import mkstemp
from shutil import move
from os import close
import subprocess
import scenario
import os

NO_SPLIT = 0
PART_SPLIT = 1
FULL_SPLIT = 2

class ScenarioUtil:
    num_files = -1
    output_dir = None


    def __init__(self, scen_file_name, dest_path, pieces):
        # read the scenario file
        scen_file = open(scen_file_name, "r")
        scen_lines = scen_file.readlines()
        original = scenario.Scenario()
        self.lines_to_data(scen_lines, original)
        original.print_me()

        # calculate the number of values in each parameter
        combos = self.calc_combos(original)

        gen_scens = []
        val_start = 0
        numper = combos / pieces
        print "pieces: {} -- numper {} -- combos {}".format(pieces, numper, combos)
        print "diffNum {} -- breedNum {} -- spreadNum {} -- slopeNum {} -- roadNum {}".format(original.diffNum, original.breedNum, original.spreadNum, original.slopeNum, original.roadNum)

        for x in range(1,pieces ):
            new_scen = scenario.Scenario()
            val_end = val_start + numper - 1

            print "prepping scenario from {} to {}".format(val_start, val_end)

            di_start = val_start / (original.breedNum * original.spreadNum * original.slopeNum * original.roadNum ) % original.diffNum
            br_start = val_start / (original.spreadNum * original.slopeNum * original.roadNum) % original.breedNum
            sp_start = val_start / (original.slopeNum * original.roadNum) % original.spreadNum
            sl_start = val_start / (original.roadNum) % original.slopeNum
            rd_start = val_start % original.roadNum

            di_end = val_end / (original.breedNum * original.spreadNum * original.slopeNum * original.roadNum ) % original.diffNum
            br_end = val_end / (original.spreadNum * original.slopeNum * original.roadNum) % original.breedNum
            sp_end = val_end / (original.slopeNum * original.roadNum) % original.spreadNum
            sl_end = val_end / (original.roadNum) % original.slopeNum
            rd_end = val_end % original.roadNum
       
            print "diff {} - {} --- breed {} - {} --- spread {} - {} --- slope {} - {} --- road {} - {}".format(di_start, di_end, br_start, br_end, sp_start, sp_end, sl_start, sl_end, rd_start, rd_end)
     
            new_scen.diffStart = original.diffStart + di_start * original.diffStep
            new_scen.breedStart = original.breedStart + br_start * original.breedStep
            new_scen.spreadStart = original.spreadStart + sp_start * original.spreadStep
            new_scen.slopeStart = original.slopeStart + sl_start * original.slopeStep
            new_scen.roadStart = original.roadStart + rd_start * original.roadStep

            new_scen.diffStep = original.diffStep
            new_scen.breedStep = original.breedStep
            new_scen.spreadStep = original.spreadStep
            new_scen.slopeStep = original.slopeStep
            new_scen.roadStep = original.roadStep

            new_scen.diffStop = original.diffStart + di_end * original.diffStep
            new_scen.breedStop = original.breedStart + br_end * original.breedStep
            new_scen.spreadStop = original.spreadStart + sp_end * original.spreadStep
            new_scen.slopeStop = original.slopeStart + sl_end * original.slopeStep
            new_scen.roadStop = original.roadStart + rd_end * original.roadStep

            print "\n -- new scenario --"
            new_scen.print_me()
            print " -------------------- "

            gen_scens.append(new_scen)
            val_start = val_end + 1

        # last one is just val to end
        new_scen = scenario.Scenario()
        val_end = combos - 1
        di_start = val_start / (original.breedNum * original.spreadNum * original.slopeNum * original.roadNum ) % original.diffNum
        br_start = val_start / (original.spreadNum * original.slopeNum * original.roadNum) % original.breedNum
        sp_start = val_start / (original.slopeNum * original.roadNum) % original.spreadNum
        sl_start = val_start / (original.roadNum) % original.slopeNum
        rd_start = val_start % original.roadNum
        
        di_end = val_end / (original.breedNum * original.spreadNum * original.slopeNum * original.roadNum ) % original.diffNum
        br_end = val_end / (original.spreadNum * original.slopeNum * original.roadNum) % original.breedNum
        sp_end = val_end / (original.slopeNum * original.roadNum) % original.spreadNum
        sl_end = val_end / (original.roadNum) % original.slopeNum
        rd_end = val_end % original.roadNum
        
        new_scen.diffStep = original.diffStep
        new_scen.breedStep = original.breedStep
        new_scen.spreadStep = original.spreadStep
        new_scen.slopeStep = original.slopeStep
        new_scen.roadStep = original.roadStep

        new_scen.diffStart = original.diffStart + di_start * original.diffStep
        new_scen.breedStart = original.breedStart + br_start * original.breedStep
        new_scen.spreadStart = original.spreadStart + sp_start * original.spreadStep
        new_scen.slopeStart = original.slopeStart + sl_start * original.slopeStep
        new_scen.roadStart = original.roadStart + rd_start * original.roadStep
        
        new_scen.diffStop = original.diffStart + di_end * original.diffStep
        new_scen.breedStop = original.breedStart + br_end * original.breedStep
        new_scen.spreadStop = original.spreadStart + sp_end * original.spreadStep
        new_scen.slopeStop = original.slopeStart + sl_end * original.slopeStep
        new_scen.roadStop = original.roadStart + rd_end * original.roadStep
        
        print "\n -- new scenario (last one) --"
        new_scen.print_me()
        print " -------------------- "

        gen_scens.append(new_scen)
        # generate the files


    def calc_combos(self, obj):
        obj.diffNum = ((obj.diffStop - obj.diffStart) / obj.diffStep) + 1
        obj.breedNum = ((obj.breedStop - obj.breedStart) / obj.breedStep) + 1
        obj.spreadNum = ((obj.spreadStop - obj.spreadStart) / obj.spreadStep) + 1
        obj.slopeNum = ((obj.slopeStop - obj.slopeStart) / obj.slopeStep) + 1
        obj.roadNum = ((obj.roadStop - obj.roadStart) / obj.roadStep) + 1
        return obj.diffNum * obj.breedNum * obj.spreadNum * obj.slopeNum * obj.roadNum

    def lines_to_data(self, lines, obj):
        for line in lines:
            line = line.strip()
            if line is not "" and line[0] != '#':
                if obj.diffusionStartPattern in line:
                    # found diffusion start
                    obj.diffStart = int(line.split("=")[1].strip())
                elif obj.diffusionStepPattern in line:
                    # found diffusion step
                    obj.diffStep = int(line.split("=")[1].strip())
                elif obj.diffusionStopPattern in line:
                    # found diffusion stop
                    obj.diffStop = int(line.split("=")[1].strip())
                elif obj.spreadStartPattern in line:
                    # found spread start
                    obj.spreadStart = int(line.split("=")[1].strip())
                elif obj.spreadStepPattern in line:
                    # found spread step
                    obj.spreadStep = int(line.split("=")[1].strip())
                elif obj.spreadStopPattern in line:
                    # found spread stop
                    obj.spreadStop = int(line.split("=")[1].strip())
                elif obj.slopeStartPattern in line:
                    # found slope start
                    obj.slopeStart = int(line.split("=")[1].strip())
                elif obj.slopeStepPattern in line:
                    # found slope step
                    obj.slopeStep = int(line.split("=")[1].strip())
                elif obj.slopeStopPattern in line:
                    # found slope stop
                    obj.slopeStop = int(line.split("=")[1].strip())
                elif obj.breedStartPattern in line:
                    # found breed start
                    obj.breedStart = int(line.split("=")[1].strip())
                elif obj.breedStepPattern in line:
                    # found breed step
                    obj.breedStep = int(line.split("=")[1].strip())
                elif obj.breedStopPattern in line:
                    # found breed stop
                    obj.breedStop = int(line.split("=")[1].strip())
                elif obj.roadStartPattern in line:
                    # found road start
                    obj.roadStart = int(line.split("=")[1].strip())
                elif obj.roadStepPattern in line:
                    # found road step
                    obj.roadStep = int(line.split("=")[1].strip())
                elif obj.roadStopPattern in line:
                    # found road stop
                    obj.roadStop = int(line.split("=")[1].strip())


    def get_num_files(self):
        return self.num_files

    def get_output_dir(self):
        return self.output_dir

