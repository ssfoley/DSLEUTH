#ifndef LANDCLASS_OBJ_H
#define LANDCLASS_OBJ_H
#include "ugm_defines.h"

#define MAX_NUM_CLASSES 20
#define MAX_NEW_INDICES 256
#define MAX_LINE_LEN 256

/* structure to hold landuse class identification data */
typedef struct
{
   int   num;
   char  id[25];
   char  name[50];
   int   idx;
   int   red;
   int   green;
   int   blue;
   BOOLEAN   EXC;
   BOOLEAN   trans;
}  Classes;

void
landclass_Init();

void landclass_MemoryLog(FILE* fp);
Classes* landclass_GetReducedClassesPtr();
Classes* landclass_GetClassesPtr();
int* landclass_GetNewIndicesPtr();
int landclass_GetUrbanCode();
int landclass_GetNumLandclasses();
int landclass_GetNumReducedclasses();
int landclass_GetMaxLandclasses();
int landclass_GetClassNum(int i);
int landclass_GetClassIDX(int i);
int landclass_GetClassColor(int i);
BOOLEAN landclass_GetClassEXC(int i);
BOOLEAN landclass_GetClassTrans(int i);
int landclass_GetReducedNum(int i);
int landclass_GetReducedIDX(int i);
int landclass_GetReducedColor(int i);
BOOLEAN landclass_GetReducedEXC(int i);
BOOLEAN landclass_GetReducedTrans(int i);
void landclass_AnnualProbInit();
void landclass_AnnualProbUpdate(GRID_P land1_ptr);
void landclass_BuildProbImage(GRID_P cum_probability_ptr, GRID_P cum_uncertainty_ptr);
void landclass_LogIt(FILE* fp);
BOOLEAN landclass_IsAlandclass(int val);
void landclassSetGrayscale (int index, int val);
void landclassSetColor (int index, int val);
void landclassSetType (int index, char* string);
void landclassSetName (int index, char* string);
void landclassSetNumClasses (int val);
#endif
