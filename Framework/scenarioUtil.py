from tempfile import mkstemp
from shutil import move
from os import close
import subprocess
import scenario
import os

NONE_SPLIT = 0
PART_SPLIT = 1
FULL_SPLIT = 2

NOTV = 0 # not viable - <1
BEST = 1 # best case - no remainder AND 1 < n < 10
NEXT = 2 # next candidates - 1 < n < 10
NOTI = 3 # not ideal - >10

class ScenarioUtil:
    num_files = -1
    output_dir = None


    def __init__(self, scen_file_name, dest_path, pieces):
        # read the scenario file
        scen_file = open(scen_file_name, "r")
        scen_lines = scen_file.readlines()
        self.original = scenario.Scenario()
        self.lines_to_data(scen_lines, original)
        self.original.print_me()
        self.pieces = pieces

        # short-hand for self.original
        orig = self.original
        
        # calculate the number of values in each parameter
        combos = self.calc_combos(original)

        gen_scens = []
        val_start = 0
        numper = combos / pieces
        print "pieces: {} -- numper {} -- combos {}".format(pieces, numper, combos)
        print "diffNum {} -- breedNum {} -- spreadNum {} -- slopeNum {} -- roadNum {}".format(original.diffNum, original.breedNum, original.spreadNum, original.slopeNum, original.roadNum)


        poss_config = self.gen_poss_config()
        selected_config = self.pick_best_config(poss_config)
        
        
        # generate the files
        self.scen_file_list = self.gen_files(selected_config, scen_file_name, dest_path)





    def gen_files(self, sel_cfg, scen_base, dest):
        """
        prep for generating files
          - cd to the correct directory for the files
          - setup the names of the files?
        generate the appropriate scenario objects
        call the appropriate function to generate these files
        returns the list of files
        """

        # cd to the appropriate direcory

        # generate the scenario objects
        scenarios = gen_scen_objs(self, sel_cfg)

        # generate the files
        # ...






    def gen_scen_objs(self, sel_cfg):
        """
        figure out and generate the scenario objects
        """

        # 1 0 0 0 0
        if sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_diff(self)
        # 1 1 0 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_diff_breed(self)
        # 1 1 1 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_diff_breed_spread(self)
        # 1 1 0 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_diff_breed_slope(self)
        # 1 1 0 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_diff_breed_road(self)
        # 1 0 1 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_diff_spread(self)
        # 1 0 1 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_diff_spread_slope(self)
        # 1 0 1 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_diff_spread_road(self)
        # 1 0 0 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_diff_slope(self)
        # 1 0 0 1 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return gen_dist_diff_slope_road(self)
        # 1 0 0 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_diff_road(self)
        # 0 1 0 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_breed(self) 
        # 0 1 1 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_breed_spread(self) 
        # 0 1 1 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_breed_spread_slope(self) 
        # 0 1 1 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_breed_spread_road(self) 
        # 0 1 0 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_breed_slope(self) 
        # 0 1 0 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return gen_dist_breed_slope_road(self) 
        # 0 1 0 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_breed_road(self) 
        # 0 0 1 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return gen_dist_spread(self) 
        # 0 0 1 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_spread_slope(self) 
        # 0 0 1 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return gen_dist_spread_slope_road(self) 
        # 0 0 1 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_spread_road(self) 
        # 0 0 0 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return gen_dist_slope(self) 
        # 0 0 0 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return gen_dist_slope_road(self) 
        # 0 0 0 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return gen_dist_road(self) 
        # oops!
        else:
            print "OOPS!!!!"
            print sel_cfg
            return []



    def gen_dist_diff(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStep, orig.diffStop):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.diffStart = di
            this_scen.diffStop = di
            # everything else is the same
            scens.append(this_scen)

        return scens


    def gen_dist_diff_breed(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          BREED
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStep, orig.diffStop):
            for br in range(orig.breedStart, orig.breedStep, orig.breedStop):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.diffStart = di
                this_scen.diffStop = di
                this_scen.breedStart = br
                this_scen.breedStop = br
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_diff_breed_spread(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          BREED
          Spread
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStep, orig.diffStop):
            for br in range(orig.breedStart, orig.breedStep, orig.breedStop):
                for sp in range(orig.spreadStart, orig.spreadStep, orig.spreadStop):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    # everything else is the same
                    scens.append(this_scen)

        return scens












    def gen_poss_config(self): 
        orig = self.original
        pieces = self.pieces

        poss_config = []
        # generate possible ways to break it up - poss_config = {0 or 1 (no split or split) diff, breed, spread, slope, road; pieces; score; case)

        # one split
        perms = orig.diffNum
        score = orig.diffNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 0, 0, 0, perms, score, case))

        perms = orig.breedNum
        score = orig.breedNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 0, 0, 0, perms, score, case))
        perms = orig.spreadNum
        score = orig.spreadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 1, 0, 0, perms, score, case))
        perms = orig.slopeNum
        score = orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 0, 1, 0, perms, score, case))
        perms = orig.roadNum
        score = orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 0, 0, 1, perms, score, case))

        # two splits
        perms = orig.diffNum * orig.breedNum
        score = orig.diffNum * orig.breedNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 1, 0, 0, 0, perms, score, case))
        perms = orig.diffNum * orig.spreadNum
        score = orig.diffNum * orig.spreadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 1, 0, 0, perms, score, case))
        perms = orig.diffNum * orig.slopeNum
        score = orig.diffNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 0, 1, 0, perms, score, case))
        perms = orig.diffNum * orig.roadNum
        score = orig.diffNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 0, 0, 1, perms, score, case))
        perms = orig.breedNum * orig.spreadNum
        score = orig.breedNum * orig.spreadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 1, 0, 0, perms, score, case))
        perms = orig.breedNum * orig.slopeNum
        score = orig.breedNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 0, 1, 0, perms, score, case))

        perms = orig.breedNum * orig.roadNum
        score = orig.breedNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 0, 0, 1, perms, score, case))
        perms = orig.spreadNum * orig.slopeNum
        score = orig.spreadNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 1, 1, 0, perms, score, case))
        perms = orig.spreadNum * orig.roadNum
        score = orig.spreadNum * orig.roadNum / float(pieces) 
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 1, 0, 1, perms, score, case))
        perms = orig.slopeNum * orig.roadNum
        score = orig.slopeNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 0, 1, 1, perms, score, case))

        # three splits
        perms = orig.diffNum * orig.breedNum * orig.spreadNum
        score = orig.diffNum * orig.breedNum * orig.spreadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 1, 1, 0, 0, perms, score, case))
        perms = orig.diffNum * orig.breedNum * orig.slopeNum
        score = orig.diffNum * orig.breedNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 1, 0, 1, 0, perms, score, case))
        perms = orig.diffNum * orig.breedNum * orig.roadNum
        score = orig.diffNum * orig.breedNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 1, 0, 0, 1, perms, score, case))
        perms = orig.diffNum * orig.spreadNum * orig.slopeNum
        score = orig.diffNum * orig.spreadNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 1, 1, 0, perms, score, case))
        perms = orig.diffNum * orig.spreadNum * orig.roadNum
        score = orig.diffNum * orig.spreadNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 1, 0, 1, perms, score, case))
        perms = orig.diffNum * orig.slopeNum * orig.roadNum
        score = orig.diffNum * orig.slopeNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((1, 0, 0, 1, 1, perms, score, case))
        perms = orig.breedNum * orig.spreadNum * orig.slopeNum
        score = orig.breedNum * orig.spreadNum * orig.slopeNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 1, 1, 0, perms, score, case))
        perms = orig.breedNum * orig.spreadNum * orig.roadNum
        score = orig.breedNum * orig.spreadNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 1, 0, 1, perms, score, case))
        perms = orig.breedNum * orig.slopeNum * orig.roadNum
        score = orig.breedNum * orig.slopeNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 1, 0, 1, 1, perms, score, case))
        perms = orig.spreadNum * orig.slopeNum * orig.roadNum
        score = orig.spreadNum * orig.slopeNum * orig.roadNum / float(pieces)
        case = self.calc_case(perms, pieces)
        poss_config.append((0, 0, 1, 1, 1, perms, score, case))

        # four slits - probably not reasonable...
        #poss_config.append((1, 1, 1, 1, 0, orig.diffNum * orig.breedNum * orig.spreadNum * orig.slopeNum))
        #poss_config.append((1, 0, 1, 1, 1, orig.diffNum * orig.spreadNum * orig.slopeNum * orig.roadNum))
        #poss_config.append((1, 1, 0, 1, 1, orig.diffNum * orig.breedNum * orig.slopeNum * orig.roadNum))
        #poss_config.append((1, 1, 1, 0, 1, orig.diffNum * orig.breedNum * orig.spreadNum * orig.roadNum))
        #poss_config.append((0, 1, 1, 1, 1, orig.breedNum * orig.spreadNum * orig.slopeNum * orig.roadNum))

        return poss_config
    






    def pick_best_config(self, poss_config):
        # find best candidate in each category
        best0 = poss_config[0]
        best1 = poss_config[0]
        best2 = poss_config[0]
        best3 = poss_config[0]

        exists = (False, False, False, False)

        for poss in poss_config:
            print poss
            exists[poss[7]] = True
            if poss[7] == 0:
                if poss[5] > best0[5]: # faster to do int comparison as opposed to float comparisons
                    best0 = poss
            elif poss[7] == 1:
                if poss[6] < best1[6]:
                    best1 = poss
            elif poss[7] == 2:
                if poss[6] - int(poss[6]) > best2[6] - int(best2[6]):
                    best2 = poss
            else: # poss[7] == 3
                if poss[6] < best3[6]:
                    best3 = poss

        selected_config = None
        if exists[0]:
            selected_config = best0
        elif exists[1]:
            selected_config = best1
        elif exists[2]:
            selected_config = best2
        elif exists[3]:
            selected_config = best3

        return selected_config

            

    def calc_case(self, perms, pieces):
        score = perms / float(pieces)
        if score <= 1:
            return NOTV
        elif score >= 10:
            return NOTI
        elif perms % pieces == 0:
            return BEST
        else:
            return NEXT
        



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

