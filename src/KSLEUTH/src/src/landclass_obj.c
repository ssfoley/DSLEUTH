#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ugm_defines.h"
#include "ugm_macros.h"
#include "landclass_obj.h"
#include "scenario_obj.h"
#include "globals.h"
#include "grid_obj.h"
#include "memory_obj.h"
#include "utilities.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char landclass_obj_c_sccs_id[] = "@(#)landclass_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static Classes class_indices[MAX_NUM_CLASSES];
static int new_indices[MAX_NEW_INDICES];
static Classes landuse_classes[MAX_NUM_CLASSES];
static int urban_code;
static int num_landclasses;
static char filename[MAX_FILENAME_LEN];
static int max_landclass_num;
static int num_reduced_classes;
static char annual_prob_filename[MAX_FILENAME_LEN];
static BOOLEAN ugm_read;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static int landclass_process_user_color (char *string2process);
static void landclass_SetMaxLandclassNum ();
static void landclass_SetUrbanCode ();
static void landclass_MapLandclassNum_2_idx ();
static void landclass_CreateReducedClasses ();

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_MemoryLog
** PURPOSE:       log memory locations onto FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &class_indices[0], sizeof (Classes), MAX_NUM_CLASSES);
  LOG_MEM (fp, &new_indices[0], sizeof (int), MAX_NEW_INDICES);
  LOG_MEM (fp, &landuse_classes[0], sizeof (Classes), MAX_NUM_CLASSES);
  LOG_MEM (fp, &urban_code, sizeof (int), 1);
  LOG_MEM (fp, &num_landclasses, sizeof (int), 1);
  LOG_MEM_CHAR_ARRAY (fp, &filename, sizeof (char), MAX_FILENAME_LEN);
  LOG_MEM (fp, &max_landclass_num, sizeof (int), 1);
  LOG_MEM (fp, &num_reduced_classes, sizeof (int), 1);
  LOG_MEM_CHAR_ARRAY (fp, &annual_prob_filename, sizeof (char), MAX_FILENAME_LEN);
  LOG_MEM (fp, &ugm_read, sizeof (int), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedClassesPtr
** PURPOSE:       return ptr to class_indices
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
Classes *
  landclass_GetReducedClassesPtr ()
{
  return class_indices;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassesPtr
** PURPOSE:       return ptr to landuse_classes
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
Classes *
  landclass_GetClassesPtr ()
{
  return landuse_classes;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetNewIndicesPtr
** PURPOSE:       return ptr to new_indices
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int *
  landclass_GetNewIndicesPtr ()
{
  return new_indices;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetUrbanCode
** PURPOSE:       return the urban code
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetUrbanCode ()
{
  return urban_code;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetNumLandclasses
** PURPOSE:       return num_landclasses
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetNumLandclasses ()
{
  return num_landclasses;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetNumReducedclasses
** PURPOSE:       return num_reduced_classes
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetNumReducedclasses ()
{
  return num_reduced_classes;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetMaxLandclasses
** PURPOSE:       return max_landclass_num
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetMaxLandclasses ()
{
  return max_landclass_num;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassNum
** PURPOSE:       return landclass num for a given index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetClassNum (int i)
{
  return landuse_classes[i].num;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_IsAlandclass
** PURPOSE:       test if val is a landclass value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  landclass_IsAlandclass (int val)
{
  char func[] = "landclass_IsAlandclass";
  BOOLEAN rv = FALSE;
  int i;

  if (!ugm_read)
  {
    sprintf (msg_buf, "landclasses file has not been read yet!");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }

  for (i = 0; i < num_landclasses; i++)
  {
    if (landuse_classes[i].num == val)
      rv = TRUE;
  }
  return rv;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassIDX
** PURPOSE:       return idx for a given index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetClassIDX (int i)
{
  return landuse_classes[i].idx;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassColor
** PURPOSE:       return color val for given landclass
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetClassColor (int i)
{
  return landuse_classes[i].red * 256 * 256 +
    landuse_classes[i].green * 256 + landuse_classes[i].blue;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassEXC
** PURPOSE:       is this an excluded class
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  landclass_GetClassEXC (int i)
{
  return landuse_classes[i].EXC;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetClassTrans
** PURPOSE:       is this a transition class
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  landclass_GetClassTrans (int i)
{
  return landuse_classes[i].trans;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedNum
** PURPOSE:       return class num for given class_indices index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetReducedNum (int i)
{
  return class_indices[i].num;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedIDX
** PURPOSE:       return idx for a given class_indices index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetReducedIDX (int i)
{
  return class_indices[i].idx;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedColor
** PURPOSE:       return color for given class_indices index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  landclass_GetReducedColor (int i)
{
  return class_indices[i].red * 256 * 256 +
    class_indices[i].green * 256 + class_indices[i].blue;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedEXC
** PURPOSE:       is class at class_indices[i] excluded
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  landclass_GetReducedEXC (int i)
{
  return class_indices[i].EXC;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_GetReducedTrans
** PURPOSE:       is this a transition class
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  landclass_GetReducedTrans (int i)
{
  return class_indices[i].trans;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_AnnualProbInit
** PURPOSE:       initializes the annual_class_probabilities file with 0's
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_AnnualProbInit ()
{
  char func[] = "landclass_AnnualProbInit";
  FILE *fp;
  int i;
  int num_pixels;
  int num_written;
  int zero = 0;

  sprintf (annual_prob_filename, "%sannual_class_probabilities_%u",
           scen_GetOutputDir (), glb_mype);

  FILE_OPEN (fp, annual_prob_filename, "wb");

  num_pixels = mem_GetTotalPixels () * landclass_GetNumLandclasses ();
  for (i = 0; i < num_pixels; i++)
  {
    num_written = fwrite (&zero, sizeof (PIXEL), 1, fp);
    if (num_written != 1)
    {
      sprintf (msg_buf, "Unable to write to file: %s", annual_prob_filename);
      LOG_ERROR (msg_buf);
      sprintf (msg_buf, "%s", strerror (errno));
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    if (scen_GetLogWritesFlag ())
    {
      fprintf (scen_GetLogFP (), "%s %u %u zeroes written to %s\n",
               __FILE__, __LINE__, num_pixels, annual_prob_filename);
    }
    scen_CloseLog ();
  }
  fclose (fp);

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_AnnualProbUpdate
** PURPOSE:       update the annual_prob_filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_AnnualProbUpdate (GRID_P land1_ptr)
{
  char func[] = "landclass_AnnualProbUpdate";
  FILE *fp;
  int total_pixels;
  int i;
  int cc;
  int rc;
  int offset;
  fpos_t pos_ptr;
  GRID_P current_class_ptr;

  if (!scen_GetDoingLanduseFlag ())
  {
    return;
  }

  current_class_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
  total_pixels = mem_GetTotalPixels ();
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    if (scen_GetLogWritesFlag ())
    {
      fprintf (scen_GetLogFP (), "%s %u updating file %s\n",
               __FILE__, __LINE__, annual_prob_filename);
    }
    scen_CloseLog ();
  }

  FILE_OPEN (fp, annual_prob_filename, "r+b");

  for (cc = 0; cc < landclass_GetNumLandclasses (); cc++)
  {
    offset = cc * total_pixels * sizeof (PIXEL);
    rc = fseek (fp, offset, SEEK_SET);
    if (rc != 0)
    {
      printf ("%s %u ERROR\n", __FILE__, __LINE__);
      EXIT (1);
    }
    rc = fgetpos (fp, &pos_ptr);
    if (rc != 0)
    {
      printf ("%s %u ERROR\n", __FILE__, __LINE__);
      EXIT (1);
    }
    rc = fread (current_class_ptr, total_pixels * sizeof (PIXEL), 1, fp);
    if (feof (fp) || ferror (fp))
    {
      printf ("%s %u ERROR\n", __FILE__, __LINE__);
      EXIT (1);
    }
    for (i = 0; i < total_pixels; i++)
    {
      if (cc == new_indices[land1_ptr[i]])
      {
        (current_class_ptr[i])++;
      }
    }
    rc = fsetpos (fp, &pos_ptr);
    if (rc != 0)
    {
      printf ("%s %u ERROR\n", __FILE__, __LINE__);
      EXIT (1);
    }
    rc = fwrite (current_class_ptr, total_pixels * sizeof (PIXEL), 1, fp);
    if (rc != 1)
    {
      printf ("%s %u ERROR\n", __FILE__, __LINE__);
      EXIT (1);
    }
  }
  fclose (fp);
  current_class_ptr = mem_GetWGridFree (__FILE__,
                                        func,
                                        __LINE__,
                                        current_class_ptr);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_BuildProbImage
** PURPOSE:       build prob images from annual_prob_filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_BuildProbImage (GRID_P cum_probability_ptr,
                            GRID_P cum_uncertainty_ptr)
{
  char func[] = "landclass_BuildProbImage";
  char command[2 * MAX_FILENAME_LEN + 20];
  int total_pixels;
  GRID_P max_grid;
  GRID_P sum_grid;
  GRID_P inp_grid;
  int num_landclasses;
  int i;
  int k;
  FILE *fp;

  num_landclasses = landclass_GetNumLandclasses ();
  total_pixels = mem_GetTotalPixels ();

  max_grid = mem_GetWGridPtr (__FILE__, func, __LINE__);
  sum_grid = mem_GetWGridPtr (__FILE__, func, __LINE__);
  inp_grid = mem_GetWGridPtr (__FILE__, func, __LINE__);
  assert (sum_grid != NULL);
  assert (max_grid != NULL);
  assert (inp_grid != NULL);
  assert (cum_probability_ptr != NULL);
  assert (cum_uncertainty_ptr != NULL);
  assert (num_landclasses > 0);
  assert (total_pixels > 0);

  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    if (scen_GetLogReadsFlag ())
    {
      fprintf (scen_GetLogFP (), "%s %u Reading file: %s\n",
               __FILE__, __LINE__, annual_prob_filename);
    }
    scen_CloseLog ();
  }

  FILE_OPEN (fp, annual_prob_filename, "rb");

  /*
   *
   * READ IN THE K=0 LANDCLASS DATA
   *
   */
  fread (max_grid, sizeof (PIXEL), total_pixels, fp);
  if (feof (fp) || ferror (fp))
  {
    sprintf (msg_buf, "reading file: %s", annual_prob_filename);
    LOG_ERROR (msg_buf);
    sprintf (msg_buf, "%s", strerror (errno));
    LOG_ERROR (msg_buf);
    EXIT (1);
  }

  /*
   *
   * INITIALIZE SUM_GRID
   *
   */
  memcpy (sum_grid, max_grid, sizeof (PIXEL) * total_pixels);

  for (k = 1; k < num_landclasses; k++)
  {
    /*
     *
     * READ IN THE K=0 LANDCLASS DATA
     *
     */
    fread (inp_grid, sizeof (PIXEL), total_pixels, fp);
    if (feof (fp) || ferror (fp))
    {
      sprintf (msg_buf, "reading file: %s", annual_prob_filename);
      LOG_ERROR (msg_buf);
      sprintf (msg_buf, "%s", strerror (errno));
      LOG_ERROR (msg_buf);
      EXIT (1);
    }

    /*
     *
     * NOW LOOK FOR THE MAX OF THE MAX AND THE SUM
     *
     */
    for (i = 0; i < total_pixels; i++)
    {
      if (inp_grid[i] > max_grid[i])
      {
        max_grid[i] = inp_grid[i];
        cum_probability_ptr[i] = k;
      }
      sum_grid[i] += inp_grid[i];
    }
  }
  fclose (fp);
  sprintf (command, "rm %s", annual_prob_filename);
  system (command);


  /*
   *
   * CALCULATE THE CUM_UNCERTAINTY GRID
   *
   */
  for (i = 0; i < total_pixels; i++)
  {
    if (sum_grid[i] != 0)
    {
      cum_uncertainty_ptr[i] = 100 - (100 * max_grid[i]) / sum_grid[i];
    }
    else
    {
      sprintf (msg_buf, "divide by zero: sum_grid[%u] = %d", i, sum_grid[i]);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
  max_grid = mem_GetWGridFree (__FILE__, func, __LINE__, max_grid);
  sum_grid = mem_GetWGridFree (__FILE__, func, __LINE__, sum_grid);
  inp_grid = mem_GetWGridFree (__FILE__, func, __LINE__, inp_grid);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_Init
** PURPOSE:       initialization routine for landclasses
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_Init ()
{
  landclass_SetMaxLandclassNum ();
  landclass_MapLandclassNum_2_idx ();
  landclass_CreateReducedClasses ();
  landclass_SetUrbanCode ();
  ugm_read = TRUE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_SetUrbanCode
** PURPOSE:       set the urban code field
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  landclass_SetUrbanCode ()
{
  char func[] = "landclass_SetUrbanCode";
  int i;
  assert (num_landclasses > 0);
  /*
   *
   * FIND URBAN CODE
   *
   */
  for (i = 0; i < num_landclasses; i++)
  {
    if (!strcmp (landuse_classes[i].id, "URB"))
    {
      urban_code = landuse_classes[i].num;
    }
  }
  assert (urban_code > 0);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_CreateReducedClasses
** PURPOSE:       create the reduced classes
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  landclass_CreateReducedClasses ()
{
  int i;
  int reduced_count = 0;

  /*
   *
   * CREATE REDUCED CLASSES ARRAY
   *
   */
  for (i = 0; i < num_landclasses; i++)
  {
    if (strcmp (landuse_classes[i].id, "EXC") &&
        strcmp (landuse_classes[i].id, "URB") &&
        strcmp (landuse_classes[i].id, "UNC"))
    {
      class_indices[reduced_count].num = landuse_classes[i].num;
      class_indices[reduced_count].idx = i;
      strcpy (class_indices[reduced_count].name, landuse_classes[i].name);
      strcpy (class_indices[reduced_count].id, landuse_classes[i].id);
      class_indices[reduced_count].EXC = landuse_classes[i].EXC;
      class_indices[reduced_count].trans = landuse_classes[i].trans;
      class_indices[reduced_count].red = landuse_classes[i].red;
      class_indices[reduced_count].green = landuse_classes[i].green;
      class_indices[reduced_count].blue = landuse_classes[i].blue;
      reduced_count++;
    }
  }
  num_reduced_classes = reduced_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_MapLandclassNum_2_idx
** PURPOSE:       CREATE MAPPING FROM LANDUSE CLASS NUM BACK INTO IDX
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  landclass_MapLandclassNum_2_idx ()
{
  int i;
  int idx;
  /*
   *
   * CREATE MAPPING FROM LANDUSE CLASS NUM BACK INTO IDX
   *
   */
  for (i = 0; i < MAX_NEW_INDICES; i++)
  {
    new_indices[i] = 0;
  }
  for (idx = 0; idx < num_landclasses; idx++)
  {
    new_indices[landuse_classes[idx].num] = idx;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclassSetNumClasses
** PURPOSE:       set num_landclasses
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
void
  landclassSetNumClasses (int val)
{
  assert (val >= 0);
  assert (val < 256);
  num_landclasses = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclassSetName
** PURPOSE:       set name
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
void
  landclassSetName (int index, char *string)
{
  assert (index >= 0);
  assert (index < num_landclasses);
  strcpy (landuse_classes[index].name, string);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclassSetType
** PURPOSE:       set Type
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
void
  landclassSetType (int index, char *string)
{
  assert (index >= 0);
  assert (index < num_landclasses);
  strcpy (landuse_classes[index].id, string);
  landuse_classes[index].EXC =
    strcmp (landuse_classes[index].id, "EXC");
  landuse_classes[index].trans = TRUE;
  if (strcmp (landuse_classes[index].id, "EXC") == 0)
  {
    landuse_classes[index].trans = FALSE;
  }
  if (strcmp (landuse_classes[index].id, "EXC") > 0)
  {
    landuse_classes[index].trans = FALSE;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclassSetColor
** PURPOSE:       set Color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
void
  landclassSetColor (int index, int val)
{
  assert (index >= 0);
  assert (index < num_landclasses);
  landuse_classes[index].red = (val & RED_MASK) >> 16;
  landuse_classes[index].green = (val & GREEN_MASK) >> 8;
  landuse_classes[index].blue = val & BLUE_MASK;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclassSetGrayscale
** PURPOSE:       set Grayscale
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
void
  landclassSetGrayscale (int index, int val)
{
  assert (index >= 0);
  assert (index < num_landclasses);
  landuse_classes[index].num = val;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_LogIt
** PURPOSE:       log landclass struct to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  landclass_LogIt (FILE * fp)
{
  int i;
  int idx;
  int color_val;
  char zeroes[] = "000000";
  char color_str[6];
  char hex_str[6];

  fprintf (fp, "*****************LOG OF LANDCLASS SUMMARY*****************\n");
  fprintf (fp, "filename: %s\n", filename);
  fprintf (fp, "num_landclasses = %u\n", num_landclasses);
  fprintf (fp, "max_landclass_num =  %u\n", max_landclass_num);
  fprintf (fp, "urban_code =  %u\n", urban_code);
  fprintf (fp,
    "  i idx num            name    id    * trans    RGB      hexRGB\n");
  for (i = 0; i < num_landclasses; i++)
  {
    color_val = landuse_classes[i].red * 256 * 256 +
      landuse_classes[i].green * 256 +
      landuse_classes[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %15s %5s %4d %3u %3u %3u %3u = 0X%s\n",
             i, landuse_classes[i].idx,
             landuse_classes[i].num,
             landuse_classes[i].name,
             landuse_classes[i].id,
             landuse_classes[i].EXC,
             landuse_classes[i].trans,
             landuse_classes[i].red,
             landuse_classes[i].green,
             landuse_classes[i].blue,
             hex_str);
  }
  fprintf (fp, "* =  strcmp (landuse_classes[idx].id, \"EXC\")\n");
  fprintf (fp, "\nnum_reduced_classes = %u\n", num_reduced_classes);
  fprintf (fp,
    "  i idx num            name    id    * trans    RGB      hexRGB\n");
  for (i = 0; i < num_reduced_classes; i++)
  {
    color_val = class_indices[i].red * 256 * 256 +
      class_indices[i].green * 256 +
      class_indices[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %15s %5s %4d %3u %3u %3u %3u = 0X%s\n",
             i, class_indices[i].idx,
             class_indices[i].num,
             class_indices[i].name,
             class_indices[i].id,
             class_indices[i].EXC,
             class_indices[i].trans,
             class_indices[i].red,
             class_indices[i].green,
             class_indices[i].blue,
             hex_str);
  }
  fprintf (fp, "\n");
  fprintf (fp, "\nnew_indices\n");
  fprintf (fp, "num   new_indices[num]\n");
  for (idx = 0; idx < num_landclasses; idx++)
  {
    fprintf (fp,
             "%3u      %3u\n",
        landuse_classes[idx].num, new_indices[landuse_classes[idx].num]);
  }

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: landclass_SetMaxLandclassNum
** PURPOSE:       set the max landclass val
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  landclass_SetMaxLandclassNum ()
{
  char func[] = "landclass_SetMaxLandclassNum";
  int i;
  int max = 0;
  /*
   *
   * FIND THE MAX LANDUSE CLASS NUM
   *
   */
  for (i = 0; i < num_landclasses; i++)
  {
    max = MAX (max, landuse_classes[i].num);
  }
  if (max >= MAX_NEW_INDICES)
  {
    sprintf (msg_buf, "The maximum class number = %d in file:%s\n",
             max, filename);
    strcat (msg_buf, "exceeds MAX_NEW_INDICES. Increase the value of \n");
    strcat (msg_buf,
    "MAX_NEW_INDICES and recompile or reduce the landuse class number\n");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  max_landclass_num = max;
}
