#define MAIN_MODULE
#define MAIN
#ifdef MPI
#include <mpi.h>
#endif
#define CATCH_SIGNALS
#ifdef CATCH_SIGNALS
#include <signal.h>
#endif
#include <omp.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "coeff_obj.h"
#include "igrid_obj.h"
#include "landclass_obj.h"
#include "globals.h"
#include "output.h"
#include "utilities.h"
#include "random.h"
#include "driver.h"
#include "input.h"
#include "scenario_obj.h"
#include "proc_obj.h"
#include "timer_obj.h"
#include "landclass_obj.h"
#include "pgrid_obj.h"
#include "color_obj.h"
#include "memory_obj.h"
#include "color_obj.h"
#include "stats_obj.h"
#include "transition_obj.h"
#include "ugm_macros.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void print_usage (char *binary);
#ifdef CATCH_SIGNALS
void catch (int signo);
#endif

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static int tracer;

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char main_c_sccs_id[] = "@(#)main.c	1.629	12/4/00";



/******************************************************************************
*******************************************************************************
** FUNCTION NAME: main
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  main (int argc, char *argv[])
{
  char func[] = "main";
  char fname[MAX_FILENAME_LEN];
  char command[5 * MAX_FILENAME_LEN];
  int restart_run = 0;
  RANDOM_SEED_TYPE random_seed;
  int diffusion_coeff;
  int breed_coeff;
  int spread_coeff;
  int slope_resistance;
  int road_gravity;
  int restart_diffusion;
  int restart_breed;
  int restart_spread;
  int restart_slope_resistance;
  int restart_road_gravity;
  time_t tp;
  char processing_str[MAX_FILENAME_LEN];
  int i;
#ifdef CATCH_SIGNALS
  // 信号操作
  // 与业务逻辑无关
  struct sigaction act, oact;

  tracer = 1;

  act.sa_handler = catch;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  sigaction (SIGFPE, &act, &oact);
  sigaction (SIGINT, &act, &oact);
  sigaction (SIGSEGV, &act, &oact);
  sigaction (SIGBUS, &act, &oact);
#endif

#ifdef MPI
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &glb_mype);
  MPI_Comm_size (MPI_COMM_WORLD, &glb_npes);
#else
  glb_mype = 0;
  glb_npes = 1;
#endif

  //初始化计时器
  timer_Init ();
  timer_Start (TOTAL_TIME);

  //初始化调用栈顶部索引
  glb_call_stack_index = -1;
  //函数入栈
  FUNC_INIT;

  /*
   *
   * PARSE COMMAND LINE
   *
   */
  if (argc != 3)
  {
    //参数不足
    print_usage (argv[0]);
  }
  if ((strcmp (argv[1], "predict")) &&
      (strcmp (argv[1], "restart")) &&
      (strcmp (argv[1], "test")) &&
      (strcmp (argv[1], "calibrate")))
  {
    //参数格式不正确
    print_usage (argv[0]);
  }
  if (strcmp (argv[1], "predict") == 0)
  {
    //初始化proc_obj.c中的静态全局变量type_of_processing
    proc_SetProcessingType (PREDICTING);
    strcpy (processing_str, "PREDICTING");
  }
  if (strcmp (argv[1], "restart") == 0)
  {
    //初始化proc_obj.c中的静态全局变量type_of_processing
    proc_SetProcessingType (CALIBRATING);
    //初始化proc_obj.c中的静态全局变量restart_flag
    proc_SetRestartFlag (TRUE);
    strcpy (processing_str, "restart CALIBRATING");
  }
  if (strcmp (argv[1], "test") == 0)
  {
    //初始化proc_obj.c中的静态全局变量type_of_processing
    proc_SetProcessingType (TESTING);
    strcpy (processing_str, "TESTING");
  }
  if (strcmp (argv[1], "calibrate") == 0)
  {
    //初始化proc_obj.c中的静态全局变量type_of_processing
    proc_SetProcessingType (CALIBRATING);
    strcpy (processing_str, "CALIBRATING");
  }
  //根据场景文件初始化scenario_obj.c中的变量，该函数将进行入栈出栈操作
  //同时，将wgrid_obj.c中的num_working_grids初始化为scenario_obj.c里结构体scenario中的num_working_grids
  scen_init (argv[2]);

  /*
   *
   * SET SOME VARIABLES
   *
   */
  random_seed = scen_GetRandomSeed ();

