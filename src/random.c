#define RANDOM_MODULE

#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "random.h"
#include "ugm_macros.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char random_c_sccs_id[] = "@(#)random.c	1.230	12/4/00";

static RANDOM_SEED_TYPE iv[NUM_THREADS][32];
static RANDOM_SEED_TYPE iy[NUM_THREADS];

/* routine from Numerical Recipes in C to generate random numbers */
/* (C) Copr. 1986-92 Numerical Recipes Software '%12'%. */

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: ran_random
** PURPOSE:       generate random number
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  ran_random (RANDOM_SEED_TYPE * ran_idum)
{
  int thread_id = omp_get_thread_num();
  char func[] = "ran_random";
  int j;
  int k;
  
  double temp;
  double random_num;
  
  // #pragma omp critical
  // {
    FUNC_INIT;
    if ((*ran_idum) <= 0 || !iy[thread_id])
    {
      if (-(*ran_idum) < 1)
      {
        (*ran_idum) = 1;
      }
      else
      {
        (*ran_idum) = -(*ran_idum);
      }
      for (j = 32 + 7; j >= 0; j--)
      {
        k = (*ran_idum) / 127773;
        (*ran_idum) = 16807 * ((*ran_idum) - k * 127773) - 2836 * k;
        if ((*ran_idum) < 0)
        {
          (*ran_idum) += 2147483647;
        }
        if (j < 32)
        {
          iv[thread_id][j] = (*ran_idum);
        }
      }
      iy[thread_id] = iv[thread_id][0];
    }
    k = (*ran_idum) / 127773;
    (*ran_idum) = 16807 * ((*ran_idum) - k * 127773) - 2836 * k;
    if ((*ran_idum) < 0)
    {
      (*ran_idum) += 2147483647;
    }
    j = iy[thread_id] / (1 + (2147483647 - 1) / 32);
    iy[thread_id] = iv[thread_id][j];
    iv[thread_id][j] = (*ran_idum);
    if ((temp = (1.0 / 2147483647) * iy[thread_id]) > (1.0 - 1.2e-7))
    {
      random_num = 1.0 - 1.2e-7;
    }
    else
    {
      random_num = temp;
    }
    FUNC_END;
  // }
  return random_num;
}
/* (C) Copr. 1986-92 Numerical Recipes Software '%12'%. */


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: InitRandom
** PURPOSE:       initialize random number generator
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  InitRandom (RANDOM_SEED_TYPE seed)
{
  char func[] = "InitRandom";
  FUNC_INIT;
  ran_seed = -labs (seed);
  RANNUM;
  FUNC_END;
}
