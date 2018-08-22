//////////////////////////////////////////////////////////////////////
//
//	acqPleora320.h : header file
//
//////////////////////////////////////////////////////////////////////


//#include <unistd.h> //for usleep

#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
#define BYTE unsigned char
#define DWORD unsigned long

#define IMAGEWIDTH 384L
#define IMAGEHEIGHT 288L
#define IMAGESIZE 110592L
//#define IMAGEWIDTH 324L
//#define IMAGEHEIGHT 256L
//#define IMAGESIZE 82944L


#pragma once


class CAcqINO
{
public:
    CAcqINO();
    virtual ~CAcqINO();

    char Status[256];
    bool PortOpen;

    int InitCamera(unsigned int *AcqWinWidth, unsigned int *AcqWinHeight);
    int GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
    //BYTE* CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3);
    //void AglcOff();
    //void GetFirmwareVer(char *Ver);
    int StartCamera();
    void ShutdownCamera();

    //bool ResetCamera();
    void AutoFFC();
    void ManualFFC();
    void DoFFC();
    void ExternalMode(short Action);
	void AglcOff();
	void AglcOn();
    int GetSN();
    //float GetTemperature();
    void GetPartNumber();
	void SetIntegrationTime(int ITime);
	void SetFilter(int FilterNum);

protected:
    static int CameraCallback(unsigned short *pushData, unsigned int iW, unsigned int iH, void *pThis);

private:
    //HANDLE m_hEventGrab;

public:
    void GetStats(float *IntegTime, float *FrameRate, float *FPATemperature);
    void OnBnClickedBtnStartStopCallback();
};
