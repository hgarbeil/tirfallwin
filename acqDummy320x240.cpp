//////////////////////////////////////////////////////////////////////
//
//  AcqDummy320x240.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////
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

#include "acqDummy320x240.h"

/* Application & library headers */

#include "defines.h"
#include "windows.h"

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
CAcqDummy320x240::CAcqDummy320x240()
{
}

//////////////////////////////////////////////////////////////////////

CAcqDummy320x240::~CAcqDummy320x240()
{
}



//////////////////////////////////////////////////////////////////////

int CAcqDummy320x240::InitCamera(uInt32 *width, uInt32 *height)
{
    // MUST return width and height.
    *width = 320;
    *height = 240;

    return(true);
}

//////////////////////////////////////////////////////////////////////

bool CAcqDummy320x240::GetCameraFrame(unsigned short *m_nPhotonFrameBuffer)
{
    static unsigned short Value=0;
    unsigned short *pBuffer;
    int Steps = 19;

    pBuffer = m_nPhotonFrameBuffer;
    FrameNum+=20;
    if(FrameNum>0x3FFF) FrameNum = 0;


    for(int j=0 ; j<320L*240L / (Steps+1) ; j++) {
        *pBuffer = FrameNum;    // This makes the top bar slowly increas in brightness
        pBuffer++;
    }

    for(int i=0 ; i<Steps ; i++) {
        Value = i * (0x3FFF/Steps+1);
        for(int j=0 ; j<320L*240L / (Steps+1) ; j++) {
            *pBuffer = Value;
            pBuffer++;
        }
    }

    COSMOS_USLEEP(33000);
    //COSMOS_USLEEP(8000);

    return(true);
}


//////////////////////////////////////////////////////////////////////

/*void CAcqDummy320x240::StartCamera()
{
}*/

//////////////////////////////////////////////////////////////////////

void CAcqDummy320x240::ShutdownCamera()
{
    //return(0);
}


//////////////////////////////////////////////////////////////////////

void CAcqDummy320x240::ResetCamera()
{
    //return(true);
}


//////////////////////////////////////////////////////////////////////

// This will set AutoDUC to 5 minutes (0x12C seconds)
void CAcqDummy320x240::AutoFFC(/*bool FfcState*/)
{
    //return(true);
}

/* //////////////////////////////////////////////////////////////////////

// This will set AutoDUC to a long time
bool CAcqDummy320x240::ManualFFC()
{
    return(true);
}*/

//////////////////////////////////////////////////////////////////////

void CAcqDummy320x240::DoFFC()
{
    //return(true);
}

/*void CAcqDummy320x240::AglcOff()
{
}
*/

// Use XOR here, if we get nothing in return, then Firware version = 0.0.0 and then we assume never use the xor in CameraComm()
/*void CAcqDummy320x240::GetFirmwareVer(char *Ver)
{
}*/


/* // Send data to camera and read reply
BYTE* CAcqDummy320x240::CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3)
{
    BYTE *b;
    return(b);
}*/
