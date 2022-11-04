from grid import Grid


class PGrid:
    z = None
    deltatron = None
    delta = None
    land1 = None
    land2 = None
    cumulate = None
    count = 6

    @staticmethod
    def init(num_pixels):
        PGrid.z = Grid()
        PGrid.z.init_grid_data(num_pixels)

        PGrid.deltatron = Grid()
        PGrid.deltatron.init_grid_data(num_pixels)

        PGrid.delta = Grid()
        PGrid.delta.init_grid_data(num_pixels)

        PGrid.land1 = Grid()
        PGrid.land1.init_grid_data(num_pixels)

        PGrid.land2 = Grid()
        PGrid.land2.init_grid_data(num_pixels)

        PGrid.cumulate = Grid()
        PGrid.cumulate.init_grid_data(num_pixels)

    @staticmethod
    def get_z():
        return PGrid.z

    @staticmethod
    def get_deltatron():
        return PGrid.deltatron

    @staticmethod
    def get_delta():
        return PGrid.delta

    @staticmethod
    def get_land1():
        return PGrid.land1

    @staticmethod
    def get_land2():
        return PGrid.land2

    @staticmethod
    def get_cumulate():
        return PGrid.cumulate

