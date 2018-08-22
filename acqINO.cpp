//////////////////////////////////////////////////////////////////////
//
//  acqINO.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "acqINO.h"
#include <memory.h>

#include "MICROXCAM-384I_CAPI.h"

/*#include "PvSampleUtils.h"
#include <PvDevice.h>
#include <PvStream.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvSerialPortIPEngine.h>


//:O#define PV_GUI_NOT_AVAILABLE
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE
#include "defines.h"
*/
#include <QMutex>

#define uint8_t unsigned char
#define uint32_t unsigned int

#define BUFFER_COUNT ( 16 )

//#define IMAGEWIDTH 320L
//#define IMAGEHEIGHT 240L
//#define IMAGESIZE 76800L
///
/// Function Prototypes
///

#ifdef OTHERSDK
#ifdef PV_GUI_NOT_AVAILABLE
const PvDeviceInfo *SelectDevice( PvSystem *aPvSystem );
#else
const PvDeviceInfo *SelectDevice( PvDeviceFinderWnd *aDeviceFinderWnd );
#endif // PV_GUI_NOT_AVAILABLE
#endif

/*PvDeviceInfo *lDeviceInfo = NULL;

PvDevice *ConnectToDevice( const PvDeviceInfo *aDeviceInfo );
PvStream *OpenStream( const PvDeviceInfo *aDeviceInfo );
void ConfigureStream( PvDevice *aDevice, PvStream *aStream );
PvPipeline* CreatePipeline( PvDevice *aDevice, PvStream *aStream );
void AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline );

PvDevice lDevice;   // = NULL;
PvStream lStream;   // = NULL;
PvPipeline *lPipeline = NULL;
PvGenCommand *lStart;
PvGenCommand *lStop;
PvGenFloat *lFrameRate;
PvGenFloat *lBandwidth;
PvSerialPortIPEngine lPort;
PvGenParameterArray *lDeviceParams;
*/
//extern char LogMessage[512];
extern int CameraSn;
extern unsigned short m_nPhotonBackgroundBuffer[];

unsigned long FrameCount=0;
unsigned long LastFrameCount=0;


unsigned short pushData2[IMAGESIZE * 2];

QMutex AcqMutex;

//////////////////////////////////////////////////////////////////////
// CAcqINO constructor
CAcqINO::CAcqINO()
{
    //    sem_init (&m_hDataReadyToReadEvent_count, 0, 0);
#ifdef CHECK_FRAME_NUMBER
    FrameNumAcqChk=0;
#endif
}

//////////////////////////////////////////////////////////////////////

CAcqINO::~CAcqINO()
{
}




int CAcqINO::CameraCallback(unsigned short *pushData, unsigned int iW, unsigned int iH, void *pThis)
{
    AcqMutex.lock();
    memcpy(pushData2, pushData, IMAGESIZE*sizeof(unsigned short));
    AcqMutex.unlock();
    FrameCount++;


    /*CCAPI_UserInterfaceSampleDlg* me = reinterpret_cast<CCAPI_UserInterfaceSampleDlg*>(pThis);

    //Convert RAW to 8 bits for display
    double dConst01   = 1.00/65536.0;
    for(unsigned int y=0;y<iH;y++)
    {
        unsigned char  *pImageD = me->m_puchImageBuf  + (y*iW);
        unsigned short *pImageS = pushData        + (y*iW);
        for(unsigned int x=0;x<iW;x++)
        {
            pImageD[x] = (unsigned char)(255.0 * ((double)pImageS[x] * dConst01));
        }
    }
    me->m_DisplayCam.SetImageBytes(me->m_puchImageBuf,NULL,iW,iH,8);*/

    return 0;
}

