#ifndef UTILITIES_H
#define UTILITIES_H
#include "ugm_defines.h"
#include "grid_obj.h"
 
#ifdef UTILITIES_MODULE
  /* stuff visable only to the utilities module */
char utilities_h_sccs_id[] = "@(#)utilities.h	1.258	12/4/00";

#endif


/*
 *
 * FUNCTION PROTOTYPES
 *
 */
void util_AllCAPS(char* str_ptr);
void util_merge_background (GRID_P foreground_gif,    /* IN     */
                            GRID_P background_gif,    /* IN     */
                            GRID_P merged_gif);       /* OUT    */
void
  util_copy_grid (GRID_P source,  /* IN     */
                 GRID_P target);  /* OUT    */


void
  util_condition_gif (int num_pixels,  /* IN     */
                      GRID_P source,  /* IN     */
                      int option,      /* IN     */
                      int cmp_value,   /* IN     */
                      GRID_P target,  /* OUT    */
                      int set_value);  /* IN     */

int
  util_count_pixels (int num_pixels,  /* IN     */
                     GRID_P pixels,  /* IN     */
                     int option,      /* IN     */
                     int value);      /* IN     */

void util_get_neighbor(int i_in,      /* IN     */
                       int j_in,      /* IN     */
                       int* i_out,    /* OUT    */
                       int* j_out);   /* OUT    */



int
  util_trim (char s[]);   /* IN/OUT */

int util_count_neighbors(GRID_P grid,   /* IN     */
                         int i,          /* IN     */
                         int j,          /* IN     */
                         int option,     /* IN     */
                         PIXEL value);   /* IN     */

void util_get_next_neighbor(int i_in,      /* IN     */
                            int j_in,      /* IN     */
                            int* i_out,    /* OUT    */
                            int* j_out,    /* OUT    */
                            int index);    /* IN     */
int
  util_img_intersection (int num_pixels,                         /* IN     */
                         GRID_P ptr1,                           /* IN     */
                         GRID_P ptr2);                          /* IN     */
void
util_map_gridpts_2_index(GRID_P in,
                         GRID_P out,
                         int* lower_bound,
                         int* upper_bound,
                         int* index,
                         int count);
void
util_overlay(GRID_P layer0,
             GRID_P layer1,
             GRID_P out);

void
  util_WriteZProbGrid (GRID_P z_ptr, char name[]);

void
  util_overlay_seed (GRID_P z_prob_ptr);

void
  util_init_grid (GRID_P gif,                                /* OUT    */
                 PIXEL value                                 /* IN     */
);
#endif
