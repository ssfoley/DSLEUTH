#ifndef COEFF_OBJ_H
#define COEFF_OBJ_H
#include <stdio.h>

typedef struct
{
   double    diffusion;
   double    spread;
   double    breed;
   double    slope_resistance;
   double    road_gravity;
} coeff_val_info;
typedef struct
{
   int diffusion;
   int spread;
   int breed;
   int slope_resistance;
   int road_gravity;
} coeff_int_info;

void coeff_MemoryLog(FILE* fp);
void coeff_WriteCurrentCoeff();
#if 1
void coeff_ConcatenateFiles();
#else
void coeff_ConcatenateFiles(int current_run);
#endif
void coeff_CreateCoeffFile();
void coeff_SetSavedDiffusion(double val);
void coeff_SetSavedSpread(double val);
void coeff_SetSavedBreed(double val);
void coeff_SetSavedSlopeResist(double val);
void coeff_SetSavedRoadGravity(double val);

void coeff_SetCurrentDiffusion(double val);
void coeff_SetCurrentSpread(double val);
void coeff_SetCurrentBreed(double val);
void coeff_SetCurrentSlopeResist(double val);
void coeff_SetCurrentRoadGravity(double val);

void coeff_SetStepDiffusion(int val);
void coeff_SetStepSpread(int val);
void coeff_SetStepBreed(int val);
void coeff_SetStepSlopeResist(int val);
void coeff_SetStepRoadGravity(int val);

void coeff_SetStartDiffusion(int val);
void coeff_SetStartSpread(int val);
void coeff_SetStartBreed(int val);
void coeff_SetStartSlopeResist(int val);
void coeff_SetStartRoadGravity(int val);

void coeff_SetStopDiffusion(int val);
void coeff_SetStopSpread(int val);
void coeff_SetStopBreed(int val);
void coeff_SetStopSlopeResist(int val);
void coeff_SetStopRoadGravity(int val);

void coeff_SetBestFitDiffusion(int val);
void coeff_SetBestFitSpread(int val);
void coeff_SetBestFitBreed(int val);
void coeff_SetBestFitSlopeResist(int val);
void coeff_SetBestFitRoadGravity(int val);

double coeff_GetSavedDiffusion();
double coeff_GetSavedSpread();
double coeff_GetSavedBreed();
double coeff_GetSavedSlopeResist();
double coeff_GetSavedRoadGravity();

double coeff_GetCurrentDiffusion();
double coeff_GetCurrentSpread();
double coeff_GetCurrentBreed();
double coeff_GetCurrentSlopeResist();
double coeff_GetCurrentRoadGravity();

int coeff_GetStepDiffusion();
int coeff_GetStepSpread();
int coeff_GetStepBreed();
int coeff_GetStepSlopeResist();
int coeff_GetStepRoadGravity();

int coeff_GetStartDiffusion();
int coeff_GetStartSpread();
int coeff_GetStartBreed();
int coeff_GetStartSlopeResist();
int coeff_GetStartRoadGravity();

int coeff_GetStopDiffusion();
int coeff_GetStopSpread();
int coeff_GetStopBreed();
int coeff_GetStopSlopeResist();
int coeff_GetStopRoadGravity();

int coeff_GetBestFitDiffusion();
int coeff_GetBestFitSpread();
int coeff_GetBestFitBreed();
int coeff_GetBestFitSlopeResist();
int coeff_GetBestFitRoadGravity();


void coeff_LogSaved(FILE* fp);
void coeff_LogCurrent(FILE* fp);
void coeff_LogStep(FILE* fp);
void coeff_LogStart(FILE* fp);
void coeff_LogStop(FILE* fp);
void coeff_LogBestFit(FILE* fp);

void coeff_SelfModication(double growth_rate, double percent_urban);
#endif
