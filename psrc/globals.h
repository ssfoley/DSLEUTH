#include <limits.h>
#ifndef GLOBALS_H
#define GLOBALS_H

#include "ugm_defines.h"
#include <stdio.h>
#ifdef MPI
  #include <mpi.h>
#endif

#ifdef MAIN_MODULE
  #ifndef GLOBALS_H_SCCS_ID
   #define GLOBALS_H_SCCS_ID
   char globals_h_sccs_id[] = "@(#)globals.h	1.259	12/4/00";
  #endif
#endif


#ifdef MAIN_MODULE
  /* stuff visible only to the main module */

  int glb_i;
  int glb_mype;
  int glb_npes;
  char msg_buf[300];
  char glb_filename[300];
  FILE* glb_fp;
  int glb_token;
#ifdef MPI
  MPI_Status glb_mpi_status;
#endif

  
#endif

extern int glb_i;
extern int glb_mype;
extern int glb_npes;
extern char msg_buf[300];
extern char glb_filename[300];
extern FILE* glb_fp;
extern int glb_token;
#ifdef MPI
extern MPI_Status glb_mpi_status;
#endif




#endif


