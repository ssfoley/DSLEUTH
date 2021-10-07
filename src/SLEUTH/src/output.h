#ifndef OUTPUT_H
#define OUTPUT_H
#include "coeff_obj.h"

#ifdef OUTPUT_MODULE
  /* stuff visable only to the output module */
char output_h_sccs_id[] = "@(#)output.h	1.259	12/4/00";

#if 0
  struct colortable* out_annual_colortable_p;
  struct colortable* out_urban_colortable_p;
  struct colortable* out_urban2_colortable_p;
  struct colortable* out_grayscale_colortable_p;
  struct colortable* out_landuse_colortable_p;
  struct colortable* out_non_landuse_colortable_p;

  /* controls for the date string on the output GIF */
  #define DATE_X 1
  #define DATE_Y igrid_GetNumRows() - 16
#endif
  
#endif

#if 0
extern struct colortable* out_annual_colortable_p;
extern struct colortable* out_urban_colortable_p;
extern struct colortable* out_urban2_colortable_p;
extern struct colortable* out_grayscale_colortable_p;
extern struct colortable* out_landuse_colortable_p;
extern struct colortable* out_non_landuse_colortable_p;

/* #defines visable to any module including this header file*/

/* max size of a colortable */
#define MAX_COLORS 256
/* types of colortables */
#define GRAYSCALE 0
#define ANNUAL    1
#define URBAN     2
#define URBAN2    3
#define LANDUSE_COLORMAP    4
#define NON_LANDUSE_COLORMAP    5


struct RGB{
  int red;
  int green;
  int blue;
};
struct colortable{
  int size;
  char name[80];
  struct RGB color[MAX_COLORS];
};

/*
 *
 * FUNCTION PROTOTYPES
 *
 */
struct colortable* out_create_colortable(int);
void out_write_gif( GRID_P, struct colortable*, char*, char*, int);
#endif
void
  out_echotruth (igrid_info* input_grid);
void out_write_restart_data(char*,int,int,int,int,int,int,int);
void
  out_write_calibrate_file (int stop_date,
                            int num_monte_carlo,
                            long random_seed,
                            coeff_int_info* step_coeff,
                            coeff_int_info* start_coeff,
                            coeff_int_info* stop_coeff);
#if 0
void
  out_write_grow_log (char* filename,
                      float percent_road,
                      coeff_val_info * current_coefficient,
                      int ticktock,
                      double growth_rate,
                      double percent_urban,
                      float average_slope,
                      int num_growth_pix,
                      int index,
                      float leesalee,
                      stats_info * stats,
                      Gstats * gstats,
                      igrid_info * input_grid);
#endif
void
  out_write_control_stats(char* filename,
                          double sum,
                          float compare,
                          float pop_r2,
                          float edge_r2,
                          float cluster_r2,
                          float mean_cluster_size_r2,
                          float leesal,
                          float average_slope_r2,
                          float pct_urban_r2,
                          float xmu_r2,
                          float ymu_r2,
                          float sdist_r2,
                          float value,
                          coeff_val_info* saved_coefficient);

void out_dump(char* filename,
              void* ptr,
              int count);

void
  out_write_param_log (char* filename,
                       coeff_val_info * current_coefficient,
                       int index,
                       igrid_info * input_grid);

void out_write_avg_dev(char* filename,      /* IN     */
                       double* ptr,         /* IN     */
                       int row_dim,         /* IN     */
                       int col_dim,         /* IN     */
                       int row_count,       /* IN     */
                       int col_count);      /* IN     */
void
  out_dump_debug (char *var_name,
                  void *var_ptr,
                  char* calling_func,
                  int line,
                  int count);

void
out_banner(FILE* fp);
void
out_center_text(FILE* fp,
                char* source,
                char* text,
                char* destination,
                int left_offset);

#endif
