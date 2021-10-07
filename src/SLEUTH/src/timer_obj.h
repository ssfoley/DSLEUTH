
#define SPREAD_TOTAL_TIME 0 
#define SPR_PHASE1N3 1
#define SPR_PHASE4 2
#define SPR_PHASE5 3
#define GDIF_WRITEGIF 4
#define GDIF_READGIF 5
#define DELTA_DELTATRON 6
#define DELTA_PHASE1 7
#define DELTA_PHASE2 8
#define GRW_GROWTH 9
#define DRV_DRIVER 10
#define TOTAL_TIME 11

void timer_Init();
void timer_Start(int val);
void timer_Stop(int val);
void timer_LogIt(FILE* fp);
void timer_MemoryLog(FILE* fp);
double timer_Read (int val);
char* timer_Format(char* buf, unsigned int sec);
