from landClass import LandClass
from imageIO import ImageIO
from logger import Logger
from scenario import Scenario

# TODO: use tools from Pillow to get these values more effiently

class Color:
    _max_color = 256
    _red_mask = 0XFF0000
    _green_mask = 0X00FF00
    _blue_mask = 0X0000FF

    color_table_landuse = None
    color_table_probability = None
    color_table_growth = None
    color_table_deltatron = None
    color_table_grayscale = None

    @staticmethod
    def get_landuse_table():
        return Color.color_table_landuse

    @staticmethod
    def get_probability_table():
        return Color.color_table_probability

    @staticmethod
    def get_growth_table():
        return Color.color_table_growth

    @staticmethod
    def get_deltatron_table():
        return Color.color_table_deltatron

    @staticmethod
    def get_grayscale_table():
        return Color.color_table_grayscale

    @staticmethod
    def init(grid_ncols):
        Color.color_fill()

        Color.write_color_key_all(grid_ncols)

    @staticmethod
    def color_fill():
        Color.__init_grayscale()
        Color.__init_landuse()
        Color.__init_probability()
        Color.__init_deltatron(Scenario.get_scen_value('deltatron_color'))
        Color.__init_growth()
        Color.__color_user_overrides()

    @staticmethod
    def __init_grayscale():
        Color.color_table_grayscale = ColorTable(Color._max_color, "GRAYSCALE_COLORMAP")
        Color.color_table_grayscale.color = [(val, val, val) for val in list(range(Color._max_color))]

    @staticmethod
    def __init_landuse():
        Color.color_table_landuse = ColorTable(Color._max_color, "LANDUSE_COLORMAP")
        Color.color_table_landuse.color = [(landuse_class.red, landuse_class.green, landuse_class.blue)
                                           for landuse_class in LandClass.landuse_classes]
        Color.color_table_landuse.size = len(Color.color_table_landuse.color)

    @staticmethod
    def __init_probability():
        Color.color_table_probability = ColorTable(0, "PROBABILITY_COLORMAP")
        Color.color_table_probability.color = []
        # self.color_table_probability.color = [(val, val, val) for val in list(range(Color._max_color))]

    @staticmethod
    def __init_deltatron(d_colors):
        deltatron_colors = [((int(val, 0) & Color._red_mask) >> 16, (int(val, 0) & Color._green_mask) >> 8,
                             (int(val, 0) & Color._blue_mask)) for val in d_colors]
        Color.color_table_deltatron = ColorTable(len(deltatron_colors), "DELTATRON_COLORMAP")
        Color.color_table_deltatron.color = deltatron_colors
        Color.color_table_deltatron.fill_color()
        # self.color_table_deltatron.color = deltatron_colors + [[(val,val,val) for val in range(
        # Color._max_color-len(deltatron_colors))]]

    @staticmethod
    def __init_growth():
        Color.color_table_growth = ColorTable(3, "GROWTH_COLORMAP")
        Color.color_table_growth.color = [(val, val, val) for val in list(range(3))]

    @staticmethod
    def __color_user_overrides():
        # water color
        water_color = Color.__hex_to_rgb(Scenario.get_scen_value('water_color'))
        Color.color_table_probability.append_color(water_color)

        # self.color_table_landuse.append_color(water_color)
        # replace first spot in landuse
        Color.color_table_landuse.color[0] = water_color

        # fill landuse
        Color.color_table_landuse.fill_color()

        # seed color
        seed_color = Color.__hex_to_rgb(Scenario.get_scen_value('seed_color'))
        Color.color_table_probability.append_color(seed_color)

        # probability color count
        for color_info in Scenario.get_scen_value('probability_color'):
            if len(color_info.color) == 0:
                Color.color_table_probability.append_color((0, 0, 0))
                # self.color_table_probability.color.append((0,0,0,0))
                # print("skip")
            else:
                rgb_color = Color.__hex_to_rgb(color_info.color)
                # self.color_table_probability.color.append(hex_color)
                Color.color_table_probability.append_color(rgb_color)

        # fill probability
        Color.color_table_probability.fill_color()

        # put date color on end
        date_color = Color.__hex_to_rgb(Scenario.get_scen_value('date_color'))
        # self.color_table_probability.append_color(date_color)
        Color.color_table_probability.color[Color.color_table_probability.size - 1] = date_color

        # phase growth
        for i in range(6):
            scen_key = f"phase{i}g_growth_color"
            phase_color = Color.__hex_to_rgb(Scenario.get_scen_value(scen_key))
            # self.color_table_growth.color.append(phase_color)
            Color.color_table_growth.append_color(phase_color)

        # fill phase growth
        Color.color_table_growth.fill_color()

    @staticmethod
    def write_color_key_all(grid_ncols):
        output_dir = Scenario.get_scen_value('output_dir')
        if Scenario.get_scen_value('write_color_key_images'):
            should_log = Scenario.get_scen_value('logging') and Scenario.get_scen_value('log_writes')

            filename = output_dir + "key_" + Color.color_table_landuse.name + ".gif"
            ImageIO.write_color_key(Color.color_table_landuse, filename, should_log, grid_ncols)

            filename = output_dir + "key_" + Color.color_table_probability.name + ".gif"
            ImageIO.write_color_key(Color.color_table_probability, filename, should_log, grid_ncols)

            filename = output_dir + "key_" + Color.color_table_growth.name + ".gif"
            ImageIO.write_color_key(Color.color_table_growth, filename, should_log, grid_ncols)

            filename = output_dir + "key_" + Color.color_table_deltatron.name + ".gif"
            ImageIO.write_color_key(Color.color_table_deltatron, filename, should_log, grid_ncols)

    @staticmethod
    def log_colors():
        Logger.log("\n*********************LOGGING COLORTABLES*****************\n")
        Color.color_table_landuse.log_table()
        Color.color_table_probability.log_table()
        Color.color_table_growth.log_table()
        Color.color_table_grayscale.log_table()

    @staticmethod
    def __hex_to_rgb(hex_string):
        hex_num = int(hex_string, 0)
        red = (hex_num & Color._red_mask) >> 16
        green = (hex_num & Color._green_mask) >> 8
        blue = hex_num & Color._blue_mask
        return red, green, blue


class ColorTable:
    def __init__(self, size, name):
        self.size = size  # size might not be accuate
        self.name = name
        self.color = []  # list of thruples (RGB)

    def append_color(self, color):
        self.color.append(color)
        self.size += 1

    def fill_color(self):
        # fill in rest of landuse_colortable
        for i in range(self.size, Color._max_color):
            self.append_color((i, i, i))

    def log_table(self):
        Logger.log(f"COLORMAP: {self.name}")
        Logger.log(f"Index      R   G   B -> Hex")
        for i, rgb in enumerate(self.color):
            hex_int = rgb[0] * 256 * 256 + rgb[1] * 256 + rgb[2]
            Logger.log(f"{i:5}    {rgb[0]:3} {rgb[1]:3} {rgb[2]:3} -> {hex(hex_int)}")
        Logger.log("")
