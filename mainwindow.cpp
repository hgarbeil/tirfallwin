#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "bbdialog.h"

#include <mmsystem.h>
#include "MICROXCAM-384I_CAPI.h"

//#include "defines.h"
//#define GLOBAL
//#include "global.h"

#define GPS_FINEALIGN 1
#define GPS_AIRALIGN 2

#define LALA
#ifdef LALA
//#define GUI     // If this is not defined, don't use the GUI (not implemented)

#define NUMOFBUFFERS 120
#define GPSSIZE 512
char m_pSaveBufferGPS[GPSSIZE*NUMOFBUFFERS];


#ifdef _WIN32
#include "afxwin.h"
#include <timeapi.h>
#include <mmsystem.h>
#else
#include "cs sl_lib.h"
#endif


#ifndef DUMMY
#define toAscii toLatin1
#endif

//#include <QtGui/QApplication>
//:P#include <QDebug>
#include <QApplication>
#include <QInputDialog>
#include <QIcon>
#include <QMutex>
#include <QWaitCondition>

#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <stdio.h>
//#include <stdlib.h>
#ifdef __unix__
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include "acqmc320.h"
#include "acqpv640.h"
#include "acqtau.h"
#include "acqtauserial.h"
#include "mainprocess.h"
#endif
#include <math.h>
#include <algorithm>

#include "threads.h"
#include "defines.h"
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
#include "acqPhoton.h"
#include "acqino.h"
//#include "deviceFctns.h"
#include "fileFctns.h"
//#include "analysisFctns.h"
#include "dlgZoom.h"
#include "configCosmos.h"
#include "COMDevice.h"
#define GLOBAL
#include "global.h"
#include "bbsr800.h"
#include "bbsr800gpib.h"
#include "smc100.h"
#include "calibrator.h"
#include "cmigitsasc.h"
//:O#include "FindCameras.h"
#include "tirthreads.h"
//#include "inosettings.h"
//#include "ui_inos ettings.h"

//void FindFocusPosition(void);


/*pthread_mutex_t mutexData;
pthread_mutex_t mutexPos;
pthread_cond_t conditionMovePos;
pthread_cond_t conditionAcquireDone;
pthread_cond_t conditionSaveDone;
pthread_cond_t conditionSaveStart;*/
//unsigned char *pSaveBufferGPS;
char *pBufferGPS;
//float *pBufferEncoder;
//float RotationASpeed;
//BOOL GotoHalfWay;
//char FramesPerSec[256];
/*float FramesPerSec;
float FramesPerSecS;
*/
/*static uInt32        AcqImgWidth;
static uInt32        AcqImgHeight;
static uInt32		DesiredBufNum;
static Int32		MissedNbBuf;
static BOOL			StopRing;
static unsigned int plotFlag;
static float		SawToothWave[256];
BOOL AbortFlag;
int ReturnValue;*/
/*Int32 nbFrame;
uInt32 currBufNum;*/
/*bool CamCold = FALSE;
bool CamHot = FALSE;
bool CamOther = FALSE;
bool CamSavedCold = FALSE;
bool CamSavedHot = FALSE;
bool CamSavedOther = FALSE;
bool IntCold = FALSE;
bool IntHot = FALSE;
bool IntOther = FALSE;*/
/*bool WaveCal = FALSE;*/
/*bool Scan = FALSE;
bool ScanSaved = FALSE;
*/
//char HeaderFilePath[2048];
char GPSFilePath[2048];
//char EncoderFilePath[2048];
/*char ScanDataDirectory[2048];
//char FileName[1024];*/
/*char TempFilePath[2148];*/
/*char Temperature[64];
//char DateStamp[64];*/
/*//RECT InvalidateArea = {4, 295, 678, 425};
//RECT InvalidateArea = {10, 500, 1000, 720};
RECT InvalidateArea = {10, 10, 170, 640};
//RECT InvalidateArea = {10, 10, 20, 20};
short CalFilterPosition;
flidev_t dev;*/
/*char HotOrCold[16];
char FilterName[128];*/
//char SpecificPath[1024];
/*char BbUnit1HdrFilename[1024];
char BBHotHdrFilename[1024];
char ScanHdrFilename[1024];*/
//bool m_bRunningScanAll;
//bool bRunningSaveRoutine;
//bool bExitSavingData;
//char DateStampWave[64];
//char DateTime4Dir[64];
//bool RunningSaveRoutine;
//double t1 = 0, t2;
//bool ViewOnly;
//bool RunningScanAll;
//float BBAngleCold;
//float BBAngleHot;
//bool bSkipBlackBodies = false;
//bool bUseBBUnit2 = false;
//int PresentTask;
//bool m_bAllPositionsFlagOn;
//char buffer[2048];

#ifdef __unix__
MainProcess *mainProc;
#endif

char LogFilePath[2048];


// Camera globals
unsigned short m_nPhotonFrameBuffer[MAXIMAGESIZE];
unsigned short m_nPhotonBackgroundBuffer[MAXIMAGESIZE];
unsigned short *BufferFall;    // Waterfall buffer


// Semaphores
#ifdef __unix__
sem_t SaveDone_count;
sem_t AcquireDone_count;
#else
HANDLE SaveDone_count;
HANDLE AcquireDone_count;
#endif

// Threads
#ifdef __unix__
pthread_t threadSavedata;
pthread_t threadAcquire;
pthread_t threadWaterfall;
pthread_t threadBbUnit1;
pthread_t threadRotationA;
pthread_t threadCalAcq;
pthread_t threadManualAcq;
pthread_t threadScan;
//pthread_t threadStare;
pthread_t threadViewRange;
//pthread_t threadStareScene;
//pthread_t threadStareBB;
//pthread_t threadGumstixCom;
//pthread_t threadFocus;
pthread_t threadAutoScan;
pthread_t threadScript;
#else
DWORD threadSavedataID;
DWORD threadAcquireID;
DWORD threadWaterfallID;
DWORD threadBbUnit1ID;
DWORD threadRotationAID;
DWORD threadCalAcqID;
DWORD threadManualAcqID;
DWORD threadScanID;
DWORD threadViewRangeID;
DWORD threadAutoScanID;
DWORD threadScriptID;

HANDLE ThreadSavedata;
HANDLE ThreadAcquire;
HANDLE ThreadWaterfall;
HANDLE ThreadRotationA;
HANDLE ThreadCalAcq;
HANDLE ThreadManualAcq;
HANDLE ThreadScan;
HANDLE ThreadAutoscan;
HANDLE ThreadScript;

#endif
char UchisVersion[256];
bool SavingData = false;
bool ExitSavingData;
int FrameCount;
int NumFramesToSave;
int NumFramesSaved;

unsigned short *pSaveBuffer;
float *pSaveBufferEncoder;
unsigned short *pBuffer;
static int BufNum;

bool BbSettledUnit1;
//bool BbSettledUnit2;

int GumstixDefaultPort;
int MotionPortNum;
int BbPortNum, Bb2PortNum;
int CalibratorPortNum;
int GpsPortNum;

bool ButtonsDisabled;

//double ImageSecondDif[IMAGESIZE - IMAGEHEIGHT]; // (Differential is one less pixel per row.)
//unsigned short ImageHist[IMAGESIZE - IMAGEHEIGHT]; // (Differential is one less pixel per row.)

int CameraRetVal=0;


//BbSr800 BBUnitA;
//BbSr800 BBUnitB;
BbSr800 BBExternal;
BbSr800Gpib BBExternal2;
/*:O
#ifdef __unix__
TauSerial TauCommand;
#endif*/
//Smc100 Rotation;
//Smc100 Goniometer;
Smc100 Motion;
CMigitsA Gps;
Calibrator Cal;

bool InitAcq = false;
int CameraType;
//int CameraType = TAU640;
int DisplayHeight;
//bool TemperatureToggle;
char MyGpsLine[1024];
int FfcDone;
//int MyInt;

#endif

/*QMutex mutex;
QWaitCondition keyPressed;
QMutex mutex2;
QWaitCondition keyPressed2;*/

extern int ns_inst, nl_inst ;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    //time_t startTime;
    //startTime = timeGetTime();


    memset(m_nPhotonBackgroundBuffer, 0, MAXIMAGESIZE*sizeof(unsigned short));


    int i;
    //char comString[16];

    ui->setupUi(this);
    TProc = new TProcess () ;

    ::ns_inst = 324 ;
    ::nl_inst = 256 ;
    bool status = readProcessFile () ;
    if (!status) {
        QMessageBox msgBox ;
        msgBox.setText ("tirprocess.ini not found") ;
        msgBox.exec() ;
    }
    // start up the mainprocess, whether needed or not
    mainProc = new MainProcess (this) ;
    mainProc->setTProcess (TProc) ;
    //mainProc->HideMainProc();

    InoSettingsDialog = new InoSettings(this);

#ifdef __unix__
    //mp = NULL;

    connect (mainProc->prdlg, SIGNAL (closeUp()), this, SLOT(closeup())) ;
    //mainProc->
#endif

    //int iretn;
    //int RotationAPortNum,
    //int BbPortNum, GumstixPortNum;
    //, FocusPortNum;
    char buff[256];
    //:PqDebug() << "Start";
    OpenLog();
    OpenLogCmd();
    //:PqDebug() << "Logs Opened";
    // Get setting from config file.
    ReadConfig(&GoniometerHome, &RotationHome, &BBRotationPosition, &SlowSpeedRotation, &StartPositionRotation, &StopPositionRotation, &NumFrames,
               &DesiredTemperaturePaddleCold, &DesiredTemperaturePaddleHot,
               &DesiredTemperatureRotorCold, &DesiredTemperatureRotorHot,
               &DesiredTemperatureBbExt, &DesiredTemperatureBbExt2,
               DataDirectory, ProcessingDirectory, UchisVersion);

    //TemperatureToggle = false;
    DesiredTemperaturePaddleCold = 23;
    DesiredTemperaturePaddleHot = 15;
    DesiredTemperatureRotorCold = 23;
    DesiredTemperatureRotorHot = 15;


#ifdef __unix__
    CFindCameras Find;
    CameraType=Find.FindCameras();
#endif

    //CameraType = PHOTON;
    CameraType = INO;
    //CameraType = TESTPATTERN320X240;

    // Initialize Camera
    switch(CameraType) {
#ifdef __unix__
    case MC320:
        CameraRetVal = CameraMc320.InitCamera(&AcqImgWidth, &AcqImgHeight);
        break;
    case TAU640:
        //CameraTau.parm.Busy = false;
        CameraRetVal = CameraTau.InitCamera(&AcqImgWidth, &AcqImgHeight);
        break;
    case PV640:
        CameraRetVal = CameraPv640.InitCamera(&AcqImgWidth, &AcqImgHeight);
        break;
#endif
    case PHOTON:
        CameraRetVal = CameraPhoton.InitCamera(&AcqImgWidth, &AcqImgHeight);
        if(CameraRetVal==true) {
            CameraPhoton.ManualFFC();
            CameraPhoton.DoFFC();
            break;
        } else {
            CameraType = INO;
            // Fall thorough
        }
    case INO:
        CameraRetVal = CameraINO.InitCamera(&AcqImgWidth, &AcqImgHeight);
        break;
    case TESTPATTERN320X240:
        CameraRetVal = CameraDummy320.InitCamera(&AcqImgWidth, &AcqImgHeight);
        break;
//    case TESTPATTERN640X512:
//        CameraRetVal = CameraDummy640.InitCamera(&AcqImgWidth, &AcqImgHeight);
//        break;
    }
    /*        if(  CameraRetVal >= 0 ) {
            // Start the camera
            Camera.StartCamera();
            pBuffer = pSaveBuffer;
            InitAcq = true;
        }*/

    AcqImgSize = AcqImgWidth*AcqImgHeight;


    memset(m_nPhotonFrameBuffer, 0, AcqImgSize);
    //m_bUseBBUnit2 = false;

    size = (long)AcqImgWidth*(long)AcqImgHeight;

    // Move to CLEAR positions
    //PresentPositionRotation = RotationHome;

    // Set Temperatures?
    //DesiredTemperatureA = DesiredTemperaturePaddleCold;


    TemperaturePaddleCold = 0;
    TemperaturePaddleHot = 0;
    TemperaturePaddleAmbient = 0;
    TemperatureRotorCold = 0;
    TemperatureRotorHot = 0;
    m_nIntermediateSpeedGoniometer = 1;
    m_nFastSpeedGoniometer = 10;
    m_nIntermediateSpeedRotation = 1;
    m_nFastSpeedRotation = 10;
    FrameRate = 50;
    FrameCount = -99;

    ExitProgram=false;  // When true, this flags to terminate the program.
    Abort = false;
    bUseDirectoryStructure = false;
    //    BbPortOpen=false;
    //    BbBusy=false;
    //    RotationAPortOpen=false;
    //    RotationABusy=false;
    AcqBusy=false;
    GumstixPortOpen=false;
    ButtonsDisabled=false;

    FfcDone = false;
    // !!!!!!! strcpy(SensorType, "Tircis System, LumaSense MC320");



    // Set "lineedit" values of BENCH TAB
    ui->lineEditDataDirectoryBench->setText(DataDirectory);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleCold);
    ui->lineEditDesiredTemperaturePaddleColdBench->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleHot);
    ui->lineEditDesiredTemperaturePaddleHotBench->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorCold);
    ui->lineEditDesiredTemperatureRotorColdBench->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorHot);
    ui->lineEditDesiredTemperatureRotorHotBench->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureBbExt);
    ui->lineEditDesiredTemperatureExtBench->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureBbExt2);
    ui->lineEditDesiredTemperatureExt2Bench->setText(buff);

    sprintf(buff, "%d" , NumFrames);
    ui->lineEditNumFramesBench->setText(buff);
    //ui->lineEditDesiredRotationAPosition->setText("0.00");
    ui->lineEditNumWaveCalFramesBench->setText("100");
    ui->lineEditWavelengthBench->setText("10.53");
    ui->lineEditNumAcquiredBench->setText("0");
    ui->lineEditShortCommentBench->setText("test");
    ui->lineEditNumBBFramesBench->setText("100");
    ui->lineEditDesiredRotationBench->setText("0");
    ui->lineEditDesiredGoniometerBench->setText("0");

    sprintf(buff, "%.2f" , ScanStageSlowSpeed);
    ui->lineEditScanStageRate->setText(buff);
    sprintf(buff, "%.2f" , ScanStageStartPosition);
    ui->lineEditScanStageStart->setText(buff);
    sprintf(buff, "%.2f" , ScanStageStopPosition);
    ui->lineEditScanStageStop->setText(buff);

    // Set "lineedit" values of TRIPOD TAB
    ui->lineEditDataDirectoryTripod->setText(DataDirectory);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleCold);
    ui->lineEditDesiredTemperaturePaddleColdTripod->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleHot);
    ui->lineEditDesiredTemperaturePaddleHotTripod->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorCold);
    ui->lineEditDesiredTemperatureRotorColdTripod->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorHot);
    ui->lineEditDesiredTemperatureRotorHotTripod->setText(buff);
    sprintf(buff, "%d" , NumFrames);
    ui->lineEditNumFramesTripod->setText(buff);
    //ui->lineEditDesiredRotationAPosition->setText("0.00");
    ui->lineEditNumWaveCalFramesTripod->setText("100");
    ui->lineEditWavelengthTripod->setText("10.53");
    ui->lineEditNumAcquiredTripod->setText("0");
    ui->lineEditShortCommentTripod->setText("test");
    ui->lineEditNumBBFramesTripod->setText("100");
    ui->lineEditDesiredRotationTripod->setText("0");
    ui->lineEditDesiredGoniometerTripod->setText("0");


    // Set "lineedit" values of FLIGHT TAB
    ui->lineEditDataDirectoryFlight->setText(DataDirectory);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleCold);
    ui->lineEditDesiredTemperaturePaddleColdFlight->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperaturePaddleHot);
    ui->lineEditDesiredTemperaturePaddleHotFlight->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorCold);
    ui->lineEditDesiredTemperatureRotorColdFlight->setText(buff);
    sprintf(buff, "%4.2f" , DesiredTemperatureRotorHot);
    ui->lineEditDesiredTemperatureRotorHotFlight->setText(buff);
    sprintf(buff, "%d" , NumFrames);
    ui->lineEditNumFramesFlight->setText(buff);
    //ui->lineEditDesiredRotationAPosition->setText("0.00");
    ui->lineEditNumWaveCalFramesFlight->setText("100");
    ui->lineEditWavelengthFlight->setText("10.53");
    ui->lineEditNumAcquiredFlight->setText("0");
    ui->lineEditShortCommentFlight->setText("test");
    ui->lineEditNumBBFramesFlight->setText("100");

    ui->lineEditNumFramesFlight->setText("90000");

    // Set Radio Buttons
    ui->radioButtonWave10Tripod->click();
    ui->radioButtonWave10Flight->click();
    ui->radioButtonPaddleColdT30->click();
    ui->radioButtonPaddleHotT50->click();
    ui->radioButtonRotorColdT30->click();
    ui->radioButtonRotorHotT50->click();

    ui->radioButtonPaddleColdOffBench->click();
    ui->radioButtonPaddleHotOffBench->click();
    //ui->radioButtonPaddleClearBench->click();
    ui->radioButtonRotorColdOffBench->click();
    ui->radioButtonRotorHotOffBench->click();
    ui->radioButtonRotorClearBench->click();


    // Set these for Gumstix (remote) operation
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         // Get Wavelength
    //strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    NumCalFrames = ui->lineEditNumWaveCalFramesBench->text().toInt();

//#define DISPLAYWIDTH 320L
//:O#define DISPLAYWIDTH 324L
#define DISPLAYWIDTH 384L
    //#define DISPLAYHEIGHT 256L
    if(AcqImgHeight==240) {
        DisplayHeight=240;
    } else {
        DisplayHeight=256;
    }

DisplayHeight=287;

//#define DISPLAYSIZE 81920L
//#define DISPLAYSIZE 82944L
#define DISPLAYSIZE 110208L

    ImgCamera = new uchar[3L*DISPLAYSIZE+15];
    if (ImgCamera==NULL) exit(-99);
    QImageCamera = new QImage(ImgCamera, DISPLAYWIDTH, DisplayHeight, DISPLAYWIDTH*3, QImage::Format_RGB888);
    if (QImageCamera==NULL) exit(-98);

//#define WATERFALLWIDTH 320L
//#define WATERFALLHEIGHT 500L
//#define WATERFALLSIZE 160000L
#define WATERFALLHEIGHT 500L
//#define WATERFALLWIDTH 324L
//#define WATERFALLSIZE 162000L
#define WATERFALLWIDTH 384L
#define WATERFALLSIZE 192000L
    ImgFall = new uchar[2L*3L*WATERFALLSIZE+15];
    if (ImgFall==NULL) exit(-97);
    QImageFall = new QImage(ImgFall, WATERFALLWIDTH, WATERFALLHEIGHT, WATERFALLWIDTH*3, QImage::Format_RGB888);
    if (QImageFall==NULL) exit(-96);

#define PLOTWIDTH 320L
#define PLOTHEIGHT 201L
#define PLOTSIZE 64320L

    ImgPlotFft = new uchar[3L*PLOTSIZE+15];
    if (ImgPlotFft==NULL) exit(-95);
    QImagePlotFft = new QImage(ImgPlotFft, PLOTWIDTH, PLOTHEIGHT, PLOTWIDTH*3, QImage::Format_RGB888);
    if (QImagePlotFft==NULL) exit(-94);

    ImgPlotPixel = new uchar[3L*PLOTSIZE+15];
    if (ImgPlotPixel==NULL) exit(-93);
    QImagePlotPixel = new QImage(ImgPlotPixel, PLOTWIDTH, PLOTHEIGHT, PLOTWIDTH*3, QImage::Format_RGB888);
    if (QImagePlotPixel==NULL) exit(-92);


    pSaveBuffer = new unsigned short[AcqImgSize * 120L];	//Allocate 120 frames worth.
    //pSaveBufferGPS = new unsigned char[GPSSIZE * 120L];	//Allocate 120 frames worth.
    //pSaveBufferEncoder = new float[120L];	//Allocate 120 frames worth.



    //Put sawtooth function in array.
    for( i=0 ; i<128 ; i++ ) {
        SawToothWave[i] = (float)(1.0*(double)i/128.0);
        SawToothWave[i+128] = (float)((128.0-(double)i)/128.0);
    }


#ifdef __unix__
    cssl_start();
