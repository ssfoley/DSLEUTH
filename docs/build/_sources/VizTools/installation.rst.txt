Installing Visualization Tools
==============================

Dependencies
------------

- To run the tools, you will need |py3 link| on your machine.

Note: It is highly recommended that you use a virtual environment while working with this project.  More on Python virtual environments |py3 venv link|.

- |gdal link| and |rasterio link| will need to be installed before the other requirements.  Find the latest versions for your computer using the above links.  For example, if you were using Python 3.9 64-bit, you would use :code:`GDAL-3.3.3-cp39-cp39-win_amd64.whl` and :code:`rasterio-1.2.10-cp39-cp39-win_amd64.whl`.
- After you have downloaded the .whl files, you can install them into your environment using the following commands in the directory that contains the files: ::

	pip install -U pip
	pip install {GDAL .whl FILE NAME}
	pip install {RASTERIO .whl FILE NAME}

Installation
------------

- Retrieve the latest version of the :code:`DSLEUTH/src/VizTools/src` directory from the :code:`master` branch of DSLEUTH.
- In the :code:`src` directory, run :code:`py -m pip install -r requirements.txt`.  This will install the rest of the necessary packages needed to run the tools.

Once the requirements have been installed, you can run the visualization tools directly from the command line.  See :doc:`map` and :doc:`graph` for specific instructions.

.. |py3 venv link| raw:: html

	<a href="https://docs.python.org/3/library/venv.html" target="_blank">here</a>

.. |py3 link| raw:: html

	<a href="https://www.python.org/downloads/" target="_blank">Python 3</a>

.. |gdal link| raw:: html

	<a href="https://www.lfd.uci.edu/~gohlke/pythonlibs/#gdal" target="_blank">GDAL</a>

.. |rasterio link| raw:: html

	<a href="https://www.lfd.uci.edu/~gohlke/pythonlibs/#rasterio" target="_blank">rasterio</a>