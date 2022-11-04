from grid import Grid
import sys
import os
from globals import Globals
from ugm_defines import UGMDefines
from imageIO import ImageIO
from color import Color
from logger import Logger
from landClass import LandClass
from scenario import Scenario
from input import Input
from output import Output

class IGrid:
    igrid = None
    igrid_count = -1
    road_pixel_count = [0] * UGMDefines.MAX_ROAD_YEARS
    excld_count = -1
    percent_road = [0.0] * UGMDefines.MAX_ROAD_YEARS
    total_pixels = -1
    nrows = -1
    ncols = -1
    using_gif = True

    @staticmethod
    def init(packing, processing_type):
        IGrid.igrid_count = 0
        IGrid.igrid = IGridInfo()
        input_location = Scenario.get_scen_value("slope_data")
        IGrid.set_location(input_location)
        IGrid.set_filenames(packing, processing_type)

    @staticmethod
    def get_total_pixels():
        return IGrid.total_pixels

    @staticmethod
    def get_road_pixel_count(year):
        for i in range(IGrid.igrid.get_num_road() - 1, 0, -1):
            if year >= IGrid.igrid.get_road_year(i):
                return IGrid.road_pixel_count[i]

        return IGrid.road_pixel_count[0]

    @staticmethod
    def get_excld_count():
        return IGrid.excld_count

    @staticmethod
    def set_location(filename):
        try:
            location_name = filename.split('.')[0]
            IGrid.igrid.location = location_name
        except Exception:
            print("Error with Data Location in Scenario File")
            sys.exit()

    @staticmethod
    def set_filenames(packing, processing_type):
        start_year = int(Scenario.get_scen_value('prediction_start_date'))

        # Urban data
        for filename in Scenario.get_scen_value('urban_data_file'):
            full_path = Scenario.get_scen_value('input_dir') + filename
            filename_parts = filename.split(".")
            this_year = int(filename_parts[2])
            base = ".".join(filename_parts[:-1])
            if processing_type == Globals.mode_enum['predict']:
                if this_year >= start_year:
                    IGrid.create_urban_grid_obj(full_path, this_year, packing, base)
            else:
                IGrid.create_urban_grid_obj(full_path, this_year, packing, base)

        # Road Data
        for filename in Scenario.get_scen_value('road_data_file'):
            full_path = Scenario.get_scen_value('input_dir') + filename
            filename_parts = filename.split(".")
            this_year = int(filename_parts[2])
            base = ".".join(filename_parts[:-1])
            if processing_type == 0:  # if it is predicting
                if this_year >= start_year or len(Scenario.get_scen_value('road_data_file')) == 1:
                    IGrid.create_road_grid_obj(full_path, this_year, packing, base)
            else:
                IGrid.create_road_grid_obj(full_path, this_year, packing, base)

        # landuse
        for filename in Scenario.get_scen_value('landuse_data_file'):
            full_path = Scenario.get_scen_value('input_dir') + filename
            filename_parts = filename.split(".")
            this_year = int(filename_parts[2])
            base = ".".join(filename_parts[:-1])
            IGrid.create_landuse_grid_obj(full_path, this_year, packing, base)

        # excluded
        IGrid.igrid.excluded.filename = Scenario.get_scen_value('input_dir') + Scenario.get_scen_value('excluded_data')
        IGrid.set_grid_sizes(IGrid.igrid.excluded)
        IGrid.igrid.excluded.year = 0
        IGrid.igrid.excluded.packed = packing
        IGrid.igrid.excluded_count = 1
        IGrid.igrid_count = IGrid.igrid_count + 1

        # slope
        IGrid.igrid.slope.filename = Scenario.get_scen_value('input_dir') + Scenario.get_scen_value('slope_data')
        IGrid.set_grid_sizes(IGrid.igrid.slope)
        IGrid.igrid.slope.year = 0
        IGrid.igrid.slope.packed = packing
        IGrid.igrid.slope_count = 1
        IGrid.igrid_count = IGrid.igrid_count + 1
        IGrid.ncols = IGrid.igrid.slope.ncols
        IGrid.nrows = IGrid.igrid.slope.nrows

        IGrid.using_gif = IGrid.igrid.slope.filename.endswith('.gif')

        #print(f"Ncols {IGrid.ncols} Nrows {IGrid.nrows}")

        # background
        IGrid.igrid.background.filename = Scenario.get_scen_value('input_dir') + Scenario.get_scen_value('background_data')
        IGrid.set_grid_sizes(IGrid.igrid.background)
        IGrid.igrid.background.year = 0
        IGrid.igrid.background.packed = packing
        IGrid.igrid.background_count = 1
        IGrid.igrid_count = IGrid.igrid_count + 1

    @staticmethod
    def create_urban_grid_obj(full_path, year, packed, base):
        new_grid = Grid()
        new_grid.filename = full_path
        IGrid.set_grid_sizes(new_grid)
        new_grid.year = year
        new_grid.base = base
        new_grid.packed = packed
        IGrid.igrid.urban.append(new_grid)
        IGrid.igrid_count = IGrid.igrid_count + 1

    @staticmethod
    def create_road_grid_obj(full_path, year, packed, base):
        new_grid = Grid()
        new_grid.filename = full_path
        IGrid.set_grid_sizes(new_grid)
        new_grid.year = year
        new_grid.base = base
        new_grid.packed = packed
        IGrid.igrid.road.append(new_grid)
        IGrid.igrid_count = IGrid.igrid_count + 1

    @staticmethod
    def create_landuse_grid_obj(full_path, year, packed, base):
        new_grid = Grid()
        new_grid.filename = full_path
        IGrid.set_grid_sizes(new_grid)
        new_grid.year = year
        new_grid.base = base
        new_grid.packed = packed
        IGrid.igrid.landuse.append(new_grid)
        IGrid.igrid_count = IGrid.igrid_count + 1

    @staticmethod
    def test_for_urban_year(year):
        for i in range(IGrid.igrid.get_num_urban()):
            if IGrid.igrid.urban[i].year == year:
                return True
        return False

    @staticmethod
    def set_grid_sizes(grid):
        '''gif_row_offset = 8
        gif_col_offset = 6
        gif_res_offset = 10

        # grid.filename = "../Input/demo200/demo200.landuse.1930.gif"

        if Globals.mype == 0:
            with open(grid.filename, 'rb') as f:  # open the file for reading
                bytes_to_read = f.read(15)

            if b'GIF87a' not in bytes_to_read and b'GIF89a' not in bytes_to_read:
                print(grid.filename + ' not a valid GIF file')
                sys.exit()

            # ncols = (bytes_to_read[gif_col_offset] << 8) | (bytes_to_read[gif_col_offset  + 1])
            # nrows = (bytes_to_read[gif_row_offset] << 8) | (bytes_to_read[gif_row_offset  + 1])
            ncols = bytes_to_read[gif_col_offset]
            nrows = bytes_to_read[gif_row_offset]

            print((bytes_to_read[gif_col_offset] << 8) | (bytes_to_read[gif_col_offset + 1]))

            print(f"ncols {ncols} nrows {nrows}")'''

        ncols, nrows = ImageIO.get_size(grid.filename)



        grid.ncols = ncols
        grid.nrows = nrows

        IGrid.total_pixels = nrows * ncols

        # resolution = int(bytes_to_read[gif_res_offset])
        #grid.color_bits = (((resolution & 112) >> 4) + 1)
        #grid.bits_per_pixel = (resolution & 7) + 1

        size_bytes = sys.getsizeof(int()) * ncols * nrows
        grid.size_bytes = size_bytes
        grid.size_words = UGMDefines.round_bytes_to_word_boundary(size_bytes)

    @staticmethod
    def read_input_files(packing, save_echo_image, outputdir):
        IGrid.read_input_file(IGrid.igrid.urban, packing, save_echo_image, outputdir)
        IGrid.read_input_file(IGrid.igrid.road, packing, save_echo_image, outputdir)
        IGrid.read_input_file(IGrid.igrid.landuse, packing, save_echo_image, outputdir)
        IGrid.read_input_file([IGrid.igrid.excluded], packing, save_echo_image, outputdir)
        IGrid.read_input_file([IGrid.igrid.slope], packing, save_echo_image, outputdir)
        IGrid.read_input_file([IGrid.igrid.background], packing, save_echo_image, outputdir)

        IGrid.count_road_pixels()
        IGrid.calculate_percent_roads()
        '''for grid in IGrid.igrid.urban:
            print(grid.filename)
            IGrid.read_into_grid(grid.filename, grid, save_echo_image, packing)
            grid.fill_histogram()'''

    @staticmethod
    def read_input_file(gif_grids, packing, save_echo_image, outputdir):
        for grid in gif_grids:
            grid.gridData = [0] * IGrid.total_pixels
            IGrid.read_into_grid(grid.filename, grid, save_echo_image, packing, outputdir)
            grid.fill_histogram()

    @staticmethod
    def read_into_grid(filepath, grid, save_echo_image, packing, outputdir):
        ImageIO.read_gif(grid, filepath, IGrid.nrows, IGrid.ncols)
        if save_echo_image:
            IGrid.echo_input(grid, outputdir, filepath)

    @staticmethod
    def echo_input(grid, outputdir, filepath):
        filename = IGrid.extract_filename(filepath)
        path = outputdir + "echo_of_" + filename
        ImageIO.write_gif(grid, Color.get_grayscale_table(), path, None, IGrid.nrows, IGrid.ncols)

        if not IGrid.using_gif:
            base = filename.split(".")
            base = ".".join(base[:-1])
            meta_filename = f"{base}.tfw"
            IGrid.echo_meta(meta_filename, "echo")

    @staticmethod
    def echo_meta(filepath, grid_name):
        '''
        self.urban = []  # list of grid objects
        self.road = []  # list of grid objects
        self.landuse = []  # list of grid objects
        self.excluded = Grid()
        self.slope = Grid()
        self.background = Grid()
        '''
        input_dir = Scenario.get_scen_value('input_dir')
        output_dir = Scenario.get_scen_value('output_dir')
        base = ""
        # figure out which metadata to use
        if grid_name == 'landuse':
            base = f"{input_dir}{IGrid.igrid.landuse[0].base}.tfw"
        elif grid_name == 'urban':
            base = f"{input_dir}{IGrid.igrid.urban[0].base}.tfw"
        elif grid_name == 'echo':
            base = f"{input_dir}{filepath}"
            filepath = f"{output_dir}echo_of_{filepath}"

        # Read meta data from correct file
        metadata = Input.copy_metadata(base)
        Output.write_list_to_file(filepath, metadata)

    @staticmethod
    def extract_filename(filepath):
        return os.path.basename(filepath)

    @staticmethod
    def extract_dir(filepath):
        grid_filename = IGrid.extract_filename(filepath)
        file_dir = grid_filename.split(".")[0]
        return file_dir

    @staticmethod
    def validate_grids(log_it):
        try:
            # validate urban
            IGrid.validate_histogram(log_it, IGrid.igrid.urban, "urban")
            # validate road
            IGrid.validate_histogram(log_it, IGrid.igrid.road, "road")
            # validate landuse
            IGrid.validate_histogram_landuse(log_it)
            # validate slope
            IGrid.validate_auto(log_it, "slope", IGrid.igrid.slope.filename)
            # validate excluded
            IGrid.validate_auto(log_it, "excluded", IGrid.igrid.excluded.filename)
            # validate background
            IGrid.validate_auto(log_it, "background", IGrid.igrid.background.filename)

            if log_it:
                Logger.log("\nValidation OK")
                Logger.log("******************************************************")
                Logger.log("******************************************************")
        except ValueError:
            if log_it:
                Logger.log("\nError")
                Logger.log("Input data images contain errors.")
                Logger.close()
            else:
                print("Error")
                print("Input data images contain errors")
            sys.exit(1)

    @staticmethod
    def validate_histogram(log_it, grids, grids_name):
        for grid in grids:
            # print(grid.filename)
            if log_it:
                Logger.log(f"\nValidating {grids_name} input grid: {grid.filename}")
                Logger.log("Index Count PercentOfImage")
                for i in range(256):
                    if grid.histogram[i] > 0:
                        Logger.log(f'{i} {grid.histogram[i]} {100.0 * grid.histogram[i] / IGrid.total_pixels}%')
                        # print(f'{i} {grid.histogram[i]} {100.0 * grid.histogram[i] / IGrid.total_pixels}%')

            if grid.histogram[0] == 0:
                error_message = f'Error input grid: {grid.filename} is 100% {grids_name}'
                if log_it:
                    Logger.log(error_message)
                else:
                    print(error_message)
                raise ValueError

    @staticmethod
    def validate_histogram_landuse(log_it):
        for grid in IGrid.igrid.landuse:
            if log_it:
                Logger.log(f"\nValidating landuse input grid: {grid.filename}")
                Logger.log("Index Count PercentOfImage")

                landuse_class_nums = [landuse_class.num for landuse_class
                                      in LandClass.landuse_classes]
                '''for num in grid.histogram:
                    print(f"****{num}****")'''

                for i in range(256):
                    # print(f"value: {grid.histogram[i]}")
                    if grid.histogram[i] > 0:
                        if log_it:
                            Logger.log(
                                f'{i} {grid.histogram[i]} {100.0 * grid.histogram[i] / IGrid.total_pixels}%')
                        if i not in landuse_class_nums:
                            if log_it:
                                Logger.log(f'Error -> landuse type {i} appears in file: {grid.filename}')
                            raise ValueError

    @staticmethod
    def validate_auto(log_it, name, filename):
        if log_it:
            Logger.log(f"\nValidating {name} input grid: {filename}")

    @staticmethod
    def count_road_pixels():
        # igrid_CountRoadPixels
        road_count = IGrid.igrid.get_num_road()
        for i in range(road_count):
            roads = IGrid.igrid.get_road_grid(i)
            count = 0
            for j in range(IGrid.total_pixels):
                if roads[j] > 0:
                    count += 1
            IGrid.road_pixel_count[i] = count

    @staticmethod
    def calculate_percent_roads():
        excld_grid = IGrid.igrid.get_excld_grid()

        count = 0
        for i in range(IGrid.total_pixels):
            if excld_grid[i] >= 100:
                count += 1

        IGrid.excld_count = count

        if IGrid.total_pixels - count <= 0:
            msg = f"Total pixels = {IGrid.total_pixels} exlcuded count = {count}"
            Logger.log(msg)
            exit(1)

        for i in range(IGrid.igrid.get_num_road()):
            IGrid.percent_road[i] = (100 * IGrid.road_pixel_count[i] / (IGrid.total_pixels - count))

    @staticmethod
    def normalize_roads():
        max_road_max = max([road.max for road
                            in IGrid.igrid.road])
        test_file = open(f"{Scenario.get_scen_value('output_dir')}testRoadNormalize", 'w')
        test_file.write(f"Max Road: {max_road_max}\n")
        for road in IGrid.igrid.road:
            test_file.write("*****************************\n")
            norm_factor = float(road.max) / float(max_road_max)
            test_file.write(f"image_max: {road.max}\n")
            test_file.write(f"norm_factor: {norm_factor}\n")
            for i in range(IGrid.total_pixels):
                cur_pixel = road.gridData[i]
                road.gridData[i] = int(((100 * cur_pixel) / road.max) * norm_factor)
                test_file.write(f"{cur_pixel} => {road.gridData[i]}\n")

        test_file.close()

    @staticmethod
    def verify_inputs(log_it, landuse_flag):
        if log_it:
            IGrid.log_info()
            Logger.log("\nVerifying Data Input Files")
        # start verification
        dir_list = []
        same_size_u, same_location_u = IGrid.verify_inputs_grid(IGrid.igrid.urban, dir_list)
        same_size_r, same_location_r = IGrid.verify_inputs_grid(IGrid.igrid.road, dir_list)
        same_size_l, same_location_l = IGrid.verify_inputs_grid(IGrid.igrid.landuse, dir_list)
        same_size_e, same_location_e = IGrid.verify_inputs_grid([IGrid.igrid.excluded], dir_list)
        same_size_b, same_location_b = IGrid.verify_inputs_grid([IGrid.igrid.background], dir_list)

        if not same_size_u or not same_size_r or not same_size_l or not same_size_e or not same_size_b:
            msg = "GIFs are not all the same size. Please check your input image sizes."
            if log_it:
                Logger.log(msg)
                Logger.close()
            else:
                print(msg)
            exit(1)
        if not same_location_u or not same_location_r or not same_location_l or not same_location_e or not same_location_b:
            msg = "GIFs are not all the same location. Please check your scenario file."
            if log_it:
                Logger.log(msg)
                Logger.close()
            else:
                print(msg)
            exit(1)
        if landuse_flag:
            if IGrid.igrid.landuse[1].year != IGrid.igrid.urban[len(IGrid.igrid.urban) - 1].year:
                msg1 = "Last landuse year does not match last urban year."
                msg2 = f"last landuse year = {IGrid.igrid.landuse[1].year} last urban year = {IGrid.igrid.urban[len(IGrid.igrid.urban) - 1].year}"
                if log_it:
                    Logger.log(f"{msg1}\n{msg2}")
                    Logger.close()
                else:
                    print(f"{msg1}\n{msg2}")
                exit(1)
        if log_it:
            Logger.log(f"igrid.py Data Input Files: OK")

    @staticmethod
    def verify_inputs_grid(grids, dir_list):
        same_size = True
        for grid in grids:
            if IGrid.nrows != grid.nrows or IGrid.ncols != grid.ncols:
                same_size = False
            dir_name = IGrid.extract_dir(grid.filename)
            if dir_name not in dir_list:
                dir_list.append(dir_name)

        same_location = len(dir_list) == 1
        return same_size, same_location

    @staticmethod
    def log_info():
        Logger.log("\n")
        asterisk = "*******************************************************"
        Logger.log(asterisk + "INPUT GIFs" + asterisk)
        Logger.log("\n")
        IGrid.log_grid_info(IGrid.igrid.urban, "Urban")
        IGrid.log_grid_info(IGrid.igrid.road, "Road")
        IGrid.log_grid_info(IGrid.igrid.landuse, "Landuse")
        IGrid.log_grid_info([IGrid.igrid.excluded], "Excluded")
        IGrid.log_grid_info([IGrid.igrid.slope], "Slope")
        IGrid.log_grid_info([IGrid.igrid.background], "Background")
        Logger.log("cb = # of color bits")
        Logger.log("bpp = # bits per pixel")
        Logger.log("\n")

    @staticmethod
    def log_grid_info(grids, gifs_name):
        Logger.log("\t" + gifs_name + " GIF(s)")
        Logger.log(f"\t\trowXcol  cb   bpp   min   max   path")
        for grid in grids:
            gsze = f"\t\t{grid.nrows}X{grid.ncols}"
            Logger.log(
                f'{gsze} {grid.color_bits:3.0f}   {grid.bits_per_pixel:3.0f}   {grid.min:3.0f}   {grid.max:3.0f}   {grid.filename}')

    @staticmethod
    def debug(caller):
        igrid = IGrid.igrid
        Logger.log("\n*******************************************")
        Logger.log(f"IGrid Debug: Caller {caller}")
        Logger.log("*******************************************")

        Logger.log(f"igrid.location = {igrid.get_location()}")
        Logger.log(f"igrid.urban_count = {igrid.get_num_urban()}")
        Logger.log(f"igrid.road_count = {igrid.get_num_road()}")
        Logger.log(f"igrid.landuse_count = {igrid.get_num_landuse()}")
        Logger.log(f"igrid.excluded_count = 1")
        Logger.log(f"igrid.slope_count = 1")
        Logger.log(f"igrid.background_count = 1\n")

        IGrid.igrid.log_urban()
        IGrid.igrid.log_road()
        IGrid.igrid.log_landuse()
        IGrid.igrid.log_excld()
        IGrid.igrid.log_slope()
        IGrid.igrid.log_background()

        Logger.log("*******************************************")

    @staticmethod
    def wrap_list(grid_data_list):
        temp = Grid()
        temp.gridData = grid_data_list
        return temp

