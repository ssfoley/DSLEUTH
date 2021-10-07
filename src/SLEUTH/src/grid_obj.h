#ifndef GRID_OBJ_H
#define GRID_OBJ_H
typedef struct
{
   int   digit;
   char  string[DIGITS_IN_YEAR + 1];
}  year_info;

typedef struct
{
   GRID_P   ptr;
   BOOLEAN packed;
   int  color_bits;
   int  bits_per_pixel;
   int size_words;
   int size_bytes;
   int  nrows;
   int  ncols;
   int  max;
   int  min;
   int  histogram[256];
   char filename[MAX_FILENAME_LEN];
   year_info year;
}  grid_info;

void grid_SetMinMax(grid_info* ptr);
void grid_dump(FILE* fp,grid_info* grid_ptr);
void grid_histogram(grid_info * grid_ptr);
#endif
