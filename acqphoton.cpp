//////////////////////////////////////////////////////////////////////
//
//  AcqPhoton.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////

//#include <QMessageBox>
#include "stdafx.h"
/*#include <CyXMLDocument.h>
#include <CyConfig.h>
#include <CyGrabber.h>
#include <CyCameraRegistry.h>
#include <CyCameraInterface.h>
#include <CyImageBuffer.h>
#include <CyDeviceFinder.h>
#include <CyDisplayEx.h>
#include <CyDevice.h>*/

//#include "CyVersion.h"
//#include "CyDevice.h"
//#include "CyGrabber.h"

#include "acqphoton.h"
#include "acqino.h"
#include "tirthreads.h"
#include "defines.h"
//using namespace std;

//#define IMAGEWIDTH 320L
//#define IMAGEHEIGHT 256L
//#define IMAGESIZE 81920L

//#define IMAGEWIDTH 324L
//#define IMAGEHEIGHT 256L
//#define IMAGESIZE 82944L

//extern unsigned short m_nPhotonFrameBuffer[];
//extern unsigned short *BufferFall;
//static unsigned short FrameNum=0;
//static unsigned short FrameNumAcqChk=0;
extern bool ExitProgram;



//int i, j;
//char IPString[256];
//static short int *DisplayBuffPtr;
unsigned short *PhotonFrameBuffPtr;
unsigned short lSizeX, lSizeY, lDecimationX, lDecimationY, lBinningX, lBinningY;

unsigned int lSize = 0;

/*CyPixelTypeID lPixelType;
CyGrabber lGrabber;
CyCameraInterface* lCamera = 0;
CyConfig lConfig;
CyDevice lDevice;
CyResult lResult;
CyImageBuffer lBuffer;*/
int lResult;

int LastCnt, PresentCnt;
char lTemp[32];

//									proc  stat   Func  Byte      CRC      Data    CRC
//									code               count
unsigned char RS232Reset[10] =		 {0x6e, 0, 0, 2,   0, 0, 0xb1,  0xdb,       0,    0};
unsigned char RS232ManShutter[12] =  {0x6e, 0, 0, 0xb, 0, 2, 0x0f,     8, 0, 0, 0,    0};
unsigned char RS232AutoShutter[12] = {0x6e, 0, 0, 0xb, 0, 2, 0x0f,     8, 0, 1, 0x10, 0x21};
unsigned char RS232DoShutter[10] =   {0x6e, 0, 0, 0xc, 0, 0, 0xaa,  0xda,       0,    0};



//////////////////////////////////////////////////////////////////////
// CAcqPhoton constructor
CAcqPhoton::CAcqPhoton()
{
}

//////////////////////////////////////////////////////////////////////

CAcqPhoton::~CAcqPhoton()
{
    //delete lCamera;
}



//////////////////////////////////////////////////////////////////////

