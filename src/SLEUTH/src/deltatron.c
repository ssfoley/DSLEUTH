#define DELTATRON_MODULE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "random.h"
#include "utilities.h"
#include "ugm_macros.h"
#include "memory_obj.h"
#include "timer_obj.h"
#include "proc_obj.h"
#include "gdif_obj.h"
#include "color_obj.h"
#include "scenario_obj.h"
#include "pgrid_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define TRANS_PROBABILITY1(arg1,arg2)                  \
    ftransition[TRANS_OFFSET(                          \
       new_indices[urban_land[OFFSET((arg1),(arg2))]], \
       new_indices[new_landuse])]

#define TRANS_PROBABILITY2(arg1,arg2,arg3,arg4)                    \
   ftransition[TRANS_OFFSET(                                       \
     new_indices[phase2_land[OFFSET((arg1),(arg2))]],              \
     new_indices[urban_land[OFFSET((arg3),(arg4))]])]*             \
     DELTA_PHASE2_SENSITIVITY


/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static
  void delta_phase1 (int drive,                              /* IN     */
                     GRID_P urban_land,                      /* IN     */
                     GRID_P phase1_land,                     /* OUT    */
                     GRID_P slope,                           /* IN     */
                     GRID_P deltatron,                       /* IN/OUT */
                     Classes * landuse_classes,              /* IN     */
                     Classes * class_indices,                /* IN     */
                     int *new_indices,                       /* IN     */
                     CLASS_SLP_TYPE * class_slope,           /* IN     */
                     FTRANS_TYPE * ftransition);           /* IN     */
static
  int delta_get_new_landuse (Classes * class_indices,        /* IN     */
                             Classes * landuse_classes,      /* IN     */
                             PIXEL local_slope,              /* IN     */
                             CLASS_SLP_TYPE * class_slope); /* IN     */
