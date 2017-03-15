#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <omp.h>
#include "globals.h"
#include "igrid_obj.h"
#include "pgrid_obj.h"
#include "wgrid_obj.h"
#include "scenario_obj.h"
#include "ugm_typedefs.h"
#include "ugm_macros.h"
#include "memory_obj.h"

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                                 MACROS                                    **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
#define MEM_ARRAY_SIZE 50
#define INVALID_VAL 0xAAAAAAAA

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                               SCCS ID                                     **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
char memory_obj_c_sccs_id[] = "@(#)memory_obj.c	1.84	12/4/00";

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                      STATIC MEMORY FOR THIS OBJECT                        **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
typedef struct
{
  char previous_owner[MAX_FILENAME_LEN];
  char current_owner[MAX_FILENAME_LEN];
  char released_by[MAX_FILENAME_LEN];
  BOOLEAN free;
  GRID_P ptr;
}
mem_track_info;
static int nrows;
static int ncols;
static int total_pixels;
static PIXEL invalid_val;
static int igrid_free[MEM_ARRAY_SIZE];
static int igrid_free_tos;
static mem_track_info igrid_array[MEM_ARRAY_SIZE];
//将所有线程共享一个pgrid栈改为每个线程单独拥有一个栈
static int pgrid_free[NUM_THREADS][MEM_ARRAY_SIZE];
static int pgrid_free_tos[NUM_THREADS];
static mem_track_info pgrid_array[NUM_THREADS][MEM_ARRAY_SIZE];
static int wgrid_free[NUM_THREADS][MEM_ARRAY_SIZE];
static int wgrid_free_tos[NUM_THREADS];
static int min_wgrid_free_tos[NUM_THREADS];
static mem_track_info wgrid_array[NUM_THREADS][MEM_ARRAY_SIZE];
static PIXEL *mem_check_array[MEM_ARRAY_SIZE];
static int mem_check_count;
static int mem_check_size;
static int igrid_size;
static int pgrid_size;
static int wgrid_size;
static int bytes_p_grid;
static int bytes_p_grid_rounded2wordboundary;
static int bytes_p_packed_grid;
static int bytes_p_packed_grid_rounded2wordboundary;
static size_t bytes2allocate;
static void *mem_ptr;
static int igrid_count;
static int pgrid_count;
static int wgrid_count;
static FILE *memlog_fp;
static char mem_log_filename[MAX_FILENAME_LEN];

/*****************************************************************************\
*******************************************************************************
**                                                                           **
**                        STATIC FUNCTION PROTOTYPES                         **
**                                                                           **
*******************************************************************************
\*****************************************************************************/
static void mem_CheckCheckArray ();
static void mem_partition ();
static void mem_allocate ();
static void mem_igrid_push (int i);
static int mem_igrid_pop ();
static void mem_pgrid_push (int i, int j);
static int mem_pgrid_pop (int i);
static void mem_wgrid_push (int i, int j);
static int mem_wgrid_pop (int i);
static void mem_InvalidateGrid (GRID_P ptr);
static void mem_CheckInvalidateGrid (GRID_P ptr);
static void mem_InvalidateCheckArray ();
void mem_Init ();
GRID_P mem_GetIGridPtr (char *owner);
GRID_P mem_GetPGridPtr (char *owner, int i);
GRID_P mem_GetWGridPtr (char *module, char *who, int line);
GRID_P mem_GetWGridFree (char *module, char *who, int line, GRID_P ptr);


