DSLEUTH
=======

Installing DSLEUTH
------------------

The source code for DSLEUTH is available on |dsleuth project link|.

Running DSLEUTH
---------------

Settings for DSLEUTH are configured within a :code:`config.ini` file.  An example of this file can be found within the :code:`sample_data/` directory, or alternatively, one can be created by running the :code:`createConfig.py` script, found in the DSLEUTH source code folder.  The parameters in the configuration file are as follows:

- :code:`sleuthpath`: The path to the SLEUTH executable, relative to the directory in which you are running DSLEUTH
- :code:`sleuthmode`: The mode to manage the processes.  Use "SMP" to rely on the operating system to manage the separate processes, or "SLURM" to run with a batchscript on a SLURM cluster.
- :code:`phase`: The phase in which DSLEUTH is being run for.  Use "calibrate" to run DSLEUTH for the calibration phase.
- :code:`scenariopath`: The path to the scenario file, relative to the directory in which you are running DSLEUTH
- :code:`processors`: The number of processors to use during the run of DSLEUTH.
- :code:`isintestmode`: True/False.  If true, DSLEUTH will not run past the initial setup.
- :code:`isindebugmode`: True/False.  If true, DSLEUTH will print extra information to the console while running.

To run DSLEUTH, place the :code:`config.ini` file in the directory in which you are running DSLEUTH and run::

	python3 <path to DSLEUTH script>

Both the example configuration file and the file created with the :code:`createConfig.py` script are created with path parameters relative to the root project directory.  To run DSLEUTH with these settings, place the required inputs in their directories as specified by the configuration, place :code:`config.ini` in the root project directory, and run::

	python3 src/DSLEUTH/src/dsleuth.py

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