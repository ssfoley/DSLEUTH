# Experiments

## FLUX

The key parameters in scenario file

    MONTE_CARLO_ITERATIONS = 4

    CALIBRATION_DIFFUSION_START=0
    CALIBRATION_DIFFUSION_STEP=25
    CALIBRATION_DIFFUSION_STOP=100

    CALIBRATION_BREED_START=0
    CALIBRATION_BREED_STEP=25
    CALIBRATION_BREED_STOP=100

    CALIBRATION_SPREAD_START=0
    CALIBRATION_SPREAD_STEP=25
    CALIBRATION_SPREAD_STOP=100

    CALIBRATION_SLOPE_START=0
    CALIBRATION_SLOPE_STEP=25
    CALIBRATION_SLOPE_STOP=100

    CALIBRATION_ROAD_START=0
    CALIBRATION_ROAD_STEP=25
    CALIBRATION_ROAD_STOP=100


### using dataset demo200

SLEUTH performance

Purpose: To make sure the three computation nodes' computation ability is nearly same

1. run SLEUTH on flux2.

1. run SLEUTH on flux3.

1. run SLEUTH on flux4.

DSLEUTH performance

Purpose: To figure out breaking which part would be the best

1. run DSLEUTH on flux2, flux3, flux4 spliting the DIFFUSION search space.

1. run DSLEUTH on flux2, flux3, flux4 spliting the BREED search space.

1. run DSLEUTH on flux2, flux3, flux4 spliting the SPREAD search space.

1. run DSLEUTH on flux2, flux3, flux4 spliting the SLOPE search space.

1. run DSLEUTH on flux2, flux3, flux4 spliting the ROAD search space.

### using dataset wb100

SLEUTH performance

1. run SLEUTH on flux2.

DSLEUTH performance

1. run DSLEUTH on flux2, flux3, flux4 spliting the search spaces into reasonable number(25) of smaller search spaces which is greater than the number of files given by user.