/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_MemoryLog
** PURPOSE:       log memory map to FILE* fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_MemoryLog (FILE * fp)
{
  LOG_MEM (fp, &nrows, sizeof (int), 1);
  LOG_MEM (fp, &ncols, sizeof (int), 1);
  LOG_MEM (fp, &total_pixels, sizeof (int), 1);
  LOG_MEM (fp, &invalid_val, sizeof (PIXEL), 1);

  LOG_MEM (fp, &igrid_free[0], sizeof (int), MEM_ARRAY_SIZE);
  LOG_MEM (fp, &igrid_free_tos, sizeof (int), 1);
  LOG_MEM (fp, &igrid_array[0], sizeof (mem_track_info), MEM_ARRAY_SIZE);

  LOG_MEM (fp, &pgrid_free[0], sizeof (int), MEM_ARRAY_SIZE);
  LOG_MEM (fp, &pgrid_free_tos, sizeof (int), 1);
  LOG_MEM (fp, &pgrid_array[0], sizeof (mem_track_info), MEM_ARRAY_SIZE);

  LOG_MEM (fp, &wgrid_free[0], sizeof (int), MEM_ARRAY_SIZE);
  LOG_MEM (fp, &wgrid_free_tos, sizeof (int), 1);
  LOG_MEM (fp, &min_wgrid_free_tos, sizeof (int), 1);
  LOG_MEM (fp, &wgrid_array[0], sizeof (mem_track_info), MEM_ARRAY_SIZE);

  LOG_MEM (fp, &mem_check_count, sizeof (int), 1);
  LOG_MEM (fp, &mem_check_size, sizeof (int), 1);
  LOG_MEM (fp, &igrid_size, sizeof (int), 1);
  LOG_MEM (fp, &pgrid_size, sizeof (int), 1);
  LOG_MEM (fp, &wgrid_size, sizeof (int), 1);

  LOG_MEM (fp, &bytes_p_grid, sizeof (int), 1);
  LOG_MEM (fp, &bytes_p_grid_rounded2wordboundary, sizeof (int), 1);
  LOG_MEM (fp, &bytes_p_packed_grid, sizeof (int), 1);
  LOG_MEM (fp, &bytes_p_packed_grid_rounded2wordboundary, sizeof (int), 1);
  LOG_MEM (fp, &bytes2allocate, sizeof (size_t), 1);
  LOG_MEM (fp, &mem_ptr, sizeof (void *), 1);
  LOG_MEM (fp, &igrid_count, sizeof (int), 1);
  LOG_MEM (fp, &pgrid_count, sizeof (int), 1);
  LOG_MEM (fp, &wgrid_count, sizeof (int), 1);
  LOG_MEM (fp, &memlog_fp, sizeof (FILE *), 1);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetPackedBytesPerGrid
** PURPOSE:       return # bytes per grid rounded to a word boundary
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  mem_GetPackedBytesPerGrid ()
{
  return bytes_p_packed_grid_rounded2wordboundary;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetTotalPixels
** PURPOSE:       return total pixel count in a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  mem_GetTotalPixels ()
{
  return total_pixels;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetIGridPtr
** PURPOSE:       return the pointer to the next igrid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  mem_GetIGridPtr (char *owner)
{
  int index;

  index = mem_igrid_pop ();
  strcpy (igrid_array[index].current_owner, owner);
  return igrid_array[index].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetPGridPtr
** PURPOSE:       return ptr to next pgrid in the specific stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  mem_GetPGridPtr (char *owner, int i)
{
  int index;

  index = mem_pgrid_pop (i);
  strcpy (pgrid_array[i][index].current_owner, owner);
  return pgrid_array[i][index].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetWGridPtr
** PURPOSE:       return ptr to next wgrid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  mem_GetWGridPtr (char *module, char *who, int line)
{
  int index;

  int thread_id = omp_get_thread_num();

  index = mem_wgrid_pop (thread_id);
  strcpy (wgrid_array[thread_id][index].previous_owner, wgrid_array[thread_id][index].current_owner);
  sprintf (wgrid_array[thread_id][index].current_owner,
           "Module: %s Function: %s Line %u", module, who, line);
  return wgrid_array[thread_id][index].ptr;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetWGridFree
** PURPOSE:       
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
GRID_P
  mem_GetWGridFree (char *module, char *who, int line, GRID_P ptr)
{
  char func[] = "mem_GetWGridFree";
  int i;
  int index;
  BOOLEAN match = FALSE;
  int thread_id = omp_get_thread_num();

  for (i = 0; i < wgrid_count; i++)
  {
    if (wgrid_array[thread_id][i].ptr == ptr)
    {
      match = TRUE;
      index = i;
      break;
    }
  }
  if (match == FALSE)
  {
    sprintf (msg_buf, "%s %u look in module %s %s %u\n",
             __FILE__, __LINE__, module, who, line);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (wgrid_array[thread_id][index].free == TRUE)
  {
    sprintf (msg_buf, "wgrid_array[%u][%u].free == TRUE", thread_id, index);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  strcpy (wgrid_array[thread_id][index].current_owner, "");
  sprintf (wgrid_array[thread_id][index].released_by,
           "Module: %s Function: %s Line %u", module, who, line);
  mem_wgrid_push (thread_id, index);
  return NULL;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_Init
** PURPOSE:       initialization routine with number of threads
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_Init ()
{
  char func[] = "mem_Init";
  int check_pixel_count;
  int i;


  sprintf (mem_log_filename, "%smemory.log", scen_GetOutputDir ());

  memlog_fp = NULL;
  if (scen_GetLogMemoryMapFlag ())
  {
    FILE_OPEN (memlog_fp, mem_log_filename, "w");
  }

  invalid_val = INVALID_VAL;
  igrid_free_tos = 0;
  for (i = 0; i < NUM_THREADS; ++i)
  {
    pgrid_free_tos[i] = 0;
  }
  for (i = 0; i < NUM_THREADS; ++i)
  {
    wgrid_free_tos[i] = 0;
  }
  //pgrid_free_tos = 0;
  //wgrid_free_tos = 0;
  nrows = igrid_GetNumRows ();
  ncols = igrid_GetNumCols ();
  total_pixels = nrows * ncols;
  igrid_count = igrid_GetIGridCount ();
  pgrid_count = pgrid_GetPGridCount ();
  wgrid_count = wgrid_GetWGridCount ();

  check_pixel_count = igrid_count + pgrid_count * NUM_THREADS + wgrid_count * NUM_THREADS + 1;

  bytes_p_grid = BYTES_PER_PIXEL * total_pixels;
  bytes_p_grid_rounded2wordboundary =
    ROUND_BYTES_TO_WORD_BNDRY (bytes_p_grid);
#ifdef PACKING
  bytes_p_packed_grid = BYTES_PER_PIXEL_PACKED * total_pixels;
  bytes_p_packed_grid_rounded2wordboundary =
    ROUND_BYTES_TO_WORD_BNDRY (bytes_p_packed_grid);
  bytes2allocate = (size_t)igrid_count * bytes_p_packed_grid_rounded2wordboundary +
    pgrid_count * bytes_p_grid_rounded2wordboundary +
    wgrid_count * bytes_p_grid_rounded2wordboundary +
      check_pixel_count * (size_t)BYTES_PER_PIXEL;
  igrid_size = bytes_p_packed_grid_rounded2wordboundary / BYTES_PER_WORD;
#else
  bytes2allocate = (size_t)igrid_count * bytes_p_grid_rounded2wordboundary +
    pgrid_count * NUM_THREADS * bytes_p_grid_rounded2wordboundary +
    wgrid_count * NUM_THREADS * bytes_p_grid_rounded2wordboundary +
      check_pixel_count * (size_t)BYTES_PER_PIXEL;
  igrid_size = bytes_p_grid_rounded2wordboundary / BYTES_PER_WORD;
#endif
  mem_check_size = 1;
  pgrid_size = bytes_p_grid_rounded2wordboundary / BYTES_PER_WORD;
  wgrid_size = bytes_p_grid_rounded2wordboundary / BYTES_PER_WORD;


  if (memlog_fp)
  {
    fprintf (memlog_fp, "nrows = %u\n", nrows);
    fprintf (memlog_fp, "ncols = %u\n", ncols);
    fprintf (memlog_fp, "total_pixels = %u\n", total_pixels);
    fprintf (memlog_fp, "igrid_count = %u\n", igrid_count);
    fprintf (memlog_fp, "pgrid_count = %u\n", pgrid_count);
    fprintf (memlog_fp, "wgrid_count = %u\n", wgrid_count);
    fprintf (memlog_fp, "check_pixel_count = %u\n", check_pixel_count);
    fprintf (memlog_fp, "BYTES_PER_WORD = %u\n", BYTES_PER_WORD);
    fprintf (memlog_fp, "BYTES_PER_PIXEL = %u\n", BYTES_PER_PIXEL);
    fprintf (memlog_fp, "bytes_p_grid = %u\n", bytes_p_grid);
    fprintf (memlog_fp, "bytes_p_grid_rounded2wordboundary = %u\n",
             bytes_p_grid_rounded2wordboundary);
    fprintf (memlog_fp, "words in a grid = %u\n",
             bytes_p_grid_rounded2wordboundary / BYTES_PER_WORD);
#ifdef PACKING
    fprintf (memlog_fp, "BYTES_PER_PIXEL_PACKED = %u\n",
             BYTES_PER_PIXEL_PACKED);
    fprintf (memlog_fp, "bytes_p_packed_grid = %u\n", bytes_p_packed_grid);
    fprintf (memlog_fp, "bytes_p_packed_grid_rounded2wordboundary = %u\n",
             bytes_p_packed_grid_rounded2wordboundary);
#endif
    fprintf (memlog_fp, "igrid_size = %u words\n", igrid_size);
    fprintf (memlog_fp, "pgrid_size = %u words\n", pgrid_size);
    fprintf (memlog_fp, "wgrid_size = %u words\n", wgrid_size);
    fprintf (memlog_fp, "mem_check_size = %u words\n", mem_check_size);
    fprintf (memlog_fp, "bytes2allocate = %u\n", bytes2allocate);
  }

  mem_allocate ();
  mem_partition (memlog_fp);
  mem_InvalidateCheckArray ();
  mem_CloseLog ();
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_GetLogFP
** PURPOSE:       return memory log fp
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
FILE *
  mem_GetLogFP ()
{
  char func[] = "mem_GetLogFP";
  if (memlog_fp == NULL)
  {
    FILE_OPEN (memlog_fp, mem_log_filename, "a");
  }
  return memlog_fp;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_CloseLog
** PURPOSE:       close log file
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_CloseLog ()
{
    if(memlog_fp != NULL ) {
        fclose (memlog_fp);
        memlog_fp = NULL;
    }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_CheckMemory
** PURPOSE:       check memory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_CheckMemory (FILE * fp, char *module, char *function, int line)
{
  char func[] = "mem_CheckMemory";
  int i;
  int j;
  int thread_id = omp_get_thread_num();

  fprintf (fp, "%s %u MEMORY CHECK at %s %s %u\n",
           __FILE__, __LINE__, module, function, line);
  mem_CheckCheckArray ();
  for (i = 0; i < wgrid_count; i++)
  {
    if (wgrid_array[thread_id][i].free)
    {
#ifdef MEMORY_CHECK_LEVEL3
      for (j = 0; j < total_pixels; j++)
      {
        if (wgrid_array[thread_id][i].ptr[j] != invalid_val)
        {
          sprintf (msg_buf, "grid %d is not invalid", wgrid_array[thread_id][i].ptr);
          LOG_ERROR (msg_buf);
          sprintf (msg_buf, "current_owner: %s", wgrid_array[thread_id][i].current_owner);
          LOG_ERROR (msg_buf);
          sprintf (msg_buf, "previous_owner: %s", wgrid_array[thread_id][i].previous_owner);
          LOG_ERROR (msg_buf);
          EXIT (1);
        }
      }
#endif
    }
    else
    {
      for (j = 0; j < total_pixels; j++)
      {
        if (!((0 <= wgrid_array[thread_id][i].ptr[j]) && (wgrid_array[thread_id][i].ptr[j] < 256)))
        {
          sprintf (msg_buf, "grid %d is out of range", wgrid_array[thread_id][i].ptr);
          LOG_ERROR (msg_buf);
          sprintf (msg_buf, "current_owner: %s", wgrid_array[thread_id][i].current_owner);
          LOG_ERROR (msg_buf);
          sprintf (msg_buf, "previous_owner: %s", wgrid_array[thread_id][i].previous_owner);
          LOG_ERROR (msg_buf);
          EXIT (1);
        }
      }
    }
  }
  fprintf (fp, "MEMORY CHECK OK\n");
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_InvalidateCheckArray
** PURPOSE:       invalidate the check memory array elements
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_InvalidateCheckArray ()
{
  int i;
  for (i = 0; i < mem_check_count; i++)
  {
    *(mem_check_array[i]) = invalid_val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_CheckCheckArray
** PURPOSE:       memory check
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_CheckCheckArray ()
{
  char func[] = "mem_CheckCheckArray";
  int i;
  for (i = 0; i < mem_check_count; i++)
  {
    if (*(mem_check_array[i]) != invalid_val)
    {
      sprintf (msg_buf, "ptr = %d failed memory check", mem_check_array[i]);
      LOG_ERROR (msg_buf);
      sprintf (msg_buf, "i=%u *ptr=%X invalid_val=%X",
               i, *(mem_check_array[i]), invalid_val);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_partition
** PURPOSE:       partition the memory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_partition (FILE * fp)
{
  int i, j;
  PIXEL *temp_ptr = (PIXEL *) mem_ptr;
  PIXEL *end_ptr;


  if (fp)
  {
    end_ptr = (temp_ptr + bytes2allocate / BYTES_PER_WORD);
    fprintf (fp, "\n\nMemory starts at %d and ends at %d\n",
             temp_ptr, end_ptr);
  }
  mem_check_count = 0;
  for (i = 0; i < igrid_GetIGridCount (); i++)
  {
    mem_check_array[mem_check_count++] = temp_ptr;
    temp_ptr += mem_check_size;
    igrid_array[i].ptr = (GRID_P) temp_ptr;
    temp_ptr += igrid_size;
    strcpy (igrid_array[i].current_owner, "");
    mem_igrid_push (i);
    if (fp)
    {
      fprintf (fp, "%d mem_check_array[%2u]\n",
               mem_check_array[mem_check_count - 1], mem_check_count - 1);
      fprintf (fp, "%d igrid_array[%2u]\n", igrid_array[i].ptr, i);
    }
  }

  for (i = 0; i < NUM_THREADS; ++i)
  {
    for (j = 0; j < pgrid_GetPGridCount (); ++j)
    {
      mem_check_array[mem_check_count++] = temp_ptr;
      temp_ptr += mem_check_size;
      pgrid_array[i][j].ptr = (GRID_P) temp_ptr;
      temp_ptr += pgrid_size;
      strcpy (pgrid_array[i][j].current_owner, "");
      mem_pgrid_push (i, j);
      if (fp)
      {
        fprintf (fp, "%d mem_check_array[%2u]\n",
                mem_check_array[mem_check_count - 1], mem_check_count - 1);
        fprintf (fp, "%d pgrid_array[%2u][%2u]\n", pgrid_array[i][j].ptr, i, j);
      }
    }
  }

  for (i = 0; i < NUM_THREADS; ++i)
  {
    for (j = 0; j < wgrid_GetWGridCount (); ++j)
    {
      mem_check_array[mem_check_count++] = temp_ptr;
      temp_ptr += mem_check_size;
      wgrid_array[i][j].ptr = (GRID_P) temp_ptr;
      printf("%d %d\n", i, j);
      mem_InvalidateGrid (wgrid_array[i][j].ptr);
      temp_ptr += wgrid_size;
      strcpy (wgrid_array[i][j].current_owner, "");
      mem_wgrid_push (i, j);
      if (fp)
      {
        fprintf (fp, "%d mem_check_array[%2u]\n",
                mem_check_array[mem_check_count - 1], mem_check_count - 1);
        fprintf (fp, "%d wgrid_array[%2u][%2u]\n", wgrid_array[i][j].ptr, i, j);
      }
    }
  }

  //printf("\n%s\n", "Reach parallel region");

  // for (i = 0; i < pgrid_GetPGridCount (); i++)
  // {
  //   mem_check_array[mem_check_count++] = temp_ptr;
  //   temp_ptr += mem_check_size;
  //   pgrid_array[i].ptr = (GRID_P) temp_ptr;
  //   temp_ptr += pgrid_size;
  //   strcpy (pgrid_array[i].current_owner, "");
  //   mem_pgrid_push (i);
  //   if (fp)
  //   {
  //     fprintf (fp, "%d mem_check_array[%2u]\n",
  //              mem_check_array[mem_check_count - 1], mem_check_count - 1);
  //     fprintf (fp, "%d pgrid_array[%2u]\n", pgrid_array[i].ptr, i);
  //   }
  // }

  // for (i = 0; i < wgrid_GetWGridCount (); i++)
  // {
  //   mem_check_array[mem_check_count++] = temp_ptr;
  //   temp_ptr += mem_check_size;
  //   wgrid_array[i].ptr = (GRID_P) temp_ptr;
  //   mem_InvalidateGrid (wgrid_array[i].ptr);
  //   temp_ptr += wgrid_size;
  //   strcpy (wgrid_array[i].current_owner, "");
  //   mem_wgrid_push (i);
  //   if (fp)
  //   {
  //     fprintf (fp, "%d mem_check_array[%2u]\n",
  //              mem_check_array[mem_check_count - 1], mem_check_count - 1);
  //     fprintf (fp, "%d wgrid_array[%2u]\n", wgrid_array[i].ptr, i);
  //   }
  // }
  mem_check_array[mem_check_count++] = temp_ptr;
  if (fp)
  {
    fprintf (fp, "%d mem_check_array[%2u]\n",
             mem_check_array[mem_check_count - 1], mem_check_count - 1);
    fprintf (fp, "%d End of memory \n", end_ptr);
  }
  // min_wgrid_free_tos = wgrid_free_tos;
  for (i = 0; i < NUM_THREADS; ++i)
  {
    min_wgrid_free_tos[i] = wgrid_free_tos[i];
  }

}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_InvalidateGrid
** PURPOSE:       invalidate a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_InvalidateGrid (GRID_P ptr)
{
  int i;

  for (i = 0; i < total_pixels; i++)
  {
    ptr[i] = invalid_val;
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_CheckInvalidateGrid
** PURPOSE:       memory check a grid
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_CheckInvalidateGrid (GRID_P ptr)
{
  char func[] = "mem_CheckInvalidateGrid";
  int i;

  for (i = 0; i < total_pixels; i++)
  {
    if (ptr[i] != invalid_val)
    {
      sprintf (msg_buf, "grid %d is not invalid", ptr);
      LOG_ERROR (msg_buf);
      EXIT (1);
    }
  }
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_allocate
** PURPOSE:       allocate memory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_allocate ()
{
  char func[] = "mem_allocate";
  mem_ptr = NULL;
  mem_ptr = malloc (bytes2allocate);
  if (mem_ptr == NULL)
  {
    sprintf (msg_buf, "Unable to allocate %u bytes of memory", bytes2allocate);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  if (scen_GetLogFlag ())
  {
    scen_Append2Log ();
    fprintf (scen_GetLogFP (), "%s %u Allocated %u bytes of memory\n",
             __FILE__, __LINE__, bytes2allocate);
    scen_CloseLog ();
  }
  // TODO: Figure out what memset is SEGV'ing when > 2GB
  //memset (mem_ptr, 0, bytes2allocate);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_igrid_push
** PURPOSE:       push igrid onto a stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_igrid_push (int i)
{
  char func[] = "mem_igrid_push";
  if (igrid_free_tos >= 50)
  {
    sprintf (msg_buf, "igrid_free_tos >= 50");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  igrid_free[igrid_free_tos] = i;
  igrid_array[i].free = TRUE;
  igrid_free_tos++;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_igrid_pop
** PURPOSE:       pop an igrid from the stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static int
  mem_igrid_pop ()
{
  char func[] = "mem_igrid_pop";
  igrid_free_tos--;
  if (igrid_free_tos < 0)
  {
    sprintf (msg_buf, "igrid_free_tos < 0");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  igrid_array[igrid_free_tos].free = FALSE;
  return igrid_free[igrid_free_tos];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_pgrid_push
** PURPOSE:       push a pgrid onto a specific stack depending on parameter i
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_pgrid_push (int i, int j)
{
  char func[] = "mem_pgrid_push";
  if (pgrid_free_tos[i] >= 50)
  {
    sprintf (msg_buf, "pgrid_free_tos[%d] >= 50", i);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  pgrid_free[i][pgrid_free_tos[i]] = j;
  pgrid_array[i][j].free = TRUE;
  pgrid_free_tos[i]++;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_pgrid_pop
** PURPOSE:       pop a pgrid from the specific stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static int
  mem_pgrid_pop (int i)
{
  char func[] = "mem_pgrid_pop";
  pgrid_free_tos[i]--;
  if (pgrid_free_tos[i] < 0)
  {
    sprintf (msg_buf, "pgrid_free_tos[%d] < 0", i);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  pgrid_array[i][pgrid_free_tos[i]].free = FALSE;
  return pgrid_free[i][pgrid_free_tos[i]];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_wgrid_push
** PURPOSE:       push a wgrid onto the stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static void
  mem_wgrid_push (int i, int j)
{
  //printf("%s\n", "push grid");
  char func[] = "mem_wgrid_push";
  if (wgrid_free_tos[i] >= 50)
  {
    sprintf (msg_buf, "wgrid_free_tos[%d] >= 50", i);
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  wgrid_free[i][wgrid_free_tos[i]] = j;
  wgrid_array[i][j].free = TRUE;
#ifdef MEMORY_CHECK_LEVEL3
  mem_InvalidateGrid (wgrid_array[i][j].ptr);
#endif
  wgrid_free_tos[i]++;
  //printf("%s\n", "end push grid");
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_wgrid_pop
** PURPOSE:       pop a wgrid from the stack
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
static int
  mem_wgrid_pop (int i)
{
  char func[] = "mem_wgrid_pop";
  wgrid_free_tos[i]--;
  if (wgrid_free_tos[i] < 0)
  {
    sprintf (msg_buf, "wgrid_free_tos[%d] < 0", i);
    LOG_ERROR (msg_buf);
    sprintf (msg_buf, "Increase NUM_WORKING_GRIDS in scenario file");
    LOG_ERROR (msg_buf);
    EXIT (1);
  }
  wgrid_array[i][wgrid_free[i][wgrid_free_tos[i]]].free = FALSE;
  min_wgrid_free_tos[i] = MIN (min_wgrid_free_tos[i], wgrid_free_tos[i]);
#ifdef MEMORY_CHECK_LEVEL3
  mem_CheckInvalidateGrid (wgrid_array[i][wgrid_free[i][wgrid_free_tos[i]]].ptr);
#endif
  return wgrid_free[i][wgrid_free_tos[i]];
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_ReinvalidateMemory
** PURPOSE:       invalidate the memory
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_ReinvalidateMemory ()
{
  int i;
  int thread_id = omp_get_thread_num();

  for (i = 0; i < wgrid_GetWGridCount (); i++)
  {
    mem_InvalidateGrid (wgrid_array[thread_id][i].ptr);
  }
  mem_InvalidateCheckArray ();
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: memGetBytesPerGridRound
** PURPOSE:       return # bytes per grid rounded to word boundary
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
int
  memGetBytesPerGridRound ()
{
  return bytes_p_grid_rounded2wordboundary;
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_LogMinFreeWGrids
** PURPOSE:       log the minmum # of free grids
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_LogMinFreeWGrids (FILE * fp)
{
  int thread_id = omp_get_thread_num();
  fprintf (fp, "Minmum number of Free working grids=%u\n",
           min_wgrid_free_tos[thread_id]);
  fprintf (fp,
   "For max efficiency of memory usage reduce NUM_WORKING_GRIDS by %u\n",
           min_wgrid_free_tos[thread_id]);
}

/******************************************************************************
*******************************************************************************
** FUNCTION NAME: mem_LogPartition
** PURPOSE:       log memory partition to FILE * fp_in
** AUTHOR:        Keith Clarke
** PROGRAMMER:    Tommy E. Cathey of NESC (919)541-1500
** CREATION DATE: 11/11/1999
** DESCRIPTION:
**
**
*/
void
  mem_LogPartition (FILE * fp_in)
{
  int i;
  FILE *fp;
  int thread_id = omp_get_thread_num();

  fp = fp_in ? fp_in : stdout;
  if (!((fp == stdout) && (!scen_GetEchoFlag ())))
  {
    for (i = 0; i < igrid_GetIGridCount (); i++)
    {
      fprintf (fp, "%d igrid_array[%u].ptr\n", igrid_array[i].ptr, i);
    }
    // for (i = 0; i < pgrid_GetPGridCount (); i++)
    // {
    //   fprintf (fp, "pgrid_array[%u].ptr = %d\n", i, pgrid_array[i].ptr);
    // }
    // for (i = 0; i < wgrid_GetWGridCount (); i++)
    // {
    //   fprintf (fp, "wgrid_array[%u].ptr = %d\n", i, wgrid_array[i].ptr);
    // }
    for (i = 0; i < mem_check_count; i++)
    {
      fprintf (fp, "mem_check_array[%u].ptr = %d\n", i, mem_check_array[i]);
    }
  }
}
