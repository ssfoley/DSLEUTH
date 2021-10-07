#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "random.h"
#include "utilities.h"
#include "memory_obj.h"
#include "igrid_obj.h"
#include "ugm_macros.h"
#include "coeff_obj.h"
#include "timer_obj.h"
#include "proc_obj.h"
#include "scenario_obj.h"
#include "stats_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define SPREAD_MODULE
#define SWGHT_TYPE float
#define SLOPE_WEIGHT_ARRAY_SZ 256

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void
    spr_LogSlopeWeights (FILE * fp, int array_size, SWGHT_TYPE * lut);
static void
    spr_spiral (int index,                                   /* IN     */
                int *i_out,                                  /* OUT    */
                int *j_out);                               /* OUT    */
static void
    spr_phase1n3 (COEFF_TYPE diffusion_coefficient,          /* IN     */
                  COEFF_TYPE breed_coefficient,              /* IN     */
                  GRID_P z,                                  /* IN     */
                  GRID_P delta,                              /* IN/OUT */
                  GRID_P slp,                                /* IN     */
                  GRID_P excld,                              /* IN     */
                  SWGHT_TYPE * swght,                        /* IN     */
                  int *sng,                                  /* IN/OUT */
                  int *sdc);                               /* IN/OUT */

static void
    spr_phase4 (COEFF_TYPE spread_coefficient,               /* IN     */
                GRID_P z,                                    /* IN     */
                GRID_P excld,                                /* IN     */
                GRID_P delta,                                /* IN/OUT */
                GRID_P slp,                                  /* IN     */
                SWGHT_TYPE * swght,                          /* IN     */
                int *og);                                  /* IN/OUT */


static void
    spr_phase5 (COEFF_TYPE road_gravity,                     /* IN     */
                COEFF_TYPE diffusion_coefficient,            /* IN     */
                COEFF_TYPE breed_coefficient,                /* IN     */
                GRID_P z,                                    /* IN     */
                GRID_P delta,                                /* IN/OUT */
                GRID_P slp,                                  /* IN     */
                GRID_P excld,                                /* IN     */
                GRID_P roads,                                /* IN     */
                SWGHT_TYPE * swght,                          /* IN     */
                int *rt,                                     /* IN/OUT */
                GRID_P workspace);                         /* MOD    */
static void
    spr_get_slp_weights (int array_size,                     /* IN     */
                         SWGHT_TYPE * lut);                /* OUT    */
static BOOLEAN spr_road_search (int i_grwth_center,          /* IN     */
                                int j_grwth_center,          /* IN     */
                                int *i_road,                 /* OUT    */
                                int *j_road,                 /* OUT    */
                                int max_search_index,        /* IN     */
                                GRID_P roads);             /* IN     */
static
  BOOLEAN spr_road_walk (int i_road_start,                   /* IN     */
                         int j_road_start,                   /* IN     */
                         int *i_road_end,                    /* OUT    */
                         int *j_road_end,                    /* OUT    */
                         GRID_P roads,                       /* IN     */
                         double diffusion_coefficient);    /* IN     */
static
  BOOLEAN spr_urbanize_nghbr (int i,                         /* IN     */
                              int j,                         /* IN     */
                              int *i_nghbr,                  /* OUT    */
                              int *j_nghbr,                  /* OUT    */
                              GRID_P z,                      /* IN     */
                              GRID_P delta,                  /* IN     */
                              GRID_P slp,                    /* IN     */
                              GRID_P excld,                  /* IN     */
                              SWGHT_TYPE * swght,            /* IN     */
                              PIXEL pixel_value,             /* IN     */
                              int *stat);                  /* OUT    */
static
  void spr_get_neighbor (int i_in,                           /* IN     */
                         int j_in,                           /* IN     */
                         int *i_out,                         /* OUT    */
                         int *j_out);                      /* OUT    */

static BOOLEAN
    spr_urbanize (int row,                                   /* IN     */
                  int col,                                   /* IN     */
                  GRID_P z,                                  /* IN     */
                  GRID_P delta,                              /* IN     */
                  GRID_P slp,                                /* IN     */
                  GRID_P excld,                              /* IN     */
                  SWGHT_TYPE * swght,                        /* IN     */
                  PIXEL pixel_value,                         /* IN     */
                  int *stat);                              /* OUT    */

static COEFF_TYPE
    spr_GetDiffusionValue (COEFF_TYPE diffusion_coeff);    /* IN    */
