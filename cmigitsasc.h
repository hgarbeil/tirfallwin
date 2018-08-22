#ifndef CMIGITSASC_H
#define CMIGITSASC_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __unix__
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> //for usleep
#include "cssl_lib.h"
#else
#include "COMDevice.h"
#endif


#define BUFSIZE 512

// C-MIGITS Message IDs
const unsigned short SYNCWORD = 0x81FF;
const short TIMEMARK_ID = 3;          //   1 time(s) a second
const short STATUS_ID   = 3500;
const short NAVSOL_ID   = 3501;       //  10 time(s) a second
const short DELTA_ID    = 3502;
const short FLIGHTCONTROL_ID = 3512;  // 100 time(s) a second
const short BITTEST_ID  = 3503;
const short ERROR_ID    = 999;

// Message Sizes (bytes including checksum but not header)
const int HEADER_SIZE   = 10;
const int TIMEMARK_SIZE = 126;	// Check
const int STATUS_SIZE   = 34;
const int NAVSOL_SIZE   = 46;
const int DELTA_SIZE    = 34;
const int FLIGHTCONTROL_SIZE = 34;
const int BITTEST_SIZE  = 202;

float FloatConvert(long value, long scale);
unsigned short Checksum(unsigned short *pData, long numwords);
double DoubleConvert(long value1, unsigned long value2, long scale);


struct type_GpsInfo {
    // Semaphores
    //sem_t GetTemperature_count;
    //    sem_t SetTemperature_count;
#ifdef __unix__
    cssl_t *Serial;
#else
    COMDevice  Serial;
#endif
    char Status[256];
    char *NewLinePos;
    char serialdata[1024];
    char comStringIn[1024];
    bool PortOpen;
    //bool Busy;
    bool MonitorInitialized;
    bool ControlInitialized;
    bool Terminate;
    float PresentTemperature;
    float DesiredTemperature;
    char name[64];
    double Roll;
    double Pitch;
    double Heading;
    float Latitude;
    float Longitude;
    float Altitude;
    BOOL GpsBackwards;
    double Time;
    char CurrentMode[64];
    int GpsValid;
    int Satellites;
    short Sat1Valid;
    short Sat2Valid;
    short Sat3Valid;
    short Sat4Valid;
    short Sat5Valid;
    float HeadingInit;
    float LatitudeInit;
    float LongitudeInit;
    int AltitudeInit;
    BOOL RunningToggle;
};


class CMigitsA
{    
public:
    type_GpsInfo parm;
    int AlignmentMode;
    CMigitsA();
    void Init();
    void Open(int port);
    //float Get();
    void Kill();
    void Backwards();
    void GroundAlign();
    void AirAlign();
    void CMigitsAlign();

#ifdef __unix__
    void StartMonitor()
    {
        pthread_create(&thread_Monitor, NULL, &CMigitsA::wrapperMonitor, &parm);
    }
#else
    CWinThread* m_pMonitorThread;

    void StartMonitor()
    {
        VERIFY(m_pMonitorThread = AfxBeginThread(Monitor, this, THREAD_PRIORITY_NORMAL));
    }
#endif





private:
    //pthread_t thread_Monitor;
    void Monitor();

#ifdef __unix__
    static void* wrapperMonitor(void* thisPtr)
    {
        ((CMigitsA*) thisPtr)->Monitor();
        return(NULL);
    }
#else
    static UINT Monitor(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((CMigitsA*)pParam)->Monitor();
        ::PostQuitMessage(0);
        return 0;
    }
#endif
};

#endif // CMIGITSASC_H
