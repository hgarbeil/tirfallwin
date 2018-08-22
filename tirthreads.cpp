#ifdef _WIN32
#include "afxwin.h"
#endif
//:O#include "cssl_lib.h"

#include <QMessageBox>
//:P#include <QDebug>
#ifdef __unix__
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include "acqmc320.h"
#include "acqpv640.h"
#include "acqtau.h"
#include "acqtauserial.h";
#endif
#include <string.h>
#include <math.h>
//#include <sys/types.h>     // stat
#include <sys/stat.h>      // stat

//#include "cssl_lib.h"
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
#include "acqphoton.h"
#include "acqino.h"

#include "smc100.h"
#include "fileFctns.h"
#include "calibrator.h"

/*#define Int16 short
#define uInt16 unsigned short
#define uInt32 unsigned int
#define BYTE unsigned char
#define DWORD unsigned long*/

#include <QImage>
#include "configCosmos.h"
#include "COMDevice.h"
#include "global.h"
#include "defines.h"
#include "fileFctns.h"
#include "threads.h"
#include "bbsr800.h"
#include "bbsr800gpib.h"
//#define BUFFERLENGTH 1024

#ifdef __unix__
#include "mainprocess.h"
extern MainProcess *mp;
#endif

int AcquireWithMotion( float SlowSpeedRotationL, float StartPositionRotationL, float StopPositionRotationL );
int AcquireNoMotion(void);

extern Calibrator Cal;
extern Smc100 Motion;
extern BbSr800 BBExternal;
extern BbSr800Gpib BBExternal2;
#ifdef __unix__
extern TauSerial TauCommand;
#endif
extern int CameraType;
extern unsigned short m_nPhotonFrameBuffer[];
extern unsigned short m_nPhotonBackgroundBuffer[];
extern int FfcDone;
//extern float PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot;

//sem_t RunAutoScan_count;
//void* runAutoScan(void *args);
int NumCalculatedFrames;
bool LogIsOpen = false;
bool LogCmdIsOpen = false;
FILE *fpLog=NULL;
FILE *fpLogCmd=NULL;


#ifdef __unix__
void tic(timeval *startTime)
{
    gettimeofday(startTime, NULL);
}
#else
void tic(time_t *startTime)
{
    time( startTime );
}
#endif

// In seconds
#ifdef __unix__
long toc(timeval startTime)
{
    long seconds, useconds;
    long duration;
    timeval endTime;

    gettimeofday(&endTime, NULL);
    seconds  = endTime.tv_sec  - startTime.tv_sec;
    useconds = endTime.tv_usec - startTime.tv_usec;
    duration = (long)((float)seconds + (float)useconds/1000000.0 + (float).5);
    return(duration);
}
#else
long toc(time_t startTime)
{
    long duration;
    time_t endTime;

    time( &endTime );
    duration = endTime - startTime;
    return(duration);
}
#endif


void Log(char *Entry)
{
    static bool LogBusy = false;
    char Message[2048];
    time_t t;
    tm *gmtm;

    if(LogIsOpen) {
        while( LogBusy == true) {
            COSMOS_USLEEP(200000);
        }
        LogBusy = true;
        // Get the time
        t = time(NULL);
        gmtm = gmtime(&t);
        if( strstr(Entry, "\r\n")==NULL ) {
            sprintf(Message, "%4d%02d%02d%02d%02d%02d,%s\r\n", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, Entry);
        } else {
            sprintf(Message, "%4d%02d%02d%02d%02d%02d,%s", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, Entry);
        }

        fwrite( Message, sizeof(char), strlen(Message), fpLog );
        LogBusy = false;
    }
}

void LogCmd(char *Entry)
{
    static bool LogCmdBusy = false;
    char Message[2048];
    time_t t;
    tm *gmtm;

    if(LogCmdIsOpen) {
        while( LogCmdBusy == true) {
            COSMOS_USLEEP(200000);
        }
        LogCmdBusy = true;
        // Get the time
        t = time(NULL);
        gmtm = gmtime(&t);
        if( strstr(Entry, "\r\n")==NULL ) {
            sprintf(Message, "%4d%02d%02d%02d%02d%02d,%s\r\n", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, Entry);
        } else {
            sprintf(Message, "%4d%02d%02d%02d%02d%02d,%s", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec, Entry);
        }

        fwrite( Message, sizeof(char), strlen(Message), fpLogCmd );
        LogCmdBusy = false;
    }
}

bool OpenLog()
{
    time_t t;
    tm *gmtm;
    struct stat st;
    char Filename[2048];
    char Message[2048];

    if (stat("log",&st) != 0) {  // && !S_ISDIR(st.st_mode))
        system("mkdir log");
    }

    // Get the time
    t = time(NULL);
    gmtm = gmtime(&t);
    sprintf(Filename, "log/%4d%02d%02d%02d%02d%02d-TircisLog.txt", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec);


    fpLog = fopen(Filename, "wt");
    if(fpLog==NULL) {
        LogIsOpen=false;
        // Send Debug Server Message?
        return(false);
    } else {
        LogIsOpen=true;
        // Send Debug Server Message?
        Log((char*)"Log opened");
        strcpy(Message, "\r\n     TimeStamp, ColdPaddle, HotPaddle, ColdRear, HotRear,   T1,   T2,   T3,   T4,   T5,   T6, Cam T, ColdPaddle, HotPaddle, ColdRear, HotRear, Rot, Goniometer\r\n");

        fwrite( Message, sizeof(char), strlen(Message), fpLog );
        return(true);
    }
}

