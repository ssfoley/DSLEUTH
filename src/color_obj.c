/******************************************************************************
*******************************************************************************
**                           MODULE PROLOG                                   **
*******************************************************************************
This object encapsulates the colortable structures.


*******************************************************************************
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "scenario_obj.h"
#include "color_obj.h"
#include "landclass_obj.h"
#include "memory_obj.h"
#include "ugm_defines.h"
#include "ugm_macros.h"
#include "gdif_obj.h"


char color_obj_c_sccs_id[] = "@(#)color_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static struct colortable *color_table_landuse_ptr;
static struct colortable *color_table_probability_ptr;
static struct colortable *color_table_growth_ptr;
static struct colortable *color_table_deltatron_ptr;
static struct colortable *color_table_grayscale_ptr;
static int initialized = FALSE;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void color_fill ();

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: color_GetColortable
** PURPOSE:       returns pointer to a selected colortable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

struct colortable *
  color_GetColortable (int i)
{
  char func[] = "color_GetColortable";
  struct colortable *return_ptr;

  if (i == LANDUSE_COLORTABLE)
  {
    return_ptr = color_table_landuse_ptr;
  }
  else if (i == PROBABILITY_COLORTABLE)
  {
    return_ptr = color_table_probability_ptr;
  }
  else if (i == GROWTH_COLORTABLE)
  {
    return_ptr = color_table_growth_ptr;
  }
  else if (i == DELTATRON_COLORTABLE)
  {
    return_ptr = color_table_deltatron_ptr;
  }
  else if (i == GRAYSCALE_COLORTABLE)
  {
    return_ptr = color_table_grayscale_ptr;
  }
  else
  {
    sprintf (msg_buf, "Unknown colortable");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  return return_ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: color_Init
** PURPOSE:       main driver for initializing the colortables
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  color_Init ()
{
  char filename[MAX_FILENAME_LEN];

  /*
   *
   * ALLOCATE SPACE FOR EACH COLORTABLE
   *
   */
  color_table_landuse_ptr = (struct colortable *)
    malloc (sizeof (struct colortable));
  color_table_probability_ptr = (struct colortable *)
    malloc (sizeof (struct colortable));
  color_table_growth_ptr = (struct colortable *)
    malloc (sizeof (struct colortable));
  color_table_deltatron_ptr = (struct colortable *)
    malloc (sizeof (struct colortable));
  color_table_grayscale_ptr = (struct colortable *)
    malloc (sizeof (struct colortable));

  /*
   *
   * FILL IN COLOR VALUES
   *
   */
  color_fill ();

  /*
   *
   * WRITE OUT A COLOR KEY FOR EACH COLORTABLE
   *
   */
  if (scen_GetWriteColorKeyFlag ())
  {
    sprintf (filename, "%skey_%s.gif",
             scen_GetOutputDir (), color_table_landuse_ptr->name);
    gdif_WriteColorKey (
                         color_table_landuse_ptr,
                         filename);
    sprintf (filename, "%skey_%s.gif",
             scen_GetOutputDir (), color_table_probability_ptr->name);
    gdif_WriteColorKey (
                         color_table_probability_ptr,
                         filename);
    sprintf (filename, "%skey_%s.gif",
             scen_GetOutputDir (), color_table_growth_ptr->name);
    gdif_WriteColorKey (
                         color_table_growth_ptr,
                         filename);
    sprintf (filename, "%skey_%s.gif",
             scen_GetOutputDir (), color_table_deltatron_ptr->name);
    gdif_WriteColorKey (
                         color_table_deltatron_ptr,
                         filename);
  }
  initialized = TRUE;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: color_fill
