//////////////////////////////////////////////////////////////////////
//
//	AcqDummy.h : header file
//
//////////////////////////////////////////////////////////////////////


#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
#define BYTE unsigned char
#define DWORD unsigned long

#pragma once


class CAcqPearlPC104
{
public:
	CAcqPearlPC104();
	virtual ~CAcqPearlPC104();


	int InitCamera(uInt32 *AcqWinWidth, uInt32 *AcqWinHeight);
	void GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
	void WriteRegister( Int16 command, Int16 param1, Int16 param2, Int16 param3);
	void StartCamera();
	int ShutdownCamera();

    bool ResetCamera();
    bool AutoFFC();
    bool ManualFFC();
    bool DoFFC();

private:
    //HANDLE m_hEventGrab;
};
