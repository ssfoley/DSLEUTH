#ifndef STATS_OBJ_H
#define STATS_OBJ_H
typedef struct
{
   int  sng;
   int  sdg;
   int  sdc;
   int  og;
   int  rt;
   int  pop;
}  Gstats;

typedef struct
{
   double area;
   double edges;
   double clusters;
   double pop;
   double xmean;
   double ymean;
   double rad;
   double average_slope;
   double mean_cluster_size;
   double percent_urban;
}  stats_info;

typedef struct
{
  double sng;
  double sdg;
  double sdc;
  double og;
  double rt;
  double pop;
  double area;
  double edges;
  double clusters;
  double xmean;
  double ymean;
  double rad;
  double slope;
  double mean_cluster_size;
  double diffusion;
  double spread;
  double breed;
  double slope_resistance;
  double road_gravity;
  double percent_urban;
  double percent_road;
  double growth_rate;
  double leesalee;
  double num_growth_pix;
} stats_val_t;

/*
 *
 * INTERFACE FUNCTIONS
 *
 */
void stats_MemoryLog(FILE* fp);
#if 1
void stats_ConcatenateControlFiles();
void stats_ConcatenateStdDevFiles();
void stats_ConcatenateAvgFiles();
#else
void stats_ConcatenateControlFiles(int current_run);
void stats_ConcatenateStdDevFiles(int current_run);
void stats_ConcatenateAvgFiles(int current_run);
#endif
void stats_Dump(char* file, int line);
void stats_Init();
void stats_Analysis(double fmatch);
void stats_Update(int num_growth_pix);
void stats_SetSNG(int val) ;
void stats_SetSDG(int val) ;
void stats_SetOG(int val) ;
void stats_SetRT(int val) ;
void stats_SetPOP(int val) ;

void stats_LogBaseStats (FILE* fp);

double stats_GetPercentUrban() ;

double stats_GetGrowthRate() ;

double stats_GetLeesalee() ;


int stats_GetSNG() ;
int stats_GetSDG() ;
int stats_GetOG() ;
int stats_GetRT() ;
int stats_GetPOP() ;

void stats_SetArea(int val) ;
void stats_SetEdges(int val) ;
void stats_SetClusters(int val) ;
void stats_SetPop(int val) ;
void stats_SetXmean(double val) ;
void stats_SetYmean(double val) ;
void stats_SetRad(double val) ;
void stats_SetAvgSlope(double val) ;
void stats_SetMeanClusterSize(double val) ;

int stats_GetArea() ;
int stats_GetEdges() ;
int stats_GetClusters() ;
int stats_GetPop() ;
double stats_GetXmean() ;
double stats_GetYmean() ;
double stats_GetRad() ;
double stats_GetAvgSlope() ;
double stats_GetMeanClusterSize() ;
void stats_InitUrbanizationAttempts();
void stats_LogUrbanizationAttempts(FILE* fp);
void stats_IncrementUrbanSuccess();
void stats_IncrementZFailure();
void stats_IncrementDeltaFailure();
void stats_IncrementSlopeFailure();
void stats_CreateControlFile (char *filename);
void stats_IncrementEcludedFailure();
void stats_CreateStatsValFile (char *filename);
#endif
