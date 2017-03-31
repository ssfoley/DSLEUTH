#ifndef GDIF_OBJ_H
#define GDIF_OBJ_H

#include "color_obj.h"

/*
 *
 * DATE_X & DATE_Y CONTROL WHERE THE DATE STRING
 * IS PRINTED ON THE GIF IMAGE
 *
 */
#define DATE_X 1
#define DATE_Y igrid_GetNumRows() - 16

void gdif_WriteColorKey ( struct colortable *colortable, char fname[]);
void gdif_ReadGIF (GRID_P gif_ptr, char *fname);
void gdif_WriteGIF(
                  GRID_P gif,
                  struct colortable *colortable,
                  char fname[],
                  char date[],
                  int date_color_index);

#endif
