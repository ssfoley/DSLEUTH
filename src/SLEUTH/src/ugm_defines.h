#ifndef UGM_DEFINES
#include <math.h>

#define UGM_DEFINES

#define CALIBRATING 1
#define PREDICTING 2
#define TESTING 3
#define PIXEL long
#define GRID_P PIXEL*
#define BOOLEAN int
#define MAX_FILENAME_LEN 150
#define RED_MASK   0XFF0000
#define GREEN_MASK 0X00FF00
#define BLUE_MASK  0X0000FF
#define TRUE  1
#define FALSE 0
#define MIN_SLOPE_RESISTANCE_VALUE 0.01
#define MAX_SLOPE_RESISTANCE_VALUE 100.0
#define MIN_ROAD_GRAVITY_VALUE 0.01
#define MAX_ROAD_GRAVITY_VALUE 100.0
#define MIN_SPREAD_VALUE 0.01
#define MAX_SPREAD_VALUE 100.0
#define MIN_DIFFUSION_VALUE 0.01
#define MAX_DIFFUSION_VALUE 100.0
#define MIN_BREED_VALUE 0.01
#define MAX_BREED_VALUE 100.0
#define MAX_ROAD_VALUE 100
#define DIGITS_IN_YEAR 4
#define LOW 0
#define MED 1
#define HIGH 2
#define MAX_FILENAME_LEN 150
#define CALL_STACK_SIZE 100
#define MAX_PROBABILITY_COLORS 100
#define PI M_PI
#define BOOLEAN int
#define RANDOM_SEED_TYPE long
#define CLASS_SLP_TYPE double
#define FTRANS_TYPE double
#define COEFF_TYPE double
#define BYTES_PER_PIXEL sizeof(PIXEL)
#define CLASS_SLP_TYPE double
#define REGION_SIZE 30
#define DELTA_PHASE2_SENSITIVITY 1.0
#define SELF_MOD_SENSITIVITY 20.0
#define MIN_YEARS_BETWEEN_TRANSITIONS 5
#define MIN_NGHBR_TO_SPREAD 2
#define MAX_URBAN_YEARS 15
#define MAX_ROAD_YEARS 15
#define MAX_LANDUSE_YEARS 2
#define RESTART_FILE "restart_file.data"
#define BYTES_PER_WORD sizeof(PIXEL)
#ifdef PACKING
  #define BYTES_PER_PIXEL_PACKED 1
#endif
#define DIGITS_IN_YEAR 4
#define PACKED 1
#define UNPACKED 0
#define NOTPACKED 0
#define PHASE0G 3
#define PHASE1G 4
#define PHASE2G 5
#define PHASE3G 6
#define PHASE4G 7
#define PHASE5G 8
#define LT 0
#define LE 1
#define EQ 2
#define NE 3
#define GE 4
#define GT 5





#endif
