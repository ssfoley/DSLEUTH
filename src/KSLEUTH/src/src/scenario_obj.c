/******************************************************************************
*******************************************************************************
**                           MODULE PROLOG                                   **
*******************************************************************************
The scenario_obj.c module or object encapsulates the UGM's interface to the
user generated scenario file. This object maintains the scenario_info scenario
structure which mirrors the input options present in the scenario file.
It reads the scenario file and then provides the rest of the UGM
code access to its data values through its member functions.

*******************************************************************************
******************************************************************************/

#define SCENARIO_OBJ_MODULE
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#ifdef MPI
#include "mpi.h"
#endif
#include "scenario_obj.h"
#include "utilities.h"
#include "globals.h"
#include "proc_obj.h"
#include "ugm_macros.h"
#include "wgrid_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define SCEN_LINE_BUF_LEN 256

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char scenario_obj_c_sccs_id[] = "@(#)scenario_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static scenario_info scenario;
static char log_filename[SCEN_MAX_FILENAME_LEN];

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void scen_open_log ();
static void scen_read_file (char *filename);
static int scen_process_user_color (char *string2process);



/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  scen_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &scenario, sizeof (scenario_info), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetScenarioFilename
** PURPOSE:       return scenario filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetScenarioFilename ()
{
  return scenario.filename;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogFP
** PURPOSE:       return log file pointer
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
FILE *
  scen_GetLogFP ()
{
  return scenario.log_fp;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_Append2Log
** PURPOSE:       open log for appending
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  scen_Append2Log ()
{
  char func[] = "scen_Append2Log";
  FUNC_INIT;

  if (scenario.log_fp == NULL)
  {
    FILE_OPEN (scenario.log_fp, log_filename, "a");
  }
  else
  {
    sprintf (msg_buf, "%s is already open", log_filename);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetOutputDir
** PURPOSE:       return output directory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetOutputDir ()
{
  return scenario.output_dir;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWhirlgifBinary
** PURPOSE:       return whirlgif_binary
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetWhirlgifBinary ()
{
  return scenario.whirlgif_binary;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetInputDir
** PURPOSE:       return input directory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetInputDir ()
{
  return scenario.input_dir;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetUrbanDataFileCount
** PURPOSE:       return # of urban input files
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetUrbanDataFileCount ()
{
  return scenario.urban_data_file_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetRoadDataFileCount
** PURPOSE:       return # road data files
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetRoadDataFileCount ()
{
  return scenario.road_data_file_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseDataFileCount
** PURPOSE:       return # landuse data files
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetLanduseDataFileCount ()
{
  return scenario.landuse_data_file_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetDoingLanduseFlag
** PURPOSE:       return doing landuse flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:   if doing landuse, then landuse_data_file_count is = 2
**
**
*/
int
  scen_GetDoingLanduseFlag ()
{
  return scenario.landuse_data_file_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetUrbanDataFilename
** PURPOSE:       return urban data filename by index, i
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetUrbanDataFilename (int i)
{
  return scenario.urban_data_file[i];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetRoadDataFilename
** PURPOSE:       return road data filename by index, i
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetRoadDataFilename (int i)
{
  return scenario.road_data_file[i];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseDataFilename
** PURPOSE:       return landuse data filename by index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetLanduseDataFilename (int i)
{
  return scenario.landuse_data_file[i];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetExcludedDataFilename
** PURPOSE:       return excluded data filename by index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetExcludedDataFilename ()
{
  return scenario.excluded_data_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetSlopeDataFilename
** PURPOSE:       return slope data filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetSlopeDataFilename ()
{
  return scenario.slope_data_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetBackgroundDataFilename
** PURPOSE:       return background data filename
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
char *
  scen_GetBackgroundDataFilename ()
{
  return scenario.background_data_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetEchoImageFlag
** PURPOSE:       return echo_image_files flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetEchoImageFlag ()
{
  return scenario.echo_image_files;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWriteColorKeyFlag
** PURPOSE:       return write_color_keys flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetWriteColorKeyFlag ()
{
  return scenario.write_color_keys;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPostprocessingFlag
** PURPOSE:       return Postprocessing flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetPostprocessingFlag ()
{
  return scenario.postprocessing;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogFlag
** PURPOSE:       return Log flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogFlag ()
{
  return scenario.logging;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetEchoFlag
** PURPOSE:       return echo flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetEchoFlag ()
{
  return scenario.echo;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetRandomSeed
** PURPOSE:       return random seed value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetRandomSeed ()
{
  return scenario.random_seed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetMonteCarloIterations
** PURPOSE:       return # monte carlo iterations
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetMonteCarloIterations ()
{
  return scenario.monte_carlo_iterations;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffDiffusionStart
** PURPOSE:       return diffusion start value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffDiffusionStart ()
{
  return scenario.start.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffBreedStart
** PURPOSE:       return breed start value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffBreedStart ()
{
  return scenario.start.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSpreadStart
** PURPOSE:       return spread start value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSpreadStart ()
{
  return scenario.start.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSlopeResistStart
** PURPOSE:       return slope resistance start value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSlopeResistStart ()
{
  return scenario.start.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffRoadGravityStart
** PURPOSE:       return road gravity start value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffRoadGravityStart ()
{
  return scenario.start.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffDiffusionStop
** PURPOSE:       return diffusion stop value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffDiffusionStop ()
{
  return scenario.stop.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffBreedStop
** PURPOSE:       return breed stop value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffBreedStop ()
{
  return scenario.stop.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSpreadStop
** PURPOSE:       return spread stop value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSpreadStop ()
{
  return scenario.stop.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSlopeResistStop
** PURPOSE:       return slope resistance stop value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSlopeResistStop ()
{
  return scenario.stop.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffRoadGravityStop
** PURPOSE:       return road gravity stop value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffRoadGravityStop ()
{
  return scenario.stop.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffDiffusionStep
** PURPOSE:       return diffustion step value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffDiffusionStep ()
{
  return scenario.step.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffBreedStep
** PURPOSE:       return breed step value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffBreedStep ()
{
  return scenario.step.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSpreadStep
** PURPOSE:       return spread step value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSpreadStep ()
{
  return scenario.step.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSlopeResistStep
** PURPOSE:       return slope resistance step value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSlopeResistStep ()
{
  return scenario.step.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffRoadGravityStep
** PURPOSE:       return road gravity step value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffRoadGravityStep ()
{
  return scenario.step.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffDiffusionBestFit
** PURPOSE:       return diffusion best fit value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffDiffusionBestFit ()
{
  return scenario.best_fit.diffusion;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffBreedBestFit
** PURPOSE:       return breed best fit value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffBreedBestFit ()
{
  return scenario.best_fit.breed;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSpreadBestFit
** PURPOSE:       return spread best fit value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSpreadBestFit ()
{
  return scenario.best_fit.spread;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffSlopeResistBestFit
** PURPOSE:       return slope resistance best fit value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffSlopeResistBestFit ()
{
  return scenario.best_fit.slope_resistance;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCoeffRoadGravityBestFit
** PURPOSE:       return road gravity best fit value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetCoeffRoadGravityBestFit ()
{
  return scenario.best_fit.road_gravity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPredictionStartDate
** PURPOSE:       return prediction start date
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPredictionStartDate ()
{
  return scenario.prediction_start_date;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPredictionStopDate
** PURPOSE:       return prediction stop date
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPredictionStopDate ()
{
  return scenario.prediction_stop_date;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetDateColor
** PURPOSE:       return date color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetDateColor ()
{
  return scenario.date_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetSeedColor
** PURPOSE:       return seed color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetSeedColor ()
{
  return scenario.seed_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWaterColor
** PURPOSE:       return water color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetWaterColor ()
{
  return scenario.water_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetProbabilityColorCount
** PURPOSE:       return # probability color count
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetProbabilityColorCount ()
{
  return scenario.probability_color_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetProbabilityColorLowerBound
** PURPOSE:       return lower bound for probability color index, i
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetProbabilityColorLowerBound (int i)
{
  return scenario.probability_color[i].lower_bound;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetProbabilityColorUpperBound
** PURPOSE:       return upper bound for probability color index, i
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetProbabilityColorUpperBound (int i)
{
  return scenario.probability_color[i].upper_bound;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetProbabilityColor
** PURPOSE:       return probability color by index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetProbabilityColor (int i)
{
  return scenario.probability_color[i].color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogMemoryMapFlag
** PURPOSE:       return log memory map flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogMemoryMapFlag ()
{
  return scenario.log_memory_map;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogSlopeWeightsFlag
** PURPOSE:       return log slope weights flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogSlopeWeightsFlag ()
{
  return scenario.log_slope_weights;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogLandclassSummaryFlag
** PURPOSE:       return log landclass summary flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogLandclassSummaryFlag ()
{
  return scenario.log_landclass_summary;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogReadsFlag
** PURPOSE:       return log reads flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogReadsFlag ()
{
  return scenario.log_reads;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogWritesFlag
** PURPOSE:       return log writes flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogWritesFlag ()
{
  return scenario.log_writes;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetNumLanduseClasses
** PURPOSE:       return num_landuse_classes
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
int
  scen_GetNumLanduseClasses ()
{
  return scenario.num_landuse_classes;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseClassType
** PURPOSE:       return ptr to landuse_class Type
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
char *
  scen_GetLanduseClassType (int i)
{
  return scenario.landuse_class[i].type;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseClassColor
** PURPOSE:       return landuse_class color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
int
  scen_GetLanduseClassColor (int i)
{
  return scenario.landuse_class[i].color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseClassGrayscale
** PURPOSE:       return landuse_class Grayscale
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
int
  scen_GetLanduseClassGrayscale (int i)
{
  return scenario.landuse_class[i].grayscale;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLanduseClassName
** PURPOSE:       return ptr to landuse_class name
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 12/1/2000
** DESCRIPTION:
**
**
*/
char *
  scen_GetLanduseClassName (int i)
{
  return scenario.landuse_class[i].name;
}


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogColortablesFlag
** PURPOSE:       return log colortables flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogColortablesFlag ()
{
  return scenario.log_colortables;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetRdGrvtySensitivity
** PURPOSE:       return road gravity sensitivity
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetRdGrvtySensitivity ()
{
  return scenario.rd_grav_sensitivity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetSlopeSensitivity
** PURPOSE:       return slope sensitivity
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetSlopeSensitivity ()
{
  return scenario.slope_sensitivity;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCriticalLow
** PURPOSE:       return critical low
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetCriticalLow ()
{
  return scenario.critical_low;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCriticalHigh
** PURPOSE:       return critical high
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetCriticalHigh ()
{
  return scenario.critical_high;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetCriticalSlope
** PURPOSE:       return log processing status flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetCriticalSlope ()
{
  return scenario.critical_slope;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetBoom
** PURPOSE:       return boom value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetBoom ()
{
  return scenario.boom;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetBust
** PURPOSE:       return bust value
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  scen_GetBust ()
{
  return scenario.bust;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWriteCoeffFileFlag
** PURPOSE:       return coeff log flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetWriteCoeffFileFlag ()
{
  return scenario.write_coeff_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWriteAvgFileFlag
** PURPOSE:       return log processing status flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetWriteAvgFileFlag ()
{
  return scenario.write_avg_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogBaseStatsFlag
** PURPOSE:       return log base statistics flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogBaseStatsFlag ()
{
  return scenario.log_base_stats;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogDebugFlag
** PURPOSE:       return log base statistics flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogDebugFlag ()
{
  return scenario.log_debug;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogUrbanizationAttemptsFlag
** PURPOSE:       return log urbanization attempts flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogUrbanizationAttemptsFlag ()
{
  return scenario.log_urbanization_attempts;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogCoeffFlag
** PURPOSE:       return log coeff flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogCoeffFlag ()
{
  return scenario.log_coeff;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogTimingsFlag
** PURPOSE:       return log timings flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetLogTimingsFlag ()
{
  return scenario.log_timings;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetWriteStdDevFileFlag
** PURPOSE:       return log processing status flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetWriteStdDevFileFlag ()
{
  return scenario.write_std_dev_file;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogTransitionMatrixFlag
** PURPOSE:       return log processing status flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogTransitionMatrixFlag ()
{
  return scenario.log_trans_matrix;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetLogProcessingStatusFlag
** PURPOSE:       return log processing status flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetLogProcessingStatusFlag ()
{
  return scenario.log_processing_status;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetViewGrowthTypesFlag
** PURPOSE:       return view growth types flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetViewGrowthTypesFlag ()
{
  if (scenario.view_growth_types)
  {
    if ((proc_GetCurrentRun () >= scenario.growth_type_window.run1) &&
        (proc_GetCurrentRun () <= scenario.growth_type_window.run2) &&
        (proc_GetCurrentMonteCarlo () >=
         scenario.growth_type_window.monte_carlo1) &&
        (proc_GetCurrentMonteCarlo () <=
         scenario.growth_type_window.monte_carlo2) &&
        (proc_GetCurrentYear () >= scenario.growth_type_window.year1) &&
        (proc_GetCurrentYear () <= scenario.growth_type_window.year2))
    {
      return scenario.view_growth_types;
    }
  }
  return FALSE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetViewDeltatronAgingFlag
** PURPOSE:       return view deltatron aging flag
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
BOOLEAN
  scen_GetViewDeltatronAgingFlag ()
{
  if (scenario.view_deltatron_aging)
  {
    if ((proc_GetCurrentRun () >= scenario.deltatron_aging_window.run1) &&
        (proc_GetCurrentRun () <= scenario.deltatron_aging_window.run2) &&
        (proc_GetCurrentMonteCarlo () >=
         scenario.deltatron_aging_window.monte_carlo1) &&
        (proc_GetCurrentMonteCarlo () <=
         scenario.deltatron_aging_window.monte_carlo2) &&
     (proc_GetCurrentYear () >= scenario.deltatron_aging_window.year1) &&
        (proc_GetCurrentYear () <= scenario.deltatron_aging_window.year2))
    {
      return scenario.view_deltatron_aging;
    }
  }
  return FALSE;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetDeltatronColorCount
** PURPOSE:       return deltatron color count
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetDeltatronColorCount ()
{
  return scenario.deltatron_color_count;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetDeltatronColor
** PURPOSE:       return deltatron color by index
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetDeltatronColor (int index)
{
  assert (index < scenario.deltatron_color_count);

  return scenario.deltatron_color[index];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase0GrowthColor
** PURPOSE:       return scenario.phase0g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase0GrowthColor ()
{
  return scenario.phase0g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase1GrowthColor
** PURPOSE:       scenario.phase1g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase1GrowthColor ()
{
  return scenario.phase1g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase2GrowthColor
** PURPOSE:       return scenario.phase2g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase2GrowthColor ()
{
  return scenario.phase2g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase3GrowthColor
** PURPOSE:       return scenario.phase3g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase3GrowthColor ()
{
  return scenario.phase3g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase4GrowthColor
** PURPOSE:       return scenario.phase4g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase4GrowthColor ()
{
  return scenario.phase4g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_GetPhase5GrowthColor
** PURPOSE:       return scenario.phase5g_growth_color
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  scen_GetPhase5GrowthColor ()
{
  return scenario.phase5g_growth_color;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_init
** PURPOSE:       initialize scenario object
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  scen_init (char *filename)
{
  char func[] = "scen_init";

  FUNC_INIT;
  scenario.deltatron_color_count = 0;
  if (glb_mype == 0)
  {
    scen_read_file (filename);
  }
#ifdef MPI
  MPI_Bcast (&scenario, sizeof (scenario), MPI_BYTE, 0, MPI_COMM_WORLD);
#endif
  wgrid_SetWGridCount (scenario.num_working_grids);

  scen_open_log ();
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_read_file
** PURPOSE:       read the scenario file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  scen_read_file (char *filename)
{
  char func[] = "scen_init";
  FILE *fp;
  char line[SCEN_MAX_FILENAME_LEN];
  char orig_line[SCEN_MAX_FILENAME_LEN];
  char *keyword;
  char *object_ptr;
  int index;

  assert (filename != NULL);

  strcpy (scenario.filename, filename);

  FILE_OPEN (fp, scenario.filename, "r");

  scenario.num_landuse_classes = 0;
  scenario.urban_data_file_count = 0;
  scenario.road_data_file_count = 0;
  scenario.landuse_data_file_count = 0;
  scenario.probability_color_count = 0;
  strcpy (scenario.whirlgif_binary, "");

  while (fgets (line, SCEN_MAX_FILENAME_LEN, fp) != NULL)
  {
    strncpy (orig_line, line, strlen (line));
    /*
     *
     * IGNORE LINES BEGINNING WITH #
     *
     */
    if (strncmp (line, "#", 1))
    {
      /*
       *
       * IGNORE TEXT TO RIGHT OF # AND BLANK LINES
       *
       */
      strtok (line, "#");
      util_trim (line);
      if (strlen (line) > 0)
      {
        keyword = strtok (line, "=");
        if (!strcmp (keyword, "INPUT_DIR"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.input_dir, object_ptr);
        }
        else if (!strcmp (keyword, "OUTPUT_DIR"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.output_dir, object_ptr);
        }
        else if (!strcmp (keyword, "WHIRLGIF_BINARY"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.whirlgif_binary, object_ptr);
        }
        else if (!strcmp (keyword, "URBAN_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          index = scenario.urban_data_file_count;
          strcpy (scenario.urban_data_file[index], object_ptr);
          scenario.urban_data_file_count++;
        }
        else if (!strcmp (keyword, "ROAD_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          index = scenario.road_data_file_count;
          strcpy (scenario.road_data_file[index], object_ptr);
          scenario.road_data_file_count++;
        }
        else if (!strcmp (keyword, "URBAN_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          index = scenario.urban_data_file_count;
          strcpy (scenario.urban_data_file[index], object_ptr);
          scenario.urban_data_file_count++;
        }
        else if (!strcmp (keyword, "LANDUSE_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          index = scenario.landuse_data_file_count;
          strcpy (scenario.landuse_data_file[index], object_ptr);
          scenario.landuse_data_file_count++;
        }
        else if (!strcmp (keyword, "EXCLUDED_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.excluded_data_file, object_ptr);
        }
        else if (!strcmp (keyword, "SLOPE_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.slope_data_file, object_ptr);
        }
        else if (!strcmp (keyword, "BACKGROUND_DATA"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          strcpy (scenario.background_data_file, object_ptr);
        }
        else if (!strcmp (keyword, "ROAD_GRAV_SENSITIVITY"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.rd_grav_sensitivity = atof (object_ptr);
        }
        else if (!strcmp (keyword, "SLOPE_SENSITIVITY"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.slope_sensitivity = atof (object_ptr);
        }
        else if (!strcmp (keyword, "CRITICAL_LOW"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.critical_low = atof (object_ptr);
        }
        else if (!strcmp (keyword, "CRITICAL_HIGH"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.critical_high = atof (object_ptr);
        }
        else if (!strcmp (keyword, "CRITICAL_SLOPE"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.critical_slope = atof (object_ptr);
        }
        else if (!strcmp (keyword, "BOOM"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.boom = atof (object_ptr);
        }
        else if (!strcmp (keyword, "BUST"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.bust = atof (object_ptr);
        }
        else if (!strcmp (keyword, "ECHO_IMAGE_FILES(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.echo_image_files = 1;
          if (!strcmp (object_ptr, "NO"))
          {
            scenario.echo_image_files = 0;
          }
        }
        else if (!strcmp (keyword, "WRITE_COLOR_KEY_IMAGES(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.write_color_keys = 1;
          if (!strcmp (object_ptr, "NO"))
          {
            scenario.write_color_keys = 0;
          }
        }
        else if (!strcmp (keyword, "WRITE_COEFF_FILE(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.write_coeff_file = 1;
          if (!strcmp (object_ptr, "NO"))
          {
            scenario.write_coeff_file = 0;
          }
        }
        else if (!strcmp (keyword, "WRITE_AVG_FILE(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.write_avg_file = 1;
          if (!strcmp (object_ptr, "NO"))
          {
            scenario.write_avg_file = 0;
          }
        }
        else if (!strcmp (keyword, "WRITE_STD_DEV_FILE(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.write_std_dev_file = 1;
          if (!strcmp (object_ptr, "NO"))
          {
            scenario.write_std_dev_file = 0;
          }
        }
        else if (!strcmp (keyword, "LOG_TRANSITION_MATRIX(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_trans_matrix = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_trans_matrix = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_URBANIZATION_ATTEMPTS(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_urbanization_attempts = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_urbanization_attempts = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_BASE_STATISTICS(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_base_stats = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_base_stats = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_DEBUG(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_debug = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_debug = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_INITIAL_COEFFICIENTS(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_coeff = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_coeff = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_TIMINGS(0:off/1:low verbosity/2:high verbosity)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.log_timings = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "RANDOM_SEED"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.random_seed = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "NUM_WORKING_GRIDS"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.num_working_grids = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "MONTE_CARLO_ITERATIONS"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.monte_carlo_iterations = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "ANIMATION(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.postprocessing = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.postprocessing = 1;
          }
        }
        else if (!strcmp (keyword, "ECHO(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.echo = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.echo = 1;
          }
        }
        else if (!strcmp (keyword, "LOGGING(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.logging = FALSE;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.logging = TRUE;
          }
        }
        else if (!strcmp (keyword, "WRITE_MEMORY_MAP(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_memory_map = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_memory_map = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_LANDCLASS_SUMMARY(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_landclass_summary = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_landclass_summary = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_SLOPE_WEIGHTS(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_slope_weights = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_slope_weights = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_READS(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_reads = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_reads = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_WRITES(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_writes = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_writes = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_COLORTABLES(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.log_colortables = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.log_colortables = 1;
          }
        }
        else if (!strcmp (keyword, "LOG_PROCESSING_STATUS(0:off/1:low verbosity/2:high verbosity)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.log_processing_status = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "VIEW_GROWTH_TYPES(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.view_growth_types = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.view_growth_types = 1;
          }
        }
        else if (!strcmp (keyword, "GROWTH_TYPE_PRINT_WINDOW"))
        {
          object_ptr = strtok (NULL, "\n");
          util_trim (object_ptr);
          scenario.growth_type_window.run1 = atoi (strtok (object_ptr, ","));
          scenario.growth_type_window.run2 = atoi (strtok (NULL, ","));
          scenario.growth_type_window.monte_carlo1 = atoi (strtok (NULL, ","));
          scenario.growth_type_window.monte_carlo2 = atoi (strtok (NULL, ","));
          scenario.growth_type_window.year1 = atoi (strtok (NULL, ","));
          scenario.growth_type_window.year2 = atoi (strtok (NULL, ","));
        }
        else if (!strcmp (keyword, "CALIBRATION_DIFFUSION_START"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.start.diffusion = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_DIFFUSION_STOP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.stop.diffusion = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_DIFFUSION_STEP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.step.diffusion = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_DIFFUSION_BEST_FIT"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.best_fit.diffusion = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_BREED_START"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.start.breed = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_BREED_STOP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.stop.breed = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_BREED_STEP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.step.breed = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_BREED_BEST_FIT"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.best_fit.breed = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SPREAD_START"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.start.spread = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SPREAD_STOP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.stop.spread = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SPREAD_STEP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.step.spread = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_SPREAD_BEST_FIT"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.best_fit.spread = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SLOPE_START"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.start.slope_resistance = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SLOPE_STOP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.stop.slope_resistance = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_SLOPE_STEP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.step.slope_resistance = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_SLOPE_BEST_FIT"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.best_fit.slope_resistance = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_ROAD_START"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.start.road_gravity = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_ROAD_STOP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.stop.road_gravity = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "CALIBRATION_ROAD_STEP"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.step.road_gravity = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_ROAD_BEST_FIT"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.best_fit.road_gravity = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_START_DATE"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.prediction_start_date = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "PREDICTION_STOP_DATE"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          scenario.prediction_stop_date = atoi (object_ptr);
        }
        else if (!strcmp (keyword, "DATE_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.date_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "SEED_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.seed_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "WATER_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.water_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PROBABILITY_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          index = scenario.probability_color_count;
          scenario.probability_color[index].lower_bound =
            atoi (strtok (object_ptr, ","));
          scenario.probability_color[index].upper_bound =
            atoi (strtok (NULL, ","));
          scenario.probability_color[index].color =
            scen_process_user_color (strtok (NULL, ","));


          scenario.probability_color_count++;
        }
        else if (!strcmp (keyword, "PHASE0G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase0g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PHASE1G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase1g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PHASE2G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase2g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PHASE3G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase3g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PHASE4G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase4g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "PHASE5G_GROWTH_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.phase5g_growth_color = scen_process_user_color (object_ptr);
        }
        else if (!strcmp (keyword, "LANDUSE_CLASS"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.landuse_class[scenario.num_landuse_classes].grayscale =
            atoi (strtok (object_ptr, ","));
          object_ptr = strtok (NULL, ",");
          util_trim (object_ptr);
          strcpy (scenario.landuse_class[scenario.num_landuse_classes].name,
                  object_ptr);
          object_ptr = strtok (NULL, ",");
          util_trim (object_ptr);
          strcpy (scenario.landuse_class[scenario.num_landuse_classes].type,
                  object_ptr);
          scenario.landuse_class[scenario.num_landuse_classes].color =
            scen_process_user_color (strtok (NULL, "\n"));

          scenario.num_landuse_classes++;
        }
        else if (!strcmp (keyword, "VIEW_DELTATRON_AGING(YES/NO)"))
        {
          object_ptr = strtok (NULL, " \n");
          util_trim (object_ptr);
          util_AllCAPS (object_ptr);
          scenario.view_deltatron_aging = 0;
          if (!strcmp (object_ptr, "YES"))
          {
            scenario.view_deltatron_aging = 1;
          }
        }
        else if (!strcmp (keyword, "DELTATRON_PRINT_WINDOW"))
        {
          object_ptr = strtok (NULL, "\n");
          util_trim (object_ptr);
          scenario.deltatron_aging_window.run1 =
            atoi (strtok (object_ptr, ","));
          scenario.deltatron_aging_window.run2 = atoi (strtok (NULL, ","));
          scenario.deltatron_aging_window.monte_carlo1 =
            atoi (strtok (NULL, ","));
          scenario.deltatron_aging_window.monte_carlo2 =
            atoi (strtok (NULL, ","));
          scenario.deltatron_aging_window.year1 = atoi (strtok (NULL, ","));
          scenario.deltatron_aging_window.year2 = atoi (strtok (NULL, ","));
        }
        else if (!strcmp (keyword, "DELTATRON_COLOR"))
        {
          object_ptr = strtok (NULL, "\0");
          util_trim (object_ptr);
          scenario.deltatron_color[scenario.deltatron_color_count++] =
            scen_process_user_color (object_ptr);
        }
      }
    }
  }
  fclose (fp);

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_process_user_color
** PURPOSE:       parse the user color input string
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static int
  scen_process_user_color (char *string2process)
{
  char func[] = "scen_process_user_color";
  int color_val;

  FUNC_INIT;
  assert (string2process != NULL);

  util_trim (string2process);
  if (strlen (string2process) == 0)
  {
    color_val = 0;
  }
  else
  {
    if ((!strncmp (string2process, "0x", 2)) ||
        (!strncmp (string2process, "0X", 2)))
    {
      sscanf (string2process, "%x", &(color_val));
    }
    else
    {
      color_val = atoi (strtok (string2process, ",")) * 256 * 256;
      color_val += atoi (strtok (NULL, ",")) * 256;
      color_val += atoi (strtok (NULL, "\0"));
    }
  }

  FUNC_END;
  return (color_val);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_echo
** PURPOSE:       echo scenario struct to FILE * fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  scen_echo (FILE * fp)
{
  char func[] = "scen_echo";
  int index;

  FUNC_INIT;
  assert (fp != NULL);

  fprintf (fp, "scenario.filename = %s\n", scenario.filename);
  fprintf (fp, "scenario.input_dir = %s\n", scenario.input_dir);
  fprintf (fp, "scenario.output_dir = %s\n", scenario.output_dir);
  fprintf (fp, "scenario.whirlgif_binary = %s\n", scenario.whirlgif_binary);
  for (index = 0; index < scenario.urban_data_file_count; index++)
  {
    fprintf (fp, "scenario.urban_data_file[%u] = %s\n",
             index, scenario.urban_data_file[index]);
  }
  for (index = 0; index < scenario.road_data_file_count; index++)
  {
    fprintf (fp, "scenario.road_data_file[%u] = %s\n",
             index, scenario.road_data_file[index]);
  }
  for (index = 0; index < scenario.landuse_data_file_count; index++)
  {
    fprintf (fp, "scenario.landuse_data_file[%u] = %s\n",
             index, scenario.landuse_data_file[index]);
  }
  fprintf (fp, "scenario.excluded_data_file = %s\n",
           scenario.excluded_data_file);
  fprintf (fp, "scenario.slope_data_file = %s\n", scenario.slope_data_file);
  fprintf (fp, "scenario.background_data_file = %s\n",
           scenario.background_data_file);
  fprintf (fp, "scenario.echo = %u\n", scenario.echo);
  fprintf (fp, "scenario.logging = %u\n", scenario.logging);
  fprintf (fp, "scenario.log_processing_status = %u\n",
           scenario.log_processing_status);
  fprintf (fp, "scenario.random_seed = %u\n", scenario.random_seed);
  fprintf (fp, "scenario.num_working_grids = %d\n", scenario.num_working_grids);
  fprintf (fp, "scenario.monte_carlo_iterations = %u\n",
           scenario.monte_carlo_iterations);
  fprintf (fp, "scenario.start.diffusion = %u\n", scenario.start.diffusion);
  fprintf (fp, "scenario.stop.diffusion = %u\n", scenario.stop.diffusion);
  fprintf (fp, "scenario.step.diffusion = %u\n", scenario.step.diffusion);
  fprintf (fp, "scenario.best_fit.diffusion = %u\n",
           scenario.best_fit.diffusion);
  fprintf (fp, "scenario.start.breed = %u\n", scenario.start.breed);
  fprintf (fp, "scenario.stop.breed = %u\n", scenario.stop.breed);
  fprintf (fp, "scenario.step.breed = %u\n", scenario.step.breed);
  fprintf (fp, "scenario.best_fit.breed = %u\n", scenario.best_fit.breed);
  fprintf (fp, "scenario.start.spread = %u\n", scenario.start.spread);
  fprintf (fp, "scenario.stop.spread = %u\n", scenario.stop.spread);
  fprintf (fp, "scenario.step.spread = %u\n", scenario.step.spread);
  fprintf (fp, "scenario.best_fit.spread = %u\n", scenario.best_fit.spread);
  fprintf (fp, "scenario.start.slope_resistance = %u\n",
           scenario.start.slope_resistance);
  fprintf (fp, "scenario.stop.slope_resistance = %u\n",
           scenario.stop.slope_resistance);
  fprintf (fp, "scenario.step.slope_resistance = %u\n",
           scenario.step.slope_resistance);
  fprintf (fp, "scenario.best_fit.slope_resistance = %u\n",
           scenario.best_fit.slope_resistance);
  fprintf (fp, "scenario.start.road_gravity = %u\n",
           scenario.start.road_gravity);
  fprintf (fp, "scenario.stop.road_gravity = %u\n",
           scenario.stop.road_gravity);
  fprintf (fp, "scenario.step.road_gravity = %u\n",
           scenario.step.road_gravity);
  fprintf (fp, "scenario.best_fit.road_gravity = %u\n",
           scenario.best_fit.road_gravity);
  fprintf (fp, "scenario.prediction_start_date = %u\n",
           scenario.prediction_start_date);
  fprintf (fp, "scenario.prediction_stop_date = %u\n",
           scenario.prediction_stop_date);
  fprintf (fp, "scenario.date_color = %x\n", scenario.date_color);
  fprintf (fp, "scenario.seed_color = %x\n", scenario.seed_color);
  fprintf (fp, "scenario.water_color = %x\n", scenario.water_color);
  fprintf (fp, "scenario.probability_color[%u].lower_bound = %u\n", index,
           scenario.probability_color[index].lower_bound);
  fprintf (fp, "scenario.probability_color[%u].upper_bound = %u\n", index,
           scenario.probability_color[index].upper_bound);
  fprintf (fp, "scenario.probability_color[%u].color = %X\n", index,
           scenario.probability_color[index].color);
  fprintf (fp, "scenario.rd_grav_sensitivity = %f\n",
           scenario.rd_grav_sensitivity);
  fprintf (fp, "scenario.slope_sensitivity = %f\n", scenario.slope_sensitivity);
  fprintf (fp, "scenario.critical_low = %f\n", scenario.critical_low);
  fprintf (fp, "scenario.critical_high = %f\n", scenario.critical_high);
  fprintf (fp, "scenario.critical_slope = %f\n", scenario.critical_slope);
  fprintf (fp, "scenario.boom = %f\n", scenario.boom);
  fprintf (fp, "scenario.bust = %f\n", scenario.bust);
  fprintf (fp, "scenario.log_base_stats = %u\n", scenario.log_base_stats);
  fprintf (fp, "scenario.log_debug = %u\n", scenario.log_debug);
  fprintf (fp, "scenario.log_urbanization_attempts = %u\n",
           scenario.log_urbanization_attempts);
  fprintf (fp, "scenario.log_coeff = %u\n", scenario.log_coeff);
  fprintf (fp, "scenario.log_timings = %u\n", scenario.log_timings);
  fprintf (fp, "scenario.write_avg_file = %u\n", scenario.write_avg_file);
  fprintf (fp, "scenario.write_std_dev_file = %u\n",
           scenario.write_std_dev_file);
  fprintf (fp, "scenario.log_memory_map = %u\n", scenario.log_memory_map);
  fprintf (fp, "scenario.log_landclass_summary = %u\n",
           scenario.log_landclass_summary);
  fprintf (fp, "scenario.log_slope_weights = %u\n", scenario.log_slope_weights);
  fprintf (fp, "scenario.log_reads = %u\n", scenario.log_reads);
  fprintf (fp, "scenario.log_writes = %u\n", scenario.log_writes);
  fprintf (fp, "scenario.log_colortables = %u\n", scenario.log_colortables);
  fprintf (fp, "scenario.log_processing_status = %u\n",
           scenario.log_processing_status);
  fprintf (fp, "scenario.log_trans_matrix = %u\n", scenario.log_trans_matrix);
  fprintf (fp, "scenario.view_growth_types = %u\n", scenario.view_growth_types);
  fprintf (fp, "scenario.growth_type_window.run1 = %d\n",
           scenario.growth_type_window.run1);
  fprintf (fp, "scenario.growth_type_window.run2 = %d\n",
           scenario.growth_type_window.run2);
  fprintf (fp, "scenario.growth_type_window.monte_carlo1 = %d\n",
           scenario.growth_type_window.monte_carlo1);
  fprintf (fp, "scenario.growth_type_window.monte_carlo2 = %d\n",
           scenario.growth_type_window.monte_carlo2);
  fprintf (fp, "scenario.growth_type_window.year1 = %d\n",
           scenario.growth_type_window.year1);
  fprintf (fp, "scenario.growth_type_window.year2 = %d\n",
           scenario.growth_type_window.year2);
  fprintf (fp, "scenario.phase0g_growth_color = %x\n",
           scenario.phase0g_growth_color);
  fprintf (fp, "scenario.phase1g_growth_color = %x\n",
           scenario.phase1g_growth_color);
  fprintf (fp, "scenario.phase2g_growth_color = %x\n",
           scenario.phase2g_growth_color);
  fprintf (fp, "scenario.phase3g_growth_color = %x\n",
           scenario.phase3g_growth_color);
  fprintf (fp, "scenario.phase4g_growth_color = %x\n",
           scenario.phase4g_growth_color);
  fprintf (fp, "scenario.phase5g_growth_color = %x\n",
           scenario.phase5g_growth_color);
  fprintf (fp, "scenario.view_deltatron_aging = %u\n",
           scenario.view_deltatron_aging);
  fprintf (fp, "scenario.deltatron_aging_window.run1 = %d\n",
           scenario.deltatron_aging_window.run1);
  fprintf (fp, "scenario.deltatron_aging_window.run2 = %d\n",
           scenario.deltatron_aging_window.run2);
  fprintf (fp, "scenario.deltatron_aging_window.monte_carlo1 = %d\n",
           scenario.deltatron_aging_window.monte_carlo1);
  fprintf (fp, "scenario.deltatron_aging_window.monte_carlo2 = %d\n",
           scenario.deltatron_aging_window.monte_carlo2);
  fprintf (fp, "scenario.deltatron_aging_window.year1 = %d\n",
           scenario.deltatron_aging_window.year1);
  fprintf (fp, "scenario.deltatron_aging_window.year2 = %d\n",
           scenario.deltatron_aging_window.year2);
  for (index = 0; index < scenario.deltatron_color_count; index++)
  {
    fprintf (fp, "scenario.deltatron_color[%u] = %u\n",
             index, scenario.deltatron_color[index]);
  }

  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_open_log
** PURPOSE:       open the log file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  scen_CloseLog ()
{
  char func[] = "scen_CloseLog";
  FUNC_INIT;

  if (scenario.log_fp)
  {
    fclose (scenario.log_fp);
    scenario.log_fp = NULL;
  }
  else
  {
    sprintf (msg_buf, "Log file is not open");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: scen_open_log
** PURPOSE:       open the log file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  scen_open_log ()
{
  char func[] = "scen_open_log";
  static int opened = 0;

  FUNC_INIT;
  if (scenario.logging == TRUE)
  {
    if (opened > 0)
    {
      sprintf (msg_buf, "log file already open");
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
    sprintf (log_filename, "%sLOG_%u", scenario.output_dir, glb_mype);

    if (!proc_GetRestartFlag ());
    {
      FILE_OPEN (scenario.log_fp, log_filename, "w");
      scen_CloseLog ();
    }

    opened = 1;
  }
  else
  {
    scenario.log_fp = NULL;
  }
  FUNC_END;
}
