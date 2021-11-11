#include <stdio.h>
#include <stdlib.h>
#include "memory_obj.h"
#include "ugm_macros.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define NUM_WORKING_GRIDS 6

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char wgrid_obj_c_sccs_id[] = "@(#)wgrid_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static int num_working_grids;
static BOOLEAN set_flag;

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: wgrid_GetWGridCount
** PURPOSE:       return num_working_grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  wgrid_GetWGridCount ()
{
  char func[] = "wgrid_GetWGridCount";
  if (set_flag != TRUE)
  {
    sprintf (msg_buf,
    "NUM_WORKING_GRIDS is not set. Please set it in your scenario file");
    LOG_ERROR (msg_buf);
    EXIT (1);

  }
  return num_working_grids;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: wgrid_SetWGridCount
** PURPOSE:       set num_working_grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  wgrid_SetWGridCount (int val)
{
  num_working_grids = val;
  set_flag = TRUE;
}
