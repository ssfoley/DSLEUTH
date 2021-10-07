/******************************************************************************
*******************************************************************************

The coeff_obj.c module encapsulates the parameter data structures

*******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "coeff_obj.h"
#include "ugm_macros.h"
#include "ugm_defines.h"
#include "scenario_obj.h"
#include "proc_obj.h"
#include "memory_obj.h"
#include "globals.h"

char coeff_obj_c_sccs_id[] = "@(#)coeff_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static coeff_val_info saved_coefficient;
static coeff_val_info current_coefficient;
static coeff_int_info step_coeff;
static coeff_int_info start_coeff;
static coeff_int_info stop_coeff;
static coeff_int_info best_fit_coeff;
static char coeff_filename[MAX_FILENAME_LEN];

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                             SET FUNCTIONS                                 **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetCurrentDiffusion
** PURPOSE:       set the diffusion field
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetCurrentDiffusion (double val)
{
  if (val == 0)
  {
    current_coefficient.diffusion = 1;
    saved_coefficient.diffusion = 1;
  }
  else
  {
    current_coefficient.diffusion = val;
    saved_coefficient.diffusion = val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetCurrentSpread
** PURPOSE:       set the spread field of current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetCurrentSpread (double val)
{
  if (val == 0)
  {
    current_coefficient.spread = 1;
    saved_coefficient.spread = 1;
  }
  else
  {
    current_coefficient.spread = val;
    saved_coefficient.spread = val;
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetCurrentBreed
** PURPOSE:       set breed field of current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetCurrentBreed (double val)
{
  if (val == 0)
  {
    current_coefficient.breed = 1;
    saved_coefficient.breed = 1;
  }
  else
  {
    current_coefficient.breed = val;
    saved_coefficient.breed = val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetCurrentSlopeResist
** PURPOSE:       set slope_resistance field of current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetCurrentSlopeResist (double val)
{
  if (val == 0)
  {
    current_coefficient.slope_resistance = 1;
    saved_coefficient.slope_resistance = 1;
  }
  else
  {
    current_coefficient.slope_resistance = val;
    saved_coefficient.slope_resistance = val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetCurrentRoadGravity
** PURPOSE:       set road_gravity field of current_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetCurrentRoadGravity (double val)
{
  if (val == 0)
  {
    current_coefficient.road_gravity = 1;
    saved_coefficient.road_gravity = 1;
  }
  else
  {
    current_coefficient.road_gravity = val;
    saved_coefficient.road_gravity = val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStepDiffusion
** PURPOSE:       set diffusion field of step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStepDiffusion (int val)
{
  step_coeff.diffusion = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStepSpread
** PURPOSE:       set spread of step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStepSpread (int val)
{
  step_coeff.spread = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStepBreed
** PURPOSE:       set breed field of step_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStepBreed (int val)
{
  step_coeff.breed = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStepSlopeResist
** PURPOSE:       set slope_resistance field of step_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStepSlopeResist (int val)
{
  step_coeff.slope_resistance = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStepRoadGravity
** PURPOSE:       set road_gravity field of step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStepRoadGravity (int val)
{
  step_coeff.road_gravity = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: diffusion
** PURPOSE:       set diffusion field of start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStartDiffusion (int val)
{
  start_coeff.diffusion = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStartSpread
** PURPOSE:       set spread field of start_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  coeff_SetStartSpread (int val)
{
  start_coeff.spread = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStartBreed
** PURPOSE:       set breed field of start_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStartBreed (int val)
{
  start_coeff.breed = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStartSlopeResist
** PURPOSE:       set slope_resistance field of start_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStartSlopeResist (int val)
{
  start_coeff.slope_resistance = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStartRoadGravity
** PURPOSE:       set road_gravity field of start_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStartRoadGravity (int val)
{
  start_coeff.road_gravity = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStopDiffusion
** PURPOSE:       set diffusion field of stop_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStopDiffusion (int val)
{
  stop_coeff.diffusion = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStopSpread
** PURPOSE:       set spread field of stop_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStopSpread (int val)
{
  stop_coeff.spread = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStopBreed
** PURPOSE:       set breed field of stop_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStopBreed (int val)
{
  stop_coeff.breed = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStopSlopeResist
** PURPOSE:       set slope_resistance field of stop_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStopSlopeResist (int val)
{
  stop_coeff.slope_resistance = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetStopRoadGravity
** PURPOSE:       set road_gravity field of stop_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetStopRoadGravity (int val)
{
  stop_coeff.road_gravity = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetBestFitDiffusion
** PURPOSE:       set diffusion field of best_fit_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetBestFitDiffusion (int val)
{
  best_fit_coeff.diffusion = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetBestFitSpread
** PURPOSE:       set spread field of best_fit_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetBestFitSpread (int val)
{
  best_fit_coeff.spread = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetBestFitBreed
** PURPOSE:       set breed field of best_fit_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetBestFitBreed (int val)
{
  best_fit_coeff.breed = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetBestFitSlopeResist
** PURPOSE:       set slope_resistance field of best_fit_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetBestFitSlopeResist (int val)
{
  best_fit_coeff.slope_resistance = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetBestFitRoadGravity
** PURPOSE:       set road_gravity field of best_fit_coeff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetBestFitRoadGravity (int val)
{
  best_fit_coeff.road_gravity = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetSavedDiffusion
** PURPOSE:       set diffusion field of saved_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetSavedDiffusion (double val)
{
  saved_coefficient.diffusion = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetSavedSpread
** PURPOSE:       set spread field of saved_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetSavedSpread (double val)
{
  saved_coefficient.spread = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetSavedBreed
** PURPOSE:       set breed field of saved_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetSavedBreed (double val)
{
  saved_coefficient.breed = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetSavedSlopeResist
** PURPOSE:       set slope_resistance field of saved_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetSavedSlopeResist (double val)
{
  saved_coefficient.slope_resistance = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SetSavedRoadGravity
** PURPOSE:       set road_gravity field of saved_coefficient
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_SetSavedRoadGravity (double val)
{
  saved_coefficient.road_gravity = val;
}


/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                             GET FUNCTIONS                                 **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetSavedDiffusion
** PURPOSE:       return diffusion from saved_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetSavedDiffusion ()
{
  return saved_coefficient.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetSavedSpread
** PURPOSE:       return spread from saved_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetSavedSpread ()
{
  return saved_coefficient.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetSavedBreed
** PURPOSE:       return breed from saved_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetSavedBreed ()
{
  return saved_coefficient.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetSavedSlopeResist
** PURPOSE:       return slope_resistance from saved_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetSavedSlopeResist ()
{
  return saved_coefficient.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetSavedRoadGravity
** PURPOSE:       return road_gravity from saved_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetSavedRoadGravity ()
{
  return saved_coefficient.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetCurrentDiffusion
** PURPOSE:       return diffusion from current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetCurrentDiffusion ()
{
  return current_coefficient.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetCurrentSpread
** PURPOSE:       return spread from current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetCurrentSpread ()
{
  return current_coefficient.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetCurrentBreed
** PURPOSE:       return breed from current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetCurrentBreed ()
{
  return current_coefficient.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetCurrentSlopeResist
** PURPOSE:       return slope_resistance from current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetCurrentSlopeResist ()
{
  return current_coefficient.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetCurrentRoadGravity
** PURPOSE:       return road_gravity from current_coefficient struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  coeff_GetCurrentRoadGravity ()
{
  return current_coefficient.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStepDiffusion
** PURPOSE:       return diffusion from step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStepDiffusion ()
{
  return step_coeff.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStepSpread
** PURPOSE:       return spread from step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStepSpread ()
{
  return step_coeff.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStepBreed
** PURPOSE:       return breed from step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStepBreed ()
{
  return step_coeff.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStepSlopeResist
** PURPOSE:       return slope_resistance from step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStepSlopeResist ()
{
  return step_coeff.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStepRoadGravity
** PURPOSE:       return road_gravity from step_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStepRoadGravity ()
{
  return step_coeff.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStartDiffusion
** PURPOSE:       return diffusion from start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStartDiffusion ()
{
  return start_coeff.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStartSpread
** PURPOSE:       return spread from start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStartSpread ()
{
  return start_coeff.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStartBreed
** PURPOSE:       return breed from start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStartBreed ()
{
  return start_coeff.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStartSlopeResist
** PURPOSE:       return slope_resistance from start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStartSlopeResist ()
{
  return start_coeff.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStartRoadGravity
** PURPOSE:       return road_gravity from start_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStartRoadGravity ()
{
  return start_coeff.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStopDiffusion
** PURPOSE:       return diffusion from stop_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStopDiffusion ()
{
  return stop_coeff.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStopSpread
** PURPOSE:       return spread from stop_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStopSpread ()
{
  return stop_coeff.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStopBreed
** PURPOSE:       return breed from stop_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStopBreed ()
{
  return stop_coeff.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStopSlopeResist
** PURPOSE:       return slope_resistance from stop_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStopSlopeResist ()
{
  return stop_coeff.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetStopRoadGravity
** PURPOSE:       return road_gravity from stop_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetStopRoadGravity ()
{
  return stop_coeff.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetBestFitDiffusion
** PURPOSE:       return diffusion from best_fit_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetBestFitDiffusion ()
{
  return best_fit_coeff.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetBestFitSpread
** PURPOSE:       return spread from best_fit_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetBestFitSpread ()
{
  return best_fit_coeff.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetBestFitBreed
** PURPOSE:       return breed from best_fit_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetBestFitBreed ()
{
  return best_fit_coeff.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetBestFitSlopeResist
** PURPOSE:       return slope_resistance from best_fit_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetBestFitSlopeResist ()
{
  return best_fit_coeff.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_GetBestFitRoadGravity
** PURPOSE:       return road_gravity from best_fit_coeff struct
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  coeff_GetBestFitRoadGravity ()
{
  return best_fit_coeff.road_gravity;
}

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                             LOGGING FUNCTIONS                             **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_MemoryLog
** PURPOSE:       log pointers to memory locations to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  coeff_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &saved_coefficient, sizeof (coeff_val_info), 1);
  LOG_MEM (fp, &current_coefficient, sizeof (coeff_val_info), 1);
  LOG_MEM (fp, &step_coeff, sizeof (coeff_val_info), 1);
  LOG_MEM (fp, &start_coeff, sizeof (coeff_val_info), 1);
  LOG_MEM (fp, &stop_coeff, sizeof (coeff_val_info), 1);
  LOG_MEM (fp, &best_fit_coeff, sizeof (coeff_val_info), 1);
  LOG_MEM_CHAR_ARRAY (fp, &coeff_filename[0], sizeof (char), MAX_FILENAME_LEN);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogSaved
** PURPOSE:       log values in saved_coefficient struct to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogSaved (FILE * fp)
{
  LOG_FLOAT (fp, saved_coefficient.diffusion);
  LOG_FLOAT (fp, saved_coefficient.spread);
  LOG_FLOAT (fp, saved_coefficient.breed);
  LOG_FLOAT (fp, saved_coefficient.slope_resistance);
  LOG_FLOAT (fp, saved_coefficient.road_gravity);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogCurrent
** PURPOSE:       log values in current_coefficient to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogCurrent (FILE * fp)
{
  LOG_FLOAT (fp, current_coefficient.diffusion);
  LOG_FLOAT (fp, current_coefficient.spread);
  LOG_FLOAT (fp, current_coefficient.breed);
  LOG_FLOAT (fp, current_coefficient.slope_resistance);
  LOG_FLOAT (fp, current_coefficient.road_gravity);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogStep
** PURPOSE:       log values in step_coeff struct to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogStep (FILE * fp)
{
  LOG_INT (fp, step_coeff.diffusion);
  LOG_INT (fp, step_coeff.spread);
  LOG_INT (fp, step_coeff.breed);
  LOG_INT (fp, step_coeff.slope_resistance);
  LOG_INT (fp, step_coeff.road_gravity);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogStart
** PURPOSE:       log values in start_coeff struct to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogStart (FILE * fp)
{
  LOG_INT (fp, start_coeff.diffusion);
  LOG_INT (fp, start_coeff.spread);
  LOG_INT (fp, start_coeff.breed);
  LOG_INT (fp, start_coeff.slope_resistance);
  LOG_INT (fp, start_coeff.road_gravity);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogStop
** PURPOSE:       log values in stop_coeff struct to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogStop (FILE * fp)
{
  LOG_INT (fp, stop_coeff.diffusion);
  LOG_INT (fp, stop_coeff.spread);
  LOG_INT (fp, stop_coeff.breed);
  LOG_INT (fp, stop_coeff.slope_resistance);
  LOG_INT (fp, stop_coeff.road_gravity);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_LogBestFit
** PURPOSE:       log values in best_fit_coeff struct to file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  coeff_LogBestFit (FILE * fp)
{
  LOG_INT (fp, best_fit_coeff.diffusion);
  LOG_INT (fp, best_fit_coeff.spread);
  LOG_INT (fp, best_fit_coeff.breed);
  LOG_INT (fp, best_fit_coeff.slope_resistance);
  LOG_INT (fp, best_fit_coeff.road_gravity);
}

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                             OUTPUT FUNCTIONS                              **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_ConcatenateFiles
** PURPOSE:       concatenate coefficient files for a given run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
#if 1
void
  coeff_ConcatenateFiles ()
#else
void
  coeff_ConcatenateFiles (int current_run)
#endif
{
#if 1
#define MAX_LINE_LEN 256
  char func[] = "coeff_ConcatenateFiles";
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  int i;
  FILE *fp;
  FILE *source_fp;
  int line_count;
  char line[MAX_LINE_LEN];


  if (scen_GetWriteCoeffFileFlag ())
  {
    /*
     *
     * create the destination coeff.log file by copying the zeroth
     * file onto coeff_log
     *
     */
    sprintf (destination_file, "%scoeff.log", scen_GetOutputDir ());
    sprintf (source_file, "%scoeff_run%u", scen_GetOutputDir (), 0);
    sprintf (command, "mv %s %s", source_file, destination_file);
    system (command);

    /*
     *
     * loop over all the files appending each to the destination file
     *
     */
    for (i = 1; i < glb_npes; i++)
    {
      FILE_OPEN (fp, destination_file, "a");

      sprintf (source_file, "%scoeff_run%u", scen_GetOutputDir (), i);

      FILE_OPEN (source_fp, source_file, "r");

      line_count = 0;
      while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
      {
        line_count++;
        if (line_count <= 1)
          continue;
        fputs (line, fp);
      }
      fclose (source_fp);
      fclose (fp);

      sprintf (command, "rm %s", source_file);
      system (command);
    }
  }

