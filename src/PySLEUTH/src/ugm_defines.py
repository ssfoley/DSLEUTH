import sys


class UGMDefines:
    CALIBRATING = 1
    PREDICTING = 2
    TESTING = 3
    # PIXEL -> long
    # GRID_P -> PIXEL*
    # BOOLEAN -> int
    MAX_FILENAME_LEN = 150
    RED_MASK = 0xFF0000
    GREEN_MASK = 0X00FF00
    BLUE_MASK = 0X0000FF
    MIN_SLOPE_RESISTANCE_VALUE = 0.01
    MAX_SLOPE_RESISTANCE_VALUE = 100.0
    MIN_ROAD_GRAVITY_VALUE = 0.01
    MAX_ROAD_GRAVITY_VALUE = 100.0
    MIN_SPREAD_VALUE = 0.01
    MAX_SPREAD_VALUE = 100.0
    MIN_DIFFUSION_VALUE = 0.01
    MAX_DIFFUSION_VALUE = 100.0
    MIN_BREED_VALUE = 0.01
    MAX_BREED_VALUE = 100.0
    MAX_ROAD_VALUE = 100
    DIGITS_IN_YEAR = 4
    LOW = 0
    MED = 1
    HIGH = 2
    CALL_STACK_SIZE = 100
    MAX_PROBABILITY_COLORS = 100
    # RANDOM_SEED_TYPE -> long
    # PI -> use Math.Pi
    # CLASS_SLP_TYPE -> double
    # FTRANS_TYPE -> double
    # COEFF_TYPE -> double
    # BYTES_PER_PIXEL -> sizeof(PIXEL)
    REGION_SIZE = 30
    DELTA_PHASE2_SENSITIVITY = 1.0
    SELF_MOD_SENSITIVITY = 20.0
    MIN_YEARS_BETWEEN_TRANSITIONS = 5
    MIN_NGHBR_TO_SPREAD = 2
    MAX_URBAN_YEARS = 15
    MAX_ROAD_YEARS = 15
    MAX_LANDUSE_YEARS = 2
    RESTART_FILE = "restart_file.data"
    BYTES_PER_WORD = sys.getsizeof(int())
    BYTES_PER_PIXEL_PACKED = 1
    PACKED = 1
    UNPACKED = 0
    NOTPACKED = 0
    PHASE0G = 3
    PHASE1G = 4
    PHASE2G = 5
    PHASE3G = 6
    PHASE4G = 7
    PHASE5G = 8
    LT = 0
    LE = 1
    EQ = 2
    NE = 3
    GE = 4
    GT = 5
    SEED_COLOR_INDEX = 1
    DATE_COLOR_INDEX = 255

    @staticmethod
    def round_bytes_to_word_boundary(byte):
        return ((byte + UGMDefines.BYTES_PER_WORD - 1) / UGMDefines.BYTES_PER_WORD) * (
            UGMDefines.BYTES_PER_WORD)
