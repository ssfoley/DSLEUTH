#include "globals.h"
#include "ugm_typedefs.h"

void mem_Init();
void mem_MemoryLog(FILE* fp);

void mem_LogPartition(FILE* fp);
int mem_GetPackedBytesPerGrid();
GRID_P mem_GetIGridPtr( char* owner );

GRID_P mem_GetPGridPtr( char* owner );

GRID_P mem_GetWGridPtr( char* module, char* who, int line );

GRID_P mem_GetWGridFree( char* module, char* who, int line,GRID_P ptr );

int mem_GetTotalPixels();

void mem_CheckMemory(FILE* fp,char* module, char* function, int line);

void mem_ReinvalidateMemory();

int memGetBytesPerGridRound();

void mem_LogMinFreeWGrids(FILE* fp);
FILE* mem_GetLogFP();
void mem_CloseLog();
