//////////////////////////////////////////////////////////////////////
//
//  AcqDummy.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////
#ifdef GUI
#include <QMessageBox>
#include <QMutex>
#include <QWaitCondition>
#endif
#include <unistd.h> //for usleep
//#include <pthread.h>
#include <semaphore.h>

#include "AcqDummy640.h"

/* Application & library headers */

#include "defines.h"

//extern unsigned short m_nPhotonFrameBuffer[IMAGESIZE];
static unsigned short FrameNum=0;

extern sem_t StartCamera_count;
extern sem_t StopCamera_count;

//////////////////////////////////////////////////////////////////////
// CAcqTau constructor
CAcqTau::CAcqTau()
{
}

//////////////////////////////////////////////////////////////////////

CAcqTau::~CAcqTau()
{
}



//////////////////////////////////////////////////////////////////////

int CAcqTau::InitCamera(uInt32 *width, uInt32 *height)
{
    // MUST return width and height.
    *width = 320;
    *height = 256;
    // -----------------------------

    return(true);
}

//////////////////////////////////////////////////////////////////////

bool CAcqTau::GetCameraFrame(unsigned short *m_nPhotonFrameBuffer)
{
    static unsigned short Value=0;
    unsigned short *pBuffer;
    int Steps = 19;

    pBuffer = m_nPhotonFrameBuffer;
    FrameNum+=20;
    if(FrameNum>0x3FFF) FrameNum = 0;
/*
    memset(pBuffer, 255, 6400);
    pBuffer += 3200;
    memset(pBuffer, 0, 6400);
    pBuffer += 3200;
    memset(pBuffer, 255, 6400);
*/

    for(int j=0 ; j<320L*256L / (Steps+1) ; j++) {
        //*pBuffer = 0x3E80;
        *pBuffer = FrameNum;    // This makes the top bar slowly increas in brightness
        pBuffer++;
    }

    for(int i=0 ; i<Steps ; i++) {
        Value = i * (0x3FFF/Steps+1);
        for(int j=0 ; j<320L*256L / (Steps+1) ; j++) {
            *pBuffer = Value;
            pBuffer++;
        }
    }

    // //usleep(25000);
    //usleep(15000);
    //usleep(1000000);
//usleep(2300);
    //usleep(33000);
usleep(200);

    //usleep(1800);
    //usleep(31000);


    /*    QMutex dummy;
    dummy.lock();
    QWaitCondition waitCondition;
    waitCondition.wait(&dummy, 33);*/

    return(true);
}


//////////////////////////////////////////////////////////////////////

void CAcqTau::StartCamera()
{
}

//////////////////////////////////////////////////////////////////////

int CAcqTau::ShutdownCamera()
{
    //return(0);
}


//////////////////////////////////////////////////////////////////////

bool CAcqTau::ResetCamera()
{
    //return(true);
}


//////////////////////////////////////////////////////////////////////

// This will set AutoDUC to 5 minutes (0x12C seconds)
bool CAcqTau::AutoFFC()
{
    //return(true);
}

/* //////////////////////////////////////////////////////////////////////

// This will set AutoDUC to a long time
bool CAcqTau::ManualFFC()
{
    return(true);
}*/

//////////////////////////////////////////////////////////////////////

bool CAcqTau::DoFFC()
{
    //return(true);
}

/*void CAcqTau::AglcOff()
{
}
*/

// Use XOR here, if we get nothing in return, then Firware version = 0.0.0 and then we assume never use the xor in CameraComm()
void CAcqTau::GetFirmwareVer(char *Ver)
{
}


/* // Send data to camera and read reply
BYTE* CAcqTau::CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3)
{
    BYTE *b;
    return(b);
}*/