#endif

    //Use UDEV instead of searching for devices.
    /*
    // Figure out wich serial port each device is on.  If device not found will show COM99 on display
    RotationAPortNum = 98;   // Set default value
    BbPortNum = 98;      // Set default value
    GumstixPortNum = 98;
    FocusPortNum = 98;

    FindDevices(&RotationAPortNum, &BbPortNum, &GumstixPortNum, &FocusPortNum);

    // If as device was not found, Set to default port number (if default is 98, then we use UDEV).
    if(RotationAPortNum==98) {
        RotationAPortNum = MotionPort;
        sprintf(buff, "COM%d", RotationAPortNum+1);
        ui->lineEditRotationAComPort->setText(buff);
    }
    if(BbPortNum==98) {
        BbPortNum = BbPortNum;
        sprintf(buff, "COM%d", BbPortNum+1);
        ui->lineEditBbComPort->setText(buff);
    }
    if(FocusPortNum==98) {
        FocusPortNum = FocusDefaultPort;
        sprintf(buff, "COM%d", FocusPortNum+1);
        ui->lineEditFocusComPort->setText(buff);
    }
    if(GumstixPortNum==98) {
        GumstixPortNum = GumstixDefaultPort;
        sprintf(buff, "COM%d", GumstixPortNum+1);
        ui->lineEditGumstixComPort->setText(buff);
    }

    if(RotationAPortNum<98) RotationAPortOpen = OpenRotationA(&SerialRotationA, RotationAPortNum, &RotationABusy);
    if(BbPortNum<98) BbPortOpen = OpenBb(&SerialBb, BbPortNum, &BbBusy);
*/

    // Here we use UDEV if the port was not detected, and the default port was 98

    // Wait up to 1 seconds for GumStix serial to be opened.
    //i=0;
    //SerialRotationA = NULL;

    /*
    //    if(!BbPortOpen) {
            SerialBb=cssl_open("/dev/ttyUSB_arroyo",0, 38400,8,0,1);
            //SerialBb=cssl_open("/dev/ttyUSB1",0, 38400,8,0,1);
            if (SerialBb) {
                cssl_settimeout(SerialBb, 0, 0.5);  // .05 second timeout.
                BbPortOpen = true;
                strcpy(buff, "BlackBody A");
                ui->lineEditBbAComPort->setText(buff);
            }
    //    }
*/

    //Rotation.Init("/dev/ttyUSB_smc100");
    //    Rotation.Init("/dev/ttyUSB0");
    //BBUnitA.Init("/dev/ttyUSB_arroyo2", 38400, "BBUnitA");
    //BBUnitB.Init("/dev/ttyUSB1", 38400);
    //Rotation.Init("/dev/ttyUSB0");
    //if (SerialRotationA) {

    Cal.Init(CalibratorPortNum);
    if(UseMotionStage) {
        Motion.Init(MotionPortNum);
    }
#ifdef __unix__
    TauCommand.Init((char*)"/dev/ttyUSB_camera", 57600);
#endif
    BBExternal.Init(BbPortNum, /*9600,*/ (char*)"SR800");
    BBExternal2.Init(Bb2PortNum, /*9600,*/ (char*)"SR800_2");
    //Gps.Init();

#ifdef TESTING13
    cssl_t *SerialRotationA;
    //    if(!RotationAPortOpen) {
    //SerialRotationA=cssl_open("/dev/ttyUSB_smc100",0, 19200,8,0,1);
    SerialRotationA=cssl_open("/dev/ttyUSB0",0, 19200,8,0,1);
    //SerialRotationA=cssl_open("/dev/ttyS2",0, 19200,8,0,1);
    if (SerialRotationA) {     //SerialRotationA=cssl_open("/dev/ttyS1",0, 19200,8,0,1);
        //if (SerialRotationA) {

        cssl_settimeout(SerialRotationA, 0, 0.5);  // .05 second timeout.
        //:)RotationAPortOpen = true;
        //strcpy(buff, "SMC100");
        //:)ui->lineEditRotationAComPort->setText(buff);
    }
    //    }
    char mybuf[256];
    int ret;

    //Send command to get Position.
    strcpy(mybuf, "1TP\r\n");
    cssl_putstring(SerialRotationA, mybuf);
    usleep(1000000);
    mybuf[0] = 0;
    ret = cssl_getdata(SerialRotationA, (uint8_t*)mybuf, 64);

    usleep(1000000);
#endif




#ifdef THISWORKS
    bool good;

    if (true) {
        // Open the port.
        good = SerialBb.Open(1, TRUE, TRUE);
        if (!good) {
            //AfxMessageBox ("Open for Commport for Focus did not work");
            //m_bUseBBUnit1 = FALSE
            ;
        }
        else {
            good = SerialBb.SetConfiguration(L"baud=9600 parity=N data=8 stop=1");
            if (!good) {
                //AfxMessageBox ("Set Configuration did not work");
                //m_bUseBBUnit1 = FALSE
                ;
            }
            else {
                good = SerialBb.SetTimeouts(500, 500, 500);
                if (!good) {
                    //MessageBox ("SetTimeouts did not work");
                    //m_bUseBBUnit1 = FALSE
                    ;
                }
            }
        }
    }
    char comStringIn[256];
    char comStringOut[256];
    DWORD nBytesIn;
    DWORD nBytesOut;
    int size = 19;

    /*sprintf(comStringOut , "\r\n");
SerialBb.Write(comStringOut,2, &nBytesOut);
SerialBb.Read(comStringIn, 100, &nBytesIn);
SerialBb.Write(comStringOut, 2, &nBytesOut);
SerialBb.Read(comStringIn, 100, &nBytesIn);
*/

    sprintf(comStringOut , "SETTEMPERATURE %d\r\n" , 23);
    SerialBb.Write(comStringOut, strlen(comStringOut), &nBytesOut);
    SerialBb.Read(comStringIn, 100, &nBytesIn);
#endif

    /*sprintf(comStringOut , "\r\n" , 2);
SerialBb.Write(comStringOut, 2, &nBytesOut);
SerialBb.Read(comStringIn, 100, &nBytesIn);*/

#ifdef __unix__
    sem_init (&SaveStart_count, 0, 0);
    sem_init (&SaveDone_count, 0, 0);
    sem_init (&AcquireDone_count, 0, 0);
    //    sem_init (&ChangeTemperatureA_count, 0, 0);
    sem_init (&ChangeTemperatureB_count, 0, 0);
    sem_init (&ChangeTemperatureExt_count, 0, 0);
    sem_init (&MovePosition_count, 0, 0);
    sem_init (&RunCalAcq_count, 0, 0);
    sem_init (&RunManualAcq_count, 0, 0);
    sem_init (&RunScan_count, 0, 0);// Used for "ScanStage"
    //    sem_init (&ViewRange_count, 0, 0);
    sem_init (&AcquireFinished_count, 0, 0);
    //    sem_init (&RunStareBB_count, 0, 0);
    //    sem_init (&RunStareScene_count, 0, 0);
    //    sem_init (&MoveFocus_count, 0, 0);
    //    sem_init (&RunAutoFocus_count, 0, 0);
    sem_init (&RunAutoscan_count, 0, 0);
    sem_init (&RunScript_count, 0, 0);
    sem_init (&RunFfc_count, 0, 0);
