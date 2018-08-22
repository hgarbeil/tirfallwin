#ifndef BBSR800GPIB_H
#define BBSR800GPIB_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __unix__
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> //for usleep
#else
#include "COMDevice.h"
#endif


struct type_ArroyoInfoGpib {
    // Semaphores
#ifdef __unix__
    //sem_t GetTemperature_count;

    sem_t SetTemperature_count;
    cssl_t *Serial;
#else
    HANDLE SetTemperature_count;

    COMDevice  Serial;
#endif

    char Status[256];
    char *NewLinePos;
    char serialdata[256];
    char comStringIn[256];
    char comStringOut[256];
    bool PortOpen;
    bool Busy;
    bool MonitorInitialized;
    bool ControlInitialized;
    bool Terminate;
    float PresentTemperature;
    float DesiredTemperature;
    BOOL YesTemperatureStable;
    char name[64];
};


class BbSr800Gpib
{    
public:
    type_ArroyoInfoGpib parm;
    BbSr800Gpib();
    void Init(int port, /*int baud,*/ char *name);
    void Open(int port/*, int baud*/);
    float Get();
    BOOL IsStable();
    short Set(float DesiredTemperature);
    void Kill();
#ifdef __unix__
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &BbSr800Gpib::wrapperMonitor, &parm);
    }
    void StartControl()
    {
        pthread_create(&thread_Control, NULL, &BbSr800Gpib::wrapperControl, &parm);
    }
#else
    CWinThread* m_pMonitorThread;
    CWinThread* m_pControlThread;

    void StartMonitor()
    {
        VERIFY(m_pMonitorThread = AfxBeginThread(Monitor, this, THREAD_PRIORITY_NORMAL));
    }
    void StartControl()
    {
        VERIFY(m_pControlThread = AfxBeginThread(Control, this, THREAD_PRIORITY_NORMAL));
    }
#endif

private:
    void Monitor();
    void Control();

#ifdef __unix__
    pthread_t thread_Monitor;
    pthread_t thread_Control;

    static void* wrapperMonitor(void* thisPtr)
    {
        ((BbSr800Gpib*) thisPtr)->Monitor();
        return(NULL);
    }
    static void* wrapperControl(void* thisPtr)
    {
        ((BbSr800Gpib*) thisPtr)->Control();
        return(NULL);
    }
#else
    //CWinThread* thread_Monitor;
    //CWinThread* thread_Control;

    static UINT Monitor(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((BbSr800Gpib*)pParam)->Monitor();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT Control(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((BbSr800Gpib*)pParam)->Control();
        ::PostQuitMessage(0);
        return 0;
    }

#endif
};

#endif // BBSR800GPIB_H
