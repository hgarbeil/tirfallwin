#ifndef ACQTAUSERIAL_H
#define ACQTAUSERIAL_H
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h> //for usleep
//#include "cssl_lib.h"

#ifdef __unix__
#include <unistd.h> //for usleep
#include "cssl_lib.h"
#endif

#ifdef _WIN32
//#include "windows.h"
#endif



struct type_CameraInfo {
    // Semaphores
    //sem_t GetTemperature_count;
    sem_t DoFFC_count;
#ifdef __unix__
    cssl_t *Serial;
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
    //float DesiredTemperature;
    //char name[64];
};


class TauSerial
{    
public:
    type_CameraInfo parm;
    TauSerial();
    void Init(char *port, int baud);
    void Open(char *port, int baud);
    float Get();
    short FFC();
    void Kill();
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &TauSerial::wrapperMonitor, &parm);
    }
    void StartControl()
    {
        pthread_create(&thread_Control, NULL, &TauSerial::wrapperControl, &parm);
    }
    void AutoFFC();
    void ManualFFC();
    void DoFFC();
    float GetTemperature();
    void ExternalMode(short Action);   //Action=0, internal, Action=1, external
    void GetSN();
    void GetPartNumber();

private:
    pthread_t thread_Monitor;
    pthread_t thread_Control;
    void Monitor();
    void Control();

    static void* wrapperMonitor(void* thisPtr)
    {
        ((TauSerial*) thisPtr)->Monitor();
        return(NULL);
    }
    static void* wrapperControl(void* thisPtr)
    {
        ((TauSerial*) thisPtr)->Control();
        return(NULL);
    }
};

#endif // ACQTAUSERIAL_H