#else
    VERIFY(SaveStart_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(SaveDone_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(AcquireDone_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(ChangeTemperatureB_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(ChangeTemperatureExt_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(MovePosition_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunCalAcq_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunManualAcq_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunScan_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(AcquireFinished_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunAutoscan_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunScript_count = CreateEvent(NULL, FALSE, FALSE, NULL));
    VERIFY(RunFfc_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif

    // *** Start threads
#ifdef __unix__
    iretn = pthread_create(&threadSavedata, NULL, runSavedata, NULL);
    iretn = pthread_create(&threadAcquire, NULL, runAcquisition, ImgCamera);
    iretn = pthread_create(&threadWaterfall, NULL, runWaterfall, ImgFall);
    //iretn = pthread_create(&threadBbUnit1, NULL, runBbUnit1, NULL);
    iretn = pthread_create(&threadRotationA, NULL, runRotationA, NULL);
    iretn = pthread_create(&threadCalAcq, NULL, runCalAcq, ui);
    iretn = pthread_create(&threadManualAcq, NULL, runManualAcq, ui);
    iretn = pthread_create(&threadScan, NULL, runScan, ui);
    iretn = pthread_create(&threadAutoScan, NULL, runAutoScan, ui);
    iretn = pthread_create(&threadScript, NULL, runScript, ui);
    iretn = pthread_create(&threadScript, NULL, runFfc, ui);
#else
    //int DataArray;
    ThreadSavedata = CreateThread(NULL, 0, runSavedata, NULL, 0, &threadSavedataID);
    ThreadAcquire = CreateThread(NULL, 0, runAcquisition, NULL, 0, &threadAcquireID);
    //ThreadWaterfall = CreateThread(NULL, 0, runWaterfall, NULL, 0, &threadWaterfallID);
    ThreadRotationA = CreateThread(NULL, 0, runRotationA, NULL, 0, &threadRotationAID);
    ThreadCalAcq = CreateThread(NULL, 0, runCalAcq, NULL, 0, &threadCalAcqID);
    ThreadManualAcq = CreateThread(NULL, 0, runManualAcq, NULL, 0, &threadManualAcqID);
    ThreadScan = CreateThread(NULL, 0, runScan, NULL, 0, &threadScanID);
    ThreadAutoscan = CreateThread(NULL, 0, runAutoScan, NULL, 0, &threadAutoScanID);
    ThreadScript = CreateThread(NULL, 0, runScript, NULL, 0, &threadScriptID);
    ThreadScript = CreateThread(NULL, 0, runFfc, NULL, 0, &threadScriptID);

    /*    VERIFY(threadSavedata = AfxBeginThread(runSavedata, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadAcquire = AfxBeginThread(runAcquisition, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadWaterfall = AfxBeginThread(runWaterfall, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadRotationA = AfxBeginThread(runRotationA, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadCalAcq = AfxBeginThread(runCalAcq, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadManualAcq = AfxBeginThread(runManualAcq, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadScan = AfxBeginThread(runScan, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadAutoscan = AfxBeginThread(runAutoScan, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadScript = AfxBeginThread(runScript, this, THREAD_PRIORITY_NORMAL));
    VERIFY(threadScript = AfxBeginThread(runFfc, this, THREAD_PRIORITY_NORMAL));*/
#endif


    //connect((QObject*)&threadGumstixCom, SIGNAL(timeout()), this, SLOT(deleteLater()));
    //threadGumstixCom
    /*pthread_cond_t *cond;
    pthread_cond_broadcast(cond);
    pthread_cond_signal(cond);*/

    connect(&TimerVideo, SIGNAL(timeout()), this, SLOT(onTimer()));     // Update screen timer
    connect(&TimerProgress, SIGNAL(timeout()), this, SLOT(onProgress()));     // Update screen timer
    connect(&TimerLogEntry, SIGNAL(timeout()), this, SLOT(onLogEntry()));     // Update screen timer

    //connect(&TimerBB, SIGNAL(timeout()), this, SLOT(MonitorTemperatureBbUnit1()));   // Black Body timer
    //connect(&TimerRotationA, SIGNAL(timeout()), this, SLOT(MonitorPositionA()));    // RotationA Timer
    //connect(&TimerFocus, SIGNAL(timeout()), this, SLOT(MonitorFocusPosition()));    // Focus Timer

    //connect(&objB, SIGNAL(sigB()), &objA, SLOT(slotA()), Qt::BlockingQueuedConnection);

    //FindFocusPosition();

    // *** Start timers
    TimerVideo.start(33);
    TimerProgress.start(1000);
    TimerLogEntry.start(10000);
    //TimerBB.start(1000);
    //TimerRotationA.start(10000);
    //TimerFocus.start(700);


    //AutoFocusOn = false;

    // Home stage
    //sprintf (comString, "1OR?\r\n" );
    //cssl_putstring(SerialRotationA, comString);
    //Rotation.Home();
    //Rotation.Set(2, 15);

    // Set BB to cold temperature
    Cal.Set(PADDLETEMPERATURECOLD, (float)DesiredTemperaturePaddleCold);
    Cal.Set(PADDLETEMPERATUREHOT, (float)DesiredTemperaturePaddleHot);
    Cal.Set(ROTORTEMPERATURECOLD, (float)DesiredTemperatureRotorCold);
    Cal.Set(ROTORTEMPERATUREHOT, (float)DesiredTemperatureRotorHot);

    Cal.Set(PADDLEPOWERCOLD, (float)POWEROFF);
    Cal.Set(PADDLEPOWERHOT, (float)POWEROFF);
    Cal.Set(ROTORPOWERCOLD, (float)POWEROFF);
    Cal.Set(ROTORPOWERHOT, (float)POWEROFF);

    // Move Rotator to Pass through
    Cal.Set(ROTORPOSITION, ROTORPASS);
    strcpy(PresentPositionRotor, "Clear");
    PresentPositionPaddleCold = 0;
    PresentPositionPaddleHot = 0;
    PresentPositionPaddleAmbient = 0;
    //strcpy(PresentPositionRotor, "n/a");

    // Move Cold Paddle into view
    //Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);


    unsigned short Intensity;
    uchar *pImgCamera = ImgCamera;
    for(int i=0 ; i<DISPLAYSIZE ; i++) {
        if(ExitProgram) break;
        Intensity = 0;
        memset(pImgCamera, Intensity, 3);
        pImgCamera+=3;
    }


    uchar *pImgFall = ImgFall;
    for(int i=0 ; i<WATERFALLSIZE ; i++) {
        if(ExitProgram) break;
        Intensity = 128;
        memset(pImgFall, Intensity, 3);
        pImgFall+=3;
    }

    // Set Default tab to the first.
    CurrentTab = 0;
    //ResumeThread (ThreadAcquire) ;


    //float MyCel = Cal.Counts2Celcius(67);
    //int MyCnt = Cal.Celcius2Counts(MyCel);
    //MyCel = MyCnt;

    ui->widgetFftPlotBench->setRescaleFlag (true) ;
    ui->widgetPixelPlotBench->setRescaleFlag (true) ;
    ui->widgetFftPlotTripod->setRescaleFlag (true) ;
    ui->widgetPixelPlotTripod->setRescaleFlag (true) ;
    ui->widgetFftPlotFlight->setRescaleFlag (true) ;
    ui->widgetPixelPlotFlight->setRescaleFlag (true) ;

    //:O
    if(UseMotionStage) {
        COSMOS_USLEEP(9000000);
    }
    Motion.Set(10, RotationHome, 0);
    Motion.Set(10, GoniometerHome, 1);

    //InoSettingsDialog->ShowStats();
    //ui->lineEditNumAcquiredBench->show();
    ui->tabWidget->setCurrentIndex(0);
}


MainWindow::~MainWindow()
{
    ShutDown();

    delete ui;
    delete mainProc ;
    delete InoSettingsDialog;
    //delete [] ImgPlotFft ;
#ifdef __unix__

#endif
}

// *****************************************************************
// Timer Function: Update image display.
// *****************************************************************
void MainWindow::onTimer(void)
{
    int i;
    static float LastSlowSpeed=0;
    static float LastDesiredTemperaturePaddleCold=0;
    static float LastDesiredTemperaturePaddleHot=0;

    char buffer[64];
    //char strSpeed[64];
    char strTemperaturePaddleCold[64];
    char strTemperaturePaddleHot[64];
    char strTemperatureRotorCold[64];
    char strTemperatureRotorHot[64];
    char strTemperatureExt[64];
    char strTemperatureExt2[64];
    char strTemperatureSensor[16][64];
    char strFrameCount[32];
    char strRotation[32];
    char strGoniometer[32];
    char strMinPix[32];
    char strMaxPix[32];
    char strFFTMaxVal[64];
    //char strDifferentialSummation[256];
    //char strCameraTemperature[64];
    char strFPS[64];
    //return;

    if( CameraRetVal == -10 ) {
        QMessageBox::warning(0, "Camera Error", "Could not find any camera's.\n\nCheck camera connections,\ncheck firewall, and\nrestart the program." );
        CameraRetVal = 0;
    }

    if( ScriptMessageDone == true ) {
        QMessageBox::information(0, "Script done", "The script has finished" );
        ScriptMessageDone = false;
    }

    if( FfcDone == true ) {
        FfcDone = false;
        ui->radioButtonRotorClearBench->click();
    }

    Cal.Get(&PresentTemperaturePaddleCold, &PresentTemperaturePaddleHot, &PresentTemperatureRotorCold, &PresentTemperatureRotorHot, (float *)TemperatureSensor);
    //PresentTemperatureBbB = Cal.Get();
    PresentTemperatureBbExt = BBExternal.Get();
    PresentTemperatureBbExt2 = BBExternal2.Get();


    // Profile bench - get min and max for scaling...
    // if auto flag
    if (ui->widgetFftPlotBench->rescaleFlag) {
        QString str = QString ("%1").arg(ui->widgetFftPlotBench->minVal) ;
        ui->lineEditFftMinBench->setText (str) ;
        str = QString ("%1").arg(ui->widgetFftPlotBench->maxVal) ;
        ui->lineEditFftMaxBench->setText (str) ;
    } else
    {
        float mn, mx ;
        mn = ui->lineEditFftMinBench->text().toFloat() ;
        mx = ui->lineEditFftMaxBench->text().toFloat() ;
        ui->widgetFftPlotBench->setRange (1, mn, mx) ;

    }


//    if (ui->widgetPixelPlotBench->rescaleFlag) {
//        QString str = QString ("%1").arg(ui->widgetPixelPlotBench->minVal) ;
//        ui->lineEditPixelMinBench->setText (str) ;
//        str = QString ("%1").arg(ui->widgetPixelPlotBench->maxVal) ;
//        ui->lineEditPixelMaxBench->setText (str) ;
//    } else
//    {
//        float mn, mx ;
//        mn = ui->lineEditPixelMinBench->text().toFloat() ;
//        mx = ui->lineEditPixelMaxBench->text().toFloat() ;
//        ui->widgetPixelPlotBench->setRange (1, mn, mx) ;

//    }


    if(BBExternal.IsStable()) {
        ui->frameBBExtStable->setStyleSheet("background-color:green;");
    } else {
        ui->frameBBExtStable->setStyleSheet("background-color:red;");
    }
    if(BBExternal2.IsStable()) {
        ui->frameBBExt2Stable->setStyleSheet("background-color:green;");
    } else {
        ui->frameBBExt2Stable->setStyleSheet("background-color:red;");
    }


    PresentPositionGoniometer = Motion.Get(1)-GoniometerHome;
    PresentPositionRotation = Motion.Get(0)-RotationHome;

    sprintf(strTemperaturePaddleCold, "%.1f", PresentTemperaturePaddleCold);
    sprintf(strTemperaturePaddleHot, "%.1f", PresentTemperaturePaddleHot);
    sprintf(strTemperatureRotorCold, "%.1f", PresentTemperatureRotorCold);
    sprintf(strTemperatureRotorHot, "%.1f", PresentTemperatureRotorHot);

    if(PresentTemperatureBbExt!=-99) {
        sprintf(strTemperatureExt, "%.1f", PresentTemperatureBbExt);
    } else {
        sprintf(strTemperatureExt, "n/a");
    }

    if(PresentTemperatureBbExt2!=-99) {
        sprintf(strTemperatureExt2, "%.1f", PresentTemperatureBbExt2);
    } else {
        sprintf(strTemperatureExt2, "n/a");
    }


    for(i=0 ; i<10 ; i++) {
        sprintf(strTemperatureSensor[i], "%.1f", TemperatureSensor[i]);
    }

    // Subtract the "program home position" when displaying to the user (otherwise always use absolute values)
    if(PresentPositionRotation!=-999) {
        sprintf(strRotation, "%.3f" , PresentPositionRotation);
    } else {
        sprintf(strRotation, "n/a");
    }

    // Subtract the "program home position" when displaying to the user (otherwise always use absolute values)
    if(PresentPositionGoniometer!=-999) {
        sprintf(strGoniometer, "%.3f" , PresentPositionGoniometer);
    } else {
        sprintf(strGoniometer, "n/a");
    }
    //sprintf(strFFTMaxVal, "%d" , FFTMaxVal);
    sprintf(strFrameCount, "%d", FrameCount-1);
    //sprintf(strMinPix, "%d", MinValPixel);
    //sprintf(strMaxPix, "%d", MaxValPixel);
    //sprintf(strCameraTemperature, "%3.1f", CameraTemperature);
    sprintf(strFPS, "%.1f", PresentFPS);

    // Update display values.
    switch(CurrentTab) {
    case 0:


        if (ui->widgetPixelPlotBench->rescaleFlag) {
            QString str = QString ("%1").arg(ui->widgetPixelPlotBench->minVal) ;
            ui->lineEditPixelMinBench->setText (str) ;
            str = QString ("%1").arg(ui->widgetPixelPlotBench->maxVal) ;
            ui->lineEditPixelMaxBench->setText (str) ;
        } else
        {
            float mn, mx ;
            mn = ui->lineEditPixelMinBench->text().toFloat() ;
            mx = ui->lineEditPixelMaxBench->text().toFloat() ;
            ui->widgetPixelPlotBench->setRange (1, mn, mx) ;

        }

        double dVal;
        char sVal[32];
        fn_GetFPATemp(dVal);
        sprintf(sVal, "%.2f", dVal);
        ui->lineEditCameraTemperatureBench->setText(sVal);

        ui->widgetImageBench->update(); // Redraw the Image (widgetView widget)
        ui->widgetFallBench->update();  // Redraw the Waterfall (widgetView widget)
        ui->widgetPixelPlotBench->updatePixelData();  // Redraw the pixel plot
        ui->widgetFftPlotBench->updateData();  // Redraw the FFT plot

        ui->lineEditPresentRotationBench->setText(strRotation);
        ui->lineEditPresentGoniometerBench->setText(strGoniometer);
        //ui->lineEditPixelMinBench->setText(strMinPix);
        //ui->lineEditPixelMaxBench->setText(strMaxPix);
        //ui->lineEditFFTMaxValBench->setText(strFFTMaxVal);

        if(FrameCount>=0) {
            ui->lineEditNumAcquiredBench->setText(strFrameCount);
        }

        ui->lineEditTemperaturePaddleColdBench->setText(strTemperaturePaddleCold);
        ui->lineEditTemperaturePaddleHotBench->setText(strTemperaturePaddleHot);
        ui->lineEditTemperatureRotorColdBench->setText(strTemperatureRotorCold);
        ui->lineEditTemperatureRotorHotBench->setText(strTemperatureRotorHot);
        ui->lineEditTemperatureBbExtBench->setText(strTemperatureExt);
        ui->lineEditTemperatureBbExt2Bench->setText(strTemperatureExt2);

        // Sensors
        ui->lineEditAcceleration1->setText("0");
        ui->lineEditAcceleration2->setText("0");
        ui->lineEditAcceleration3->setText("0");
        ui->lineEditAcceleration4->setText("0");
        ui->lineEditTemperatureSensor1->setText(strTemperatureSensor[0]);
        ui->lineEditTemperatureSensor2->setText(strTemperatureSensor[1]);
        ui->lineEditTemperatureSensor3->setText(strTemperatureSensor[2]);
        ui->lineEditTemperatureSensor4->setText(strTemperatureSensor[3]);
        ui->lineEditTemperatureSensor5->setText(strTemperatureSensor[4]);
        ui->lineEditTemperatureSensor6->setText(strTemperatureSensor[5]);
        //ui->lineEditTemperatureSensor7->setText(strTemperatureSensor[6]);
        //ui->lineEditTemperatureSensor8->setText(strTemperatureSensor[7]);
        //ui->lineEditTemperatureSensor9->setText(strTemperatureSensor[8]);
        //ui->lineEditTemperatureSensor10->setText(strTemperatureSensor[9]);

        //ui->lineEditCameraTemperatureBench->setText("0");
        ui->lineEditCameraFPSBench->setText(strFPS);

        // Com Ports
        //ui->lineEditRotationComPort->setText(Rotation.parm.Status);
        //ui->lineEditGoniometerComPort->setText(Goniometer.parm.Status);


        sprintf(buffer, "%d" , MouseX);
        ui->label_MouseX->setText(buffer);
        sprintf(buffer, "%d" , MouseY);
        ui->label_MouseY->setText(buffer);
        sprintf(buffer, "%d" , MouseIntensity);
        ui->label_MouseIntensity->setText(buffer);
        break;
    case 1:

        if (ui->widgetPixelPlotTripod->rescaleFlag) {
            QString str = QString ("%1").arg(ui->widgetPixelPlotTripod->minVal) ;
            ui->lineEditPixelMinTripod->setText (str) ;
            str = QString ("%1").arg(ui->widgetPixelPlotTripod->maxVal) ;
            ui->lineEditPixelMaxTripod->setText (str) ;
        } else
        {
            float mn, mx ;
            mn = ui->lineEditPixelMinTripod->text().toFloat() ;
            mx = ui->lineEditPixelMaxTripod->text().toFloat() ;
            ui->widgetPixelPlotTripod->setRange (1, mn, mx) ;

        }

        ui->widgetImageTripod->update(); // Redraw the Image (widgetView widget)
        ui->widgetFallTripod->update();  // Redraw the Waterfall (widgetView widget)
        //ui->widgetFallTripod->update();  // Redraw the Waterfall (widgetView widget)
        //ui->widgetPixelPlotTripod->update();  // Redraw the pixel plot
        ui->widgetPixelPlotTripod->updatePixelData();  // Redraw the pixel plot
        ui->widgetFftPlotTripod->updateData();  // Redraw the FFT plot

        ui->lineEditPresentRotationTripod->setText(strRotation);
        ui->lineEditPresentGoniometerTripod->setText(strGoniometer);
        //ui->lineEditPixelMinTripod->setText(strMinPix);
        //ui->lineEditPixelMaxTripod->setText(strMaxPix);

        if(FrameCount>=0) {
            ui->lineEditNumAcquiredTripod->setText(strFrameCount);
        }

        ui->lineEditTemperaturePaddleColdTripod->setText(strTemperaturePaddleCold);
        ui->lineEditTemperaturePaddleHotTripod->setText(strTemperaturePaddleHot);
        ui->lineEditTemperatureRotorColdTripod->setText(strTemperatureRotorCold);
        ui->lineEditTemperatureRotorHotTripod->setText(strTemperatureRotorHot);
        break;
    case 2:

        if (ui->widgetPixelPlotFlight->rescaleFlag) {
            QString str = QString ("%1").arg(ui->widgetPixelPlotFlight->minVal) ;
            ui->lineEditPixelMinFlight->setText (str) ;
            str = QString ("%1").arg(ui->widgetPixelPlotFlight->maxVal) ;
            ui->lineEditPixelMaxFlight->setText (str) ;
        } else
        {
            float mn, mx ;
            mn = ui->lineEditPixelMinFlight->text().toFloat() ;
            mx = ui->lineEditPixelMaxFlight->text().toFloat() ;
            ui->widgetPixelPlotFlight->setRange (1, mn, mx) ;

        }


        ui->widgetImageFlight->update(); // Redraw the Image (widgetView widget)
        ui->widgetFallFlight->update();  // Redraw the Waterfall (widgetView widget)
        //ui->widgetFallFlight->update();  // Redraw the Waterfall (widgetView widget)
        //ui->widgetPixelPlotFlight->update();  // Redraw the pixel plot
        ui->widgetPixelPlotFlight->updatePixelData();
        ui->widgetFftPlotFlight->updateData();  // Redraw the FFT plot
        //ui->lineEditPixelMinFlight->setText(strMinPix);
        //ui->lineEditPixelMaxFlight->setText(strMaxPix);

        if(FrameCount>=0) {
            ui->lineEditNumAcquiredFlight->setText(strFrameCount);
        }

        ui->lineEditTemperaturePaddleColdFlight->setText(strTemperaturePaddleCold);
        ui->lineEditTemperaturePaddleHotFlight->setText(strTemperaturePaddleHot);
        ui->lineEditTemperatureRotorColdFlight->setText(strTemperatureRotorCold);
        ui->lineEditTemperatureRotorHotFlight->setText(strTemperatureRotorHot);

        sprintf(buffer, "%.2f", Gps.parm.Latitude);
        ui->lineEditLatitude->setText(buffer);
        sprintf(buffer, "%.2f", Gps.parm.Longitude);
        ui->lineEditLongitude->setText(buffer);
        sprintf(buffer, "%.0f", Gps.parm.Altitude);
        ui->lineEditAltitude->setText(buffer);

        sprintf(buffer, "%.2f", Gps.parm.Roll);
        ui->lineEditRoll->setText(buffer);
        sprintf(buffer, "%.2f", Gps.parm.Pitch);
        ui->lineEditPitch->setText(buffer);
        sprintf(buffer, "%.0f", Gps.parm.Heading);
        ui->lineEditHeading->setText(buffer);

        ui->lineEditCurrentMode->setText(Gps.parm.CurrentMode);

        sprintf(buffer, "%d", Gps.parm.GpsValid);
        ui->lineEditGpsValid->setText(buffer);
        sprintf(buffer, "%d", Gps.parm.Satellites);
        ui->lineEditSatellites->setText(buffer);
        sprintf(buffer, "%hd", Gps.parm.Sat1Valid);
        ui->lineEditSV1->setText(buffer);
        sprintf(buffer, "%hd", Gps.parm.Sat2Valid);
        ui->lineEditSV2->setText(buffer);
        sprintf(buffer, "%hd", Gps.parm.Sat3Valid);
        ui->lineEditSV3->setText(buffer);
        sprintf(buffer, "%hd", Gps.parm.Sat4Valid);
        ui->lineEditSV4->setText(buffer);
        sprintf(buffer, "%hd", Gps.parm.Sat5Valid);
        ui->lineEditSV5->setText(buffer);

        sprintf(buffer, "%.2f", Gps.parm.LatitudeInit);
        ui->lineEditLatitudeInit->setText(buffer);
        sprintf(buffer, "%.2f", Gps.parm.LongitudeInit);
        ui->lineEditLongitudeInit->setText(buffer);
        sprintf(buffer, "%d", Gps.parm.AltitudeInit);
        ui->lineEditAltitudeInit->setText(buffer);
        sprintf(buffer, "%.2f", Gps.parm.HeadingInit);
        ui->lineEditHeadingInit->setText(buffer);

        if(Gps.parm.RunningToggle) {
            ui->frameRunningToggle->setStyleSheet("background-color:lime;");
        } else {
            ui->frameRunningToggle->setStyleSheet("background-color:black;");
        }

        break;
    }



    // This is where buttons are re-enabled
    if( !AcqBusy && ButtonsDisabled ) enableButtons(true);



    /*    // Update Scan speed (from Gumstix interface)
    if(SlowSpeed!=LastSlowSpeed) {
        sprintf(strSpeed, "%4.2f" , SlowSpeed);
        ui->lineEditSlowSpeed->setText(strSpeed);
    } else {
        // Update Scan speed (from UI)
        SlowSpeed = ui->lineEditSlowSpeed->text().to    // Get the temperature
    memset (comStringIn, 0, 128);
    cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);


    //Send command to get temperature (So it will be ready when we come through the loop again.)
    strcpy(comStringOut, "TEC:T?\r\n");
    cssl_putstring(SerialBb,comStringOut);Double();
    }
    LastSlowSpeed = SlowSpeed;
*/


    /*    // Update Cold BB (from Gumstix interface)
    if(DesiredTemperatureCold!=LastDesiredTemperatureCold) {
        sprintf(strTemperatureCold, "%4.2f" , DesiredTemperatureCold);
        ui->lineEditDesiredTemperatureCold->setText(strTemperatureCold);
    } else {
        // Update Cold BB (from UI)
        DesiredTemperatureCold = ui->lineEditDesiredTemperatureCold->text().toDouble();
    }
    LastDesiredTemperatureCold=DesiredTemperatureCold;

    // Update Hot BB (from Gumstix interface)
    if(DesiredTemperatureHot!=LastDesiredTemperatureHot) {
        sprintf(strTemperatureHot, "%4.2f" , DesiredTemperatureHot);
        ui->lineEditDesiredTemperatureHot->setText(strTemperatureHot);
    } else {
        // Update Hot BB (from UI)
        DesiredTemperatureHot = ui->lineEditDesiredTemperatureHot->text().toDouble();
    }
    LastDesiredTemperatureHot=DesiredTemperatureHot;
*/



    //// Update differential value display
    //sprintf(strDifferentialSummation, "%.2f" , FinalDifferentialSummation);
    //ui->lineEditAverage->setText(strDifferentialSummation);


    //sprintf(strDifferentialSummation, "%.2f" , LastBestValue);
    //ui->lineEditLastBestValue->setText(strDifferentialSummation);


}

void MainWindow::onLogEntry()
{
    //float CameraTemperature;
    //char strCameraTemperature[64];
    char Message[1024];

#ifdef __unix__
    // Get camera temperature
    switch(CameraType) {
    case MC320:
        //CameraMc320.;
        break;
    case TAU640:
        sprintf(strCameraTemperature, "%.2f" , TauCommand.Get());
        ui->lineEditCameraTemperatureBench->setText(strCameraTemperature);
        break;
    case PV640:
        sprintf(strCameraTemperature, "%d" , CameraPv640.GetTemperature());
        ui->lineEditCameraTemperatureBench->setText(strCameraTemperature);
        break;
    }
#endif
    /*    sprintf(Message, "  %2.1f,       %2.1f,      %2.1f,     %2.1f,    %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f,   %2.1f, %2.1f,   %2.1f, %2.1f, %2.1f, %2.1f" ,
            PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot,
            TemperatureSensor[0], TemperatureSensor[1], TemperatureSensor[2], TemperatureSensor[3], TemperatureSensor[4], TemperatureSensor[5],
            CameraTemperature,
            PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient, PresentPositionRotor,
            PresentPositionRotation, PresentPositionGoniometer);*/

    sprintf(Message, "  %4.1f,       %4.1f,      %4.1f,     %4.1f,    %4.1f, %4.1f, %4.1f, %4.1f, %4.1f, %4.1f, %4.1f,   %3d,       %3d,       %3d,     %s,  %5.3f, %5.3f" ,
            PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot,
            TemperatureSensor[0], TemperatureSensor[1], TemperatureSensor[2], TemperatureSensor[3], TemperatureSensor[4], TemperatureSensor[5],
            CameraTemperature,
            PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient, PresentPositionRotor,
            PresentPositionRotation, PresentPositionGoniometer);


    //:PLog(Message);
}

// *****************************************************************
// Monitor Cold BlackBody Temperature TimerEvent
// *****************************************************************
void MainWindow::onProgress()
{
    int PctDone;

    // Update display values.
    if(FrameCount>=0) {
        PctDone = (int)((float)FrameCount*100.0/(float)NumFrames);
        if(PctDone>100) PctDone=100;

        switch(CurrentTab) {
        case 0:
            switch(TircisStatus) {
            case MANUALACQUIRE:
                ui->progressBarManualBench->setValue( PctDone );  // Update Progress bar
                break;
            case BBCOLD:
                ui->progressBarCalBench->setValue( PctDone );  // Update Progress bar
                break;
            case BBHOT:
                ui->progressBarCalBench->setValue( PctDone );  // Update Progress bar
                break;
            case WAVECAL:
                ui->progressBarCalBench->setValue( PctDone );  // Update Progress bar
                break;
            }
        case 1:
            switch(TircisStatus) {
            case BBCOLD:
                ui->progressBarCalTripod->setValue( PctDone );  // Update Progress bar
                break;
            case BBHOT:
                ui->progressBarCalTripod->setValue( PctDone );  // Update Progress bar
                break;
            case WAVECAL:
                ui->progressBarCalTripod->setValue( PctDone );  // Update Progress bar
                break;
            }
        case 2:
            switch(TircisStatus) {
            case MANUALACQUIRE:
                ui->progressBarFlight->setValue( PctDone );  // Update Progress bar
                break;
            case BBCOLD:
                ui->progressBarCalFlight->setValue( PctDone );  // Update Progress bar
                break;
            case BBHOT:
                ui->progressBarCalFlight->setValue( PctDone );  // Update Progress bar
                break;
            case WAVECAL:
                ui->progressBarCalFlight->setValue( PctDone );  // Update Progress bar
                break;
            }
        }

        if(PctDone==100) TircisStatus = IDLE;
    }
}

#ifdef OBSOLETE
void MainWindow::FindDevices(int *RotationAPortNum, int *BbPortNum)
{
    char strBbAComPort[16];
    char strBbBComPort[16];
    char strRotationAComPort[16];
    /*
    FindRotationAPort(RotationAPortNum, BbPortNum);
    FindBbPort(RotationAPortNum, BbPortNum);

    sprintf(strRotationAComPort, "COM%d", *RotationAPortNum+1);
    sprintf(strBbAComPort, "COM%d", *BbPortNum+1);
*/
    // Display on Bench Tab
    //    if(CurrentTab==0) {
    //        ui->lineEditBbAComPort->setText(strBbAComPort);
    //        ui->lineEditBbAComPort->setText(strBbBComPort);
    //        ui->lineEditRotationComPort->setText(strRotationComPort);
    //    }
}
#endif



#ifdef PIGS
// *****************************************************************
// Monitor Cold BlackBody Temperature TimerEvent
// *****************************************************************
void MainWindow::MonitorTemperatureBbUnit1()
{
    int nBytes=255;
    //float DesiredTemp;
    char comStringIn[256];
    char comStringOut[256];


    if( !BbPortOpen ) return;
    if( !SerialBb ) return;

    //If the temperature is being set, skip getting the temperature this time.
    if(BbBusy) return;
    BbBusy = true;


    // Get the temperature
    memset (comStringIn, 0, 128);
    cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);


    //Send command to get temperature (So it will be ready when we come through the loop again.)
    strcpy(comStringOut, "TEC:T?\r\n");
    cssl_putstring(SerialBb,comStringOut);


    //Print the temperature to the main window.
    char *NewLinePos = (strstr(comStringIn, "\r"));
    if(NewLinePos!=NULL) {
        *NewLinePos = (char)0;
    }
    PresentTemperatureBbA = atof( comStringIn ); // PresentTemperatureBb1 is global for use elsewhere


    switch(CurrentTab) {
    case 0:
        ui->lineEditTemperatureBbABench->setText(comStringIn);
        break;
    case 1:
        ui->lineEditTemperatureBbATripod->setText(comStringIn);
        break;
    case 2:
        ui->lineEditTemperatureBbAFlight->setText(comStringIn);
        break;
    }



#ifdef SETTLED
    // Flag what the desired tempearture is
    switch( BBLevel ) {
    case TEMPERATURECOLD:
        DesiredTemp = m_nBbUnit1Value;
        break;
    case TEMPERATUREHOT:
        DesiredTemp = m_nBBHotValue;
        break;
    case TEMPERATUREOTHER:
        DesiredTemp = m_nBBOtherValue;
        break;
    }

    // Then figure out if we reached the desired temperature
    if( fabs(PresentTemperatureBb1-DesiredTemp) <= 0.45 ) {
        BbSettledCold = TRUE;
    } else {
        BbSettledCold = FALSE;
    }
#endif

    BbBusy = false;
}
#endif
// *****************************************************************
// Monitor RotationA Position TimerEvent
// *****************************************************************
void MainWindow::MonitorPositionA()
{
    /*    char comString[256], buffer[256];
    //float DesiredTemp;
    int length, nBytes=255;

    if( !RotationAPortOpen ) return;
    if( !SerialRotationA ) return;

    //If the position is being set, skip getting the position this time.
    if(RotationABusy) return;
    RotationABusy = true;

    // Get the slide position
    //usleep(100000);
    memset (comString, 0, 128);
    cssl_getdata(SerialRotationA, (uint8_t*)comString, nBytes);


    //Print the position to the main window.
    if( strlen(comString)>1 ) {
        //strcpy(buffer, &comString[3]);
        length = strlen( buffer );  // Give 2 places accuracy instead of 4 places NOT
        strcpy(buffer, &comString[0]);
        buffer[length] = '\0';
        PresentPosition = atof(buffer);	//SMC100
    }
    else {
        PresentPosition = 9999;
    }

    // Subtract the "program home position" when displaying to the user (otherwise always use absolute values)
    //sprintf(buffer, "%4.2f" , PresentPosition - HomePosition);
    //ui->lineEditPresentPositionABench->setText(buffer);


    //Send command to get RotationA Position (So it will be ready when we come through the loop again.)
    strcpy(comString, "1TP\r\n");	//SMC100 get position string
    cssl_putstring(SerialRotationA,comString);

    RotationABusy = false;*/
}

#ifdef DELETEME
// *****************************************************************
// Monitor Focus Position TimerEvent
// *****************************************************************
void MainWindow::MonitorFocusPosition()
{
    char comString[256], buffer[256];
    //float DesiredTemp;
    //int length,
    int nBytes=255;
    char *pBuffer, *pBuffer2;

    if( !FocusPortOpen ) return;
    if( !SerialFocus ) return;

    //If the position is being set, skip getting the position this time.
    if(FocusBusy) return;
    FocusBusy = true;


    //Send command to get Focus Position
    strcpy(comString, "/GMS1[2]\r\n");
    cssl_putstring(SerialFocus,comString);

    //usleep(100000);

    // Get the Focus position
    memset (comString, 0, 255);
    cssl_getdata(SerialFocus, (uint8_t*)comString, nBytes);


    //Print the position to the main window.
    if( strlen(comString)>1 ) {
        strcpy(buffer, comString);
        pBuffer = strstr(buffer, "GMS1[2]\r");
        if(pBuffer!=NULL) {
            pBuffer+=8;
            pBuffer2 = strstr(buffer, " ");
            if( pBuffer2 != NULL ) *pBuffer2 = 0;
            PresentFocus = atoi(pBuffer);	//SMC100
        } else {
            PresentFocus = 9999;
        }
    }
    else {
        PresentFocus = 9999;
    }

    // Do this in main timer
    //    // Display focus position to the user
    //    sprintf(buffer, "%4.2f" , PresentFocus);
    //    ui->lineEditFocusPosition->setText(buffer);


    /*
    // Do autofocus if on
    double StartSum = FinalDifferentialSummation;

    int StepSize = 5000;
    if(false) {
        if( PresentFocus !=9999) {

            char units[2]={'e', 0}; //e-counts, p-percent (NULL terminated)
            //FocusPosition = 100;
            int FocusSpeed = 50;
            short VelocityTimeFlag = 0;   //0-velocity, 1-time. 2-ramp


            // Move focus out
            PresentFocus += StepSize;
            sprintf(comString, "/MPA1 %d, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
            cssl_putstring(SerialFocus, comString);
            usleep(500000); // Wait for move to finish

            // If focus is not better move back in
            if(StartSum >= FinalDifferentialSummation) {
                // Move focus back in and then another step in (the other direction)
                PresentFocus -= 2*StepSize;
                sprintf(comString, "/MPA1 %d, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
                cssl_putstring(SerialFocus, comString);
                usleep(500000); // Wait for move to finish

                if(StartSum >= FinalDifferentialSummation) {
                    // If focus is not better move back out (to original position)
                    PresentFocus += StepSize;
                    sprintf(comString, "/MPA1 %d, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
                    cssl_putstring(SerialFocus, comString);
                    usleep(500000); // Wait for move to finish
                }
            }


/ *
            // Move focus out

            // If focus is better
                // While it is better keep moving out


            // Else focus in

            // If focus is better
                // While it is better keep moving in
* /


#ifdef PIGS            // Step focus motor out to see if improvement
            PresentFocus += StepSize;

            char units[2]={'e', 0}; //e-counts, p-percent (NULL terminated)
            //FocusPosition = 100;
            int FocusSpeed = 50;
            short VelocityTimeFlag = 0;   //0-velocity, 1-time. 2-ramp

            //Set Focus Position (and speed)
            sprintf(comString, "/MPA1 %5.3f, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
            cssl_putstring(SerialFocus, comString);

            if(StartSum < FinalDifferentialSummation) {
                PresentFocus += StepSize;
                StartSum = FinalDifferentialSummation;
                //Set Focus Position (and speed)
                sprintf(comString, "/MPA1 %5.3f, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
                cssl_putstring(SerialFocus, comString);
            }

            if(StartSum < FinalDifferentialSummation) {
                PresentFocus += StepSize;
                StartSum = FinalDifferentialSummation;
                //Set Focus Position (and speed)
                sprintf(comString, "/MPA1 %5.3f, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
                cssl_putstring(SerialFocus, comString);
            }

            if(StartSum < FinalDifferentialSummation) {
                PresentFocus += StepSize;
                StartSum = FinalDifferentialSummation;
                //Set Focus Position (and speed)
                sprintf(comString, "/MPA1 %5.3f, %s, %d, %d\r\n" , PresentFocus, units, FocusSpeed, VelocityTimeFlag);
                cssl_putstring(SerialFocus, comString);
            }
#endif
        }
    }
*/
    usleep(10000);

    /*    //Send command to get Focus Position (So it will be ready when we come through the loop again.)
    strcpy(comString, "/GMS1[2]\r\n");
    cssl_putstring(SerialFocus,comString);*/

    //usleep(100000);
    FocusBusy = false;
}
#endif




/*// *****************************************************************
// User pushed the Scan button.
// *****************************************************************
void MainWindow::on_pushButtonCollectScan_clicked()     // RotationA mode
{
    // Reset Abort Flag
    Abort = false;
    if(SlowSpeed<=0.0) return;

    strcpy( DataDirectory, ui->lineEditDataDirectory->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         // Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    //StartPosition = ui->lineEditRotationAStartPosition->text().toFloat();       // Get start position
    //StopPosition = ui->lineEditRotationAStopPosition->text().toFloat();         // Get stop position
    DesiredTemperatureHot = ui->lineEditDesiredTemperatureHot->text().toFloat();
    DesiredTemperatureCold = ui->lineEditDesiredTemperatureCold->text().toFloat();
    //SlowSpeed = ui->lineEditSlowSpeed->text().toFloat();
    NumCalFrames = ui->lineEditNumCalFrames->text().toInt();
    NumTargetFrames = fabs(StopPosition-StartPosition+SCANALIGNMENTBUFFERWIDTH)*FrameRate/SlowSpeed;

    // Start scaning
    AcqBusy = true;
    enableButtons(false);
    sem_post (&RunScan_count);
}
*/

// *****************************************************************
// User pushed the Acquire Manual button.
// *****************************************************************
void MainWindow::on_pushButtonAcquireManualBench_clicked()
{
    strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment

    NumFrames = ui->lineEditNumFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireManual(TProc);
}

/* // *****************************************************************
// User pushed the WaveCal button.
// *****************************************************************
void MainWindow::on_pushButtonWaveCal_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );                   //Get Wavelength
    ShortComment[0] = 0;
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireWaveCal();
}*/

/*// *****************************************************************
// User pushed the collect Cold button.
// *****************************************************************
void MainWindow::on_pushButtonSaveCold_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectory->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFrames->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireCold();
}

// *****************************************************************
// User pushed the Collect Hot button.
// *****************************************************************
void MainWindow::on_pushButtonSaveHot_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectory->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFrames->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireHot();
}*/

/*// *****************************************************************
// User pushed the ViewRange button.
// *****************************************************************
void MainWindow::on_pushButtonViewRange_clicked()
{
    // Reset Abort Flag
    Abort = false;

    StartPosition = ui->lineEditRotationAStartPosition->text().toFloat();
    StopPosition = ui->lineEditRotationAStopPosition->text().toFloat();

    sem_post (&ViewRange_count);
}*/

/*// *****************************************************************
// User pushed the Set Temperature Cold button.
// *****************************************************************
void MainWindow::on_pushButtonSetCold_clicked()
{
    // Get the desired temperature from the line edit.
    DesiredTemperature = ui->lineEditDesiredTemperatureCold->text().toFloat();
    sem_post (&ChangeTemperature_count);
}

// *****************************************************************
// User pushed the Set Temperature Hot button.
// *****************************************************************
void MainWindow::on_pushButtonSetHot_clicked()
{
    // Get the desired temperature from the line edit.
    DesiredTemperature = ui->lineEditDesiredTemperatureHot->text().toFloat();
    sem_post (&ChangeTemperature_count);
}*/

/*// *****************************************************************
// User pushed the Set Position button.
// *****************************************************************
void MainWindow::on_pushButtonSetRotationAPosition_clicked()
{
    // Get the desired position from the line edit.
    DesiredPosition = HomePosition + ui->lineEditDesiredRotationAPosition->text().toFloat();
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}*/

/*// *****************************************************************
// User pushed the Home button.
// *****************************************************************
void MainWindow::on_pushButtonMoveHome_clicked()
{
    // Move to Home.
    DesiredPosition = HomePosition;
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}
*/
/*// *****************************************************************
// User pushed the Move to BB button.
// *****************************************************************
void MainWindow::on_pushButtonMoveBB_clicked()
{
    // Move to BB.
    DesiredPosition = BBPosition;
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}
*/
/*// *****************************************************************
// User pushed Reopen serial ports button.
// *****************************************************************
void MainWindow::on_pushButtonReopenSerialPorts_clicked()
{
    int RotationAPortNum, BbPortNum, GumstixPortNum, FocusPortNum;
    //int BbRetVal, RotationARetVal;

    // Don't need this since we are using UDEV
    return;

    // Figure out wich serial port each device is on.
    RotationAPortNum = 98;   // Set default value
    BbPortNum = 98;      // Set default value
    GumstixPortNum = 98; // Set default value
    FocusPortNum = 98; // Set default value


    if(SerialRotationA) {
        cssl_drain(SerialRotationA);
        cssl_close(SerialRotationA);
    }
    if(SerialBb) {
        cssl_drain(SerialBb);
        cssl_close(SerialBb);
    }
    if(SerialGumstix) {
        cssl_drain(SerialGumstix);
        cssl_close(SerialGumstix);
    }
    if(SerialFocus) {
        cssl_drain(SerialFocus);
        cssl_close(SerialFocus);
    }



    FindDevices(&RotationAPortNum, &BbPortNum);

    if(RotationAPortNum<98) RotationAPortOpen = OpenRotationA(&SerialRotationA, RotationAPortNum, &RotationABusy);
    if(BbPortNum<98) BbPortOpen = OpenBb(&SerialBb, BbPortNum, &BbBusy);
}
*/

// *****************************************************************
// ABORT
// *****************************************************************
void MainWindow::on_pushButtonAbort_clicked()
{
    //char comString[256];
    Abort = true;

    // Wait if position monitor is getting the position (put a timeout here)
    /*while(RotationABusy) {
        usleep(100000);
    }*/
    //RotationABusy = true;

    //Stop RotationA Stage.
    //sprintf (comString, "1ST\r\n" );
    //cssl_putstring(SerialRotationA, comString);
    //Rotation.Halt();
    //Goniometer.Halt();

    /*    //Home RotationA Stage.
    sprintf (comString, "1OR?\r\n" );
    cssl_putstring(SerialRotationA, comString);*/
    //RotationABusy = false;

    COSMOS_USLEEP(300000);
    DesiredPositionRotation = PresentPositionRotation;
    DesiredPositionGoniometer = PresentPositionGoniometer;

    // Enable buttons
    enableButtons(true);
}

void MainWindow::ShutDown()
{
    ExitProgram=true;

#ifdef __unix__
    sem_post(&SaveStart_count);
    sem_post(&SaveDone_count);
    sem_post(&AcquireDone_count);
    //    sem_post(&ChangeTemperatureA_count);
    sem_post(&ChangeTemperatureB_count);
    sem_post(&ChangeTemperatureExt_count);
    sem_post(&MovePosition_count);
    sem_post(&ViewRange_count);
    sem_post(&AcquireFinished_count);
    sem_post(&RunCalAcq_count);
    sem_post(&RunManualAcq_count);
    sem_post(&RunScan_count);
#else
    SetEvent(SaveStart_count);
    SetEvent(SaveDone_count);
    SetEvent(AcquireDone_count);
    //    SetEvent(ChangeTemperatureA_count);
    SetEvent(ChangeTemperatureB_count);
    SetEvent(ChangeTemperatureExt_count);
    SetEvent(MovePosition_count);
    SetEvent(ViewRange_count);
    SetEvent(AcquireFinished_count);
    SetEvent(RunCalAcq_count);
    SetEvent(RunManualAcq_count);
    SetEvent(RunScan_count);
#endif

    // Get values from UI
    //SlowSpeed = ui->lineEditSlowSpeed->text().toFloat();
    NumFrames = (int)ui->lineEditNumFramesBench->text().toFloat();
    //StartPosition = ui->lineEditRotationAStartPosition->text().toFloat();
    //StopPosition = ui->lineEditRotationAStopPosition->text().toFloat();

    switch(CurrentTab) {
    case 0:
        DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdBench->text().toFloat();
        DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotBench->text().toFloat();
        DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdBench->text().toFloat();
        DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotBench->text().toFloat();
        DesiredTemperatureBbExt = ui->lineEditDesiredTemperatureExtBench->text().toFloat();
        DesiredTemperatureBbExt2 = ui->lineEditDesiredTemperatureExt2Bench->text().toFloat();
        //DesiredTemperatureExtHot = ui->lineEditDesiredTemperatureExtHotBench->text().toFloat();
        break;
    case 1:
        DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdTripod->text().toFloat();
        DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotTripod->text().toFloat();
        DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdTripod->text().toFloat();
        DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotTripod->text().toFloat();
        break;
    case 2:
        DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdFlight->text().toFloat();
        DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotFlight->text().toFloat();
        DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdFlight->text().toFloat();
        DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotFlight->text().toFloat();
        break;
    }
    //strcpy( UchisVersion, ui->lineEditUchisVersion->text().toAscii() );

    // Save settings to config file.
    WriteConfig(GoniometerHome, RotationHome,
                BBRotationPosition, SlowSpeedRotation, StartPositionRotation, StopPositionRotation, NumFrames,
                DesiredTemperaturePaddleCold, DesiredTemperaturePaddleHot,
                DesiredTemperatureRotorCold, DesiredTemperatureRotorHot,
                DesiredTemperatureBbExt, DesiredTemperatureBbExt2,
                DataDirectory, ProcessingDirectory, UchisVersion);


    //if (SerialBb) cssl_close(SerialBb);
    //if (SerialRotationA) cssl_close(SerialRotation);
    //if (SerialGumstix) cssl_close(SerialGumstix);
    //if (SerialFocus) cssl_close(SerialFocus);
    BBExternal.Kill();
    BBExternal2.Kill();
    Motion.Kill();
    //Rotation.Kill();

    //close?????


#ifdef __unix__
    cssl_stop();
#endif

#ifdef __unix__
    // Shut down camera
    switch(CameraType) {
    case MC320:
        CameraMc320.ShutdownCamera();
        break;
    case TAU640:
        CameraTau.ShutdownCamera();
        break;
    case PV640:
        CameraPv640.ShutdownCamera();
        break;
    }
#else
    switch(CameraType) {
    case PHOTON:
        CameraPhoton.ShutdownCamera();
        break;
    case INO:
        CameraINO.ShutdownCamera();
        break;
    }
#endif

    //:PLog((char*)"User exited program");
    CloseLog();
    CloseLogCmd();

    delete QImagePlotFft;
    delete ImgPlotFft;
    delete QImagePlotPixel;
    delete ImgPlotPixel;
    delete QImageCamera;
    delete ImgCamera;
    delete QImageFall;
    delete ImgFall;

    delete pSaveBuffer;
    //delete pSaveBufferGPS;
    delete pSaveBufferEncoder;



    //WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    CloseHandle(ThreadSavedata);
    CloseHandle(ThreadAcquire);
    CloseHandle(ThreadWaterfall);
    CloseHandle(ThreadRotationA);
    CloseHandle(ThreadCalAcq);
    CloseHandle(ThreadManualAcq);
    CloseHandle(ThreadScan);
    CloseHandle(ThreadAutoscan);
    CloseHandle(ThreadScript);

}

// *****************************************************************
// EXIT THE PROGRAM
// *****************************************************************
void MainWindow::on_pushButtonExit_clicked()
{
    //ShutDown();
    qApp->quit();
}

#ifdef __unix__
void* runWaterfall(void *args)
#else
DWORD WINAPI runWaterfall(LPVOID pParam)
#endif
{
    //uInt32        j, k;
    //uInt32        AcqImgWidth;
    //uInt32        AcqImgHeight;
    //unsigned short *Input;
    uchar *ImgData2;
    short Intensity;
    //ImgData2 = (uchar*)args; // Let this "friend function" access ImgData
    ImgData2 = ImgFall;

    uInt32 WaterfallLineNum = 0;

    unsigned short *pData;
    unsigned short *pBufferFall;
    BufferFall = new unsigned short[WATERFALLSIZE];
    if (BufferFall==NULL) exit(-90);
    //memset(BufferFall, 96, WATERFALLSIZE*sizeof(unsigned short));
    memset(BufferFall, 0, WATERFALLSIZE*sizeof(unsigned short));

    //pthread_exit(NULL);

    while(!ExitProgram) {

        int MaxVal = 10000;
        int MinVal = 0;

        // Autoscale data
        MaxVal = -1;
        MinVal = 0x3FFF;
        for(unsigned i=0 ; i<AcqImgSize ; i++) {

            if((MaxVal<m_nPhotonFrameBuffer[i]) /*&& (m_nPhotonFrameBuffer[i]<35000)*/)
                MaxVal=m_nPhotonFrameBuffer[i];
            //if((i!=6385)&&(i!=10266)&&(i!=10587)&&(i!=28967)&&(i!=31323)&&(i!=50388)&& (MinVal>m_nPhotonFrameBuffer[i]) && (m_nPhotonFrameBuffer[i]>300))
            if(MinVal>m_nPhotonFrameBuffer[i])
                MinVal=m_nPhotonFrameBuffer[i];
        }
        if(MaxVal == MinVal) {
            MaxVal = MinVal + 1;
        }

        /*
        Input = m_nPhotonFrameBuffer;
        for(int i=0 ; i<IMAGESIZE*3 ; i+=3) {
            if(ExitProgram) break;
            Intensity = (int)( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5);
            memset(ImgData, Intensity, 3);
            ImgData+=3;
            Input++;
        }

        usleep(1000000);
    }*/


        WaterfallLineNum++;
        if( WaterfallLineNum>WATERFALLHEIGHT ) {
            WaterfallLineNum = 0;
        }

        //::WaitForSingleObject(m_hDisplayUIDoneEvent, INFINITE);	//Wait for Message from Syncronizer thread.
        if (ExitProgram) break;	// Exit if program is terminating.



        // Blit Photon Line (band 128) to Waterfall buffer.
        //BitBlt( m_hdcWaterfall , 0,m_nWaterfallLineNum , IMAGEHEIGHT,1 , m_hdcPhotonLineBuffer, 0,0 , SRCCOPY);

        //int WaterfallBand = AcqImgWidth/2;
        int WaterfallBand = AcqImgHeight/2;
        //int WaterfallBand = 4;
        //int WaterfallBand = 256;
        //for(i=0 ; i<IMAGEHEIGHT ; i++) *pBufferLine = m_nPhotonFrameBuffer[WaterfallBand + i*IMAGEWIDTH];
        pBufferFall = &BufferFall[WaterfallLineNum*DISPLAYWIDTH];

        //memcpy(pBufferFall, &m_nPhotonFrameBuffer[WaterfallBand*AcqImgWidth], AcqImgWidth * sizeof(unsigned short));
        pData = &m_nPhotonFrameBuffer[WaterfallBand*AcqImgWidth];
        for(int i=0 ; i<WATERFALLWIDTH ; i++) {
            pBufferFall[i] = *pData;
            pData+=(AcqImgWidth/DISPLAYWIDTH);
        }


        //Blit Top Portion of Waterfall
        if( WaterfallLineNum < WATERFALLHEIGHT-1 ) {
            //			BitBlt( hdcPaint, /*13+268*/750,/*13*/12 , IMAGEHEIGHT,(WATERFALLHEIGHT-1) - m_nWaterfallLineNum,
            //									m_hdcWaterfall , 0,m_nWaterfallLineNum+1 , SRCCOPY);
            //            BitBlt( hdcPaint, WaterFallLeft, WaterFallTop, IMAGEHEIGHT,(WATERFALLHEIGHT-1) - m_nWaterfallLineNum, m_hdcWaterfall , 0,m_nWaterfallLineNum+1 , SRCCOPY);

            pBufferFall = &BufferFall[(WaterfallLineNum+1) * DISPLAYWIDTH];
#ifdef __unix__
            ImgData2 = (uchar*)args; // Let this "friend function" access ImgData
#else
            ImgData2 = ImgFall;
#endif
            for(unsigned i=0 ; i<( (WATERFALLHEIGHT-1) - WaterfallLineNum ) * WATERFALLWIDTH ; i++) {
                if(ExitProgram) break;
                Intensity = (int)( 255 * (double)(*pBufferFall-MinVal) / (double)(MaxVal-MinVal+1) + .5);
                if(Intensity>255)
                    Intensity=255;
                if(Intensity<0)
                    Intensity=0;
                memset(ImgData2, Intensity, 3);
                ImgData2+=3;
                pBufferFall++;
            }

        }


        //Blit Bottom Portion of Waterfall
        //		BitBlt( hdcPaint , /*13+268*/750,/*13*/12 + (WATERFALLHEIGHT-1)-m_nWaterfallLineNum, IMAGEHEIGHT, m_nWaterfallLineNum+1,
        //									m_hdcWaterfall, 0,0 , SRCCOPY);
        //        BitBlt( hdcPaint , WaterFallLeft,WaterFallTop + (WATERFALLHEIGHT-1)-m_nWaterfallLineNum, IMAGEHEIGHT, m_nWaterfallLineNum+1, m_hdcWaterfall, 0,0 , SRCCOPY);

        pBufferFall = BufferFall;
#ifdef __unix__
        ImgData2 = (uchar*)(args + (WATERFALLHEIGHT-WaterfallLineNum)*WATERFALLWIDTH*3); // Let this "friend function" access ImgData
#else
        ImgData2 = (uchar*)(ImgFall + (WATERFALLHEIGHT-WaterfallLineNum)*WATERFALLWIDTH*3); // Let this "friend function" access ImgData
#endif
        for(unsigned i=0 ; i<WaterfallLineNum * WATERFALLWIDTH ; i++) {
            if(ExitProgram) break;
            Intensity = (int)( 255 * (*pBufferFall-MinVal) / (MaxVal-MinVal+1) + .5);
            memset(ImgData2, Intensity, 3);
            ImgData2+=3;
            pBufferFall++;
        }


        /*        Input = BufferFall;

        for(int i=0 ; i<WATERFALLSIZE*3 ; i+=3) {
            if(ExitProgram) break;
            Intensity = (int)( 255);
            Intensity = (int)( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5);
            memset(ImgData2, Intensity, 3);
            ImgData2+=3;
            Input++;
        }*/

        COSMOS_USLEEP(100000);
    }

    //if (BufferFall!=NULL) delete(BufferFall);

#ifdef __unix__
    pthread_exit(NULL);
#else
    return(0);
#endif
}


//#ifdef REMOVEXX
//// *****************************************************************
//// Get data from camera
//// *****************************************************************
//void* runAcquisition(void *args)
//{
//    uInt32        i, j, k;
//    //uInt32        AcqImgWidth;
//    //uInt32        AcqImgHeight;
//    unsigned short *Input;
//    uchar *ImgData;
//    uchar Intensity;
//    ImgData = (uchar*)args; // Let this "friend function" access ImgData
//    bool InitAcq = false;

//    double DifferentialSummation;
//    uInt32 PixelPtr;
//    uInt16 CutoffVal;
//    uInt32 NumHighVals;

//    static uInt32 TMinus[64];
//    double SumTMinus;   //, MaxDifferential;

//    // Clear TMinus array (this array is to average the last 7 differential values)
//    memset(TMinus, 0, 7*sizeof(uInt32));



//    /*    // Initialize Camera
//    CameraRetVal = Camera.InitCamera(&AcqImgWidth, &AcqImgHeight);
//    if(  CameraRetVal < 0 ) {
//        //exit(-73);
//        //QMessageBox::warning(0, "Application Name", "A warning message." ); (this is not part of Qt gui)
//        //pthread_exit(NULL);
//        return(&CameraRetVal);
//    } else {
//*/
//    // Start the camera
//    Camera.StartCamera();
//    pBuffer = pSaveBuffer;
//    /*        InitAcq = true;
//    }*/

//    while(!ExitProgram) {
//Abort3:
//        // ***************************************************************************************************
//        // Wait for START
//        // ***************************************************************************************************

//        if (ExitProgram) {
//            sem_post (&AcquireDone_count);  // If exiting, let Save routine stop waiting and exit.
//            break;
//        }

//        Camera.GetCameraFrame(m_nPhotonFrameBuffer);



//        /*        // Get second differential for autofocus
//        k=0;
//        if(AutoFocusOn) {
//            memset(ImageSecondDif, 0, sizeof(double)*(IMAGESIZE-IMAGEHEIGHT));

//            // Get second differential of each image line.
//            for(j=0 ; j<IMAGEHEIGHT-1 ; j++) {
//                for(i=1 ; i<IMAGEWIDTH-2 ; i++) {
//                    PixelPtr = j*IMAGEWIDTH + i;
//                    //ImageSecondDif[k] += ( (double)m_nPhotonFrameBuffer[PixelPos+1] - 2.0*(double)m_nPhotonFrameBuffer[PixelPos] + (double)m_nPhotonFrameBuffer[PixelPos-1] ) / IMAGESIZE;
//                    ImageSecondDif[k] += ( (double)m_nPhotonFrameBuffer[PixelPtr+1] - 2.0*(double)m_nPhotonFrameBuffer[PixelPtr] + (double)m_nPhotonFrameBuffer[PixelPtr-1] );
//                    k++;
//                }
//            }



//            // ***** Sum up the number of values that are greater than the cuttoff (cutoff is by trial and error)
//            //CutoffVal = 300;
//            CutoffVal = 100;
//            NumHighVals = 0;
//            DifferentialSummation = 0;

//            //long MaxDiffVal = -9999;
//            for(i=0 ; i<IMAGESIZE-IMAGEWIDTH ; i++) {
//                if( ImageSecondDif[i] > CutoffVal ) {
//                    DifferentialSummation += ImageSecondDif[i];
//                    NumHighVals++;
//                }
//            }
//            //FinalDifferentialSummation = DifferentialSummation;
//            //FinalDifferentialSummation = NumHighVals;



//            // ***** Average the last 7 diff values
//            // Fill in the next TMinus set.  (each gets shifted back one)
//            for(i=10-1 ; i>0 ; i--) {
//                TMinus[i] = TMinus[i-1];
//            }
//            //TMinus[0] = DifferentialSummation;
//            TMinus[0] = NumHighVals;

//            // Get the average of the last 7 (333ms seconds worth)
//            SumTMinus = 0;
//            for(i=0 ; i<10 ; i++) {
//                SumTMinus += (int)TMinus[i];
//            }
//            FinalDifferentialSummation = SumTMinus / 10.0;


/// *for(i=0 ; i<IMAGESIZE - IMAGEHEIGHT ; i++) {
//    ImageHist[ (int)(ImageSecondDif[i]-CutoffVal) ]++;
//}* /

//        }
//*/


//        // If Running, wait for save data routine to finish
//        if( SavingData ) {

//            if(Abort) {
//                sem_post (&AcquireDone_count);
//                goto Abort3;
//            }


//            memcpy( pBuffer, m_nPhotonFrameBuffer, AcqImgSize * sizeof(unsigned short) );
//            pBuffer+=AcqImgSize; // Move pointer to the next image space in the buffer.

//            //memcpy( pBufferGPS, m_pGPSBuffer+m_nGPSIndex, GPSSIZE );
//            //pBufferGPS+=GPSSIZE;

//            //memcpy( pBufferEncoder, &DesiredPosition, sizeof(float) );
//            //*pBufferEncoder = DesiredPosition;
//            //pBufferEncoder++;

//            if( FrameCount%60 == 0 ) {

//                if( BufNum==0 ) {
//                    BufNum=1;
//                    pBuffer = pSaveBuffer + (60L*AcqImgSize);	// There are two 60 image buffers
//                    //pBufferGPS = m_pSaveBufferGPS + (60L*GPSSIZE); // we toggle between them.
//                    //pBufferEncoder = m_pSaveBufferEncoder + (60L); // we toggle between them.
//                } else {
//                    BufNum=0;
//                    pBuffer = pSaveBuffer;
//                    //pBufferGPS = m_pSaveBufferGPS;
//                    //pBufferEncoder = m_pSaveBufferEncoder;
//                }
//                if( FrameCount != 0 ) {
//                    if(!InitAcq) {
//                        //Wait for Message from Syncronizer thread.
//                        sem_wait (&SaveDone_count);
//                        if(ExitProgram) break;
//                    } else {
//                        InitAcq=false;
//                    }

//                    NumFramesToSave = 60;
//                    sem_post (&AcquireDone_count);
//                }
//            }
//            if( FrameCount==NumFrames ) {
//                SavingData = false;
//                if( NumFrames%60 != 0 ) {
//                    BufNum = !BufNum;
//                    NumFramesToSave = NumFrames%60;
//                    sem_post (&AcquireDone_count);
//                }
//            } else {
//                FrameCount++;
//            }
//        }


//        /*        // *** Calculate the number of frames per seconds every second
//        nbFrame++;
//        if (nbFrame>=FrameRate) {
//            t2 = timeGetTime();
//            FramesPerSec = 1000.0 * (double)nbFrame / (double)(t2-t1);
//            t1 = t2;
//            nbFrame=0;
//        }*/

//        if (ExitProgram) {
//            break;
//        }

//        ImgData = (uchar*)args; // Let this "friend function" access ImgData
//        int MaxVal = 10000;
//        int MinVal = 0;

//        // Autoscale data
//        MaxVal = -99999;
//        MinVal = 99999;
//        for(int i=0 ; i<AcqImgSize ; i++) {
//            if(MaxVal<m_nPhotonFrameBuffer[i]) MaxVal=m_nPhotonFrameBuffer[i];
//            if(MinVal>m_nPhotonFrameBuffer[i]) MinVal=m_nPhotonFrameBuffer[i];
//        }
//        //MinVal = 22400;
//        //MaxVal = 27000;

//        /*      Input = m_nPhotonFrameBuffer;
//        for(int i=0 ; i<AcqImgSize ; i++) {
//            if(ExitProgram) break;
//            Intensity = (int)( 255 * (float)(*Input-MinVal) / (MaxVal-MinVal+1) + .5);
//            memset(ImgData, Intensity, 3);
//            ImgData+=3;
//            Input++;
//        }*/


//        // Fill GUI image buffer
//        for(int i=0 ; i<AcqImgHeight ; i+=2) {
//            Input=m_nPhotonFrameBuffer+i*AcqImgWidth;

//            for(int j=0 ; j<AcqImgWidth ; j++) {
//                if(ExitProgram) break;
//                Intensity = (int)( 255 * (float)(*Input-MinVal) / (MaxVal-MinVal+1) + .5);
//                memset(ImgData, Intensity, 3);
//                ImgData+=3;
//                Input+=2;
//            }
//        }


//        //DoFFT(m_nPhotonFrameBuffer);

//        /*        ImgData = (uchar*)args; // Let this "friend function" access ImgData

//        Input = m_nPhotonFrameBuffer;
//        for(int i=0 ; i<IMAGESIZE*3 ; i+=3) {
//            if(ExitProgram) break;
//            Intensity = (int)( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5);
//            memset(ImgData, Intensity, 3);
//            ImgData+=3;
//            Input++;
//        }*/

//        //:)        DoFFT(m_nPhotonFrameBuffer);

//    }
//    pthread_exit(NULL);
//}


//// *****************************************************************
//// Save the image data
//// *****************************************************************
//void* runSavedata(void *)
//{
//    int i;
//    int NumWritten;
//    int Loops;
//    //char GPSFilePath[2048];
//    //char Message[1024];
//    //FILE *fptr;
//    //FILE *fptrEncoder;
//    FILE *FH;


//    /*    // ***** Allow Savedata thread to access Form UI.
//    Ui::MainWindow *ui;
//    ui = (Ui::MainWindow*)args; // Let this "friend function" access ImgData*/


//    while(!ExitProgram) {
//Abort4:

//        //Reset missed buffer variable.
//        //MissedNbBuf = 0;
//        //RunningSaveRoutine = FALSE;

//        /* // If doing manual acquisition, don't add extension here.
//        if( strcmp(FileExtension, ".bsq") == 0 ) {
//            FileExtension[0] = 0;
//        }*/


//        //Wait for collect message.
//        sem_wait (&SaveStart_count);
//        if (ExitProgram) break;	// Exit if program is terminating.

//        //RunningSaveRoutine = TRUE;

//        FrameCount = 0;
//        BufNum = 1;
//        pBuffer = pSaveBuffer;
//        //pBufferGPS = m_pSaveBufferGPS;
//        //pBufferEncoder = m_pSaveBufferEncoder;


//        //GetDlgItemText( IDC_DATADIRECTORY, DataDirectory, 1024);
//        /*        if( DataDirectory[strlen(DataDirectory)-1] != '/' ) {
//            strcat(DataDirectory, "/");
//        }*/

//        //Open File (Always use "path/temp.bsq",  when user presses "Save", then rename it to "path/name.bsq")
//        if ((FH = fopen( FilePath, "wb" )) != NULL ) {

//            /*if (m_bUseCMigit) {
//                // *** Make GPS Filename
//                strcpy(GPSFilePath, FilePath);
//                GPSFilePath[strlen(GPSFilePath)-6] = 0;
//                strcat(GPSFilePath, ".gps");

//                // open file for Scan GPS
//                if ((fptr = fopen (GPSFilePath, "wb")) == NULL) {
//                    AfxMessageBox ("Could Not Open GPS output File");
//                    NumFrames = 0;
//                }
//                m_HandleGPS = fptr;
//            }

//            if (m_bUseEncoder) {
//                // *** Make Encoder Filename
//                strcpy(EncoderFilePath, FilePath);
//                EncoderFilePath[strlen(EncoderFilePath)-6] = 0;
//                strcat(EncoderFilePath, ".encod");

//                // open file for Encoder
//                if ((fptrEncoder = fopen (EncoderFilePath, "wt")) == NULL) {
//                    AfxMessageBox ("Could Not Open Encoder output File");
//                    NumFrames = 0;
//                }
//                m_HandleEncoder = fptrEncoder;
//            }*/


//            // *** Make Log Filename
//            strcpy(LogFilePath, FilePath);
//            LogFilePath[strlen(LogFilePath)-6] = 0;
//            strcat(LogFilePath, ".txt");

//            // open file for Log
//            if ((fptrLog = fopen (LogFilePath, "wb")) == NULL) {
//                AfxMessageBox ("Could Not Open Log output File");
//                NumFrames = 0;
//            }
//            m_HandleLog = fptrLog;


//            //t1S = timeGetTime();

//            SavingData = true;

//            //*** Take selected number of scans
//            if( NumFrames%60 == 0 ) {
//                Loops = NumFrames / 60;
//            }
//            else {
//                Loops = (int)((double)NumFrames/60.0+1);
//            }


//            for( i=0 ; i<Loops ; i++ ) {

//                //Wait for scan to complete.
//                sem_wait (&AcquireDone_count);
//                if(ExitProgram) break;

//                if(Abort) {
//                    SavingData = false;
//                    sem_post (&SaveDone_count);
//                    sem_post (&AcquireFinished_count);
//                    goto Abort4;
//                }


//                //EnterCriticalSection(&obj_FileAccess);

//                //***************************
//                //Save Binary data
//                if( BufNum==1 ) {
//                    NumWritten = fwrite(pSaveBuffer, 2, AcqImgSize*NumFramesToSave, FH);
//                }
//                else {
//                    NumWritten = fwrite(pSaveBuffer+(60L*AcqImgSize), 2, AcqImgSize*NumFramesToSave, FH);
//                }
//                if (NumWritten != AcqImgSize*NumFramesToSave) {
//                    wprintf(L"fwrite failed!\n");
//                }


//                /* // ***************************
//                //Save encoder data
//                if (m_HandleEncoder != NULL) {

//                    if( BufNum==1 ) {
//                        if ((nBytes = fwrite (m_pSaveBufferEncoder, sizeof(float), 1*NumFramesToSave, m_HandleEncoder)) != 1*NumFramesToSave){
//                            wprintf(L"fwrite failed!\n");
//                            //AfxMessageBox ("Error writing GPS data");
//                        }
//                    }
//                    else {
//                        if ((nBytes = fwrite (m_pSaveBufferEncoder+(60L*1), sizeof(float), 1*NumFramesToSave, m_HandleEncoder)) != 1*NumFramesToSave){
//                            wprintf(L"fwrite failed!\n");
//                            //AfxMessageBox ("Error writing GPS data");
//                        }
//                    }
//                }

//                // ***************************
//                //Save GPS data
//                if (m_HandleGPS != NULL) {

//                    if( BufNum==1 ) {
//                        if ((nBytes = fwrite (m_pSaveBufferGPS, sizeof(char), GPSSIZE*NumFramesToSave, m_HandleGPS)) != GPSSIZE*NumFramesToSave){
//                            wprintf(L"fwrite failed!\n");
//                            //AfxMessageBox ("Error writing GPS data");
//                        }
//                    }
//                    else {
//                        if ((nBytes = fwrite (m_pSaveBufferGPS+(60L*GPSSIZE), sizeof(char), GPSSIZE*NumFramesToSave, m_HandleGPS)) != GPSSIZE*NumFramesToSave){
//                            wprintf(L"fwrite failed!\n");
//                            //AfxMessageBox ("Error writing GPS data");
//                        }
//                    }
//                }*/

//                //LeaveCriticalSection(&obj_FileAccess);


//                // Tell Acquire routine that it can do next frame.
//                //::SetEvent(m_hSaveDoneEvent);
//                //pthread_cond_signal(&conditionSaveDone);
//                sem_post (&SaveDone_count);

//                //
//                if( ExitSavingData ) {
//                    break;
//                }

//                /*// *** Calculate the number of frames per seconds every 60 frames
//                t2S = timeGetTime();
//                FramesPerSecS = 1000.0 * (double)NumFramesToSave / (double)(t2S-t1S);
//                t1S = t2S;*/

//                //if(AbortFlag) goto RestartScan3;
//            }

//            //EnterCriticalSection(&obj_FileAccess);
//            // Close the file.
//            if (fclose(FH)) {
//                wprintf(L"fclose failed!\n");
//            }
//            /*		if (m_bUseCMigit) {
//            if (fclose(fptr)) {
//                wprintf(L"fclose failed!\n");
//            }
//        }
//        if (m_bUseEncoder) {
//            if (fclose(fptrEncoder)) {
//                wprintf(L"fclose failed!\n");
//            }
//        }*/
//            //LeaveCriticalSection(&obj_FileAccess);
//        }

//        sem_post (&AcquireFinished_count);
//        SavingData = false;
//    }

//    return(NULL);
//}
//#endif



unsigned long Median25(unsigned short *InputArray)
{
    std::sort(InputArray, &InputArray[24]);
    return(InputArray[12]);
}


unsigned long StdDev(unsigned short *InputArray, unsigned long size, double *aMean)
{
    double DatSummation;
    //double aMean;
    double StdDev, SumStdDev;

    DatSummation = 0;
    for(unsigned i=0 ; i<size ; i++) {
        DatSummation +=	(double)InputArray[i];
    }

    *aMean =	DatSummation / (double)size;

    // Find	the	Std	Deviation
    SumStdDev =	0;
    for(unsigned i=0 ; i<size ; i++) {
        SumStdDev += fabs(*aMean - InputArray[i]);
    }
    StdDev = SumStdDev / (double)size;
    return(StdDev);
}


// *****************************************************************
// Get data from camera
// *****************************************************************
#ifdef __unix__
void* runAcquisition(void *args)
#else
DWORD WINAPI runAcquisition(LPVOID pParam)
#endif
{
    //unsigned short medianimage[13107]; // (640*512)/25
    int nbFrames = 0;
#ifdef __unix__
    timeval endTime;
    timeval startTime;
#else
    time_t endTime;
    time_t startTime;
#endif
    //long seconds, useconds;
    double duration;
    //unsigned char *pImgData8;
    //int short MyShort=15000;
    int MaxVal;
    int MinVal;
    uchar ImgData8;
    //uchar Intensity;
    //ImgData = (uchar*)args; // Let this "friend function" access ImgData

    //unsigned int NumOfFramesPerSaveBlock;
    //unsigned int NumOfBuffers;
    //unsigned int BufferNum=0;
    //BufferNumCamera=0;

    // Initialize ring buffer (pSaveBuffer for Acquisition loop)
    //pBuffer = pSaveBuffer;

    // Initialize Camera

    /*    unsigned width, height;
    if( !Camera.InitCamera(&width, &height) ) {
        exit(-73);
    }
*/
#ifdef __unix__
    // Start the camera
    switch(CameraType) {
    case MC320:
        CameraMc320.StartCamera();
        break;
    case TAU640:
        CameraTau.StartCamera();
        break;
    case PV640:
        CameraPv640.StartCamera();
        break;
    }
#endif

    pBuffer = pSaveBuffer;
    pBufferGPS = m_pSaveBufferGPS;
    *pBufferGPS = 0;
    //bool InitAcq = true;


    // For calculating FPS the first go-round
#ifdef __unix__
    gettimeofday(&startTime, NULL);
#else
    startTime = timeGetTime();
#endif


    while(!ExitProgram) {
Abort3:
        // ***************************************************************************************************
        // Wait for START
        // ***************************************************************************************************
        /*
//#define TESTING
#ifdef TESTING
            RunningAcquire=true;
            BufferNumCamera = 0;
            pBufferCamera = pSaveBuffer;
            BufferNumUDP = 0;
            pBufferUDP = pSaveBuffer;
#else
        if(!RunningAcquire) {
            //RunningAcquire=true;
            sem_wait (&StartCamera_count);
            BufferNumCamera = 0;
            pBufferCamera = pSaveBuffer;
            BufferNumUDP = 0;
            pBufferUDP = pSaveBuffer;
            //usleep(1000000);
        }
#endif
*/
        if (ExitProgram) {
#ifdef __unix__
            sem_post (&AcquireDone_count);  // If exiting, let Save routine stop waiting and exit.
#else
            SetEvent (AcquireDone_count);  // If exiting, let Save routine stop waiting and exit.
/*            mutex2.lock();
            keyPressed2.wakeAll();
            mutex2.unlock();*/

#endif
            //sem_post (&AcquireDone2_count);  // If exiting, let Save routine stop waiting and exit.
            break;
        }


        //
        switch(CameraType) {
#ifdef __unix__
        case MC320:
            CameraMc320.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
        case TAU640:
            CameraTau.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
        case PV640:
            CameraPv640.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
#endif
        case TESTPATTERN320X240:
            CameraDummy320.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
        case TESTPATTERN640X512:
            CameraDummy640.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
        case PHOTON:
            CameraPhoton.GetCameraFrame(m_nPhotonFrameBuffer);
            break;
        case INO:
//            long AdjustedValue;

//            WaitForSingleObject(ghMutex, INFINITE);

            CameraINO.GetCameraFrame(m_nPhotonFrameBuffer);

//            if( (m_nPhotonBackgroundBuffer[0]!=0) && (m_nPhotonBackgroundBuffer[100]!=0) ) {
//                for(int i=0 ; i<(IMAGESIZE) ; i++) {
//                    AdjustedValue = (long)m_nPhotonFrameBuffer[i] - (long)m_nPhotonBackgroundBuffer[i] + BASEVALUE;
//                    m_nPhotonFrameBuffer[i] = (unsigned short)AdjustedValue;
//                }
//            }
            break;
        }
        //Camera.GetCameraFrame(pBufferCamera);

//#ifdef BADCODE
//        // If Running, wait for save data routine to finish
//        if( SavingData ) {
//            MyShort++;

//            for(int i=0 ; i<6400 ; i++) {
//                m_nPhotonFrameBuffer[i] = MyShort;
//            }
//            if(Abort) {
//                sem_post (&AcquireDone_count);
//                goto Abort3;
//            }


//            memcpy( pBuffer, m_nPhotonFrameBuffer, IMAGESIZE*2 );
//            //memcpy( pBuffer, pBufferCamera, IMAGESIZE*2 );
//            pBuffer+=IMAGESIZE; // Move pointer to the next image space in the buffer.

//            //memcpy( pBufferGPS, m_pGPSBuffer+m_nGPSIndex, GPSSIZE );
//            //pBufferGPS+=GPSSIZE;

//            //memcpy( pBufferEncoder, &DesiredPosition, sizeof(float) );
//            //*pBufferEncoder = DesiredPosition;
//            //pBufferEncoder++;

//            if( FrameCount%60 == 0 ) {

//                if( BufNum==0 ) {
//                    BufNum=1;
//                    pBuffer = pSaveBuffer + (60L*IMAGESIZE);	// There are two 60 image buffers
//                    //pBufferGPS = m_pSaveBufferGPS + (60L*GPSSIZE); // we toggle between them.
//                    //pBufferEncoder = m_pSaveBufferEncoder + (60L); // we toggle between them.
//                } else {
//                    BufNum=0;
//                    pBuffer = pSaveBuffer;
//                    //pBufferGPS = m_pSaveBufferGPS;
//                    //pBufferEncoder = m_pSaveBufferEncoder;
//                }
//                if( FrameCount != 0 ) {
//                    if(!InitAcq) {
//                        //Wait for Message from Syncronizer thread.
//                        sem_wait (&SaveDone_count);
//                        if(ExitProgram) break;
//                    } else {
//                        InitAcq=false;
//                    }

//                    NumFramesToSave = 60;
//                    sem_post (&AcquireDone_count);
//                }
//            }
//            if( FrameCount==NumFrames ) {
//                SavingData = false;
//                if( NumFrames%60 != 0 ) {
//                    BufNum = !BufNum;
//                    NumFramesToSave = NumFrames%60;
//                    sem_post (&AcquireDone_count);
//                }
//            }
//            FrameCount++;
//        }

//        else {
//            MyShort=15000;      // For testing
//        }
//#endif



        // If Running, wait for save data routine to finish
        if( SavingData ) {
            if(Abort) {
#ifdef __unix__
                sem_post (&AcquireDone_count);
#else
                SetEvent (AcquireDone_count);
                /*mutex2.lock();
                keyPressed2.wakeAll();
                mutex2.unlock();*/
#endif
                goto Abort3;
            }

            memcpy( pBuffer, m_nPhotonFrameBuffer, AcqImgSize*2 );
            pBuffer+=AcqImgSize; // Move pointer to the next image space in the buffer.
            FrameCount++;

            // Put GPS into asc line
            sprintf(MyGpsLine, "%f, %f, %f, %f, %f, %f, %f, %d\n" , Gps.parm.Time, Gps.parm.Pitch, Gps.parm.Roll, Gps.parm.Heading, Gps.parm.Latitude, Gps.parm.Longitude, Gps.parm.Altitude, Gps.parm.GpsValid);
            //MyInt++;
            //sprintf(MyGpsLine, "%d\n" , MyInt);
            strcat( (char*)pBufferGPS, MyGpsLine );
            //qDebug() << ">>>>>>>>GPS Write" << Gps.parm.Time*10 - 2e+6;


            if(FrameCount%60 == 0) {
                if( FrameCount != 60 ) {
                    // This "if" will never execute!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    //Wait for Message saying save is done.
#ifdef __unix__
                    sem_wait (&SaveDone_count);
#else
                    WaitForSingleObject (SaveDone_count, INFINITE);
                    //qDebug() << "FC=" << FrameCount;
#endif

                    if(ExitProgram) break;
                }

                if( BufNum==0 ) {
                    BufNum=1;
                    pBuffer = pSaveBuffer + (60L*AcqImgSize);	// There are two 60 image buffers
                    pBufferGPS = m_pSaveBufferGPS + (60L*GPSSIZE); // we toggle between them.
                    *pBufferGPS = 0;
                    //pBufferEncoder = m_pSaveBufferEncoder + (60L); // we toggle between them.
                } else {
                    BufNum=0;
                    pBuffer = pSaveBuffer;
                    pBufferGPS = m_pSaveBufferGPS;
                    *pBufferGPS = 0;
                    //pBufferEncoder = m_pSaveBufferEncoder;
                }

                NumFramesToSave = 60;

#ifdef __unix__
                sem_post (&AcquireDone_count);
#else
                SetEvent (AcquireDone_count);
                /*mutex2.lock();
                keyPressed2.wakeAll();
                mutex2.unlock();*/

#endif
            }


            if( FrameCount==NumFrames ) {
                SavingData = false;
                if( NumFrames%60 != 0 ) {
                    BufNum = !BufNum;
                    NumFramesToSave = NumFrames%60;
#ifdef __unix__
                    sem_post (&AcquireDone_count);
#else
                    SetEvent (AcquireDone_count);
                    /*mutex2.lock();
                    keyPressed2.wakeAll();
                    mutex2.unlock();*/
#endif
                }
            }
        }

        /*
m_nPhotonFrameBuffer[100] = 0;
m_nPhotonFrameBuffer[640+100] = 0;
m_nPhotonFrameBuffer[640+150] = 0;
m_nPhotonFrameBuffer[640+200] = 0;
m_nPhotonFrameBuffer[640+250] = 0x3FFF;
m_nPhotonFrameBuffer[640+300] = 0x3FFF;
m_nPhotonFrameBuffer[1000] = 0x3FFF;
*/
        //#ifdef UDP320X240
        if (!ExitProgram) {
            //      PUT THIS IN A SEPARATE THREAD (this is the video display data)
            unsigned short *Input;
            unsigned short *Background;
            uchar *ImgDataRGB;
            //uchar *pImgDataRGB;
            //uchar Intensity;
#ifdef __unix__
            ImgDataRGB = (uchar*)args; // Let this "friend function" access ImgDataRGB
#else
            ImgDataRGB = ImgCamera;
#endif

            // Autoscale data
            MaxVal = -1;
            MinVal = 0xFFFF;
#define BASEVALUE 10000
            for(unsigned i=1 ; i<AcqImgSize ; i++) {
                //if(m_nPhotonFrameBuffer[i]>23000)
                //int Dummy=i;
                //if((i!=282)&&(i!=8684)&&(i!=55878)&&(i!=55879)&&(i!=63804)&& (MaxVal<m_nPhotonFrameBuffer[i]) && (m_nPhotonFrameBuffer[i]<35000))
                if(MaxVal<m_nPhotonFrameBuffer[i]-m_nPhotonBackgroundBuffer[i]+BASEVALUE)
                    MaxVal=m_nPhotonFrameBuffer[i]-m_nPhotonBackgroundBuffer[i]+BASEVALUE;
                //if((i!=6385)&&(i!=10266)&&(i!=10587)&&(i!=28967)&&(i!=31323)&&(i!=50388)&& (MinVal>m_nPhotonFrameBuffer[i]) && (m_nPhotonFrameBuffer[i]>300))
                if( (MinVal>m_nPhotonFrameBuffer[i]-m_nPhotonBackgroundBuffer[i]+BASEVALUE) /*&& (m_nPhotonFrameBuffer[i]>100)*/ )
                    MinVal=m_nPhotonFrameBuffer[i]-m_nPhotonBackgroundBuffer[i]+BASEVALUE;
            }
            if(MaxVal == MinVal) {
                MaxVal = MinVal+1;
            }

//MinVal = 13000;
//MaxVal = 39000;
            //(i!=6385)&&(i!=10266)&&(i!=10587)&&(i!=28967)&&(i!=31323)&&(i!=50388)&&
            // 6385, 10266, 10587, 28967, 31323, 50388
            // 282, 642, 55879, 8684, 63804
            /* 55878, 55879,
        // Do median of 5x5, (well, how about 25x1)
        // 640*512/25=13107
        unsigned short SegmentArray[25];
        Input = m_nPhotonFrameBuffer;
        for(int i=0 ; i<13107 ; i++) {
            memcpy(SegmentArray, Input, 25*sizeof(unsigned short));
            medianimage[i] = Median25(SegmentArray);
            Input+=25;
        }

        double aMean;
        float MyStdDev = StdDev(medianimage, 13107, &aMean);
        int StdMin = aMean-2.5*MyStdDev;
        int StdMax = aMean+2.5*MyStdDev;

        if(StdMin>0) MinVal=StdMin; else MinVal = 0;
        if(StdMax<0x3FFF) MaxVal=StdMax; else MaxVal = 0x3FFF;
*/



            /*
        // Do median of 5x5, (well, how about 25x1)
        // 640*512/25=13107
        unsigned short SegmentArray[25];
        Input = m_nPhotonFrameBuffer;
        for(int i=0 ; i<13107 ; i++) {
            memcpy(SegmentArray, Input, 25*sizeof(unsigned short));
            medianimage[i] = Median25(SegmentArray);
            Input+=25;
        }


        double aMean;
        float MyStdDev = StdDev(medianimage, 13107, &aMean);
        int StdMin = aMean-10.0*MyStdDev;
        int StdMax = aMean+10.0*MyStdDev;



/ *qDebug() << "Mean:" << aMean;
qDebug() << "StdDev:" << MyStdDev;
qDebug() << "MinVal:" << MinVal;
qDebug() << "MaxVal:" << MaxVal;
qDebug() << "StdMin:" << StdMin;
qDebug() << "StdMax:" << StdMax;* /

        if(StdMin>0) MinVal=StdMin; else MinVal = 0;
        if(StdMax<0x3FFF) MaxVal=StdMax; else MaxVal = 0x3FFF;
*/
            //MinVal = 2200;
            //MaxVal = 4500;

            //if(!ImageDisplaySleeping) {
            //Input = m_nPhotonFrameBuffer;
            //pImgData8 = ImgData;
            //pImgDataRGB = ImgDataRGB;


#ifdef lolo
            //No Rotation
            //for(int i=0 ; i<AcqImgSize*2-(256*640*2) ; i+=3) {
            for(int i=0 ; i<DISPLAYHEIGHT ; i++) {
                Input = m_nPhotonFrameBuffer + i*1*1*DISPLAYWIDTH;
                Background = m_nPhotonBackgroundBuffer + i*DISPLAYWIDTH;
                for(int j=0 ; j<DISPLAYWIDTH ; j++) {
                    if(ExitProgram) break;
                    ImgData8 = (uchar)( 255 * (*Input-*Background+BASEVALUE-MinVal) / (MaxVal-MinVal+1) + .5);
                    memset(ImgDataRGB, ImgData8, 3);
                    //memset(ImgDataRGB, 128, 3);
                    //pImgData8++;
                    ImgDataRGB+=3;
                    //Input+=2;    // Throw out every other pixel to make image half size
                    Input++;
                }
            }
#endif
#define lala
#ifdef lala
/*            //No Rotation
            //for(int i=0 ; i<AcqImgSize*2-(256*640*2) ; i+=3) {
            //for(int i=0 ; i<DisplayHeight * .9 ; i++) {
            for(int i=0 ; i<DisplayHeight ; i++) {
                //Input = m_nPhotonFrameBuffer + i*AcqImgWidth/DISPLAYWIDTH*AcqImgWidth;
                Input = m_nPhotonFrameBuffer + i*DISPLAYWIDTH;
                Background = m_nPhotonBackgroundBuffer + i*DISPLAYWIDTH;
                for(int j=0 ; j<DISPLAYWIDTH ; j++) {
                    if(ExitProgram) break;


                    double AA = (double)((long)BASEVALUE+(long)(*Input)-(long)(*Background)-MinVal);
                    double BB = (double)(MaxVal-MinVal+1);

                    double CC = (uchar)(255.0 * AA/BB + .5);

                    ImgData8 = (uchar)( 255.0 * (double)((long)BASEVALUE+(long)(*Input)-(long)(*Background)-MinVal) / (double)(MaxVal-MinVal+1) + .5);
                    //                        if(( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5) > 255) {
                    //                            ImgData8 = 0;
                    //                        }
                    //                        if(( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5) < 0) {
                    //                            ImgData8 = 0;
                    //                        }
                    //ImgData8 =
                    //:O
                    memset(ImgDataRGB, ImgData8, 3);
                    //memset(ImgDataRGB, 128, 3);
                    //pImgData8++;
                    //:O

                    if( (i<DisplayHeight-1) || (j<DISPLAYWIDTH-1) ) {
                        ImgDataRGB+=3;
                        Input++;    //=AcqImgWidth/DISPLAYWIDTH;    // Throw out every other pixel to make image half size
                    }
                }
            }

*/

            for(unsigned i=1 ; i<AcqImgSize ; i++) {
                ImgData8 = (uchar)( 255.0 * (double)((long)BASEVALUE+(long)(m_nPhotonFrameBuffer[i])-(long)(m_nPhotonBackgroundBuffer[i])-MinVal) / (double)(MaxVal-MinVal+1) + .5);
                memset(ImgDataRGB, ImgData8, 3);
                if( i<AcqImgSize-AcqImgWidth ) {
                    ImgDataRGB+=3;
                }
            }


#endif
#ifdef REMOVE
            /*//Rotate ClockWise
                for(int i=IMAGEHEIGHT-1 ; i>=0 ; i--) {
                    pImgData8 = ImgData + i;
                    pImgDataRGB= ImgDataRGB + 3*i;
                    for(int j=0 ; j<IMAGEWIDTH ; j++) {
                        if(ExitProgram) break;
                        *pImgData8 = (uchar)( 255 * (*Input-MinVal) / (MaxVal-MinVal+1) + .5);
                        memset(pImgDataRGB, *pImgData8, 3);
                        pImgData8 += IMAGEHEIGHT;
                        pImgDataRGB += IMAGEHEIGHT*3;
                        Input++;
                    }
                }*/

            //Rotate CounterClockWise
            for(int i=0 ; i<ImageHeight ; i++) {

                pImgData8 = ImgData + ImageHeight*(ImageWidth-1) + i;
                pImgDataRGB= ImgDataRGB + 3*(ImageHeight*(ImageWidth-1) + i);

                for(int j=0 ; j<ImageWidth ; j++) {
                    if(ExitProgram) break;

                    if(*Input>MinVal) {
                        int iVal = (int)( 255 * (double)(*Input-*Background+BASEVALUE-MinVal) / (double)(MaxVal-MinVal+1.0) + .5);
                        if(iVal>255) {
                            *pImgData8 = 255;
                        } else {
                            *pImgData8 = (uchar)iVal;
                        }
                    } else {
                        *pImgData8 = 0;
                    }

                    memset(pImgDataRGB, *pImgData8, 3);

                    pImgData8 -= ImageHeight;
                    pImgDataRGB -= (ImageHeight*3);
                    Input++;
                }
            }
        }
#endif
    }
    //#endif

    /*        BufferNumCamera++;
        if(BufferNumCamera>=NUMOFBUFFERS) {
            BufferNumCamera = 0;
            pBufferCamera = pSaveBuffer;
        } else {
            pBufferCamera+=IMAGESIZE;
        }*/

    /*        if((BufferNumCamera % NUMOFFRAMESPERSAVEBLOCK) == 0) {
            sem_post (&AcquireDone2_count);
        }
*/
    //sem_post (&AcquireDone2_count); // Do this every frame now, for TCP.


    //!!!!!!!!!! DoFFT(m_nPhotonFrameBuffer);


    // *** Calculate the number of frames per seconds every second
    nbFrames++;
    if (nbFrames>=FrameRate) {
#ifdef __unix__
        gettimeofday(&endTime, NULL);
        seconds  = endTime.tv_sec  - startTime.tv_sec;
        useconds = endTime.tv_usec - startTime.tv_usec;
        duration = seconds + useconds/1000000.0;
#else
        endTime = timeGetTime();
        duration = (endTime - startTime)/1000.0;
#endif

        PresentFPS = (double)nbFrames / (double)(duration);

#ifdef __unix__
        gettimeofday(&startTime, NULL);
#else
        startTime = timeGetTime();
#endif
        nbFrames=0;
    }


//#ifdef __unix__
//            sem_post (&AcquireDone_count);  // If exiting, let Save routine stop waiting and exit.
//#else
//            SetEvent (AcquireDone_count);  // If exiting, let Save routine stop waiting and exit.
///*            mutex2.lock();
//            keyPressed2.wakeAll();
//            mutex2.unlock();*/

//#endif

//    if (ExitProgram) {
//        break;
//    }
}
#ifdef __unix__
pthread_exit(NULL);
#endif

return(NULL);
}



// *****************************************************************
// Save the image data
// *****************************************************************
#ifdef __unix__
void* runSavedata(void*)
#else
DWORD WINAPI runSavedata(LPVOID pParam)
#endif
{
    int i;
    int NumWritten;
    int Loops;
    int nBytes;
    char GPSFilePath[2048];
    //char Message[1024];
    FILE *fptr, *m_HandleGPS;
    //FILE *fptrEncoder;
    FILE *fptrLog;
    FILE *FH;

    char Message[3048];
    char DataString[2048];
    char buffer[2048];
    time_t t;
    tm *gmtm;


    /*    // ***** Allow Savedata thread to access Form UI.
    Ui::MainWindow *ui;
    ui = (Ui::MainWindow*)args; // Let this "friend function" access ImgDataRGB*/


    while(!ExitProgram) {
Abort4:

        //Reset missed buffer variable.
        //MissedNbBuf = 0;
        //RunningSaveRoutine = FALSE;

        /* // If doing manual acquisition, don't add extension here.
        if( strcmp(FileExtension, ".bsq") == 0 ) {
            FileExtension[0] = 0;
        }*/


        //Wait for collect message.
#ifdef __unix__
        sem_wait (&SaveStart_count);
#else
        WaitForSingleObject (SaveStart_count, INFINITE);

        /*mutex.lock();
        keyPressed.wait(&mutex);
        mutex.unlock();*/

#endif

        if (ExitProgram) break;	// Exit if program is terminating.

        //RunningSaveRoutine = TRUE;

        FrameCount = 0;
        BufNum = 0;
        pBuffer = pSaveBuffer;
        pBufferGPS = m_pSaveBufferGPS;
        *pBufferGPS = 0;

        //pBufferEncoder = m_pSaveBufferEncoder;


        //GetDlgItemText( IDC_DATADIRECTORY, DataDirectory, 1024);
        /*        if( DataDirectory[strlen(DataDirectory)-1] != '/' ) {
            strcat(DataDirectory, "\\");
        }*/

        //Open File (Always use "path/temp.bsq",  when user presses "Save", then rename it to "path/name.bsq")
        if ((FH = fopen( FilePath, "wb" )) != NULL ) {
BOOL m_bUseCMigit = TRUE;
            if (m_bUseCMigit) {
                // *** Make GPS Filename
                strcpy(GPSFilePath, FilePath);
                GPSFilePath[strlen(GPSFilePath)-4] = 0;
                strcat(GPSFilePath, ".gps");

                // open file for Scan GPS
                if ((fptr = fopen (GPSFilePath, "wt")) == NULL) {
                    //AfxMessageBox ("Could Not Open GPS output File");
                    NumFrames = 0;
                }
                m_HandleGPS = fptr;
                strcpy(buffer , "Time           Pitch      Roll       Heading   Latitude    Longitude   Altitude   GPSValid\n");
                nBytes = fwrite (buffer, sizeof(char), strlen(buffer), m_HandleGPS);
            }

            /*if (m_bUseEncoder) {
                // *** Make Encoder Filename
                strcpy(EncoderFilePath, FilePath);
                EncoderFilePath[strlen(EncoderFilePath)-6] = 0;
                strcat(EncoderFilePath, ".encod");

                // open file for Encoder
                if ((fptrEncoder = fopen (EncoderFilePath, "wt")) == NULL) {
                    AfxMessageBox ("Could Not Open Encoder output File");
                    NumFrames = 0;
                }
                m_HandleEncoder = fptrEncoder;
            }*/


            // *** Make Log Filename
            strcpy(LogFilePath, FilePath);
            LogFilePath[strlen(LogFilePath)-4] = 0;
            strcat(LogFilePath, ".txt");

            // open file for Log
            if ((fptrLog = fopen (LogFilePath, "wt")) == NULL) {
                //NumFrames = 0;
            } else {
                strcpy(Message, "   TimeStamp,   ColdPaddle, HotPaddle, ColdRear, HotRear,   T1,   T2,   T3,   T4,   T5,   T6, Cam T, ColdPaddle, HotPaddle, ColdRear, HotRear, Rot, Goniometer\r\n");
                fwrite( Message, strlen(Message), 1, fptrLog );
            }

            //t1S = timeGetTime();

            SavingData = true;

            //*** Take selected number of scans
            if( NumFrames%60 == 0 ) {
                Loops = NumFrames / 60;
            }
            else {
                Loops = (int)((double)NumFrames/60.0+1);
            }


            for( i=0 ; i<Loops ; i++ ) {

                //Wait for scan to complete.
#ifdef __unix__
                sem_wait (&AcquireDone_count);
#else
                WaitForSingleObject (AcquireDone_count, INFINITE);
                //mutex2.lock();
                //keyPressed2.wait(&mutex2);
                //mutex2.unlock();
#endif

                if(ExitProgram) break;


                //EnterCriticalSection(&obj_FileAccess);

                //***************************
                //Save Binary data
                if( BufNum==1 ) {
                    NumWritten = fwrite(pSaveBuffer, 2, AcqImgSize*NumFramesToSave, FH);
                    //for(int n=0 ; n<60 ; n++) {
                    //    printf("%d" , pSaveBuffer[n*327680]);
                    //}
                }
                else {
                    NumWritten = fwrite(pSaveBuffer+(60L*AcqImgSize), 2, AcqImgSize*NumFramesToSave, FH);
                }

                //                if (NumWritten != IMAGESIZE*NumFramesToSave) {
                //printf("fwrite failed!\n");
                //                }


                /* // ***************************
                //Save encoder data
                if (m_HandleEncoder != NULL) {

                    if( BufNum==1 ) {
                        if ((nBytes = fwrite (m_pSaveBufferEncoder, sizeof(float), 1*NumFramesToSave, m_HandleEncoder)) != 1*NumFramesToSave){
                            wprintf(L"fwrite failed!\n");
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }
                    else {
                        if ((nBytes = fwrite (m_pSaveBufferEncoder+(60L*1), sizeof(float), 1*NumFramesToSave, m_HandleEncoder)) != 1*NumFramesToSave){
                            wprintf(L"fwrite failed!\n");
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }
                }
*/
                // ***************************
                //Save GPS data
                if (m_HandleGPS != NULL) {

                    /*if( BufNum==1 ) {
                        if ((nBytes = fwrite (m_pSaveBufferGPS, sizeof(char), GPSSIZE*NumFramesToSave, m_HandleGPS)) != GPSSIZE*NumFramesToSave){
                            wprintf(L"fwrite failed!\n");
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }
                    else {
                        if ((nBytes = fwrite (m_pSaveBufferGPS+(60L*GPSSIZE), sizeof(char), GPSSIZE*NumFramesToSave, m_HandleGPS)) != GPSSIZE*NumFramesToSave){
                            wprintf(L"fwrite failed!\n");
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }*/


                    //pthread_spin_lock(&spinlock);
                    if( BufNum==1 ) {
                        if ((nBytes = fwrite (m_pSaveBufferGPS, sizeof(char), strlen((char*)m_pSaveBufferGPS), m_HandleGPS)) != strlen((char*)m_pSaveBufferGPS)){
                            //wprintf(L"fwrite failed!\n")
                            ;
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }
                    else {
                        //if ((nBytes = fwrite (m_pSaveBufferGPS+(60L*GPSSIZE), sizeof(char), GPSSIZE*NumFramesToSave, m_HandleGPS)) != GPSSIZE*NumFramesToSave){
                        if ((nBytes = fwrite (m_pSaveBufferGPS+((long)(NUMOFBUFFERS>>1)*GPSSIZE), sizeof(char), strlen((char*)m_pSaveBufferGPS+((long)(NUMOFBUFFERS>>1)*GPSSIZE)), m_HandleGPS)) != strlen((char*)m_pSaveBufferGPS+((long)(NUMOFBUFFERS>>1)*GPSSIZE))){
                            //wprintf(L"fwrite failed!\n")
                            ;
                            //AfxMessageBox ("Error writing GPS data");
                        }
                    }
                    //pthread_spin_unlock(&spinlock);

                }

                //LeaveCriticalSection(&obj_FileAccess);


                // Tell Acquire routine that it can do next frame.
                //::SetEvent(m_hSaveDoneEvent);
                //pthread_cond_signal(&conditionSaveDone);


                sprintf(DataString, "  %4.1f,       %4.1f,      %4.1f,     %4.1f,    %4.1f, %4.1f, %4.1f, %4.1f, %4.1f, %4.1f, %4.1f,   %3d,       %3d,       %3d,     %s,  %5.3f, %5.3f" ,
                        PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot,
                        TemperatureSensor[0], TemperatureSensor[1], TemperatureSensor[2], TemperatureSensor[3], TemperatureSensor[4], TemperatureSensor[5],
                        CameraTemperature,
                        PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient, PresentPositionRotor,
                        PresentPositionRotation-RotationHome, PresentPositionGoniometer-GoniometerHome);

                // Get the time
                t = time(NULL);
                gmtm = gmtime(&t);
                sprintf(Message, "%4d%02d%02d%02d%02d%02d,%s\r\n", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, DataString);
                fwrite( Message, strlen(Message), 1, fptrLog );



                if(Abort) {
                    NumFramesSaved = i*60 + NumFramesToSave;
                    fclose(fptrLog);

                    // Close the file.
                    if (fclose(FH)) {
                        ;//printf("fclose failed!\n");
                    }
                    if (m_bUseCMigit) {
                        if (fclose(fptr)) {
                            ;//printf("fclose failed!\n");
                        }
                    }

                    SavingData = false;
#ifdef __unix__
                    sem_post (&SaveDone_count);
                    sem_post (&AcquireFinished_count);
#else
                    SetEvent (SaveDone_count);
                    SetEvent (AcquireFinished_count);
#endif
                    goto Abort4;
                } else {
                    NumFramesSaved = NumFrames;
                }




#ifdef __unix__
                sem_post (&SaveDone_count);
#else
                SetEvent (SaveDone_count);
#endif
                //
                if( ExitSavingData ) {
                    break;
                }

                /*// *** Calculate the number of frames per seconds every 60 frames
                t2S = timeGetTime();
                FramesPerSecS = 1000.0 * (double)NumFramesToSave / (double)(t2S-t1S);
                t1S = t2S;*/

                //if(AbortFlag) goto RestartScan3;
            }

            fclose(fptrLog);

            //EnterCriticalSection(&obj_FileAccess);
            // Close the file.
            if (fclose(FH)) {
                ;//printf("fclose failed!\n");
            }
            if (m_bUseCMigit) {
                if (fclose(fptr)) {
                    ;//printf("fclose failed!\n");
                }
            }
            /*if (m_bUseEncoder) {
                if (fclose(fptrEncoder)) {
                    wprintf(L"fclose failed!\n");
                }
            }
        }
*/
            //LeaveCriticalSection(&obj_FileAccess);
        }

#ifdef __unix__
        sem_post (&AcquireFinished_count);
#else
        SetEvent (AcquireFinished_count);
#endif
        SavingData = false;
    }

    return(NULL);
}




// *****************************************************************
// Set the enabled state of buttons
// *****************************************************************
void MainWindow::enableButtons(bool ButtonState)
{
    if(ButtonState==false) ButtonsDisabled=true;
    else ButtonsDisabled=false;

    // ***** Disable all acquisition buttons *****
    ui->pushButtonAcquireManualBench->setEnabled(ButtonState);
    ui->pushButtonWaveCalBench->setEnabled(ButtonState);
    ui->pushButtonSavePaddleColdBench->setEnabled(ButtonState);
    ui->pushButtonSavePaddleHotBench->setEnabled(ButtonState);
    ui->pushButtonSaveRotorColdBench->setEnabled(ButtonState);
    ui->pushButtonSaveRotorHotBench->setEnabled(ButtonState);

    ui->pushButtonWaveCalTripod->setEnabled(ButtonState);
    ui->pushButtonSavePaddleColdTripod->setEnabled(ButtonState);
    ui->pushButtonSavePaddleHotTripod->setEnabled(ButtonState);
    ui->pushButtonSaveRotorColdTripod->setEnabled(ButtonState);
    ui->pushButtonSaveRotorHotTripod->setEnabled(ButtonState);

    ui->pushButtonWaveCalFlight->setEnabled(ButtonState);
    ui->pushButtonSavePaddleColdFlight->setEnabled(ButtonState);
    ui->pushButtonSavePaddleHotFlight->setEnabled(ButtonState);
    ui->pushButtonSaveRotorColdFlight->setEnabled(ButtonState);
    ui->pushButtonSaveRotorHotFlight->setEnabled(ButtonState);

    //ui->pushButtonCollectScan->setEnabled(ButtonState);
    //ui->pushButtonMoveBB->setEnabled(ButtonState);
    //ui->pushButtonMoveHome->setEnabled(ButtonState);
    //ui->pushButtonSetRotationAPosition->setEnabled(ButtonState);
    //ui->pushButtonReopenSerialPorts->setEnabled(ButtonState);
}


// *****************************************************************
// *********************** USER BUTTONS ****************************
// *****************************************************************


/*// *****************************************************************
// User pushed the Scan button.
// *****************************************************************
void MainWindow::on_pushButtonCollectScan_clicked()     // RotationA mode
{
    // Reset Abort Flag
    Abort = false;
    if(SlowSpeed<=0.0) return;

    strcpy( DataDirectory, ui->lineEditDataDirectory->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         // Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    //StartPosition = ui->lineEditRotationAStartPosition->text().toFloat();       // Get start position
    //StopPosition = ui->lineEditRotationAStopPosition->text().toFloat();         // Get stop position
    DesiredTemperatureHot = ui->lineEditDesiredTemperatureHot->text().toFloat();
    DesiredTemperatureCold = ui->lineEditDesiredTemperatureCold->text().toFloat();
    //SlowSpeed = ui->lineEditSlowSpeed->text().toFloat();
    NumCalFrames = ui->lineEditNumCalFrames->text().toInt();
    NumTargetFrames = fabs(StopPosition-StartPosition+SCANALIGNMENTBUFFERWIDTH)*FrameRate/SlowSpeed;

    // Start scaning
    AcqBusy = true;
    enableButtons(false);
    sem_post (&RunScan_count);
}
*/

/*// *****************************************************************
// User pushed the collect Cold button.
// *****************************************************************
void MainWindow::on_pushButtonSaveCold_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectory->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFrames->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireCold();
}

// *****************************************************************
// User pushed the Collect Hot button.
// *****************************************************************
void MainWindow::on_pushButtonSaveHot_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectory->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelength->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortComment->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFrames->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireHot();
}*/

/*// *****************************************************************
// User pushed the ViewRange button.
// *****************************************************************
void MainWindow::on_pushButtonViewRange_clicked()
{
    // Reset Abort Flag
    Abort = false;

    StartPosition = ui->lineEditRotationAStartPosition->text().toFloat();
    StopPosition = ui->lineEditRotationAStopPosition->text().toFloat();

    sem_post (&ViewRange_count);
}*/

/*// *****************************************************************
// User pushed the Set Temperature Cold button.
// *****************************************************************
void MainWindow::on_pushButtonSetCold_clicked()
{
    // Get the desired temperature from the line edit.
    DesiredTemperature = ui->lineEditDesiredTemperatureCold->text().toFloat();
    sem_post (&ChangeTemperature_count);
}

// *****************************************************************
// User pushed the Set Temperature Hot button.
// *****************************************************************
void MainWindow::on_pushButtonSetHot_clicked()
{
    // Get the desired temperature from the line edit.
    DesiredTemperature = ui->lineEditDesiredTemperatureHot->text().toFloat();
    sem_post (&ChangeTemperature_count);
}*/

/*// *****************************************************************
// User pushed the Set Position button.
// *****************************************************************
void MainWindow::on_pushButtonSetRotationAPosition_clicked()
{
    // Get the desired position from the line edit.
    DesiredPosition = HomePosition + ui->lineEditDesiredRotationAPosition->text().toFloat();
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}*/

/*// *****************************************************************
// User pushed the Home button.
// *****************************************************************
void MainWindow::on_pushButtonMoveHome_clicked()
{
    // Move to Home.
    DesiredPosition = HomePosition;
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}
*/
/*// *****************************************************************
// User pushed the Move to BB button.
// *****************************************************************
void MainWindow::on_pushButtonMoveBB_clicked()
{
    // Move to BB.
    DesiredPosition = BBPosition;
    DesiredSpeed = m_nFastSpeed;
    sem_post (&MovePosition_count);
}
*/
/*// *****************************************************************
// User pushed Reopen serial ports button.
// *****************************************************************
void MainWindow::on_pushButtonReopenSerialPorts_clicked()
{
    int RotationAPortNum, BbPortNum, GumstixPortNum, FocusPortNum;
    //int BbRetVal, RotationARetVal;

    // Don't need this since we are using UDEV
    return;

    // Figure out wich serial port each device is on.
    RotationAPortNum = 98;   // Set default value
    BbPortNum = 98;      // Set default value
    GumstixPortNum = 98; // Set default value
    FocusPortNum = 98; // Set default value


    if(SerialRotationA) {
        cssl_drain(SerialRotationA);
        cssl_close(SerialRotationA);
    }
    if(SerialBb) {
        cssl_drain(SerialBb);
        cssl_close(SerialBb);
    }
    if(SerialGumstix) {
        cssl_drain(SerialGumstix);
        cssl_close(SerialGumstix);
    }
    if(SerialFocus) {
        cssl_drain(SerialFocus);
        cssl_close(SerialFocus);
    }



    FindDevices(&RotationAPortNum, &BbPortNum);

    if(RotationAPortNum<98) RotationAPortOpen = OpenRotationA(&SerialRotationA, RotationAPortNum, &RotationABusy);
    if(BbPortNum<98) BbPortOpen = OpenBb(&SerialBb, BbPortNum, &BbBusy);
}
*/

void MainWindow::on_tabWidget_currentChanged(int index)
{
    CurrentTab = index;
}

/* // *****************************************************************
// ABORT
// *****************************************************************
void MainWindow::on_pushButtonAbort_clicked()
{
    char comString[256];
    Abort = true;

    // Wait if position monitor is getting the position (put a timeout here)
    / *while(RotationABusy) {
        usleep(100000);
    }* /
    //RotationABusy = true;

    //Stop RotationA Stage.
    //sprintf (comString, "1ST\r\n" );
    //cssl_putstring(SerialRotationA, comString);
    Rotation.Halt();
    Goniometer.Halt();

    / *    //Home RotationA Stage.
    sprintf (comString, "1OR?\r\n" );
    cssl_putstring(SerialRotationA, comString);* /
    //RotationABusy = false;

    usleep(300000);
    DesiredRotation = PresentRotation;
    DesiredGoniometer = PresentGoniometer;

    // Enable buttons
    enableButtons(true);
}*/

void MainWindow::on_pushButtonMagnifyBench_clicked()
{
    dlgZoom wZoom;

    //wMagnify.setWindowIcon(QIcon(":/myIcon.bmp"));
    wZoom.exec();    // open as modal dialog
}

void MainWindow::on_pushButton_FFC_clicked()
{
#ifdef __unix__
    switch(CameraType) {
    case MC320:
        CameraMc320.DoFFC();
        break;
    case TAU640:
        TauCommand.FFC();
        break;
    case PV640:
        CameraPv640.DoFFC();
        break;
    }
#else
    switch(CameraType) {
    case PHOTON:
        CameraPhoton.DoFFC();
        break;
    case INO:
        ui->radioButtonRotorAmbientBench->click();  // Set to clear in timer function
        enableButtons(false);// ?? can we re-enable?


        strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
        strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
        strcpy( ShortComment, "Dark" );     // Get the Short Comment

        NumFrames = ui->lineEditNumBBFramesBench->text().toFloat();

        AcqBusy = true;


#ifdef __unix__
        sem_post(&RunFfc_count);
#else
        SetEvent(RunFfc_count);
#endif


//        Cal.Set(ROTORPOSITION, ROTORAMBIENT);
//        Sleep(25000);   // Wait 25 seconds for wheel to finish moving

//        memcpy(m_nPhotonBackgroundBuffer, m_nPhotonFrameBuffer, IMAGESIZE*sizeof(unsigned short));

//        Cal.Set(ROTORPOSITION, ROTORPASS);
        break;
    }
#endif
}


/*// *****************************************************************
// User pushed the Acquire Manual button.
// *****************************************************************
void MainWindow::on_pushButtonAcquireManualBench_clicked()
{
    strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireManual();
}*/

void MainWindow::on_pushButtonAutoAcquireBench_clicked()
{
#ifdef __unix__
    sem_post(&RunAutoscan_count);
#else
    SetEvent(RunAutoscan_count);
#endif
}

// *****************************************************************
// User pushed the WaveCal button.
// *****************************************************************
void MainWindow::on_pushButtonWaveCalBench_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );                   //Get Wavelength
    ShortComment[0] = 0;
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireWaveCal();
}

void MainWindow::on_pushButtonAutoAcquireTripod_clicked()
{
}
void MainWindow::on_pushButtonWaveCalTripod_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryTripod->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthTripod->text().toAscii() );                   //Get Wavelength
    ShortComment[0] = 0;
    NumFrames = ui->lineEditNumWaveCalFramesTripod->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireWaveCal();
}

void MainWindow::on_pushButtonAutoAcquireFlight_clicked()
{
}

void MainWindow::on_pushButtonWaveCalFlight_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthFlight->text().toAscii() );                   //Get Wavelength
    ShortComment[0] = 0;
    NumFrames = ui->lineEditNumWaveCalFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireWaveCal();
}



