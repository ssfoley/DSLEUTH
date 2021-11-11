#define UTILITIES_MODULE
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "coeff_obj.h"
#include "scenario_obj.h"
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "utilities.h"
#include "random.h"
#include "input.h"
#include "ugm_macros.h"
#include "memory_obj.h"
#include "proc_obj.h"
#include "color_obj.h"
#include "gdif_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char utilities_c_sccs_id[] = "@(#)utilities.c	1.479	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_copy_grid
** PURPOSE:       copy grid from source to target
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_copy_grid (GRID_P source,                             /* IN     */
                  GRID_P target)                           /* OUT    */
{
  char func[] = "util_copy_grid";
  int total_pixels;

  FUNC_INIT;
  assert (source != NULL);
  assert (target != NULL);
  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);
  memcpy (target, source, sizeof (PIXEL) * total_pixels);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_init_grid
** PURPOSE:       initialize a grid with value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_init_grid (GRID_P gif,                                /* OUT    */
                  PIXEL value                                /* IN     */
  )                                                        /* IN     */
{
  char func[] = "util_init_grid";
  int i;
  int total_pixels;

  FUNC_INIT;
  total_pixels = mem_GetTotalPixels ();

  assert (gif != NULL);
  assert (total_pixels > 0);

  for (i = 0; i < total_pixels; i++)
  {
    gif[i] = value;
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_condition_gif
** PURPOSE:       set the pixels in target based on values in source grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_condition_gif (int num_pixels,                        /* IN     */
                      GRID_P source,                         /* IN     */
                      int option,                            /* IN     */
                      int cmp_value,                         /* IN     */
                      GRID_P target,                         /* OUT    */
                      int set_value)                       /* IN     */
{
  char func[] = "util_condition_gif";
  int i;

  FUNC_INIT;
  assert (num_pixels > 0);
  assert (source != NULL);
  assert (target != NULL);

  switch (option)
  {
  case LT:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] < cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  case LE:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] <= cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  case EQ:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] == cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  case NE:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] != cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  case GE:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] >= cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  case GT:
    for (i = 0; i < num_pixels; i++)
    {
      if (source[i] > cmp_value)
      {
        target[i] = set_value;
      }
    }
    break;
  default:
    sprintf (msg_buf, "Unknown option = %d", option);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_img_intersection
** PURPOSE:       count the # of similar pixels in two given grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  util_img_intersection (int num_pixels,                     /* IN     */
                         GRID_P ptr1,                        /* IN     */
                         GRID_P ptr2)                      /* IN     */
{
  char func[] = "util_image_intersection";
  int i;
  int count = 0;

  FUNC_INIT;
  assert (num_pixels > 0);
  assert (ptr1 != NULL);
  assert (ptr2 != NULL);

  for (i = 0; i < num_pixels; i++)
  {
    if (ptr1[i] == ptr2[i])
    {
      count++;
    }
  }
  FUNC_END;
  return count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_count_pixels
** PURPOSE:       count pixels meeting option & value conditionals
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  util_count_pixels (int num_pixels,                         /* IN     */
                     GRID_P pixels,                          /* IN     */
                     int option,                             /* IN     */
                     int value)                            /* IN     */
{
  char func[] = "util_count_pixels";
  int i;
  int count = 0;

  FUNC_INIT;
  assert (num_pixels > 0);
  assert (pixels != NULL);

  switch (option)
  {
  case LT:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] < value)
      {
        count++;
      }
    }
    break;
  case LE:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] <= value)
      {
        count++;
      }
    }
    break;
  case EQ:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] == value)
      {
        count++;
      }
    }
    break;
  case NE:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] != value)
      {
        count++;
      }
    }
    break;
  case GE:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] >= value)
      {
        count++;
      }
    }
    break;
  case GT:
    for (i = 0; i < num_pixels; i++)
    {
      if (pixels[i] > value)
      {
        count++;
      }
    }
    break;
  default:
    sprintf (msg_buf, "Unknown option = %d", option);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
  return (count);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_merge_background
