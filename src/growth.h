#ifndef GROWTH_H
#define GROWTH_H

#ifdef GROWTH_MODULE
  /* stuff visable only to the growth module */
char growth_h_sccs_id[] = "@(#)growth.h	1.245	12/4/00";




  
#endif


/* #defines visable to any module including this header file*/

/*
 *
 * FUNCTION PROTOTYPES
 *
 */
void grw_grow(GRID_P z_ptr, GRID_P land1_ptr);
void Growth (int stop_date,
             Classes* landuse_classes,
             Classes* class_indices,
             int* new_indices,
             int urban_code,
             int num_road_pixels,
             int num_excld_pixels,
             float percent_road,
             coeff_val_info* current_coefficient,
             igrid_info* input_grid,
             GRID_P z_ptr,
             GRID_P land1_ptr,
             GRID_P land2_ptr,
             GRID_P scratch_gif1,
             GRID_P scratch_gif2,
             GRID_P scratch_gif3,
             GRID_P scratch_gif4,
             GRID_P scratch_gif5,
             GRID_P scratch_gif6,
             GRID_P scratch_gif7,
             GRID_P scratch_gif8,
             double *class_slope,
             double *ftransition,
             int num_monte_carlo);


#endif