void CAcqINO::OnBnClickedBtnStartStopCallback()
{
bool m_bCallbackInit = false;
    if(!m_bCallbackInit)
    {
        int iRet = fn_SetCameraCallback(CameraCallback,this);
        if(iRet == eNO_ERROR)
        {
            m_bCallbackInit = TRUE;
            //GetDlgItem(IDC_BTN_START_STOP_CALLBACK)->SetWindowText("STOP Callback");
        }
        else
        {
            CString strMsg;
            //strMsg.Format("Get Image Error --> %s","Image buffer is NULL");
            //MessageBox(strMsg,"Error !",MB_OK+MB_ICONSTOP);
        }
    }
    else
    {
        fn_SetCameraCallback(NULL,NULL);
        m_bCallbackInit = FALSE;
        //GetDlgItem(IDC_BTN_START_STOP_CALLBACK)->SetWindowText("Start Callback");
    }
}






//////////////////////////////////////////////////////////////////////

// Send data to camera and read reply
void CameraComm( Int16 command, Int16 param1, Int16 param2, Int16 param3, uInt16 *Data)
{
    long ChkSum;
    long XORChk;
    DWORD dwLen;
    unsigned char cmd[11]={0,0,0,0,0,0,0,0,0,0,0};
    uint8_t inStr[512];
    uint32_t numBytes;

    ChkSum = (command&0xFF) + ((command>>8)&0xFF) + (param1&0xFF) + ((param1>>8)&0xFF) + (param2&0xFF) + ((param2>>8)&0xFF) + (param3&0xFF) + ((param3>>8)&0xFF);
    XORChk = ChkSum;


    // Words are swapped here
    cmd[1] = command&0xFF;
    cmd[2] = (command>>8)&0xFF;
    cmd[3] = param1&0xFF;
    cmd[4] = (param1>>8)&0xFF;
    cmd[5] = param2&0xFF;
    cmd[6] = (param2>>8)&0xFF;
    cmd[9] = XORChk & 0xFF;
    cmd[10] = (XORChk>>8) & 0xFF;
#ifdef PLEORA
    // Send Command
    dwLen = 11;
    lPort.Write(cmd, dwLen, numBytes);

    // Read Reply
    numBytes = 32;
    lPort.Read(inStr, 512, numBytes, 0);

    memcpy( Data, &cmd[6], sizeof(uInt16) );
#endif
}


