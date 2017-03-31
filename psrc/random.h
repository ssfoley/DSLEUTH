#ifndef RANDOM_H
#define RANDOM_H

#ifdef RANDOM_MODULE
  /* stuff visable only to the random module */
char random_h_sccs_id[] = "@(#)random.h	1.230	12/4/00";

  RANDOM_SEED_TYPE   ran_seed;
  int    glb_random_count;

#else

  extern RANDOM_SEED_TYPE   ran_seed;
  extern int    glb_random_count;

#endif
/* #defines visable to any module including this header file*/


#if 1
#define RANNUM ran_random(&ran_seed)
#else
#endif


/* reassign random numbers */
#define RANDOM_ROW  ((int)  (RANNUM * igrid_GetNumRows()))
#define RANDOM_COL  ((int)  (RANNUM * igrid_GetNumCols()))
#define RANDOM_INT(a)  ((int)  (RANNUM * (a)))
#define RANDOM_FLOAT  (RANNUM)


double ran_random(RANDOM_SEED_TYPE*);
void  InitRandom (RANDOM_SEED_TYPE);
#endif
