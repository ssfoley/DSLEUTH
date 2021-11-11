#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "timer_obj.h"
#include "scenario_obj.h"
#include "ugm_defines.h"
#include "globals.h"
#include "ugm_macros.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define MAX_NUM_TIMERS 20

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
typedef struct
{
  char name[125];
  clock_t start;
  clock_t stop;
  int num_calls;
  float total_time;
  float average_time;
  BOOLEAN running;
}
ugm_timer_t;

static ugm_timer_t array[MAX_NUM_TIMERS];
static int actual_num_timers;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char timer_obj_c_sccs_id[] = "@(#)timer_obj.c	1.81	12/4/00";


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  timer_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &array[0], sizeof (ugm_timer_t), MAX_NUM_TIMERS);
  LOG_MEM (fp, &actual_num_timers, sizeof (int), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_Init
** PURPOSE:       initialize timers
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  timer_Init ()
{
  char func[] = "timer_Init";
  int i;

  actual_num_timers = 12;
  if (actual_num_timers > MAX_NUM_TIMERS)
  {
    sprintf (msg_buf, "actual_num_timers > MAX_NUM_TIMERS");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  strcpy (array[0].name, "spr_spread");
  strcpy (array[1].name, "spr_phase1n3");
  strcpy (array[2].name, "spr_phase4");
  strcpy (array[3].name, "spr_phase5");
  strcpy (array[4].name, "gdif_WriteGIF");
  strcpy (array[5].name, "gdif_ReadGIF");
  strcpy (array[6].name, "delta_deltatron");
  strcpy (array[7].name, "delta_phase1");
  strcpy (array[8].name, "delta_phase2");
  strcpy (array[9].name, "grw_growth");
  strcpy (array[10].name, "drv_driver");
  strcpy (array[11].name, "main");

  for (i = 0; i < actual_num_timers; i++)
  {
    array[i].num_calls = 0;
    array[i].running = FALSE;
    array[i].total_time = 0.0;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_Start
** PURPOSE:       start a timer
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  timer_Start (int val)
{
  char func[] = "timer_Start";
  if ((val < 0) || (val > actual_num_timers))
  {
    sprintf (msg_buf, "(val < 0) || (val > actual_num_timers)");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (array[val].running)
  {
    sprintf (msg_buf, "array[%u].running", val);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  array[val].start = clock ();
  array[val].num_calls++;
  array[val].running = TRUE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_Read
** PURPOSE:       read a timer
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  timer_Read (int val)
{
  char func[] = "timer_Read";
  if ((val < 0) || (val > actual_num_timers))
  {
    sprintf (msg_buf, "(val < 0) || (val > actual_num_timers)");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (!array[val].running)
  {
    sprintf (msg_buf, "array[%u].running", val);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  return (((double) (clock () - array[val].start)
           * 1000) / CLOCKS_PER_SEC) + array[val].total_time;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_Stop
** PURPOSE:       stop a timer
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  timer_Stop (int val)
{
  char func[] = "timer_Stop";
  if ((val < 0) || (val > actual_num_timers))
  {
    sprintf (msg_buf, "(val < 0) || (val > actual_num_timers)");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (!array[val].running)
  {
    sprintf (msg_buf, "array[%u].running", val);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  array[val].running = FALSE;
  array[val].stop = clock ();
  array[val].total_time += ((float) (array[val].stop - array[val].start)
                            * 1000) / CLOCKS_PER_SEC;
  array[val].average_time = array[val].total_time / array[val].num_calls;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: timer_LogIt
** PURPOSE:       log timer results to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  timer_LogIt (FILE * fp)
{
  char buf[15];
  int i;
  fprintf (fp, "\n\n****************************LOG OF TIMINGS ");
  fprintf (fp, "***********************************\n");
  fprintf (fp, "        Routine #Calls    Avg Time    Total Time\n");
  fprintf (fp, "                         (millisec)   (millisec)\n");
  for (i = 0; i < actual_num_timers; i++)
  {
    fprintf (fp, "%15s %5u     %8.2f      %10.2f = %s\n",
             array[i].name,
             array[i].num_calls,
             array[i].average_time,
             array[i].total_time,
        timer_Format (buf, (unsigned int) (array[i].total_time / 1000)));
  }
  fprintf (fp, "Number of CPUS = %u\n", glb_npes);
}
char *
  timer_Format (char *buf, unsigned int sec)
{
  unsigned int days;
  unsigned int hrs;
  unsigned int min;

#define SEC_PER_DAY (60*60*24)
#define SEC_PER_HR  (60*60)
#define SEC_PER_MIN 60

  days = sec / SEC_PER_DAY;
  sec -= MAX ((days * SEC_PER_DAY), 0);

  hrs = sec / SEC_PER_HR;
  sec -= MAX ((hrs * SEC_PER_HR), 0);

  min = sec / SEC_PER_MIN;
  sec -= MAX ((min * SEC_PER_MIN), 0);

  sprintf (buf, "%04u:%02u:%02u:%02u", days, hrs, min, sec);
  return buf;
}