int CAcqPhoton::InitCamera(unsigned *width, unsigned *height)
{
    //int iretn;

    *width = IMAGEWIDTH;
    *height = IMAGEHEIGHT;



    char MACAddress[256];
    char IPAddress[256];
    char IPString[256];
    //unsigned short m_nPhotonFrameBuffer[320*256*2];
/*
    CyAdapterID lAdapterID;
    // :O CyAdapterID::GetIdentifier( CyConfig::MODE_DRV, 0, lAdapterID );



    strcpy(MACAddress, "00-11-1C-00-D5-F2");    // Mac address of Photon Adapter
    strcpy(IPAddress, "192.168.1.100");         // IP address of photon adapter
    sprintf( IPString, "[%s]", IPAddress);

    //QMessageBox::information(this, "Title", "Start");

    // (Step 0 sets the IP address of the Photon Adapter)

    // Step 0 (optional): Set the IP address on the module.  Since the
    // XML file may have a forced address, we need to send it to the module.
    // You may skip this step if:
    // - you have a BOOTP server that sets the module's address
    // - if you have a direct connection from an performance driver card
    //   to the iPORT and that your XML file uses an empty address.
    CyDeviceFinder lFinder;
    if ( lFinder.SetIP( CyConfig::MODE_UDP,  // or CyConfig::MODE_UDP
                        lAdapterID,
                        MACAddress, // the MAC address of the module to update
                        IPString )   // The address to set
         != CY_RESULT_OK )
    {
        // error
        return(-2);
    }


    // Step 1:  Open the configuration file.  We create a XML
    // document with the name of the file as parameter.
    CyXMLDocument lDocument( "Photon324x256Config.xml" );
    if ( lDocument.LoadDocument() != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 1 failed");
        return(-3);
    }



    // Step 2a: Create a configuration object and fill it from the XML document.
    //CyConfig lConfig;
    if ( lConfig.LoadFromXML( lDocument ) != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 2 failed");
        return(-4);
    }

    // Step 2b:  We could also fill the config by calling AddDevice and
    // the Set... functions to force values.
    //     lConfig.AddDevice();
    //     lConfig.SetAccessMode( CyConfig::MODE_DRV );
    //     lConfig.SetAddress( "" ); // direct connection
    //     lConfig.SetAdapterID( lAdapterID );  // first Pleora Ethernet card
    //     lConfig.SetDeviceType( "Standard CameraLink Camera" );
    //     lConfig.SetName( "DeviceName" );


    // Step 3a: Set the configuration on the entry to connect to.
    // In this case, we only have one entry, so index 0, is good.
    // Select the device you want to use
    lConfig.SetIndex( 0 );


    // Step 3b: Set the configuration on the entry to connect to.
    // Here, we know the name of the device, so we search for it.
    // if ( lConfig.FindDeviceByName( "DeviceName" ) != CY_RESULT_OK )
    // {
    //     // error
    //     return 1;
    // }



    // Step 4: Connect to the grabber.  It will use the currently
    // selected entry in the config object, hence step 3.
    //CyGrabber lGrabber;
    if ( lGrabber.Connect( lConfig ) != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 4 failed");
        return(-5);
    }



    // Step 5: Create a camera object on top of the grabber.  This camera
    // object takes care of configuring both the iPORT and the camera.

    // Find the camera type from the configuration
    char lCameraType[128];
    lConfig.GetDeviceType( lCameraType, sizeof( lCameraType ) );

    // Find the camera in the registry
    CyCameraRegistry lReg;
    if ( lReg.FindCamera( lCameraType ) != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 5 failed");
        return(-6);
    }

    // Create the camera.  The previous operation placed the registry
    // internal settings on the found camera.  The next step will create
    // a camera object of that type.
    //CyCameraInterface* lCamera = 0;
    if ( lReg.CreateCamera( &lCamera, &lGrabber ) != CY_RESULT_OK )
    {
        // error
        return(-7);
    }



    // Step 6: Load the camera settings from the XML document
    if ( lCamera->LoadFromXML( lDocument ) != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 6 failed");
        return(-8);
    };



    // Step 7: Send the settings to iPORT and the camera
    if ( lCamera->UpdateToCamera() != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 7 failed");
        return(-9);
    };



    // Step 8:  Create a buffer for grabbing images.
    // Get some information from the camera
    //unsigned short lSizeX, lSizeY, lDecimationX, lDecimationY, lBinningX, lBinningY;
    //CyPixelTypeID lPixelType;
    lCamera->GetSizeX( lSizeX );
    lCamera->GetSizeY( lSizeY );
    lCamera->GetDecimationX( lDecimationX );
    lCamera->GetDecimationY( lDecimationY );
    lCamera->GetBinningX( lBinningX );
    lCamera->GetBinningY( lBinningY );
    lCamera->GetEffectivePixelType( lPixelType );

    if ( ( lDecimationX != 0 ) && ( lDecimationY != 0 ) && ( lBinningX != 0 ) && ( lBinningY != 0 ) )
    {
        lSizeX = (( lSizeX / lBinningX ) + (( lSizeX % lBinningX ) ? 1 : 0));
        lSizeX = (( lSizeX / lDecimationX ) + (( lSizeX % lDecimationX ) ? 1 : 0));
        lSizeY = (( lSizeY / lBinningY ) + (( lSizeY % lBinningY ) ? 1 : 0));
        lSizeY = (( lSizeY / lDecimationY ) + (( lSizeY % lDecimationY ) ? 1 : 0));
    }

    // Create the buffer.
    //CyImageBuffer lBuffer( lSizeX, lSizeY, lPixelType );
    lBuffer.SetCapacity(lSizeX, lSizeY, lPixelType);


    // Step 9: Grab an image
    // In this case, it does not change anything, but some future camera
    // module may need to perform initialization before the grabbing, so
    // grabbing through the camera is preferred.
    lGrabber.StartContinuous( CyChannel( 0 ) );
*/
    return(true);
}

//////////////////////////////////////////////////////////////////////

void CAcqPhoton::GetCameraFrame(unsigned short *m_nPhotonFrameBuffer)
{
/*
    //if(ExitProgram==true) return;
    if ( lCamera->Grab(  CyChannel( 0 ), / *always this for now,* / lBuffer, 0 ) != CY_RESULT_OK )
    {
        // error
        //QMessageBox::information(this, "Title", "Step 9 failed");
        //delete lCamera; // to avoid memory leak
        //return;
    }



    // Step 10: Getting the buffer pointer from the CyBuffer class
    const unsigned char* lPtr;
    unsigned long lSize;
    if ( lBuffer.LockForRead( (void**) &lPtr, &lSize, CyBuffer::FLAG_NO_WAIT ) == CY_RESULT_OK )
    {
        // Now, lPtr points to the data and lSize contains the number of bytes available.
        memcpy( m_nPhotonFrameBuffer, lPtr, lSize );
        // Now release the buffer
        lBuffer.SignalReadEnd();
    }

    //char MyMessage[512];
    //sprintf(MyMessage, "Pixel[0] = %d (%x)" , m_nPhotonFrameBuffer[0],  m_nPhotonFrameBuffer[0]);
    //QMessageBox::information(this, "Title", MyMessage);
*/
    return;
}

//////////////////////////////////////////////////////////////////////