bool OpenLogCmd()
{
    time_t t;
    tm *gmtm;
    struct stat st;
    char Filename[2048];
    //char Message[2048];

    if (stat("log",&st) != 0) {  // && !S_ISDIR(st.st_mode))
        system("mkdir log");
    }

    // Get the time
    t = time(NULL);
    gmtm = gmtime(&t);
    sprintf(Filename, "log/%4d%02d%02d%02d%02d%02d-TircisCmdLog.txt", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec);


    fpLogCmd = fopen(Filename, "w");
    if(fpLogCmd==NULL) {
        LogCmdIsOpen=false;
        // Send Debug Server Message?
        return(false);
    } else {
        LogCmdIsOpen=true;
        // Send Debug Server Message?
        //:PLogCmd((char*)"Log opened");
        //strcpy(Message, "\r\n   TimeStamp,   ColdPaddle, HotPaddle, ColdRear, HotRear,   T1,   T2,   T3,   T4,   T5,   T6, Cam T, ColdPaddle, HotPaddle, ColdRear, HotRear, Rot, Goniometer\r\n");
        //fwrite( Message, sizeof(char), strlen(Message), fpLogCmd );
        return(true);
    }
}
void CloseLog()
{
    if(LogIsOpen) {
        fclose(fpLog);
    }
}

void CloseLogCmd()
{
    if(LogCmdIsOpen) {
        fclose(fpLogCmd);
    }
}

void FlushLog()
{
    if(LogIsOpen) {
        fflush(fpLog);
    }
}




