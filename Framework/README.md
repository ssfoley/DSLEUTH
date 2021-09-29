# README

## How the code is distributed

1. the DSLEUTH should be run on an cluster that shared the storage so that every node can reach the code that stored in the shared storage.

## How to run it

1. clone this respository

2. install python-hostlist-1.16 see [hostlist](https://www.nsc.liu.se/~kent/python-hostlist/)

3. configure the basic.sh script about how many nodes you want to use in this run and etc.

4. submit the basic.sh to your cluster as an job. For example: in SLURM:

 ```bash
 sbatch basic.sh
 ```
 
5. Make sure that scenario files in DSLEUTH/Scenarios and the run_settings is set to your liking

6. Make sure that the DSLEUTH/Output has a folder that matches Scenario name and that said folder is empty

7. run with command: python main.py ../src/grow [Mode] [Scenario Files]

8. After done running, there will be a top50b.log file -> created by reading the SLEUTH output control_stats.log file and compute OSM, then order

Make clean removes the top50b.log file

Future goal: make clean removes the files inside Output folder so it is easier to run again
