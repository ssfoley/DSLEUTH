#!/bin/bash

#SBATCH -n 1             # Ensure that all cores are on one machine
#SBATCH -c 8
#SBATCH -J 8threads
#SBATCH --mail-user=yuan.zhihao@uwlax.edu
#SBATCH -t 6-16:00       # Runtime in D-HH:MM
#SBATCH -o hostname_DSLEUTH8Threads.out  # File to which STDOUT and STDERR will be written

#run the application:
srun time ./grow calibrate Scenarios/wb100_calibrate