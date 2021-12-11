Map Visualization
=================

The map visualization produces individual change maps and an animation from the data produced from the prediction phase of DSLEUTH.  

Input Files for Map Visualization
---------------------------------

The folder :code:`dsleuth_wb100pred_output` contains the output data from DSLEUTH needed to run the map visualization.  This includes a log of the numerical data and individual maps depicting urban growth for each year.

Running Map Visualization
-------------------------

Running :code:`py -m visualization -input inputFolder` in the :code:`src` directory will use the DSLEUTH input data provided in :code:`inputFolder` to produce change maps in a new :code:`change_maps` directory and an animated .gif in a new :code:`animation` directory.  Running :code:`py -m visualization -input dsleuth_wb100pred_output` will run the visualization tool on the sample data provided in the :code:`dsleuth_wb100pred_output` directory.

.. image:: img/animation.gif
  :width: 400