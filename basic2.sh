#!/bin/bash

#SBATCH -n 1             # Ensure that all cores are on one machine
#SBATCH -c 4
#SBATCH -J TopParallel
#SBATCH -t 6-16:00       # Runtime in D-HH:MM
#SBATCH -o hostname_DSLEUTH.out  # File to which STDOUT and STDERR will be written

#run the application:
srun time ./src/grow calibrate Scenarios/wb100_calibrateCopy