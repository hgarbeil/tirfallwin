#ifndef MAESTRO_H
#define MAESTRO_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
//#include <unistd.h> //for usleep

#ifdef __unix__
#include <unistd.h> //for usleep
#endif

//#ifdef _WIN32
////#include "windows.h"
//#endif
////:O#include "cssl_lib.h"




struct type_PaddleInfo {
    // Semaphores
    //sem_t GetPosition_count;
    sem_t SetPosition_count;
    cssl_t *Serial;
    char Status[256];
    char *NewLinePos;
    char comStringIn[256];
    char comStringOut[256];
    bool PortOpen;
    bool Busy;
    bool MonitorInitialized;
    bool ControlInitialized;
    bool Terminate;
    unsigned char DesiredChannel;
    unsigned short DesiredPosition;
    unsigned short PresentPositionCh0;
    unsigned short PresentPositionCh1;
    unsigned short OnOff;
    unsigned short PowerLevel;
};


class Maestro
{    
public:
    type_PaddleInfo parm;
    Maestro();
    void Init(char *port);
    void Open(char *port);
    void Get(unsigned short *PresentPositionCh0, unsigned short *PresentPositionCh1);
    void Set(unsigned char DesiredChannel, unsigned short DesiredPosition);
    void Kill();
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &Maestro::wrapperMonitor, &parm);
    }
    void StartControl()
    {
        pthread_create(&thread_Control, NULL, &Maestro::wrapperControl, &parm);
    }
    void SetIlluminator(unsigned char DesiredChannel, unsigned short OnOff, unsigned short PowerLevel);

private:
#ifdef __unix__
    pthread_t thread_Monitor;
    pthread_t thread_Control;
#else
    CWinThread* thread_Monitor;
    CWinThread* thread_Control;
#endif
    void Monitor();
    void Control();
    void MaestroGoHome(void);
    void SetMaestroPos(unsigned char Channel, unsigned short Position);
    void SetMaestroSpeed(unsigned char Channel, unsigned short Speed);
    void SetMaestroAcceleration(unsigned char Channel, unsigned short Acceleration);
    unsigned short GetMaestroPos(unsigned char Channel);
    void IlluminatorControl(unsigned short OnOff, unsigned short PowerLevel);


    static void* wrapperMonitor(void* thisPtr)
    {
        ((Maestro*) thisPtr)->Monitor();
        return(NULL);
    }
    static void* wrapperControl(void* thisPtr)
    {
        ((Maestro*) thisPtr)->Control();
        return(NULL);
    }
};

#endif // MAESTRO_H