** PURPOSE:       merge the background image with the foreground image
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_merge_background (GRID_P foreground_gif,              /* IN     */
                         GRID_P background_gif,              /* IN     */
                         GRID_P merged_gif)                /* OUT    */
{
  char func[] = "util_merge_background";
  int i;
  int fore_pixel;
  int back_pixel;
  int merge_pixel;
  int total_pixels;

  FUNC_INIT;
  assert (foreground_gif != NULL);
  assert (background_gif != NULL);
  assert (merged_gif != NULL);
  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);

  for (i = 0; i < total_pixels; i++)
  {
    fore_pixel = foreground_gif[i];
    back_pixel = background_gif[i];
    if (fore_pixel < 50)
    {
      if (back_pixel > 11)
      {
        merge_pixel = back_pixel;
      }
      else
      {
        merge_pixel = 12;
      }
    }
    else
    {
      if (fore_pixel == 100)
      {
        merge_pixel = 11;
      }
      else
      {
        merge_pixel = ((fore_pixel - 50) / 5) + 1;
      }
    }
    merged_gif[i] = merge_pixel;
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_trim
** PURPOSE:       trim string from left and right
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  util_trim (char s[])                                     /* IN/OUT */
{
  int n;
  char *temp;
  char func[] = "util_trim";

  FUNC_INIT;

  if (s != NULL)
  {
    temp = s;

    for (n = strlen (s) - 1; n >= 0; n--)
    {
      if ((s[n] != ' ') && (s[n] != '\t' && s[n] != '\n'))
        break;
    }
    s[n + 1] = '\0';
    for (n = 0; n < strlen (s); n++)
    {
      if ((s[n] != ' ') && (s[n] != '\t' && s[n] != '\n'))
      {
        temp = &s[n];
        break;
      }
    }
    if (temp != s)
    {
      for (n = 0; n <= strlen (temp); n++)
      {
        s[n] = temp[n];
      }
    }
  }
  FUNC_END;
  return strlen (s);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_get_neighbor
** PURPOSE:       return a randomly selected neighbor
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_get_neighbor (int i_in,                               /* IN     */
                     int j_in,                               /* IN     */
                     int *i_out,                             /* OUT    */
                     int *j_out)                           /* OUT    */
{
  char func[] = "util_get_neighbor";

  int random_int;
  int i_adj;
  int j_adj;
  int row[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
  int col[8] = {-1, -1, -1, 0, 1, 1, 1, 0};

  FUNC_INIT;
  assert (i_out != NULL);
  assert (j_out != NULL);

  /*
   *
   *    --------------------------------------
   *    |0          |7           |6          |
   *    |  (-1,-1)  |  (-1, 0)   |  (-1, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *    |1          |            |5          |
   *    |  ( 0,-1)  |(i_in,j_in) |  ( 0, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *    |2          |3           |4          |
   *    |  ( 1,-1)  |  ( 1, 0)   |  ( 1, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *
   *
   * row[] and col[] contain offsets from (i_in,j_in)
   *
   * i_out - row coord. of (i_in,j_in) randomly selected neighbor
   * j_out - col coord. of (i_in,j_in) randomly selected neighbor
   *
   */

  random_int = RANDOM_INT (8);
  i_adj = row[random_int];
  j_adj = col[random_int];
  (*i_out) = i_in + i_adj;
  (*j_out) = j_in + j_adj;

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_count_neighbors
** PURPOSE:       count the neighbors meeting certain criteria
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  util_count_neighbors (GRID_P grid,                         /* IN     */
                        int i,                               /* IN     */
                        int j,                               /* IN     */
                        int option,                          /* IN     */
                        PIXEL value)                       /* IN     */
{
  char func[] = "util_count_neighbors";
  int count = 0;

  FUNC_INIT;
  assert (grid != NULL);

  switch (option)
  {
  case LT:
    count =
      ((grid[OFFSET (i - 1, j - 1)] < value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] < value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] < value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] < value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] < value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] < value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] < value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] < value) ? 1 : 0);
    break;
  case LE:
    count =
      ((grid[OFFSET (i - 1, j - 1)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] <= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] <= value) ? 1 : 0);
    break;
  case EQ:
    count =
      ((grid[OFFSET (i - 1, j - 1)] == value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] == value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] == value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] == value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] == value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] == value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] == value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] == value) ? 1 : 0);
    break;
  case NE:
    count =
      ((grid[OFFSET (i - 1, j - 1)] != value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] != value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] != value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] != value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] != value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] != value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] != value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] != value) ? 1 : 0);
    break;
  case GE:
    count =
      ((grid[OFFSET (i - 1, j - 1)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] >= value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] >= value) ? 1 : 0);
    break;
  case GT:
    count =
      ((grid[OFFSET (i - 1, j - 1)] > value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j)] > value) ? 1 : 0) +
      ((grid[OFFSET (i - 1, j + 1)] > value) ? 1 : 0) +
      ((grid[OFFSET (i, j - 1)] > value) ? 1 : 0) +
      ((grid[OFFSET (i, j + 1)] > value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j - 1)] > value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j)] > value) ? 1 : 0) +
      ((grid[OFFSET (i + 1, j + 1)] > value) ? 1 : 0);
    break;
  default:
    sprintf (msg_buf, "Unknown option = %d", option);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
  return (count);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_get_next_neighbor
