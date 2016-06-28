#ifndef DELTATRON_H
#define DELTATRON_H

#ifdef DELTATRON_MODULE
  /* stuff visable only to the deltatron module */
char deltatron_h_sccs_id[] = "@(#)deltatron.h	1.243	12/4/00";

  
#endif
/* #defines visable to any module including this header file*/

void
  delta_deltatron (int *new_indices,                               /* IN     */
                   Classes * landuse_classes,                      /* IN     */
                   Classes * class_indices,                        /* IN     */
                   GRID_P workspace1,                             /* MOD    */
                   GRID_P deltatron,                              /* IN/OUT */
                   GRID_P urban_land,                             /* IN     */
                   GRID_P land_out,                               /* OUT    */
                   GRID_P slp,                                    /* IN     */
                   int drive,                                      /* IN     */
                   CLASS_SLP_TYPE* class_slope,                    /* IN     */
                   FTRANS_TYPE* ftransition);                      /* IN     */

#endif
