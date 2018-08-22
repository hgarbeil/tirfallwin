#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h> //for usleep

#ifdef __unix__
#include <unistd.h> //for usleep
#include <pthread.h>
#include <semaphore.h>
#else
#include "COMDevice.h"
#endif



struct type_PaddleInfo {
    // Semaphores
#ifdef __unix__
    sem_t SetPositionPaddleCold_count;
    sem_t SetPositionPaddleHot_count;
    sem_t SetPositionPaddleAmbient_count;
    sem_t SetPositionRotor_count;
    sem_t SetTemperaturePaddleCold_count;
    sem_t SetTemperaturePaddleHot_count;
    sem_t SetTemperatureRotorCold_count;
    sem_t SetTemperatureRotorHot_count;
    sem_t SetPowerRotorCold_count;
    sem_t SetPowerRotorHot_count;
    sem_t SetPowerPaddleCold_count;
    sem_t SetPowerPaddleHot_count;
    cssl_t *Serial;
#else
    HANDLE SetPositionPaddleCold_count;
    HANDLE SetPositionPaddleHot_count;
    HANDLE SetPositionPaddleAmbient_count;
    HANDLE SetPositionRotor_count;
    HANDLE SetTemperaturePaddleCold_count;
    HANDLE SetTemperaturePaddleHot_count;
    HANDLE SetTemperatureRotorCold_count;
    HANDLE SetTemperatureRotorHot_count;
    HANDLE SetPowerRotorCold_count;
    HANDLE SetPowerRotorHot_count;
    HANDLE SetPowerPaddleCold_count;
    HANDLE SetPowerPaddleHot_count;
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
    //unsigned char DesiredChannel;
    unsigned short SetPositionPaddleCold;
    unsigned short SetPositionPaddleHot;
    unsigned short SetPositionPaddleAmbient;
    unsigned short SetPositionRotor;
    //unsigned short SetTemperaturePaddleCold;
    //unsigned short SetTemperaturePaddleHot;
    //unsigned short SetTemperatureRotorCold;
    //unsigned short SetTemperatureRotorHot;
    //unsigned short TemperaturePaddleCold;
    //unsigned short TemperaturePaddleHot;
    //unsigned short TemperatureRotorCold;
    //unsigned short TemperatureRotorHot;

    unsigned short AdcValue[4];
    unsigned short DacValue[4];
    unsigned short PowValue[4];
    float TemperatureSensor[16];
};



class Calibrator
{    
public:
    type_PaddleInfo parm;
    Calibrator();
    void Init(int port);
    void Open(int port);
    short Get(float *TemperaturePaddleCold, float *TemperaturePaddleHot,
              float *TemperatureRotorCold, float *TemperatureRotorHot, float *TemperatureSensor);
    short Set(unsigned char DesiredChannel, float DesiredPosition);
    void SetIlluminator(unsigned short OnOff, unsigned short PowerLevel);
    void Kill();
#ifdef __unix__
    void StartMonitor() { pthread_create(&thread_Monitor, NULL, &Calibrator::wrapperMonitor, &parm); }
    void StartControlPosPaddleCold() { pthread_create(&thread_ControlPosPaddleCold, NULL, &Calibrator::wrapperControlPosPaddleCold, &parm); }
    void StartControlPosPaddleHot() { pthread_create(&thread_ControlPosPaddleHot, NULL, &Calibrator::wrapperControlPosPaddleHot, &parm); }
    void StartControlPosPaddleAmbient() { pthread_create(&thread_ControlPosPaddleAmbient, NULL, &Calibrator::wrapperControlPosPaddleAmbient, &parm); }
    void StartControlPosRotor() { pthread_create(&thread_ControlPosRotor, NULL, &Calibrator::wrapperControlPosRotor, &parm); }

    void StartControlTempPaddleCold() { pthread_create(&thread_ControlTempPaddleCold, NULL, &Calibrator::wrapperControlTempPaddleCold, &parm); }
    void StartControlTempPaddleHot() { pthread_create(&thread_ControlTempPaddleHot, NULL, &Calibrator::wrapperControlTempPaddleHot, &parm); }
    void StartControlTempRotorCold() { pthread_create(&thread_ControlTempRotorCold, NULL, &Calibrator::wrapperControlTempRotorCold, &parm); }
    void StartControlTempRotorHot() { pthread_create(&thread_ControlTempRotorHot, NULL, &Calibrator::wrapperControlTempRotorHot, &parm); }

