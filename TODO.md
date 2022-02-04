# To Do Items for the project

Notes 2/4/22
 - Heather will see what she can do to run DSLEUTH locally and write down all her questions
 - Next meeting, we will both look at them and fix all the things we can
 - Next steps:
    - figure out other target machines -- VMs possibly on Euca
    - look into getting KSLEUTH going too
    - XSEDE resources for both

Notes from 1/28/22
 - last semester: finished getting viz tools tested, documented, and working
 - to do for 2/4/22: go back and make sure we can get the viz tools running to verify the docs (@heather)
 - to do for 2/4/22: make a list of tasks for the semester and continue repo cleanup work (@ssfoley)




## Clean up repo

 - [x] merge Elise's work on DSLEUTH
 - [ ] test Elise's work on DSLEUTH
 - [ ] merge Annika's viz work
 - [ ] test Annika's viz work
 - [x] make dev branch
 - [x] make dev branch the default branch
 - [ ] setup directory structure for different versions


## New Directory Structure
- top level:
  - README
  - license
  - authors
- docs
  - where the sphinx docs go at the top level, specific docs for each subproject are in those directories
- src
  - SLEUTH - Readme and sample run scripts, docs directory, src directory
  - DSLEUTH - Readme and sample run scripts, docs directory, src directory (this will contain the files from current Framework)
  - KSLEUTH - Readme and sample run scripts, docs directory, src directory (this will contain the files from current Framework)
  - PySLEUTH - Readme and sample run scripts, docs directory, src directory
  - VizTools - Readme and sample run scripts, docs directory, src directory
- all the other directories from the original will still be here

## Define branching and development process
Probably a workflow with a stable master, a development branch, and feature branches.

 - all development should be done using feature branches off of the dev branch
 - a pull request should be issued when the feature is complete
 - at least one other team member, preferably Dr. Foley, will review and approve the merge
 - when a collection of features are ready, they will be merged into master, our release branch for users of DSLEUTH
