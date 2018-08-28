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


    def __init__(self, scen_file_name, dest_path, pieces, log_file):
        # read the scenario file
        self.original = scenario.Scenario()
        self.original.read_file(scen_file_name)
        self.original.print_me()
        self.pieces = pieces
        self.output_dir = self.original.outputDir
        self.log_file = log_file
        # short-hand for self.original
        orig = self.original
        
        # calculate the number of values in each parameter
        combos = self.calc_combos(orig)

        gen_scens = []
        val_start = 0
        numper = combos / pieces
        print >> log_file, "pieces: {} -- numper {} -- combos {}".format(pieces, numper, combos)
        print >> log_file, "diffNum {} -- breedNum {} -- spreadNum {} -- slopeNum {} -- roadNum {}".format(orig.diffNum, orig.breedNum, orig.spreadNum, orig.slopeNum, orig.roadNum)


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
        log_file = self.log_file
        file_list = []

        # cd to the appropriate directory

        try:
            os.makedirs(dest)
        except OSError:
            print >> log_file, "WARNING: file path exists for scenario files, old files may be overwritten"

        try:
            os.makedirs(self.original.outputDir)
        except OSError:
            print >> log_file, "WARNING: file path exists for output files, old files may be overwritten"

        # generate the scenario objects
        scenarios = self.gen_scen_objs(sel_cfg, log_file)
        i = 1
        for scen in scenarios:
            scen.print_me(log_file)
            print >> log_file, " ------ "
            scen.write_file(scen_base, dest + str(i), str(i))
            file_list.append(str(i))
            i += 1
                            
        return file_list





    def gen_scen_objs(self, sel_cfg, log_file):
        """
        figure out and generate the scenario objects
        """

        # 1 0 0 0 0
        if sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_diff()
        # 1 1 0 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_diff_breed()
        # 1 1 1 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_diff_breed_spread()
        # 1 1 0 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_diff_breed_slope()
        # 1 1 0 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_diff_breed_road()
        # 1 0 1 0 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_diff_spread()
        # 1 0 1 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_diff_spread_slope()
        # 1 0 1 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_diff_spread_road()
        # 1 0 0 1 0
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_diff_slope()
        # 1 0 0 1 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return self.gen_dist_diff_slope_road()
        # 1 0 0 0 1
        elif sel_cfg[0] == 1 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_diff_road()
        # 0 1 0 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_breed() 
        # 0 1 1 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_breed_spread() 
        # 0 1 1 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_breed_spread_slope() 
        # 0 1 1 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_breed_spread_road() 
        # 0 1 0 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_breed_slope() 
        # 0 1 0 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return self.gen_dist_breed_slope_road() 
        # 0 1 0 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 1 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_breed_road() 
        # 0 0 1 0 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 0:
            return self.gen_dist_spread() 
        # 0 0 1 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_spread_slope() 
        # 0 0 1 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return self.gen_dist_spread_slope_road() 
        # 0 0 1 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 1 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_spread_road() 
        # 0 0 0 1 0
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 0:
            return self.gen_dist_slope() 
        # 0 0 0 1 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 1 and sel_cfg[4] == 1:
            return self.gen_dist_slope_road() 
        # 0 0 0 0 1
        elif sel_cfg[0] == 0 and sel_cfg[1] == 0 and sel_cfg[2] == 0 and sel_cfg[3] == 0 and sel_cfg[4] == 1:
            return self.gen_dist_road() 
        # oops!
        else:
            print >> log_file, "OOPS!!!!"
            print >> log_file, sel_cfg
            return []



    def gen_dist_diff(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
        """
        scens = []
        orig = self.original
        # Note: python syntax for range(start, stop (exclusive), step)
        # SLEUTH uses an inclusive stop value, hence the +1
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.diffStart = di
            this_scen.diffStop = di
            # everything else is the same
            #this_scen.print_me()
            scens.append(this_scen)

        return scens


    def gen_dist_breed(self):
        """
        generate and return scenario based on original distributing by:
          BREED
        """
        scens = []
        orig = self.original
        # Note: python syntax for range(start, stop (exclusive), step)
        # SLEUTH uses an inclusive stop value, hence the +1
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.breedStart = br
            this_scen.breedStop = br
            # everything else is the same
            #this_scen.print_me()
            scens.append(this_scen)

        return scens


    def gen_dist_spread(self):
        """
        generate and return scenario based on original distributing by:
          SPREAD
        """
        scens = []
        orig = self.original
        # Note: python syntax for range(start, stop (exclusive), step)
        # SLEUTH uses an inclusive stop value, hence the +1
        for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.spreadStart = sp
            this_scen.spreadStop = sp
            # everything else is the same
            #this_scen.print_me()
            scens.append(this_scen)

        return scens


    def gen_dist_slope(self):
        """
        generate and return scenario based on original distributing by:
          SLOPE
        """
        scens = []
        orig = self.original
        # Note: python syntax for range(start, stop (exclusive), step)
        # SLEUTH uses an inclusive stop value, hence the +1
        for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.slopeStart = sl
            this_scen.slopeStop = sl
            # everything else is the same
            #this_scen.print_me()
            scens.append(this_scen)

        return scens

    def gen_dist_road(self):
        """
        generate and return scenario based on original distributing by:
          ROAD
        """
        scens = []
        orig = self.original
        # Note: python syntax for range(start, stop (exclusive), step)
        # SLEUTH uses an inclusive stop value, hence the +1
        for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
            this_scen = scenario.Scenario()
            this_scen.copy(orig)
            this_scen.roadStart = rd
            this_scen.roadStop = rd
            # everything else is the same
            #this_scen.print_me()
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
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.diffStart = di
                this_scen.diffStop = di
                this_scen.breedStart = br
                this_scen.breedStop = br
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_diff_spread(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          SPREAD
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.diffStart = di
                this_scen.diffStop = di
                this_scen.spreadStart = sp
                this_scen.spreadStop = sp
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_diff_slope(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          SLOPE
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.diffStart = di
                this_scen.diffStop = di
                this_scen.slopeStart = sl
                this_scen.slopeStop = sl
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_diff_road(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          ROAD
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.diffStart = di
                this_scen.diffStop = di
                this_scen.roadStart = rd
                this_scen.roadStop = rd
                # everything else is the same
                scens.append(this_scen)

        return scens


    def gen_dist_breed_spread(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          SPREAD
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.breedStart = br
                this_scen.breedStop = br
                this_scen.spreadStart = sp
                this_scen.spreadStop = sp
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_breed_slope(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          SLOPE
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.breedStart = br
                this_scen.breedStop = br
                this_scen.slopeStart = sl
                this_scen.slopeStop = sl
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_breed_road(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          ROAD
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.breedStart = br
                this_scen.breedStop = br
                this_scen.roadStart = rd
                this_scen.roadStop = rd
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_spread_slope(self):
        """
        generate and return scenario based on original distributing by:
          SPRED
          SLOPE
        """
        scens = []
        orig = self.original
        for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.spreadStart = sp
                this_scen.spreadStop = sp
                this_scen.slopeStart = sl
                this_scen.slopeStop = sl
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_spread_road(self):
        """
        generate and return scenario based on original distributing by:
          SPREAD
          ROAD
        """
        scens = []
        orig = self.original
        for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
            for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.spreadStart = sp
                this_scen.spreadStop = sp
                this_scen.roadStart = rd
                this_scen.roadStop = rd
                # everything else is the same
                scens.append(this_scen)

        return scens

    def gen_dist_slope_road(self):
        """
        generate and return scenario based on original distributing by:
          SLOPE
          ROAD
        """
        scens = []
        orig = self.original
        for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
            for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                this_scen = scenario.Scenario()
                this_scen.copy(orig)
                this_scen.slopeStart = sl
                this_scen.slopeStop = sl
                this_scen.roadStart = rd
                this_scen.roadStop = rd
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
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
                for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
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

    def gen_dist_diff_breed_slope(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          BREED
          Spread
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
                for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    # everything else is the same
                    scens.append(this_scen)

        return scens
    def gen_dist_diff_breed_road(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          BREED
          road
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_diff_spread_slope(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          Spread
          slope
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_diff_spread_road(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          Spread
          ROAD
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_diff_slope_road(self):
        """
        generate and return scenario based on original distributing by:
          DIFFUSION
          SLOPE
          ROAD
        """
        scens = []
        orig = self.original
        for di in range(orig.diffStart, orig.diffStop + 1, orig.diffStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.diffStart = di
                    this_scen.diffStop = di
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_breed_spread_slope(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          SPREAD
          SLOPE
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_breed_spread_road(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          SPREAD
          ROAD
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_breed_slope_road(self):
        """
        generate and return scenario based on original distributing by:
          BREED
          SLOPE
          ROAD
        """
        scens = []
        orig = self.original
        for br in range(orig.breedStart, orig.breedStop + 1, orig.breedStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.breedStart = br
                    this_scen.breedStop = br
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
                    # everything else is the same
                    scens.append(this_scen)

        return scens

    def gen_dist_spread_slope_road(self):
        """
        generate and return scenario based on original distributing by:
          SPREAD
          SLOPE
          ROAD
        """
        scens = []
        orig = self.original
        for sp in range(orig.spreadStart, orig.spreadStop + 1, orig.spreadStep):
            for sl in range(orig.slopeStart, orig.slopeStop + 1, orig.slopeStep):
                for rd in range(orig.roadStart, orig.roadStop + 1, orig.roadStep):
                    this_scen = scenario.Scenario()
                    this_scen.copy(orig)
                    this_scen.spreadStart = sp
                    this_scen.spreadStop = sp
                    this_scen.slopeStart = sl
                    this_scen.slopeStop = sl
                    this_scen.roadStart = rd
                    this_scen.roadStop = rd
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

        exists = [False, False, False, False]

        for poss in poss_config:
            print poss, poss[7]

            if poss[7] == 0:
                if exists[0] == False or poss[5] > best0[5]: # faster to do int comparison as opposed to float comparisons
                    best0 = poss
                    print "new best 0"
            elif poss[7] == 1:
                if exists[1] == False or poss[6] < best1[6]:
                    best1 = poss
                    print "new best 1"
            elif poss[7] == 2:
                if exists[2] == False or poss[6] - int(poss[6]) > best2[6] - int(best2[6]):
                    best2 = poss
                    print "new best 2"
            else: # poss[7] == 3
                if exists[3] == False or poss[6] < best3[6]:
                    best3 = poss
                    print "new best 3"
            exists[poss[7]] = True

        selected_config = None

        if exists[1]:
            selected_config = best1
        elif exists[2]:
            selected_config = best2
        elif exists[3]:
            selected_config = best3
        elif exists[0]:
            selected_config = best0

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



    def get_num_files(self):
        return len(self.scen_file_list)

    def get_output_dir(self):
        return self.output_dir

