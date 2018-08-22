#ifndef SMC100_H
#define SMC100_H
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


struct type_StageInfo {
    // Semaphores
#ifdef __unix__
    //sem_t GetPosition_count;
    sem_t SetPositionRotation_count;
    sem_t SetPositionGoniometer_count;
    cssl_t *Serial;
#else
    HANDLE SetPositionRotation_count;
    HANDLE SetPositionGoniometer_count;
    COMDevice Serial;
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
    float PresentPositionCh0;
    float PresentPositionCh1;
    float DesiredPositionRotation;
    float DesiredSpeedRotation;
    float DesiredPositionGoniometer;
    float DesiredSpeedGoniometer;
    //int ChannelNum;
};


class Smc100
{    
public:
    type_StageInfo parm;
    Smc100();
    void Init(int port);
    void Open(int port);
    void Home(int ChannelNum);
    float Get(int ChannelNum);
    void Set(float DesiredSpeed, float DesiredPosition, int ChannelNum);
    float GetPosition(int ChannelNum);
    void Kill();
    void Halt();
#ifdef __unix__
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &Smc100::wrapperMonitor, &parm);
    }
    void StartControlRotation()
    {
        pthread_create(&thread_ControlRotation, NULL, &Smc100::wrapperControlRotation, &parm);
    }
    void StartControlGoniometer()
    {
        pthread_create(&thread_ControlGoniometer, NULL, &Smc100::wrapperControlGoniometer, &parm);
    }
#else
    CWinThread* m_pMonitorThread;
    CWinThread* m_pRotationThread;
    CWinThread* m_pGoniometerThread;

    void StartMonitor()
    {
        VERIFY(m_pMonitorThread = AfxBeginThread(Monitor, this, THREAD_PRIORITY_NORMAL));
    }
    void StartControlRotation()
    {
        VERIFY(m_pRotationThread = AfxBeginThread(ControlRotation, this, THREAD_PRIORITY_NORMAL));
    }
    void StartControlGoniometer()
    {
        VERIFY(m_pGoniometerThread = AfxBeginThread(ControlGoniometer, this, THREAD_PRIORITY_NORMAL));
    }
#endif
private:
    void Monitor();
    void ControlRotation();
    void ControlGoniometer();

#ifdef __unix__
    pthread_t thread_Monitor;
    pthread_t thread_ControlRotation;
    pthread_t thread_ControlGoniometer;

    static void* wrapperMonitor(void* thisPtr)
    {
        ((Smc100*) thisPtr)->Monitor();
        return(NULL);
    }
    static void* wrapperControlRotation(void* thisPtr)
    {
        ((Smc100*) thisPtr)->ControlRotation();
        return(NULL);
    }
    static void* wrapperControlGoniometer(void* thisPtr)
    {
        ((Smc100*) thisPtr)->ControlGoniometer();
        return(NULL);
    }
#else
    //CWinThread* thread_Monitor;
    //CWinThread* thread_ControlRotation;
    //CWinThread* thread_ControlGoniometer;

    static UINT Monitor(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Smc100*)pParam)->Monitor();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlRotation(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Smc100*)pParam)->ControlRotation();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlGoniometer(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Smc100*)pParam)->ControlGoniometer();
        ::PostQuitMessage(0);
        return 0;
    }
#endif

};

#endif // SMC100_H