// Save BBs
void MainWindow::on_pushButtonSavePaddleColdBench_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleCold();
}
void MainWindow::on_pushButtonSavePaddleHotBench_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleHot();
}
void MainWindow::on_pushButtonSaveRotorColdBench_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorCold();
}
void MainWindow::on_pushButtonSaveRotorHotBench_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesBench->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorHot();
}
void MainWindow::on_pushButtonSaveBbExtBench_clicked()
{
    char comstring [240] ;
    bool okConv = true ;
    // get the blackbody temp
    // if we have serial connect, will have the BB temp in textfield
    float bbVal = ui->lineEditTemperatureBbExtBench->text().toFloat(&okConv) ;
    
    BBDialog *bbdlg = new BBDialog (this) ;
    bbdlg->setTemp (bbVal) ;
    bbdlg->exec() ;
    qDebug () << "BB TEMP is : " << bbdlg->bbTemp ;
    qDebug () << "BB hot flag is : " << bbdlg->hotFlag ;

    TProc->setBBTemp (bbdlg->bbTemp, bbdlg->hotFlag) ;
    strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    sprintf (comstring, "bb_%04d", int(bbdlg->bbTemp+.1)) ;
    strcpy( ShortComment, comstring );     // Get the Short Comment
    NumFrames = ui->lineEditNumBBFramesBench->text().toInt();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireManualBB(bbdlg->bbTemp, bbdlg->hotFlag, TProc);
    // set the black body to hot or cold...


    delete bbdlg ;
}

