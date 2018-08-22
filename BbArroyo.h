#ifndef BBARROYO_H
#define BBARROYO_H
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> //for usleep
#include "cssl_lib.h"


struct type_ArroyoInfo {
    // Semaphores
    //sem_t GetTemperature_count;
    sem_t SetTemperature_count;
    cssl_t *Serial;
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
    char name[64];
};


class BbArroyo
{    
public:
    type_ArroyoInfo parm;
    BbArroyo();
    void Init(char *port, int baud, char *name);
    void Open(char *port, int baud);
    float Get();
    void Set(float DesiredTemperature);
    void Kill();
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &BbArroyo::wrapperMonitor, &parm);
    }
    void StartControl()
    {
        pthread_create(&thread_Control, NULL, &BbArroyo::wrapperControl, &parm);
    }

private:
    pthread_t thread_Monitor;
    pthread_t thread_Control;
    void Monitor();
    void Control();

    static void* wrapperMonitor(void* thisPtr)
    {
        ((BbArroyo*) thisPtr)->Monitor();
        return(NULL);
    }
    static void* wrapperControl(void* thisPtr)
    {
        ((BbArroyo*) thisPtr)->Control();
        return(NULL);
    }
};

#endif // BBARROYO_H
