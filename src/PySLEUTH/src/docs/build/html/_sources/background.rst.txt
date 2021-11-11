Background
==========   
 
The process of urbanization occurs mainly due to population growth, rural migration to cities, and industrialization, 
and often produces nearly irreversible changes to the land around it. It increases land used by people, which affects 
the biodiversity, ecosystems, urban climate, and natural areas.[1] Because of these affects, many academics and planners 
use models to mimic the growth of urbanization and forecast land use change. Urban growth models, also known as land use 
change models, are a type of spatial simulation model and are currently used to simulate and predict this urbanization.[2] 
A popular urban growth model used by domain scientists today is a cellular automata-based sequential land use change model 
called SLEUTH. The aim of this project is to ease the use and utility of SLEUTH for domain scientists and users as well as 
improve portability.

SLEUTH is an open-source cellular automata-based land use change simulation model that is a combination of the Land Cover 
Deltatron Model and the Urban Growth Model. [3] Its name, SLEUTH, is an acronym for the data layers that the model uses to
predict growth (Slope, Land use, Exclusion, Urban extent, Transportation, and Hill shade). The purpose of the SLEUTH model 
is to simulate future land use and urban planning scenarios. The SLEUTH model uses historical geographical data to calculate 
and predict future urban growth.[2] Its popularity stems from some of its features, including minimum data requirements, 
relatively simple modeling workflow, and ease of adaptation.

Despite its many advantages, SLEUTH has disadvantages as well. The computational complexity of the model is large, especially 
with large data sets due to its brute-force calibration process. The model also needs a C compiler and runs on the command line, 
making it difficult to run on different operating systems and computer environments. Additionally, the data input and output of 
SLEUTH does not match current data sets available, so it requires an extra step to convert data into and out of the required format.
The purpose of this project is to adapt the SLEUTH model to improve ease of use and portability to increase the usability of the model 
for land use or urban modeling simulation.


References
""""""""""

1. M. Eslahi. “Urban Growth Simulations in order to Represent the Impacts of Constructions and Environmental Constraints on Urban Sprawl,” Ph.D. dissertation, Université Paris-Est, 2019.
2. G. Chaudhuri and S. Foley, "Dsleuth: A Distributed Version of Sleuth Urban Growth Model," 2019 Spring Simulation Conference (SpringSim), 2019, pp. 1-11, doi: 10.23919/SpringSim.2019.8732879.
3. G. Chaudhuri and K. Clarke, “The Sleuth Land Use Change Model: A Review”, International Journal Of Environmental Resource Research, vol. 1, pp. 88-105, 2013