/*
 * void landclassSetGrayscale (int index, int val);
 * void landclassSetColor (int index, int val);
 * void landclassSetType (int index, char* string);
 * void landclassSetName (int index, char* string);
 * void landclassSetNumClasses (int val);
 * int scen_GetNumLanduseClasses ();
 * char* scen_GetLanduseClassName (int);
 * char* scen_GetLanduseClassType (int);
 * int scen_GetLanduseClassColor (int);
 * int scen_GetLanduseClassGrayscale (int i);
 * 
 */
  //将landclass_obj.c中的静态全局变量num_landclasses初始化为scenario_obj.c里结构体scenario中的num_landuse_classes
  landclassSetNumClasses (scen_GetNumLanduseClasses ());
  for (i = 0; i < scen_GetNumLanduseClasses (); i++)
  {
    //将landclass_obj.c中的静态全局数组的第i个Classes的num属性设置为scenario_obj.c里结构体scenario中的数组landuse_class中的第i个landuse_class_info的属性grayscale
    landclassSetGrayscale (i, scen_GetLanduseClassGrayscale (i));
    //将landclass_obj.c中的静态全局数组的第i个Classes的name属性设置为scenario_obj.c里结构体scenario中的数组landuse_class中的第i个landuse_class_info的属性name
    landclassSetName (i, scen_GetLanduseClassName (i));
    //将landclass_obj.c中的静态全局数组的第i个Classes的id属性设置为scenario_obj.c里结构体scenario中的数组landuse_class中的第i个landuse_class_info的属性type
    //同时，将id的值与字符串EXC比较以设置trans属性
    landclassSetType (i, scen_GetLanduseClassType (i));
    //将scenario_obj.c里结构体scenario中的数组landuse_class中的第i个landuse_class_info的属性color分解为红、绿、蓝三色存储在landclass_obj.c中的静态全局数组的第i个Classes的属性里
    landclassSetColor (i, scen_GetLanduseClassColor (i));
  }

  /*
   *
   * SET UP COEFFICIENTS
   *
   */
  if (strcmp (argv[1], "restart") == 0)
  {
    if (scen_GetLogFlag ())
    {
      scen_Append2Log ();
      if (scen_GetLogFP ())
      {
        fprintf (scen_GetLogFP (), "%s %u Reading restart file\n",
                 __FILE__, __LINE__);
      }
      scen_CloseLog ();
    }
    inp_read_restart_file (&restart_diffusion,
                           &restart_breed,
                           &restart_spread,
                           &restart_slope_resistance,
                           &restart_road_gravity,
                           &random_seed,
                           &restart_run);
    proc_SetCurrentRun (restart_run);

  }
  else
  {
    //将proc_obj.c中的静态全局变量current_run设置为0
    proc_SetCurrentRun (0);
  }
  //将coeff_obj.c中的静态全局结构体start_coeff的diffusion属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体start里的diffusion属性
  coeff_SetStartDiffusion (scen_GetCoeffDiffusionStart ());
  //将coeff_obj.c中的静态全局结构体start_coeff的spread属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体start里的spread属性
  coeff_SetStartSpread (scen_GetCoeffSpreadStart ());
  //将coeff_obj.c中的静态全局结构体start_coeff的breed属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体start里的breed属性
  coeff_SetStartBreed (scen_GetCoeffBreedStart ());
  //将coeff_obj.c中的静态全局结构体start_coeff的slope_resistance属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体start里的slope_resistance属性
  coeff_SetStartSlopeResist (scen_GetCoeffSlopeResistStart ());
  //将coeff_obj.c中的静态全局结构体start_coeff的road_gravity属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体start里的road_gravity属性
  coeff_SetStartRoadGravity (scen_GetCoeffRoadGravityStart ());

  //将coeff_obj.c中的静态全局结构体stop_coeff的diffusion属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体stop里的diffusion属性
  coeff_SetStopDiffusion (scen_GetCoeffDiffusionStop ());
  //将coeff_obj.c中的静态全局结构体stop_coeff的spread属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体stop里的spread属性
  coeff_SetStopSpread (scen_GetCoeffSpreadStop ());
  //将coeff_obj.c中的静态全局结构体stop_coeff的breed属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体stop里的breed属性
  coeff_SetStopBreed (scen_GetCoeffBreedStop ());
  //将coeff_obj.c中的静态全局结构体stop_coeff的slope_resistance属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体stop里的slope_resistance属性
  coeff_SetStopSlopeResist (scen_GetCoeffSlopeResistStop ());
  //将coeff_obj.c中的静态全局结构体stop_coeff的road_gravity属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体stop里的road_gravity属性
  coeff_SetStopRoadGravity (scen_GetCoeffRoadGravityStop ());

  //将coeff_obj.c中的静态全局结构体step_coeff的diffusion属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体step里的diffusion属性
  coeff_SetStepDiffusion (scen_GetCoeffDiffusionStep ());
  //将coeff_obj.c中的静态全局结构体step_coeff的spread属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体step里的spread属性
  coeff_SetStepSpread (scen_GetCoeffSpreadStep ());
  //将coeff_obj.c中的静态全局结构体step_coeff的breed属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体step里的breed属性
  coeff_SetStepBreed (scen_GetCoeffBreedStep ());
  //将coeff_obj.c中的静态全局结构体step_coeff的slope_resistance属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体step里的slope_resistance属性
  coeff_SetStepSlopeResist (scen_GetCoeffSlopeResistStep ());
  //将coeff_obj.c中的静态全局结构体step_coeff的road_gravity属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体step里的road_gravity属性
  coeff_SetStepRoadGravity (scen_GetCoeffRoadGravityStep ());

  //将coeff_obj.c中的静态全局结构体best_fit_coeff的diffusion属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体best_fit里的diffusion属性
  coeff_SetBestFitDiffusion (scen_GetCoeffDiffusionBestFit ());
  //将coeff_obj.c中的静态全局结构体best_fit_coeff的spread属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体best_fit里的spread属性
  coeff_SetBestFitSpread (scen_GetCoeffSpreadBestFit ());
  //将coeff_obj.c中的静态全局结构体best_fit_coeff的breed属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体best_fit里的breed属性
  coeff_SetBestFitBreed (scen_GetCoeffBreedBestFit ());
  //将coeff_obj.c中的静态全局结构体best_fit_coeff的slope_resistance属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体best_fit里的slope_resistance属性
  coeff_SetBestFitSlopeResist (scen_GetCoeffSlopeResistBestFit ());
  //将coeff_obj.c中的静态全局结构体best_fit_coeff的road_gravity属性设置为scenario_obj.c中的静态全局结构体scenario中的结构体best_fit里的road_gravity属性
  coeff_SetBestFitRoadGravity (scen_GetCoeffRoadGravityBestFit ());

  /*
   *
   * INITIALIZE IGRID
   *
   */
   //根据场景文件设置igrid_bj.c中的静态全局结构体igrid里的location属性
   //根据输入文件对igrid进行初始化
  igrid_init ();

  /*
   *
   * PRINT BANNER
   *
   */
  if (scen_GetEchoFlag ())
  {
    out_banner (stdout);
  }
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    out_banner (scen_GetLogFP ());
    scen_CloseLog ();
  }

  /*
   *
   * LOG SOME STUFF
   *
   */
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    time (&tp);
    fprintf (scen_GetLogFP (), "DATE OF RUN: %s\n",
             asctime (localtime (&tp)));
    fprintf (scen_GetLogFP (), "USER: %s\n", getenv ("USER"));
    fprintf (scen_GetLogFP (), "HOST: %s\n", getenv ("HOST"));
    fprintf (scen_GetLogFP (), "HOSTTYPE: %s\n", getenv ("HOSTTYPE"));
    fprintf (scen_GetLogFP (), "OSTYPE: %s\n", getenv ("OSTYPE"));
    fprintf (scen_GetLogFP (), "Type of architecture: %u bit\n\n",
             BYTES_PER_WORD*8);
    fprintf (scen_GetLogFP (), "Number of CPUs %u \n\n",
             glb_npes);
    fprintf (scen_GetLogFP (), "PWD: %s\n", getenv ("PWD"));
    fprintf (scen_GetLogFP (), "Scenario File: %s\n",
             scen_GetScenarioFilename ());
    fprintf (scen_GetLogFP (), "Type of Processing: %s\n",
             processing_str);
    fprintf (scen_GetLogFP (), "\n\n");

    scen_echo (scen_GetLogFP ());
    coeff_LogStart (scen_GetLogFP ());
    coeff_LogStop (scen_GetLogFP ());
    coeff_LogStep (scen_GetLogFP ());
    coeff_LogBestFit (scen_GetLogFP ());
    scen_CloseLog ();
    tracer = 2;
  }

  /*
   *
   * SET UP FLAT MEMORY
   *
   */