// *****************************************************************
// Do a complete stare (scan cold, stare scene, wscan hot)
// *****************************************************************
#ifdef __unix__
void* runAutoScan(void *args)
#else
DWORD WINAPI runAutoScan(LPVOID pParam)
#endif
{
    //int iVal;
    //char *RetVal;
    //struct stat st;
    //int FileNameLength;
    //char SrcFilename[4096];
    //, DestFilename[4096]; //, OldFiles[4096];
    //char PathToCreate[2048];
    //int PosInStr;
    char FullPath[2048];
    char DateTimeStamp[2048];
    char TimeStamp[2048];
    char FilenameCold[2048];
    char FilenameScene[2048];
    char FilenameHot[2048];
    char LongComment[2048];
    char FileExtension[16];
    char FilenameWithExtensionCold[2064];
    char FilenameWithExtensionScene[2064];
    char FilenameWithExtensionHot[2064];
    //char HeaderFilePath[2048];
    //char CommandLine[2048];
    //char outBuffer[BUFFERLENGTH];
    float PresentTemperatureCold;
    //float PresentTemperatureScene;
    float PresentTemperatureHot;
    //bool WaveCalCopied;
    //timeval startTime;

    // ***** Allow access to UI.
    //    Ui::MainWindow *ui;
    //    ui = (Ui::MainWindow*)args; // Let this "friend function" access Ui

    //sem_init (&RunAutoScan_count, 0, 0);

    // Home Rotation
    //Rotation.Home();
    //DesiredSpeedRotation = m_nFastSpeedRotation;
    //DesiredPositionRotation = RotationHome;
    //Rotation.Set(DesiredSpeedRotation, DesiredPositionRotation);


    strcpy(FileExtension, ".bsq");
    //strcpy(LongComment, "WaxView Stare on Linux System");
    //strcpy(Wavelength, "10.53");


    while(!ExitProgram) {
Abort2:
        // If the setpoint is already at proper temperature, then these return without doing anything.
        //        Cal.Set(PADDLETEMPERATURECOLD, 25);
        //        Cal.Set(PADDLETEMPERATUREHOT, 25);
        AcqBusy = false;
        //Fiducial.Set(FIDUCIALOFF);


        // ************ Wait for stare message. ************
#ifdef __unix__
        sem_wait (&RunAutoscan_count);
#else
        WaitForSingleObject (RunAutoscan_count, INFINITE);
#endif

        if(ExitProgram) break;

        // Turn on BB's
        Cal.Set(PADDLETEMPERATURECOLD, DesiredTemperaturePaddleCold);
        Cal.Set(PADDLETEMPERATUREHOT, DesiredTemperatureRotorCold);

        // Get number of frames
        NumCalculatedFrames = (fabs(StopPositionRotation-StartPositionRotation))*(float)FrameRate/SlowSpeedRotation;


        // Turn on heater
        //Fiducial.Set(FIDUCIALON);

#ifdef __unix__
        // Do a FFC (Flat Field Correction) before starting the scan.
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
#endif
        switch(CameraType) {
        case PHOTON:
            CameraPhoton.DoFFC();
            break;
        }

        // *****************  Stow Paddles  *****************
        Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
        Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
        COSMOS_USLEEP(1000000);


        /*        // Wait or timeout after 25 secs
        tic(&startTime);
        while( Fiducial.Get()<FIDUCIALON-2 ) {
            if(toc(startTime)>30) { // Check for timeout 25 sec
                break;
            }
            COSMOS_USLEEP(300000);
            if(Abort) {
                goto Abort2;
            }
        }*/


        // ********  Make Directory for Stare set  *********
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(DateTimeStamp, "%4d%02d%02d_%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");     // Is this already done??
        //CreateFilePathString(FullPath, DataDirectory, DateTimeStamp, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
        CreateFilePathString(FullPath, DataDirectory, DateTimeStamp, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
        MakeDirectoryPath( FullPath/*, DateStamp, DataDirectory, DateTimeStamp, bUseDirectoryStructure, ShortComment, bSkipBlackBodies*/ );
        //strcpy( SaveDirectoryArray, FullPath );



        // ************  Make Scene File Name  *************
        t = time(NULL);
        tm = *localtime(&t);
        //sprintf(DateTimeStamp, "%4d%02d%02d_%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        sprintf(TimeStamp, "%02d%02d%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        //sprintf( FilenameScene, "LWIR_%s%s", DateTimeStamp, ShortComment);
        sprintf( FilenameScene, "LWIR_%s", TimeStamp);
        sprintf( FilenameWithExtensionScene, "%s%s", FilenameScene, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameScene, FileExtension );

        // *****************  Stare Scene  *****************
        if( AcquireWithMotion(SlowSpeedRotation, StartPositionRotation, StopPositionRotation)<0 ) goto Abort2;
        if(Abort) goto Abort2;





        // Get number of frames for CALS
        //NumCalculatedFrames = (fabs(StopPositionRotationCal-StartPositionRotationCal))*(float)FrameRate/SlowSpeedRotationCal;


        // ***********  Make Cold BB File Name  ************
        //PresentTemperatureCold =
        Cal.Get(&TemperaturePaddleCold, &TemperaturePaddleHot,
                &TemperatureRotorCold, &TemperatureRotorHot, TemperatureSensor);
        PresentTemperatureCold = TemperaturePaddleCold;
        //sprintf( FilenameCold, "LWIR_%s_BB_COLD%02d", DateTimeStamp, (int)(PresentTemperatureCold+0.5) );
        sprintf( FilenameCold, "LWIR_%s_BB_COLD%02d", TimeStamp, (int)(PresentTemperatureCold+0.5) );
        sprintf( FilenameWithExtensionCold, "%s%s", FilenameCold, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameCold, FileExtension );

        // *****************  Activate Cold Paddle  *****************
        Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);

        // ****************  Stare Cold BB  ****************
        AcquireNoMotion();
        if(Abort) goto Abort2;





        /*        // Make Temp Directory
        if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
        sprintf(PathToCreate, "%sTemp/" , DataDirectory);

        if (stat(PathToCreate,&st) != 0) {  // && !S_ISDIR(st.st_mode))
            iVal = mkdir(PathToCreate, 0777);
        }

        // Copy Cold BB to Temp directory
        sprintf(DestFilename, "%sTemp/ColdBB.bsq" , DataDirectory);
        CopyFile(FilePath, DestFilename);	// Copy Wavelength*/






        // ************  Make Hot BB File Name  ************
        t = time(NULL);
        tm = *localtime(&t);
        sprintf(DateTimeStamp, "%4d%02d%02d_%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        //PresentTemperatureHot = BbUnitB.Get();
        Cal.Get(&TemperaturePaddleCold, &TemperaturePaddleHot,
                &TemperatureRotorCold, &TemperatureRotorHot, TemperatureSensor);
        PresentTemperatureHot = TemperaturePaddleHot;

        //sprintf( FilenameHot, "LWIR-%s-BB_HOT%02d", DateTimeStamp, (int)(PresentTemperatureHot+0.5) );
        sprintf( FilenameHot, "LWIR_%s_BB_HOT%02d", TimeStamp, (int)(PresentTemperatureHot+0.5) );
        sprintf( FilenameWithExtensionHot, "%s%s", FilenameHot, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameHot, FileExtension );



        // *****************  Stow Cold Paddle  *****************
        Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
        COSMOS_USLEEP(1000000);


        // *****************  Activate Hot Paddle  *****************
        Cal.Set(PADDLEPOSITIONHOT, PADDLEON);

        // ****************  Stare Hot BB  *****************
        AcquireNoMotion();
        if(Abort) goto Abort2;



        /*        // Copy Hot BB to Temp directory
        sprintf(DestFilename, "%sTemp/HotBB.bsq" , DataDirectory);
        CopyFile(FilePath, DestFilename);	// Copy Wavelength*/






        // *****************  Stow Hot Paddle  *****************
        Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
        COSMOS_USLEEP(1000000);



        //        // *****  Copy files to processing directory  *****
        //        WaveCalCopied = CopyToProcessingDir( FullPath, DataDirectory);



        // *************  Save Cold BB Header  *************
        MakeHeader( FullPath, FilenameWithExtensionCold, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, 0, PresentTemperatureCold,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );



        //*** Write Header
        /*        sprintf(HeaderFilePath, "%s%s", FullPath, FilenameWithExtensionCold);
        FileNameLength = strlen(HeaderFilePath);
        PosInStr = FileNameLength-4;
        HeaderFilePath[PosInStr] = 0;	// Chop off extension
        strcat(HeaderFilePath, ".hdr");

        // Copy Cold BB header to Temp directory
        sprintf(DestFilename, "%sTemp/ColdBB.hdr" , DataDirectory);
        CopyFile(HeaderFilePath, DestFilename);	// Copy Wavelength
*/



        // **************  Save Hot BB Header  *************
        MakeHeader( FullPath, FilenameWithExtensionHot,  NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, PresentTemperatureHot, 0,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );



        //*** Write Header
        /*        sprintf(HeaderFilePath, "%s%s", FullPath, FilenameWithExtensionHot);
        FileNameLength = strlen(HeaderFilePath);
        PosInStr = FileNameLength-4;
        HeaderFilePath[PosInStr] = 0;	// Chop off extension
        strcat(HeaderFilePath, ".hdr");

        // Copy Cold BB header to Temp directory
        sprintf(DestFilename, "%sTemp/HotBB.hdr" , DataDirectory);
        CopyFile(HeaderFilePath, DestFilename);	// Copy Wavelength
*/




        // Get number of frames
        NumCalculatedFrames = (fabs(StopPositionRotation-StartPositionRotation))*(float)FrameRate/SlowSpeedRotation;

        // **************  Save Scene Header  **************
        MakeHeader( FullPath, FilenameWithExtensionScene,NumCalculatedFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, PresentTemperatureHot, PresentTemperatureCold,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


        // *************  Done, move to Home  **************
        //DesiredPositionRotation = RotationHome + StartPositionRotation + (StopPositionRotation-StartPositionRotation)/2.0;
        Motion.Set(m_nFastSpeedRotation, 0, 0);


        /*        if(WaveCalCopied && AutoprocessOn) {

            sprintf(outBuffer, "#LOG,1,DEBUG, Processing Data: %s\r\n" , FullPath);
            SendMsgDebug(outBuffer);

            // ***** Call Mike's Process function (while moving to position) ***** MIKE MIKE MIKE !!!!!
            sprintf( FilePath, "%s%s", FullPath, FilenameWithExtensionScene );
            sprintf(outBuffer, "#CmdComplete:%lu,PASS\r\n" , StareSequenceNumber);SendMsgCmd(outBuffer);

            WaxViewStatus = PROCESSINGTASK;
            sprintf(CommandLine, "/home/spiuser/Desktop/WaxViewP2/PacificSpectral/process.sh %s" , FilePath);
            system( CommandLine );
            wait();
        }*/

        //        Fiducial.Set(FIDUCIALOFF);
        //        Paddle.SetIlluminator(1, 0);    // Turn off Illuminator
        AcqBusy = false;

        /*        // Wait till arrived home
        tic(&startTime);
        while( fabs(Rotation.Get()-HOMEVIEW) > .5 ) {
            if(toc(startTime)>10) {
                break;
            }
            COSMOS_USLEEP(100000);
            if(Abort) goto Abort2;
        }*/

        /*        WaxViewStatus = IDLETASK;

        strcpy(outBuffer, "#LOG,1,DEBUG, Stare Complete\r\n");
        SendMsgDebug(outBuffer);

        sprintf(outBuffer, "#CmdComplete:%lu,PASS\r\n" , StareSequenceNumber);
        SendMsgCmd(outBuffer);*/
    }
    return(NULL);
}



// *****************************************************************
// Do a stare or scan on one component (item)
// *****************************************************************
int AcquireWithMotion( float SlowSpeedRotationL, float StartPositionRotationL, float StopPositionRotationL )
{
    int Timeout;
#ifdef __UNIX__
    timeval startTime;
#else
    time_t startTime;
#endif
    bool TimeoutRotationDone = false;
    //TimeoutFiducial = false;


    // ***** Move to Start
    if(StartPositionRotationL<StopPositionRotationL) {
        DesiredPositionRotation = StartPositionRotationL - RotationHome;
    } else {
        DesiredPositionRotation = StartPositionRotationL - RotationHome;
    }
    Motion.Set(m_nFastSpeedRotation, DesiredPositionRotation, 0);


    // ***** Wait till in position
    Timeout = 50/m_nFastSpeedRotation * 2;    // * 2 so the timout is double what it should take.
    tic(&startTime);  // Get start time
    while( fabs(Motion.Get(0)-DesiredPositionRotation) > .1 ) {
        if(toc(startTime)>Timeout) { // Check for 10 second timeout
            //TimeoutRotationStart = true;
            //StatusRotation = -1;   //Timeout
            return(-1);
        }
        COSMOS_USLEEP(100000);
        if(Abort) {
            Motion.Halt();
            return(0);
        }
    }
    COSMOS_USLEEP(600000);


    // ***** Start Motion (Move to End)
    if(StartPositionRotationL<StopPositionRotationL) {
        DesiredPositionRotation = StopPositionRotationL - RotationHome;
    } else {
        DesiredPositionRotation = StopPositionRotationL - RotationHome;
    }
    Motion.Set(SlowSpeedRotationL, DesiredPositionRotation, 0);


    // ***** Start acquire
    NumCalculatedFrames = abs( (int)((ScanStageStopPosition-ScanStageStartPosition) * FrameRate/ScanStageSlowSpeed + 0.5) );
    NumFrames = NumCalculatedFrames;
#ifdef __unix__
    sem_post (&SaveStart_count);
#else
    SetEvent (SaveStart_count);
#endif

    // ***** Wait till done
#ifdef __unix__
    sem_wait (&AcquireFinished_count);
#else
    WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif

    if(ExitProgram) return(0);


    // ***** Wait for Motion to stop.  We need this so that the home can be done
    Timeout = abs( (int)(StopPositionRotationL-StartPositionRotationL) ) / SlowSpeedRotationL * 2;    // * 2 so the timout is double what it should take.
    tic(&startTime);  // Get start time
    while( fabs(Motion.Get(0)-DesiredPositionRotation) > .5 ) {
        if(toc(startTime)>Timeout) { // Check for timeout
            TimeoutRotationDone = true;
            //StatusRotation = -1;   //Timeout
            return(-2);
        }
        COSMOS_USLEEP(100000);
        if(Abort) {
            Motion.Halt();
            return(0);
        }
    }
    COSMOS_USLEEP(100000);


    // ***** Rotation.Home();
    Motion.Set(m_nFastSpeedRotation, 0, 0);

    return(0);
}


// *****************************************************************
// Do a stare or scan on one component (item)
// *****************************************************************
int AcquireNoMotion(void)
{
    // ***** Start acquire
    NumFrames = NumCalculatedFrames;
#ifdef __unix__
    sem_post (&SaveStart_count);
#else
    SetEvent (SaveStart_count);
#endif

    // ***** Wait till done
#ifdef __unix__
    sem_wait (&AcquireFinished_count);
#else
    WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif

    //if(ExitProgram)
    return(0);
}


// *****************************************************************
// Do a complete stare (scan cold, stare scene, wscan hot)
// *****************************************************************
#ifdef __unix__
void* runScript(void *args)
#else
DWORD WINAPI runScript(LPVOID pParam)
#endif
{
    int i;
    //, iVal;
    //char *RetVal;
    //int FileNameLength;
    //char SrcFilename[4096], DestFilename[4096];
    //char PathToCreate[2048];
    //int PosInStr;
    //char FullPath[2048];
    //char DateTimeStamp[2048];
    //char TimeStamp[2048];
    //char FilenameCold[2048];
    //char FilenameScene[2048];
    //char FilenameHot[2048];
    //char LongComment[2048];
    //char FileExtension[16];
    //char FilenameWithExtensionCold[2064];
    //char FilenameWithExtensionScene[2064];
    //char FilenameWithExtensionHot[2064];
    //char HeaderFilePath[2048];
    //char CommandLine[2048];
    //float PresentTemperatureCold;
    //float PresentTemperatureScene;
    //float PresentTemperatureHot;
    //bool WaveCalCopied;
    //timeval startTime;

    char* strPosition1;
    //char* strPosition2;
    //char* strPosition3;
    char strColdOrHot[64];
    float ColdTemperature;
    float HotTemperature;
    char line[512];
    //char strParm1[512];
    static char ScriptDirectory[2048];
    int Parm;
    float SpeedRotation, PositionRotation;
    float SpeedGoniometer, PositionGoniometer;
    //int Channel;
    FILE *fp;
    FILE *fpProc;
    char FilenameProc[2048];
    float PaddleColdSetPoint, PaddleHotSetPoint, RotorHotSetPoint;
    //strcpy(FileExtension, ".bsq");
    //char FilenameWithExtension[2048];
    //char Filename[2048];
    char ColdFile[512];
    char HotFile[512];

    //char WorkingDir[512];
    char ParmsList[512];
    char Wavecal[2048];
    char ScanFile[512];
    char buffer[512];

    while(!ExitProgram) {
//Abort2:
        AcqBusy = false;


        // ************ Wait for script message. ************
#ifdef __unix__
        sem_wait (&RunScript_count);
#else
        WaitForSingleObject (RunScript_count, INFINITE);
#endif

        if(ExitProgram) break;




        // Script loop


        if( (fp = fopen(ScriptFile, "rt")) == NULL ) {
            return(NULL);
        }

        //:PLog((char *)"--> Starting Script <--");

        *line = 0;
        while ( fgets ( line, sizeof(line), fp ) != NULL ) { /* read a line */
            /*SpacePosition = strstr(line, " ");
            if(SpacePosition != NULL) {
                *SpacePosition=0;
                SpacePosition++;*/

            //:PqDebug() << "Line:" << line;

            for(i=0 ; i<7 ; i++) {
                line[i] = toupper(line[i]);
            }

            // if we are getting filenames or directories, dont ucase the parameters
            if( (strncmp(line, "ACQUIRE", 7)!=0) && (strncmp(line, "DIRECTO", 7)!=0) ) {
                // Make uppercase
                strupper(line);
            }

            if(        strncmp(line, "PADDLE AMBIENT IN", 17) == 0 ) {
                Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEON);
                Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
                Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
                //} else if( strncmp(line, "ROTATION STAGE", 14) == 0 ) {

            } else if( strncmp(line, "PADDLE COLD IN", 14) == 0 ) {
                Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);
                Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
                Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
            } else if( strncmp(line, "PADDLE HOT IN", 13) == 0 ) {
                Cal.Set(PADDLEPOSITIONHOT, PADDLEON);
                Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
                Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
            } else if( strncmp(line, "PADDLE CLEAR", 12) == 0 ) {
                Cal.Set(PADDLEPOSITIONAMBIENT, PADDLEOFF);
                Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
                Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);

            } else if( strncmp(line, "ROTOR AMBIENT IN", 16) == 0 ) {
                Cal.Set(ROTORPOSITION, ROTORAMBIENT);
            } else if( strncmp(line, "ROTOR COLD IN", 13) == 0 ) {
                Cal.Set(ROTORPOSITION, ROTORCOLD);
            } else if( strncmp(line, "ROTOR HOT IN", 12) == 0 ) {
                Cal.Set(ROTORPOSITION, ROTORHOT);
            } else if( strncmp(line, "ROTOR CLEAR", 11) == 0 ) {
                Cal.Set(ROTORPOSITION, ROTORPASS);


            } else if( strncmp(line, "PADDLE COLD WAIT", 16) == 0 ) {
                PaddleColdSetPoint = atof(&line[16]);
                Cal.Set(PADDLEPOWERCOLD, POWERON);
                Cal.Set(PADDLETEMPERATURECOLD, PaddleColdSetPoint);
                while(PresentTemperaturePaddleCold > PaddleColdSetPoint)
                    ;
                Cal.Set(PADDLEPOWERCOLD, POWEROFF);

            } else if( strncmp(line, "PADDLE HOT TEMPERATURE", 22) == 0 ) {
                // wait till gets to temperature
                PaddleHotSetPoint = atof(&line[22]);
                Cal.Set(PADDLEPOWERHOT, POWERON);
                Cal.Set(PADDLETEMPERATUREHOT, PaddleHotSetPoint);

            } else if( strncmp(line, "PADDLE HOT WAIT", 15) == 0 ) {
                // wait till gets to temperature
                PaddleHotSetPoint = atof(&line[15]);
                Cal.Set(PADDLEPOWERHOT, POWERON);
                Cal.Set(PADDLETEMPERATUREHOT, PaddleHotSetPoint);
                while(PresentTemperaturePaddleHot < PaddleHotSetPoint)
                    ;
                Cal.Set(PADDLEPOWERHOT, POWEROFF);


            } else if( strncmp(line, "ROTOR HOT WAIT", 14) == 0 ) {
                // wait till gets to temperature
                //sscanf(line, "%*s %f" , &val);
                RotorHotSetPoint = atof(&line[14]);
                Cal.Set(ROTORPOWERHOT, POWERON);
                Cal.Set(ROTORTEMPERATUREHOT, RotorHotSetPoint);
                while(PresentTemperatureRotorHot < RotorHotSetPoint)
                    ;
                Cal.Set(ROTORPOWERHOT, POWEROFF);


            } else if( strncmp(line, "PADDLE COLD", 11) == 0 ) {
                Parm = atoi(&line[11]);
                if( strncmp(&line[12], "DISABLE", 7)==0 ) {
                    Cal.Set(PADDLEPOWERCOLD, POWEROFF);
                } else if( strncmp(&line[12], "ENABLE", 6)==0 ) {
                    Cal.Set(PADDLEPOWERCOLD, POWERON);
                } else if( Parm>5 ) {
                    Cal.Set(PADDLETEMPERATURECOLD, Parm);
                }
            } else if( strncmp(line, "PADDLE HOT", 10) == 0 ) {
                Parm = atoi(&line[10]);
                if( strncmp(&line[11], "DISABLE", 7)==0 ) {
                    Cal.Set(PADDLEPOWERHOT, POWEROFF);
                } else if( strncmp(&line[11], "ENABLE", 6)==0 ) {
                    Cal.Set(PADDLEPOWERHOT, POWERON);
                } else if( Parm>5 ) {
                    Cal.Set(PADDLETEMPERATUREHOT, Parm);
                }
            } else if( strncmp(line, "ROTOR COLD", 10) == 0 ) {
                Parm = atoi(&line[10]);
                if( strncmp(&line[11], "DISABLE", 7)==0 ) {
                    Cal.Set(ROTORPOWERCOLD, POWEROFF);
                } else if( strncmp(&line[11], "ENABLE", 6)==0 ) {
                    Cal.Set(ROTORPOWERCOLD, POWERON);
                } else if( Parm>5 ) {
                    Cal.Set(ROTORTEMPERATURECOLD, Parm);
                }
            } else if( strncmp(line, "ROTOR HOT", 9) == 0 ) {
                Parm = atoi(&line[9]);
                if( strncmp(&line[10], "DISABLE", 7)==0 ) {
                    Cal.Set(ROTORPOWERHOT, POWEROFF);
                } else if( strncmp(&line[10], "ENABLE", 6)==0 ) {
                    Cal.Set(ROTORPOWERHOT, POWERON);
                } else if( Parm>5 ) {
                    Cal.Set(ROTORTEMPERATUREHOT, Parm);
                }

            } else if( strncmp(line, "EXTERNALBB2", 11) == 0 ) {    // Do before EXTERNALBB
                Parm = atoi(&line[11]);
                BBExternal2.Set(Parm);

            } else if( strncmp(line, "EXTERNALBB", 10) == 0 ) {
                Parm = atoi(&line[10]);
                BBExternal.Set(Parm);


            } else if( strncmp(line, "WAITBB2", 7) == 0 ) {    // Do before WAITBB
                COSMOS_USLEEP(5000000);
                while(!BBExternal2.IsStable()) {
                    COSMOS_USLEEP(1000000);
                }

            } else if( strncmp(line, "WAITBB", 6) == 0 ) {
                COSMOS_USLEEP(5000000);
                while(!BBExternal.IsStable()) {
                    COSMOS_USLEEP(1000000);
                }


            } else if( strncmp(line, "ROTATION", 8) == 0 ) {
                //Channel = 0;
                sscanf(line, "%*s %f %f" , &PositionRotation, &SpeedRotation);
                SlowSpeedRotation = SpeedRotation;
                Motion.Set(SpeedRotation, RotationHome+PositionRotation, 0);
            } else if( strncmp(line, "GONIOMETER", 10) == 0 ) {
                //Channel = 1;
                sscanf(line, "%*s %f %f" , &PositionGoniometer, &SpeedGoniometer);
                SlowSpeedGoniometer = SpeedGoniometer;
                Motion.Set(SpeedGoniometer, GoniometerHome+PositionGoniometer, 1);
            } else if( strncmp(line, "FFC", 3) == 0 ) {
#ifdef __unix__
                // FFC Camera
                switch(CameraType) {
                case MC320:
                    break;
                case TAU640:
                    TauCommand.DoFFC();
                    break;
                case PV640:
                    break;
                }
#endif

            } else if( strncmp(line, "MSLEEP", 6) == 0 ) {
                Parm = atoi(&line[6]);
                COSMOS_USLEEP(Parm*1000);
            } else if( strncmp(line, "SLEEP", 5) == 0 ) {
                Parm = atoi(&line[5]);
                COSMOS_USLEEP(Parm*1000000);
            } else if( strncmp(line, "END", 3) == 0 ) {
                //ScriptMessageDone = true;
                //FlushLog();
                break;

            } else if( strncmp(line, "DIRECTO", 7) == 0 ) {
                strPosition1 = strstr(line, "\n");
                if(strPosition1 != NULL) *strPosition1=0;

                strcpy( ScriptDirectory, &line[10] );


                //} else if( strncmp(line, "WAVECAL", 7) == 0 ) {

                //} else if( strncmp(line, "BBHOT", 5) == 0 ) {

                //} else if( strncmp(line, "BBCOLD", 6) == 0 ) {

            } else if( strncmp(line, "PROCESS", 7) == 0 ) {
                // Make NewProc.txt file
                if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
                sprintf(FilenameProc, "%snewproc.txt" , DataDirectory);
                fpProc = fopen(FilenameProc, "wt");

                //strcpy(ParmsList, "0 73 460 0 1 0 .80 ; nwavefiles startsamp endsamp leftflag constoffset xoff yof\n");
                //strcpy(Wavecal, "/home/winsys/source/data/wavecal_15mrad/wave_73_460.txt\n");
                strcpy(ParmsList, "0 76 420 0 1 0 .80 ; nwavefiles startsamp endsamp leftflag constoffset xoff yof\n");
                strcpy(Wavecal, "/home/winsys/data/wavecal_15mrad/wave_76_420.txt\n");

                sprintf(buffer, "%s\n" , DataDirectory);
                int retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc );
                retval = fwrite( ParmsList, sizeof(char), strlen(ParmsList), fpProc );
                retval = fwrite( Wavecal, sizeof(char), strlen(Wavecal), fpProc );


                //                    sprintf(buffer, "%s,%f\n" , HotFile, HotTemperature);
                //                    retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc );
                //                    sprintf(buffer, "%s,%f\n" , ColdFile, ColdTemperature);
                //                    retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc);

                sprintf(buffer, "%s,%f\n" , "TIR-160318180946-bb80-scan.bsq", 80.0);
                retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc );
                sprintf(buffer, "%s,%f\n" , "TIR-160318181127-bb60-scan.bsq", 60.0);
                retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc);

                sprintf(buffer, "%s\n" , ScanFile);
                retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc );
                sprintf(buffer, "%soutput\n" , DataDirectory);
                retval = fwrite( buffer, sizeof(char), strlen(buffer), fpProc );

                fclose(fpProc);


                // Call the process routine
                if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
                sprintf(FilenameProc, "%snewproc.txt" , DataDirectory);

