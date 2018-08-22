//////////////////////////////////////////////////////////////////////
//
//	acqmc320.h : header file
//
//////////////////////////////////////////////////////////////////////

#ifdef __unix__
#define Int16 short
#define uInt16 unsigned short
#define BYTE unsigned char
#define DWORD unsigned long
#endif
#define uInt32 unsigned int

#pragma once


class CAcqMc320
{
public:
    CAcqMc320();
    virtual ~CAcqMc320();

	int InitCamera(uInt32 *AcqWinWidth, uInt32 *AcqWinHeight);
    bool GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);    
	void StartCamera();
    void DoFFC();
    void AutoFFC(bool FfcState);
    void RawMode(bool RawState);
    void GetFirmwareVer(char *Ver);
    void ResetCamera();
    void ShutdownCamera();

private:
    //HANDLE m_hEventGrab;
};