#ifdef MPI
  MPI_Barrier (MPI_COMM_WORLD);
#endif
  //为igrid，pgrid，wgrid分配空间
  mem_Init ();
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    mem_LogPartition (scen_GetLogFP ());
    mem_CheckMemory (scen_GetLogFP (), __FILE__, func, __LINE__);
    scen_CloseLog ();
  }
#ifdef MPI
  MPI_Barrier (MPI_COMM_WORLD);
#endif

  /*
   *
   * INITIALIZE LANDUSE
   *
   */
  if (scen_GetDoingLanduseFlag ())
  {
    //根据场景文件中LANDUSE_CLASS的最大pix设置landclass_obj.c里的max_landclass_num
    //根据是否在场景文件中有flag属性建立小规模LANDUSE_CLASS集合class_indices，并num_reduced_classes设置为该小集合中LANDUSE_CLASS的数目
    //将flag为URB的LANDUSE_CLASS的pix赋给urban_code
    //将ugm_read设为真
    landclass_Init ();
    if (scen_GetLogLandclassSummaryFlag ())
    {
      if (scen_GetLogFlag ())
      {
        scen_Append2Log ();
        landclass_LogIt (scen_GetLogFP ());
        scen_CloseLog ();
      }
    }
  }

  /*
   *
   * INITIALIZE COLORTABLES
   *
   */
  color_Init ();

  /*
   *
   * WRITE MEMORY MAPS
   *
   */
  if (scen_GetLogMemoryMapFlag ())
  {
    color_MemoryLog (mem_GetLogFP ());
    coeff_MemoryLog (mem_GetLogFP ());
    timer_MemoryLog (mem_GetLogFP ());
    igrid_MemoryLog (mem_GetLogFP ());
    pgrid_MemoryLog (mem_GetLogFP ());
    stats_MemoryLog (mem_GetLogFP ());
    mem_MemoryLog (mem_GetLogFP ());
    proc_MemoryLog (mem_GetLogFP ());
    landclass_MemoryLog (mem_GetLogFP ());
    scen_MemoryLog (mem_GetLogFP ());
    trans_MemoryLog (mem_GetLogFP ());
    mem_CloseLog ();
  }

  /*
   *
   * READ INPUT DATA FILES
   *
   */
  igrid_ReadFiles ();
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    igrid_ValidateGrids (scen_GetLogFP ());
    scen_CloseLog ();
  }
  else
  {
    igrid_ValidateGrids (NULL);
  }
  igrid_NormalizeRoads ();
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    igrid_LogIt (scen_GetLogFP ());
    igrid_VerifyInputs (scen_GetLogFP ());
    scen_CloseLog ();
  }
  else
  {
    igrid_VerifyInputs (NULL);
  }

  /*
   *
   * INITIALIZE THE PGRID GRIDS
   *
   */
   //将memory_obj.c中分配的6个pgrid对应到pgrid_obj.c中的6个变量
  pgrid_Init ();

  if (scen_GetLogFlag ())
  {
    if (scen_GetLogColortablesFlag ())
    {
      scen_Append2Log ();
      color_LogIt (scen_GetLogFP ());
      scen_CloseLog ();
    }
  }

  /*
   *
   * COUNT THE NUMBER OF RUNS
   *
   */
  proc_SetTotalRuns ();
  if (scen_GetLogFlag ())
  {
    if (proc_GetProcessingType () == CALIBRATING)
    {
      scen_Append2Log ();
      fprintf (scen_GetLogFP (), "%s %u Total Number of Runs = %u\n",
               __FILE__, __LINE__, proc_GetTotalRuns ());
      scen_CloseLog ();
    }
  }

  //设置最大迭代值
  proc_SetLastMonteCarlo (scen_GetMonteCarloIterations () - 1);
  /*
   *
   * COMPUTE THE TRANSITION MATRIX
   *
   */
  if (scen_GetDoingLanduseFlag ())
  {
    trans_Init ();
    if (scen_GetLogFlag ())
    {
      if (scen_GetLogTransitionMatrixFlag ())
      {
        scen_Append2Log ();
        trans_LogTransition (scen_GetLogFP ());
        scen_CloseLog ();
      }
    }
  }
  /*
   *
   * COMPUTE THE BASE STATISTICS AGAINST WHICH CALIBRATION WILL TAKE PLACE
   *
   */
  stats_Init ();
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogBaseStatsFlag ())
    {
      scen_Append2Log ();
      stats_LogBaseStats (scen_GetLogFP ());
      scen_CloseLog ();
    }
  }
  if (scen_GetLogFlag ())
  {
    if (scen_GetLogDebugFlag ())
    {
      scen_Append2Log ();
      igrid_Debug (scen_GetLogFP (), __FILE__, __LINE__);
      scen_CloseLog ();
    }
  }

  proc_SetNumRunsExecThisCPU (0);
  if (proc_GetCurrentRun () == 0 && glb_mype == 0)
  {
    if (proc_GetProcessingType () != PREDICTING)
    {
      sprintf (fname, "%scontrol_stats.log", scen_GetOutputDir ());
      stats_CreateControlFile (fname);
    }
    if (scen_GetWriteStdDevFileFlag ())
    {
      sprintf (fname, "%sstd_dev.log", scen_GetOutputDir ());
      stats_CreateStatsValFile (fname);
    }
    if (scen_GetWriteAvgFileFlag ())
    {
      sprintf (fname, "%savg.log", scen_GetOutputDir ());
      stats_CreateStatsValFile (fname);
    }
  }

  coeff_CreateCoeffFile ();

