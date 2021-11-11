stats_obj.o: stats_obj.c ugm_defines.h pgrid_obj.h proc_obj.h \
 igrid_obj.h globals.h grid_obj.h utilities.h ugm_typedefs.h \
 memory_obj.h scenario_obj.h ugm_macros.h stats_obj.h coeff_obj.h
timer_obj.o: timer_obj.c timer_obj.h scenario_obj.h ugm_defines.h \
 globals.h ugm_macros.h
proc_obj.o: proc_obj.c ugm_defines.h ugm_macros.h scenario_obj.h \
 globals.h coeff_obj.h proc_obj.h
transition_obj.o: transition_obj.c ugm_defines.h ugm_macros.h \
 scenario_obj.h globals.h transition_obj.h landclass_obj.h \
 memory_obj.h ugm_typedefs.h igrid_obj.h grid_obj.h utilities.h
coeff_obj.o: coeff_obj.c coeff_obj.h ugm_macros.h scenario_obj.h \
 ugm_defines.h globals.h proc_obj.h memory_obj.h ugm_typedefs.h
landclass_obj.o: landclass_obj.c ugm_defines.h ugm_macros.h \
 scenario_obj.h globals.h landclass_obj.h grid_obj.h memory_obj.h \
 ugm_typedefs.h utilities.h
deltatron.o: deltatron.c igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h landclass_obj.h random.h \
 ugm_macros.h scenario_obj.h memory_obj.h timer_obj.h proc_obj.h \
 gdif_obj.h color_obj.h pgrid_obj.h
growth.o: growth.c coeff_obj.h igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h pgrid_obj.h landclass_obj.h \
 input.h output.h growth.h spread.h random.h deltatron.h ugm_macros.h \
 scenario_obj.h proc_obj.h memory_obj.h transition_obj.h color_obj.h \
 timer_obj.h gdif_obj.h stats_obj.h
output.o: output.c coeff_obj.h igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h landclass_obj.h output.h \
 GD/gd.h color_obj.h ugm_macros.h scenario_obj.h
utilities.o: utilities.c coeff_obj.h scenario_obj.h ugm_defines.h \
 igrid_obj.h globals.h grid_obj.h utilities.h ugm_typedefs.h \
 landclass_obj.h random.h input.h ugm_macros.h memory_obj.h proc_obj.h \
 color_obj.h gdif_obj.h
spread.o: spread.c igrid_obj.h globals.h ugm_defines.h grid_obj.h \
 utilities.h ugm_typedefs.h landclass_obj.h random.h memory_obj.h \
 ugm_macros.h scenario_obj.h coeff_obj.h timer_obj.h proc_obj.h \
 stats_obj.h
random.o: random.c igrid_obj.h globals.h ugm_defines.h grid_obj.h \
 utilities.h ugm_typedefs.h landclass_obj.h random.h ugm_macros.h \
 scenario_obj.h
scenario_obj.o: scenario_obj.c scenario_obj.h ugm_defines.h \
 coeff_obj.h utilities.h grid_obj.h globals.h proc_obj.h ugm_macros.h \
 wgrid_obj.h
igrid_obj.o: igrid_obj.c igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h scenario_obj.h memory_obj.h \
 gdif_obj.h color_obj.h output.h coeff_obj.h landclass_obj.h \
 ugm_macros.h proc_obj.h
gdif_obj.o: gdif_obj.c GD/gd.h GD/gdfonts.h GD/gdfontg.h globals.h \
 ugm_defines.h gdif_obj.h color_obj.h memory_obj.h ugm_typedefs.h \
 igrid_obj.h grid_obj.h utilities.h timer_obj.h scenario_obj.h \
 ugm_macros.h
pgrid_obj.o: pgrid_obj.c globals.h ugm_defines.h grid_obj.h \
 memory_obj.h ugm_typedefs.h scenario_obj.h ugm_macros.h
memory_obj.o: memory_obj.c globals.h ugm_defines.h igrid_obj.h \
 grid_obj.h utilities.h ugm_typedefs.h pgrid_obj.h wgrid_obj.h \
 scenario_obj.h ugm_macros.h memory_obj.h
wgrid_obj.o: wgrid_obj.c memory_obj.h globals.h ugm_defines.h \
 ugm_typedefs.h ugm_macros.h scenario_obj.h
grid_obj.o: grid_obj.c ugm_defines.h ugm_macros.h scenario_obj.h \
 globals.h grid_obj.h memory_obj.h ugm_typedefs.h
color_obj.o: color_obj.c scenario_obj.h ugm_defines.h color_obj.h \
 landclass_obj.h memory_obj.h globals.h ugm_typedefs.h ugm_macros.h \
 gdif_obj.h
driver.o: driver.c igrid_obj.h globals.h ugm_defines.h grid_obj.h \
 utilities.h ugm_typedefs.h pgrid_obj.h landclass_obj.h color_obj.h \
 coeff_obj.h memory_obj.h scenario_obj.h transition_obj.h ugm_macros.h \
 proc_obj.h gdif_obj.h growth.h stats_obj.h timer_obj.h
input.o: input.c coeff_obj.h igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h landclass_obj.h output.h \
 input.h ugm_macros.h scenario_obj.h
main.o: main.c coeff_obj.h igrid_obj.h globals.h ugm_defines.h \
 grid_obj.h utilities.h ugm_typedefs.h landclass_obj.h output.h \
 random.h driver.h input.h scenario_obj.h proc_obj.h timer_obj.h \
 pgrid_obj.h color_obj.h memory_obj.h stats_obj.h transition_obj.h \
 ugm_macros.h