** PURPOSE:       initializes RGB values for each colortable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  color_fill ()
{
  int i;
  int index;
  int color_val;

  strcpy (color_table_grayscale_ptr->name, "GRAYSCALE_COLORMAP");
  color_table_grayscale_ptr->size = MAX_COLORS;
  for (i = 0; i < MAX_COLORS; i++)
  {
    color_table_grayscale_ptr->color[i].red = i;
    color_table_grayscale_ptr->color[i].green = i;
    color_table_grayscale_ptr->color[i].blue = i;
  }
  strcpy (color_table_landuse_ptr->name, "LANDUSE_COLORMAP");
  color_table_landuse_ptr->size = MAX_COLORS;
  for (i = 0; i < MAX_COLORS; i++)
  {
    color_table_landuse_ptr->color[i].red = i;
    color_table_landuse_ptr->color[i].green = i;
    color_table_landuse_ptr->color[i].blue = i;
  }
  for (i = 0; i < landclass_GetNumLandclasses (); i++)
  {
    color_val = landclass_GetClassColor (i);
    color_table_landuse_ptr->color[i].red = (color_val & RED_MASK) >> 16;
    color_table_landuse_ptr->color[i].green = (color_val & GREEN_MASK) >> 8;
    color_table_landuse_ptr->color[i].blue = color_val & BLUE_MASK;
  }

  strcpy (color_table_probability_ptr->name, "PROBABILITY_COLORMAP");
  color_table_probability_ptr->size = MAX_COLORS;
  for (i = 0; i < MAX_COLORS; i++)
  {
    color_table_probability_ptr->color[i].red = i;
    color_table_probability_ptr->color[i].green = i;
    color_table_probability_ptr->color[i].blue = i;
  }

  strcpy (color_table_deltatron_ptr->name, "DELTATRON_COLORMAP");
  color_table_deltatron_ptr->size = MAX_COLORS;
  for (i = 0; i < scen_GetDeltatronColorCount (); i++)
  {
    color_val = scen_GetDeltatronColor (i);
    color_table_deltatron_ptr->color[i].red = (color_val & RED_MASK) >> 16;
    color_table_deltatron_ptr->color[i].green =
      (color_val & GREEN_MASK) >> 8;
    color_table_deltatron_ptr->color[i].blue = color_val & BLUE_MASK;
  }
  for (i = scen_GetDeltatronColorCount (); i < MAX_COLORS; i++)
  {
    color_table_deltatron_ptr->color[i].red = i;
    color_table_deltatron_ptr->color[i].green = i;
    color_table_deltatron_ptr->color[i].blue = i;
  }
  /*
   *
   * USER OVER RIDES
   *
   */
  index = WATER_COLOR_INDEX;
  color_val = scen_GetWaterColor ();
  color_table_probability_ptr->color[index].red =
    (color_val & RED_MASK) >> 16;
  color_table_probability_ptr->color[index].green =
    (color_val & GREEN_MASK) >> 8;
  color_table_probability_ptr->color[index].blue = color_val & BLUE_MASK;

color_table_landuse_ptr->color[index].red = (color_val & RED_MASK) >> 16;
color_table_landuse_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
color_table_landuse_ptr->color[index].blue = color_val & BLUE_MASK;
  
index = SEED_COLOR_INDEX;
  color_val = scen_GetSeedColor ();
  color_table_probability_ptr->color[index].red =
    (color_val & RED_MASK) >> 16;
  color_table_probability_ptr->color[index].green =
    (color_val & GREEN_MASK) >> 8;
  color_table_probability_ptr->color[index].blue = color_val & BLUE_MASK;

  index = DATE_COLOR_INDEX;
  color_val = scen_GetDateColor ();
  color_table_probability_ptr->color[index].red =
    (color_val & RED_MASK) >> 16;
  color_table_probability_ptr->color[index].green =
    (color_val & GREEN_MASK) >> 8;
  color_table_probability_ptr->color[index].blue = color_val & BLUE_MASK;

  for (i = 0; i < scen_GetProbabilityColorCount (); i++)
  {
    index = i + START_INDEX_FOR_PROBABILITY_COLORS;
    color_val = scen_GetProbabilityColor (i);
    color_table_probability_ptr->color[index].red =
      (color_val & RED_MASK) >> 16;
    color_table_probability_ptr->color[index].green =
      (color_val & GREEN_MASK) >> 8;
    color_table_probability_ptr->color[index].blue = color_val & BLUE_MASK;
  }

  strcpy (color_table_growth_ptr->name, "GROWTH_COLORMAP");
  color_table_growth_ptr->size = MAX_COLORS;
  for (i = 0; i < MAX_COLORS; i++)
  {
    color_table_growth_ptr->color[i].red = i;
    color_table_growth_ptr->color[i].green = i;
    color_table_growth_ptr->color[i].blue = i;
  }
  index = PHASE0G;
  color_val = scen_GetPhase0GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;
  index = PHASE1G;
  color_val = scen_GetPhase1GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;
  index = PHASE2G;
  color_val = scen_GetPhase2GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;
  index = PHASE3G;
  color_val = scen_GetPhase3GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;
  index = PHASE4G;
  color_val = scen_GetPhase4GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;
  index = PHASE5G;
  color_val = scen_GetPhase5GrowthColor ();
  color_table_growth_ptr->color[index].red = (color_val & RED_MASK) >> 16;
  color_table_growth_ptr->color[index].green = (color_val & GREEN_MASK) >> 8;
  color_table_growth_ptr->color[index].blue = color_val & BLUE_MASK;

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: color_LogIt
** PURPOSE:       logs the colortable values on file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  color_LogIt (FILE * fp)
{
  int i;
  int color_val;
  char zeroes[] = "000000";
  char color_str[6];
  char hex_str[6];

  fprintf (fp,
      "\n*********************LOGGING COLORTABLES*****************\n\n");

  fprintf (fp, "COLORMAP: %s\n", color_table_landuse_ptr->name);
  for (i = 0; i < color_table_landuse_ptr->size; i++)
  {
    color_val = color_table_landuse_ptr->color[i].red * 256 * 256 +
      color_table_landuse_ptr->color[i].green * 256 +
      color_table_landuse_ptr->color[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %3u = %s\n", i,
             color_table_landuse_ptr->color[i].red,
             color_table_landuse_ptr->color[i].green,
             color_table_landuse_ptr->color[i].blue,
             hex_str);
  }

  fprintf (fp, "COLORMAP: %s\n", color_table_probability_ptr->name);
  for (i = 0; i < color_table_probability_ptr->size; i++)
  {
    color_val = color_table_probability_ptr->color[i].red * 256 * 256 +
      color_table_probability_ptr->color[i].green * 256 +
      color_table_probability_ptr->color[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %3u = %s\n", i,
             color_table_probability_ptr->color[i].red,
             color_table_probability_ptr->color[i].green,
             color_table_probability_ptr->color[i].blue,
             hex_str);
  }

  fprintf (fp, "COLORMAP: %s\n", color_table_growth_ptr->name);
  for (i = 0; i < color_table_growth_ptr->size; i++)
  {
    color_val = color_table_growth_ptr->color[i].red * 256 * 256 +
      color_table_growth_ptr->color[i].green * 256 +
      color_table_growth_ptr->color[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %3u = %s\n", i,
             color_table_growth_ptr->color[i].red,
             color_table_growth_ptr->color[i].green,
             color_table_growth_ptr->color[i].blue,
             hex_str);
  }

  fprintf (fp, "COLORMAP: %s\n", color_table_grayscale_ptr->name);
  for (i = 0; i < color_table_grayscale_ptr->size; i++)
  {
    color_val = color_table_grayscale_ptr->color[i].red * 256 * 256 +
      color_table_grayscale_ptr->color[i].green * 256 +
      color_table_grayscale_ptr->color[i].blue;
    sprintf (color_str, "%X", color_val);
    strcpy (hex_str, zeroes);
    strcpy (hex_str + 6 - strlen (color_str), color_str);
    fprintf (fp, "%3u %3u %3u %3u = %s\n", i,
             color_table_grayscale_ptr->color[i].red,
             color_table_grayscale_ptr->color[i].green,
             color_table_grayscale_ptr->color[i].blue,
             hex_str);
  }
  fflush (fp);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: color_MemoryLog
** PURPOSE:       log the memory locations on file descriptor fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  color_MemoryLog (FILE * fp)
{
  char func[] = "color_MemoryLog";
  if (!initialized)
  {
    sprintf (msg_buf, "color_Init() has not been called yet");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  LOG_MEM (fp, color_table_landuse_ptr, sizeof (struct colortable), 1);
  LOG_MEM (fp, color_table_probability_ptr, sizeof (struct colortable), 1);
  LOG_MEM (fp, color_table_growth_ptr, sizeof (struct colortable), 1);
  LOG_MEM (fp, color_table_deltatron_ptr, sizeof (struct colortable), 1);
  LOG_MEM (fp, color_table_grayscale_ptr, sizeof (struct colortable), 1);
}
