//////////////////////////////////////////////////////////////////////
//
//	AcqTau.h : header file
//
//////////////////////////////////////////////////////////////////////
#ifndef ACQTAU_H
#define ACQTAU_H

#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int

#ifdef __unix__
#define BYTE unsigned char
#define DWORD unsigned long
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> //for usleep
#endif
#include "cssl_lib.h"

#ifdef _WIN32
//#include "windows.h"
#endif

#pragma once

/*struct type_CameraInfo {
    cssl_t *Serial;
    bool Busy;
    bool PortOpen;
    bool MonitorInitialized;
    bool Terminate;
    float CameraTemperature;
};*/

class CAcqTau
{
public:
    CAcqTau();
    virtual ~CAcqTau();

    //char Status[256];
    //bool m_bPortOpen;

    //bool m_bBusy;
    //bool m_bMonitorInitialized;
    //bool m_bTerminate;
    //float m_nCameraTemperature;

    int InitCamera(unsigned *width, unsigned *height);
	void GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
    //BYTE* CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3);
    //void AglcOff();
    //void GetFirmwareVer(char *Ver);
    void StartCamera();
    void ShutdownCamera();

    //bool ResetCamera();
    //void AutoFFC();
    //void ManualFFC();
    //void DoFFC();
    //void ExternalMode(short Action);
    //void GetSN();
    //float GetTemperature();
    //void GetPartNumber();
    //void Get(float *Temperature);
    //type_CameraInfo parm;
    //void StartMonitorL() { pthread_create(&thread_MonitorL, NULL, &CAcqTau::wrapperMonitorL, &parm); }
private:
    //HANDLE m_hEventGrab;
    //pthread_t thread_MonitorL;
    //void MonitorL();
    //static void* wrapperMonitorL(void* thisPtr) { ((CAcqTau*) thisPtr)->MonitorL(); return(NULL); }
};

#endif // ACQTAU_H