void MainWindow::on_pushButtonSaveBbExt2Bench_clicked()
{
}


bool MainWindow::readProcessFile () {
    char wavefile[240], segfile[240] ;
    int nsampsInterf ;
    float xoff, yoff ;
    bool status ;

    status = ReadTircisConfig (wavefile, segfile, &nsampsInterf, &xoff, &yoff);
    if (!status) return (status) ;
    qDebug() << "Wavefile is : " << wavefile ;
    qDebug() << "Segfile is : " << segfile ;
    qDebug () << "Number of samples in interf segment : " << nsampsInterf ;
    TProc->setConfigParams (wavefile, segfile, nsampsInterf, xoff, yoff) ;

    return (status) ;
}

void MainWindow::on_pushButtonSavePaddleColdTripod_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryTripod->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthTripod->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentTripod->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesTripod->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleCold();
}
void MainWindow::on_pushButtonSavePaddleHotTripod_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryTripod->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthTripod->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentTripod->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesTripod->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleHot();
}
void MainWindow::on_pushButtonSaveRotorColdTripod_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryTripod->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthTripod->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentTripod->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesTripod->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorCold();
}
void MainWindow::on_pushButtonSaveRotorHotTripod_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryTripod->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthTripod->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentTripod->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesTripod->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorHot();
}