#ifdef MPI
  MPI_Barrier (MPI_COMM_WORLD);
#endif

  if (proc_GetProcessingType () == PREDICTING)
  {
    /*
     *
     * PREDICTION RUNS
     *
     */
    proc_SetStopYear (scen_GetPredictionStopDate ());
    InitRandom (scen_GetRandomSeed ());
    coeff_SetCurrentDiffusion ((double) coeff_GetBestFitDiffusion ());
    coeff_SetCurrentSpread ((double) coeff_GetBestFitSpread ());
    coeff_SetCurrentBreed ((double) coeff_GetBestFitBreed ());
    coeff_SetCurrentSlopeResist ((double) coeff_GetBestFitSlopeResist ());
    coeff_SetCurrentRoadGravity ((double) coeff_GetBestFitRoadGravity ());
    if (glb_mype == 0)
    {
      drv_driver ();
      proc_IncrementNumRunsExecThisCPU ();
    }
    if (scen_GetLogFlag ())
    {
      if (scen_GetLogTimingsFlag () > 1)
      {
        scen_Append2Log ();
        timer_LogIt (scen_GetLogFP ());
        scen_CloseLog ();
      }
    }
  }
  else
  {
    /*
     *
     * CALIBRATION AND TEST RUNS
     *
     */
    proc_SetStopYear (igrid_GetUrbanYear (igrid_GetUrbanCount () - 1));

    for (diffusion_coeff = coeff_GetStartDiffusion ();
         diffusion_coeff <= coeff_GetStopDiffusion ();
         diffusion_coeff += coeff_GetStepDiffusion ())
    {
      for (breed_coeff = coeff_GetStartBreed ();
           breed_coeff <= coeff_GetStopBreed ();
           breed_coeff += coeff_GetStepBreed ())
      {
        for (spread_coeff = coeff_GetStartSpread ();
             spread_coeff <= coeff_GetStopSpread ();
             spread_coeff += coeff_GetStepSpread ())
        {
          for (slope_resistance = coeff_GetStartSlopeResist ();
               slope_resistance <= coeff_GetStopSlopeResist ();
               slope_resistance += coeff_GetStepSlopeResist ())
          {
            //#pragma omp parallel for default(shared)
            for (road_gravity = coeff_GetStartRoadGravity ();
                 road_gravity <= coeff_GetStopRoadGravity ();
                 road_gravity += coeff_GetStepRoadGravity ())
            {
              
              sprintf (fname, "%s%s%u", scen_GetOutputDir (),
                       RESTART_FILE, glb_mype);
              out_write_restart_data (fname,
                                        diffusion_coeff,
                                        breed_coeff,
                                        spread_coeff,
                                        slope_resistance,
                                        road_gravity,
                                        scen_GetRandomSeed (),
                                        restart_run);

              InitRandom (scen_GetRandomSeed ());

              restart_run++;

              coeff_SetCurrentDiffusion ((double) diffusion_coeff);
              coeff_SetCurrentSpread ((double) spread_coeff);
              coeff_SetCurrentBreed ((double) breed_coeff);
              coeff_SetCurrentSlopeResist ((double) slope_resistance);
              coeff_SetCurrentRoadGravity ((double) road_gravity);


#ifdef MPI
              if (proc_GetCurrentRun () % glb_npes == glb_mype)
              {
                drv_driver ();
                proc_IncrementNumRunsExecThisCPU ();
                if (scen_GetLogFlag ())
                {
                  if (scen_GetLogTimingsFlag () > 1)
                  {
                    scen_Append2Log ();
                    timer_LogIt (scen_GetLogFP ());
                    scen_CloseLog ();
                  }
                }
              }
#else
              drv_driver ();
              #pragma omp critical
              {
                proc_IncrementNumRunsExecThisCPU ();
              }
              if (scen_GetLogFlag ())
              {
                if (scen_GetLogTimingsFlag () > 1)
                {
                  scen_Append2Log ();
                  timer_LogIt (scen_GetLogFP ());
                  scen_CloseLog ();
                }
              }
#endif
              #pragma omp critical
              {
                proc_IncrementCurrentRun ();
              }
              if (proc_GetProcessingType () == TESTING)
              {
                stats_ConcatenateControlFiles ();
                if (scen_GetWriteCoeffFileFlag ())
                {
                  coeff_ConcatenateFiles ();
                }
                if (scen_GetWriteAvgFileFlag ())
                {
                  stats_ConcatenateAvgFiles ();
                }
                if (scen_GetWriteStdDevFileFlag ())
                {
                  stats_ConcatenateStdDevFiles ();
                }

                timer_Stop (TOTAL_TIME);
                if (scen_GetLogFlag ())
                {
                  scen_Append2Log ();
                  if (scen_GetLogTimingsFlag () > 0)
                  {
                    timer_LogIt (scen_GetLogFP ());
                  }
                  mem_LogMinFreeWGrids (scen_GetLogFP ());
                  scen_CloseLog ();
                }
                EXIT (0);
              }
            }
          }
        }
      }
    }
  }

