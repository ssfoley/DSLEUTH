Documentation
=============

About
-----

Author(s): Heather Miller

This DSLEUTH documentation was created using |sphinx link| and hosted using GitHub Pages.  The source files for DSLEUTH Documentation are available in the |dsleuth project link|.

Editing/Publishing Documentation
--------------------------------

- Make sure you have the latest version of |py3 link| on your machine.
- Install Sphinx on your machine using the instructions |sphinx inst link|.

Note: It is highly recommended that you use a virtual environment while working with this project.  More on Python virtual environments |py3 venv link|.

- Once Sphinx is installed, retrieve the latest version of the :code:`docs/source` and :code:`docs/build` directories from the :code:`master` branch of DSLEUTH.
- After the :code:`source` directory is edited, run :code:`sphinx-build -b html source build` in the directory containing :code:`source` and :code:`build`.  More information can be found on Sphinx builds |sphinx build link|.
- Once satisfactory changes have been made, push the new :code:`source` and :code:`build` directories to DSLEUTH.

.. |sphinx link| raw:: html

	<a href="https://www.sphinx-doc.org/en/master/index.html" target="_blank">Sphinx</a>

.. |dsleuth project link| raw:: html

	<a href="https://github.com/ssfoley/DSLEUTH" target="_blank">DSLEUTH Github Project</a>

.. |py3 link| raw:: html

	<a href="https://www.python.org/downloads/" target="_blank">Python 3</a>

.. |sphinx inst link| raw:: html

	<a href="https://www.sphinx-doc.org/en/master/usage/installation.html" target="_blank">here</a>

.. |py3 venv link| raw:: html

	<a href="https://docs.python.org/3/library/venv.html" target="_blank">here</a>

.. |sphinx build link| raw:: html

	<a href="https://www.sphinx-doc.org/en/master/usage/quickstart.html" target="_blank">here</a>