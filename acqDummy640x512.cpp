//////////////////////////////////////////////////////////////////////
//
//  AcqDummy640x512.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include "afxwin.h"
#endif
#ifdef GUI
#include <QMessageBox>
#include <QMutex>
#include <QWaitCondition>
#endif
#include "configCosmos.h"
#ifdef __unix__
#include <unistd.h> //for COSMOS_USLEEP
//#include <pthread.h>
#include <semaphore.h>
#endif

#include "acqDummy640x512.h"

/* Application & library headers */

#include "defines.h"

//extern unsigned short m_nPhotonFrameBuffer[];
static unsigned short FrameNum=0;
static unsigned short FrameNumAcqChk=0;

#ifdef __unix__
extern sem_t StartCamera_count;
extern sem_t StopCamera_count;
#else
extern HANDLE StartCamera_count;
extern HANDLE StopCamera_count;
#endif

//////////////////////////////////////////////////////////////////////
// CAcqPearlPC104 constructor
CAcqDummy640x512::CAcqDummy640x512()
{
}

//////////////////////////////////////////////////////////////////////

CAcqDummy640x512::~CAcqDummy640x512()
{
}



//////////////////////////////////////////////////////////////////////

int CAcqDummy640x512::InitCamera(uInt32 *width, uInt32 *height)
{
    // MUST return width and height.
    *width = 640;
    *height = 512;

    return(true);
}

//////////////////////////////////////////////////////////////////////

bool CAcqDummy640x512::GetCameraFrame(unsigned short *m_nPhotonFrameBuffer)
{
    static unsigned short Value=0;
     unsigned short *pBuffer;
     int Steps = 19;

     pBuffer = m_nPhotonFrameBuffer;
     FrameNum++;
     if(FrameNum>0x3FFF) FrameNum = 0;

     for(int j=0 ; j<640L*512L / (Steps+1) ; j++) {
         *pBuffer = FrameNum;    // This makes the top bar slowly increase in brightness
         pBuffer++;
     }

     for(int i=0 ; i<Steps ; i++) {
         Value = i * (0x3FFF/Steps+1);
         for(int j=0 ; j<640L*512L / (Steps+1) ; j++) {
             *pBuffer = Value;
             pBuffer++;
         }
     }

     m_nPhotonFrameBuffer[0] = FrameNumAcqChk;
     if(FrameNumAcqChk<0xFFFF) {
         FrameNumAcqChk++;
     } else {
         FrameNumAcqChk = 0;
     }

     //COSMOS_USLEEP(33000);
     COSMOS_USLEEP(15000);

    return(true);
}


//////////////////////////////////////////////////////////////////////

/*void CAcqDummy640x512::StartCamera()
{
}*/

//////////////////////////////////////////////////////////////////////

void CAcqDummy640x512::ShutdownCamera()
{
    //return(0);
}


//////////////////////////////////////////////////////////////////////

void CAcqDummy640x512::ResetCamera()
{
    //return(true);
}


//////////////////////////////////////////////////////////////////////

// This will set AutoDUC to 5 minutes (0x12C seconds)
void CAcqDummy640x512::AutoFFC(/*bool FfcState*/)
{
    //return(true);
}

/* //////////////////////////////////////////////////////////////////////

// This will set AutoDUC to a long time
bool CAcqDummy640x512::ManualFFC()
{
    return(true);
}*/

//////////////////////////////////////////////////////////////////////

void CAcqDummy640x512::DoFFC()
{
    //return(true);
}

/*void CAcqDummy640x512::AglcOff()
{
}
*/

// Use XOR here, if we get nothing in return, then Firware version = 0.0.0 and then we assume never use the xor in CameraComm()
/*void CAcqDummy640x512::GetFirmwareVer(char *Ver)
{
}*/


/* // Send data to camera and read reply
BYTE* CAcqDummy640x512::CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3)
{
    BYTE *b;
    return(b);
}*/
