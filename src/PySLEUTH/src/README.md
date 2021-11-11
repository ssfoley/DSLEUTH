## Python SLEUTH

SLEUTH is an open-source cellular automata-based land use change simulation model. 
The purpose of the SLEUTH model is to simulate future land use and urban planning scenarios. 
However, the current model of SLEUTH was originally written in the language C and can only be 
run in environments that have a C-compiler. Because of the complexity of several components, it 
is difficult for new users to get started using the model. 

## Motivation
The purpose of this project is to adapt the SLEUTH model to improve ease of use and 
portability to increase the usability of the model for land use or urban modeling 
simulation. This is done by translating the SLEUTH model into Python. For more information on the project, feel free to browse [the documentation](https://elise-baumgartner.github.io/Python-Sleuth/build/html/index.html) for this project.


## Installation
1. Ensure Python is installed on your system. To check if Python is installed and/or to install Python, follow [this guide](https://wiki.python.org/moin/BeginnersGuide/Download/).
2. To process and produce images, Python SLEUTH uses the Pillow library. To install Pillow, follow [this guide](https://pillow.readthedocs.io/en/stable/installation.html).
3. Clone or Download this Repository


## How to use?
1. Open a command line terminal and navigate to the project repository
2. Set up the Scenario file and data for the simulation. Sample data is provided in the repo.
3. Navigate into the project src directory 
4. Use the following command to run the project
```python3 main.py [mode] [path-to-scenario-file]```

Accepted Modes: 
* test
* calibrate
* predict

For more information on how the project runs, visit the Python SLEUTH's [documentation](https://elise-baumgartner.github.io/Python-Sleuth/build/html/index.html)



## Credits
[Project Gigalopolis:](http://www.ncgia.ucsb.edu/projects/gig/index.html) The open source C version of SLEUTH