#ifdef MPI
  MPI_Barrier (MPI_COMM_WORLD);
#endif
  if (glb_mype == 0)
  {
    if (scen_GetWriteCoeffFileFlag ())
    {
      coeff_ConcatenateFiles ();
    }
    if (scen_GetWriteAvgFileFlag ())
    {
      stats_ConcatenateAvgFiles ();
    }
    if (scen_GetWriteStdDevFileFlag ())
    {
      stats_ConcatenateStdDevFiles ();
    }
    if (proc_GetProcessingType () != PREDICTING)
    {
      stats_ConcatenateControlFiles ();
    }
  }

  if (scen_GetPostprocessingFlag ())
  {
#ifdef MPI
    MPI_Barrier (MPI_COMM_WORLD);
#endif
    if (glb_mype == 0)
    {
      if (strlen (scen_GetWhirlgifBinary ()) > 0)
      {
        if (scen_GetViewDeltatronAgingFlag ())
        {
          sprintf (command,
            "%s -time 100 -o %sanimated_deltatron.gif %sdeltatron_*.gif",
                   scen_GetWhirlgifBinary (), scen_GetOutputDir (),
                   scen_GetOutputDir ());
          system (command);
        }
        if (scen_GetViewGrowthTypesFlag ())
        {
          sprintf (command,
                   "%s -time 100 -o %sanimated_z_growth.gif %sz_growth_types_*.gif",
                   scen_GetWhirlgifBinary (), scen_GetOutputDir (),
                   scen_GetOutputDir ());
          system (command);
        }
        if (proc_GetProcessingType () != CALIBRATING)
        {
          if (scen_GetDoingLanduseFlag ())
          {
            sprintf (command,
                     "%s -time 100 -o %sanimated_land_n_urban.gif %s*_land_n_urban*.gif",
                     scen_GetWhirlgifBinary (), scen_GetOutputDir (),
                     scen_GetOutputDir ());
            system (command);
          }
          else
          {
            sprintf (command,
                  "%s -time 100 -o %sanimated_urban.gif %s*_urban_*.gif",
                     scen_GetWhirlgifBinary (),
                     scen_GetOutputDir (), scen_GetOutputDir ());
            system (command);
          }
        }
      }
    }
  }
