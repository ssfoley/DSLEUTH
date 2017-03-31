#define OUTPUT_MODULE

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "coeff_obj.h"
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "output.h"
#include "gd.h"
#include "color_obj.h"
#include "ugm_macros.h"
#include "scenario_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char output_c_sccs_id[] = "@(#)output.c	1.629	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: out_write_restart_data
** PURPOSE:       write restart data to filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  out_write_restart_data (char *filename, int diffusion_coeff,
                          int breed_coeff,
                          int spread_coeff, int slope_resistance,
                          int road_gravity,
                          int count, int counter)
{
  char func[] = "out_write_restart_data";
  FILE *fp;

  FUNC_INIT;
  assert (strlen (filename) > 0);

  if (scen_GetLogFlag ())
  {
    if (scen_GetLogWritesFlag ())
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (),
               "%s %u writing restart data to file: %s\n",
               __FILE__, __LINE__, filename);
      scen_CloseLog ();
    }
  }

  FILE_OPEN (fp, filename, "w");

  fprintf (fp, "%d %d %d %d %d %ld %d", diffusion_coeff,
           breed_coeff, spread_coeff, slope_resistance,
           road_gravity, count, counter);

  fclose (fp);
  FUNC_END;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: out_dump
** PURPOSE:       write count chars to filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  out_dump (char *filename,
            void *ptr,
            int count)
{
  char func[] = "out_dump";
  FILE *fp;
  int actual;

  FUNC_INIT;
  assert (filename != NULL);
  assert (ptr != NULL);
  assert (count > 0);

  if (scen_GetLogFlag ())
  {
    if (scen_GetLogWritesFlag ())
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (), "%s %u writing to file: %s\n",
               __FILE__, __LINE__, filename);
      scen_CloseLog ();
    }
  }

  FILE_OPEN (fp, filename, "w");

  actual = fwrite (ptr, sizeof (char), count, fp);
  if (actual != count)
  {
    sprintf (msg_buf, "Write failed. %u bytes of %u written to file %s",
             actual, count, filename);
    LOG_ERROR (msg_buf);
  }
  fclose (fp);
  FUNC_END;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: out_banner
** PURPOSE:       write banner to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  out_banner (FILE * fp)
{
  char func[] = "out_banner";
  char asterisks[256];
  char blank_line[256];

  FUNC_INIT;

  sprintf (blank_line, "%s", "**                                ");
  strcat (blank_line, "                                       **\n");
  sprintf (asterisks, "%s", "*******************************");
  strcat (asterisks, "********************************************");

  fprintf (fp, "\n\n");
  fprintf (fp, "%s\n", asterisks);
  fprintf (fp, "%s\n", asterisks);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "SLEUTH",
                   msg_buf, -1);
  out_center_text (fp, blank_line,
                   "(URBAN GROWTH MODEL)",
                   msg_buf, -1);
  out_center_text (fp, blank_line,
                   "Beta Version 3.0",
                   msg_buf, -1);
  out_center_text (fp, blank_line,
                   "Release Date: December4, 2000",
                   msg_buf, -1);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
     "Notice:  This is a beta version.  It has been formally released  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
         "by the U.S. Environmental Protection Agency (EPA) and should ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
    "not be construed to represent Agency policy. This model is being  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
    "circulated for comments on its technical merit and potential for  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "policy implications.  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
         "The U.S. Environmental Protection Agency through its Office  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
  "of Research and Development Interagency Agreement #DW14938148-01-2  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
       "with the United States Geological Survey partially funded and  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
        "collaborated in the model described here. Implementation and  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
   "redesign of the model code was conducted under contract #68W70055  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
      "to Lockheed Martin Technical Services.  The model has not been  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
             "subjected to Agency review .  Mention of trade names or  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
           "commercial products does not constitute an endorsement or  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "recommendation for use.   ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Contributors:  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Ronald W. Matheny ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "U.S. Environmental Protection Agency ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Landscape Characterization Branch MD-56 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Research Triangle Park, NC 27771 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
          "(Project Officer, coordination, parallelization, technical  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "assistance, review and testing) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "William Acevedo ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "United States Geological Survey ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Ames Research Center (242-4) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Moffettfield, CA 94035 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "(Project officer and coordination) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "                                              ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Keith Clarke                            ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Department of Geography ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "University of California, Santa Barbara ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Santa Barbara, CA 93117   ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "(Originator, theoretical constructs, testing)  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "                                ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Jeannette Candau ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "United States Geological Survey & ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Department of Geography ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "University of California, Santa Barbara ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Santa Barbara, CA 93117   ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
           "(Theory, model development, redesign, review and testing)  ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "David Hester ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "United States Geological Survey          ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Rocky Mountain Mapping Center ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "P.O. Box 25046, MS-516 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Denver, CO 80225-0046 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "(Review and testing) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Mark Feller ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "United States Geological Survey          ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Rocky Mountain Mapping Center ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "P.O. Box 25046, MS-516 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Denver, CO 80225-0046 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "(Review and testing) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "George Xian      ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Raytheon Corporation (contract with USGS #8701) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "EROS Data Center ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Sioux Falls, SD 57198                            ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "(Review and testing) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "                           ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Tommy E. Cathey ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Lockheed Martin Technical Services ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "National Environmental Supercomputing Center ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "United States Environmental Protection Agency ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   "Research Triangle Park, NC 27711 ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
        "(Implementation, model redesign, parallelization, and coding) ",
                   msg_buf, 4);
  out_center_text (fp, blank_line,
                   " ",
                   msg_buf, 4);

  fprintf (fp, "%s\n", asterisks);
  fprintf (fp, "%s\n", asterisks);
  fprintf (fp, "\n\n");

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: out_center_text
** PURPOSE:       center text within a given string
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  out_center_text (FILE * fp,
                   char *source,
                   char *text,
                   char *destination,
                   int left_offset)
{
  char func[] = "out_center_text";
  int s_len;
  int t_len;
  int offset;
  int max_copy;
#define MARGIN_BUFFER 2

  assert (source != NULL);
  assert (text != NULL);
  assert (destination != NULL);

  FUNC_INIT;

  strcpy (destination, source);
  s_len = strlen (source);
  t_len = strlen (text);
  if (left_offset >= 0)
  {
    max_copy = MIN ((s_len - left_offset - MARGIN_BUFFER - 1), (t_len));
    strncpy (destination + left_offset, text, max_copy);
  }
  else
  {
    offset = (s_len - t_len) / 2;
    offset = MAX (offset, MARGIN_BUFFER);
    max_copy = MIN ((s_len - offset - MARGIN_BUFFER - 1), (t_len));
    strncpy (destination + offset, text, max_copy);
  }
  fprintf (fp, "%s", destination);
  fflush (fp);

  FUNC_END;
}
