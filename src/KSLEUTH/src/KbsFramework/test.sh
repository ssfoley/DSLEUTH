#!/usr/bin/env bash
# this script can only run through docker,
# if wants to run it without docker,
# modify the ROOT_FILE to the corresponding absolute path
starttime=$(date +'%Y-%m-%d %H:%M:%S')
ROOT_FILE="/home/cent/KSLEUTH"
MODE="calibrate"
SCENARIO_FILE="${ROOT_FILE}/Scenarios/scenario.demo200-test"

# clean all the output file to make sure the 'make' command can run smooth
cd ${ROOT_FILE}/src/GD
make clean
make

cd ${ROOT_FILE}/src
make clean
make
# the input formate like this, do not use ',' to divide the input parameters
${ROOT_FILE}/src/grow calibrate ${SCENARIO_FILE}
endtime=$(date +'%Y-%m-%d %H:%M:%S')
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s);
echo "runing time： "$((end_seconds-start_seconds))"s"