#ifdef MPI
  MPI_Finalize ();
#endif
  timer_Stop (TOTAL_TIME);

  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    if (scen_GetLogTimingsFlag () > 0)
    {
      timer_LogIt (scen_GetLogFP ());
    }
    mem_LogMinFreeWGrids (scen_GetLogFP ());
    scen_CloseLog ();
  }
  return (0);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: print_usage
** PURPOSE:       help the user
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  print_usage (char *binary)
{
  printf ("Usage:\n");
  printf ("%s <mode> <scenario file>\n", binary);
  printf ("Allowable modes are:\n");
  printf ("  calibrate\n");
  printf ("  restart\n");
  printf ("  test\n");
  printf ("  predict\n");
  EXIT (1);
}
#ifdef CATCH_SIGNALS

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: catch
** PURPOSE:       catch signals
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  catch (int signo)
{
  int i;
  if (tracer < 2)
  {
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
            "Please make sure the following env variables are defined");
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
            "USER -- set to your username");
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
            "HOST -- set to your machines's name = uname -n");
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
          "HOSTTYPE -- set to your machines's type such as Sparc, Cray");
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
          "OSTYPE -- set to your machines's OS type such as Solaris2.7");
    printf ("%s %u pe: %u %s\n", __FILE__, __LINE__, glb_mype,
            "PWD -- set to your current working directory");
  }
  if (signo == SIGBUS)
  {
    printf ("%s %u caught signo SIGBUS : bus error\n",
            __FILE__, __LINE__);
    printf ("Currently executing function: %s\n",
            glb_call_stack[glb_call_stack_index]);
    for (i = glb_call_stack_index; i >= 0; i--)
    {
      printf ("glb_call_stack[%d]= %s\n", i, glb_call_stack[i]);
    }
    EXIT (1);
  }
  if (signo == SIGSEGV)
  {
    printf ("%s %u pe: %u caught signo SIGSEGV : Invalid storage access\n",
            __FILE__, __LINE__, glb_mype);
    printf ("Currently executing function: %s\n",
            glb_call_stack[glb_call_stack_index]);
    for (i = glb_call_stack_index; i >= 0; i--)
    {
      printf ("glb_call_stack[%d]= %s\n", i, glb_call_stack[i]);
    }
    EXIT (1);
  }
  if (signo == SIGINT)
  {
    printf ("%s %u caught signo SIGINT : Interrupt\n",
            __FILE__, __LINE__);
    printf ("Currently executing function: %s\n",
            glb_call_stack[glb_call_stack_index]);
    for (i = glb_call_stack_index; i >= 0; i--)
    {
      printf ("glb_call_stack[%d]= %s\n", i, glb_call_stack[i]);
    }
    EXIT (1);
  }
  if (signo == SIGFPE)
  {
    printf ("%s %u caught signo SIGFPE : Floating-point exception\n",
            __FILE__, __LINE__);
    printf ("Currently executing function: %s\n",
            glb_call_stack[glb_call_stack_index]);
    for (i = glb_call_stack_index; i >= 0; i--)
    {
      printf ("glb_call_stack[%d]= %s\n", i, glb_call_stack[i]);
    }
    EXIT (1);
  }
  printf ("%s %u caught signo %d\n", __FILE__, __LINE__, signo);
  EXIT (1);
}
#endif
