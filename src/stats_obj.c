#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <omp.h>
#include "ugm_defines.h"
#include "pgrid_obj.h"
#include "proc_obj.h"
#include "igrid_obj.h"
#include "memory_obj.h"
#include "scenario_obj.h"
#include "ugm_macros.h"
#include "stats_obj.h"
#include "coeff_obj.h"
#include "utilities.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char stats_obj_c_sccs_id[] = "@(#)stats_obj.c	1.72	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define MAX_LINE_LEN 256
#define SIZE_CIR_Q 5000

#define Q_STORE(R,C)                                                     \
  int temp_variable = omp_get_thread_num();                               \
  if((sidx[temp_variable]+1==ridx[temp_variable])||((sidx[temp_variable]+1==SIZE_CIR_Q)&&!ridx[temp_variable])){                     \
    printf("Error Circular Queue Full\n");                               \
    printf("Increase SIZE_CIR_Q and recompile\n");                       \
    printf("sidx=%d ridx=%d SIZE_CIR_Q=%d\n",sidx[temp_variable],ridx[temp_variable],SIZE_CIR_Q);      \
    EXIT(1);}                                                            \
  cir_q[temp_variable][sidx[temp_variable]].row = R;                                                   \
  cir_q[temp_variable][sidx[temp_variable]].col = C;                                                   \
  sidx[temp_variable]++;                                                                \
  depth++;                                                               \
  sidx[temp_variable] %= SIZE_CIR_Q
#define Q_RETREIVE(R,C)                                                  \
  ridx[temp_variable] = ridx[temp_variable]%SIZE_CIR_Q;                                                \
  R = cir_q[temp_variable][ridx[temp_variable]].row;                                                   \
  C = cir_q[temp_variable][ridx[temp_variable]].col;                                                   \
  ridx[temp_variable]++;                                                                \
  depth--


/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static char *stats_val_t_names[] = {
  "sng",
  "sdg",
  "sdc",
  "og",
  "rt",
  "pop",
  "area",
  "edges",
  "clusters",
  "xmean",
  "ymean",
  "rad",
  "slope",
  "cl_size",
  "diffus",
  "spread",
  "breed",
  "slp_res",
  "rd_grav",
  "%urban",
  "%road",
  "grw_rate",
  "leesalee",
  "grw_pix"
};
static stats_info stats_actual[NUM_THREADS][MAX_URBAN_YEARS];
static stats_info regression[NUM_THREADS];
static stats_val_t average[NUM_THREADS][MAX_URBAN_YEARS];
static stats_val_t std_dev[NUM_THREADS][MAX_URBAN_YEARS];
static stats_val_t running_total[NUM_THREADS][MAX_URBAN_YEARS];
static struct
{
  int run;
  int monte_carlo;
  int year;
  stats_val_t this_year;
}
record[NUM_THREADS];

static struct
{
  double fmatch;
  double actual;
  double simulated;
  double compare;
  double leesalee;
  double product;
}
aggregate[NUM_THREADS];

static struct
{
  long successes;
  long z_failure;
  long delta_failure;
  long slope_failure;
  long excluded_failure;
}
urbanization_attempt[NUM_THREADS];

static int sidx[NUM_THREADS];
static int ridx[NUM_THREADS];

/* link element for Cluster routine */
typedef struct ugm_link
{
  int row;
  int col;
}
ugm_link;

static struct ugm_link cir_q[NUM_THREADS][SIZE_CIR_Q];

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void stats_Save (char *filename);
static void stats_LogThisYearStats (FILE * fp);
static void stats_CalGrowthRate ();
static void stats_CalPercentUrban (int, int, int);
static void stats_CalAverages (int index);
static void stats_WriteControlStats (char *filename);
static void stats_WriteStatsValLine (char *filename, int run,
                           int year, stats_val_t * stats_ptr, int index);
static void stats_LogStatInfoHdr (FILE * fp);
static void stats_LogStatInfo (int run, int year, int index,
                               stats_info * stats_ptr, FILE * fp);
static void stats_LogStatVal (int run, int year, int index,
                              stats_val_t * stats_ptr, FILE * fp);
static void stats_LogStatValHdr (FILE * fp);
static void stats_ComputeThisYearStats ();
static void stats_SetNumGrowthPixels (int val);
static void stats_CalLeesalee ();
static void stats_ProcessGrowLog (int run, int year);
static void stats_DoAggregate (double fmatch);
static void stats_DoRegressions ();
static double stats_linefit (double *dependent,
                             double *independent,
                             int number_of_observations);
static void stats_LogControlStats (FILE * fp);
static void stats_LogControlStatsHdr (FILE * fp);
static void
    stats_compute_stats (GRID_P Z,                           /* IN     */
                         GRID_P slp,                         /* IN     */
                         double *stats_area,                 /* OUT    */
                         double *stats_edges,                /* OUT    */
                         double *stats_clusters,             /* OUT    */
                         double *stats_pop,                  /* OUT    */
                         double *stats_xmean,                /* OUT    */
                         double *stats_ymean,                /* OUT    */
                         double *stats_average_slope,        /* OUT    */
                         double *stats_rad,                  /* OUT    */
                         double *stats_mean_cluster_size,    /* OUT    */
                         GRID_P scratch_gif1,                /* MOD    */
                         GRID_P scratch_gif2);             /* MOD    */
static void
    stats_edge (GRID_P Z,                                    /* IN     */
                double *stats_area,                          /* OUT    */
                double *stats_edges);                      /* OUT    */
static void
    stats_circle (GRID_P Z,                                  /* IN     */
                  GRID_P slp,                                /* IN     */
                  int stats_area,                            /* IN     */
                  double *stats_xmean,                       /* OUT    */
                  double *stats_ymean,                       /* OUT    */
                  double *stats_average_slope,               /* OUT    */
                  double *stats_rad);                      /* OUT    */
static void
    stats_cluster (GRID_P Z,                                 /* IN     */
                   double *stats_clusters,                   /* OUT    */
                   double *stats_pop,                        /* OUT    */
                   double *stats_mean_cluster_size,          /* OUT    */
                   GRID_P scratch_gif1,                      /* MOD    */
                   GRID_P scratch_gif2);                   /* MOD    */
static void stats_ClearStatsValArrays ();
static void stats_ComputeBaseStats ();
static void stats_CalStdDev (int index);
static void
    stats_compute_leesalee (GRID_P Z,                        /* IN     */
                            GRID_P urban,                    /* IN     */
                            double *leesalee);             /* OUT    */




