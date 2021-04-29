Documentation
=============

About
-----

This DSLEUTH documentation was created using `Sphinx <https://www.sphinx-doc.org/en/master/index.html>`_ and hosted using GitHub Pages.  The source files for DSLEUTH Documentation are available in the `DSLEUTH Github Project <https://github.com/ssfoley/DSLEUTH>`_.

Editing/Publishing Documentation
--------------------------------

- Make sure you have the latest version of `Python 3 <https://www.python.org/downloads/>`_ on your machine.
- Install Sphinx on your machine using the instructions `here <https://www.sphinx-doc.org/en/master/usage/installation.html>`__.

Note: It is highly recommended that you use a virtual environment while working with this project.  More on Python virtual environments `here <https://docs.python.org/3/library/venv.html>`__.

- Once Sphinx is installed, retrieve the latest version of the :code:`docs/source` and :code:`docs/build` directories from the master branch of DSLEUTH.
- After the source directory is edited, run :code:`sphinx-build -b html source build` in the directory containing :code:`source` and :code:`build`.  More information can be found on Sphinx builds `here <https://www.sphinx-doc.org/en/master/usage/quickstart.html>`__.
- Once satisfactory changes have been made, push the new :code:`source` and :code:`build` directories to DSLEUTH.

