#ifndef COLOR_OBJ_H
#define COLOR_OBJ_H

#define LANDUSE_COLORTABLE 0
#define NONLANDUSE_COLORTABLE 1
#define PROBABILITY_COLORTABLE 2
#define GROWTH_COLORTABLE 3
#define DELTATRON_COLORTABLE 4
#define GRAYSCALE_COLORTABLE 5

#define WATER_COLOR_INDEX 0
#define SEED_COLOR_INDEX 1
#define DATE_COLOR_INDEX 255

#define MAX_COLORS 256
#define RED_MASK   0XFF0000
#define GREEN_MASK 0X00FF00
#define BLUE_MASK  0X0000FF
#define START_INDEX_FOR_PROBABILITY_COLORS 2
#define PHASE0G 3
#define PHASE1G 4
#define PHASE2G 5
#define PHASE3G 6
#define PHASE4G 7
#define PHASE5G 8

typedef struct RGB
{
  int red;
  int green;
  int blue;
}
RGB_t;

struct colortable
{
  int size;
  char name[80];
  RGB_t color[MAX_COLORS];
};

struct colortable*
color_GetColortable(int i);

void color_Init();

void color_LogIt(FILE* fp);
void color_MemoryLog(FILE* fp);
#endif
