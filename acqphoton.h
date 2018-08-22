//////////////////////////////////////////////////////////////////////
//
//	AcqPhoton.h : header file
//
//////////////////////////////////////////////////////////////////////
#ifndef ACQPHOTON_H
#define ACQPHOTON_H

//#include <unistd.h> //for usleep

#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
#define BYTE unsigned char
#define DWORD unsigned long
//#include <pthread.h>
//#include <semaphore.h>
//#include "cssl_lib.h"

//#define IMAGEWIDTH 324L
//#define IMAGEHEIGHT 256L
#define IMAGEWIDTH 384L
#define IMAGEHEIGHT 287L
#define IMAGESIZE 110208L

#pragma once

class CAcqPhoton
{
public:
    CAcqPhoton();
    virtual ~CAcqPhoton();

    int InitCamera(unsigned *width, unsigned *height);
    void GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
    void StartCamera();
    void StopCamera();
    int ResetCamera();
    void ShutdownCamera();
    BOOL AutoFFC();
    BOOL ManualFFC();
    BOOL DoFFC();
private:
};

#endif // ACQPHOTON_H
