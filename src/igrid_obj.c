/******************************************************************************
*******************************************************************************
**                           MODULE PROLOG                                   **
*******************************************************************************
This object encapsulates the management of the input data grids.


*******************************************************************************
******************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#ifdef MPI
#include "mpi.h"
#endif
#include "igrid_obj.h"
#include "scenario_obj.h"
#include "globals.h"
#include "memory_obj.h"
#include "gdif_obj.h"
#include "output.h"
#include "color_obj.h"
#include "landclass_obj.h"
#include "ugm_macros.h"
#include "proc_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char igrid_obj_c_sccs_id[] = "@(#)igrid_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static igrid_info igrid;
static int igrid_count;
static int road_pixel_count[MAX_ROAD_YEARS];
static int excld_count;
static road_percent_t percent_road[MAX_ROAD_YEARS];
static int total_pixels;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/

#define EXTRACT_FILENAME(a)                            \
  filename = strrchr ((a), '/');                       \
  if (filename)                                        \
  {                                                    \
    filename++;                                        \
  }                                                    \
  else                                                 \
  {                                                    \
    filename = (a);                                    \
  }
/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void igrid_CalculatePercentRoads ();
static BOOLEAN igrid_ValidateUrbanGrids (FILE * fp);
static BOOLEAN igrid_ValidateRoadGrids (FILE * fp);
static BOOLEAN igrid_ValidateLanduseGrids (FILE * fp);
static BOOLEAN igrid_ValidateSlopeGrid (FILE * fp);
static BOOLEAN igrid_ValidateExcludedGrid (FILE * fp);
static BOOLEAN igrid_ValidateBackgroundGrid (FILE * fp);
static void igrid_CountRoadPixels ();
static void igrid_echo_input (GRID_P ptr, char *filename);
static void igrid_SetLocation ();
static void igrid_SetFilenames ();
static void igrid_SetGridSizes (grid_info * grid_ptr);
static void igrid_ReadGrid (char *filepath, GRID_P scrtch_pad, GRID_P grid_p);

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_MemoryLog
** PURPOSE:       write out memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &igrid, sizeof (igrid_info), 1);
  LOG_MEM (fp, &igrid_count, sizeof (int), 1);
  LOG_MEM (fp, &road_pixel_count[0], sizeof (int), MAX_ROAD_YEARS);
  LOG_MEM (fp, &excld_count, sizeof (int), 1);
  LOG_MEM (fp, &percent_road[0], sizeof (road_percent_t), MAX_ROAD_YEARS);
  LOG_MEM (fp, &total_pixels, sizeof (int), 1);
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetIGridRoadPixelCount
** PURPOSE:       get road year pixel count by date
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   A year is pasted into the function and it then searches
**                all the road years igrid structs looking for the road
**                eqaul to or previous to the requested year. It returns
**                the road pixel count for that year.
**
*/
int
  igrid_GetIGridRoadPixelCount (int year)
{
  int i;

  for (i = igrid.road_count - 1; i > 0; i--)
  {
    if (year >= igrid.road[i].year.digit)
    {
      break;
    }
  }
  return road_pixel_count[i];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetIGridExcludedPixelCount
** PURPOSE:       return the # of excluded pixels
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetIGridExcludedPixelCount ()
{
  return excld_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetIGridRoadPercentage
** PURPOSE:       return the % of road pixels for a given year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
road_percent_t
  igrid_GetIGridRoadPercentage (int year)
{
  int i;
  for (i = igrid.road_count - 1; i > 0; i--)
  {
    if (year > igrid.road[i].year.digit)
    {
      break;
    }
  }
  return percent_road[i];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetIGridCount
** PURPOSE:       return the # of igrids (or input grids)
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetIGridCount ()
{
  return igrid_count;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetNumRows
** PURPOSE:       return the # rows in a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetNumRows ()
{
  return igrid.slope.nrows;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetNumCols
** PURPOSE:       return the # cols in a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetNumCols ()
{
  return igrid.slope.ncols;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetNumTotalPixels
** PURPOSE:       return the total # pixels in grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetNumTotalPixels ()
{
  return igrid.slope.ncols * igrid.slope.nrows;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetUrbanYear
** PURPOSE:       return urban date as a digit for a given urban index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetUrbanYear (int i)
{
  return igrid.urban[i].year.digit;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetLanduseYear
** PURPOSE:       return landuse date as a digit for a given landuse index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetLanduseYear (int i)
{
  return igrid.landuse[i].year.digit;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetUrbanCount
** PURPOSE:       return the # of urban grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_GetUrbanCount ()
{
  return igrid.urban_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetLocation
** PURPOSE:       return location string for this scenario
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  igrid_GetLocation ()
{
  return igrid.location;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GridRelease
** PURPOSE:       release the memory used by a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  igrid_GridRelease (char *file, char *fun, int line, GRID_P ptr)
{
#ifdef PACKING
  return mem_GetWGridFree (file, fun, line, ptr);
#else
  return NULL;
#endif
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetUrbanGridPtr
** PURPOSE:       return ptr to urban data
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to 
**                hold the unpacked data and then unpack it.
*/
GRID_P
  igrid_GetUrbanGridPtr (char *file, char *fun, int line, int index)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.urban[index].ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.urban[index].ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetRoadGridPtr
** PURPOSE:       return ptr to road grid data
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to 
**                hold the unpacked data and then unpack it.
*/
GRID_P
  igrid_GetRoadGridPtr (char *file, char *fun, int line, int index)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.road[index].ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.road[index].ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetUrbanGridPtrByYear
** PURPOSE:       return ptr to urban grid data by year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetUrbanGridPtrByYear (char *file, char *fun, int line, int year)
{
  GRID_P ptr;
  int i;

  assert (year >= igrid.urban[0].year.digit);

  for (i = igrid.urban_count - 1; i > 0; i--)
  {
    if (year >= igrid.urban[i].year.digit)
    {
      break;
    }
  }

#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.urban[i].ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.urban[i].ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetRoadGridPtrByYear
** PURPOSE:       return ptr to road grid data by year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetRoadGridPtrByYear (char *file, char *fun, int line, int year)
{
  GRID_P ptr;
  int i;

  for (i = igrid.road_count - 1; i > 0; i--)
  {
    if (year >= igrid.road[i].year.digit)
    {
      break;
    }
  }

#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.road[i].ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.road[i].ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetLanduseGridPtr
** PURPOSE:       return ptr to landuse grid data by index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetLanduseGridPtr (char *file, char *fun, int line, int index)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.landuse[index].ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.landuse[index].ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetSlopeGridPtr
** PURPOSE:       return ptr to slope grid data
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetSlopeGridPtr (char *file, char *fun, int line)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.slope.ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.slope.ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetExcludedGridPtr
** PURPOSE:       return ptr to excluded grid data
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetExcludedGridPtr (char *file, char *fun, int line)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.excluded.ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.excluded.ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_GetBackgroundGridPtr
** PURPOSE:       return ptr to background grid data
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if the data is packed then we need to allocate space to
**                hold the unpacked data and then unpack it.
**
*/
GRID_P
  igrid_GetBackgroundGridPtr (char *file, char *fun, int line)
{
  GRID_P ptr;
#ifdef PACKING
  ptr = mem_GetWGridPtr (file, fun, line);
  _unpack ((char *) igrid.background.ptr,
           ptr,
           total_pixels,
           -1);

#else
  ptr = igrid.background.ptr;
#endif
  return ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_Debug
** PURPOSE:       a debug routine which can be called at various places
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_Debug (FILE * fp, char *caller, int location)
{
  int i;
  fprintf (fp, "%s %u *******************************************\n",
           __FILE__, __LINE__);
  fprintf (fp, "%s %u   CURRENT STATE OF IGRID OBJECT\n", __FILE__, __LINE__);
  fprintf (fp, "%s %u   module: %s line: %u\n",
           __FILE__, __LINE__, caller, location);
  fprintf (fp, "%s %u *******************************************\n",
           __FILE__, __LINE__);
  fprintf (fp, "%s %u &igrid = %d\n", __FILE__, __LINE__, &igrid);
  fprintf (fp, "%s %u igrid.location = %s\n", __FILE__, __LINE__,
           igrid.location);
  fprintf (fp, "%s %u igrid.urban_count = %d\n",
           __FILE__, __LINE__, igrid.urban_count);
  fprintf (fp, "%s %u igrid.road_count = %d\n",
           __FILE__, __LINE__, igrid.road_count);
  fprintf (fp, "%s %u igrid.landuse_count = %d\n",
           __FILE__, __LINE__, igrid.landuse_count);
  fprintf (fp, "%s %u igrid.excluded_count = %d\n",
           __FILE__, __LINE__, igrid.excluded_count);
  fprintf (fp, "%s %u igrid.slope_count = %d\n",
           __FILE__, __LINE__, igrid.slope_count);
  fprintf (fp, "%s %u igrid.background_count = %d\n",
           __FILE__, __LINE__, igrid.background_count);
  for (i = 0; i < igrid.urban_count; i++)
  {
    fprintf (fp, "\n%s %u Urban grid #%u \n", __FILE__, __LINE__, i);
    grid_dump (fp, &igrid.urban[i]);
  }
  for (i = 0; i < igrid.road_count; i++)
  {
    fprintf (fp, "\n%s %u Road grid #%u \n", __FILE__, __LINE__, i);
    grid_dump (fp, &igrid.road[i]);
  }
  for (i = 0; i < igrid.landuse_count; i++)
  {
    fprintf (fp, "\n%s %u Landuse grid #%u \n", __FILE__, __LINE__, i);
    grid_dump (fp, &igrid.landuse[i]);
  }
  fprintf (fp, "\n%s %u Excluded grid\n", __FILE__, __LINE__);
  grid_dump (fp, &igrid.excluded);
  fprintf (fp, "\n%s %u Slope grid\n", __FILE__, __LINE__);
  grid_dump (fp, &igrid.slope);
  fprintf (fp, "\n%s %u Background grid\n", __FILE__, __LINE__);
  grid_dump (fp, &igrid.background);
  fprintf (fp, "%s %u *******************************************\n",
           __FILE__, __LINE__);
  fprintf (fp, "%s %u *******************************************\n",
           __FILE__, __LINE__);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_Dump
** PURPOSE:       dump the values in a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_Dump (GRID_P ptr, FILE * fp)
{
  int i;
  int j;
  int k = 0;
  int rows;
  int cols;

  rows = igrid_GetNumRows ();
  cols = igrid_GetNumCols ();
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      fprintf (fp, " %3u", ptr[k++]);
    }
    fprintf (fp, "\n");
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_NormalizeRoads
** PURPOSE:       normalizes road grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_NormalizeRoads ()
{
  char func[] = "igrid_NormalizeRoads";
  int i;
  int j;
  PIXEL max_of_max = 0;
  GRID_P grid_ptr;
  float image_max;
  float norm_factor;

#ifdef PACKING
  grid_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
#endif
  for (i = 0; i < igrid.road_count; i++)
  {
    max_of_max = MAX (max_of_max, igrid.road[i].max);
  }
  for (i = 0; i < igrid.road_count; i++)
  {
#ifdef PACKING
    _unpack ((char *) igrid.road[i].ptr,
             grid_ptr,
             total_pixels,
             -1);
#else
    grid_ptr = igrid.road[i].ptr;
#endif
    image_max = (float) igrid.road[i].max;
    norm_factor = image_max / (float) max_of_max;
    for (j = 0; j < total_pixels; j++)
    {
      grid_ptr[j] =
        (PIXEL) (((100.0 * grid_ptr[j]) / image_max) * norm_factor);
    }

  }
#ifdef PACKING
  mem_GetWGridFree (__FILE__, func, __LINE__, grid_ptr);
#endif
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateGrids
** PURPOSE:       validate all input grid values
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_ValidateGrids (FILE * fp)
{
  FILE *loc_fp;
  BOOLEAN AOK = TRUE;

  if (fp)
  {
    fprintf (fp,
          "\n*******************************************************\n");
    fprintf (fp,
             "*******************************************************\n");
    fprintf (fp, "         VALIDATING INPUT GRIDS\n");
  }
  if (!igrid_ValidateUrbanGrids (fp))
    AOK = FALSE;
  if (!igrid_ValidateRoadGrids (fp))
    AOK = FALSE;
  if (!igrid_ValidateLanduseGrids (fp))
    AOK = FALSE;
  if (!igrid_ValidateSlopeGrid (fp))
    AOK = FALSE;
  if (!igrid_ValidateExcludedGrid (fp))
    AOK = FALSE;
  if (!igrid_ValidateBackgroundGrid (fp))
    AOK = FALSE;
  if (!AOK)
  {
    if (!fp)
      loc_fp = stderr;
    fprintf (loc_fp, "\nERROR\n");
    fprintf (loc_fp, "\nInput data images contain errors.\n");
    EXIT (1);
  }
  else
  {
    if (fp)
    {
      fprintf (fp, "\nValidation OK\n");
    }
  }
  if (fp)
  {
    fprintf (fp, "*******************************************************\n");
    fprintf (fp, "*******************************************************\n");
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateUrbanGrids
** PURPOSE:       check the validity of the urban grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static BOOLEAN
  igrid_ValidateUrbanGrids (FILE * fp)
{
  BOOLEAN rv = TRUE;
  int i;
  int j;

  for (i = 0; i < igrid.urban_count; i++)
  {
    if (fp)
    {
      fprintf (fp, "\nValidating urban input grid: %s\n",
               igrid.urban[i].filename);
      fprintf (fp, "\nIndex Count PercentOfImage\n");
    }
    for (j = 0; j < 256; j++)
    {
      if (igrid.urban[i].histogram[j] > 0)
      {
        if (fp)
        {
          fprintf (fp, "%3u  %5u  %8.2f%%\n", j, igrid.urban[i].histogram[j],
                   100.0 * igrid.urban[i].histogram[j] / total_pixels);
        }
      }
    }
    if (igrid.urban[i].histogram[0] == 0)
    {
      if (fp)
      {
        fprintf (fp, "ERROR input grid: %s is completely urbanized\n",
                 igrid.urban[i].filename);
      }
      else
      {
        fprintf (stderr, "ERROR input grid: %s is completely urbanized\n",
                 igrid.urban[i].filename);
      }
      rv = FALSE;
    }
  }
  return rv;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateRoadGrids
** PURPOSE:       check the validity of the road grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static BOOLEAN
  igrid_ValidateRoadGrids (FILE * fp)
{
  BOOLEAN rv = TRUE;
  int i;
  int j;

  for (i = 0; i < igrid.road_count; i++)
  {
    if (fp)
    {
      fprintf (fp, "\nValidating road input grid: %s\n", igrid.road[i].filename);
      fprintf (fp, "\nIndex Count PercentOfImage\n");
      for (j = 0; j < 256; j++)
      {
        if (igrid.road[i].histogram[j] > 0)
        {
          fprintf (fp, "%3u  %5u  %8.2f%%\n", j, igrid.road[i].histogram[j],
                   100.0 * igrid.road[i].histogram[j] / total_pixels);
        }
      }
    }
    if (igrid.road[i].histogram[0] == 0)
    {
      if (fp)
      {
        fprintf (fp, "ERROR input grid: %s is 100%% roads\n",
                 igrid.road[i].filename);
      }
      else
      {
        fprintf (stderr, "ERROR input grid: %s is 100%% roads\n",
                 igrid.road[i].filename);
      }
      rv = FALSE;
    }
  }
  return rv;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateLanduseGrids
** PURPOSE:       check validity of landuse grid values
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static BOOLEAN
  igrid_ValidateLanduseGrids (FILE * fp)
{
  char func[] = "igrid_ValidateLanduseGrids";
  BOOLEAN rv = TRUE;
  int i;
  int j;

  for (i = 0; i < igrid.landuse_count; i++)
  {
    if (fp)
    {
      fprintf (fp, "\nValidating landuse input grid: %s\n",
               igrid.landuse[i].filename);
      fprintf (fp, "\nIndex Count PercentOfImage\n");
    }
    for (j = 0; j < 256; j++)
    {
      if (igrid.landuse[i].histogram[j] > 0)
      {
        if (fp)
        {
          fprintf (fp, "%3u  %5u  %8.2f%%\n", j, igrid.landuse[i].histogram[j],
                   100.0 * igrid.landuse[i].histogram[j] / total_pixels);
        }
        if (!landclass_IsAlandclass (j))
        {
          rv = FALSE;
          sprintf (msg_buf, "landuse type %u appears in file: %s",
                   j, igrid.landuse[i].filename);
          LOG_ERROR (msg_buf);
        }
      }
    }
  }
  return rv;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateSlopeGrid
** PURPOSE:       check validity of slope grid values
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   could not think of good validity check!!!
**
**
*/
static BOOLEAN
  igrid_ValidateSlopeGrid (FILE * fp)
{
  if (fp)
  {
    fprintf (fp, "\nValidating slope input grid: %s\n", igrid.slope.filename);
  }
  return TRUE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateExcludedGrid
** PURPOSE:       check validity of excluded grid values
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   could not think of good validity check!!!
**
**
*/
static BOOLEAN
  igrid_ValidateExcludedGrid (FILE * fp)
{
  if (fp)
  {
    fprintf (fp, "\nValidating excluded input grid: %s\n",
             igrid.excluded.filename);
  }
  return TRUE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ValidateBackgroundGrid
** PURPOSE:       check validity of background grid values
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   could not think of good validity check!!!
**
**
*/
static BOOLEAN
  igrid_ValidateBackgroundGrid (FILE * fp)
{
  if (fp)
  {
    fprintf (fp, "\nValidating background input grid: %s\n",
             igrid.background.filename);
  }
  return TRUE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ReadGrid
** PURPOSE:       read input grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_ReadGrid (char *filepath, GRID_P scratch_pad, GRID_P grid_ptr)
{
#ifndef lint
  char func[] = "igrid_ReadGrid";
#endif
  char *filename;

  EXTRACT_FILENAME (filepath);
  if (scratch_pad != NULL)
  {
    if (glb_mype == 0)
    {
      gdif_ReadGIF (scratch_pad, filepath);
    }
#ifdef MPI
    MPI_Bcast (scratch_pad, memGetBytesPerGridRound (),
               MPI_BYTE, 0, MPI_COMM_WORLD);
#endif
    if (glb_mype == 0)
    {
      if (scen_GetEchoImageFlag ())
      {
        igrid_echo_input (scratch_pad, filename);
      }
    }
#ifdef PACKING
    _pack (scratch_pad, (char *) grid_ptr,
           mem_GetTotalPixels (), -1);
#else
    sprintf (msg_buf, "PACKING not defined");
    LOG_ERROR (msg_buf);
    exit (1);
#endif
  }
  else
  {
    if (glb_mype == 0)
    {
      gdif_ReadGIF (grid_ptr, filepath);
    }
#ifdef MPI
    MPI_Bcast (grid_ptr, memGetBytesPerGridRound (),
               MPI_BYTE, 0, MPI_COMM_WORLD);
#endif
    if (glb_mype == 0)
    {
      if (scen_GetEchoImageFlag ())
      {
        igrid_echo_input (grid_ptr, filename);
      }
    }
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_ReadFiles
** PURPOSE:       read input grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_ReadFiles ()
{
  char func[] = "igrid_ReadFiles";
  int i;
  GRID_P scratch_pad = NULL;
#ifdef PACKING
  scratch_pad = mem_GetWGridPtr (__FILE__, func, __LINE__);
  assert (scratch_pad != NULL);
#endif


  for (i = 0; i < igrid.urban_count; i++)
  {
    igrid.urban[i].ptr = mem_GetIGridPtr (func);
    igrid_ReadGrid (igrid.urban[i].filename,
                    scratch_pad, igrid.urban[i].ptr);
    grid_SetMinMax (&igrid.urban[i]);
    grid_histogram (&igrid.urban[i]);
  }

  for (i = 0; i < igrid.road_count; i++)
  {
    igrid.road[i].ptr = mem_GetIGridPtr (func);
    igrid_ReadGrid (igrid.road[i].filename,
                    scratch_pad, igrid.road[i].ptr);
    grid_SetMinMax (&igrid.road[i]);
    grid_histogram (&igrid.road[i]);
  }

  for (i = 0; i < igrid.landuse_count; i++)
  {
    igrid.landuse[i].ptr = mem_GetIGridPtr (func);
    igrid_ReadGrid (igrid.landuse[i].filename,
                    scratch_pad, igrid.landuse[i].ptr);
    grid_SetMinMax (&igrid.landuse[i]);
    grid_histogram (&igrid.landuse[i]);
  }

  igrid.excluded.ptr = mem_GetIGridPtr (func);
  igrid_ReadGrid (igrid.excluded.filename,
                  scratch_pad, igrid.excluded.ptr);
  grid_SetMinMax (&igrid.excluded);
  grid_histogram (&igrid.excluded);

  igrid.slope.ptr = mem_GetIGridPtr (func);
  igrid_ReadGrid (igrid.slope.filename,
                  scratch_pad, igrid.slope.ptr);
  grid_SetMinMax (&igrid.slope);
  grid_histogram (&igrid.slope);

  igrid.background.ptr = mem_GetIGridPtr (func);
  igrid_ReadGrid (igrid.background.filename,
                  scratch_pad, igrid.background.ptr);
  grid_SetMinMax (&igrid.background);
  grid_histogram (&igrid.excluded);

#ifdef PACKING
  scratch_pad = mem_GetWGridFree (__FILE__, func, __LINE__, scratch_pad);
#endif

  igrid_CountRoadPixels ();
  igrid_CalculatePercentRoads ();
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_init
** PURPOSE:       initialize some variables
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_init ()
{

  igrid_count = 0;
  igrid_SetLocation ();
  igrid_SetFilenames ();

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_SetLocation
** PURPOSE:       set the location string variable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_SetLocation ()
{
  char buf[256];

  strcpy (buf, scen_GetSlopeDataFilename ());
  strcpy (igrid.location, strtok (buf, "."));
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_SetFilenames
** PURPOSE:       set the filenames of the input files
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_SetFilenames ()
{
  int i;
  int j;
  int start_year;
  int this_year;
  char *this_year_str;
  char buf[256];
  BOOLEAN packed;

  start_year = scen_GetPredictionStartDate ();
#ifdef PACKING
  packed = TRUE;
#else
  packed = FALSE;
#endif

  j = 0;
  for (i = 0; i < scen_GetUrbanDataFileCount (); i++)
  {
    strcpy (buf, scen_GetUrbanDataFilename (i));
    strtok (buf, ".");
    strtok (NULL, ".");
    this_year_str = strtok (NULL, ".");
    this_year = atoi (this_year_str);
    if (proc_GetProcessingType () == PREDICTING)
    {
      if (this_year >= start_year)
      {
        strcpy (igrid.urban[j].filename, scen_GetInputDir ());
        strcat (igrid.urban[j].filename, scen_GetUrbanDataFilename (i));
        igrid_SetGridSizes (&igrid.urban[j]);
        strcpy (igrid.urban[j].year.string, this_year_str);
        igrid.urban[j].year.digit = this_year;
        igrid.urban[j].packed = packed;
        j++;
      }
    }
    else
    {
      strcpy (igrid.urban[j].filename, scen_GetInputDir ());
      strcat (igrid.urban[j].filename, scen_GetUrbanDataFilename (i));
      igrid_SetGridSizes (&igrid.urban[j]);
      strcpy (igrid.urban[j].year.string, this_year_str);
      igrid.urban[j].year.digit = this_year;
      igrid.urban[j].packed = packed;
      j++;
    }
  }
  igrid.urban_count = j;

  j = 0;
  for (i = 0; i < scen_GetRoadDataFileCount (); i++)
  {
    strcpy (buf, scen_GetRoadDataFilename (i));
    strtok (buf, ".");
    strtok (NULL, ".");
    this_year_str = strtok (NULL, ".");
    this_year = atoi (this_year_str);
    if (proc_GetProcessingType () == PREDICTING)
    {
      if ((this_year >= start_year) | (i = scen_GetRoadDataFileCount () - 1))
      {
        strcpy (igrid.road[j].filename, scen_GetInputDir ());
        strcat (igrid.road[j].filename, scen_GetRoadDataFilename (i));
        igrid_SetGridSizes (&igrid.road[j]);
        strcpy (igrid.road[j].year.string, this_year_str);
        igrid.road[j].year.digit = this_year;
        igrid.road[j].packed = packed;
        j++;
      }
    }
    else
    {
      strcpy (igrid.road[j].filename, scen_GetInputDir ());
      strcat (igrid.road[j].filename, scen_GetRoadDataFilename (i));
      igrid_SetGridSizes (&igrid.road[j]);
      strcpy (igrid.road[j].year.string, this_year_str);
      igrid.road[j].year.digit = this_year;
      igrid.road[j].packed = packed;
      j++;
    }
  }
  igrid.road_count = j;

  for (i = 0; i < scen_GetLanduseDataFileCount (); i++)
  {
    strcpy (buf, scen_GetLanduseDataFilename (i));
    strtok (buf, ".");
    strtok (NULL, ".");
    this_year_str = strtok (NULL, ".");
    this_year = atoi (this_year_str);
    strcpy (igrid.landuse[i].filename, scen_GetInputDir ());
    strcat (igrid.landuse[i].filename, scen_GetLanduseDataFilename (i));
    strcpy (igrid.landuse[i].year.string, this_year_str);
    igrid.landuse[i].year.digit = this_year;
    igrid_SetGridSizes (&igrid.landuse[i]);
    igrid.landuse[i].packed = packed;
  }
  igrid.landuse_count = scen_GetLanduseDataFileCount ();

  strcpy (igrid.excluded.filename, scen_GetInputDir ());
  strcat (igrid.excluded.filename, scen_GetExcludedDataFilename ());
  igrid_SetGridSizes (&igrid.excluded);
  strcpy (igrid.excluded.year.string, "");
  igrid.excluded.year.digit = 0;
  igrid.excluded.packed = packed;
  igrid.excluded_count = 1;

  strcpy (igrid.slope.filename, scen_GetInputDir ());
  strcat (igrid.slope.filename, scen_GetSlopeDataFilename ());
  igrid_SetGridSizes (&igrid.slope);
  strcpy (igrid.slope.year.string, "");
  igrid.slope.year.digit = 0;
  igrid.slope.packed = packed;
  igrid.slope_count = 1;

  strcpy (igrid.background.filename, scen_GetInputDir ());
  strcat (igrid.background.filename, scen_GetBackgroundDataFilename ());
  igrid_SetGridSizes (&igrid.background);
  strcpy (igrid.background.year.string, "");
  igrid.background.year.digit = 0;
  igrid.background.packed = packed;
  igrid.background_count = 1;

  igrid_count = igrid.urban_count +
    igrid.road_count +
    igrid.landuse_count +
    igrid.excluded_count +
    igrid.slope_count +
    igrid.background_count;

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_SetGridSizes
** PURPOSE:       scan the input GIFs for size and other parameters
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_SetGridSizes (grid_info * grid_ptr)
{
#define BYTES2READ 15
#define GIF_ID "GIF"
#define GIF_ROW_OFFSET 8
#define GIF_COL_OFFSET 6
#define GIF_RES_OFFSET 10
#define CONVERT2UINT(a,b) (((b)<<8)|(a))
  char func[] = "igrid_SetGridSizes";
  FILE *fp;
  unsigned char buffer[BYTES2READ + 1];
  char id_str[7];
  int resolution;
  int ncols;
  int nrows;
  int bits_per_pixel;
  int color_bits;


  if (glb_mype == 0)
  {
    FILE_OPEN (fp, grid_ptr->filename, "rb");
    fgets ((char *) buffer, BYTES2READ, fp);
    fclose (fp);
  }
#ifdef MPI
  MPI_Bcast (buffer, BYTES2READ, MPI_BYTE, 0, MPI_COMM_WORLD);
#endif


  strncpy (id_str, (char *) buffer, strlen (GIF_ID));
  if (strncmp (GIF_ID, id_str, strlen (GIF_ID)) != 0)
  {
    printf ("\n\n%s %d file: %s is not a %s format\n",
            __FILE__, __LINE__, grid_ptr->filename, GIF_ID);
    EXIT (1);
  }
  ncols = CONVERT2UINT (buffer[GIF_COL_OFFSET], buffer[GIF_COL_OFFSET + 1]);
  nrows = CONVERT2UINT (buffer[GIF_ROW_OFFSET], buffer[GIF_ROW_OFFSET + 1]);
  total_pixels = nrows * ncols;
  resolution = (int) buffer[GIF_RES_OFFSET];
  color_bits = (((resolution & 112) >> 4) + 1);
  bits_per_pixel = (resolution & 7) + 1;
  grid_ptr->ncols = ncols;
  grid_ptr->nrows = nrows;
  grid_ptr->color_bits = color_bits;
  grid_ptr->bits_per_pixel = bits_per_pixel;
  grid_ptr->size_bytes = BYTES_PER_PIXEL * ncols * nrows;
  grid_ptr->size_words = ROUND_BYTES_TO_WORD_BNDRY (grid_ptr->size_bytes);

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_LogIt
** PURPOSE:       log the igrid structs to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_LogIt (FILE * fp)
{
  char func[] = "igrid_LogIt";
  char buf[256];
  char asterisks[256];
  int i;

  FUNC_INIT;
  fprintf (fp, "\n\n");
  sprintf (asterisks, "%s", "************************************");
  strcat (asterisks, "************************************");
  out_center_text (fp,
                   asterisks,
                   "INPUT GIFs",
                   buf,
                   -1);
  fprintf (fp, "\n\n");
  fprintf (fp, "  Urban GIFs\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  fprintf (fp, "      rowXcol cb bpp min max path\n");

  for (i = 0; i < igrid.urban_count; i++)
  {
    sprintf (buf, "%uX%u", igrid.urban[i].nrows, igrid.urban[i].ncols);
    fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
             igrid.urban[i].color_bits,
             igrid.urban[i].bits_per_pixel,
             igrid.urban[i].min,
             igrid.urban[i].max,
             igrid.urban[i].filename);
  }
  fprintf (fp, "  Road GIFs\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  for (i = 0; i < igrid.road_count; i++)
  {
    sprintf (buf, "%uX%u", igrid.road[i].nrows, igrid.road[i].ncols);
    fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
             igrid.road[i].color_bits,
             igrid.road[i].bits_per_pixel,
             igrid.road[i].min,
             igrid.road[i].max,
             igrid.road[i].filename);
  }
  fprintf (fp, "  Landuse GIFs\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  for (i = 0; i < igrid.landuse_count; i++)
  {
    sprintf (buf, "%uX%u", igrid.landuse[i].nrows, igrid.landuse[i].ncols);
    fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
             igrid.landuse[i].color_bits,
             igrid.landuse[i].bits_per_pixel,
             igrid.landuse[i].min,
             igrid.landuse[i].max,
             igrid.landuse[i].filename);
  }
  fprintf (fp, "  Excluded GIF\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  sprintf (buf, "%uX%u", igrid.excluded.nrows, igrid.excluded.ncols);
  fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
           igrid.excluded.color_bits,
           igrid.excluded.bits_per_pixel,
           igrid.excluded.min,
           igrid.excluded.max,
           igrid.excluded.filename);

  fprintf (fp, "  Slope GIF\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  sprintf (buf, "%uX%u", igrid.slope.nrows, igrid.slope.ncols);
  fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
           igrid.slope.color_bits,
           igrid.slope.bits_per_pixel,
           igrid.slope.min,
           igrid.slope.max,
           igrid.slope.filename);

  fprintf (fp, "  Background GIF\n");
  fprintf (fp, "      rowXcol cb bpp path\n");
  sprintf (buf, "%uX%u", igrid.background.nrows, igrid.background.ncols);
  fprintf (fp, "    %9s  %u  %u  %3u %3u %s\n", buf,
           igrid.background.color_bits,
           igrid.background.bits_per_pixel,
           igrid.background.min,
           igrid.background.max,
           igrid.background.filename);

  fprintf (fp, "cb = # of color bits\n");
  fprintf (fp, "bpp = # bits per pixel\n");
  fprintf (fp, "\n\n");

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_VerifyInputs
** PURPOSE:       verify the grid sizes and some other stuff
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  igrid_VerifyInputs (FILE * fp)
{
  char func[] = "igrid_VerifyInputs";
  char buf[256];
  char *my_location;
  char *filename;
  int i;
  int rows;
  int cols;
  BOOLEAN all_sizes_the_same = TRUE;
  BOOLEAN all_locations_the_same = TRUE;

#define CHECK_LOCATION(path)                                 \
strcpy (buf, (path));                                        \
EXTRACT_FILENAME(buf);                                       \
my_location = strtok (filename, ".");                        \
if(strcmp(my_location,igrid.location) != 0)                  \
{                                                            \
  all_locations_the_same = FALSE;                            \
}

  if (fp)
  {
    fprintf (fp, "\nVerifying Data Input Files\n");
  }
  rows = igrid.slope.nrows;
  cols = igrid.slope.ncols;
  for (i = 0; i < igrid.urban_count; i++)
  {
    if ((rows != igrid.urban[i].nrows) || (cols != igrid.urban[i].ncols))
    {
      all_sizes_the_same = FALSE;
    }
    CHECK_LOCATION (igrid.urban[i].filename);
  }
  for (i = 0; i < igrid.road_count; i++)
  {
    if ((rows != igrid.road[i].nrows) || (cols != igrid.road[i].ncols))
    {
      all_sizes_the_same = FALSE;
    }
    CHECK_LOCATION (igrid.road[i].filename);
  }
  for (i = 0; i < igrid.landuse_count; i++)
  {
    if ((rows != igrid.landuse[i].nrows) || (cols != igrid.landuse[i].ncols))
    {
      all_sizes_the_same = FALSE;
    }
    CHECK_LOCATION (igrid.landuse[i].filename);
  }
  if ((rows != igrid.excluded.nrows) || (cols != igrid.excluded.ncols))
  {
    all_sizes_the_same = FALSE;
    CHECK_LOCATION (igrid.excluded.filename);
  }

  if ((rows != igrid.background.nrows) || (cols != igrid.background.ncols))
  {
    all_sizes_the_same = FALSE;
    CHECK_LOCATION (igrid.background.filename);
  }
  if (!all_sizes_the_same)
  {
    sprintf (msg_buf, "GIFs are not all the same size.");
    LOG_ERROR (msg_buf);
    sprintf (msg_buf, "Please check your input image sizes");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (!all_locations_the_same)
  {
    sprintf (msg_buf, "GIFs do not all have the same location.");
    LOG_ERROR (msg_buf);
    sprintf (msg_buf, "Please check your scenario file");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (scen_GetDoingLanduseFlag ())
  {
    if (igrid.landuse[1].year.digit !=
        igrid.urban[igrid.urban_count - 1].year.digit)
    {
      sprintf (msg_buf, "Last landuse year does not match last urban year.");
      LOG_ERROR (msg_buf);
      sprintf (msg_buf, "last landuse year = %u last urban year = %u",
               igrid.landuse[1].year.digit,
               igrid.urban[igrid.urban_count - 1].year.digit);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
  if (fp)
  {
    fprintf (fp, "%s %u Data Input Files: OK\n", __FILE__, __LINE__);
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_CountRoadPixels
** PURPOSE:       count the number of road pixels
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_CountRoadPixels ()
{
  char func[] = "igrid_CountRoadPixel";
  GRID_P roads;
  int i;

  assert (total_pixels > 0);

  for (i = 0; i < igrid.road_count; i++)
  {
#ifdef PACKING
    roads = mem_GetWGridPtr (__FILE__, func, __LINE__);
    _unpack ((char *) igrid.road[i].ptr,
             roads,
             total_pixels,
             -1);
#else
    roads = igrid.road[i].ptr;
#endif
    road_pixel_count[i] = util_count_pixels (total_pixels,
                                             roads,
                                             GT,
                                             0);
#ifdef PACKING
    mem_GetWGridFree (__FILE__, func, __LINE__, roads);
#endif
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_CalculatePercentRoads
** PURPOSE:       calculate road percentage
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_CalculatePercentRoads ()
{
  char func[] = "igrid_CalculatePercentRoads";
  GRID_P excld;
  int i;


  assert (total_pixels > 0);
#ifdef PACKING
  excld = mem_GetWGridPtr (__FILE__, func, __LINE__);
  _unpack ((char *) igrid.excluded.ptr,
           excld,
           total_pixels,
           -1);
#else
  excld = igrid.excluded.ptr;
#endif
  excld_count = util_count_pixels (total_pixels,
                                   excld,
                                   GE,
                                   100);
  if (total_pixels - excld_count <= 0)
  {
    sprintf (msg_buf, "mem_GetTotalPixels()=%d excld_count = %d\n",
             mem_GetTotalPixels (), excld_count);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  for (i = 0; i < igrid.road_count; i++)
  {
    percent_road[i] =
      (100.0 * road_pixel_count[i]) / (total_pixels - excld_count);
  }
#ifdef PACKING
  mem_GetWGridFree (__FILE__, func, __LINE__, excld);
#endif
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_TestForUrbanYear
** PURPOSE:       test if year matches an urban year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  igrid_TestForUrbanYear (int year)
{
  int i;

  for (i = 0; i < igrid.urban_count; i++)
  {
    if (igrid.urban[i].year.digit == year)
      return TRUE;
  }
  return FALSE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_UrbanYear2Index
** PURPOSE:       convert an urban year into an urban index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  igrid_UrbanYear2Index (int year)
{
  BOOLEAN flag = FALSE;
  int i;

  for (i = 0; i < igrid.urban_count; i++)
  {
    if (igrid.urban[i].year.digit == year)
    {
      flag = TRUE;
      break;
    }
  }
  if (flag)
  {
    return i;
  }
  else
  {
    printf ("%s %u ERROR year=%u is not an urban year\n",
            __FILE__, __LINE__, year);
    EXIT (1);
  }
  return -1;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_TestForRoadYear
** PURPOSE:       test if year is a road year
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  igrid_TestForRoadYear (int year)
{
  int i;

  for (i = 0; i < igrid.road_count; i++)
  {
    if (igrid.road[i].year.digit == year)
      return TRUE;
  }
  return FALSE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: igrid_echo_input
** PURPOSE:       routine to echo input grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  igrid_echo_input (GRID_P ptr, char *filename)
{
  char path[MAX_FILENAME_LEN];
  char date_str[] = "";

  sprintf (path, "%secho_of_%s", scen_GetOutputDir (), filename);
  gdif_WriteGIF (ptr,
                 color_GetColortable (GRAYSCALE_COLORTABLE),
                 path,
                 date_str,
                 255);
}
