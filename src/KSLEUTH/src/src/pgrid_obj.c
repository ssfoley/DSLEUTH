#include "globals.h"
#include "grid_obj.h"
#include "memory_obj.h"
#include "scenario_obj.h"
#include "ugm_macros.h"

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
static grid_info z;
static grid_info deltatron;
static grid_info delta;
static grid_info land1;
static grid_info land2;
static grid_info cumulate;

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
  LOG_MEM (fp, &z, sizeof (grid_info), 1);
  LOG_MEM (fp, &deltatron, sizeof (grid_info), 1);
  LOG_MEM (fp, &delta, sizeof (grid_info), 1);
  LOG_MEM (fp, &land1, sizeof (grid_info), 1);
  LOG_MEM (fp, &land2, sizeof (grid_info), 1);
  LOG_MEM (fp, &cumulate, sizeof (grid_info), 1);
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

  z.ptr = mem_GetPGridPtr (func);
  deltatron.ptr = mem_GetPGridPtr (func);
  delta.ptr = mem_GetPGridPtr (func);
  land1.ptr = mem_GetPGridPtr (func);
  land2.ptr = mem_GetPGridPtr (func);
  cumulate.ptr = mem_GetPGridPtr (func);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetZPtr
** PURPOSE:       return pointer to z grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetZPtr ()
{
  return z.ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetDeltatronPtr
** PURPOSE:       return pointer to deltatron grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetDeltatronPtr ()
{
  return deltatron.ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetDeltaPtr
** PURPOSE:       return pointer to delta grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetDeltaPtr ()
{
  return delta.ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetLand1Ptr
** PURPOSE:       return pointer to land1 grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetLand1Ptr ()
{
  return land1.ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetLand2Ptr
** PURPOSE:       return pointer to land2 grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetLand2Ptr ()
{
  return land2.ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: pgrid_GetCumulatePtr
** PURPOSE:       return pointer to cumulate grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  pgrid_GetCumulatePtr ()
{
  return cumulate.ptr;
}