#ifdef __unix__
                mainProc->readProcessFile (FilenameProc) ;
                mainProc->start () ;
#endif

            } else if( strncmp(line, "ACQUIRE", 7) == 0 ) {

                //:PLog((char*)"Acquire Data in Script");
                //strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength


                /*                    strPosition1 = strstr(&line[7], " ");
                    if(strPosition1 != NULL) {
                        *strPosition1=0;
                        strPosition1++;
                    }
                    while(*strPosition1==20)
                        strPosition1++;

                    strPosition2 = strstr(strPosition1, " ");
                    if(strPosition2 != NULL) {
                        *strPosition2=0;
                        strPosition2++;
                    }
                    while(*strPosition2==20)
                        strPosition2++;


                    strcpy( ShortComment, strPosition1 );
                    Parm = atoi(strPosition2);
                    NumFrames = Parm;
*/

                // Clear
                *ShortComment = 0;
                NumFrames = 0;
                *strColdOrHot = 0;

                sscanf(line, "%*s %s %d %s" , &ShortComment, &NumFrames, strColdOrHot);


                strcpy( DataDirectory,ScriptDirectory );   // Get the Data Directory
                //if( DataDirectory[strlen(DataDirectory)-1] != '\' ) strcat(DataDirectory, "\\");
                //MakeDirectoryPath( DataDirectory );


                AcqBusy = true;
                //enableButtons(false);
                //DoAcquireManual();

                // Filenmame????
                //strcpy( FileExtension, ".bsq" );
                //sprintf( Filename, "TIR-%s-%s-scan", DateTimeStamp, ShortComment );
                //sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );



                char DateTimeStamp[2048];
                char Filename[2048];
                char LongComment[512];
                char FilenameWithExtension[2064];
                char FilenameWithExtensionHot[8]={0};
                char FilenameWithExtensionCold[8]={0};
                char FilenameWithExtensionScene[8]={0};
                char FileExtension[8];
                //char ScanDirection[4];

                // Reset Abort Flag
                Abort = false;

                strcpy( LongComment, "Tircis manual acquire on Linux System" );
                //strcpy( ScanDirection, "M" );
                strcpy( FileExtension, ".bsq" );

                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                //MakeFileName
                sprintf( Filename, "TIR-%s-%s-scan", DateTimeStamp, ShortComment );
                sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

                if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
                MakeDirectoryPath( DataDirectory );
                sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


                // *************  Save Header  *************
                MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                            FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

                // Start Saving
#ifdef __unix__
                sem_post (&RunManualAcq_count);
#else
                SetEvent (RunManualAcq_count);
#endif



                // Make uppercase
                strupper(strColdOrHot);

                if(strncmp(strColdOrHot, "COLD", 4) == 0) {
                    Cal.Get(&PresentTemperaturePaddleCold, &PresentTemperaturePaddleHot, &PresentTemperatureRotorCold, &PresentTemperatureRotorHot, (float *)TemperatureSensor);
                    ColdTemperature = PresentTemperaturePaddleCold;
                    strcpy(ColdFile, FilenameWithExtension);
                }
                else if(strncmp(strColdOrHot, "HOT", 3) == 0) {
                    Cal.Get(&PresentTemperaturePaddleCold, &PresentTemperaturePaddleHot, &PresentTemperatureRotorCold, &PresentTemperatureRotorHot, (float *)TemperatureSensor);
                    HotTemperature = PresentTemperaturePaddleHot;
                    strcpy(HotFile, FilenameWithExtension);
                } else {
                    strcpy(ScanFile, FilenameWithExtension);
                }
                /*if(strncmp(strColdOrHot, "COLDROTOR", 9) == 0) {
                        Cal.Get(&PresentTemperaturePaddleCold, &PresentTemperaturePaddleHot, &PresentTemperatureRotorCold, &PresentTemperatureRotorHot, (float *)TemperatureSensor);
                        ColdTemperature = PresentTemperatureRotorCold;
                    }
                    if(strncmp(strColdOrHot, "HOTROTOR", 8) == 0) {
                        Cal.Get(&PresentTemperaturePaddleCold, &PresentTemperaturePaddleHot, &PresentTemperatureRotorCold, &PresentTemperatureRotorHot, (float *)TemperatureSensor);
                        ColdTemperature = PresentTemperatureRotorHot;
                    }*/
            }
        }
        *line = 0;

        fclose(fp);

        // Notify user script is done
        ScriptMessageDone = true;
        FlushLog();
    }


