#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ugm_defines.h"
#include "ugm_macros.h"
#include "transition_obj.h"
#include "landclass_obj.h"
#include "memory_obj.h"
#include "igrid_obj.h"
#include "globals.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char transition_obj_c_sccs_id[] = "@(#)transition_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static double class_slope[MAX_NUM_CLASSES];
static double ftransition[MAX_NUM_CLASSES * MAX_NUM_CLASSES];
static double pct;
static int transition[MAX_NUM_CLASSES * MAX_NUM_CLASSES];
static int class_count[MAX_NUM_CLASSES][2];
static int trans_count;
static int class_count_sum0;
static int class_count_sum1;
static int num_classes;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void
    trans_transition (GRID_P land1,                          /* IN  */
                      GRID_P land2,                          /* IN  */
                      GRID_P slope,                          /* IN  */
                      int *new_indices,                      /* IN  */
                      Classes * landuse_classes,             /* IN  */
                      double *ftransition,                   /* OUT */
                      double *class_slope);                /* OUT */


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  trans_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &class_slope[0], sizeof (double), MAX_NUM_CLASSES);
  LOG_MEM (fp, &ftransition[0], sizeof (double), MAX_NUM_CLASSES * MAX_NUM_CLASSES);
  LOG_MEM (fp, &pct, sizeof (double), 1);
  LOG_MEM (fp, &transition[0], sizeof (int), MAX_NUM_CLASSES * MAX_NUM_CLASSES);
  LOG_MEM (fp, &class_count[0][0], sizeof (int), MAX_NUM_CLASSES * 2);
  LOG_MEM (fp, &trans_count, sizeof (int), 1);
  LOG_MEM (fp, &class_count_sum0, sizeof (int), 1);
  LOG_MEM (fp, &class_count_sum1, sizeof (int), 1);
  LOG_MEM (fp, &num_classes, sizeof (int), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_GetClassSlope
** PURPOSE:       return class slope
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double *
  trans_GetClassSlope ()
{
  return class_slope;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_GetFTransition
** PURPOSE:       return ftransition
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double *
  trans_GetFTransition ()
{
  return ftransition;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_Init
** PURPOSE:       driver to initialize transition matrix
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  trans_Init ()
{
  char func[] = "trans_Init";
  GRID_P landuse0_ptr;
  GRID_P landuse1_ptr;
  GRID_P slope_ptr;

  FUNC_INIT;
  num_classes = landclass_GetNumLandclasses ();

  landuse0_ptr = igrid_GetLanduseGridPtr (__FILE__, func, __LINE__, 0);
  landuse1_ptr = igrid_GetLanduseGridPtr (__FILE__, func, __LINE__, 1);
  slope_ptr = igrid_GetSlopeGridPtr (__FILE__, func, __LINE__);
  trans_transition (landuse0_ptr,                            /* IN  */
                    landuse1_ptr,                            /* IN  */
                    slope_ptr,                               /* IN  */
                    landclass_GetNewIndicesPtr (),           /* IN  */
                    landclass_GetClassesPtr (),              /* IN  */
                    ftransition,                             /* OUT */
                    class_slope);                          /* OUT */
  landuse0_ptr = igrid_GridRelease (__FILE__, func, __LINE__, landuse0_ptr);
  landuse1_ptr = igrid_GridRelease (__FILE__, func, __LINE__, landuse1_ptr);
  slope_ptr = igrid_GridRelease (__FILE__, func, __LINE__, slope_ptr);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_transition
** PURPOSE:       initialize the transition matrix
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  trans_transition (GRID_P land1,                            /* IN  */
                    GRID_P land2,                            /* IN  */
                    GRID_P slope,                            /* IN  */
                    int *new_indices,                        /* IN  */
                    Classes * landuse_classes,               /* IN  */
                    double *ftransition,                     /* OUT */
                    double *class_slope)                   /* OUT */
{

  char func[] = "trans_transition";
  int i;
  int j;
  int k;
  int l;
  int lsum;
  PIXEL land1_val;
  PIXEL land2_val;
  int land1_idx;
  int land2_idx;


  FUNC_INIT;
  assert (land1 != NULL);
  assert (land2 != NULL);
  assert (slope != NULL);
  assert ((num_classes > 0) && (num_classes <= MAX_NUM_CLASSES));
  assert (new_indices != NULL);
  assert (landuse_classes != NULL);
  assert (transition != NULL);
  assert (ftransition != NULL);
  assert (class_slope != NULL);

  trans_count = 0;
  class_count_sum0 = 0;
  class_count_sum1 = 0;

  /*
   *
   * ZERO OUT MEMORY
   *
   */
  memset ((void *) transition, 0,
          num_classes * sizeof (transition[0]) * num_classes);
  memset ((void *) ftransition, 0,
          num_classes * sizeof (ftransition[0]) * num_classes);
  for (k = 0; k < num_classes; k++)
  {
    class_count[k][0] = 0;
    class_count[k][1] = 0;
    class_slope[k] = 0.0;
  }

  for (i = 0; i < igrid_GetNumRows (); i++)
  {
    for (j = 0; j < igrid_GetNumCols (); j++)
    {
      land1_val = land1[OFFSET (i, j)];
      land2_val = land2[OFFSET (i, j)];

      land1_idx = new_indices[land1_val];
      land2_idx = new_indices[land2_val];

      class_count[land1_idx][0]++;
      class_count[land2_idx][1]++;

      class_slope[land2_idx] +=
        (double) slope[OFFSET (i, j)];


      transition[TRANS_OFFSET (land1_idx, land2_idx)]++;

      if (land1_val != land2_val)
      {
        trans_count++;
      }
    }
  }

  for (k = 0; k < num_classes; k++)
  {
    lsum = 0;
    class_count_sum0 += class_count[k][0];
    class_count_sum1 += class_count[k][1];

    for (l = 0; l < num_classes; l++)
    {
      ftransition[TRANS_OFFSET (k, l)] =
        (double) transition[TRANS_OFFSET (k, l)];
      lsum += transition[TRANS_OFFSET (k, l)];
    }

    for (l = 0; l < num_classes; l++)
    {
      ftransition[TRANS_OFFSET (k, l)] = (lsum == 0) ?
        0.0 : ftransition[TRANS_OFFSET (k, l)] / (double) lsum;
    }
  }

  FUNC_END;
  return;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: trans_LogTransition
** PURPOSE:       log transition matrix to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  trans_LogTransition (FILE * fp)
{
  int k;
  int l;
  Classes *landuse_classes;

  landuse_classes = landclass_GetClassesPtr ();

  fprintf (fp, "\n**************LOGGING TRANSITION MATRICES");
  fprintf (fp, "**************\n");
  fprintf (fp, "Land 1 classed pixel count (class_count_sum0) = %d\n", class_count_sum0);
  fprintf (fp, "Land 2 classed pixel count (class_count_sum1) = %d\n", class_count_sum1);
  fprintf (fp, "\n");
/*
 *
 * PRINT PIXEL COUNT TRANSITIONS FOR LAND COVER DATA
 *
 */
  fprintf (fp, "       LOGGING CLASS PER PIXEL TRANSITION    \n");
  fprintf (fp, "        ");
  for (k = 0; k < num_classes; k++)
    fprintf (fp, "%9s ", landuse_classes[k].name);
  fprintf (fp, "\n");

  for (k = 0; k < num_classes; k++)
  {
    fprintf (fp, "%8s", landuse_classes[k].name);
    for (l = 0; l < num_classes; l++)
    {
      fprintf (fp, " %8u ", transition[TRANS_OFFSET (k, l)]);
    }
    fprintf (fp, "\n");
  }
  fprintf (fp, "\n");

/*
 *
 * PRINT ANNUAL TRANSITION PROBABILITIES FOR LAND COVER DATA
 *
 */
  fprintf (fp, "       LOGGING ANNUAL TRANSITION PROBABILITIES \n");
  fprintf (fp, "        ");
  for (k = 0; k < num_classes; k++)
    fprintf (fp, "%9s ", landuse_classes[k].name);
  fprintf (fp, "\n");

  for (k = 0; k < num_classes; k++)
  {
    fprintf (fp, "%8s", landuse_classes[k].name);
    for (l = 0; l < num_classes; l++)
    {
      fprintf (fp, " %8.2f ", 100 * ftransition[TRANS_OFFSET (k, l)]);
    }
    fprintf (fp, "\n");
  }
  fprintf (fp, "\n");

/*
 *
 * PRINT AVERAGE SLOPE PER CLASS FOR LAND COVER DATA
 *
 */
  fprintf (fp, "       LOGGING LAND CLASS AVERAGE SLOPES \n");
  fprintf (fp, "                        Land1");
  fprintf (fp, "             Land2         Average\n");
  fprintf (fp, "Class Totals:   count[pct_change]  ");
  fprintf (fp, "count[pct_change]     Slope\n");
  for (k = 0; k < num_classes; k++)
  {
    fprintf (fp, "%12s ", landuse_classes[k].name);

    pct = (class_count[k][1] > 0) ?
      class_slope[k] / class_count[k][1] : 0.0;

    class_slope[k] = pct;

    fprintf (fp, "%10d [%5.1f] %10d [%5.1f]     %7.3f\n", class_count[k][0],
             100.0 * class_count[k][0] / class_count_sum0,
             class_count[k][1], 100.0 * class_count[k][1] /
             class_count_sum1, pct);
  }
  fprintf (fp, "\n");
}