void CAcqPhoton::StartCamera()
{
    /*    // Get the channel
    CString lChannelName;
    const CyChannel lChannel( (CyChannelID)0 , (LPCTSTR) lChannelName );

    CyImageBuffer& lBuffer = GetBuffer( lChannel );
    CyAssert( !lCamera->IsStarted( lChannel ) );

    // Clear the buffer queue
    lBuffer.ClearQueue();

    // Generate the window name from the device and the channel
    char lName[ 128 ];
    lConfig.GetName( lName, sizeof( lName ) - 1 );
    string lWindowName( lName );

    unsigned long lFlags = lCamera->GetPixelConversionFlags();
    lCamera->StartGrabbing( lChannel, lBuffer );

    //lGrabber.StartContinuous( CyChannel( 0 ) );*/
}

//////////////////////////////////////////////////////////////////////

void CAcqPhoton::StopCamera()
{
/*    const CyChannel lChannel( (CyChannelID)0, "" );

    if ( lCamera->IsStarted( lChannel ) )
        lCamera->StopGrabbing( lChannel );*/
}

//////////////////////////////////////////////////////////////////////

void CAcqPhoton::ShutdownCamera()
{
/*    lGrabber.StopContinuous(CyChannel( 0 ));
    delete lCamera;
    lCamera = 0;
    lGrabber.Disconnect();
*/
    return;
}

//////////////////////////////////////////////////////////////////////

int CAcqPhoton::ResetCamera()
{
    unsigned lSize;
    //lGrabber.StopContinuous( CyChannel( 0 ) );
    //StopCamera();

    //lSize = 10;
/*    lDevice.Connect(lConfig);
    lResult = lDevice.ConfigPort( CY_DEVICE_PORT_BULK_0, "N1-57600-128-128,MODE:UART,BF:1" );
    lResult = lDevice.SendCommand( CY_DEVICE_PORT_BULK_0, RS232Reset, 10 );
    lResult = lDevice.ReceiveAnswer( CY_DEVICE_PORT_BULK_0, lTemp, &lSize ); // while wait for 24 bytes
    lDevice.Disconnect();
*/
    // Wait 1 second and then continue running aquire
    Sleep(2000);
    //lGrabber.StartContinuous( CyChannel( 0 ) );
    //StartCamera();

    return(lResult);
}

//////////////////////////////////////////////////////////////////////

BOOL CAcqPhoton::AutoFFC()
{
    unsigned lSize;
    //DWORD nBytes;
    //	unsigned char RS232AutoShutter[12] = {0x6e, 0, 0, 0xb, 0, 2, 0xf, 8, 0, 1, 0x10, 0x21};

/*    lSize = 12;
    lDevice.Connect(lConfig);
    lResult = lDevice.ConfigPort( CY_DEVICE_PORT_BULK_0, "N1-57600-128-128,MODE:UART,BF:1" );
    lResult = lDevice.SendCommand( CY_DEVICE_PORT_BULK_0, RS232AutoShutter, 12 );
    lResult = lDevice.ReceiveAnswer( CY_DEVICE_PORT_BULK_0, lTemp, &lSize ); // while wait for 24 bytes
    lDevice.Disconnect();
*/
    return( memcmp(RS232AutoShutter, lTemp, 10) );	// Check the first 10 characters
}

//////////////////////////////////////////////////////////////////////

BOOL CAcqPhoton::ManualFFC()
{
    unsigned lSize;
    //DWORD nBytes;
    //	unsigned char RS232ManShutter[12] = {0x6e, 0, 0, 0xb, 0, 2, 0xf, 8, 0, 0, 0, 0};

/*    lSize = 12;
    lDevice.Connect(lConfig);
    lResult = lDevice.ConfigPort( CY_DEVICE_PORT_BULK_0, "N1-57600-128-128,MODE:UART,BF:1" );
    lResult = lDevice.SendCommand( CY_DEVICE_PORT_BULK_0, RS232ManShutter, 12 );
    lResult = lDevice.ReceiveAnswer( CY_DEVICE_PORT_BULK_0, lTemp, &lSize ); // while wait for 24 bytes
    lDevice.Disconnect();*/

    return(lResult);
}

//////////////////////////////////////////////////////////////////////

BOOL CAcqPhoton::DoFFC()
{
    unsigned lSize;
    //DWORD nBytes;

    //lGrabber.StopContinuous( CyChannel( 0 ) );
    //StopCamera();

/*    lSize = 10;
    lDevice.Connect(lConfig);
    lResult = lDevice.ConfigPort( CY_DEVICE_PORT_BULK_0, "N1-57600-128-128,MODE:UART,BF:1" );
    lResult = lDevice.SendCommand( CY_DEVICE_PORT_BULK_0, RS232DoShutter, 10 );
    lResult = lDevice.ReceiveAnswer( CY_DEVICE_PORT_BULK_0, lTemp, &lSize ); // while wait for 20 bytes
    lDevice.Disconnect();
*/
    // Wait 1 second and then continue running aquire
    Sleep(1500);

    //StartCamera();

    return(lResult);
}
