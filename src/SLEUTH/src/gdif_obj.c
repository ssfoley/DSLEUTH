/******************************************************************************
*******************************************************************************
**                           MODULE PROLOG                                   **
*******************************************************************************
This module encapsulates the image I/O functionality. The UGM code only
interfaces with GD, the GIF image library, through the functions in this
module.
*******************************************************************************
******************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gd.h"
#include "gdfonts.h"
#include "gdfontg.h"
#include "globals.h"
#include "gdif_obj.h"
#include "memory_obj.h"
#include "igrid_obj.h"
#include "timer_obj.h"
#include "scenario_obj.h"
#include "ugm_macros.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char gdif_obj_c_sccs_id[] = "@(#)gdif_obj.c	1.84	12/4/00";



/******************************************************************************
*******************************************************************************
** FUNCTION NAME: gdif_WriteColorKey
** PURPOSE:       write colorkeys for a given colortable
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  gdif_WriteColorKey (
                       struct colortable *colortable,
                       char fname[]
  )
{
  char func[] = "gdif_WriteColorKey";
  gdImagePtr im_out;
  FILE *fp;
  int i;
  int j;
  int sx;
  int sy;
  assert (colortable != NULL);
  assert (fname != NULL);

  FUNC_INIT;

  /*
   *
   * LOG STUFF
   *
   */
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogWritesFlag ())
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (), "\n%s %s %d \nwriting GIF %s\n",
               __FILE__, func, __LINE__, fname);
      fprintf (scen_GetLogFP (), "colortable name=%s\n",
               colortable->name);
      fprintf (scen_GetLogFP (), "colortable pointer=%d rows=%u cols=%u \n",
               colortable, colortable->size, igrid_GetNumCols ());
      scen_CloseLog ();
    }
  }
  /*
   *
   * OPEN OUTPUT GIF FILE
   *
   */
  FILE_OPEN (fp, fname, "w");

  /*
   *
   * CALL GD TO CREATE A GIF
   *
   */
  sx = igrid_GetNumCols ();
  sy = colortable->size;
  im_out = gdImageCreate (sx, sy);
  /*
   *
   * SET UP GD'S COLORTABLE
   *
   */
  for (i = 0; i < colortable->size; i++)
  {
    gdImageColorAllocate (im_out,
                          colortable->color[i].red,
                          colortable->color[i].green,
                          colortable->color[i].blue);
  }
  /*
   *
   * WRITE GIF TO GD'S MEMORY
   *
   */
  for (i = 0; i < colortable->size; i++)
  {
    for (j = 0; j < igrid_GetNumCols (); j++)
    {
      im_out->pixels[j][i] = (unsigned char) i;
    }
  }
  /*
   *
   * OUTPUT THE GIF TO DISK
   *
   */
  gdImageGif (im_out, fp);
  /*
   *
   * CLOSE THE OUTPUT FILE
   *
   */
  fclose (fp);
  /*
   *
   * FREE GD'S MEMORY
   *
   */
  gdImageDestroy (im_out);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: gdif_WriteGIF
** PURPOSE:       interface to GD for writing an GIF image
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  gdif_WriteGIF (
                  GRID_P gif,
                  struct colortable *colortable,
                  char fname[],
                  char date[],
                  int date_color_index
  )
{
  char func[] = "qgdif_WriteGIF";
  gdImagePtr im_out;
  FILE *fp;
  int i;
  int j;
  timer_Start (GDIF_WRITEGIF);
  assert (gif != NULL);
  assert (colortable != NULL);
  assert (fname != NULL);
  FUNC_INIT;

  /*
   *
   * LOG STUFF
   *
   */
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogWritesFlag ())
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (), "\n%s %s %d \nwriting GIF %s\n",
               __FILE__, func, __LINE__, fname);
      fprintf (scen_GetLogFP (), "colortable name=%s date=%s\n",
               colortable->name, date);
      fprintf (scen_GetLogFP (), "colortable pointer=%d rows=%u cols=%u \n",
               colortable, igrid_GetNumRows (), igrid_GetNumCols ());
      fprintf (scen_GetLogFP (), "image pointer = %d \n",
               gif);
      fprintf (scen_GetLogFP (), "date color index = %u \n",
               date_color_index);
      fprintf (scen_GetLogFP (), "\n");
      scen_CloseLog ();
    }
  }
  /*
   *
   * OPEN OUTPUT GIF FILE
   *
   */
  FILE_OPEN (fp, fname, "w");

  /*
   *
   * CALL GD TO CREATE A GIF
   *
   */
  im_out = gdImageCreate (igrid_GetNumCols (), igrid_GetNumRows ());
  im_out->sx = igrid_GetNumCols ();
  im_out->sy = igrid_GetNumRows ();

  /*
   *
   * SET UP GD'S COLORTABLE
   *
   */
  for (i = 0; i < colortable->size; i++)
  {
    gdImageColorAllocate (im_out,
                          colortable->color[i].red,
                          colortable->color[i].green,
                          colortable->color[i].blue);
  }
  /*
   *
   * WRITE GRID TO GD'S MEMORY
   *
   */
  for (i = 0; i < igrid_GetNumRows (); i++)
  {
    for (j = 0; j < igrid_GetNumCols (); j++)
    {
      im_out->pixels[j][i] = (unsigned char) gif[OFFSET (i, j)];
    }
  }
  /*
   *
   * WRITE THE DATE IF REQUESTED
   *
   */
  if (strlen (date) > 0)
  {
    gdImageString (im_out, gdFontGiant, DATE_X, DATE_Y, date, date_color_index);
  }
  /*
   *
   * OUTPUT THE GIF TO DISK
   *
   */
  gdImageGif (im_out, fp);
  /*
   *
   * CLOSE THE OUTPUT FILE
   *
   */
  fclose (fp);

  /*
   *
   * FREE GD'S MEMORY
   *
   */
  gdImageDestroy (im_out);
  FUNC_END;
  timer_Stop (GDIF_WRITEGIF);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: gdif_ReadGIF
