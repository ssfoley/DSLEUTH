/******************************************************************************
*******************************************************************************
**                           MODULE PROLOG                                   **
*******************************************************************************
This object encapsulates the basic gird structures.


*******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ugm_defines.h"
#include "ugm_macros.h"
#include "grid_obj.h"
#include "memory_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char grid_obj_c_sccs_id[] = "@(#)grid_obj.c	1.84	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grid_SetMinMax
** PURPOSE:       find the min and max for a given grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  grid_SetMinMax (grid_info * grid_info_ptr)
{
  char func[] = "grid_SetMinMax";
#ifdef PACKING
  int i;
  int total_pixels;
  int min;
  int max;
  GRID_P unpacked_ptr;

  total_pixels = grid_info_ptr->ncols * grid_info_ptr->nrows;
  unpacked_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);

  _unpack ((char *) grid_info_ptr->ptr,
           unpacked_ptr,
           total_pixels,
           -1);

  min = unpacked_ptr[0];
  max = unpacked_ptr[0];
  for (i = 0; i < total_pixels; i++)
  {
    min = MIN (min, unpacked_ptr[i]);
    max = MAX (max, unpacked_ptr[i]);
  }
  mem_GetWGridFree (__FILE__, func, __LINE__, unpacked_ptr);
  grid_info_ptr->min = min;
  grid_info_ptr->max = max;
#else
  int i;
  int total_pixels;
  int min;
  int max;

  min = grid_info_ptr->ptr[0];
  max = grid_info_ptr->ptr[0];
  total_pixels = grid_info_ptr->ncols * grid_info_ptr->nrows;
  for (i = 0; i < total_pixels; i++)
  {
    min = MIN (min, grid_info_ptr->ptr[i]);
    max = MAX (max, grid_info_ptr->ptr[i]);
  }
  grid_info_ptr->min = min;
  grid_info_ptr->max = max;
#endif
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grid_histogram
** PURPOSE:       histogram the values in a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  grid_histogram (grid_info * grid_ptr)
{
  int i;

  for (i = 0; i < 256; i++)
  {
    grid_ptr->histogram[i] = 0;
  }
  for (i = 0; i < grid_ptr->nrows * grid_ptr->ncols; i++)
  {
    grid_ptr->histogram[grid_ptr->ptr[i]]++;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: grid_dump
** PURPOSE:       dump some of the values pertaining to a given grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  grid_dump (FILE * fp, grid_info * grid_ptr)
{
  int i;
  int total_pixels;

  total_pixels = grid_ptr->nrows * grid_ptr->ncols;
  LOG_INT (fp, grid_ptr);
  LOG_STRING (fp, grid_ptr->filename);
  LOG_INT (fp, grid_ptr->ptr);
  LOG_INT (fp, grid_ptr->packed);
  LOG_INT (fp, grid_ptr->color_bits);
  LOG_INT (fp, grid_ptr->bits_per_pixel);
  LOG_INT (fp, grid_ptr->size_words);
  LOG_INT (fp, grid_ptr->size_bytes);
  LOG_INT (fp, grid_ptr->nrows);
  LOG_INT (fp, grid_ptr->ncols);
  LOG_INT (fp, grid_ptr->max);
  LOG_INT (fp, grid_ptr->min);
  LOG_INT (fp, grid_ptr->year.digit);
  LOG_STRING (fp, grid_ptr->year.string);
  fprintf (fp, "%s %u Index Count PercentOfImage\n", __FILE__, __LINE__);
  for (i = 0; i < 256; i++)
  {
    if (grid_ptr->histogram[i] > 0)
    {
      fprintf (fp, "%s %u grid_ptr->histogram[%3u]=%5u %8.2f%%\n", __FILE__, __LINE__, i,
               grid_ptr->histogram[i], 100.0 * grid_ptr->histogram[i] / total_pixels);
    }
  }
}
