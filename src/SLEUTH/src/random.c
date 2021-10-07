#define RANDOM_MODULE

#include <stdlib.h>
#include <string.h>
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
  char func[] = "ran_random";
  int j;
  int k;
  static RANDOM_SEED_TYPE iv[32];
  static RANDOM_SEED_TYPE iy;
  double temp;
  double random_num;

  FUNC_INIT;
  if ((*ran_idum) <= 0 || !iy)
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
        iv[j] = (*ran_idum);
      }
    }
    iy = iv[0];
  }
  k = (*ran_idum) / 127773;
  (*ran_idum) = 16807 * ((*ran_idum) - k * 127773) - 2836 * k;
  if ((*ran_idum) < 0)
  {
    (*ran_idum) += 2147483647;
  }
  j = iy / (1 + (2147483647 - 1) / 32);
  iy = iv[j];
  iv[j] = (*ran_idum);
  if ((temp = (1.0 / 2147483647) * iy) > (1.0 - 1.2e-7))
  {
    random_num = 1.0 - 1.2e-7;
  }
  else
  {
    random_num = temp;
  }
  FUNC_END;
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