void MainWindow::on_pushButtonSavePaddleColdFlight_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthFlight->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentFlight->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleCold();
}
void MainWindow::on_pushButtonSavePaddleHotFlight_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthFlight->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentFlight->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquirePaddleHot();
}
void MainWindow::on_pushButtonSaveRotorColdFlight_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthFlight->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentFlight->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorCold();
}
void MainWindow::on_pushButtonSaveRotorHotFlight_clicked()
{
    strcpy(DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );    // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthFlight->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentFlight->text().toAscii() );     // Get the Short Comment
    NumFrames = ui->lineEditNumWaveCalFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireRotorHot();
}


// ********** SET BLACK BODY BENCH **********
void MainWindow::on_pushButtonSetPaddleColdBench_clicked()
{
    DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdBench->text().toFloat();
    Cal.Set(PADDLETEMPERATURECOLD, (float)DesiredTemperaturePaddleCold);
}
void MainWindow::on_pushButtonSetPaddleHotBench_clicked()
{
    DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotBench->text().toFloat();
    Cal.Set(PADDLETEMPERATUREHOT, (float)DesiredTemperaturePaddleHot);
}
void MainWindow::on_pushButtonSetRotorColdBench_clicked()
{
    DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdBench->text().toFloat();
    Cal.Set(ROTORTEMPERATURECOLD, (float)DesiredTemperatureRotorCold);
}
void MainWindow::on_pushButtonSetRotorHotBench_clicked()
{
    DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotBench->text().toFloat();
    Cal.Set(ROTORTEMPERATUREHOT, (float)DesiredTemperatureRotorHot);
}
void MainWindow::on_pushButtonSetExtBench_clicked()
{
    DesiredTemperatureBbExt = ui->lineEditDesiredTemperatureExtBench->text().toFloat();
    BBExternal.Set(DesiredTemperatureBbExt);
}

