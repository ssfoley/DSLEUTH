DSLEUTH
=======

Installing DSLEUTH
------------------

The source code for DSLEUTH is available on |dsleuth project link|.

Running DSLEUTH
---------------

To run the framework to distribute your code, run with a batchscript on a SLURM cluster, or in localhost mode to rely on the operating system to manage the separate processes. To set the mode and the number of nodes, write the run_settings file with the following::

	localhost num-nodes

or ::

	SLURM num-nodes

Then to run do::

	python Framework/main.py src/grow mode scenario-file

**OLD RUN INSTRUCTIONS**

Assuming you are in the top-level directory and you have completed writing your scenario file, you can run the code from the commandline as follows::

	src/grow *<mode> Scenarios/<scenario file>*

You will want to replace *<mode>* with the correct mode of the code you want to use. The available modes are:

* *test* - a mode where the images to be used are analyzed for anamolies that would cause the code to break.
* *calibrate* - a mode where a parameter sweep over possible prediction parameters is performed to narrow down what the parameters for the final prediction phase should be.
* *prediction* - a mode where future images are predicted based on previous data and parameters.

Directory Structure
-------------------

Input/ - where the input images reside. (Not version controlled.)

Output/ - where the generated output will be put. (Not version controlled.)

Scenarios/ - simulation configuration files. (Not version controlled.)

Whirlgif/ - an animated gif package. (Not version controlled.)

sample_data/ - a directory of input and output files from Gargi Chaudhuri as a reference.

src/ - the source code for SLEUTH

**fwk/ - the distributed framework for running subsets of the parameters on different nodes.**

**psrc/ - the parallel version of the SLEUTH source code using OpenMP.**

**doc/ - documents and results for MSE capstone.**

.. |dsleuth project link| raw:: html

	<a href="https://github.com/ssfoley/DSLEUTH" target="_blank">GitHub</a>