    void StartControlPowerPaddleCold() { pthread_create(&thread_ControlPowerPaddleCold, NULL, &Calibrator::wrapperControlPowerPaddleCold, &parm); }
    void StartControlPowerPaddleHot() { pthread_create(&thread_ControlPowerPaddleHot, NULL, &Calibrator::wrapperControlPowerPaddleHot, &parm); }
    void StartControlPowerRotorCold() { pthread_create(&thread_ControlPowerRotorCold, NULL, &Calibrator::wrapperControlPowerRotorCold, &parm); }
    void StartControlPowerRotorHot() { pthread_create(&thread_ControlPowerRotorHot, NULL, &Calibrator::wrapperControlPowerRotorHot, &parm); }
#else
    CWinThread* thread_Monitor;
    CWinThread* thread_ControlPosPaddleCold;
    CWinThread* thread_ControlPosPaddleHot;
    CWinThread* thread_ControlPosPaddleAmbient;
    CWinThread* thread_ControlPosRotor;
    CWinThread* thread_ControlTempPaddleCold;
    CWinThread* thread_ControlTempPaddleHot;
    CWinThread* thread_ControlTempRotorCold;
    CWinThread* thread_ControlTempRotorHot;
    CWinThread* thread_ControlPowerPaddleCold;
    CWinThread* thread_ControlPowerPaddleHot;
    CWinThread* thread_ControlPowerRotorCold;
    CWinThread* thread_ControlPowerRotorHot;