/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
#if 1
void
  stats_ConcatenateControlFiles ()
{
  char func[] = "stats_ConcatenateControlFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;
  int i;

  sprintf (destination_file, "%scontrol_stats.log", scen_GetOutputDir ());
  sprintf (source_file, "%scontrol_stats_pe_%u.log", scen_GetOutputDir (), 0);
  sprintf (command, "mv %s %s", source_file, destination_file);
  system (command);

  FILE_OPEN (fp, destination_file, "a");
  for (i = 1; i < glb_npes; i++)
  {
    sprintf (source_file, "%scontrol_stats_pe_%u.log", scen_GetOutputDir (), i);

    FILE_OPEN (source_fp, source_file, "r");

    line_count = 0;
    while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
    {
      line_count++;
      if (line_count <= 2)
        continue;
      fputs (line, fp);
    }
    fclose (source_fp);
    sprintf (command, "rm %s", source_file);
    printf ("%s %u command: %s\n", __FILE__, __LINE__, command);
    system (command);

  }
  fclose (fp);
}
#else
void
  stats_ConcatenateControlFiles (int current_run)
{
  char func[] = "stats_ConcatenateControlFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;

  sprintf (destination_file, "%scontrol_stats.log", scen_GetOutputDir ());
  sprintf (source_file, "%scontrol_stats_pe_%u.log", scen_GetOutputDir (), 0);

  FILE_OPEN (fp, destination_file, "a");

  sprintf (source_file, "%scontrol_stats_pe_%u.log",
           scen_GetOutputDir (), current_run);

  FILE_OPEN (source_fp, source_file, "r");

  line_count = 0;
  while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
  {
    line_count++;
    if (line_count <= 2)
      continue;
    fputs (line, fp);
  }
  fclose (source_fp);
  fclose (fp);
  sprintf (command, "rm %s", source_file);
  system (command);
}
#endif
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
#if 1
void
  stats_ConcatenateStdDevFiles ()
{
  char func[] = "stats_ConcatenateStdDevFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;
  int i;

  sprintf (destination_file, "%sstd_dev.log", scen_GetOutputDir ());
  sprintf (source_file, "%sstd_dev_pe_%u.log", scen_GetOutputDir (), 0);
  sprintf (command, "mv %s %s", source_file, destination_file);
  system (command);

  FILE_OPEN (fp, destination_file, "a");
  for (i = 1; i < glb_npes; i++)
  {
    sprintf (source_file, "%sstd_dev_pe_%u.log", scen_GetOutputDir (), i);

    FILE_OPEN (source_fp, source_file, "r");

    line_count = 0;
    while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
    {
      line_count++;
      if (line_count <= 1)
        continue;
      fputs (line, fp);
    }
    fclose (source_fp);
    sprintf (command, "rm %s", source_file);
    printf ("%s %u command: %s\n", __FILE__, __LINE__, command);
    system (command);

  }
  fclose (fp);
#else
void
  stats_ConcatenateStdDevFiles (int current_run)
{
  char func[] = "stats_ConcatenateStdDevFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;


  sprintf (destination_file, "%sstd_dev.log", scen_GetOutputDir ());
  sprintf (source_file, "%sstd_dev_pe_%u.log", scen_GetOutputDir (), 0);

  FILE_OPEN (fp, destination_file, "a");

  sprintf (source_file, "%sstd_dev_pe_%u.log",
           scen_GetOutputDir (), current_run);

  FILE_OPEN (source_fp, source_file, "r");

  line_count = 0;
  while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
  {
    line_count++;
    if (line_count <= 1)
      continue;
    fputs (line, fp);
  }
  fclose (source_fp);
  fclose (fp);
  sprintf (command, "rm %s", source_file);
  system (command);
#endif
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
#if 1
void
  stats_ConcatenateAvgFiles ()
{

  char func[] = "stats_ConcatenateAvgFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;
  int i;

  sprintf (destination_file, "%savg.log", scen_GetOutputDir ());
  sprintf (source_file, "%savg_pe_%u.log", scen_GetOutputDir (), 0);
  sprintf (command, "mv %s %s", source_file, destination_file);
  system (command);

  FILE_OPEN (fp, destination_file, "a");
  for (i = 1; i < glb_npes; i++)
  {
    sprintf (source_file, "%savg_pe_%u.log", scen_GetOutputDir (), i);

    FILE_OPEN (source_fp, source_file, "r");

    line_count = 0;
    while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
    {
      line_count++;
      if (line_count <= 1)
        continue;
      fputs (line, fp);
    }
    fclose (source_fp);
    sprintf (command, "rm %s", source_file);
    printf ("%s %u command: %s\n", __FILE__, __LINE__, command);
    system (command);

  }
  fclose (fp);
#else

void
  stats_ConcatenateAvgFiles (int current_run)
{
  char func[] = "stats_ConcatenateAvgFiles";
  char line[MAX_LINE_LEN];
  char source_file[MAX_FILENAME_LEN];
  char destination_file[MAX_FILENAME_LEN];
  char command[2 * MAX_FILENAME_LEN + 20];
  FILE *fp;
  FILE *source_fp;
  int line_count;

  sprintf (destination_file, "%savg.log", scen_GetOutputDir ());
  sprintf (source_file, "%savg_pe_%u.log", scen_GetOutputDir (), 0);

  FILE_OPEN (fp, destination_file, "a");

  sprintf (source_file, "%savg_pe_%u.log", scen_GetOutputDir (), current_run);

  FILE_OPEN (source_fp, source_file, "r");

  line_count = 0;
  while (fgets (line, MAX_LINE_LEN, source_fp) != NULL)
  {
    line_count++;
    if (line_count <= 1)
      continue;
    fputs (line, fp);
  }
  fclose (source_fp);
  fclose (fp);
  sprintf (command, "rm %s", source_file);
#if 1
  system (command);
#endif
#endif
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/



void
  stats_Update (int num_growth_pix)
{
  char func[] = "stats_Update";
  char filename[MAX_FILENAME_LEN];
  int total_pixels;
  int road_pixel_count;
  int excluded_pixel_count;

  total_pixels = mem_GetTotalPixels ();
  road_pixel_count = igrid_GetIGridRoadPixelCount (proc_GetCurrentYear ());
  excluded_pixel_count = igrid_GetIGridExcludedPixelCount ();

  stats_ComputeThisYearStats ();
  stats_SetNumGrowthPixels (num_growth_pix);
  stats_CalGrowthRate ();
  stats_CalPercentUrban (total_pixels, road_pixel_count, excluded_pixel_count);

  if (igrid_TestForUrbanYear (proc_GetCurrentYear ()))
  {
    stats_CalLeesalee ();
    sprintf (filename, "%sgrow_%u_%u.log",
    scen_GetOutputDir (), proc_GetRun (), proc_GetCurrentYear ());
    stats_Save (filename);
  }
  if (proc_GetProcessingType () == PREDICTING)
  {
    sprintf (filename, "%sgrow_%u_%u.log", scen_GetOutputDir (),
             proc_GetRun (), proc_GetCurrentYear ());
    stats_Save (filename);
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_Init ()
{
  static BOOLEAN first_call = TRUE;

  //stats_ClearStatsValArrays ();
  int i, j;

  for (i = 0; i < NUM_THREADS; ++i)
  {
    for (j = 0; j < MAX_URBAN_YEARS; ++j)
    {
      memset ((void *) (&running_total[i][j]), 0, sizeof (stats_val_t));
      memset ((void *) (&average[i][j]), 0, sizeof (stats_val_t));
      memset ((void *) (&std_dev[i][j]), 0, sizeof (stats_val_t));
    }
    memset ((void *) (&regression[i]), 0, sizeof (stats_info));
  }

  // for (i = 0; i < MAX_URBAN_YEARS; i++)
  // {
  //   memset ((void *) (&running_total[thread_id][i]), 0, sizeof (stats_val_t));
  //   memset ((void *) (&average[thread_id][i]), 0, sizeof (stats_val_t));
  //   memset ((void *) (&std_dev[thread_id][i]), 0, sizeof (stats_val_t));
  // }
  // memset ((void *) (&regression[thread_id]), 0, sizeof (stats_info));
  if (first_call)
  {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
      stats_ComputeBaseStats ();
    }
    first_call = FALSE;
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_CalStdDev (int index)
{
#define SD(val) pow(((val)*(val)/total_monte_carlo),0.5)
  int total_monte_carlo;
  int thread_id = omp_get_thread_num();

  total_monte_carlo = scen_GetMonteCarloIterations ();

  std_dev[thread_id][index].sng = SD (record[thread_id].this_year.sng - average[thread_id][index].sng);
  std_dev[thread_id][index].sdg = SD (record[thread_id].this_year.sdg - average[thread_id][index].sdg);
  std_dev[thread_id][index].sdc = SD (record[thread_id].this_year.sdc - average[thread_id][index].sdc);
  std_dev[thread_id][index].og = SD (record[thread_id].this_year.og - average[thread_id][index].og);
  std_dev[thread_id][index].rt = SD (record[thread_id].this_year.rt - average[thread_id][index].rt);
  std_dev[thread_id][index].pop = SD (record[thread_id].this_year.pop - average[thread_id][index].pop);
  std_dev[thread_id][index].area = SD (record[thread_id].this_year.area - average[thread_id][index].area);
  std_dev[thread_id][index].edges = SD (record[thread_id].this_year.edges - average[thread_id][index].edges);
  std_dev[thread_id][index].clusters =
    SD (record[thread_id].this_year.clusters - average[thread_id][index].clusters);
  std_dev[thread_id][index].xmean = SD (record[thread_id].this_year.xmean - average[thread_id][index].xmean);
  std_dev[thread_id][index].ymean = SD (record[thread_id].this_year.ymean - average[thread_id][index].ymean);
  std_dev[thread_id][index].rad = SD (record[thread_id].this_year.rad - average[thread_id][index].rad);
  std_dev[thread_id][index].slope = SD (record[thread_id].this_year.slope - average[thread_id][index].slope);
  std_dev[thread_id][index].mean_cluster_size =
    SD (record[thread_id].this_year.mean_cluster_size - average[thread_id][index].mean_cluster_size);
  std_dev[thread_id][index].diffusion =
    SD (record[thread_id].this_year.diffusion - average[thread_id][index].diffusion);
  std_dev[thread_id][index].spread = SD (record[thread_id].this_year.spread - average[thread_id][index].spread);
  std_dev[thread_id][index].breed = SD (record[thread_id].this_year.breed - average[thread_id][index].breed);
  std_dev[thread_id][index].slope_resistance =
    SD (record[thread_id].this_year.slope_resistance - average[thread_id][index].slope_resistance);
  std_dev[thread_id][index].road_gravity =
    SD (record[thread_id].this_year.road_gravity - average[thread_id][index].road_gravity);
  std_dev[thread_id][index].percent_urban =
    SD (record[thread_id].this_year.percent_urban - average[thread_id][index].percent_urban);
  std_dev[thread_id][index].percent_road =
    SD (record[thread_id].this_year.percent_road - average[thread_id][index].percent_road);
  std_dev[thread_id][index].growth_rate =
    SD (record[thread_id].this_year.growth_rate - average[thread_id][index].growth_rate);
  std_dev[thread_id][index].leesalee =
    SD (record[thread_id].this_year.leesalee - average[thread_id][index].leesalee);
  std_dev[thread_id][index].num_growth_pix =
    SD (record[thread_id].this_year.num_growth_pix - average[thread_id][index].num_growth_pix);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_CalAverages (int index)
{
  int total_monte_carlo;
  int thread_id = omp_get_thread_num();

  total_monte_carlo = scen_GetMonteCarloIterations ();

  average[thread_id][index].sng = running_total[thread_id][index].sng / total_monte_carlo;
  average[thread_id][index].sdg = running_total[thread_id][index].sdg / total_monte_carlo;
  average[thread_id][index].sdc = running_total[thread_id][index].sdc / total_monte_carlo;
  average[thread_id][index].og = running_total[thread_id][index].og / total_monte_carlo;
  average[thread_id][index].rt = running_total[thread_id][index].rt / total_monte_carlo;
  average[thread_id][index].pop = running_total[thread_id][index].pop / total_monte_carlo;
  average[thread_id][index].area = running_total[thread_id][index].area / total_monte_carlo;
  average[thread_id][index].edges = running_total[thread_id][index].edges / total_monte_carlo;
  average[thread_id][index].clusters = running_total[thread_id][index].clusters / total_monte_carlo;
  average[thread_id][index].xmean = running_total[thread_id][index].xmean / total_monte_carlo;
  average[thread_id][index].ymean = running_total[thread_id][index].ymean / total_monte_carlo;
  average[thread_id][index].rad = running_total[thread_id][index].rad / total_monte_carlo;
  average[thread_id][index].slope = running_total[thread_id][index].slope / total_monte_carlo;
  average[thread_id][index].mean_cluster_size =
    running_total[thread_id][index].mean_cluster_size / total_monte_carlo;
  average[thread_id][index].diffusion =
    running_total[thread_id][index].diffusion / total_monte_carlo;
  average[thread_id][index].spread = running_total[thread_id][index].spread / total_monte_carlo;
  average[thread_id][index].breed = running_total[thread_id][index].breed / total_monte_carlo;
  average[thread_id][index].slope_resistance =
    running_total[thread_id][index].slope_resistance / total_monte_carlo;
  average[thread_id][index].road_gravity =
    running_total[thread_id][index].road_gravity / total_monte_carlo;
  average[thread_id][index].percent_urban =
    running_total[thread_id][index].percent_urban / total_monte_carlo;
  average[thread_id][index].percent_road =
    running_total[thread_id][index].percent_road / total_monte_carlo;
  average[thread_id][index].growth_rate =
    running_total[thread_id][index].growth_rate / total_monte_carlo;
  average[thread_id][index].leesalee = running_total[thread_id][index].leesalee / total_monte_carlo;
  average[thread_id][index].num_growth_pix =
    running_total[thread_id][index].num_growth_pix / total_monte_carlo;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static void
  stats_UpdateRunningTotal (int index)
{
#ifndef lint
  char func[] = "stats_UpdateRunningTotal";
#endif

  int thread_id = omp_get_thread_num();

  running_total[thread_id][index].sng += record[thread_id].this_year.sng;
  running_total[thread_id][index].sdg += record[thread_id].this_year.sdg;
  running_total[thread_id][index].sdc += record[thread_id].this_year.sdc;
  running_total[thread_id][index].og += record[thread_id].this_year.og;
  running_total[thread_id][index].rt += record[thread_id].this_year.rt;
  running_total[thread_id][index].pop += record[thread_id].this_year.pop;
  running_total[thread_id][index].area += record[thread_id].this_year.area;
  running_total[thread_id][index].edges += record[thread_id].this_year.edges;
  running_total[thread_id][index].clusters += record[thread_id].this_year.clusters;
  running_total[thread_id][index].xmean += record[thread_id].this_year.xmean;
  running_total[thread_id][index].ymean += record[thread_id].this_year.ymean;
  running_total[thread_id][index].rad += record[thread_id].this_year.rad;
  running_total[thread_id][index].slope += record[thread_id].this_year.slope;
  running_total[thread_id][index].mean_cluster_size += record[thread_id].this_year.mean_cluster_size;
  running_total[thread_id][index].diffusion += record[thread_id].this_year.diffusion;
  running_total[thread_id][index].spread += record[thread_id].this_year.spread;
  running_total[thread_id][index].breed += record[thread_id].this_year.breed;
  running_total[thread_id][index].slope_resistance += record[thread_id].this_year.slope_resistance;
  running_total[thread_id][index].road_gravity += record[thread_id].this_year.road_gravity;
  running_total[thread_id][index].percent_urban += record[thread_id].this_year.percent_urban;
  running_total[thread_id][index].percent_road += record[thread_id].this_year.percent_road;
  running_total[thread_id][index].growth_rate += record[thread_id].this_year.growth_rate;
  running_total[thread_id][index].leesalee += record[thread_id].this_year.leesalee;
  running_total[thread_id][index].num_growth_pix += record[thread_id].this_year.num_growth_pix;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static void
  stats_ClearStatsValArrays ()
{
  char func[] = "stats_ClearStatsValArrays";
  int i;
  int thread_id = omp_get_thread_num();

  for (i = 0; i < MAX_URBAN_YEARS; i++)
  {
    memset ((void *) (&running_total[thread_id][i]), 0, sizeof (stats_val_t));
    memset ((void *) (&average[thread_id][i]), 0, sizeof (stats_val_t));
    memset ((void *) (&std_dev[thread_id][i]), 0, sizeof (stats_val_t));
  }
  memset ((void *) (&regression[thread_id]), 0, sizeof (stats_info));
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetLeesalee ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.leesalee;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_CalLeesalee ()
{
  char func[] = "stats_CalLeesalee";
  GRID_P z_ptr;
  GRID_P urban_ptr;
  int thread_id = omp_get_thread_num();

  z_ptr = pgrid_GetZPtr (thread_id);
  urban_ptr = igrid_GetUrbanGridPtrByYear (__FILE__, func,
                                       __LINE__, proc_GetCurrentYear ());
  record[thread_id].this_year.leesalee = 1.0;
  if (proc_GetProcessingType () != PREDICTING)
  {
    stats_compute_leesalee (z_ptr,                           /* IN     */
                            urban_ptr,                       /* IN     */
                            &record[thread_id].this_year.leesalee);   /* OUT    */
  }
  urban_ptr = igrid_GridRelease (__FILE__, func, __LINE__, urban_ptr);

}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_SetNumGrowthPixels (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.num_growth_pix = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetNumGrowthPixels ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.num_growth_pix;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  stats_SetPercentUrban (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.percent_urban = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_CalPercentUrban (int total_pixels, int road_pixels, int excld_pixels)
{
  int i = omp_get_thread_num();
  record[i].this_year.percent_urban =
    (double) (100.0 * (record[i].this_year.pop + road_pixels) /
              (total_pixels - road_pixels - excld_pixels));
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetPercentUrban ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.percent_urban;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static void
  stats_CalGrowthRate ()
{
  int i = omp_get_thread_num();
  record[i].this_year.growth_rate =
    record[i].this_year.num_growth_pix / record[i].this_year.pop * 100.0;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetGrowthRate ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.growth_rate;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/


void
  stats_SetSNG (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.sng = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetSDG (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.sdg = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetOG (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.og = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetRT (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.rt = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetPOP (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.pop = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

int
  stats_GetSNG ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.sng;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetSDG ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.sdg;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetOG ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.og;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetRT ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.rt;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetPOP ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.pop;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  stats_SetArea (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.area = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetEdges (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.edges = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetClusters (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.clusters = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetPop (int val)
{
  int i = omp_get_thread_num();
  record[i].this_year.pop = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetXmean (double val)
{
  int i = omp_get_thread_num();
  record[i].this_year.xmean = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetYmean (double val)
{
  int i = omp_get_thread_num();
  record[i].this_year.ymean = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetRad (double val)
{
  int i = omp_get_thread_num();
  record[i].this_year.rad = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetAvgSlope (double val)
{
  int i = omp_get_thread_num();
  record[i].this_year.slope = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_SetMeanClusterSize (double val)
{
  int i = omp_get_thread_num();
  record[i].this_year.mean_cluster_size = val;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

int
  stats_GetArea ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.area;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetEdges ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.edges;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetClusters ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.clusters;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  stats_GetPop ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.pop;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetXmean ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.xmean;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetYmean ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.ymean;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetRad ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.rad;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetAvgSlope ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.slope;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
double
  stats_GetMeanClusterSize ()
{
  int i = omp_get_thread_num();
  return record[i].this_year.mean_cluster_size;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static void
  stats_ComputeThisYearStats ()
{
  char func[] = "stats_ComputeThisYearStats";
  int total_pixels;
  GRID_P z_ptr;
  GRID_P slope_ptr;
  GRID_P stats_workspace1;
  GRID_P stats_workspace2;
  double area;
  double edges;
  double clusters;
  double pop;
  double xmean;
  double ymean;
  double slope;
  double rad;
  double mean_cluster_size;
  int thread_id = omp_get_thread_num();

  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);
  z_ptr = pgrid_GetZPtr (thread_id);
  assert (z_ptr != NULL);

  slope_ptr = igrid_GetSlopeGridPtr (__FILE__, func, __LINE__);
  stats_workspace1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
  stats_workspace2 = mem_GetWGridPtr (__FILE__, func, __LINE__);

  stats_compute_stats (z_ptr,                                /* IN     */
                       slope_ptr,                            /* IN     */
                       &area,                                /* OUT    */
                       &edges,                               /* OUT    */
                       &clusters,                            /* OUT    */
                       &pop,                                 /* OUT    */
                       &xmean,                               /* OUT    */
                       &ymean,                               /* OUT    */
                       &slope,                               /* OUT    */
                       &rad,                                 /* OUT    */
                       &mean_cluster_size,                   /* OUT    */
                       stats_workspace1,                     /* MOD    */
                       stats_workspace2);                  /* MOD    */
  #pragma omp critical
  {
    printf("\n%s %f %f %f %f %f %f %f %f %s\n", "开始", area, edges, clusters, pop, xmean, ymean, slope, rad, mean_cluster_size, "结束");
  }
  record[thread_id].this_year.area = area;
  record[thread_id].this_year.edges = edges;
  record[thread_id].this_year.clusters = clusters;
  record[thread_id].this_year.pop = pop;
  record[thread_id].this_year.xmean = xmean;
  record[thread_id].this_year.ymean = ymean;
  record[thread_id].this_year.slope = slope;
  record[thread_id].this_year.rad = rad;
  record[thread_id].this_year.mean_cluster_size = mean_cluster_size;
  record[thread_id].this_year.diffusion = coeff_GetCurrentDiffusion ();
  record[thread_id].this_year.spread = coeff_GetCurrentSpread ();
  record[thread_id].this_year.breed = coeff_GetCurrentBreed ();
  record[thread_id].this_year.slope_resistance = coeff_GetCurrentSlopeResist ();
  record[thread_id].this_year.road_gravity = coeff_GetCurrentRoadGravity ();

  slope_ptr = igrid_GridRelease (__FILE__, func, __LINE__, slope_ptr);
  stats_workspace1 = mem_GetWGridFree (__FILE__, func, __LINE__,
                                       stats_workspace1);
  stats_workspace2 = mem_GetWGridFree (__FILE__, func, __LINE__,
                                       stats_workspace2);


}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_CreateControlFile (char *filename)
{
  char func[] = "stats_CreateControlFile";
  FILE *fp;

  FILE_OPEN (fp, filename, "w");

  stats_LogControlStatsHdr (fp);
  fclose (fp);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_CreateStatsValFile (char *filename)
{
  char func[] = "stats_CreateStatsValFile";
  FILE *fp;

  FILE_OPEN (fp, filename, "w");

  stats_LogStatValHdr (fp);
  fclose (fp);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_WriteStatsValLine (char *filename, int run, int year,
                           stats_val_t * stats_ptr, int index)
{
  char func[] = "stats_WriteStatsValLine";
  FILE *fp;

  FILE_OPEN (fp, filename, "a");

  stats_LogStatVal (run, year, index, &(stats_ptr[index]), fp);
  fclose (fp);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogStatInfoHdr (FILE * fp)
{
  fprintf (fp, "  run year index  area    edges clusters      pop    xmean");
  fprintf (fp, "    ymean      rad    slope cluster_size  %%urban\n");
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogStatValHdr (FILE * fp)
{
#if 1
  int i;
  int num_elements;
  num_elements = sizeof (stats_val_t) / sizeof (double);
  fprintf (fp, "  run year index");
  for (i = 0; i < num_elements; i++)
  {
    fprintf (fp, "%8s ", stats_val_t_names[i]);
  }
  fprintf (fp, "\n");
#else
  fprintf (fp, "\n");
  fprintf (fp, "  run year index  area    edges clusters      pop    xmean");
  fprintf (fp, "    ymean      rad    slope cluster_size  sng      sdg");
  fprintf (fp, "      sdc       og       rt      pop\n");
#endif
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogStatInfo (int run, int year, int index,
                     stats_info * stats_ptr, FILE * fp)
{

  fprintf (fp, "%5u %4u %2u ", run, year, index);
  fprintf (fp, "%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
           stats_ptr->area,
           stats_ptr->edges,
           stats_ptr->clusters,
           stats_ptr->pop,
           stats_ptr->xmean,
           stats_ptr->ymean,
           stats_ptr->rad,
           stats_ptr->average_slope,
           stats_ptr->mean_cluster_size,
           stats_ptr->percent_urban
    );
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogStatVal (int run, int year, int index,
                    stats_val_t * stats_ptr, FILE * fp)
{
  int i;
  int num_elements;
  double *ptr;

  /*num_elements = sizeof(struct stats_val_t)/sizeof(double); */
  num_elements = sizeof (stats_val_t) / sizeof (double);
  ptr = (double *) stats_ptr;

  fprintf (fp, "%5u %4u %2u   ", run, year, index);
#if 1
  for (i = 0; i < num_elements; i++)
  {
    fprintf (fp, "%8.2f ", *ptr);
    ptr++;
  }
  fprintf (fp, "\n");
#else
  fprintf (fp, "%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f ",
           stats_ptr->area,
           stats_ptr->edges,
           stats_ptr->clusters,
           stats_ptr->pop,
           stats_ptr->xmean,
           stats_ptr->ymean,
           stats_ptr->rad,
           stats_ptr->slope
    );
  fprintf (fp, "%8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f \n",
           stats_ptr->mean_cluster_size,
           stats_ptr->sng,
           stats_ptr->sdg,
           stats_ptr->sdc,
           stats_ptr->og,
           stats_ptr->rt,
           stats_ptr->pop
    );
#endif
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_LogAverages (int index, FILE * fp)
{
  int thread_id = omp_get_thread_num();
  LOG_INT (fp, index);
  LOG_FLOAT (fp, average[thread_id][index].area);
  LOG_FLOAT (fp, average[thread_id][index].edges);
  LOG_FLOAT (fp, average[thread_id][index].clusters);
  LOG_FLOAT (fp, average[thread_id][index].pop);
  LOG_FLOAT (fp, average[thread_id][index].xmean);
  LOG_FLOAT (fp, average[thread_id][index].ymean);
  LOG_FLOAT (fp, average[thread_id][index].rad);
  LOG_FLOAT (fp, average[thread_id][index].slope);
  LOG_FLOAT (fp, average[thread_id][index].mean_cluster_size);
  LOG_FLOAT (fp, average[thread_id][index].sng);
  LOG_FLOAT (fp, average[thread_id][index].sdg);
  LOG_FLOAT (fp, average[thread_id][index].sdc);
  LOG_FLOAT (fp, average[thread_id][index].og);
  LOG_FLOAT (fp, average[thread_id][index].rt);
  LOG_FLOAT (fp, average[thread_id][index].pop);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogThisYearStats (FILE * fp)
{
  int thread_id = omp_get_thread_num();
  LOG_FLOAT (fp, record[thread_id].this_year.area);
  LOG_FLOAT (fp, record[thread_id].this_year.edges);
  LOG_FLOAT (fp, record[thread_id].this_year.clusters);
  LOG_FLOAT (fp, record[thread_id].this_year.pop);
  LOG_FLOAT (fp, record[thread_id].this_year.xmean);
  LOG_FLOAT (fp, record[thread_id].this_year.ymean);
  LOG_FLOAT (fp, record[thread_id].this_year.rad);
  LOG_FLOAT (fp, record[thread_id].this_year.slope);
  LOG_FLOAT (fp, record[thread_id].this_year.mean_cluster_size);
  LOG_FLOAT (fp, record[thread_id].this_year.sng);
  LOG_FLOAT (fp, record[thread_id].this_year.sdg);
  LOG_FLOAT (fp, record[thread_id].this_year.sdc);
  LOG_FLOAT (fp, record[thread_id].this_year.og);
  LOG_FLOAT (fp, record[thread_id].this_year.rt);
  LOG_FLOAT (fp, record[thread_id].this_year.pop);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_LogRecord (FILE * fp)
{
  int thread_id = omp_get_thread_num();
  LOG_INT (fp, record[thread_id].run);
  LOG_INT (fp, record[thread_id].monte_carlo);
  LOG_INT (fp, record[thread_id].year);
  stats_LogThisYearStats (fp);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_compute_leesalee (GRID_P Z,                          /* IN     */
                          GRID_P urban,                      /* IN     */
                          double *leesalee)                /* OUT    */
{
  char func[] = "stats_compute_leesalee";
  int i;
  int the_union;
  int intersection;

  FUNC_INIT;
  assert (Z != NULL);
  assert (urban != NULL);
  assert (leesalee != NULL);

  the_union = 0;
  intersection = 0;
  for (i = 0; i < mem_GetTotalPixels (); i++)
  {
    if ((Z[i] != 0) || (urban[i] != 0))
    {
      the_union++;
    }

    if ((Z[i] != 0) && (urban[i] != 0))
    {
      intersection++;
    }
  }

  *leesalee = (double) intersection / the_union;
  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_Analysis (double fmatch)
{
#if 1
  char func[] = "stats_Analysis";
#endif
  char std_filename[MAX_FILENAME_LEN];
  char avg_filename[MAX_FILENAME_LEN];
  char cntrl_filename[MAX_FILENAME_LEN];
  char *output_dir;
  int yr;
  int i;
  int run;
  static int avg_log_created = 0;
  static int std_dev_log_created = 0;
  static int control_stats_log_created = 0;
  int thread_id = omp_get_thread_num();

  output_dir = scen_GetOutputDir ();
  run = proc_GetRun ();

  if (scen_GetWriteAvgFileFlag ())
  {
    sprintf (avg_filename, "%savg_pe_%u.log", output_dir, glb_mype);
    if (!avg_log_created)
    {
      stats_CreateStatsValFile (avg_filename);
      avg_log_created = 1;
    }
  }

  if (scen_GetWriteStdDevFileFlag ())
  {
    sprintf (std_filename, "%sstd_dev_pe_%u.log", output_dir, glb_mype);
    if (!std_dev_log_created)
    {
      stats_CreateStatsValFile (std_filename);
      std_dev_log_created = 1;
    }
  }

  if (proc_GetProcessingType () != PREDICTING)
  {
    sprintf (cntrl_filename, "%scontrol_stats_pe_%u.log", output_dir, glb_mype);
    if (!control_stats_log_created)
    {
      stats_CreateControlFile (cntrl_filename);
      control_stats_log_created = 1;
    }
  }

  if (proc_GetProcessingType () != PREDICTING)
  {
    /*
     *
     * start at i = 1, i = 0 is the initial seed
     *
     */
    for (i = 1; i < igrid_GetUrbanCount (); i++)
    {
      yr = igrid_GetUrbanYear (i);
      stats_CalAverages (i);
      stats_ProcessGrowLog (run, yr);

      if (scen_GetWriteAvgFileFlag ())
      {
        stats_WriteStatsValLine (avg_filename, run, yr, average[thread_id], i);
      }
      if (scen_GetWriteStdDevFileFlag ())
      {
        stats_WriteStatsValLine (std_filename, run, yr, std_dev[thread_id], i);
      }
    }
    stats_DoRegressions ();
    stats_DoAggregate (fmatch);
    stats_WriteControlStats (cntrl_filename);
  }
  if (proc_GetProcessingType () == PREDICTING)
  {
    for (yr = scen_GetPredictionStartDate () + 1;
         yr <= proc_GetStopYear (); yr++)
    {
#if 1
      stats_ClearStatsValArrays ();
#endif
      stats_ProcessGrowLog (run, yr);
      if (scen_GetWriteAvgFileFlag ())
      {
        stats_WriteStatsValLine (avg_filename, run, yr, average[thread_id], 0);
      }
      if (scen_GetWriteStdDevFileFlag ())
      {
        stats_WriteStatsValLine (std_filename, run, yr, std_dev[thread_id], 0);
      }
#if 1
      stats_ClearStatsValArrays ();
#endif
    }
  }
  stats_ClearStatsValArrays ();
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_Dump (char *file, int line)
{
  int i;
  int yr;
  int thread_id = omp_get_thread_num();

  fprintf (stdout, "%s %u stats_Dump\n", file, line);
  stats_LogStatValHdr (stdout);
  fprintf (stdout, "this_year:\n");
  stats_LogStatVal (proc_GetRun (), proc_GetCurrentYear (),
                    0, &record[thread_id].this_year, stdout);
  fprintf (stdout, "running_total:\n");
  for (i = 0; i < MAX_URBAN_YEARS; i++)
  {
    yr = igrid_GetUrbanYear (i);
    if (i == 0)
    {
      yr = 0;
    }
    stats_LogStatVal (proc_GetRun (), yr, i,
                      &running_total[thread_id][i], stdout);
  }
  fprintf (stdout, "average:\n");
  for (i = 0; i < MAX_URBAN_YEARS; i++)
  {
    yr = igrid_GetUrbanYear (i);
    if (i == 0)
    {
      yr = 0;
    }
    stats_LogStatVal (proc_GetRun (), yr, i, &average[thread_id][i], stdout);
  }
  fprintf (stdout, "std_dev:\n");
  for (i = 0; i < MAX_URBAN_YEARS; i++)
  {
    yr = igrid_GetUrbanYear (i);
    if (i == 0)
    {
      yr = 0;
    }
    stats_LogStatVal (proc_GetRun (), yr, i, &std_dev[thread_id][i], stdout);
  }
  stats_LogStatInfoHdr (stdout);
  fprintf (stdout, "stats_actual:\n");
  for (i = 0; i < MAX_URBAN_YEARS; i++)
  {
    yr = igrid_GetUrbanYear (i);
    if (i == 0)
    {
      yr = 0;
    }
    stats_LogStatInfo (proc_GetRun (), yr, i,
                       &stats_actual[thread_id][i], stdout);
  }
  fprintf (stdout, "regression:\n");
  stats_LogStatInfo (proc_GetRun (), 0, 0, &regression[thread_id], stdout);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogControlStatsHdr (FILE * fp)
{
  fprintf (fp, "                                               Cluster\n");
  fprintf (fp, "  Run  Product Compare     Pop   Edges Clusters   ");
  fprintf (fp, "Size Leesalee  Slope ");
  fprintf (fp, " %%Urban   Xmean   Ymean     Rad  Fmatch ");
  fprintf (fp, "Diff  Brd Sprd  Slp   RG\n");
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_LogControlStats (FILE * fp)
{
  int thread_id = omp_get_thread_num();
  #pragma omp critical
  {
    fprintf (fp, "%5u %8.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f ",
            proc_GetRun (),
            aggregate[thread_id].product,
            aggregate[thread_id].compare,
            regression[thread_id].pop,
            regression[thread_id].edges,
            regression[thread_id].clusters,
            regression[thread_id].mean_cluster_size,
            aggregate[thread_id].leesalee,
            regression[thread_id].average_slope,
            regression[thread_id].percent_urban);
    fprintf (fp, "%7.5f %7.5f %7.5f %7.5f %4.0f %4.0f %4.0f %4.0f %4.0f\n",
            regression[thread_id].xmean,
            regression[thread_id].ymean,
            regression[thread_id].rad,
            aggregate[thread_id].fmatch,
            coeff_GetSavedDiffusion (),
            coeff_GetSavedBreed (),
            coeff_GetSavedSpread (),
            coeff_GetSavedSlopeResist (),
            coeff_GetSavedRoadGravity ());
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_DoAggregate (double fmatch)
{
  char func[] = "stats_DoAggregate";
  int last_index;
  int i;
  double fmatch_tmp = 1.0;
  double numerator;
  double denominator;
  int thread_id = omp_get_thread_num();

  last_index = igrid_GetUrbanCount () - 1;
  aggregate[thread_id].fmatch = fmatch;
  aggregate[thread_id].actual = stats_actual[thread_id][last_index].pop;
  aggregate[thread_id].simulated = average[thread_id][last_index].pop;
  aggregate[thread_id].leesalee = 0.0;
  for (i = 1; i < igrid_GetUrbanCount (); i++)
  {
    aggregate[thread_id].leesalee += average[thread_id][i].leesalee;
  }
  aggregate[thread_id].leesalee /= (igrid_GetUrbanCount () - 1);
  if (aggregate[thread_id].actual > aggregate[thread_id].simulated)
  {
    if (aggregate[thread_id].actual != 0.0)
    {
      denominator = aggregate[thread_id].actual;
      numerator = aggregate[thread_id].simulated;
      aggregate[thread_id].compare = numerator / denominator;
    }
    else
    {
      sprintf (msg_buf, "aggregate[%d].actual = 0.0", thread_id);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
  else
  {
    if (aggregate[thread_id].simulated != 0.0)
    {
      denominator = aggregate[thread_id].simulated;
      numerator = aggregate[thread_id].actual;
      aggregate[thread_id].compare = numerator / denominator;
    }
    else
    {
      sprintf (msg_buf, "aggregate[%d].simulated = 0.0", thread_id);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
  if (scen_GetDoingLanduseFlag ())
  {
    fmatch_tmp = fmatch;
  }
  aggregate[thread_id].product =
    aggregate[thread_id].compare *
    aggregate[thread_id].leesalee *
    regression[thread_id].edges *
    regression[thread_id].clusters *
    regression[thread_id].pop *
    regression[thread_id].xmean *
    regression[thread_id].ymean *
    regression[thread_id].rad *
    regression[thread_id].average_slope *
    regression[thread_id].mean_cluster_size *
    regression[thread_id].percent_urban *
    fmatch_tmp;

}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_DoRegressions ()
{
  double dependent[MAX_URBAN_YEARS];
  double independent[MAX_URBAN_YEARS];
  int nobs;
  int i;
  int thread_id = omp_get_thread_num();

  nobs = igrid_GetUrbanCount () - 1;
  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].area;
    independent[i - 1] = average[thread_id][i].area;
  }
  regression[thread_id].area = stats_linefit (dependent, independent, nobs);

  nobs = igrid_GetUrbanCount () - 1;
  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].edges;
    independent[i - 1] = average[thread_id][i].edges;
  }
  regression[thread_id].edges = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].clusters;
    independent[i - 1] = average[thread_id][i].clusters;
  }
  regression[thread_id].clusters = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].pop;
    independent[i - 1] = average[thread_id][i].pop;
  }
  regression[thread_id].pop = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].xmean;
    independent[i - 1] = average[thread_id][i].xmean;
  }
  regression[thread_id].xmean = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].ymean;
    independent[i - 1] = average[thread_id][i].ymean;
  }
  regression[thread_id].ymean = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].rad;
    independent[i - 1] = average[thread_id][i].rad;
  }
  regression[thread_id].rad = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].average_slope;
    independent[i - 1] = average[thread_id][i].slope;
  }
  regression[thread_id].average_slope = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].mean_cluster_size;
    independent[i - 1] = average[thread_id][i].mean_cluster_size;
  }
  regression[thread_id].mean_cluster_size = stats_linefit (dependent, independent, nobs);

  for (i = 1; i <= nobs; i++)
  {
    dependent[i - 1] = stats_actual[thread_id][i].percent_urban;
    independent[i - 1] = average[thread_id][i].percent_urban;
  }
  regression[thread_id].percent_urban = stats_linefit (dependent, independent, nobs);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_Save (char *filename)
{
  char func[] = "stats_Save";
  int num_written;
  int fseek_loc;
  int index;
  int i;
  FILE *fp;
  int thread_id = omp_get_thread_num();
  record[thread_id].run = proc_GetRun ();
  record[thread_id].monte_carlo = proc_GetCurrentMonteCarlo ();
  record[thread_id].year = proc_GetCurrentYear ();
  index = 0;
  if (proc_GetProcessingType () != PREDICTING)
  {
    index = igrid_UrbanYear2Index (record[thread_id].year);
  }

  stats_UpdateRunningTotal (index);

  #pragma omp critical
  {
    if (record[thread_id].monte_carlo == 0)
    {
      FILE_OPEN (fp, filename, "wb");
      for (i = 0; i < scen_GetMonteCarloIterations (); i++)
      {
       num_written = fwrite (&record[thread_id], sizeof (record[thread_id]), 1, fp);
       if (num_written != 1)
       {
         printf ("%s %u ERROR\n", __FILE__, __LINE__);
        }
      }
    }
    else
    {
      FILE_OPEN (fp, filename, "r+b");
      rewind (fp);
      fseek_loc = fseek (fp, sizeof (record[thread_id]) * record[thread_id].monte_carlo, SEEK_SET);
      num_written = fwrite (&record[thread_id], sizeof (record[thread_id]), 1, fp);
      if (num_written != 1)
      {
         printf ("%s %u ERROR\n", __FILE__, __LINE__);
      }
    }
    fclose (fp);
  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_ProcessGrowLog (int run, int year)
{
  char func[] = "stats_ProcessGrowLog";
  FILE *fp;
  int index;
  int fseek_loc;
  int i;
  int mc_count = 0;
  char filename[MAX_FILENAME_LEN];
  char command[MAX_FILENAME_LEN + 3];
  int thread_id = omp_get_thread_num();

  sprintf (filename, "%sgrow_%u_%u.log", scen_GetOutputDir (), run, year);
  sprintf (command, "rm %s", filename);

  FILE_OPEN (fp, filename, "rb");

  if (proc_GetProcessingType () != PREDICTING)
  {
    while (fread (&record[thread_id], sizeof (record[thread_id]), 1, fp))
    {
      if (mc_count >= scen_GetMonteCarloIterations ())
      {
        sprintf (msg_buf, "mc_count >= scen_GetMonteCarloIterations ()");
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      if (feof (fp) || ferror (fp))
      {
        sprintf (msg_buf, "feof (fp) || ferror (fp)");
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      index = igrid_UrbanYear2Index (year);
      stats_CalStdDev (index);
      mc_count++;
    }
  }
  else
  {
    while (fread (&record[thread_id], sizeof (record[thread_id]), 1, fp))
    {
      if (mc_count >= scen_GetMonteCarloIterations ())
      {
        sprintf (msg_buf, "mc_count >= scen_GetMonteCarloIterations ()");
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      if (feof (fp) || ferror (fp))
      {
        sprintf (msg_buf, "feof (fp) || ferror (fp)");
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      stats_UpdateRunningTotal (0);
    }
    stats_CalAverages (0);
    rewind (fp);
    mc_count = 0;
    while (fread (&record[thread_id], sizeof (record[thread_id]), 1, fp))
    {
      if (mc_count >= scen_GetMonteCarloIterations ())
      {
        sprintf (msg_buf, "mc_count >= scen_GetMonteCarloIterations ()");
        LOG_ERROR (msg_buf);
        sprintf (msg_buf, "mc_count= %u scen_GetMonteCarloIterations= %u",
                 mc_count, scen_GetMonteCarloIterations ());
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      if (feof (fp) || ferror (fp))
      {
        sprintf (msg_buf, "feof (fp) || ferror (fp)");
        LOG_ERROR (msg_buf);
        EXIT (1);
      }
      stats_CalStdDev (0);
      mc_count++;
    }
  }
  fclose (fp);
  system (command);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static double
  stats_linefit (double *dependent,
                 double *independent,
                 int number_of_observations)
{
  char func[] = "Linefit";
  double dependent_avg;
  double independent_avg;
  double cross;
  double sum_dependent;
  double sum_independent;
  double r;
  int n;

  FUNC_INIT;
  assert (dependent != NULL);
  assert (independent != NULL);
  assert (number_of_observations > 0);

  dependent_avg = 0;
  independent_avg = 0;

  for (n = 0; n < number_of_observations; n++)

  {
    dependent_avg += dependent[n];
    independent_avg += independent[n];
  }

  if (number_of_observations > 0)
  {
    dependent_avg /= (double) number_of_observations;
    independent_avg /= (double) number_of_observations;
  }
  else
  {
    sprintf (msg_buf, "number_of_observations = %d", number_of_observations);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  cross = 0;
  sum_dependent = 0;
  sum_independent = 0;

  for (n = 0; n < number_of_observations; n++)
  {
    cross += ((dependent[n] - dependent_avg) * (independent[n] -
                                                independent_avg));
    sum_dependent += ((dependent[n] - dependent_avg) * (dependent[n] -
                                                        dependent_avg));
    sum_independent += ((independent[n] - independent_avg) * (independent[n]
                                                     - independent_avg));
  }

  r = 0;

  if (sum_dependent * sum_independent < 1e-11)
    r = 0;
  else
    r = cross / pow (sum_dependent * sum_independent, 0.5);

  FUNC_END;
  return (r * r);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

static void
  stats_ComputeBaseStats ()
{
  char func[] = "stats_ComputeBaseStats";
  int i;
  int total_pixels;
  GRID_P urban_ptr;
  GRID_P slope_ptr;
  GRID_P stats_workspace1;
  GRID_P stats_workspace2;
  int road_pixel_count;
  int excluded_pixel_count;
  int thread_id = omp_get_thread_num();

  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);

  for (i = 0; i < igrid_GetUrbanCount (); i++)
  {
    urban_ptr = igrid_GetUrbanGridPtr (__FILE__, func, __LINE__, i);
    slope_ptr = igrid_GetSlopeGridPtr (__FILE__, func, __LINE__);
    stats_workspace1 = mem_GetWGridPtr (__FILE__, func, __LINE__);
    stats_workspace2 = mem_GetWGridPtr (__FILE__, func, __LINE__);

    stats_compute_stats (urban_ptr,                          /* IN     */
                         slope_ptr,                          /* IN     */
                         &stats_actual[thread_id][i].area,              /* OUT    */
                         &stats_actual[thread_id][i].edges,             /* OUT    */
                         &stats_actual[thread_id][i].clusters,          /* OUT    */
                         &stats_actual[thread_id][i].pop,               /* OUT    */
                         &stats_actual[thread_id][i].xmean,             /* OUT    */
                         &stats_actual[thread_id][i].ymean,             /* OUT    */
                         &stats_actual[thread_id][i].average_slope,     /* OUT    */
                         &stats_actual[thread_id][i].rad,               /* OUT    */
                         &stats_actual[thread_id][i].mean_cluster_size,   /* OUT    */
                         stats_workspace1,                   /* MOD    */
                         stats_workspace2);                /* MOD    */

    // #pragma omp critical
    // {
    //   printf("\n%s\n%4.5f %4.5f %4.5f %4.5f %4.5f %4.5f %4.5f %4.5f %4.5f\n%s\n", "开始", stats_actual[thread_id][i].area, stats_actual[thread_id][i].edges, stats_actual[thread_id][i].clusters, stats_actual[thread_id][i].pop, stats_actual[thread_id][i].xmean, stats_actual[thread_id][i].ymean, stats_actual[thread_id][i].average_slope, stats_actual[thread_id][i].rad, stats_actual[thread_id][i].mean_cluster_size, "结束");
    // }

    road_pixel_count = igrid_GetIGridRoadPixelCount (proc_GetCurrentYear ());
    excluded_pixel_count = igrid_GetIGridExcludedPixelCount ();
    stats_actual[thread_id][i].percent_urban = 100.0 *
      100.0 * (stats_actual[thread_id][i].pop + road_pixel_count) /
      (igrid_GetNumRows () * igrid_GetNumCols () - road_pixel_count -
       excluded_pixel_count);

    urban_ptr = igrid_GridRelease (__FILE__, func, __LINE__, urban_ptr);
    slope_ptr = igrid_GridRelease (__FILE__, func, __LINE__, slope_ptr);
    stats_workspace1 = mem_GetWGridFree (__FILE__, func, __LINE__,
                                         stats_workspace1);
    stats_workspace2 = mem_GetWGridFree (__FILE__, func, __LINE__,
                                         stats_workspace2);

  }
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_compute_stats (GRID_P Z,                             /* IN     */
                       GRID_P slp,                           /* IN     */
                       double *stats_area,                   /* OUT    */
                       double *stats_edges,                  /* OUT    */
                       double *stats_clusters,               /* OUT    */
                       double *stats_pop,                    /* OUT    */
                       double *stats_xmean,                  /* OUT    */
                       double *stats_ymean,                  /* OUT    */
                       double *stats_average_slope,          /* OUT    */
                       double *stats_rad,                    /* OUT    */
                       double *stats_mean_cluster_size,      /* OUT    */
                       GRID_P scratch_gif1,                  /* MOD    */
                       GRID_P scratch_gif2)                /* MOD    */

{
  char func[] = "stats_compute_stats";

  FUNC_INIT;
  assert (Z != NULL);
  assert (slp != NULL);
  assert (stats_area != NULL);
  assert (stats_edges != NULL);
  assert (stats_clusters != NULL);
  assert (stats_pop != NULL);
  assert (stats_xmean != NULL);
  assert (stats_ymean != NULL);
  assert (stats_average_slope != NULL);
  assert (stats_rad != NULL);
  assert (stats_mean_cluster_size != NULL);
  assert (scratch_gif1 != NULL);
  assert (scratch_gif2 != NULL);
  /*
   *
   * compute the number of edge pixels
   *
   */
  stats_edge (Z,                                             /* IN     */
              stats_area,                                    /* OUT    */
              stats_edges);                                /* OUT    */


  /*
   *
   * compute the number of clusters
   *
   */
  stats_cluster (Z,                                          /* IN     */
                 stats_clusters,                             /* OUT    */
                 stats_pop,                                  /* OUT    */
                 stats_mean_cluster_size,                    /* OUT    */
                 scratch_gif1,                               /* MOD    */
                 scratch_gif2);                            /* MOD    */

  /*
   *
   * compute means
   *
   */
  stats_circle (Z,                                           /* IN     */
                slp,                                         /* IN     */
                *stats_area,                                 /* IN     */
                stats_xmean,                                 /* OUT    */
                stats_ymean,                                 /* OUT    */
                stats_average_slope,                         /* OUT    */
                stats_rad);                                /* OUT    */

  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_edge (GRID_P Z,                                      /* IN     */
              double *stats_area,                            /* OUT    */
              double *stats_edges)                         /* OUT    */
{
  char func[] = "stats_edge";
  int i;
  int j;
  int edge;
  int edges;
  int area;
  int rowi[4] = {-1, 1, 0, 0};
  int colj[4] = {0, 0, -1, 1};
  int loop;
  int row;
  int col;
  int nrows;
  int ncols;

  FUNC_INIT;
  assert (stats_area != NULL);
  assert (stats_edges != NULL);
  assert (Z != NULL);
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);

  edges = 0;
  area = 0;

  // #pragma omp paraller for default(shared) private(i,j,edge,loop,row,col) reduction(+:area,edges)
  for (i = 0; i < nrows; i++)
  {
    for (j = 0; j < ncols; j++)
    {
      edge = FALSE;

      if (Z[OFFSET (i, j)] != 0)
      {
        area++;

        /* this does a 4 neighbor search (N, S, E, W) */
        for (loop = 0; loop <= 3; loop++)
        {
          row = i + rowi[loop];
          col = j + colj[loop];

          if (IMAGE_PT (row, col))
          {
            if (Z[OFFSET (row, col)] == 0)
            {
              edge = TRUE;
            }
          }
        }

        if (edge)
        {
          edges++;
        }
      }
    }
  }
  *stats_area = area;
  *stats_edges = edges;
  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_circle (GRID_P Z,                                    /* IN     */
                GRID_P slp,                                  /* IN     */
                int stats_area,                              /* IN     */
                double *stats_xmean,                         /* OUT    */
                double *stats_ymean,                         /* OUT    */
                double *stats_average_slope,                 /* OUT    */
                double *stats_rad)                         /* OUT    */
{
  char func[] = "stats_circle";
  int i;
  int j;
  int number;
  double xmean;
  double ymean;
  double addslope;
  int nrows;
  int ncols;

  FUNC_INIT;
  assert (stats_xmean != NULL);
  assert (stats_ymean != NULL);
  assert (stats_average_slope != NULL);
  assert (stats_rad != NULL);
  assert (Z != NULL);
  assert (slp != NULL);

  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);
  addslope = 0.0;
  ymean = 0.0;
  xmean = 0.0;
  number = 0.0;

  /*
   *
   * first, compute the means
   *
   */
  // #pragma omp paraller for default(shared) private(i,j) reduction(+:addslope,xmean,ymean,number)
  for (i = 0; i < nrows; i++)
  {
    for (j = 0; j < ncols; j++)
    {
      if (Z[OFFSET (i, j)] > 0)
      {
        addslope += slp[OFFSET (i, j)];
        xmean += (double) j;
        ymean += (double) i;
        number++;
      }
    }
  }

  if (number <= 0)
  {
    sprintf (msg_buf, "number = %d", number);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  xmean /= (double) number;
  ymean /= (double) number;
  *stats_xmean = xmean;
  *stats_ymean = ymean;
  *stats_average_slope = addslope / number;

  /*
   *
   * compute the radius of the circle with same area as number
   *
   */
  *stats_rad = pow ((stats_area / PI), 0.5);

  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_MemoryLog (FILE * fp)
{
  // LOG_MEM (fp, &cir_q[0], sizeof (ugm_link), SIZE_CIR_Q);
  // LOG_MEM (fp, &stats_actual[0], sizeof (stats_info), MAX_URBAN_YEARS);
  // LOG_MEM (fp, &regression, sizeof (stats_info), 1);
  // LOG_MEM (fp, &average[0], sizeof (stats_val_t), MAX_URBAN_YEARS);
  // LOG_MEM (fp, &std_dev[0], sizeof (stats_val_t), MAX_URBAN_YEARS);
  // LOG_MEM (fp, &running_total[0], sizeof (stats_val_t), MAX_URBAN_YEARS);
  // LOG_MEM (fp, &urbanization_attempt, sizeof (urbanization_attempt), 1);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_cluster (GRID_P Z,                                   /* IN     */
                 double *stats_clusters,                     /* OUT    */
                 double *stats_pop,                          /* OUT    */
                 double *stats_mean_cluster_size,            /* OUT    */
                 GRID_P scratch_gif1,                        /* MOD    */
                 GRID_P scratch_gif2)                      /* MOD    */
{
  char func[] = "stats_cluster";
  int i;
  int j;
  int depth;
  int num_clusters;
  int sum;
  int row;
  int col;
  int rrow;
  int ccol;
  int rowi[4] = {1, 0, -1, 0};
  int colj[4] = {0, 1, 0, -1};
  int loop;
  long *visited;
  long *clusters;
  int total_pixels;
  int nrows;
  int ncols;

  FUNC_INIT;
  assert (stats_clusters != NULL);
  assert (stats_pop != NULL);
  assert (stats_mean_cluster_size != NULL);
  assert (Z != NULL);
  assert (scratch_gif1 != NULL);
  assert (scratch_gif2 != NULL);
  assert (scratch_gif1 != scratch_gif2);
  total_pixels = mem_GetTotalPixels ();
  assert (total_pixels > 0);
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  assert (nrows > 0);
  assert (ncols > 0);

  sum = 0;
  *stats_pop = 0;
  depth = 0;
  num_clusters = 0;

  visited = scratch_gif1;
  clusters = scratch_gif2;
  //#pragma omp paraller for 
  for (i = 0; i < total_pixels; i++)
  {
    visited[i] = 0;
  }
  //double pop = *stats_pop;
  //#pragma omp paraller for default(shared) private(i) reduction(+:pop)
  for (i = 0; i < total_pixels; i++)
  {
    if (Z[i] != 0)
    {
      clusters[i] = 1;
      (*stats_pop)++;
    }
    else
    {
      clusters[i] = 0;
    }
  }
  // *stats_pop = pop;
  // #pragma omp paraller for
  for (j = 0; j < ncols; j++)
  {
    clusters[OFFSET (0, j)] = 0;
    clusters[OFFSET (nrows - 1, j)] = 0;
  }
  // #pragma omp paraller for
  for (i = 0; i < nrows; i++)
  {
    clusters[OFFSET (i, 0)] = 0;
    clusters[OFFSET (i, ncols - 1)] = 0;
  }
  // #pragma omp paraller for default(shared) private(i,j,rrow,ccol,loop,row,col,depth,sum) reduction(+:num_clusters)
  for (i = 1; i < nrows - 1; i++)
  {
    for (j = 1; j < ncols - 1; j++)
    {
      if (clusters[OFFSET (i, j)] == 1 && visited[OFFSET (i, j)] == 0)
      {
        sum++;
        rrow = i;
        ccol = j;
        visited[OFFSET (i, j)] = 1;
        Q_STORE (rrow, ccol);
        do
        {
          Q_RETREIVE (row, col);
          for (loop = 0; loop <= 3; loop++)
          {
            rrow = row + rowi[loop];
            ccol = col + colj[loop];

            if (IMAGE_PT (rrow, ccol))
            {
              if (clusters[OFFSET (rrow, ccol)] == 1 &&
                  !visited[OFFSET (rrow, ccol)])
              {
                visited[OFFSET (rrow, ccol)] = 1;
                Q_STORE (rrow, ccol);

                sum++;
              }
            }
          }
        }
        while (depth > 0);

        num_clusters++;
      }
    }
  }

  *stats_clusters = num_clusters;
  if (num_clusters > 0)
  {
    *stats_mean_cluster_size = sum / num_clusters;
  }
  else
  {
    sprintf (msg_buf, "num_clusters=%d %d", num_clusters, omp_get_thread_num());
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/

void
  stats_LogBaseStats (FILE * fp)
{
  char func[] = "stats_LogBaseStats";
  int i;
  int count;
  int thread_id = omp_get_thread_num();

  FUNC_INIT;
  count = igrid_GetUrbanCount ();
  assert (count > 0);


  fprintf (fp, "\n\n");
  fprintf (fp, "************************LOG OF BASE STATISTICS");
  fprintf (fp, " FOR URBAN INPUT DATA********************\n");
  fprintf (fp, " Year       Area      Edges   Clusters         ");
  fprintf (fp, "Pop       Mean Center        Radius");
  fprintf (fp, "   Avg Slope  MeanClusterSize\n");
  for (i = 0; i < count; i++)
  {
    fprintf (fp, "%5d   %8.2f   %8.2f   %8.2f    %8.2f  (%8.2f,%8.2f)",
             igrid_GetUrbanYear (i),
             stats_actual[thread_id][i].area,
             stats_actual[thread_id][i].edges,
             stats_actual[thread_id][i].clusters,
             stats_actual[thread_id][i].pop,
             stats_actual[thread_id][i].xmean,
             stats_actual[thread_id][i].ymean);
    fprintf (fp, "   %8.2f  %10.2f      %6.3f\n",
             stats_actual[thread_id][i].rad,
             stats_actual[thread_id][i].average_slope,
             stats_actual[thread_id][i].mean_cluster_size);
  }
  fprintf (fp, "\n\n");
  FUNC_END;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  stats_WriteControlStats (char *filename)
{
  char func[] = "stats_WriteControlStats";
  FILE *fp;

  FILE_OPEN (fp, filename, "a");

  stats_LogControlStats (fp);
  fclose (fp);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_InitUrbanizationAttempts ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].successes = 0;
  urbanization_attempt[i].z_failure = 0;
  urbanization_attempt[i].delta_failure = 0;
  urbanization_attempt[i].slope_failure = 0;
  urbanization_attempt[i].excluded_failure = 0;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_LogUrbanizationAttempts (FILE * fp)
{
  int total;

  int i = omp_get_thread_num();

  total = urbanization_attempt[i].successes +
    urbanization_attempt[i].z_failure +
    urbanization_attempt[i].delta_failure +
    urbanization_attempt[i].slope_failure +
    urbanization_attempt[i].excluded_failure;

  fprintf (fp, "\nLOG OF URBANIZATION ATTEMPTS\n");
  fprintf (fp, "Num Success                = %u\n",
           urbanization_attempt[i].successes);
  fprintf (fp, "Num Z Type Failures        = %u\n",
           urbanization_attempt[i].z_failure);
  fprintf (fp, "Num Delta Type Failures    = %u\n",
           urbanization_attempt[i].delta_failure);
  fprintf (fp, "Num Slope Type Failures    = %u\n",
           urbanization_attempt[i].slope_failure);
  fprintf (fp, "Num Exlcuded Type Failures = %u\n",
           urbanization_attempt[i].excluded_failure);
  fprintf (fp, "Total Attempts             = %u\n", total);
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_IncrementUrbanSuccess ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].successes++;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_IncrementZFailure ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].z_failure++;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_IncrementDeltaFailure ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].delta_failure++;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_IncrementSlopeFailure ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].slope_failure++;
}
/******************************************************************************
*******************************************************************************
** FUNCTION NAME: 
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  stats_IncrementEcludedFailure ()
{
  int i = omp_get_thread_num();
  urbanization_attempt[i].excluded_failure++;
}