class IGridInfo:
    def __init__(self):
        self.location = ""
        self.excluded_count = -1
        self.slope_count = -1
        self.background_count = -1

        self.urban = []  # list of grid objects
        self.road = []  # list of grid objects
        self.landuse = []  # list of grid objects
        self.excluded = Grid()
        self.slope = Grid()
        self.background = Grid()

    def get_location(self):
        return self.location

    def get_num_landuse(self):
        return len(self.landuse)

    def get_landuse_idx(self, idx):
        return self.landuse[idx]

    def get_landuse_igrid(self, idx):
        return self.landuse[idx].gridData

    def get_landuse_year(self, idx):
        return self.landuse[idx].year

    def log_landuse(self):
        for landuse in self.landuse:
            Logger.log(landuse.log_grid())

    def get_slope(self):
        return self.slope

    def get_slope_grid(self):
        return self.slope.gridData


    def log_slope(self):
        Logger.log(self.slope.log_grid())

    def get_urban(self):
        return self.urban

    def get_num_urban(self):
        return len(self.urban)

    def get_urban_idx(self, idx):
        return self.urban[idx]

    def get_urban_year(self, idx):
        return self.urban[idx].year

    def log_urban(self):
        for urban in self.urban:
            Logger.log(urban.log_grid())

    def get_urban_grid(self, idx):
        return self.urban[idx].gridData

    def get_urban_grid_by_yr(self, year):
        for i in range(self.get_num_urban() - 1, 0, -1):
            if year >= self.urban[i].year:
                return self.urban[i].gridData

        return self.urban[0].gridData

    def urban_yr_to_idx(self, year):
        for i, urban in enumerate(self.urban):
            if urban.year == year:
                return i

        print(f"Error -> year={year} is not an urban year")
        sys.exit(1)

    def get_num_road(self):
        return len(self.road)

    def get_road_grid(self, idx):
        return self.road[idx].gridData

    def get_road_grid_by_year(self, year):
        for i in range(len(self.road) - 1, 0):
            if year >= self.road[i].year:
                return self.road[i].gridData
        return self.road[0].gridData

    def get_road_year(self, index):
        return self.road[index].year

    def log_road(self):
        for road in self.road:
            Logger.log(road.log_grid())

    def get_excld(self):
        return self.excluded

    def get_excld_grid(self):
        return self.excluded.gridData

    def log_excld(self):
        Logger.log(self.excluded.log_grid())

    def get_background(self):
        return self.background

    def get_background_grid(self):
        return self.background.gridData

    def log_background(self):
        Logger.log(self.background.log_grid())

    def __str__(self):
        return self.location + " " + str(self.excluded_count) + " " + str(self.slope_count) + " " + str(
            self.background_count)
