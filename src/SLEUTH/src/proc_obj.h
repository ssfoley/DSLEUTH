void proc_MemoryLog(FILE* fp);
void proc_SetProcessingType(int i);
void proc_SetTotalRuns();
void proc_SetCurrentRun(int i);
void proc_SetCurrentMonteCarlo(int i);
void proc_SetCurrentYear(int i);
void proc_SetStopYear(int i);

int proc_GetProcessingType();
int proc_GetTotalRuns();
int proc_GetCurrentRun();
int proc_GetCurrentMonteCarlo();
int proc_GetCurrentYear();
int proc_GetStopYear();
int proc_GetLastRun();
BOOLEAN proc_GetLastRunFlag();
BOOLEAN proc_GetLastMonteCarloFlag ();
void proc_SetLastMonteCarlo(int val);

int proc_IncrementCurrentRun();
int proc_SetLastMonteCarloFlag();
int proc_IncrementCurrentYear();
void proc_SetNumRunsExecThisCPU (int val);
int proc_GetNumRunsExecThisCPU ();
void proc_IncrementNumRunsExecThisCPU ();
BOOLEAN proc_GetRestartFlag ();
void proc_SetRestartFlag (BOOLEAN i);