#else


#define MAX_LINE_LEN 256
  char func[] = "coeff_ConcatenateFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;


  if (scen_GetWriteCoeffFileFlag ())
  {
    sprintf (destination_file, "%scoeff.log", scen_GetOutputDir ());
    if (current_run == 0)
    {
      /*
       *
       * CURRENT_RUN == 0 SO THERE IS ONLY 1 FILE SO WE JUST MOVE IT TO 
       * THE RIGHT LOCATION
       *
       */
      sprintf (source_file, "%scoeff_run%u", scen_GetOutputDir (), 0);
      sprintf (command, "mv %s %s", source_file, destination_file);
      system (command);
    }
    else
    {
      /*
       *
       * CURRENT_RUN != 0 SO WE MUST OPEN THE DESTINATION FILE IN APPEND MODE
       * APPEND EACH LINE OF THE SOURCE FILE TO IT
       *
       */
      FILE_OPEN (fp, destination_file, "a");

      sprintf (source_file, "%scoeff_run%u", scen_GetOutputDir (), current_run);

      FILE_OPEN (source_fp, source_file, "r");

      line_count = 0;
      while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
      {
        line_count++;
        if (line_count <= 1)
          continue;
        fputs (line, fp);
      }
      fclose (source_fp);
      fclose (fp);
      /*
       *
       * REMOVE THE NO LONGER NEEDED SOURCE FILE
       *
       */
      sprintf (command, "rm %s", source_file);
      system (command);
    }
  }
