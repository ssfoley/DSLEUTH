# DSLEUTH
A distributed framework for running the SLEUTH application and a parallel version of the SLEUTH using OpenMP.  SLEUTH was originally developed by Project Gigalopolis. The website where more information about the model and the original source code can be found:

http://www.ncgia.ucsb.edu/projects/gig/index.html

# Directory Structure

Input/ - where the input images reside. (Not version controlled.)

Output/ - where the generated output will be put. (Not version controlled.)

Scenarios/ - simulation configuration files. (Not version controlled.)

Whirlgif/ - an animated gif package. (Not version controlled.)

sample_data/ - a directory of input and output files from Gargi Chaudhuri as a reference.

src/ - the source code for SLEUTH

**fwk/ - the distributed framework for running subsets of the parameters on different nodes.**

**psrc/ - the parallel version of the SLEUTH source code using OpenMP.**

**doc/ - documents and results for MSE capstone.**

# Installation

Coming soon... hopefully.

# How to run

Assuming you are in the top-level directory and you have completed writing your scenario file, you can run the code from the commandline as follows:

src/grow *\<mode\>* *Scenarios/\<scenario file\>*

You will want to replace *\<mode\>* with the correct mode of the code you want to use.  The available modes are:

* *test* - a mode where the images to be used are analyzed for anamolies that would cause the code to break.
* *calibrate* - a mode where a parameter sweep over possible prediction parameters is performed to narrow down what the parameters for the final prediction phase should be.
* *prediction* - a mode where future images are predicted based on previous data and parameters.