int CAcqINO::InitCamera(unsigned int *AcqWinWidth, unsigned int *AcqWinHeight)
{
    int m_iCamW,m_iCamH;
    unsigned char  *m_puchImageBuf;

    int iRet = fn_Initialize(false, NULL);

    if(iRet == eNO_ERROR) {

        //Get Image Size...
        iRet = fn_GetImageSize(m_iCamW,m_iCamH);
        m_puchImageBuf = new unsigned char [m_iCamW*m_iCamH];

        *AcqWinWidth = m_iCamW;
        *AcqWinHeight = m_iCamH;

        OnBnClickedBtnStartStopCallback();

//        char RplFile[128];
//        strcpy(RplFile, "C:\\Program Files (x86)\\INO\\MICROXCAM-384I\\!RPL\\RPL MXC-002 10-65deg High Dynamic Range 2017-05-02.rpl");
//        iRet = fn_LoadRPLFile (RplFile);

//        Sleep(500);

//        int iNbrFrame = 16;
//        bool bUseInternalShutter = false;
//        bool bVerbose = true;

//        iRet = fn_TakeOffset (iNbrFrame , bUseInternalShutter, bVerbose);

/*        unsigned short *pImgBuff16 = new unsigned short[m_iCamW*m_iCamH];
        //memset(pImgBuff16,0xAA,m_iCamW*m_iCamH*2);
        int iRet = fn_GetCameraImage(pImgBuff16,m_iCamW*m_iCamH);
        if(iRet == eNO_ERROR)
        {        */
    }


/*
#ifdef PLEORAXX
    PvResult lResult;

    const PvDeviceInfo *lDeviceInfo = NULL;
    *AcqWinWidth = 384;
    *AcqWinHeight = 287;

    //PV_SAMPLE_INIT();

#ifdef PV_GUI_NOT_AVAILABLE
    PvSystem lSystem;
    lDeviceInfo = PvSelectDevice( lSystem );
#else
    PvDeviceFinderWnd lDeviceFinderWnd;
    lResult = lDeviceFinderWnd.ShowModal();
    if( !lResult.IsOK() )
    {
        cout << "No device selected." << endl;
        return false;
    }
    lDeviceInfo = lDeviceFinderWnd.GetSelected();
#endif // PV_GUI_NOT_AVAILABLE


    if ( lDeviceInfo == NULL ) return(-1);

#ifdef OTHERSDK
    lDevice = ConnectToDevice( lDeviceInfo );
    if ( lDevice == NULL ) return(-2);

    lStream = OpenStream( lDeviceInfo );
    if ( lStream == NULL ) return(-3);

    lPipeline = NULL;
    ConfigureStream( lDevice, lStream );
    lPipeline = CreatePipeline( lDevice, lStream );
    if( lPipeline  == NULL ) return(-4);
#endif



    //    PvDevice lDevice;
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    {
        return false;
    }


    // Get device parameters need to control streaming
    lDeviceParams = lDevice.GetGenParameters();


    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();


    // Open stream - have the PvDevice do it for us
    lStream.Open( lDeviceInfo->GetIPAddress() );


    // Create the PvPipeline object
    lPipeline = new PvPipeline( &lStream );

    // Reading payload size from device
    PvInt64 lSize = 0;
    lDeviceParams->GetIntegerValue( "PayloadSize", lSize );

    // Set the Buffer size and the Buffer count
    lPipeline->SetBufferSize( static_cast<PvUInt32>( lSize ) );
    lPipeline->SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );


    // Camera doesn't start the first time, so start it here, before the starts fctn is called.
    StartCamera();
    //    Sleep(300);
    //    ShutdownCamera();



    // Init Serial
#define UART0
#define SPEED ( "Baud115200" )
#define STOPBITS ( "One" )
#define PARITY ( "None" )



    // Configure serial port - this is done directly on the device GenICam interface, not
    // on the serial port object!
#ifdef UART0
    lDeviceParams->SetEnumValue( "Uart0BaudRate", SPEED );
    lDeviceParams->SetEnumValue( "Uart0NumOfStopBits", STOPBITS );
    lDeviceParams->SetEnumValue( "Uart0Parity", PARITY );

    // For this test to work without attached serial hardware we enable the port loop back
    lDeviceParams->SetBooleanValue( "Uart0Loopback", true );
#endif
#ifdef BULK0
    lDeviceParams->SetEnumValue( "BulkSelector", "Bulk0" );
    lDeviceParams->SetEnumValue( "BulkMode", "UART" );
    lDeviceParams->SetEnumValue( "BulkBaudRate", SPEED );
    lDeviceParams->SetEnumValue( "BulkNumOfStopBits", STOPBITS );
    lDeviceParams->SetEnumValue( "BulkParity", PARITY );

    // For this test to work without attached serial hardware we enable the port loop back
    lDeviceParams->SetBooleanValue( "BulkLoopback", false );
#endif // BULK0


    //PvResult lResult;

    // Open serial port
#ifdef UART0
    lResult = lPort.Open( &lDevice, PvIPEngineSerial0 );
#endif // UART0
#ifdef BULK0
    lResult = lPort.Open( &lDevice, PvIPEngineSerialBulk0 );
#endif // BULK0

    if ( !lResult.IsOK() )
    {
        //cout << "Unable to open serial port on device: " << lResult.GetCodeString().GetAscii() << " " <<  lResult.GetDescription().GetAscii() << endl;
        return(-1);
    }

    // Make sure the PvDeviceSerialPort receive queue is big enough
    lPort.SetRxBufferSize( 512 );
#endif
*/
    return(0);
}