static COEFF_TYPE
    spr_GetRoadGravValue (COEFF_TYPE rg_coeff);            /* IN    */

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char spread_c_sccs_id[] = "@(#)spread.c	1.427	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_phase1n3
** PURPOSE:       perform phase 1 & 3 growth types
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_phase1n3 (COEFF_TYPE diffusion_coefficient,            /* IN     */
                COEFF_TYPE breed_coefficient,                /* IN     */
                GRID_P z,                                    /* IN     */
                GRID_P delta,                                /* IN/OUT */
                GRID_P slp,                                  /* IN     */
                GRID_P excld,                                /* IN     */
                SWGHT_TYPE * swght,                          /* IN     */
                int *sng,                                    /* IN/OUT */
                int *sdc)                                  /* IN/OUT */
{
  char func[] = "spr_phase1n3";
  int i;
  int j;
  int i_out;
  int j_out;
  int k;
  int count;
  int tries;
  int max_tries;
  COEFF_TYPE diffusion_value;
  BOOLEAN urbanized;

  FUNC_INIT;
  assert (MIN_DIFFUSION_VALUE <= diffusion_coefficient);
  assert (diffusion_coefficient <= MAX_DIFFUSION_VALUE);
  assert (MIN_BREED_VALUE <= breed_coefficient);
  assert (breed_coefficient <= MAX_BREED_VALUE);
  assert (z != NULL);
  assert (delta != NULL);
  assert (slp != NULL);
  assert (excld != NULL);
  assert (swght != NULL);
  assert (sng != NULL);
  assert (sdc != NULL);

  diffusion_value = spr_GetDiffusionValue (diffusion_coefficient);

  for (k = 0; k < 1 + (int) diffusion_value; k++)
  {
    i = RANDOM_ROW;
    j = RANDOM_COL;

    if (INTERIOR_PT (i, j))
    {
      if (spr_urbanize (i,                                     /* IN     */
                        j,                                     /* IN     */
                        z,                                     /* IN     */
                        delta,                                 /* IN/OUT */
                        slp,                                   /* IN     */
                        excld,                                 /* IN     */
                        swght,                                 /* IN     */
                        PHASE1G,                               /* IN     */
                        sng))                              /* IN/OUT */
      {
        if (RANDOM_INT (101) < (int) breed_coefficient)
        {
          count = 0;
          max_tries = 8;
          for (tries = 0; tries < max_tries; tries++)
          {
            urbanized = FALSE;
            urbanized =
              spr_urbanize_nghbr (i,                         /* IN     */
                                  j,                         /* IN     */
                                  &i_out,                    /* OUT    */
                                  &j_out,                    /* OUT    */
                                  z,                         /* IN     */
                                  delta,                     /* IN/OUT */
                                  slp,                       /* IN     */
                                  excld,                     /* IN     */
                                  swght,                     /* IN     */
                                  PHASE3G,                   /* IN     */
                                  sdc);                    /* IN/OUT */
            if (urbanized)
            {
              count++;
              if (count == MIN_NGHBR_TO_SPREAD)
              {
                break;
              }
            }
          }
        }
      }
    }
  }

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_phase4
** PURPOSE:       perform phase 4 growth
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_phase4 (COEFF_TYPE spread_coefficient,                 /* IN     */
              GRID_P z,                                      /* IN     */
              GRID_P excld,                                  /* IN     */
              GRID_P delta,                                  /* IN/OUT */
              GRID_P slp,                                    /* IN     */
              SWGHT_TYPE * swght,                            /* IN     */
              int *og)                                     /* IN/OUT */
{
  char func[] = "spr_phase4";
  int row;
  int col;
  int row_nghbr;
  int col_nghbr;
  int pixel;
  int walkabout_row[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int walkabout_col[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  int urb_count;
  int nrows;
  int ncols;

  FUNC_INIT;
  assert (z != NULL);
  assert (excld != NULL);
  assert (delta != NULL);
  assert (slp != NULL);
  assert (swght != NULL);
  assert (og != NULL);

  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);

  /*
   *
   * LOOP OVER THE INTERIOR PIXELS LOOKING FOR URBAN FROM WHICH
   * TO PERFORM ORGANIC GROWTH
   *
   */
  for (row = 1; row < nrows - 1; row++)
  {
    for (col = 1; col < ncols - 1; col++)
    {

      /*
       *
       * IS THIS AN URBAN PIXEL AND DO WE PASS THE RANDOM 
       * SPREAD COEFFICIENT TEST
       *
       */
      if ((z[OFFSET (row, col)] > 0) &&
          (RANDOM_INT (101) < spread_coefficient))
      {
        /*
         * EXAMINE THE EIGHT CELL NEIGHBORS
         * SPREAD AT RANDOM IF AT LEAST TWO ARE URBAN
         * PIXEL ITSELF MUST BE URBAN (3)
         *
         */
        urb_count = util_count_neighbors (z, row, col, GT, 0);
        if ((urb_count >= 2) && (urb_count < 8))
        {
          pixel = RANDOM_INT (8);

          row_nghbr = row + walkabout_row[pixel];
          col_nghbr = col + walkabout_col[pixel];

          spr_urbanize (row_nghbr,                           /* IN     */
                        col_nghbr,                           /* IN     */
                        z,                                   /* IN     */
                        delta,                               /* IN/OUT */
                        slp,                                 /* IN     */
                        excld,                               /* IN     */
                        swght,                               /* IN     */
                        PHASE4G,                             /* IN     */
                        og);                               /* IN/OUT */
        }
      }
    }
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_phase5
** PURPOSE:       perform phase 5 growth
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_phase5 (COEFF_TYPE road_gravity,                       /* IN     */
              COEFF_TYPE diffusion_coefficient,              /* IN     */
              COEFF_TYPE breed_coefficient,                  /* IN     */
              GRID_P z,                                      /* IN     */
              GRID_P delta,                                  /* IN/OUT */
              GRID_P slp,                                    /* IN     */
              GRID_P excld,                                  /* IN     */
              GRID_P roads,                                  /* IN     */
              SWGHT_TYPE * swght,                            /* IN     */
              int *rt,                                       /* IN/OUT */
              GRID_P workspace)                            /* MOD    */

{
  char func[] = "spr_phase5";
  int iii;
  int int_road_gravity;
  int growth_count;
  int *growth_row;
  int *growth_col;
  int max_search_index;
  int growth_index;
  BOOLEAN road_found;
  int i_rd_start;
  int j_rd_start;
  int max_tries;
  BOOLEAN spread;
  BOOLEAN urbanized;
  int i_rd_end;
  int j_rd_end;
  int i_rd_end_nghbr;
  int j_rd_end_nghbr;
  int i_rd_end_nghbr_nghbr;
  int j_rd_end_nghbr_nghbr;
  int tries;
  int nrows;
  int ncols;
  int total_pixels;

  FUNC_INIT;
  assert (road_gravity > 0.0);
  assert (diffusion_coefficient > 0.0);
  assert (breed_coefficient > 0.0);
  assert (z != NULL);
  assert (delta != NULL);
  assert (slp != NULL);
  assert (excld != NULL);
  assert (roads != NULL);
  assert (swght != NULL);
  assert (rt != NULL);
  assert (workspace != NULL);
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);


  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);
  /*
   *
   * SET UP WORKSPACE
   *
   */
  growth_row = (int *) workspace;
  growth_col = (int *) workspace + (nrows);
  /*
   *
   * DETERMINE THE TOTAL GROWTH COUNT AND SAVE THE
   * ROW AND COL LOCATIONS OF THE NEW GROWTH
   *
   */
  growth_count = 0;
#ifdef CRAY_C90
#pragma _CRI ivdep
#endif
  for (iii = 0; iii < total_pixels; iii++)
  {
    if (delta[iii] > 0)
    {
      growth_row[growth_count] = iii / ncols;
      growth_col[growth_count] = iii % ncols;
      growth_count++;
    }
  }
  /*
   *
   * PHASE 5:  ROAD TRIPS
   * IF THERE IS NEW GROWTH, BEGIN PROCESSING ROAD TRIPS
   *
   */
  if (growth_count > 0)
  {
    for (iii = 0; iii < 1 + (int) (breed_coefficient); iii++)
    {
      /*
       *
       * DETERMINE THE MAX INDEX INTO THE GLB_RD_SEARCH_INDICES ARRAY
       * for road_gravity of 1 we have  8 values
       * for road_gravity of 2 we have 16 values
       * for road_gravity of 3 we have 24 values
       *    and so on....
       *
       * if we need to cover N road_gravity values, then total number of 
       * indexed values would be
       * 8 + 16 + 24 + ... + 8*N = 8*(1+2+3+...+N) = 8*(N(1+N))/2
       *
       */
      int_road_gravity = spr_GetRoadGravValue (road_gravity);
      max_search_index = 4 * (int_road_gravity * (1 + int_road_gravity));
      max_search_index = MAX (max_search_index, nrows);
      max_search_index = MAX (max_search_index, ncols);

      /*
       *
       * RANDOMLY SELECT A GROWTH PIXEL TO START SEARCH
       * FOR ROAD
       *
       */
      growth_index = (int) ((double) growth_count * RANDOM_FLOAT);

      /*
       *
       * SEARCH FOR ROAD ABOUT THIS GROWTH POINT
       *
       */
      road_found =
        spr_road_search (growth_row[growth_index],
                         growth_col[growth_index],
                         &i_rd_start,
                         &j_rd_start,
                         max_search_index,
                         roads);

      /*
       *
       * IF THERE'S A ROAD FOUND THEN WALK ALONG IT
       *
       */
      if (road_found)
      {
        spread = spr_road_walk (i_rd_start,                  /* IN     */
                                j_rd_start,                  /* IN     */
                                &i_rd_end,                   /* OUT    */
                                &j_rd_end,                   /* OUT    */
                                roads,                       /* IN     */
                                diffusion_coefficient);    /* IN     */

        if (spread == TRUE)
        {
          urbanized =
            spr_urbanize_nghbr (i_rd_end,                    /* IN     */
                                j_rd_end,                    /* IN     */
                                &i_rd_end_nghbr,             /* OUT    */
                                &j_rd_end_nghbr,             /* OUT    */
                                z,                           /* IN     */
                                delta,                       /* IN/OUT */
                                slp,                         /* IN     */
                                excld,                       /* IN     */
                                swght,                       /* IN     */
                                PHASE5G,                     /* IN     */
                                rt);                       /* IN/OUT */
          if (urbanized)
          {
            max_tries = 3;
            for (tries = 0; tries < max_tries; tries++)
            {
              urbanized =
                spr_urbanize_nghbr (i_rd_end_nghbr,          /* IN     */
                                    j_rd_end_nghbr,          /* IN     */
                                    &i_rd_end_nghbr_nghbr,   /* OUT    */
                                    &j_rd_end_nghbr_nghbr,   /* OUT    */
                                    z,                       /* IN     */
                                    delta,                   /* IN/OUT */
                                    slp,                     /* IN     */
                                    excld,                   /* IN     */
                                    swght,                   /* IN     */
                                    PHASE5G,                 /* IN     */
                                    rt);                   /* IN/OUT */

            }
          }
        }
      }
    }
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_get_slp_weights
** PURPOSE:       calculate the slope weights
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_get_slp_weights (int array_size,                       /* IN     */
                       SWGHT_TYPE * lut)                   /* OUT    */
{
  char func[] = "spr_get_slp_weights";
  float val;
  float exp;
  int i;

  FUNC_INIT;
  assert (lut != NULL);

  exp = coeff_GetCurrentSlopeResist () / (MAX_SLOPE_RESISTANCE_VALUE / 2.0);
  for (i = 0; i < array_size; i++)
  {
    if (i < scen_GetCriticalSlope ())
    {
      val = (scen_GetCriticalSlope () - (SWGHT_TYPE) i) / scen_GetCriticalSlope ();
      lut[i] = 1.0 - pow (val, exp);
    }
    else
    {
      lut[i] = 1.0;
    }
  }
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogSlopeWeightsFlag ())
    {
      scen_Append2Log ();
      spr_LogSlopeWeights (scen_GetLogFP (), array_size, lut);
      scen_CloseLog ();
    }
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_LogSlopeWeights
** PURPOSE:       log slope weights to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_LogSlopeWeights (FILE * fp, int array_size, SWGHT_TYPE * lut)
{
  int i;

  assert (fp != NULL);
  assert (array_size > 0);
  assert (lut != NULL);

  fprintf (fp, "\n%s %5u ***** LOG OF SLOPE WEIGHTS *****\n",
           __FILE__, __LINE__);
  fprintf (fp, "%s %5u CRITICAL_SLOPE= %f\n",
           __FILE__, __LINE__, scen_GetCriticalSlope ());
  fprintf (fp, "%s %5u coeff_GetCurrentSlopeResist= %f\n",
           __FILE__, __LINE__, coeff_GetCurrentSlopeResist ());
  fprintf (fp, "%s %5u MAX_SLOPE_RESISTANCE_VALUE= %f\n",
           __FILE__, __LINE__, MAX_SLOPE_RESISTANCE_VALUE);
  for (i = 0; i < array_size; i++)
  {
    if (i < scen_GetCriticalSlope ())
    {
      fprintf (fp, "%s %5u lut[%3u]= %f\n",
               __FILE__, __LINE__, i, lut[i]);
    }
  }
  fprintf (fp, "All values other values to lut[%u] = 1.000000\n", array_size);

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_GetDiffusionValue
** PURPOSE:       calculate the diffusion value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static COEFF_TYPE
  spr_GetDiffusionValue (COEFF_TYPE diffusion_coeff)
{

  COEFF_TYPE diffusion_value;
  double rows_sq;
  double cols_sq;

  rows_sq = igrid_GetNumRows () * igrid_GetNumRows ();
  cols_sq = igrid_GetNumCols () * igrid_GetNumCols ();

  /*
   * diffusion_value's MAXIMUM (IF diffusion_coeff == 100)
   * WILL BE 5% OF THE IMAGE DIAGONAL. 
   */

  diffusion_value = ((diffusion_coeff * 0.005) * sqrt (rows_sq + cols_sq));
  return diffusion_value;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_GetRoadGravValue
** PURPOSE:       calculate the road gravity value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static COEFF_TYPE
  spr_GetRoadGravValue (COEFF_TYPE rg_coeff)
{

  int rg_value;
  int row;
  int col;

  row = igrid_GetNumRows ();
  col = igrid_GetNumCols ();

  /*
   * rg_value's MAXIMUM (IF rg_coeff == 100)
   * WILL BE 1/16 OF THE IMAGE DIMENSIONS. 
   */

  rg_value = (rg_coeff / MAX_ROAD_VALUE) * ((row + col) / 16.0);

  return rg_value;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_urbanize
** PURPOSE:       try to urbanize a pixel
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static BOOLEAN
  spr_urbanize (int row,                                     /* IN     */
                int col,                                     /* IN     */
                GRID_P z,                                    /* IN     */
                GRID_P delta,                                /* IN/OUT */
                GRID_P slp,                                  /* IN     */
                GRID_P excld,                                /* IN     */
                SWGHT_TYPE * swght,                          /* IN     */
                PIXEL pixel_value,                           /* IN     */
                int *stat)                                 /* IN/OUT */
{
  char func[] = "spr_urbanize";
  BOOLEAN val;
  int nrows;
  int ncols;

  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);

  FUNC_INIT;
  assert ((row >= 0) && (row < nrows));
  assert ((col >= 0) && (col < ncols));
  assert (z != NULL);
  assert (delta != NULL);
  assert (slp != NULL);
  assert (excld != NULL);
  assert (swght != NULL);
  assert (stat != NULL);


  val = FALSE;
  if (z[OFFSET ((row), (col))] == 0)
  {
    if (delta[OFFSET ((row), (col))] == 0)
    {
      if (RANDOM_FLOAT > swght[slp[OFFSET ((row), (col))]])
      {
        if (excld[OFFSET ((row), (col))] < RANDOM_INT (100))
        {
          val = TRUE;
          delta[OFFSET (row, col)] = pixel_value;
          (*stat)++;
          stats_IncrementUrbanSuccess ();
        }
        else
        {
          stats_IncrementEcludedFailure ();
        }
      }
      else
      {
        stats_IncrementSlopeFailure ();
      }
    }
    else
    {
      stats_IncrementDeltaFailure ();
    }
  }
  else
  {
    stats_IncrementZFailure ();
  }

  FUNC_END;


  return val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_get_neighbor
** PURPOSE:       find a neighboring pixel
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  void
  spr_get_neighbor (int i_in,                                /* IN     */
                    int j_in,                                /* IN     */
                    int *i_out,                              /* OUT    */
                    int *j_out)                            /* OUT    */
{
  char func[] = "spr_get_neighbor";
  int i;
  int j;
  int k;
  int nrows;
  int ncols;

  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);

  FUNC_INIT;
  assert (nrows > i_in);
  assert (ncols > j_in);
  assert (0 <= i_in);
  assert (0 <= j_in);
  assert (i_out != NULL);
  assert (j_out != NULL);

  util_get_next_neighbor (i_in, j_in, i_out, j_out, RANDOM_INT (8));
  for (k = 0; k < 8; k++)
  {
    i = (*i_out);
    j = (*j_out);
    if (IMAGE_PT (i, j))
    {
      break;
    }
    util_get_next_neighbor (i_in, j_in, i_out, j_out, -1);
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_urbanize_nghbr
** PURPOSE:       try to urbanize a neighbor
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
    BOOLEAN
  spr_urbanize_nghbr (int i,                                 /* IN     */
                      int j,                                 /* IN     */
                      int *i_nghbr,                          /* OUT    */
                      int *j_nghbr,                          /* OUT    */
                      GRID_P z,                              /* IN     */
                      GRID_P delta,                          /* IN/OUT */
                      GRID_P slp,                            /* IN     */
                      GRID_P excld,                          /* IN     */
                      SWGHT_TYPE * swght,                    /* IN     */
                      PIXEL pixel_value,                     /* IN     */
                      int *stat)                           /* IN/OUT */
{
  char func[] = "spr_urbanize_nghbr";
  BOOLEAN status = FALSE;

  FUNC_INIT;
  assert (i_nghbr != NULL);
  assert (j_nghbr != NULL);
  assert (z != NULL);
  assert (delta != NULL);
  assert (slp != NULL);
  assert (excld != NULL);
  assert (swght != NULL);
  assert (stat != NULL);

  if (IMAGE_PT (i, j))
  {
    spr_get_neighbor (i,                                     /* IN    */
                      j,                                     /* IN    */
                      i_nghbr,                               /* OUT   */
                      j_nghbr);                            /* OUT   */

    status = spr_urbanize ((*i_nghbr),                       /* IN     */
                           (*j_nghbr),                       /* IN     */
                           z,                                /* IN     */
                           delta,                            /* IN/OUT */
                           slp,                              /* IN     */
                           excld,                            /* IN     */
                           swght,                            /* IN     */
                           pixel_value,                      /* IN     */
                           stat);                          /* IN/OUT */
  }
  FUNC_END;

  return status;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_road_walk
** PURPOSE:       perform road walk
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
    BOOLEAN
  spr_road_walk (int i_road_start,                           /* IN     */
                 int j_road_start,                           /* IN     */
                 int *i_road_end,                            /* OUT    */
                 int *j_road_end,                            /* OUT    */
                 GRID_P roads,                               /* IN     */
                 double diffusion_coefficient)             /* IN     */
{
  char func[] = "spr_road_walk";
  int i;
  int j;
  int i_nghbr;
  int j_nghbr;
  int k;
  BOOLEAN end_of_road;
  BOOLEAN spread = FALSE;
  int run_value;
  int run = 0;

  FUNC_INIT;
  assert (i_road_end != NULL);
  assert (j_road_end != NULL);
  assert (roads != NULL);

  i = i_road_start;
  j = j_road_start;
  end_of_road = FALSE;
  while (!end_of_road)
  {
    end_of_road = TRUE;
    util_get_next_neighbor (i, j, &i_nghbr, &j_nghbr, RANDOM_INT (8));
    for (k = 0; k < 8; k++)
    {
      if (IMAGE_PT (i_nghbr, j_nghbr))
      {
        if (roads[OFFSET (i_nghbr, j_nghbr)])
        {
          end_of_road = FALSE;
          run++;
          i = i_nghbr;
          j = j_nghbr;
          break;
        }
      }
      util_get_next_neighbor (i, j, &i_nghbr, &j_nghbr, -1);
    }
    run_value = (int) (roads[OFFSET (i, j)] / MAX_ROAD_VALUE *
                       diffusion_coefficient);
    if (run > run_value)
    {
      end_of_road = TRUE;
      spread = TRUE;
      (*i_road_end) = i;
      (*j_road_end) = j;
    }
  }
  FUNC_END;
  return spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_road_search
** PURPOSE:       perform road search
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
    BOOLEAN
  spr_road_search (int i_grwth_center,                       /* IN     */
                   int j_grwth_center,                       /* IN     */
                   int *i_road,                              /* OUT    */
                   int *j_road,                              /* OUT    */
                   int max_search_index,                     /* IN     */
                   GRID_P roads)                           /* IN     */
{
  char func[] = "spr_road_search";
  int i;
  int j;
  int i_offset;
  int j_offset;
  BOOLEAN road_found = FALSE;
  int srch_index;

  FUNC_INIT;
  assert (i_road != NULL);
  assert (j_road != NULL);
  assert (max_search_index >= 0);

  for (srch_index = 0; srch_index < max_search_index; srch_index++)
  {
    spr_spiral (srch_index, &i_offset, &j_offset);
    i = i_grwth_center + i_offset;
    j = j_grwth_center + j_offset;

    if (IMAGE_PT (i, j))
    {
      if (roads[OFFSET (i, j)])
      {
        road_found = TRUE;
        (*i_road) = i;
        (*j_road) = j;
        break;
      }
    }
  }

  FUNC_END;
  return road_found;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_spiral
** PURPOSE:       generate spiral search pattern
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  spr_spiral (int index,                                     /* IN     */
              int *i_out,                                    /* OUT    */
              int *j_out)                                  /* OUT    */
{
  char func[] = "spr_spiral";
  BOOLEAN bn_found;
  int i;
  int j;
  int bn;
  int bo;
  int total;
  int left_side_len;
  int right_side_len;
  int top_len;
  int bot_len;
  int range1;
  int range2;
  int range3;
  int range4;
  int region_offset;
  int nrows;
  int ncols;

  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);


  FUNC_INIT;
  assert (i_out != NULL);
  assert (j_out != NULL);

  bn_found = FALSE;
  for (bn = 1; bn < MAX (ncols, nrows); bn++)
  {
    total = 8 * ((1 + bn) * bn) / 2;
    if (total > index)
    {
      bn_found = TRUE;
      break;
    }
  }
  if (!bn_found)
  {
    sprintf (msg_buf, "Unable to find road search band, bn.");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  bo = index - 8 * ((bn - 1) * bn) / 2;
  left_side_len = right_side_len = bn * 2 + 1;
  top_len = bot_len = bn * 2 - 1;
  range1 = left_side_len;
  range2 = left_side_len + bot_len;
  range3 = left_side_len + bot_len + right_side_len;
  range4 = left_side_len + bot_len + right_side_len + top_len;
  if (bo < range1)
  {
    region_offset = bo % range1;
    i = -bn + region_offset;
    j = -bn;
  }
  else if (bo < range2)
  {
    region_offset = (bo - range1) % range2;
    i = bn;
    j = -bn + 1 + region_offset;
  }
  else if (bo < range3)
  {
    region_offset = (bo - range2) % range3;
    i = bn - region_offset;
    j = bn;
  }
  else if (bo < range4)
  {
    region_offset = (bo - range3) % range4;
    i = -bn;
    j = bn - 1 - region_offset;
  }
  else
  {
    sprintf (msg_buf, "Unable to calculate (i,j) for road search");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  *i_out = i;
  *j_out = j;
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: spr_spread
** PURPOSE:       main spread routine
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  spr_spread (
               float *average_slope,                         /* OUT    */
               int *num_growth_pix,                          /* OUT    */
               int *sng,
               int *sdc,
               int *og,
               int *rt,
               int *pop,
               GRID_P z                                      /* IN/OUT */
  )                                                        /* MOD    */
{
  char func[] = "Spread";
  GRID_P delta;
  int i;
  int total_pixels;
  int nrows;
  int ncols;
  double road_gravity;
  COEFF_TYPE diffusion_coefficient;
  COEFF_TYPE breed_coefficient;
  COEFF_TYPE spread_coefficient;
  GRID_P excld;
  GRID_P roads;
  GRID_P slp;
  GRID_P scratch_gif1;
  GRID_P scratch_gif3;
  SWGHT_TYPE swght[SLOPE_WEIGHT_ARRAY_SZ];

  road_gravity = coeff_GetCurrentRoadGravity ();
  diffusion_coefficient = coeff_GetCurrentDiffusion ();
  breed_coefficient = coeff_GetCurrentBreed ();
  spread_coefficient = coeff_GetCurrentSpread ();

  scratch_gif1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  scratch_gif3 = mem_GetWGridPtr (__FILE__, func, __LINE__);

  excld = igrid_GetExcludedGridPtr (__FILE__, func, __LINE__);
  roads = igrid_GetRoadGridPtrByYear (__FILE__, func,
                                      __LINE__, proc_GetCurrentYear ());
  slp = igrid_GetSlopeGridPtr (__FILE__, func, __LINE__);
  FUNC_INIT;
  assert (road_gravity > 0.0);
  assert (diffusion_coefficient > 0.0);
  assert (breed_coefficient > 0.0);
  assert (spread_coefficient > 0.0);
  assert (z != NULL);
  assert (excld != NULL);
  assert (roads != NULL);
  assert (slp != NULL);
  assert (scratch_gif1 != NULL);
  assert (scratch_gif3 != NULL);

  total_pixels = mem_GetTotalPixels ();
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();

  assert (total_pixels > 0);
  assert (nrows > 0);
  assert (ncols > 0);


  /*
   *
   * SET UP WORKSPACE
   *
   */
  delta = scratch_gif1;

  /*
   *
   * ZERO THE GROWTH ARRAY FOR THIS TIME PERIOD
   *
   */
  util_init_grid (delta, 0);

  /*
   *
   * GET SLOPE RATES
   *
   */
  spr_get_slp_weights (SLOPE_WEIGHT_ARRAY_SZ,                /* IN     */
                       swght);                             /* OUT    */

  /*
   *
   * PHASE 1N3 - SPONTANEOUS NEIGHBORHOOD GROWTH AND SPREADING
   *
   */


  timer_Start (SPR_PHASE1N3);
  spr_phase1n3 (diffusion_coefficient,                       /* IN     */
                breed_coefficient,                           /* IN     */
                z,                                           /* IN     */
                delta,                                       /* IN/OUT */
                slp,                                         /* IN     */
                excld,                                       /* IN     */
                swght,                                       /* IN     */
                sng,                                         /* IN/OUT */
                sdc);                                      /* IN/OUT */
  timer_Stop (SPR_PHASE1N3);

  /*
   *
   * PHASE 4 - ORGANIC GROWTH
   *
   */
  timer_Start (SPR_PHASE4);
  spr_phase4 (spread_coefficient,                            /* IN     */
              z,                                             /* IN     */
              excld,                                         /* IN     */
              delta,                                         /* IN/OUT */
              slp,                                           /* IN     */
              swght,                                         /* IN     */
              og);                                         /* IN/OUT */
  timer_Stop (SPR_PHASE4);

  /*
   *
   * PHASE 5 - ROAD INFLUENCE GROWTH
   *
   */
  timer_Start (SPR_PHASE5);
  spr_phase5 (road_gravity,                                  /* IN     */
              diffusion_coefficient,                         /* IN     */
              breed_coefficient,                             /* IN     */
              z,                                             /* IN     */
              delta,                                         /* IN/OUT */
              slp,                                           /* IN     */
              excld,                                         /* IN     */
              roads,                                         /* IN     */
              swght,                                         /* IN     */
              rt,                                            /* IN/OUT */
              scratch_gif3);                               /* MOD    */
  timer_Stop (SPR_PHASE5);

  util_condition_gif (total_pixels,                          /* IN     */
                      delta,                                 /* IN     */
                      GT,                                    /* IN     */
                      PHASE5G,                               /* IN     */
                      delta,                                 /* IN/OUT */
                      0);                                  /* IN     */

  util_condition_gif (total_pixels,                          /* IN     */
                      excld,                                 /* IN     */
                      GE,                                    /* IN     */
                      100,                                   /* IN     */
                      delta,                                 /* IN/OUT */
                      0);                                  /* IN     */

  /* now place growth array into current array */
  (*num_growth_pix) = 0;
  (*average_slope) = 0.0;

  for (i = 0; i < total_pixels; i++)
  {
    if ((z[i] == 0) && (delta[i] > 0))
    {
      /* new growth being placed into array */
      (*average_slope) += (float) slp[i];
      z[i] = delta[i];
      (*num_growth_pix)++;
    }
  }
  *pop = util_count_pixels (total_pixels, z, GE, PHASE0G);

  if (*num_growth_pix == 0)
  {
    *average_slope = 0.0;
  }
  else
  {
    *average_slope /= (float) *num_growth_pix;
  }

  roads = igrid_GridRelease (__FILE__, func, __LINE__, roads);
  excld = igrid_GridRelease (__FILE__, func, __LINE__, excld);
  slp = igrid_GridRelease (__FILE__, func, __LINE__, slp);
  scratch_gif1 = mem_GetWGridFree (__FILE__, func, __LINE__, scratch_gif1);
  scratch_gif3 = mem_GetWGridFree (__FILE__, func, __LINE__, scratch_gif3);
  FUNC_END;
}