void MainWindow::on_pushButtonSetExt2Bench_clicked()
{
    DesiredTemperatureBbExt2 = ui->lineEditDesiredTemperatureExt2Bench->text().toFloat();
    BBExternal2.Set(DesiredTemperatureBbExt2);
}


// ********** SET BLACK BODY TRIPOD **********
void MainWindow::on_pushButtonSetPaddleColdTripod_clicked()
{
    DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdTripod->text().toFloat();
    Cal.Set(PADDLETEMPERATURECOLD, DesiredTemperaturePaddleCold );
}
void MainWindow::on_pushButtonSetPaddleHotTripod_clicked()
{
    DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotTripod->text().toFloat();
    Cal.Set(PADDLETEMPERATUREHOT, DesiredTemperaturePaddleHot);
}
void MainWindow::on_pushButtonSetRotorColdTripod_clicked()
{
    DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdTripod->text().toFloat();
    Cal.Set(ROTORTEMPERATURECOLD, DesiredTemperatureRotorCold);
}
void MainWindow::on_pushButtonSetRotorHotTripod_clicked()
{
    DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotTripod->text().toFloat();
    Cal.Set(ROTORTEMPERATUREHOT, DesiredTemperatureRotorHot);
}


// ********** SET BLACK BODY FLIGHT **********
void MainWindow::on_pushButtonSetPaddleColdFlight_clicked()
{
    DesiredTemperaturePaddleCold = ui->lineEditDesiredTemperaturePaddleColdFlight->text().toFloat();
    Cal.Set(PADDLETEMPERATURECOLD, DesiredTemperaturePaddleCold );
}
void MainWindow::on_pushButtonSetPaddleHotFlight_clicked()
{
    DesiredTemperaturePaddleHot = ui->lineEditDesiredTemperaturePaddleHotFlight->text().toFloat();
    Cal.Set(PADDLETEMPERATUREHOT, DesiredTemperaturePaddleHot );
}
void MainWindow::on_pushButtonSetRotorColdFlight_clicked()
{
    DesiredTemperatureRotorCold = ui->lineEditDesiredTemperatureRotorColdFlight->text().toFloat();
    Cal.Set(ROTORTEMPERATURECOLD, DesiredTemperatureRotorCold);
}
void MainWindow::on_pushButtonSetRotorHotFlight_clicked()
{
    DesiredTemperatureRotorHot = ui->lineEditDesiredTemperatureRotorHotFlight->text().toFloat();
    Cal.Set(ROTORTEMPERATUREHOT, DesiredTemperatureRotorHot);
}


