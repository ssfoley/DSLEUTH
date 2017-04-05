#!/bin/bash

#SBATCH -N 1             # Ensure that all cores are on one machine
#SBATCH -J sequ
#SBATCH --mail-user=yuan.zhihao@uwlax.edu
#SBATCH -t 9-16:00       # Runtime in D-HH:MM
#SBATCH -o hostname_DSLEUTH.out  # File to which STDOUT and STDERR will be written

#run the application:
srun time ./src/grow calibrate Scenarios/wb100_calibrate