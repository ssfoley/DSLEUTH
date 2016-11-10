#define GROWTH_MODULE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coeff_obj.h"
#include "igrid_obj.h"
#include "pgrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "input.h"
#include "output.h"
#include "utilities.h"
#include "growth.h"
#include "spread.h"
#include "random.h"
#include "deltatron.h"
#include "ugm_macros.h"
#include "proc_obj.h"
#include "scenario_obj.h"
#include "memory_obj.h"
#include "transition_obj.h"
#include "color_obj.h"
#include "timer_obj.h"
#include "gdif_obj.h"
#include "timer_obj.h"
#include "stats_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char growth_c_sccs_id[] = "@(#)growth.c	1.629	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static
  void grw_non_landuse (GRID_P z_ptr);

static
  void grw_landuse_init (GRID_P deltatron_ptr,
                         GRID_P land1_ptr);
static
  void grw_landuse (
                     GRID_P land1_ptr,
                     int num_growth_pix);
static void grw_completion_status (FILE * fp);

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grw_grow
** PURPOSE:       loop over simulated years
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  grw_grow (GRID_P z_ptr, GRID_P land1_ptr)
{
  char func[] = "grw_grow";
  char gif_filename[MAX_FILENAME_LEN];
  char date_str[5];
  GRID_P deltatron_ptr;
  GRID_P seed_ptr;
  int total_pixels;
  float average_slope;
  int num_growth_pix = 0;
  int sng;
  int sdg;
  int sdc;
  int og;
  int rt;
  int pop;


  FUNC_INIT;
  timer_Start (GRW_GROWTH);
  total_pixels = mem_GetTotalPixels ();
  deltatron_ptr = pgrid_GetDeltatronPtr ();
  assert (total_pixels > 0);
  assert (deltatron_ptr != NULL);


  if (proc_GetProcessingType () == PREDICTING)
  {
    proc_SetCurrentYear (scen_GetPredictionStartDate ());
  }
  else
  {
    proc_SetCurrentYear (igrid_GetUrbanYear (0));
  }
  util_init_grid (z_ptr, 0);
  if (scen_GetDoingLanduseFlag ())
  {
    grw_landuse_init (deltatron_ptr,
                      land1_ptr);
  }

  seed_ptr = igrid_GetUrbanGridPtr (__FILE__, func, __LINE__, 0);
  util_condition_gif (total_pixels,
                      seed_ptr,
                      GT,
                      0,
                      z_ptr,
                      PHASE0G);
  seed_ptr = igrid_GridRelease (__FILE__, func, __LINE__, seed_ptr);

  if (scen_GetEchoFlag ())
  {
    printf ("\n%s %u ******************************************\n",
            __FILE__, __LINE__);
    if (proc_GetProcessingType () == CALIBRATING)
    {
      printf ("%s %u Run = %u of %u (%8.1f percent complete)\n",
              __FILE__, __LINE__,
              proc_GetCurrentRun (), proc_GetTotalRuns (),
              (100.0 * proc_GetCurrentRun ()) / proc_GetTotalRuns ());
    }
    printf ("%s %u Monte Carlo = %u of %u\n", __FILE__, __LINE__,
      proc_GetCurrentMonteCarlo () + 1, scen_GetMonteCarloIterations ());
    fprintf (stdout, "%s %u proc_GetCurrentYear=%u\n",
             __FILE__, __LINE__, proc_GetCurrentYear ());
    fprintf (stdout, "%s %u proc_GetStopYear=%u\n",
             __FILE__, __LINE__, proc_GetStopYear ());
  }

  if (scen_GetLogFlag ())
  {
    if (scen_GetLogProcessingStatusFlag () > 0)
    {
      scen_Append2Log ();
      grw_completion_status (scen_GetLogFP ());
      scen_CloseLog ();
    }
  }
  while (proc_GetCurrentYear () < proc_GetStopYear ())
  {
    /*
     *
     * INCREMENT CURRENT YEAR
     *
     */
    proc_IncrementCurrentYear ();

    if (scen_GetEchoFlag ())
    {
      fprintf (stdout, " %u", proc_GetCurrentYear ());
      fflush (stdout);
      if (((proc_GetCurrentYear () + 1) % 10) == 0)
      {
        fprintf (stdout, "\n");
        fflush (stdout);
      }
      if (proc_GetCurrentYear () == proc_GetStopYear ())
      {
        fprintf (stdout, "\n");
        fflush (stdout);
      }
    }

    if (scen_GetLogFlag ())
    {
      if (scen_GetLogProcessingStatusFlag () > 1)
      {
        scen_Append2Log ();
        fprintf (scen_GetLogFP (), " %u", proc_GetCurrentYear ());
        if (((proc_GetCurrentYear () + 1) % 10) == 0)
        {
          fprintf (scen_GetLogFP (), "\n");
          fflush (scen_GetLogFP ());
        }
        if (proc_GetCurrentYear () == proc_GetStopYear ())
        {
          fprintf (scen_GetLogFP (), "\n");
          fflush (scen_GetLogFP ());
        }
        scen_CloseLog ();
      }
    }

    /* 
     *
     * APPLY THE CELLULAR AUTOMATON RULES FOR THIS YEAR 
     *
     */
    sng = 0;
    sdg = 0;
    sdc = 0;
    og = 0;
    rt = 0;
    pop = 0;
    timer_Start (SPREAD_TOTAL_TIME);
    spr_spread (&average_slope,
                &num_growth_pix,
                &sng,
                &sdc,
                &og,
                &rt,
                &pop,
                z_ptr);
    timer_Stop (SPREAD_TOTAL_TIME);
    stats_SetSNG (sng);
    stats_SetSDG (sdg);
    stats_SetSDG (sdc);
    stats_SetOG (og);
    stats_SetRT (rt);
    stats_SetPOP (pop);

    if (scen_GetViewGrowthTypesFlag ())
    {
      sprintf (gif_filename, "%sz_growth_types_%u_%u_%u.gif",
               scen_GetOutputDir (), proc_GetCurrentRun (),
               proc_GetCurrentMonteCarlo (), proc_GetCurrentYear ());
      sprintf (date_str, "%u", proc_GetCurrentYear ());
      gdif_WriteGIF (z_ptr,
                     color_GetColortable (GROWTH_COLORTABLE),
                     gif_filename,
                     date_str,
                     255);
    }

    if (scen_GetDoingLanduseFlag ())
    {
      grw_landuse (land1_ptr, num_growth_pix);
    }
    else
    {
      grw_non_landuse (z_ptr);
    }
    seed_ptr = igrid_GetUrbanGridPtr (__FILE__, func, __LINE__, 0);
    util_condition_gif (total_pixels,
                        seed_ptr,
                        GT,
                        0,
                        z_ptr,
                        PHASE0G);
    seed_ptr = igrid_GridRelease (__FILE__, func, __LINE__, seed_ptr);

    /*
     *
     * DO STATISTICS
     *
     */
    stats_Update (num_growth_pix);

    /*
     *
     * DO SELF MODIFICATION
     *
     */
    coeff_SelfModication (stats_GetGrowthRate (), stats_GetPercentUrban ());

    coeff_WriteCurrentCoeff ();
  }
  timer_Stop (GRW_GROWTH);
  FUNC_END;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grw_landuse_init
** PURPOSE:       initial variables for doing landuse
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  void
  grw_landuse_init (GRID_P deltatron_ptr,                    /* OUT    */
                    GRID_P land1_ptr)                      /* OUT    */
{
  char func[] = "grw_landuse_init";
  int i;
  int total_pixels;
  GRID_P landuse0_ptr;
  GRID_P landuse1_ptr;


  FUNC_INIT;

  total_pixels = mem_GetTotalPixels ();
  assert (deltatron_ptr != NULL);
  assert (land1_ptr != NULL);
  assert (total_pixels > 0);
  /*
   *
   * INITIALIZE DELTATRON GRID TO ZERO
   *
   */
  for (i = 0; i < total_pixels; i++)
  {
    deltatron_ptr[i] = 0;
  }
  /*
   *
   * IF PREDICTING USE LANDUSE 1 AS THE STARTING LANDUSE
   * ELSE USE LANDUSE 0 AS THE STARTING LANDUSE
   *
   */
  if (proc_GetProcessingType () == PREDICTING)
  {
    landuse1_ptr = igrid_GetLanduseGridPtr (__FILE__, func, __LINE__, 1);
    assert (landuse1_ptr != NULL);
    for (i = 0; i < total_pixels; i++)
    {
      land1_ptr[i] = landuse1_ptr[i];
    }
    landuse1_ptr = igrid_GridRelease (__FILE__, func, __LINE__, landuse1_ptr);
  }
  else
  {
    landuse0_ptr = igrid_GetLanduseGridPtr (__FILE__, func, __LINE__, 0);
    assert (landuse0_ptr != NULL);
    for (i = 0; i < total_pixels; i++)
    {
      land1_ptr[i] = landuse0_ptr[i];
    }
    landuse0_ptr = igrid_GridRelease (__FILE__, func, __LINE__, landuse0_ptr);
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grw_landuse
** PURPOSE:       routine for handling landuse type of processing
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  void
  grw_landuse (
                GRID_P land1_ptr,                            /* OUT    */
                int num_growth_pix                           /* IN     */
  )

{

  char func[] = "grw_landuse";
  char gif_filename[MAX_FILENAME_LEN];
  char date_str[5];
  int ticktock;
  int landuse0_year;
  int landuse1_year;
  int urban_code;
  int *new_indices;
  Classes *landuse_classes;
  Classes *class_indices;
  GRID_P background_ptr;
  GRID_P grw_landuse_ws1;
  GRID_P deltatron_ptr;
  GRID_P z_ptr;
  GRID_P deltatron_workspace1;
  GRID_P slp_ptr;
  GRID_P land2_ptr;
  double *class_slope;
  double *ftransition;


  FUNC_INIT;
  ticktock = proc_GetCurrentYear ();
  landuse0_year = igrid_GetLanduseYear (0);
  landuse1_year = igrid_GetLanduseYear (1);
  urban_code = landclass_GetUrbanCode ();
  new_indices = landclass_GetNewIndicesPtr ();
  landuse_classes = landclass_GetClassesPtr ();
  class_indices = landclass_GetReducedClassesPtr ();
  background_ptr = igrid_GetBackgroundGridPtr (__FILE__, func, __LINE__);
  grw_landuse_ws1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  deltatron_workspace1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  slp_ptr = igrid_GetSlopeGridPtr (__FILE__, func, __LINE__);
  deltatron_ptr = pgrid_GetDeltatronPtr ();
  z_ptr = pgrid_GetZPtr ();
  land2_ptr = pgrid_GetLand2Ptr ();
  class_slope = trans_GetClassSlope ();
  ftransition = trans_GetFTransition ();

  assert (ticktock >= 0);
  assert (z_ptr != NULL);
  assert (urban_code > 0);
  assert (new_indices != NULL);
  assert (landuse_classes != NULL);
  assert (class_indices != NULL);
  assert (grw_landuse_ws1 != NULL);
  assert (deltatron_workspace1 != NULL);
  assert (deltatron_ptr != NULL);
  assert (land1_ptr != NULL);
  assert (land2_ptr != NULL);
  assert (slp_ptr != NULL);
  assert (class_slope != NULL);
  assert (ftransition != NULL);

  /* influence land use */
  if (ticktock >= landuse0_year)
  {

    /*
     *
     * PLACE THE NEW URBAN SIMULATION INTO THE LAND USE IMAGE
     *
     */
    util_condition_gif (mem_GetTotalPixels (),
                        z_ptr,
                        GT,
                        0,
                        land1_ptr,
                        urban_code);

    delta_deltatron (new_indices,                            /* IN     */
                     landuse_classes,                        /* IN     */
                     class_indices,                          /* IN     */
                     deltatron_workspace1,                   /* MOD    */
                     deltatron_ptr,                          /* IN/OUT */
                     land1_ptr,                              /* IN     */
                     land2_ptr,                              /* OUT    */
                     slp_ptr,                                /* IN     */
                     num_growth_pix,                         /* IN     */
                     class_slope,                            /* IN     */
                     ftransition);                         /* IN     */

    /*
     *
     * SWITCH THE OLD AND THE NEW
     *
     */
    util_copy_grid (land2_ptr,
                    land1_ptr);
  }

  if ((proc_GetProcessingType () == PREDICTING) ||
      (proc_GetProcessingType () == TESTING) &&
      (proc_GetLastMonteCarloFlag ()))
  {
    /*
     *
     * WRITE LAND1 GIF TO FILE
     *
     */

    sprintf (gif_filename, "%s%s_land_n_urban.%u.gif",
     scen_GetOutputDir (), igrid_GetLocation (), proc_GetCurrentYear ());
    sprintf (date_str, "%u", proc_GetCurrentYear ());
    gdif_WriteGIF (land1_ptr,
                   color_GetColortable (LANDUSE_COLORTABLE),
                   gif_filename,
                   date_str,
                   255);
  }

  /*
   *
   * COMPUTE FINAL MATCH STATISTIC FOR LANDUSE
   *
   */
  if (proc_GetCurrentYear () == landuse1_year)
  {

    util_condition_gif (mem_GetTotalPixels (),
                        z_ptr,
                        GT,
                        0,
                        land1_ptr,
                        urban_code);
  }
  background_ptr =
    igrid_GridRelease (__FILE__, func, __LINE__, background_ptr);
  grw_landuse_ws1 =
    mem_GetWGridFree (__FILE__, func, __LINE__, grw_landuse_ws1);
  deltatron_workspace1 =
    mem_GetWGridFree (__FILE__, func, __LINE__, deltatron_workspace1);
  slp_ptr = igrid_GridRelease (__FILE__, func, __LINE__, slp_ptr);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grw_non_landuse
** PURPOSE:       routine for handling non landuse processing
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  grw_non_landuse (GRID_P z_ptr)
{
  char func[] = "grw_non_landuse";
  char command[2 * MAX_FILENAME_LEN + 20];
  GRID_P workspace1;
  GRID_P workspace2;
  int num_monte_carlo;
  char name[] = "_urban_";
  char gif_filename[MAX_FILENAME_LEN];
  GRID_P cumulate_monte_carlo;
  int i;

  FUNC_INIT;
  workspace1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  workspace2 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  num_monte_carlo = scen_GetMonteCarloIterations ();

  assert (workspace1 != NULL);
  assert (workspace2 != NULL);
  assert (z_ptr != NULL);


  cumulate_monte_carlo = workspace1;

  if (proc_GetProcessingType () != CALIBRATING)
  {
    if (proc_GetCurrentMonteCarlo () == 0)
    {
      /*
       *
       * ZERO OUT THE ACCUMULATION GRID
       *
       */

      util_init_grid (cumulate_monte_carlo, 0);
    }
    else
    {
      /*
       *
       * READ IN THE ACCUMULATION GRID
       *
       */
      sprintf (gif_filename, "%scumulate_monte_carlo.year_%u",
               scen_GetOutputDir (), proc_GetCurrentYear ());
      inp_slurp (gif_filename,                               /* IN    */
                 cumulate_monte_carlo,                       /* OUT   */
                 memGetBytesPerGridRound ());              /* IN    */
    }
    /*
     *
     * ACCUMULATE Z OVER MONTE CARLOS
     *
     */
    for (i = 0; i < mem_GetTotalPixels (); i++)
    {
      if (z_ptr[i] > 0)
      {
        cumulate_monte_carlo[i]++;
      }
    }


    if (proc_GetCurrentMonteCarlo () == num_monte_carlo - 1)
    {
      if (proc_GetProcessingType () == TESTING)
      {
        util_condition_gif (mem_GetTotalPixels (),           /* IN     */
                            z_ptr,                           /* IN     */
                            GT,                              /* IN     */
                            0,                               /* IN     */
                            cumulate_monte_carlo,            /* IN/OUT */
                            100);                          /* IN     */
      }
      else
      {

        /*
         *
         * NORMALIZE ACCULUMLATED GRID
         *
         */
        for (i = 0; i < mem_GetTotalPixels (); i++)
        {
          cumulate_monte_carlo[i] =
            100 * cumulate_monte_carlo[i] / num_monte_carlo;
        }
      }
      util_WriteZProbGrid (cumulate_monte_carlo, name);
      if (proc_GetCurrentMonteCarlo () != 0)
      {
        sprintf (command, "rm %s", gif_filename);
        system (command);
      }
    }
    else
    {
      /*
       *
       * DUMP ACCULUMLATED GRID TO DISK
       *
       */
      sprintf (gif_filename, "%scumulate_monte_carlo.year_%u",
               scen_GetOutputDir (), proc_GetCurrentYear ());
      out_dump (gif_filename,
                cumulate_monte_carlo,
                memGetBytesPerGridRound ());
    }
  }

  workspace1 = mem_GetWGridFree (__FILE__, func, __LINE__, workspace1);
  workspace2 = mem_GetWGridFree (__FILE__, func, __LINE__, workspace2);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grw_completion_status
** PURPOSE:       write completion status on FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  grw_completion_status (FILE * fp)
{
  int total_mc;
  int total_mc_executed;
  int wd1;
  int wd2;
  char buf1[25];
  char buf2[25];
  char buf3[25];
  char buf4[25];
  float complete;
  float elapsed_sec;
  float est_execution_time;
  float est_remaining_sec;
  char elapsed_sec_f[15];
  char est_remaining_sec_f[15];

  total_mc =
    (scen_GetMonteCarloIterations () * proc_GetTotalRuns ()) / glb_npes;
  total_mc_executed = scen_GetMonteCarloIterations () *
    proc_GetNumRunsExecThisCPU () + proc_GetCurrentMonteCarlo ();
  complete = (float) total_mc_executed / (float) total_mc;
  complete = MIN (complete, 1.0);
  elapsed_sec = timer_Read (TOTAL_TIME) / 1000;
  est_remaining_sec = 0.0;

  sprintf (buf1, "%15u", proc_GetTotalRuns ());
  sprintf (buf2, "%15u", proc_GetCurrentRun ());
  util_trim (buf1);
  wd1 = strlen (buf1);

  sprintf (buf3, "%15u", scen_GetMonteCarloIterations ());
  sprintf (buf4, "%15u", proc_GetCurrentMonteCarlo ());
  util_trim (buf3);
  wd2 = strlen (buf3);

#if 1
  fprintf (fp, "%s %u Run= %s of %s MC=%s of %s ",
           __FILE__, __LINE__,
           &buf2[15 - wd1], buf1,
           &buf4[15 - wd2], buf3);
#else
  fprintf (fp, "%s %u Run= %u of %u MC=%u of %u ",
           __FILE__, __LINE__,
           proc_GetCurrentRun (), proc_GetTotalRuns (),
           proc_GetCurrentMonteCarlo (), scen_GetMonteCarloIterations ());
#endif

  if (complete > 0.0)
  {
    est_execution_time = elapsed_sec / complete;
    est_remaining_sec = (est_execution_time - elapsed_sec);
    timer_Format (elapsed_sec_f, (unsigned int) elapsed_sec);
    timer_Format (est_remaining_sec_f, (unsigned int) est_remaining_sec);
    fprintf (fp,
             "%7.3f%% complete; Elapsed=%s ; ETC=%s\n",
             100.0 * complete, elapsed_sec_f,
             est_remaining_sec_f);
  }
  else
  {
    fprintf (fp, "\n");
  }
}