** PURPOSE:       return next neighbor in a sequence
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_get_next_neighbor (int i_in,                          /* IN     */
                          int j_in,                          /* IN     */
                          int *i_out,                        /* OUT    */
                          int *j_out,                        /* OUT    */
                          int index)                       /* IN     */
{
  char func[] = "util_get_next_neighbor";

  static int last_index;
  int i_adj;
  int j_adj;
  int row[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
  int col[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
  int nrows;
  int ncols;

  FUNC_INIT;
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > i_in);
  assert (ncols > j_in);
  assert (i_in >= 0);
  assert (j_in >= 0);
  assert (i_out != NULL);
  assert (j_out != NULL);
  assert (index >= -1);
  assert (index <= 7);

  /*
   *
   *    --------------------------------------
   *    |0          |7           |6          |
   *    |  (-1,-1)  |  (-1, 0)   |  (-1, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *    |1          |            |5          |
   *    |  ( 0,-1)  |(i_in,j_in) |  ( 0, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *    |2          |3           |4          |
   *    |  ( 1,-1)  |  ( 1, 0)   |  ( 1, 1)  |
   *    |           |            |           |
   *    --------------------------------------
   *
   *
   * row[] and col[] contain offsets from (i_in,j_in)
   *
   * if index has a value from 0 to 7, then the offsets
   * coresponding to index in row[] and col[] are
   * returned
   *
   * if index = -1 then the next set of offsets are returned.
   */

  if (index == -1)
  {
    last_index++;
    last_index = (last_index) % 8;
  }
  else
  {
    last_index = index;
  }
  i_adj = row[last_index];
  j_adj = col[last_index];
  (*i_out) = i_in + i_adj;
  (*j_out) = j_in + j_adj;

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_map_gridpts_2_index
** PURPOSE:       map selected pixels into a new index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_map_gridpts_2_index (GRID_P in,
                            GRID_P out,
                            int *lower_bound,
                            int *upper_bound,
                            int *index,
                            int count)
{
  char func[] = "util_map_gridpts_2_index";
  int i;
  int j;
  int total_pixels;

  assert (in != NULL);
  assert (out != NULL);
  assert (lower_bound != NULL);
  assert (upper_bound != NULL);
  assert (index != NULL);
  assert (count > 0);
  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);

  FUNC_INIT;
  for (i = 0; i < total_pixels; i++)
  {
    for (j = 0; j < count; j++)
    {
      if ((in[i] >= lower_bound[j]) && (in[i] <= upper_bound[j]))
      {
        out[i] = index[j];
        break;
      }
      else
      {
        out[i] = in[i];
      }
    }
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_overlay
** PURPOSE:       overlay one image onto another
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_overlay (GRID_P layer0,
                GRID_P layer1,
                GRID_P out)
{
  char func[] = "util_overlay";
  int i;
  int total_pixels;

  assert (layer0 != NULL);
  assert (layer1 != NULL);
  assert (out != NULL);
  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);

  FUNC_INIT;

  for (i = 0; i < total_pixels; i++)
  {
    if (layer1[i] > 0)
    {
      out[i] = layer1[i];
    }
    else
    {
      out[i] = layer0[i];
    }
  }

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_AllCAPS
** PURPOSE:       convert a string to all caps
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_AllCAPS (char *str_ptr)
{
  int i;

  for (i = 0; i < strlen (str_ptr); i++)
  {
    str_ptr[i] = toupper (str_ptr[i]);
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_WriteZProbGrid
** PURPOSE:       write probability grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_WriteZProbGrid (GRID_P z_ptr, char name[])
{
  char func[] = "util_WriteZProbGrid";
  char filename[MAX_FILENAME_LEN];
  char date_str[4];
  int lower_bounds[MAX_PROBABILITY_COLORS];
  int upper_bounds[MAX_PROBABILITY_COLORS];
  int indices[MAX_PROBABILITY_COLORS];
  int index_count;
  GRID_P background_ptr;
  GRID_P z_prob_ptr;
  GRID_P overlay_ptr;
  int i;

  overlay_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
  assert (overlay_ptr != NULL);
  z_prob_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
  assert (z_prob_ptr != NULL);


  /*
   * COPY BACKGROUND INTO Z_PROB_PTR AND REMAP BACKGROUND PIXELS
   * WHICH COLLIDE WITH THE SEED, PROBABILITY COLORS, AND THE
   * DATE
   *
   */
  background_ptr = igrid_GetBackgroundGridPtr (__FILE__, func, __LINE__);
  assert (background_ptr != NULL);


  lower_bounds[0] = SEED_COLOR_INDEX;
  upper_bounds[0] = SEED_COLOR_INDEX + scen_GetProbabilityColorCount ();
  indices[0] = SEED_COLOR_INDEX + scen_GetProbabilityColorCount () + 1;

  lower_bounds[1] = DATE_COLOR_INDEX;
  upper_bounds[1] = DATE_COLOR_INDEX;
  indices[1] = DATE_COLOR_INDEX - 1;

  index_count = 2;
  util_map_gridpts_2_index (background_ptr,
                            z_prob_ptr,
                            lower_bounds,
                            upper_bounds,
                            indices,
                            index_count);

  background_ptr =
    igrid_GridRelease (__FILE__, func, __LINE__, background_ptr);

  if (proc_GetProcessingType () == PREDICTING)
  {
    /*
     *
     * MAP Z_PTR PIXELS INTO DESIRED PROBABILITY INDICES 
     * AND SAVE IN OVERLAY_PTR
     *
     */
    lower_bounds[0] = scen_GetProbabilityColorLowerBound (0);
    upper_bounds[0] = scen_GetProbabilityColorUpperBound (0);
    indices[0] = 0;
    for (i = 1; i < scen_GetProbabilityColorCount (); i++)
    {
      lower_bounds[i] = scen_GetProbabilityColorLowerBound (i);
      upper_bounds[i] = scen_GetProbabilityColorUpperBound (i);
      indices[i] = i + 2;
    }
    util_map_gridpts_2_index (z_ptr,
                              overlay_ptr,
                              lower_bounds,
                              upper_bounds,
                              indices,
                              scen_GetProbabilityColorCount ());

    /*
     *
     * OVERLAY  OVERLAY_PTR ONTO THE Z_PROB_PTR GRID
     *
     */
    util_overlay (z_prob_ptr,
                  overlay_ptr,
                  z_prob_ptr);
    /*
     *
     * OVERLAY  URBAN_SEED_PTR ONTO THE Z_PROB_PTR GRID
     *
     */

    util_overlay_seed (z_prob_ptr);

  }
  /*end IF PREDICTING */
  else
    /*TESTING*/
  {
    /*
     *
     * MAP Z_PTR PIXELS INTO DESIRED SEED_COLOR_INDEX 
     * AND SAVE IN OVERLAY_PTR
     *
     */
    lower_bounds[0] = 1;
    upper_bounds[0] = 100;
    indices[0] = SEED_COLOR_INDEX;

    util_map_gridpts_2_index (z_ptr,
                              overlay_ptr,
                              lower_bounds,
                              upper_bounds,
                              indices,
                              1);

    /*
     *
     * OVERLAY  OVERLAY_PTR ONTO THE Z_PROB_PTR GRID
     *
     */
    util_overlay (z_prob_ptr,
                  overlay_ptr,
                  z_prob_ptr);

  }
  /*END ELSE TESTING */

  overlay_ptr = mem_GetWGridFree (__FILE__, func, __LINE__, overlay_ptr);

  /*
   *
   * WRITE OUT PROBABILITY IMAGE
   *
   */
  sprintf (filename, "%s%s%s%u.gif",
           scen_GetOutputDir (),
           igrid_GetLocation (),
           name,
           proc_GetCurrentYear ());
  sprintf (date_str, "%u", proc_GetCurrentYear ());
  gdif_WriteGIF (z_prob_ptr,
                 color_GetColortable (PROBABILITY_COLORTABLE),
                 filename,
                 date_str,
                 DATE_COLOR_INDEX);

  z_prob_ptr = mem_GetWGridFree (__FILE__, func, __LINE__, z_prob_ptr);

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: util_overlay_seed
** PURPOSE:       overlay the seed onto a probability image
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  util_overlay_seed (GRID_P z_prob_ptr)
{
  char func[] = "util_overlay_seed";

  int lower_bounds[1];
  int upper_bounds[1];
  int index[1];
  int index_count;
  GRID_P urban_seed_ptr;
  GRID_P urban_overlay_ptr;

  FUNC_INIT;
  urban_overlay_ptr = mem_GetWGridPtr (__FILE__, func, __LINE__);
  assert (urban_overlay_ptr != NULL);

  urban_seed_ptr = igrid_GetUrbanGridPtr (__FILE__, func, __LINE__, 0);
  assert (urban_seed_ptr != NULL);

  lower_bounds[0] = 1;
  upper_bounds[0] = 255;
  index[0] = SEED_COLOR_INDEX;
  index_count = 1;

  util_map_gridpts_2_index (urban_seed_ptr,
                            urban_overlay_ptr,
                            lower_bounds,
                            upper_bounds,
                            index,
                            index_count);

  util_overlay (z_prob_ptr,
                urban_overlay_ptr,
                z_prob_ptr);

  urban_seed_ptr = igrid_GridRelease (
                               __FILE__, func, __LINE__, urban_seed_ptr);
  urban_overlay_ptr = mem_GetWGridFree (
                            __FILE__, func, __LINE__, urban_overlay_ptr);

  FUNC_END;
}
