#include "globals.h"
#include "grid_obj.h"
#include "memory_obj.h"
#include "scenario_obj.h"
#include "ugm_macros.h"
#include <omp.h>

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define PGRID_COUNT 6

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char pgrid_obj_c_sccs_id[] = "@(#)pgrid_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static grid_info z[NUM_THREADS];
static grid_info deltatron[NUM_THREADS];
static grid_info delta[NUM_THREADS];
static grid_info land1[NUM_THREADS];
static grid_info land2[NUM_THREADS];
static grid_info cumulate[NUM_THREADS];

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  pgrid_MemoryLog (FILE * fp)
{
  // LOG_MEM (fp, &z, sizeof (grid_info), 1);
  // LOG_MEM (fp, &deltatron, sizeof (grid_info), 1);
  // LOG_MEM (fp, &delta, sizeof (grid_info), 1);
  // LOG_MEM (fp, &land1, sizeof (grid_info), 1);
  // LOG_MEM (fp, &land2, sizeof (grid_info), 1);
  // LOG_MEM (fp, &cumulate, sizeof (grid_info), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetPGridCount
** PURPOSE:       return PGRID_COUNT
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  pgrid_GetPGridCount ()
{
  return PGRID_COUNT;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_Init
** PURPOSE:       initialize the p type grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  pgrid_Init ()
{
  char func[] = "pgrid_Init";

  int i;

  for (i = 0; i < NUM_THREADS; ++i)
  {
    z[i].ptr = mem_GetPGridPtr (func, i);
    deltatron[i].ptr = mem_GetPGridPtr (func, i);
    delta[i].ptr = mem_GetPGridPtr (func, i);
    land1[i].ptr = mem_GetPGridPtr (func, i);
    land2[i].ptr = mem_GetPGridPtr (func, i);
    cumulate[i].ptr = mem_GetPGridPtr (func, i);
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetZPtr
** PURPOSE:       return pointer to z grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetZPtr (int i)
{
  return z[i].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetDeltatronPtr
** PURPOSE:       return pointer to deltatron grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetDeltatronPtr (int i)
{
  return deltatron[i].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetDeltaPtr
** PURPOSE:       return pointer to delta grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetDeltaPtr (int i)
{
  return delta[i].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetLand1Ptr
** PURPOSE:       return pointer to land1 grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetLand1Ptr (int i)
{
  return land1[i].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetLand2Ptr
** PURPOSE:       return pointer to land2 grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetLand2Ptr (int i)
{
  return land2[i].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetCumulatePtr
** PURPOSE:       return pointer to cumulate grid with thread id
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetCumulatePtr (int i)
{
  return cumulate[i].ptr;
}