#ifdef QWERTY
    // Turn on BB's
    Cal.Set(PADDLETEMPERATURECOLD, DesiredTemperaturePaddleCold);
    Cal.Set(PADDLETEMPERATUREHOT, DesiredTemperatureRotorCold);

    // Get number of frames
    NumCalculatedFrames = (fabs(StopPositionRotation-StartPositionRotation))*(float)FrameRate/SlowSpeedRotation;


    // Do a FFC (Flat Field Correction) before starting the scan.
    switch(CameraType) {
    case MC320:
        CameraMc320.DoFFC();
        break;
    case TAU640:
        CameraTau.DoFFC();
        break;
    case PV640:
        CameraPv640.DoFFC();
        break;
    }
    // *****************  Stow Paddles  *****************
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
    COSMOS_USLEEP(1000000);



    // ********  Make Directory for Stare set  *********
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%4d%02d%02d_%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");     // Is this already done??
    CreateFilePathString(FullPath, DataDirectory, DateTimeStamp, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
    MakeDirectoryPath( FullPath/*, DateStamp, DataDirectory, DateTimeStamp, bUseDirectoryStructure, ShortComment, bSkipBlackBodies*/ );



    // ************  Make Scene File Name  *************
    t = time(NULL);
    tm = *localtime(&t);
    sprintf(TimeStamp, "%02d%02d%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf( FilenameScene, "LWIR_%s", TimeStamp);
    sprintf( FilenameWithExtensionScene, "%s%s", FilenameScene, FileExtension );
    sprintf( FilePath, "%s%s%s", FullPath, FilenameScene, FileExtension );

    // *****************  Stare Scene  *****************
    if( AcquireWithMotion(SlowSpeedRotation, StartPositionRotation, StopPositionRotation)<0 ) goto Abort2;
    if(Abort) goto Abort2;


    // ***********  Make Cold BB File Name  ************
    Cal.Get(&TemperaturePaddleCold, &TemperaturePaddleHot,
            &TemperatureRotorCold, &TemperatureRotorHot, TemperatureSensor);
    sprintf( FilenameCold, "LWIR_%s_BB_COLD%02d", TimeStamp, (int)(PresentTemperatureCold+0.5) );
    sprintf( FilenameWithExtensionCold, "%s%s", FilenameCold, FileExtension );
    sprintf( FilePath, "%s%s%s", FullPath, FilenameCold, FileExtension );

    // *****************  Activate Cold Paddle  *****************
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEON);

    // ****************  Stare Cold BB  ****************
    AcquireNoMotion();
    if(Abort) goto Abort2;


    // ************  Make Hot BB File Name  ************
    t = time(NULL);
    tm = *localtime(&t);
    sprintf(DateTimeStamp, "%4d%02d%02d_%02d%02d%02d", tm.tm_year-100+2000, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    Cal.Get(&TemperaturePaddleCold, &TemperaturePaddleHot,
            &TemperatureRotorCold, &TemperatureRotorHot, TemperatureSensor);

    sprintf( FilenameHot, "LWIR_%s_BB_HOT%02d", TimeStamp, (int)(PresentTemperatureHot+0.5) );
    sprintf( FilenameWithExtensionHot, "%s%s", FilenameHot, FileExtension );
    sprintf( FilePath, "%s%s%s", FullPath, FilenameHot, FileExtension );



    // *****************  Stow Cold Paddle  *****************
    Cal.Set(PADDLEPOSITIONCOLD, PADDLEOFF);
    COSMOS_USLEEP(1000000);


    // *****************  Activate Hot Paddle  *****************
    Cal.Set(PADDLEPOSITIONHOT, PADDLEON);

    // ****************  Stare Hot BB  *****************
    AcquireNoMotion();
    if(Abort) goto Abort2;


    // *****************  Stow Hot Paddle  *****************
    Cal.Set(PADDLEPOSITIONHOT, PADDLEOFF);
    COSMOS_USLEEP(1000000);



    // *************  Save Cold BB Header  *************
    MakeHeader( FullPath, FilenameWithExtensionCold, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, 0, PresentTemperatureCold,
                FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );



    // **************  Save Hot BB Header  *************
    MakeHeader( FullPath, FilenameWithExtensionHot,  NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, PresentTemperatureHot, 0,
                FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


    // Get number of frames
    NumCalculatedFrames = (fabs(StopPositionRotation-StartPositionRotation))*(float)FrameRate/SlowSpeedRotation;

    // **************  Save Scene Header  **************
    MakeHeader( FullPath, FilenameWithExtensionScene,NumCalculatedFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, 0, PresentTemperatureHot, PresentTemperatureCold,
                FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


    // *************  Done, move to Home  **************
    Motion.Set(m_nFastSpeedRotation, 0, 0);

    AcqBusy = false;
}
#endif
return(NULL);
}


// *****************************************************************
// Do FFC
// *****************************************************************
#ifdef __unix__
void* runFfc(void *args)
#else
DWORD WINAPI runFfc(LPVOID pParam)
#endif
{
    while(!ExitProgram) {
        //AbortFFC:
        // ***************************************************************************************************
        // Wait for do FFC message.
        // ***************************************************************************************************
#ifdef __unix__
        sem_wait (&RunFfc_count);
#else
        WaitForSingleObject (RunFfc_count, INFINITE);
#endif
        if(ExitProgram) break;

        //Clear Background
        memset(m_nPhotonBackgroundBuffer, 0, MAXIMAGESIZE*sizeof(unsigned short));

        //Cal.Set(ROTORPOSITION, ROTORAMBIENT);
        Sleep(15000);   // Wait 15 seconds for wheel to finish moving

        memcpy(m_nPhotonBackgroundBuffer, m_nPhotonFrameBuffer, IMAGESIZE*sizeof(unsigned short));


        // Save dark image file.
/*        strcpy( DataDirectory, ui->lineEditDataDirectoryBench->text().toAscii() );   // Get the Data Directory from the lineedit
        strcpy( Wavelength, ui->lineEditWavelengthBench->text().toAscii() );         //Get Wavelength
        strcpy( ShortComment, Dark );     // Get the Short Comment

        NumFrames = ui->lineEditNumFramesBench->text().toFloat();

        AcqBusy = true;
        enableButtons(false);*/
        DoAcquireManual();



        //Cal.Set(ROTORPOSITION, ROTORPASS);
        FfcDone = true;
    }
    return(NULL);
}
