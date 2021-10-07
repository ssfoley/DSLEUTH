#!/bin/bash

#SBATCH -N 3             # Ensure that all cores are on one machine
#SBATCH -t 2-00:00       # Runtime in D-HH:MM
#SBATCH -o hostname_SLEUTH_wb100.out  # File to which STDOUT and STDERR will be written
#SBATCH --open-mode=append # Append to the output file if it exists

date

echo "-------------------------"

echo "Use srun to show the names of all the nodes in the allocation"
echo "------------------------"
srun hostname

echo " Python version"
python --version
echo "-------------------------"
#python launch.py test1 test2
#python main.py src/grow calibrate Scenarios/scenario.demo200_calibrate
src/grow calibrate Scenarios/scenario.wb100_calibrate
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/1
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/2
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/3
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/4
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/5
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/6
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/7
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/8
#src/grow calibrate Scenarios/scenario.demo200_calibrate_steps/9
echo "-------------------------"
date

echo $SLURM_JOB_ID
echo $SLURM_JOB_NODELIST

#qnmap -p 6379 localhost

exit 0