static
  void delta_phase2 (GRID_P urban_land,                      /* IN     */
                     GRID_P phase1_land,                     /* IN     */
                     GRID_P deltatron,                       /* IN/OUT */
                     GRID_P phase2_land,                     /* OUT    */
                     Classes * landuse_classes,              /* IN     */
                     int *new_indices,                       /* IN     */
                     FTRANS_TYPE * ftransition);           /* IN     */

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char deltatron_c_sccs_id[] = "@(#)deltatron.c	1.372	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: delta_deltatron
** PURPOSE:       main driver which calls delta_phase1 & delta_phase2
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  delta_deltatron (int *new_indices,                         /* IN     */
                   Classes * landuse_classes,                /* IN     */
                   Classes * class_indices,                  /* IN     */
                   GRID_P workspace1,                        /* MOD    */
                   GRID_P deltatron,                         /* IN/OUT */
                   GRID_P urban_land,                        /* IN     */
                   GRID_P land_out,                          /* OUT    */
                   GRID_P slp,                               /* IN     */
                   int drive,                                /* IN     */
                   CLASS_SLP_TYPE * class_slope,             /* IN     */
                   FTRANS_TYPE * ftransition)              /* IN     */
{
  char func[] = "DeltaTron";
  GRID_P phase1_land;
  GRID_P phase2_land;


  timer_Start (DELTA_DELTATRON);
  FUNC_INIT;
  assert (new_indices != NULL);
  assert (landuse_classes != NULL);
  assert (class_indices != NULL);
  assert (workspace1 != NULL);
  assert (urban_land != NULL);
  assert (land_out != NULL);
  assert (slp != NULL);
  assert (class_slope != NULL);
  assert (ftransition != NULL);

  phase1_land = workspace1;
  phase2_land = land_out;

  delta_phase1 (drive,                                       /* IN     */
                urban_land,                                  /* IN     */
                phase1_land,                                 /* OUT    */
                slp,                                         /* IN     */
                deltatron,                                   /* IN/OUT */
                landuse_classes,                             /* IN     */
                class_indices,                               /* IN     */
                new_indices,                                 /* IN     */
                class_slope,                                 /* IN     */
                ftransition                                  /* IN     */
    );

  delta_phase2 (urban_land,                                  /* IN     */
                phase1_land,                                 /* IN     */
                deltatron,                                   /* IN/OUT */
                phase2_land,                                 /* OUT    */
                landuse_classes,                             /* IN     */
                new_indices,                                 /* IN     */
                ftransition                                  /* IN     */
    );

  timer_Stop (DELTA_DELTATRON);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: delta_phase1
** PURPOSE:       performs deltatron phase 1 growth
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static
  void
  delta_phase1 (int drive,                                   /* IN     */
                GRID_P urban_land,                           /* IN     */
                GRID_P phase1_land,                          /* OUT    */
                GRID_P slope,                                /* IN     */
                GRID_P deltatron,                            /* IN/OUT */
                Classes * landuse_classes,                   /* IN     */
                Classes * class_indices,                     /* IN     */
                int *new_indices,                            /* IN     */
                CLASS_SLP_TYPE * class_slope,                /* IN     */
                FTRANS_TYPE * ftransition)                 /* IN     */
{
  char func[] = "delta_phase1";
  int index;
  int i;
  int j;
  int k;
  int l;
  int i_center;
  int j_center;
  PIXEL new_landuse;
  int random_int;

  timer_Start (DELTA_PHASE1);
  FUNC_INIT;
  assert (drive >= 0);
  assert (urban_land != NULL);
  assert (phase1_land != NULL);
  assert (slope != NULL);
  assert (deltatron != NULL);
  assert (landuse_classes != NULL);
  assert (class_indices != NULL);
  assert (new_indices != NULL);
  assert (class_slope != NULL);
  assert (ftransition != NULL);

  /*
   *
   * COPY INPUT LAND GRID INTO OUTPUT LAND GRID
   *
   */
  util_copy_grid (urban_land, phase1_land);

  /*
   *
   * TRY DRIVE TIMES TO MAKE TRANSITIONS
   *
   */
  for (k = 0; k < drive; k++)
  {
    /*
     *
     * SELECT A TRANSITION PIXEL TO BE CENTER OF SPREADING CLUSTER
     *
     */
    do
    {
      i_center = RANDOM_ROW;
      j_center = RANDOM_COL;
      index = new_indices[urban_land[OFFSET (i_center, j_center)]];
    }
    while (landuse_classes[index].trans == FALSE);
    /*
     *
     * RANDOMLY CHOOSE NEW LANDUSE NUMBER
     *
     */
    new_landuse =
      delta_get_new_landuse (class_indices,                  /* IN    */
                             landuse_classes,                /* IN    */
                             slope[OFFSET (i_center, j_center)],   /* IN    */
                             class_slope);                 /* IN    */

    /*
     *
     * TEST TRANSITION PROBABILITY FOR NEW CLUSTER
     *
     */
    if (RANDOM_FLOAT < TRANS_PROBABILITY1 (i_center, j_center))
    {
      /*
       *
       * TRANSITION THE CENTER PIXEL
       *
       */
      phase1_land[OFFSET (i_center, j_center)] = new_landuse;
      deltatron[OFFSET (i_center, j_center)] = 1;

      /*
       *
       * TRY BUILDING UP CLUSTER AROUND THIS CENTER PIXEL
       *
       */
      i = i_center;
      j = j_center;
      for (l = 0; l < REGION_SIZE; l++)
      {

        /*
         *
         * OCCASIONALLY RESET TO CENTER OF CLUSTER
         *
         */
        random_int = RANDOM_INT (8);
        if (random_int == 7)
        {
          i = i_center;
          j = j_center;
        }
        /*
         *
         * GET A NEIGHBOR
         *
         */
        util_get_neighbor (i, j, &i, &j);
        if (IMAGE_PT (i, j))
        {
          /*
           *
           * TEST NEW PIXEL AGAINST TRANSITION PROBABILITY
           *
           */
          if (RANDOM_FLOAT < TRANS_PROBABILITY1 (i, j))
          {
            /*
             *
             * IF THE IMMEDIATE PIXEL IS ALLOWED TO TRANSITION, THEN
             * CHANGE IT
             *
             */
            index = new_indices[urban_land[OFFSET (i, j)]];
            if (landuse_classes[index].trans == TRUE)
            {
              phase1_land[OFFSET (i, j)] = new_landuse;
              deltatron[OFFSET (i, j)] = 1;
            }
            /*
             *
             * TRY TO TRANSITION A NEIGHBORING PIXEL
             *
             */
            util_get_neighbor (i, j, &i, &j);
            if (IMAGE_PT (i, j))
            {
              index = new_indices[urban_land[OFFSET (i, j)]];
              if (landuse_classes[index].trans == TRUE)
              {
                phase1_land[OFFSET (i, j)] = new_landuse;
                deltatron[OFFSET (i, j)] = 1;
              }
            }
          }
          /* if( random_float < trans_probability ) */
        }
        /* end of if (IMAGE_PT (i,j)) */
      }
      /* end of for(l=0;l<REGION_SIZE;l++) */
    }
    /* end of if( random_float < transition_probability ) */
  }
  /* end of for(k=0;K<drive;k++) */
  FUNC_END;
  timer_Stop (DELTA_PHASE1);
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: delta_get_new_landuse
** PURPOSE:       determines new landuse to transition to
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  int
  delta_get_new_landuse (Classes * class_indices,            /* IN     */
                         Classes * landuse_classes,          /* IN     */
                         PIXEL local_slope,                  /* IN     */
                         CLASS_SLP_TYPE * class_slope)     /* IN     */
{
  char func[] = "delta_get_new_landuse";
  int class_num;
  int first_choice_idx;
  int second_choice_idx;
  CLASS_SLP_TYPE slope_diff1;
  CLASS_SLP_TYPE slope_diff2;
  PIXEL new_landuse;

  FUNC_INIT;
  assert (class_indices != NULL);
  assert (landuse_classes != NULL);
  assert (class_slope != NULL);
  /*
   *
   * FIND TWO UNIQUE LAND CLASSES
   *
   */
  do
  {
    class_num = RANDOM_INT (landclass_GetNumReducedclasses ());
    first_choice_idx = class_indices[class_num].idx;
    class_num = RANDOM_INT (landclass_GetNumReducedclasses ());
    second_choice_idx = class_indices[class_num].idx;
  }
  while (first_choice_idx == second_choice_idx);

  /*
   *
   * CHOOSE LAND USE WITH THE MOST SIMILAR TOPOGRAPHICAL SLOPE
   *
   */
  slope_diff1 = (CLASS_SLP_TYPE) local_slope -
    class_slope[first_choice_idx];
  slope_diff2 = (CLASS_SLP_TYPE) local_slope -
    class_slope[second_choice_idx];
  if ((slope_diff1 * slope_diff1) < (slope_diff2 * slope_diff2))
  {
    new_landuse = landuse_classes[first_choice_idx].num;
  }
  else
  {
    new_landuse = landuse_classes[second_choice_idx].num;
  }

  FUNC_END;
  return (new_landuse);
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: delta_phase2
** PURPOSE:       performs phase 2 deltatron growth
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static
  void
  delta_phase2 (GRID_P urban_land,                           /* IN     */
                GRID_P phase1_land,                          /* IN     */
                GRID_P deltatron,                            /* IN/OUT */
                GRID_P phase2_land,                          /* OUT    */
                Classes * landuse_classes,                   /* IN     */
                int *new_indices,                            /* IN     */
                FTRANS_TYPE * ftransition)                 /* IN     */
{
  char func[] = "delta_phase2";
  char gif_filename[256];
  char date_str[5];
  int i;
  int j;
  int k;
  int i_nghbr;
  int j_nghbr;
  int index;
  int deltatron_neighbors;
  int random_int;
  int max_num_tries = 16;

  timer_Start (DELTA_PHASE2);
  FUNC_INIT;
  assert (urban_land != NULL);
  assert (phase1_land != NULL);
  assert (deltatron != NULL);
  assert (phase2_land != NULL);
  assert (landuse_classes != NULL);
  assert (new_indices != NULL);
  assert (ftransition != NULL);

  /*
   *
   * SAVE CURRENT LAND TO PHASE2_LAND
   *
   */
  util_copy_grid (phase1_land, phase2_land);

  /*
   *
   * FOR EACH INTERIOR POINT
   *
   */
  for (i = 1; i < igrid_GetNumRows () - 1; i++)
  {
    for (j = 1; j < igrid_GetNumCols () - 1; j++)
    {
      index = new_indices[phase1_land[OFFSET (i, j)]];
      if ((landuse_classes[index].trans == TRUE) &&
          (deltatron[OFFSET (i, j)] == 0))
      {
        /*
         *
         * I,J IS A TRANSITIONAL PIXEL WHICH HAS NOT TRANSITIONED
         * WITHIN THE LAST MIN_YEARS_BETWEEN_TRANSTIONS YEARS; COUNT
         * ITS NEIGHBORS WHICH HAVE TRANSITIONED IN THE PREVIOUS
         * YEAR (IE. DELTATRON == 2)
         *
         */
        deltatron_neighbors = util_count_neighbors (deltatron, i, j, EQ, 2);

        random_int = 1 + RANDOM_INT (2);
        if (deltatron_neighbors >= random_int)
        {
          for (k = 0; k < max_num_tries; k++)
          {
            util_get_neighbor (i, j, &i_nghbr, &j_nghbr);
            index = new_indices[phase1_land[OFFSET (i_nghbr, j_nghbr)]];
            if ((deltatron[OFFSET (i_nghbr, j_nghbr)] == 2) &&
                (landuse_classes[index].trans == TRUE))
            {
              if (RANDOM_FLOAT < TRANS_PROBABILITY2 (i, j, i_nghbr, j_nghbr))
              {
                phase2_land[OFFSET (i, j)] =
                  urban_land[OFFSET (i_nghbr, j_nghbr)];
                deltatron[OFFSET (i, j)] = 1;
              }
              /*
               *
               * BREAK OUT OF FOR LOOP
               *
               */
              break;
            }
            /* end of if( deltatron[OFFSET(i_nghbr,j_nghbr)] == 2) && */
          }
          /* end of for(k=0;k<max_num_tries;k++) */
        }
        /* end of if (deltatron_neighbors >= random_int) */
      }
      /* end of if(( landuse_classes[index].trans == TRUE ) && */
    }
    /* end of for (j = 1; j < igrid_GetNumCols() - 1; j++) */
  }
  /* end of for (i = 1; i < igrid_GetNumRows() - 1; i++) */

  if (scen_GetViewDeltatronAgingFlag ())
  {
    sprintf (gif_filename, "%sdeltatron_%u_%u_%u.gif",
             scen_GetOutputDir (), proc_GetCurrentRun (), proc_GetCurrentMonteCarlo (), proc_GetCurrentYear ());
    sprintf (date_str, "%u", proc_GetCurrentYear ());
    gdif_WriteGIF (deltatron,
                   color_GetColortable (DELTATRON_COLORTABLE),
                   gif_filename,
                   date_str,
                   255);
  }

  /*
   *
   * AGE THE DELTATRONS
   *
   */
  for (i = 0; i < mem_GetTotalPixels (); i++)
  {
    if (deltatron[i] > 0)
    {
      deltatron[i]++;
    }
  }
  /*
   *
   * KILL OLD DELTATRONS
   *
   * E-Mail from: Jeannette Candau
   * Date: Mon, 20 Dec 1999 14:33:46 -0800
   * The count of the deltatrons is, as you said, a track of how
   * many "years" it has been since a transition has occured at a
   * pixel location. If a tranition occurs in phase1 the
   * deltatron value of that pixel will be 1. At the end of
   * deltatron() it is aged. So what phase2 does with "deltatron
   * == 2" is look for neighborhood change that has occured in
   * the previous year. The value of "MIN_YEARS_BETWEEN_TRANSITIONS"
   * is a control on how many
   * years must pass before another transition may occur at
   * (i,j).
   */
  util_condition_gif (mem_GetTotalPixels (),
                      deltatron,
                      GT,
                      MIN_YEARS_BETWEEN_TRANSITIONS,
                      deltatron,
                      0);
  FUNC_END;
  timer_Stop (DELTA_PHASE2);
}
