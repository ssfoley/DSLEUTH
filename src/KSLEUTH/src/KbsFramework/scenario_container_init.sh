#!/usr/bin/env bash
# this script can only run through docker,
# if wants to run it without docker,
# modify the ROOT_FILE to the corresponding absolute path

ROOT_FILE="/SLEUTH"
MODE="calibrate"
SCENARIO_FILE="${ROOT_FILE}/KbsFramework/splitScenario/$1"

# clean all the output file to make sure the 'make' command can run smooth
cd ${ROOT_FILE}/src/GD
make clean
make

cd ${ROOT_FILE}/src
make clean
make
# the input formate like this, do not use ',' to divide the input parameters
${ROOT_FILE}/src/grow "${MODE}" "${SCENARIO_FILE}"