#endif
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_WriteCurrentCoeff
** PURPOSE:       write current coefficients to a coefficient file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   the coefficient file is initially created by
**                coeff_CreateCoeffFile () which initializes
**                the coeff_filename variable.
**
**
*/
void
  coeff_WriteCurrentCoeff ()
{
  char func[] = "coeff_WriteCurrentCoeff";
  FILE *fp;

  if (scen_GetWriteCoeffFileFlag ())
  {
    FILE_OPEN (fp, coeff_filename, "a");

    fprintf (fp, "%5u %5u %4u %8.2f %8.2f %8.2f %8.2f %8.2f\n",
             proc_GetCurrentRun (),
             proc_GetCurrentMonteCarlo (),
             proc_GetCurrentYear (),
             current_coefficient.diffusion,
             current_coefficient.breed,
             current_coefficient.spread,
             current_coefficient.slope_resistance,
             current_coefficient.road_gravity);
    fclose (fp);
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_CreateCoeffFile
** PURPOSE:       creates a coefficient file for the current run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  coeff_CreateCoeffFile ()
{
  char func[] = "coeff_CreateCoeffFile";
  FILE *fp;
  if (scen_GetWriteCoeffFileFlag ())
  {
#if 1
    sprintf (coeff_filename, "%scoeff_run%u",
             scen_GetOutputDir (), glb_mype);
#else
    sprintf (coeff_filename, "%scoeff_run%u",
             scen_GetOutputDir (), proc_GetCurrentRun ());
#endif

    FILE_OPEN (fp, coeff_filename, "w");

    fprintf (fp,
    "  Run    MC Year Diffusion   Breed   Spread SlopeResist RoadGrav\n");
    fclose (fp);
  }
}

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                          MANIPULATIVE FUNCTIONS                           **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: coeff_SelfModication
** PURPOSE:       performs self modification of parameters
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  coeff_SelfModication (double growth_rate, double percent_urban)
{
  char func[] = "coeff_SelfModication";

  /*
   *
   * boom year
   *
   */
  if (growth_rate > scen_GetCriticalHigh ())
  {
    current_coefficient.slope_resistance -=
      (double) (percent_urban * scen_GetSlopeSensitivity ());
    if (current_coefficient.slope_resistance <= MIN_SLOPE_RESISTANCE_VALUE)
    {
      current_coefficient.slope_resistance = 1.0;
    }

    current_coefficient.road_gravity +=
      (double) (percent_urban * scen_GetRdGrvtySensitivity ());
    if (current_coefficient.road_gravity > MAX_ROAD_GRAVITY_VALUE)
    {
      current_coefficient.road_gravity = MAX_ROAD_GRAVITY_VALUE;
    }

    if (current_coefficient.diffusion < MAX_DIFFUSION_VALUE)
    {
      current_coefficient.diffusion *= scen_GetBoom ();

      if (current_coefficient.diffusion > MAX_DIFFUSION_VALUE)
      {
        current_coefficient.diffusion = MAX_DIFFUSION_VALUE;
      }

      current_coefficient.breed *= scen_GetBoom ();
      if (current_coefficient.breed > MAX_BREED_VALUE)
      {
        current_coefficient.breed = MAX_BREED_VALUE;
      }

      current_coefficient.spread *= scen_GetBoom ();
      if (current_coefficient.spread > MAX_SPREAD_VALUE)
      {
        current_coefficient.spread = MAX_SPREAD_VALUE;
      }
    }
  }

  /*
   *
   * bust year
   *
   */
  if (growth_rate < scen_GetCriticalLow ())
  {
    current_coefficient.slope_resistance +=
      (double) (percent_urban * scen_GetSlopeSensitivity ());
    if (current_coefficient.slope_resistance > MAX_SLOPE_RESISTANCE_VALUE)
    {
      current_coefficient.slope_resistance = MAX_SLOPE_RESISTANCE_VALUE;
    }

    current_coefficient.road_gravity -=
      (double) (percent_urban * scen_GetRdGrvtySensitivity ());
    if (current_coefficient.road_gravity <= MIN_ROAD_GRAVITY_VALUE)
    {
      current_coefficient.road_gravity = 1.0;
    }

    if ((growth_rate < scen_GetCriticalLow ()) &&
        (current_coefficient.diffusion > 0))
    {
      current_coefficient.diffusion *= scen_GetBust ();
      if (current_coefficient.diffusion <= MIN_DIFFUSION_VALUE)
      {
        current_coefficient.diffusion = 1.0;
      }

      current_coefficient.spread *= scen_GetBust ();
      if (current_coefficient.spread <= MIN_SPREAD_VALUE)
      {
        current_coefficient.spread = 1.0;
      }

      current_coefficient.breed *= scen_GetBust ();

      if (current_coefficient.breed <= MIN_BREED_VALUE)
      {
        current_coefficient.breed = 1.0;
      }
    }
  }
}
