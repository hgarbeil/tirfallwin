//////////////////////////////////////////////////////////////////////
//
//	AcqPV640.h : header file
//
//////////////////////////////////////////////////////////////////////


#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
//#define BYTE unsigned char
//#define DWORD unsigned long

#ifdef __unix__
#include <unistd.h> //for usleep
#include "cssl_lib.h"
#endif

#ifdef _WIN32
//#include "windows.h"
#endif

#pragma once


class CAcqPv640
{
public:
    CAcqPv640();
    virtual ~CAcqPv640();

    char Status[256];
    bool PortOpen;

    int InitCamera(unsigned *width, unsigned *height);
	void GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
    //BYTE* CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3);
    //void AglcOff();
    //void GetFirmwareVer(char *Ver);
    void StartCamera();
    void ShutdownCamera();

    //bool ResetCamera();
    void AutoFFC();
    void ManualFFC();
    void DoFFC();
    void ExternalMode(short Action);
    void GetSN();
    int GetTemperature();
    void GetPartNumber();


private:
    //HANDLE m_hEventGrab;
};