** PURPOSE:       interface to GD for reading an GIF image
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  gdif_ReadGIF (GRID_P gif_ptr, char *fname)
{
  char func[] = "gdif_ReadGIF";
  FILE *in;
  int i = 0;
  int j = 0;
  gdImagePtr im_in;
  unsigned short int index_val;
  int red;
  int green;
  int blue;
  int row;
  int col;

  timer_Start (GDIF_READGIF);
  FUNC_INIT;
  assert (gif_ptr != NULL);
  assert (igrid_GetNumRows () > 0);
  assert (igrid_GetNumCols () > 0);

  /*
   *
   * LOGGING STUFF
   *
   */
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogWritesFlag ())
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (), "\n%s %s %d \nreading GIF %s\n",
               __FILE__, func, __LINE__, fname);
      fprintf (scen_GetLogFP (), "rows=%u cols=%u storage pointer = %d\n",
               igrid_GetNumRows (), igrid_GetNumCols (), gif_ptr);
      fprintf (scen_GetLogFP (), "\n");
      scen_CloseLog ();
    }
  }

  /*
   *
   * OPEN THE GIF IMAGE FILE FOR READING
   *
   */
  FILE_OPEN (in, fname, "rb");

  /*
   *
   * HAVE GD GET THE IMAGE AND THEN CHECK THE SIZE OF THE IMAGE
   *
   */
  im_in = gdImageCreateFromGif (in);
  row = im_in->sy;
  col = im_in->sx;
  if ((row != igrid_GetNumRows ()) || (col != igrid_GetNumCols ()))
  {
    sprintf (msg_buf, "%4uX%4u image doesn't match expected size %4uX%4u\n",
             row, col, igrid_GetNumRows (), igrid_GetNumCols ());
    LOG_ERROR (msg_buf);
    EXIT (1);
  }

  /*
   *
   * CLOSE THE FILE
   *
   */
  fclose (in);

  /*
   *
   * FILL IN THE GRID WITH VALUES FROM GD'S MEMORY
   * CHECK THAT THE IMAGE IS A TRUE GRAYSCALE IMAGE
   *
   */
  for (j = 0; j < igrid_GetNumCols (); j++)
  {
    for (i = 0; i < igrid_GetNumRows (); i++)
    {
      index_val = (unsigned short int) gdImageGetPixel (im_in, j, i);
      red = gdImageRed (im_in, index_val);
      green = gdImageGreen (im_in, index_val);
      blue = gdImageBlue (im_in, index_val);
      if ((red == green) && (red == blue))
      {
        gif_ptr[OFFSET (i, j)] = red;
      }
      else
      {
        sprintf (msg_buf, "file:%s is not a true gray scale image\n", fname);
        LOG_ERROR (msg_buf);
        sprintf (msg_buf, "index=%u RGB= (%u,%u,%u)\n", index_val, red, green, blue);
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
    }
  }

  /*
   *
   * FREE GD'S MEMORY
   *
   */
  gdImageDestroy (im_in);
  FUNC_END;
  timer_Stop (GDIF_READGIF);
}
