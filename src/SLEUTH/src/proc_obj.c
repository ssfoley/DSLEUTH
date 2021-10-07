#include <stdio.h>
#include "ugm_defines.h"
#include "ugm_macros.h"
#include "coeff_obj.h"
#include "proc_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char proc_obj_c_sccs_id[] = "@(#)proc_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static int type_of_processing;
static int total_runs;
static int total_runs_exec_this_cpu;
static int last_run;
static int last_mc;
static int current_run;
static int current_monte_carlo;
static int current_year;
static int stop_year;
static BOOLEAN restart_flag;
static BOOLEAN last_run_flag;
static BOOLEAN last_mc_flag;

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &type_of_processing, sizeof (int), 1);
  LOG_MEM (fp, &total_runs, sizeof (int), 1);
  LOG_MEM (fp, &last_run, sizeof (int), 1);
  LOG_MEM (fp, &last_mc, sizeof (int), 1);
  LOG_MEM (fp, &current_run, sizeof (int), 1);
  LOG_MEM (fp, &current_monte_carlo, sizeof (int), 1);
  LOG_MEM (fp, &current_year, sizeof (int), 1);
  LOG_MEM (fp, &stop_year, sizeof (int), 1);
  LOG_MEM (fp, &last_run_flag, sizeof (BOOLEAN), 1);
  LOG_MEM (fp, &last_mc_flag, sizeof (BOOLEAN), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetRestartFlag
** PURPOSE:       set the restart flag variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetRestartFlag (BOOLEAN i)
{
  restart_flag = i;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetRestartFlag
** PURPOSE:       return the restart flag variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  proc_GetRestartFlag ()
{
  return restart_flag;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetProcessingType
** PURPOSE:       set the processing type variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetProcessingType (int i)
{
  type_of_processing = i;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetTotalRuns
** PURPOSE:       count the total # of runs
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetTotalRuns ()
{
  int i;
  int j;
  int k;
  int l;
  int m;
  int x1 = 0;
  int x2 = 0;
  int x3 = 0;
  int x4 = 0;
  int x5 = 0;
  for (i = coeff_GetStartDiffusion ();
       i <= coeff_GetStopDiffusion ();
       i += coeff_GetStepDiffusion ())
  {
    x1++;
  }
  for (j = coeff_GetStartBreed ();
       j <= coeff_GetStopBreed ();
       j += coeff_GetStepBreed ())
  {
    x2++;
  }
  for (k = coeff_GetStartSpread ();
       k <= coeff_GetStopSpread ();
       k += coeff_GetStepSpread ())
  {
    x3++;
  }
  for (l = coeff_GetStartSlopeResist ();
       l <= coeff_GetStopSlopeResist ();
       l += coeff_GetStepSlopeResist ())
  {
    x4++;
  }
  for (m = coeff_GetStartRoadGravity ();
       m <= coeff_GetStopRoadGravity ();
       m += coeff_GetStepRoadGravity ())
  {
    x5++;
  }
  x1 = MAX (x1, 1);
  x2 = MAX (x2, 1);
  x3 = MAX (x3, 1);
  x4 = MAX (x4, 1);
  x5 = MAX (x5, 1);
  total_runs = x1 * x2 * x3 * x4 * x5;
  last_run_flag = FALSE;
  last_mc_flag = FALSE;
  last_run = total_runs - 1;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetCurrentRun
** PURPOSE:       set the current run variable, current_run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetCurrentRun (int i)
{
  current_run = i;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetCurrentMonteCarlo
** PURPOSE:       set current monte carlo variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetCurrentMonteCarlo (int i)
{
  current_monte_carlo = i;
  proc_SetLastMonteCarloFlag ();
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetCurrentYear
** PURPOSE:       set current year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetCurrentYear (int i)
{
  current_year = i;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetStopYear
** PURPOSE:       set the stop year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetStopYear (int i)
{
  stop_year = i;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetProcessingType
** PURPOSE:       return the processing type
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetProcessingType ()
{
  return type_of_processing;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetTotalRuns
** PURPOSE:       return total run count
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetTotalRuns ()
{
  return total_runs;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetCurrentRun
** PURPOSE:       return the current run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetCurrentRun ()
{
  return current_run;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetCurrentMonteCarlo
** PURPOSE:       return the current monte carlo
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetCurrentMonteCarlo ()
{
  return current_monte_carlo;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetCurrentYear
** PURPOSE:       return the current year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetCurrentYear ()
{
  return current_year;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetStopYear
** PURPOSE:       return the stop year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetStopYear ()
{
  return stop_year;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetLastRun
** PURPOSE:       return last run count
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetLastRun ()
{
  return last_run;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetLastRunFlag
** PURPOSE:       return last run flag; TRUE if this is the last run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  proc_GetLastRunFlag ()
{
  return last_run_flag;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetLastMonteCarlo
** PURPOSE:       set last monte carlo run
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetLastMonteCarlo (int val)
{
  last_mc = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetLastMonteCarloFlag
** PURPOSE:       return last monte carlo flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  proc_GetLastMonteCarloFlag ()
{
  return last_mc_flag;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetNumRunsExecThisCPU
** PURPOSE:       set the num runs executed by this cpu
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_SetNumRunsExecThisCPU (int val)
{
  total_runs_exec_this_cpu = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_GetNumRunsExecThisCPU
** PURPOSE:       return the num runs executed by this cpu
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_GetNumRunsExecThisCPU ()
{
  return total_runs_exec_this_cpu;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_IncrementNumRunsExecThisCPU
** PURPOSE:       increment the num runs executed by this cpu
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  proc_IncrementNumRunsExecThisCPU ()
{
  total_runs_exec_this_cpu++;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_IncrementCurrentRun
** PURPOSE:       increment current run variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_IncrementCurrentRun ()
{
  if ((++current_run) == last_run)
  {
    last_run_flag = TRUE;
  }
  return (current_run);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_SetLastMonteCarloFlag
** PURPOSE:       increment current monte carlo variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_SetLastMonteCarloFlag ()
{
  if ((current_monte_carlo) == last_mc)
  {
    last_mc_flag = TRUE;
  }
  return (current_monte_carlo);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: proc_IncrementCurrentYear
** PURPOSE:       increment current year variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  proc_IncrementCurrentYear ()
{
  return (++current_year);
}