//////////////////////////////////////////////////////////////////////

int CAcqINO::GetCameraFrame(unsigned short *m_nPhotonFrameBufferL)
{
    unsigned short m_nPhotonFrameBuffer2[IMAGESIZE];

    while(FrameCount == LastFrameCount) {
        Sleep(5);
    }
    LastFrameCount = FrameCount;

    AcqMutex.lock();
    memcpy(m_nPhotonFrameBufferL, pushData2, IMAGESIZE*sizeof(unsigned short));
    AcqMutex.unlock();

/*#ifdef PLEORA
    uint32_t lWidth = 0, lHeight = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;
    PvBuffer *lBuffer = NULL;
    PvResult lOperationResult;
    PvPayloadType lType;

    // Retrieve next buffer
    PvResult lResult = lPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );

    if ( lResult.IsOK() ) {
        if ( lOperationResult.IsOK() ) {
            //
            // We now have a valid buffer. This is where you would typically process the buffer.
            // -----------------------------------------------------------------------------------------

            lFrameRate->GetValue( lFrameRateVal );
            lBandwidth->GetValue( lBandwidthVal );

            // If the buffer contains an image, display width and height.
            lType = lBuffer->GetPayloadType();

            if ( lType == PvPayloadTypeImage ) {
                // Get image specific buffer interface.
                PvImage *lImage = lBuffer->GetImage();
                uint8_t *MyDP = lImage->GetDataPointer();
                memcpy(m_nPhotonFrameBuffer2, MyDP, IMAGESIZE*2);

                // Read width, height.
                lWidth = lImage->GetWidth();
                lHeight = lImage->GetHeight();
            } else {
                //cout << " (buffer does not contain image)";
                return(-1);
            }
        } else {
            // Non OK operational result
            return(-2);
        }

        // Release the buffer back to the pipeline
        lPipeline->ReleaseBuffer( lBuffer );
    } else {
        // Retrieve buffer failure
        return(-3);
    }
#endif*/

#ifdef DONTDOTHIS
    long AdjustedValue;
#define BASEVALUE 10000
    if( (m_nPhotonBackgroundBuffer[0]!=0) && (m_nPhotonBackgroundBuffer[100]!=0) ) {
        for(int i=0 ; i<IMAGESIZE ; i++) {
            AdjustedValue = (long)m_nPhotonFrameBuffer2[i] - (long)m_nPhotonBackgroundBuffer[i] + BASEVALUE;
            m_nPhotonFrameBuffer2[i] = (unsigned short)AdjustedValue;
        }
    }
    memcpy(m_nPhotonFrameBufferL, m_nPhotonFrameBuffer2, IMAGESIZE*2);
#endif
    return(0);
}

//////////////////////////////////////////////////////////////////////

