# README

## How the code is distributed

1. the DSLEUTH should be run on an cluster that shared the storage so that every node can reach the code that stored in the shared storage.

## How to run it

1. clone this respository

1. install python-hostlist-1.16 see [hostlist](https://www.nsc.liu.se/~kent/python-hostlist/)

1. configure the basic.sh script about how many nodes you want to use in this run and etc.

1. submit the basic.sh to your cluster as an job. For example: in SLURM:
 ```bash
 sbatch basic.sh
 ```