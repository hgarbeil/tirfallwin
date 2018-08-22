//////////////////////////////////////////////////////////////////////
//
//	acqmc320.h : header file
//
//////////////////////////////////////////////////////////////////////


#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
//#define BYTE unsigned char
//#define DWORD unsigned long


#pragma once



class CAcqDummy320x240
{
public:
    CAcqDummy320x240();
    virtual ~CAcqDummy320x240();

	int InitCamera(uInt32 *AcqWinWidth, uInt32 *AcqWinHeight);
    bool GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);    
    //void StartCamera();
    void ShutdownCamera();

    void DoFFC();
    void AutoFFC(/*bool FfcState*/);
    void RawMode(/*bool RawState*/);
    //void GetFirmwareVer(char *Ver);
    void ResetCamera();

private:
};