void MainWindow::on_pushButtonHomeRotationBench_clicked()
{
    Motion.Home(0);
    //usleep(5500000);
    //Motion.Set(10, RotationHome, 0);
}

void MainWindow::on_pushButtonMoveRotationBench_clicked()
{
    // Get the desired position from the line edit.
    DesiredPositionRotation = RotationHome + ui->lineEditDesiredRotationBench->text().toFloat();
    DesiredSpeedRotation = m_nFastSpeedRotation;
    Motion.Set(DesiredSpeedRotation, DesiredPositionRotation, 0);
}
void MainWindow::on_pushButtonHomeGoniometerBench_clicked()
{
    Motion.Home(1);
    //Motion.Set(10, GoniometerHome, 1);
}
void MainWindow::on_pushButtonMoveGoniometerBench_clicked()
{
    float DesiredPosition = GoniometerHome + ui->lineEditDesiredGoniometerBench->text().toFloat();

    Motion.Set(m_nFastSpeedRotation, DesiredPosition, 1);
}


void MainWindow::on_pushButtonHomeRotationTripod_clicked()
{
    Motion.Home(0);
    //Motion.Set(10, RotationHome, 0);
}

void MainWindow::on_pushButtonMoveRotationTripod_clicked()
{
    // Get the desired position from the line edit.
    DesiredPositionRotation = ui->lineEditDesiredRotationTripod->text().toFloat() - RotationHome;
    DesiredSpeedRotation = m_nFastSpeedRotation;
    Motion.Set(DesiredSpeedRotation, DesiredPositionRotation, 0);
}
void MainWindow::on_pushButtonHomeGoniometerTripod_clicked()
{
    Motion.Home(1);
    //Motion.Set(10, GoniometerHome, 1);
}
void MainWindow::on_pushButtonMoveGoniometerTripod_clicked()
{
    float DesiredPosition = ui->lineEditDesiredGoniometerTripod->text().toFloat() - GoniometerHome;
    Motion.Set(m_nFastSpeedRotation, DesiredPosition, 1);
}


void MainWindow::on_radioButtonWave8Tripod_clicked(){ui->lineEditWavelengthTripod->setText("8.0");}
void MainWindow::on_radioButtonWave9Tripod_clicked(){ui->lineEditWavelengthTripod->setText("9.7");}
void MainWindow::on_radioButtonWave10Tripod_clicked(){ui->lineEditWavelengthTripod->setText("10.3");}

void MainWindow::on_radioButtonWave8Flight_clicked(){ui->lineEditWavelengthFlight->setText("8.0");}
void MainWindow::on_radioButtonWave9Flight_clicked(){ui->lineEditWavelengthFlight->setText("9.7");}
void MainWindow::on_radioButtonWave10Flight_clicked(){ui->lineEditWavelengthFlight->setText("10.3");}

void MainWindow::on_radioButtonPaddleColdT30_clicked(){ui->lineEditDesiredTemperaturePaddleColdFlight->setText("30");}
void MainWindow::on_radioButtonPaddleColdT35_clicked(){ui->lineEditDesiredTemperaturePaddleColdFlight->setText("35");}
void MainWindow::on_radioButtonPaddleColdT40_clicked(){ui->lineEditDesiredTemperaturePaddleColdFlight->setText("40");}
void MainWindow::on_radioButtonPaddleColdT45_clicked(){ui->lineEditDesiredTemperaturePaddleColdFlight->setText("45");}
void MainWindow::on_radioButtonPaddleColdT50_clicked(){ui->lineEditDesiredTemperaturePaddleColdFlight->setText("50");}

void MainWindow::on_radioButtonPaddleHotT30_clicked(){ui->lineEditDesiredTemperaturePaddleHotFlight->setText("30");}
void MainWindow::on_radioButtonPaddleHotT35_clicked(){ui->lineEditDesiredTemperaturePaddleHotFlight->setText("35");}
void MainWindow::on_radioButtonPaddleHotT40_clicked(){ui->lineEditDesiredTemperaturePaddleHotFlight->setText("40");}
void MainWindow::on_radioButtonPaddleHotT45_clicked(){ui->lineEditDesiredTemperaturePaddleHotFlight->setText("45");}
void MainWindow::on_radioButtonPaddleHotT50_clicked(){ui->lineEditDesiredTemperaturePaddleHotFlight->setText("50");}

void MainWindow::on_radioButtonRotorColdT30_clicked(){ui->lineEditDesiredTemperatureRotorColdFlight->setText("30");}
void MainWindow::on_radioButtonRotorColdT35_clicked(){ui->lineEditDesiredTemperatureRotorColdFlight->setText("35");}
void MainWindow::on_radioButtonRotorColdT40_clicked(){ui->lineEditDesiredTemperatureRotorColdFlight->setText("40");}
void MainWindow::on_radioButtonRotorColdT45_clicked(){ui->lineEditDesiredTemperatureRotorColdFlight->setText("45");}
void MainWindow::on_radioButtonRotorColdT50_clicked(){ui->lineEditDesiredTemperatureRotorColdFlight->setText("50");}

void MainWindow::on_radioButtonRotorHotT30_clicked(){ui->lineEditDesiredTemperatureRotorHotFlight->setText("30");}
void MainWindow::on_radioButtonRotorHotT35_clicked(){ui->lineEditDesiredTemperatureRotorHotFlight->setText("35");}
void MainWindow::on_radioButtonRotorHotT40_clicked(){ui->lineEditDesiredTemperatureRotorHotFlight->setText("40");}
void MainWindow::on_radioButtonRotorHotT45_clicked(){ui->lineEditDesiredTemperatureRotorHotFlight->setText("45");}
void MainWindow::on_radioButtonRotorHotT50_clicked(){ui->lineEditDesiredTemperatureRotorHotFlight->setText("50");}

/*MainWindow::on_pushButtonHomeRotationBench_clicked()
{
    //Rotation.Home();
}*/




void MainWindow::on_radioButtonPaddleColdBench_clicked()
{
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleHotBench_clicked()
{
    Cal.Set(PADDLEPOSITIONHOT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleAmbientBench_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleClearBench_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonRotorColdBench_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORCOLD);
}

void MainWindow::on_radioButtonRotorHotBench_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORHOT);
}

void MainWindow::on_radioButtonRotorAmbientBench_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORAMBIENT);
}

void MainWindow::on_radioButtonRotorClearBench_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORPASS);
}

void MainWindow::on_radioButtonPaddleColdTripod_clicked()
{
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleHotTripod_clicked()
{
    Cal.Set(PADDLEPOSITIONHOT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleAmbientTripod_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleClearTripod_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonRotorColdTripod_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORCOLD);
}

void MainWindow::on_radioButtonRotorHotTripod_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORHOT);
}

void MainWindow::on_radioButtonRotorAmbientTripod_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORAMBIENT);
}

void MainWindow::on_radioButtonRotorClearTripod_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORPASS);
}

void MainWindow::on_radioButtonPaddleColdFlight_clicked()
{
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleHotFlight_clicked()
{
    Cal.Set(PADDLEPOSITIONHOT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleAmbientFlight_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEON);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonPaddleClearFlight_clicked()
{
    Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
}

void MainWindow::on_radioButtonRotorColdFlight_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORCOLD);
}

void MainWindow::on_radioButtonRotorHotFlight_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORHOT);
}

void MainWindow::on_radioButtonRotorAmbientFlight_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORAMBIENT);
}

void MainWindow::on_radioButtonRotorClearFlight_clicked()
{
    Cal.Set(ROTORPOSITION, ROTORPASS);
}

void MainWindow::on_pushButton_RunScript_clicked()
{
    QString str = QFileDialog::getOpenFileName (this, "Work directory")  ;
    strcpy(ScriptFile, str.toLatin1());

    //AcqBusy = true;
    enableButtons(false);// ?? can we re-enable?

#ifdef __unix__
    sem_post(&RunScript_count);
#else
    SetEvent(RunScript_count);
#endif
    /*    on_radioButtonPaddleColdBench_clicked();
    usleep(20000000);
    o n_radioButtonPaddleHotBench_clicked();
    usleep(20000000);
    o n_radioButtonPaddleAmbientBench_clicked();
    usleep(20000000);
    o n_radioButtonPaddleClearBench_clicked();
    usleep(20000000);
    o n_radioButtonRotorColdBench_clicked();
    usleep(20000000);
    o n_radioButtonRotorHotBench_clicked();
    usleep(20000000);
    o n_radioButtonRotorAmbientBench_clicked();
    usleep(20000000);
    o n_radioButtonRotorClearBench_clicked();*/
}

void MainWindow::on_radioButtonPaddleColdOnBench_clicked()
{
    Cal.Set(PADDLEPOWERCOLD, POWERON);
}

void MainWindow::on_radioButtonPaddleColdOffBench_clicked()
{
    Cal.Set(PADDLEPOWERCOLD, POWEROFF);
}

void MainWindow::on_radioButtonRotorColdOnBench_clicked()
{
    Cal.Set(ROTORPOWERCOLD, POWERON);
}

void MainWindow::on_radioButtonRotorColdOffBench_clicked()
{
    Cal.Set(ROTORPOWERCOLD, POWEROFF);
}


void MainWindow::on_radioButtonPaddleHotOnBench_clicked()
{
    Cal.Set(PADDLEPOWERHOT, POWERON);
}

void MainWindow::on_radioButtonPaddleHotOffBench_clicked()
{
    Cal.Set(PADDLEPOWERHOT, POWEROFF);
}

void MainWindow::on_radioButtonRotorHotOnBench_clicked()
{
    Cal.Set(ROTORPOWERHOT, POWERON);
}

void MainWindow::on_radioButtonRotorHotOffBench_clicked()
{
    Cal.Set(ROTORPOWERHOT, POWEROFF);
}

//void MainWindow::on_pushButton_GetTemperature_clicked()
//{
//     TemperatureToggle = !TemperatureToggle;
//}

void MainWindow::on_pushButtonProcessDataBench_clicked()
{
    //mainProc->ShowMainProc();
/*    // set the blackbodies
    //mainProc->setBlackBodies (TProc->coldFile, TProc->hotFile, TProc->coldTemp, TProc->hotTemp) ;
    //mainProc->setScanFile (TProc->scanFile) ;
    mainProc->setTProcess (TProc) ;
    mainProc->parseTProcess () ;
    char FilenameProc[2048];
    mainProc->startProcess() ;
    mainProc->start() ;


    if( DataDirectory[strlen(DataDirectory)-1] != '/' ) strcat(DataDirectory, "/");
    sprintf(FilenameProc, "%snewproc.txt" , DataDirectory);

    //    //Tircis_Proc_start StartProcess;
    //    //StartProcess.exec();    // open as modal dialo

    //if(mp == NULL) {
    //mp = new MainProcess () ;
    //connect (mp->prdlg, SIGNAL (closeUp()), this, SLOT(closeup())) ;
    //}

#ifdef __unix__
    mainProc->readProcessFile (FilenameProc) ;
    mainProc->start () ;
#endif*/
}

void MainWindow::on_pushButtonProcessData_clicked()
{
    //    Tircis_Proc_start StartProcess;
    //    StartProcess.exec();    // open as modal dialog
    on_pushButtonProcessDataBench_clicked();
}

void MainWindow::on_lineEditWavelengthBench_editingFinished()
{
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
}



void MainWindow::on_pushButtonGroundAlign_clicked()
{
    Gps.AlignmentMode = GPS_FINEALIGN;
    Gps.Init();
}

void MainWindow::on_pushButtonAirAlign_clicked()
{
    Gps.AlignmentMode = GPS_AIRALIGN;
    Gps.Init();
}

void MainWindow::on_pushButtonAcquireManualStopBench_clicked()
{
    Abort = TRUE;
}

void MainWindow::on_pushButton_ShowRaw_clicked()
{
    memset(m_nPhotonBackgroundBuffer, 0, MAXIMAGESIZE*sizeof(unsigned short));
}


// this is the auto scale for the single frame display, if its on, auto scale is in
// effect
void MainWindow::on_radioButtonImageAutoBench_toggled(bool checked)
{

    //float mn, mx ;
    //bool autoState = checked ;
    // turn on or off the auto scaling of the profile plot

}

// this is the auto scale for the raw or fft profile display, if its on, auto scale is in
// effect
void MainWindow::on_radioButtonFftAutoBench_toggled(bool checked)
{
    ui->widgetFftPlotBench->setRescaleFlag (checked) ;

}

void MainWindow::on_radioButtonPixelAutoBench_toggled(bool checked)
{
    ui->widgetPixelPlotBench->setRescaleFlag (checked) ;
}

void MainWindow::on_pushButtonCamerasettings_clicked()
{
    InoSettingsDialog->show();
    InoSettingsDialog->ShowStats();
}

void MainWindow::on_pushButtonScanStage_clicked()
{
    char buff[128];

    int Start = ui->lineEditScanStageStart->text().toInt();
    int Stop = ui->lineEditScanStageStop->text().toInt();
    int Rate = ui->lineEditScanStageRate->text().toInt();

    // *** Set num of frames to match the scan angle.
    NumTargetFrames = abs( (int)((ScanStageStopPosition-ScanStageStartPosition) * FrameRate/ScanStageSlowSpeed + 0.5) );
    sprintf(buff, "%d" , NumTargetFrames);
    ui->lineEditScanStageNumFrames->setText(buff);


    strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentBench->text().toAscii() );     // Get the Short Comment

    SetEvent(RunScan_count);
}

void MainWindow::on_pushButtonAcquireManualFlight_clicked()
{
    strcpy( DataDirectory, ui->lineEditDataDirectoryFlight->text().toAscii() );   // Get the Data Directory from the lineedit
    strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
    strcpy( ShortComment, ui->lineEditShortCommentFlight->text().toAscii() );     // Get the Short Comment

    NumFrames = ui->lineEditNumFramesFlight->text().toFloat();

    AcqBusy = true;
    enableButtons(false);
    DoAcquireManual(TProc);
}

void MainWindow::on_pushButtonAcquireManualStopFlight_clicked()
{
    Abort = TRUE;
}

// check box for fft toggle
// set fftState to whatever cb  state
void MainWindow::on_checkBox_clicked()
{
    bool cbstate = ui->checkBox->isChecked() ;
    ui->widgetFftPlotBench->setFFTState(cbstate) ;
}