    void StartMonitor() { VERIFY(thread_Monitor = AfxBeginThread(Monitor, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPosPaddleCold() { VERIFY(thread_ControlPosPaddleCold = AfxBeginThread(ControlPosPaddleCold, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPosPaddleHot() { VERIFY(thread_ControlPosPaddleHot = AfxBeginThread(ControlPosPaddleHot, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPosPaddleAmbient() { VERIFY(thread_ControlPosPaddleAmbient = AfxBeginThread(ControlPosPaddleAmbient, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPosRotor() { VERIFY(thread_ControlPosRotor = AfxBeginThread(ControlPosRotor, this, THREAD_PRIORITY_NORMAL)); }

    void StartControlTempPaddleCold() { VERIFY(thread_ControlTempPaddleCold = AfxBeginThread(ControlTempPaddleCold, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlTempPaddleHot() { VERIFY(thread_ControlTempPaddleHot = AfxBeginThread(ControlTempPaddleHot, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlTempRotorCold() { VERIFY(thread_ControlTempRotorCold = AfxBeginThread(ControlTempRotorCold, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlTempRotorHot() { VERIFY(thread_ControlTempRotorHot = AfxBeginThread(ControlTempRotorHot, this, THREAD_PRIORITY_NORMAL)); }

    void StartControlPowerPaddleCold() { VERIFY(thread_ControlPowerPaddleCold = AfxBeginThread(ControlPowerPaddleCold, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPowerPaddleHot() { VERIFY(thread_ControlPowerPaddleHot = AfxBeginThread(ControlPowerPaddleHot, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPowerRotorCold() { VERIFY(thread_ControlPowerRotorCold = AfxBeginThread(ControlPowerRotorCold, this, THREAD_PRIORITY_NORMAL)); }
    void StartControlPowerRotorHot() { VERIFY(thread_ControlPowerRotorHot = AfxBeginThread(ControlPowerRotorHot, this, THREAD_PRIORITY_NORMAL)); }
#endif

    float Counts2Celcius(int Counts);
    int Celcius2Counts(float Celcius);
    float HexstrToCelcius(char *HexStr);

/*
    sem_t SetPositionPaddleCold_count;
    sem_t SetPositionPaddleHot_count;
    sem_t SetPositionPaddleAmbient_count;
    sem_t SetPositionRotor_count;
    sem_t SetTemperaturePaddleCold_count;
    sem_t SetTemperaturePaddleHot_count;
    sem_t SetTemperatureRotorCold_count;
    sem_t SetTemperatureRotorHot_count;
*/
    //void StartControlPaddle3() { pthread_create(&thread_ControlPaddle3, NULL, &Calibrator::wrapperControlPaddle3, &parm); }
    //void StartControlPaddle7() { pthread_create(&thread_ControlPaddle7, NULL, &Calibrator::wrapperControlPaddle7, &parm); }

private:
#ifdef __unix__
    pthread_t thread_Monitor;
    pthread_t thread_ControlPosPaddleCold;
    pthread_t thread_ControlPosPaddleHot;
    pthread_t thread_ControlPosPaddleAmbient;
    pthread_t thread_ControlPosRotor;
    pthread_t thread_ControlTempPaddleCold;
    pthread_t thread_ControlTempPaddleHot;
    pthread_t thread_ControlTempRotorCold;
    pthread_t thread_ControlTempRotorHot;
    pthread_t thread_ControlPowerPaddleCold;
    pthread_t thread_ControlPowerPaddleHot;
    pthread_t thread_ControlPowerRotorCold;
    pthread_t thread_ControlPowerRotorHot;
#endif

    void ReadComLine(int Timeout);
    void Monitor();
    void ControlPosPaddleCold();
    void ControlPosPaddleHot();
    void ControlPosPaddleAmbient();
    void ControlPosRotor();

    void ControlTempPaddleCold();
    void ControlTempPaddleHot();
    void ControlTempRotorCold();
    void ControlTempRotorHot();
    void ControlPowerPaddleCold();
    void ControlPowerPaddleHot();
    void ControlPowerRotorCold();
    void ControlPowerRotorHot();
    //void CalibratorGoHome(void);
    //void SetCalibratorPos(unsigned char Channel, unsigned short Position);
    //void SetCalibratorSpeed(unsigned char Channel, unsigned short Speed);
    //void SetCalibratorAcceleration(unsigned char Channel, unsigned short Acceleration);
    //unsigned short GetCalibratorPos(unsigned char Channel);
    //void IlluminatorControl(unsigned short OnOff, unsigned short PowerLevel);
    //void SetNewHome(unsigned char Channel, unsigned short StartupPos);
#ifdef __unix__
    static void* wrapperMonitor(void* thisPtr) { ((Calibrator*) thisPtr)->Monitor(); return(NULL); }
    static void* wrapperControlPosPaddleCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPosPaddleCold(); return(NULL); }
    static void* wrapperControlPosPaddleHot(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPosPaddleHot(); return(NULL); }
    static void* wrapperControlPosPaddleAmbient(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPosPaddleAmbient(); return(NULL); }
    static void* wrapperControlPosRotor(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPosRotor(); return(NULL); }
    static void* wrapperControlTempPaddleCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlTempPaddleCold(); return(NULL); }
    static void* wrapperControlTempPaddleHot(void* thisPtr) { ((Calibrator*) thisPtr)->ControlTempPaddleHot(); return(NULL); }
    static void* wrapperControlTempRotorCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlTempRotorCold(); return(NULL); }
    static void* wrapperControlTempRotorHot(void* thisPtr) { ((Calibrator*) thisPtr)->ControlTempRotorHot(); return(NULL); }
    //static void* wrapperControlTempPaddleCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlTempPaddleCold(); return(NULL); }

    static void* wrapperControlPowerPaddleCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPowerPaddleCold(); return(NULL); }
    static void* wrapperControlPowerPaddleHot(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPowerPaddleHot(); return(NULL); }
    static void* wrapperControlPowerRotorCold(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPowerRotorCold(); return(NULL); }
    static void* wrapperControlPowerRotorHot(void* thisPtr) { ((Calibrator*) thisPtr)->ControlPowerRotorHot(); return(NULL); }
#else

    static UINT Monitor(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->Monitor();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPosPaddleCold(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPosPaddleCold();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPosPaddleHot(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPosPaddleHot();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPosPaddleAmbient(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPosPaddleAmbient();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPosRotor(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPosRotor();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlTempPaddleCold(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlTempPaddleCold();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlTempPaddleHot(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlTempPaddleHot();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlTempRotorCold(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlTempRotorCold();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlTempRotorHot(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlTempRotorHot();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPowerPaddleCold(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPowerPaddleCold();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPowerPaddleHot(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPowerPaddleHot();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPowerRotorCold(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPowerRotorCold();
        ::PostQuitMessage(0);
        return 0;
    }
    static UINT ControlPowerRotorHot(LPVOID pParam)
    {
        AfxGetThread()->m_bAutoDelete = FALSE;
        ((Calibrator*)pParam)->ControlPowerRotorHot();
        ::PostQuitMessage(0);
        return 0;
    }
#endif
};

#endif // CALIBRATOR_H
