#ifndef TIRTHREADS_H
#define TIRTHREADS_H

#ifdef __unix
void tic(timeval *startTime);
long toc(timeval startTime);
#else
void tic(time_t *startTime);
long toc(time_t startTime);
#endif
//void *runAutoScan(void*);
//void *runScript(void*);

//DWORD WINAPI runAutoScan(LPVOID pParam);

bool Log(char *Entry);
bool OpenLog();
void CloseLog();
bool LogCmd(char *Entry);
bool OpenLogCmd();
void CloseLogCmd();

//#define sem_wait(a) WaitForSingleObject(a, INFINITE)

#endif // TIRTHREADS_H
