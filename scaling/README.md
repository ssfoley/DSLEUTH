Scaling Study
=============

This directory will contain the scaling study results of the original sequential version of SLEUTH, the distributed version (DSLEUTH), the parallel version in ../psrc, and the combination of the distributed and parallel versions.  This experiements will be done with the demo200 and wb100 datasets.

List of experiments:

| Mode | Nodes | Threads | Tasks | Machine | Dataset | Status | Runtime | Speedup | efficiency | Scenario | Input | Output |
|:----:|:-----:|:-------:|:-----:|---------|---------|--------|--------:|--------:|-----------:|----------|-------|--------|
| S    | 1     | 1       | 1     | flux    | demo200 |        |         |         |            |          |       |        | 
| P    | 1     | 1       | 1     | flux    | demo200 |        |         |         |            |          |       |        | 
| P    | 1     | 4       | 5     | flux    | demo200 |        |         |         |            |  breed   |       |        | 
| P    | 1     | 4       | 5     | flux    | demo200 |        |         |         |            |  slope   |       |        | 
| P    | 1     | 4       | 5     | flux    | demo200 |        |         |         |            |  road    |       |        | 
| P    | 1     | 4       | 5     | flux    | demo200 |        |         |         |            |  diff    |       |        | 
| P    | 1     | 4       | 5     | flux    | demo200 |        |         |         |            |  spread  |       |        | 
| P    | 1     | 8       | 25    | flux    | demo200 |        |         |         |            |  b & s   |       |        | 
| P    | 1     | 16      | 25    | flux    | demo200 |        |         |         |            |          |       |        | 
...




<orginal performance>
 - sequential, 1 node, 1 thread, flux, demo200, not done, runtime, input dir, output dir
 - sequential, 1 node, 1 thread, flux, wb100  , not done, runtime, input dir, output dir
 - sequential, 1 node, 1 thread, grrc, demo200, not done, runtime, input dir, output dir
 - sequential, 1 node, 1 thread, grrc, wb100  , not done, runtime, input dir, output dir

<measure overhead of running with distributed framework>
 - distributed, 1 node, 1 thread, flux, demo200, not done, runtime, speedup, efficiency, input dir, output dir
 - distributed, 1 node, 1 thread, flux, wb100  , not done, runtime, speedup, efficiency, input dir, output dir
 - distributed, 1 node, 1 thread, grrc, demo200, not done, runtime, speedup, efficiency, input dir, output dir
 - distributed, 1 node, 1 thread, grrc, wb100  , not done, runtime, speedup, efficiency, input dir, output dir