#ifndef SCENARIO_OBJ_H
#define SCENARIO_OBJ_H
#include <stdio.h>
#include "ugm_defines.h"
#define SCEN_MAX_FILENAME_LEN 256
#define SCEN_MAX_URBAN_YEARS 20
#define SCEN_MAX_ROAD_YEARS 20
#define SCEN_MAX_LANDUSE_YEARS 2
#define SCEN_MAX_LANDUSE_CLASSES 256

#ifdef SCENARIO_OBJ_MODULE
#include "coeff_obj.h"


typedef struct
{
  int run1;
  int run2;
  int monte_carlo1;
  int monte_carlo2;
  int year1;
  int year2;
} print_window_t;

typedef struct
{
  int lower_bound;
  int upper_bound;
  int color;
} prob_color_info;

typedef struct
{
  char name[80];
  char type[80];
  int color;
  int grayscale;
} landuse_class_info;

typedef struct
{
  FILE* log_fp;
  char filename[SCEN_MAX_FILENAME_LEN];
  char input_dir[SCEN_MAX_FILENAME_LEN];
  char output_dir[SCEN_MAX_FILENAME_LEN];
  char whirlgif_binary[SCEN_MAX_FILENAME_LEN];
  char urban_data_file[SCEN_MAX_URBAN_YEARS][SCEN_MAX_FILENAME_LEN];
  int urban_data_file_count;
  char road_data_file[SCEN_MAX_ROAD_YEARS][SCEN_MAX_FILENAME_LEN];
  int road_data_file_count;
  char landuse_data_file[SCEN_MAX_LANDUSE_YEARS][SCEN_MAX_FILENAME_LEN];
  int landuse_data_file_count;
  char excluded_data_file[SCEN_MAX_FILENAME_LEN];
  char slope_data_file[SCEN_MAX_FILENAME_LEN];
  char background_data_file[SCEN_MAX_FILENAME_LEN];
  BOOLEAN echo;
  BOOLEAN logging;
  BOOLEAN postprocessing;
  int random_seed;
  int num_working_grids;
  int monte_carlo_iterations;
  coeff_int_info start;
  coeff_int_info stop;
  coeff_int_info step;
  coeff_int_info best_fit;
  int prediction_start_date;
  int prediction_stop_date;
  int date_color;
  int seed_color;
  int water_color;
  prob_color_info probability_color[MAX_PROBABILITY_COLORS];
  int num_landuse_classes;
  landuse_class_info landuse_class[SCEN_MAX_LANDUSE_CLASSES];
  int probability_color_count;
  double rd_grav_sensitivity;
  double slope_sensitivity;
  double critical_low;
  double critical_high;
  double critical_slope;
  double boom;
  double bust;
  BOOLEAN log_base_stats;
  BOOLEAN log_debug;
  BOOLEAN log_urbanization_attempts;
  BOOLEAN log_coeff;
  int log_timings;
  BOOLEAN write_coeff_file;
  BOOLEAN write_avg_file;
  BOOLEAN echo_image_files;
  BOOLEAN write_color_keys;
  BOOLEAN write_std_dev_file;
  BOOLEAN log_memory_map;
  BOOLEAN log_landclass_summary;
  BOOLEAN log_slope_weights;
  BOOLEAN log_reads;
  BOOLEAN log_writes;
  BOOLEAN log_colortables;
  BOOLEAN log_processing_status;
  BOOLEAN log_trans_matrix;
  BOOLEAN view_growth_types;
  print_window_t growth_type_window;
  int phase0g_growth_color;
  int phase1g_growth_color;
  int phase2g_growth_color;
  int phase3g_growth_color;
  int phase4g_growth_color;
  int phase5g_growth_color;
  BOOLEAN view_deltatron_aging;
  print_window_t deltatron_aging_window;
  int deltatron_color[256];
  int deltatron_color_count;
} scenario_info;
#endif

void scen_MemoryLog(FILE* fp);
void
scen_init(char* filename);

