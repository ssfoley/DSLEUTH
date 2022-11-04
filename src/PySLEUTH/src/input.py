class Input:

    @staticmethod
    def read_file_to_grid(filename, grid):
        grid.clear_gridData()
        with open(filename, "r") as f:
            line_data = f.readline()
            for data in line_data.split():
                grid.gridData.append(float(data))

    @staticmethod
    def copy_metadata(filename):
        metadata = []
        with open(filename, "r") as orig:
            for line in orig:
                metadata.append(line)
        return metadata



    @staticmethod
    def read_restart_file(outputdir):

        filename = '{}{}{}'.format(outputdir, UGMDefines.RESTART_FILE, Globals.mype)

        print(f"Reading restart file: {filename}")

        with open(filename, 'r') as f:  # open the file for reading
            line = f.readline()
            if len(line.split()) != 7:
                print(f"EOF occurred when reading file {filename}")
                sys.exit(1)
            # split it by whitespace
            diffusion, breed, spread, slope_resistance, road_gravity, random_seed, counter = line.split()
            diffusion = int(diffusion)
            breed = int(breed)
            spread = int(spread)
            slope_resistance = int(slope_resistance)
            road_gravity = int(road_gravity)
            random_seed = int(random_seed)
            counter = int(counter)

        return diffusion, breed, spread, slope_resistance, road_gravity, random_seed, counter
