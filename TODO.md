# To Do Items for the project

## Clean up repo

 - [x] merge Elise's work on DSLEUTH
 - [ ] test Elise's work on DSLEUTH
 - [ ] merge Annika's viz work
 - [ ] test Annika's viz work
 - [ ] make dev branch
 - [ ] make dev branch the default branch
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
