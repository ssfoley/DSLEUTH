#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
#include "ugm_typedefs.h"

#ifdef INPUT_MODULE
  /* stuff visable only to the input module */
char input_h_sccs_id[] = "@(#)input.h	1.230	12/4/00";

  
#endif
/* #defines visable to any module including this header file*/


void
    inp_read_grow_log(char* filename,
                      stats_data_t* data,
                      int num_rows,
                      int num_cols);

void inp_slurp(char* filename,
               void* ptr,
               int count);
void
  inp_read_restart_file (int* diffusion,
                         int* breed,
                         int* spread,
                         int* slope_resistance,
                         int* road_gravity,
                         long *random_seed,
                         int *counter);

#endif
