//////////////////////////////////////////////////////////////////////
//
//	FindCameras.h : header file
//
//////////////////////////////////////////////////////////////////////


#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
//#define BYTE unsigned char
//#define DWORD unsigned long

#pragma once


class CFindCameras
{
public:
    CFindCameras();
    virtual ~CFindCameras();

    int FindCameras();

private:
    //HANDLE m_hEventGrab;
};
