#ifndef THREADS_H
#define THREADS_H
#include "tprocess.h"
#ifdef __unix__
void *runBbUnit1(void*);
void *runRotationA(void*);
void *runCalAcq(void*);
void *runManualAcq(void*);
void *runScan(void*);
void *runViewRange(void*);

void *runAcquisition(void*);
void *runSavedata(void*);
void *runWaterfall(void*);

void AcquireAtPosition(float Position);
void AcquireWithMotion(void);

void DoAcquireManual(void);
void DoAcquireManualBB(float temp, bool hotFlag, TProcess *tp) ;
void DoAcquireWaveCal(void);
void DoAcquirePaddleCold(void);
void DoAcquirePaddleHot(void);
void DoAcquireRotorCold(void);
void DoAcquireRotorHot(void);
#else
void DoAcquireManual(void);
void DoAcquireManual(TProcess *tp);
void DoAcquireManualBB(float temp, bool hotFlag, TProcess *tp) ;
void DoAcquireWaveCal(void);
void DoAcquirePaddleCold(void);
void DoAcquirePaddleHot(void);
void DoAcquireRotorCold(void);
void DoAcquireRotorHot(void);

DWORD WINAPI runAcquisition(LPVOID pParam);
DWORD WINAPI runSavedata(LPVOID pParam);
DWORD WINAPI runWaterfall(LPVOID pParam);
DWORD WINAPI runRotationA(LPVOID pParam);
DWORD WINAPI runCalAcq(LPVOID pParam);
DWORD WINAPI runManualAcq(LPVOID pParam);
DWORD WINAPI runScan(LPVOID pParam);
DWORD WINAPI runAutoScan(LPVOID pParam);
DWORD WINAPI runScript(LPVOID pParam);
DWORD WINAPI runFfc(LPVOID pParam);

#endif
#endif // THREADS_H
