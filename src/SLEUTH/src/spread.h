#ifndef SPREAD_H
#define SPREAD_H

#ifdef SPREAD_MODULE
  /* stuff visable only to the growth module */
char spread_h_sccs_id[] = "@(#)spread.h	1.243	12/4/00";

  
#endif


/* #defines visable to any module including this header file*/

/*
 *
 * FUNCTION PROTOTYPES
 *
 */

void
  spr_spread  (
              float *average_slope,                          /* OUT    */
              int *num_growth_pix,                           /* OUT    */
              int* sng,
              int* sdc,
              int* og,
              int* rt,
              int* pop,
              GRID_P z                                     /* IN/OUT */
              );                       /* MOD    */

#endif
