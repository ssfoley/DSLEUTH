#define DRIVER_MODULE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "igrid_obj.h"
#include "pgrid_obj.h"
#include "landclass_obj.h"
#include "color_obj.h"
#include "coeff_obj.h"
#include "utilities.h"
#include "memory_obj.h"
#include "scenario_obj.h"
#include "transition_obj.h"
#include "ugm_macros.h"
#include "ugm_defines.h"
#include "proc_obj.h"
#include "gdif_obj.h"
#include "growth.h"
#include "stats_obj.h"
#include "timer_obj.h"
#include "color_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                            TYPEDEFS                                       **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
typedef double fmatch_t;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static
  void drv_monte_carlo (GRID_P z_cumulate_ptr, GRID_P sim_landuse_ptr);

static
  fmatch_t drv_fmatch (GRID_P cum_probability_ptr,
                       GRID_P landuse1_ptr);

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char driver_c_sccs_id[] = "@(#)driver.c	1.629	12/4/00";


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: drv_driver
** PURPOSE:       main function for driving the simulation grw_growth()
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  drv_driver ()
{
  char func[] = "drv_driver";
  char name[] = "_cumcolor_urban_";
  GRID_P sim_landuse_ptr;
  GRID_P z_cumulate_ptr;
  GRID_P cum_probability_ptr;
  GRID_P cum_uncertainty_ptr;
  GRID_P landuse1_ptr;
  int total_pixels;
  char filename[256];
  fmatch_t fmatch = 0.0;
  struct colortable *colortable_ptr;

  FUNC_INIT;
  timer_Start (DRV_DRIVER);
  total_pixels = mem_GetTotalPixels ();
  z_cumulate_ptr = pgrid_GetCumulatePtr ();
  sim_landuse_ptr = pgrid_GetLand1Ptr ();

  assert (total_pixels > 0);
  assert (z_cumulate_ptr != NULL);
  assert (sim_landuse_ptr != NULL);


  /*
   *
   * CREATE ANNUAL LANDUSE PROBABILITY FILE
   *
   */
  if (proc_GetProcessingType () == PREDICTING)
  {
    if (scen_GetDoingLanduseFlag ())
    {
      landclass_AnnualProbInit ();
    }
  }

  /*
   *
   * MONTE CARLO SIMULATION
   *
   */
  drv_monte_carlo (z_cumulate_ptr, sim_landuse_ptr);

  if (proc_GetProcessingType () == PREDICTING)
  {
    /*
     *
     * OUTPUT URBAN IMAGES
     *
     */
    sprintf (filename, "%scumulate_urban.gif", scen_GetOutputDir ());
    colortable_ptr = color_GetColortable (GRAYSCALE_COLORTABLE);
    gdif_WriteGIF (z_cumulate_ptr,
                   colortable_ptr,
                   filename,
                   "",
                   SEED_COLOR_INDEX);

    util_WriteZProbGrid (z_cumulate_ptr, name);

    if (scen_GetDoingLanduseFlag ())
    {
      cum_probability_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
      cum_uncertainty_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
      assert (cum_probability_ptr != NULL);
      assert (cum_uncertainty_ptr != NULL);

      landclass_BuildProbImage (cum_probability_ptr, cum_uncertainty_ptr);

      /*
       *
       * OUTPUT CUMULATIVE PROBABILITY IMAGE
       *
       */
      sprintf (filename, "%scumcolor_landuse.gif", scen_GetOutputDir ());
      colortable_ptr = color_GetColortable (LANDUSE_COLORTABLE);
      assert (colortable_ptr != NULL);
      gdif_WriteGIF (cum_probability_ptr,
                     colortable_ptr,
                     filename,
                     "",
                     SEED_COLOR_INDEX);
      /*
       *
       * OUTPUT CUMULATIVE UNCERTAINTY IMAGE
       *
       */
      sprintf (filename, "%suncertainty.landuse.gif", scen_GetOutputDir ());
      colortable_ptr = color_GetColortable (GRAYSCALE_COLORTABLE);
      assert (colortable_ptr != NULL);
      gdif_WriteGIF (cum_uncertainty_ptr,
                     colortable_ptr,
                     filename,
                     "",
                     SEED_COLOR_INDEX);

      cum_probability_ptr = mem_GetWGridFree (__FILE__, func, __LINE__, cum_probability_ptr);
      cum_uncertainty_ptr = mem_GetWGridFree (__FILE__, func, __LINE__, cum_uncertainty_ptr);
    }
    /* end of:   if (scen_GetDoingLanduseFlag()) */
  }
  /* end of:    if(proc_GetProcessingType() == PREDICTING) */

  if ((!scen_GetDoingLanduseFlag ()) || (proc_GetProcessingType () == PREDICTING))
  {
    fmatch = 0.0;
  }
  else
  {
    landuse1_ptr = igrid_GetLanduseGridPtr (__FILE__, func, __LINE__, 1);
    fmatch = drv_fmatch (sim_landuse_ptr, landuse1_ptr);
    landuse1_ptr = igrid_GridRelease (__FILE__, func, __LINE__, landuse1_ptr);
  }

  stats_Analysis (fmatch);

  /* end of:  if(proc_GetProcessingType() == PREDICTING) */
  timer_Stop (DRV_DRIVER);

  /*
   *
   * kludge to stop the TOTAL_TIME timer in main.c from
   * overflowing
   *
   */
  timer_Stop (TOTAL_TIME);
  timer_Start (TOTAL_TIME);
  FUNC_END;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: drv_monte_carlo
** PURPOSE:       Monte Carlo loop
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  void
  drv_monte_carlo (GRID_P cumulate_ptr, GRID_P land1_ptr)
{
  char func[] = "drv_monte_carlo";
  int imc;
  int i;
  double *class_slope;
  double *ftransition;
  GRID_P z_ptr;
  int total_pixels;
  int num_monte_carlo;
  int *new_indices;

  FUNC_INIT;
  class_slope = trans_GetClassSlope ();
  ftransition = trans_GetFTransition ();
  z_ptr = pgrid_GetZPtr ();
  total_pixels = mem_GetTotalPixels ();
  new_indices = landclass_GetNewIndicesPtr ();
  num_monte_carlo = scen_GetMonteCarloIterations ();

  assert (total_pixels > 0);
  assert (land1_ptr != NULL);
  assert (z_ptr != NULL);
  assert (ftransition != NULL);
  assert (class_slope != NULL);
  assert (new_indices != NULL);
  assert (num_monte_carlo > 0);

  for (imc = 0; imc < scen_GetMonteCarloIterations (); imc++)
  {
    proc_SetCurrentMonteCarlo (imc);

    /*
     *
     * RESET THE PARAMETERS
     *
     */
    coeff_SetCurrentDiffusion (coeff_GetSavedDiffusion ());
    coeff_SetCurrentSpread (coeff_GetSavedSpread ());
    coeff_SetCurrentBreed (coeff_GetSavedBreed ());
    coeff_SetCurrentSlopeResist (coeff_GetSavedSlopeResist ());
    coeff_SetCurrentRoadGravity (coeff_GetSavedRoadGravity ());

    if (scen_GetLogFlag ())
    {
      if (scen_GetLogCoeffFlag ())
      {
        scen_Append2Log ();
        coeff_LogCurrent (scen_GetLogFP ());
        scen_CloseLog ();
      }
    }

    /*
     *
     * RUN SIMULATION
     *
     */
    stats_InitUrbanizationAttempts ();
    grw_grow (z_ptr, land1_ptr);
    if (scen_GetLogFlag ())
    {
      if (scen_GetLogUrbanizationAttemptsFlag ())
      {
        scen_Append2Log ();
        stats_LogUrbanizationAttempts (scen_GetLogFP ());
        scen_CloseLog ();
      }
    }

    /*
     *
     * UPDATE CUMULATE GRID
     *
     */
    for (i = 0; i < total_pixels; i++)
    {
      if (z_ptr[i] > 0)
      {
        cumulate_ptr[i]++;
      }
    }

    /*
     *
     * UPDATE ANNUAL LAND CLASS PROBABILITIES
     *
     */
    if (proc_GetProcessingType () == PREDICTING)
    {
      landclass_AnnualProbUpdate (land1_ptr);
    }

  }
  /*
   *
   * NORMALIZE CUMULATIVE URBAN IMAGE
   *
   */
  for (i = 0; i < total_pixels; i++)
  {
    cumulate_ptr[i] = (100.0 * cumulate_ptr[i]) / num_monte_carlo;
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: drv_fmatch
** PURPOSE:       calculate fmatch
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
    fmatch_t
  drv_fmatch (GRID_P cum_probability_ptr,
              GRID_P landuse1_ptr)
{
  char func[] = "drv_fmatch";
  int match_count;
  int trans_count;
  float fmatch;

  FUNC_INIT;
  assert (cum_probability_ptr != NULL);
  if (scen_GetDoingLanduseFlag ())
  {
    assert (landuse1_ptr != NULL);
  }

  if (!scen_GetDoingLanduseFlag ())
  {
    fmatch = 1.0;
  }
  else
  {
    match_count = util_img_intersection (mem_GetTotalPixels (),
                                         cum_probability_ptr,
                                         landuse1_ptr);
    trans_count = mem_GetTotalPixels () - match_count;

    if ((match_count == 0) && (trans_count == 0))
    {
      fmatch = 0.0;
    }
    else
    {
      fmatch = (float) match_count / (match_count + trans_count);
    }
  }
  FUNC_END;
  return fmatch;
}
