Installation
============


1. Follow this guide to install Python on your system: `Install Python <https://wiki.python.org/moin/BeginnersGuide/Download/>`_.
2. Follow this guide to install the Pillow library on your system: `Install Pillow <https://pillow.readthedocs.io/en/stable/installation.html>`_.
3. Download or clone the source code from the Git Repository: `Python Sleuth <https://github.com/elise-baumgartner/Python-Sleuth>`_. 

To get more information about cloning a repository from Git, reference this guide: `GitHub Guide <https://www.earthdatascience.org/workshops/intro-version-control-git/basic-git-commands/>`_. 


To Run
""""""

1. Open a command line terminal and navigate to the project repository
2. Set up a senario file and data for the simulation. For more information on how to set up, visit the :ref:`data` page)
3. Navigate into the project src directory
4. In the command line, type the following command below::

    python main.py [mode] [path-to-scenario-file]

Accepted Modes:

* test

* calibrate

* predict

For more information about the modes, visit the :ref:`structure` page