void
scen_echo(FILE* fp);
FILE* scen_GetLogFP();
char* scen_GetScenarioFilename();
char* scen_GetOutputDir();
char* scen_GetWhirlgifBinary ();
char* scen_GetInputDir();
int   scen_GetUrbanDataFileCount();
int   scen_GetRoadDataFileCount();
int   scen_GetLanduseDataFileCount();
int   scen_GetDoingLanduseFlag();
char* scen_GetUrbanDataFilename(int i);
char* scen_GetRoadDataFilename(int i);
char* scen_GetLanduseDataFilename(int i);
char* scen_GetExcludedDataFilename();
char* scen_GetSlopeDataFilename();
char* scen_GetBackgroundDataFilename();
BOOLEAN scen_GetEchoFlag();
BOOLEAN scen_GetLogFlag();
BOOLEAN scen_GetPostprocessingFlag();
int   scen_GetRandomSeed();
int   scen_GetMonteCarloIterations();
int   scen_GetCoeffDiffusionStart();
int   scen_GetCoeffBreedStart();
int   scen_GetCoeffSpreadStart();
int   scen_GetCoeffSlopeResistStart();
int   scen_GetCoeffRoadGravityStart();
int   scen_GetCoeffDiffusionStop();
int   scen_GetCoeffBreedStop();
int   scen_GetCoeffSpreadStop();
int   scen_GetCoeffSlopeResistStop();
int   scen_GetCoeffRoadGravityStop();
int   scen_GetCoeffDiffusionStep();
int   scen_GetCoeffBreedStep();
int   scen_GetCoeffSpreadStep();
int   scen_GetCoeffSlopeResistStep();
int   scen_GetCoeffRoadGravityStep();
int   scen_GetCoeffDiffusionBestFit();
int   scen_GetCoeffBreedBestFit();
int   scen_GetCoeffSpreadBestFit();
int   scen_GetCoeffSlopeResistBestFit();
int   scen_GetCoeffRoadGravityBestFit();
int   scen_GetPredictionStartDate();
int   scen_GetPredictionStopDate();
int   scen_GetDateColor();
int   scen_GetSeedColor();
int   scen_GetWaterColor();
int   scen_GetProbabilityColorCount();
int   scen_GetProbabilityColorLowerBound(int i);
int   scen_GetProbabilityColorUpperBound(int i);
int   scen_GetProbabilityColor(int i);
BOOLEAN   scen_GetLogMemoryMapFlag();
BOOLEAN   scen_GetLogLandclassSummaryFlag();
BOOLEAN   scen_GetLogSlopeWeightsFlag();
BOOLEAN   scen_GetLogReadsFlag();
BOOLEAN   scen_GetLogWritesFlag();
BOOLEAN   scen_GetLogColortablesFlag();
BOOLEAN   scen_GetLogProcessingStatusFlag();
BOOLEAN   scen_GetViewGrowthTypesFlag();
BOOLEAN   scen_GetViewDeltatronAgingFlag();
int   scen_GetPhase0GrowthColor();
int   scen_GetPhase1GrowthColor();
int   scen_GetPhase2GrowthColor();
int   scen_GetPhase3GrowthColor();
int   scen_GetPhase4GrowthColor();
int   scen_GetPhase5GrowthColor();
int scen_GetDeltatronColor (int index);
int scen_GetDeltatronColorCount ();
void scen_CloseLog ();
void scen_Append2Log ();
BOOLEAN scen_GetLogTransitionMatrixFlag ();
double scen_GetRdGrvtySensitivity ();
double scen_GetSlopeSensitivity ();
double scen_GetCriticalHigh ();
double scen_GetCriticalLow ();
double scen_GetCriticalSlope ();
double scen_GetBoom ();
double scen_GetBust ();
int scen_GetLogTimingsFlag();
BOOLEAN scen_GetLogCoeffFlag();
BOOLEAN scen_GetLogUrbanizationAttemptsFlag();
BOOLEAN scen_GetLogBaseStatsFlag();
BOOLEAN scen_GetLogDebugFlag();
BOOLEAN scen_GetWriteCoeffFileFlag();
BOOLEAN scen_GetWriteAvgFileFlag();
BOOLEAN scen_GetWriteStdDevFileFlag();
BOOLEAN scen_GetWriteColorKeyFlag();
BOOLEAN scen_GetEchoImageFlag();
int scen_GetNumLanduseClasses ();
char* scen_GetLanduseClassName (int);
char* scen_GetLanduseClassType (int);
int scen_GetLanduseClassColor (int);
int scen_GetLanduseClassGrayscale (int i);
#endif

