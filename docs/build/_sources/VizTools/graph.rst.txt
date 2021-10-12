Graph Visualization
===================

The graph visualization produces average value graph images for each parameter (Breed, Spread, Road, etc.) from the data produced from the calibration phase of DSLEUTH.  Running :code:`py -m graph_avg_log -input inputFolder` in the :code:`visualization` directory will use the DSLEUTH input logs provided in :code:`inputFolder` to produce graph image files in the current directory.  Running :code:`py -m graph_avg_log -input avg_log_files/` will run the visualization tool on the sample logs provided in the :code:`visualization` directory.

.. image:: img/Og.png