int CAcqINO::StartCamera()
{    
#ifdef PLEORA
    PvString Parm;
    PvGenParameter *lParameter;
    PvGenInteger *lWidthParameter;
    PvGenInteger *lHeightParameter;
    //PvGenBoolean *lPixValidParameter;
    PvGenEnum *lTestImgParameter;
    PvGenEnum *lPixFormatParameter;
    //return(0);


    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Note: the pipeline must be initialized before we start acquisition
    lPipeline->Start();

    // Get stream parameters
    PvGenParameterArray *lStreamParams = lStream.GetParameters();

    // Map a few GenICam stream stats counters
    lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRate" ) );
    lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "Bandwidth" ) );


    // Set Parameters
    lParameter = lDeviceParams->Get( "Width" );
    lWidthParameter = dynamic_cast<PvGenInteger *>( lParameter );
    lWidthParameter->SetValue( IMAGEWIDTH );

    lParameter = lDeviceParams->Get( "Height" );
    lHeightParameter = dynamic_cast<PvGenInteger *>( lParameter );
    lHeightParameter->SetValue( IMAGEHEIGHT );

    lParameter = lDeviceParams->Get( "PixelFormat" );
    lPixFormatParameter = dynamic_cast<PvGenEnum *>( lParameter );
    Parm="Mono16";
    lPixFormatParameter->SetValue( Parm );

    lParameter = lDeviceParams->Get( "TestImageSelector" );
    lTestImgParameter = dynamic_cast<PvGenEnum *>( lParameter );
    Parm="Off";
    lTestImgParameter->SetValue( Parm );

    //    lParameter = lDeviceParams->Get( "PixelBusDataValidEnabled" );
    //    lPixValidParameter = dynamic_cast<PvGenBoolean *>( lParameter );
    //    lPixValidParameter->SetValue( true);


    //    lParameter = lDeviceParams->Get( "PixelSize" );
    //    PvGenEnum *lPixSizeParameter = dynamic_cast<PvGenEnum *>( lParameter );
    //    Parm="Bpp14";
    //    lPixSizeParameter->SetValue( Parm );

    //    lParameter = lDeviceParams->Get( "TestImageSelector" );
    //    PvGenEnum *lTestImgParameter = dynamic_cast<PvGenEnum *>( lParameter );
    //    Parm="Off";
    //    lTestImgParameter->SetValue( Parm );

    //    lParameter = lDeviceParams->Get( "ConnectionSpeed" );
    //    lConnectionSpeedParameter = dynamic_cast<PvGenEnum *>( lParameter );
    //    Parm="HighSpeed";
    //    lConnectionSpeedParameter->SetValue( Parm );


    // Enable streaming and send the AcquisitionStart command
    //lDevice->StreamEnable();
    //lStart->Execute();

    lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );
    lDeviceParams->ExecuteCommand( "AcquisitionStart" );
#endif
    return(0);
}

//////////////////////////////////////////////////////////////////////

void CAcqINO::ShutdownCamera()
{
#ifdef PLEORA
    lDeviceParams->ExecuteCommand( "AcquisitionStop" );

    // Stop the pipeline
    lPipeline->Stop();

    lStream.Close();
    lDevice.Disconnect();

    lPort.Close();
#endif
}


//////////////////////////////////////////////////////////////////////

//
void CAcqINO::AutoFFC()
{
    Int16 command, param1, param2, param3;
    uInt16 Data;

    command=4;		// Write Register
    param1=0xF;		// Write to AutoDUC
    param2=0x12C;	// Number of seconds between AutoDuc's
    param3=0;		// Always 0

    CameraComm( command, param1, param2, param3, &Data );
}

//////////////////////////////////////////////////////////////////////

//
void CAcqINO::ManualFFC()
{
    Int16 command, param1, param2, param3;
    uInt16 Data;

    // *** Set NUC interval ***
#define NUC_DELAY 0xFFFF;	// Number of seconds ()

    command=4;		// Write Register
    param3=0;		// Always 0

    // FPA Temp Interval for Cal
    param1=0xC;
    param2=NUC_DELAY;	// Number of seconds (25200 seconds - 7 hours)
    CameraComm( command, param1, param2, param3, &Data );

    // Optics Temp Interval for Cal
    param1=0xD;
    param2=NUC_DELAY;	// Number of seconds (25200 seconds - 7 hours)
    CameraComm( command, param1, param2, param3, &Data );

    // Chip Temp Interval for Cal
    param1=0xE;
    param2=NUC_DELAY;	// Number of seconds (25200 seconds - 7 hours)
    CameraComm( command, param1, param2, param3, &Data );

    // Cal timer duration
    param1=0xF;		// Write to AutoDUC
    param2=NUC_DELAY;	// Number of seconds between AutoDuc's (25200 seconds - 7 hours)
    CameraComm( command, param1, param2, param3, &Data );



    // *** Disable NUC ***
#define NUC_STATE 0

    command=0x60C;		// Write memory
    param3=0;		// Always 0

    param1=0x322;	// EE_SCHEDULE_NUC_ENABLE
    param2=NUC_STATE;		// 0=off 1=on
    CameraComm( command, param1, param2, param3, &Data );

    param1=0x324;	// EE_SCHEDULE_FPA_TEMP_ENABLE
    param2=NUC_STATE;		// 0=off 1=on
    CameraComm( command, param1, param2, param3, &Data );

    param1=0x326;	// EE_SCHEDULE_OPTICS_TEMP_ENABLE
    param2=NUC_STATE;		// 0=off 1=on
    CameraComm( command, param1, param2, param3, &Data );

    param1=0x328;	// EE_SCHEDULE_CHIP_TEMP_ENABLE
    param2=NUC_STATE;		// 0=off 1=on
    CameraComm( command, param1, param2, param3, &Data );
}

