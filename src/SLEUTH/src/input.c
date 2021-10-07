#define INPUT_MODULE

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include "coeff_obj.h"
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "ugm_typedefs.h"
#include "output.h"
#include "input.h"
#include "utilities.h"
#include "ugm_macros.h"
#include "scenario_obj.h"


/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define GIF_ID "GIF87a"
#define GIF_ROW_OFFSET 8
#define GIF_COL_OFFSET 6
#define GIF_RES_OFFSET 10
#define MAX_ANSWER_STR_LEN 20
#define MAX_DUMMY_STR_LEN 80
#define MAX_CMD_STR_LEN 120
#ifdef DEBUG
#define CHECK_EOF(arg0,arg1,arg2,arg3)                           \
  if((arg0) != 2)                                                \
  {                                                              \
    printf("%s Line %d EOF occurred while reading file: %s\n",   \
           (arg1),(arg2),(arg3));                                \
    EXIT(1);                                                     \
  }
#else
#define CHECK_EOF(arg0,arg1,arg2,arg3)
#endif

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char input_c_sccs_id[] = "@(#)input.c	1.629	12/4/00";

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: inp_slurp
** PURPOSE:       read count chars from filename into ptr
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  inp_slurp (char *filename,
             void *ptr,
             int count)
{
  char func[] = "inp_slurp";
  FILE *fp;
  int actual;

  FUNC_INIT;
  assert (filename != NULL);
  assert (ptr != NULL);
  assert (count > 0);

  FILE_OPEN (fp, (filename), "r");

  actual = fread (ptr, sizeof (char), count, fp);
  if (actual != count)
  {
    sprintf (msg_buf, "Read failed. %u bytes of %u read from file %s",
             actual, count, filename);
    LOG_ERROR (msg_buf);
  }
  fclose (fp);
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: inp_read_restart_file
** PURPOSE:       read the restart file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  inp_read_restart_file (int *diffusion,
                         int *breed,
                         int *spread,
                         int *slope_resistance,
                         int *road_gravity,
                         long *random_seed,
                         int *counter)
{
  char func[] = "inp_read_restart_file";
  char filename[MAX_FILENAME_LEN];
  FILE *FileToRead;
  int rc;

  assert (diffusion != NULL);
  assert (breed != NULL);
  assert (spread != NULL);
  assert (slope_resistance != NULL);
  assert (road_gravity != NULL);
  assert (random_seed != NULL);
  assert (counter != NULL);

  FUNC_INIT;
  sprintf (filename, "%s%s%u", scen_GetOutputDir (), RESTART_FILE, glb_mype);

  FILE_OPEN (FileToRead, filename, "r");

  /*
   * Read the restart file
   */
  printf ("Reading restart file: %s\n", filename);
  rc = fscanf (FileToRead, "%d %d %d %d %d %ld %d",
               diffusion,
               breed,
               spread,
               slope_resistance,
               road_gravity,
               random_seed,
               counter);
  if (rc != 7)
  {
    sprintf (msg_buf, "EOF occurred when reading file %s", filename);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  fclose (FileToRead);

  FUNC_END;
}
