Map Visualization
=================

The map visualization produces individual change maps and an animation from the data produced from the prediction phase of DSLEUTH.  Running :code:`py -m visualization -input inputFolder` in the :code:`visualization` directory will use the DSLEUTH input data provided in :code:`inputFolder` to produce change maps in a new :code:`change_maps` directory and an animated .gif in a new :code:`animation` directory.  Running :code:`py -m visualization -input dsleuth_wb100pred_output` will run the visualization tool on the sample data provided in the :code:`visualization` directory.

.. image:: img/animation.gif