//////////////////////////////////////////////////////////////////////

void CAcqINO::DoFFC()
{
    Int16 command, param1, param2, param3;
    uInt16 Data;

    ManualFFC();
    Sleep(500);  // Needed?

    AglcOff();		// Turn off automatic gain/level control
    Sleep(500);  // Needed?

    command=0xE0;	// Initiate Calibration
    param1=1;		// Do NUC and offset
    param2=0;		// Always 0
    param3=0;		// Always 0
    CameraComm( command, param1, param2, param3 , &Data);
}

//////////////////////////////////////////////////////////////////////



void CAcqINO::ExternalMode(short Action)   //Action=0, internal, Action=1, external
{
}


void CAcqINO::AglcOff()
{
    Int16 command, param1, param2, param3=0; // Always 0
    uInt16 Data;

    command=4;		// Write Register
    param1=0x11;	// Register 0x11
    param2=0x6208;	// Manual gain/level control (See ITCPearl-Manual-EN.pdf page 74)

    param2=0x6238;	// Manual gain/level control (See ITCPearl-Manual-EN.pdf page 74)
    CameraComm( command, param1, param2, param3, &Data );
}

void CAcqINO::AglcOn()
{
    Int16 command, param1, param2, param3=0; // Always 0
    uInt16 Data;

    command=4;		// Write Register
    param1=0x11;	// Register 0x11
    param2=0x62C8;	// Automatic gain/level control (See ITCPearl-Manual-EN.pdf page 74)
    CameraComm( command, param1, param2, param3, &Data );
}


void Swap4(char *inbuf, char *outbuf)
{
    outbuf[0] = inbuf[3];
    outbuf[1] = inbuf[2];
    outbuf[2] = inbuf[1];
    outbuf[3] = inbuf[0];
}

void Swap2(char *inbuf, char *outbuf)
{
    outbuf[0] = inbuf[1];
    outbuf[1] = inbuf[0];
    outbuf[2] = 0;
    outbuf[3] = 0;
}

int CAcqINO::GetSN()
{
    return(0);
}

//float CAcqINO::GetTemperature()
//{
//}

void CAcqINO::GetPartNumber()
{
}


void CAcqINO::SetIntegrationTime(int ITime)
{
}

void CAcqINO::SetFilter(int FilterNum)
{
}

void CAcqINO::GetStats(float *IntegTime, float *FrameRate, float *FPATemperature)
{
    Int16 command, param1, param2, param3;
    uInt16 Data;
    int CameraTemperature;

    // Get Temperature
    command=5;		// Read Register
    param1=0x1E;	// Number 0x1E
    param2=0;		// Always 0
    param3=0;		// Always 0
    CameraComm( command, param1, param2, param3 , &Data);
    *FPATemperature = Data;
    //Sleep(500);  // Needed?

    // Get Integration time
    command=5;		// Read Register
    param1=0x4;		// Number 0x4
    param2=0;		// Always 0
    param3=0;		// Always 0
    CameraComm( command, param1, param2, param3 , &Data);
    *IntegTime = Data;
}
