class Grid:

    def __init__(self):
        self.packed = False
        self.color_bits = -1
        self.bits_per_pixel = -1
        self.size_words = -1
        self.size_bytes = -1
        self.nrows = -1
        self.ncols = -1
        self.max = -1
        self.min = -1
        self.histogram = []  # full of ints
        self.filename = ""
        self.base = ""
        self.year = -1
        self.gridData = []

    def init_grid_data(self, length):
        self.gridData = [0 for i in list(range(length))]

    def fill_histogram(self):
        # fill histograms with 0s
        self.histogram = [0 for i in list(range(256))]

        # add a count for each data in grid data to the histogram
        for data in self.gridData:
            self.histogram[data] += 1

    def log_grid(self):
        basic_info = f"filename = {self.filename}\n" \
                     f"packed = {self.packed}\n" \
                     f"color_bits = {self.color_bits}\n" \
                     f"bits/pixel = {self.bits_per_pixel}\n" \
                     f"size words = {self.size_words}\n" \
                     f"size bytes = {self.size_bytes}\n" \
                     f"nrows = {self.nrows}\n" \
                     f"ncols = {self.ncols}\n" \
                     f"max = {self.max}\n" \
                     f"min = {self.min}\n" \
                     f"year = {self.year}\n"

        header = "Index Count PercentOfImage\n"
        grid_info = ""
        for i in range(256):
            val = int(self.histogram[i])
            if val > 0:
                grid_info += f"grid_ptr->histogram[{i}]={val:5d} {(100 * val) / (self.nrows * self.ncols):8.2f}%\n"

        return basic_info + header + grid_info + "\n"

    def clear_gridData(self):
        self.gridData = []

    def __str__(self):
        basic_info = "   " + str(self.packed) + " " + self.filename + " " + str(self.year) + "\n"
        grid_info = f"   cb:{self.color_bits} b/p:{self.bits_per_pixel} sw:{self.size_words} sb:{self.size_bytes} row:{self.nrows} col:{self.ncols}\n"
        return basic_info + grid_info
