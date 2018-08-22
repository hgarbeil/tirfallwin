#ifdef _WIN32
#include "afxwin.h"
#endif
//:O#include "cssl_lib.h"

#include <QMutex>
#include <QWaitCondition>

#ifdef __unix__
#include <unistd.h> //for COSMOS_USLEEP
#include <sys/time.h>   /* for settimeofday() */
#include <pthread.h>
#include <semaphore.h>
#include "acqmc320.h"
#include "acqtau.h"
#include "acqtauserial.h"
#include "acqpv640.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>       /* for mktime()       */
#include <math.h>

#include <QMessageBox>
#include <QtGui>
#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "defines.h"
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
#include "acqPhoton.h"
#include "acqino.h"
//#include "deviceFctns.h"
#include "threads.h"
#include "fileFctns.h"
#include "COMDevice.h"
#include "global.h"
#include "configCosmos.h"
#include "tprocess.h"

//#define HOTFIRST 0
//#define COLDFIRST 1

int AcquireWithMotion( float SlowSpeedRotationL, float StartPositionRotationL, float StopPositionRotationL );


#ifdef __unix__
extern TauSerial TauCommand;
#endif
bool DoingFullRange = false;
//bool DoingStepAutofocus = false;

//bool HotOrCold = HOTFIRST;  // This is set to !HotOrCold at start of function.


//extern char UchisVersion[256];
//char SaveDirectoryArray[2048];
extern int CameraType;

char DateTimeStampM[2048];
char FilenameWithExtensionM[2064];
//extern int FrameCount;
extern int NumFramesSaved;

/*extern QMutex mutex;
extern QWaitCondition keyPressed;*/

#ifdef PIGS
// *****************************************************************
// Set the black body temperature
// *****************************************************************
void* runBbUnit1(void *)
{
    char comStringIn[256];    //, buffer[256];
    char comStringOut[256];    //, buffer[256];
    int nBytes=255;


    // Keep checking to see if the Bb is open
    while( !BbPortOpen && !ExitProgram ) {
        COSMOS_USLEEP(1000000);
    }


    while(!ExitProgram) {

        sem_wait (&ChangeTemperatureA_count);
        if(ExitProgram) break;

        if(BbPortOpen) {

            // Wait if BB monitor is getting the temperature (put a timeout here)
            while(BbBusy) {
                COSMOS_USLEEP(100000);
            }
            BbBusy = true;


            // Get the temperature since it is waiting.
            memset (comStringIn, 0, 40);
            cssl_getdata(SerialBb, (uint8_t*)comStringIn, nBytes);

            /*//Print the temperature to the main window since we have it.
            char *NewLinePos = (strstr(comString, "\r"));
            if(NewLinePos!=NULL) {
                *NewLinePos = (char)0;
            }
            PresentTemperatureBb1 = atof( comString );
            //ui->lineEditTemperatureBbUnit1->setText(comString); //(have not used the temperature value)*/


            // Get the desired temperature from the line edit.
            //strcpy( buffer, ui->lineEditDesiredTemperature->text().toAscii() );
            //DesiredTemperature = atof(buffer);

            // Set the temperature.
            if(DesiredTemperatureA < 0) {
                sprintf (comStringOut, "TEC:T -%.1f;TEC:OUT 1\r\n", fabs(DesiredTemperatureA) );
            }
            else {
                sprintf (comStringOut, "TEC:T +%.1f;TEC:OUT 1\r\n", DesiredTemperatureA);
            }
            cssl_putstring(SerialBb,comStringOut);


            //Send command to get temperature (So it will be ready for next request.)
            sprintf (comStringOut, "TEC:T?\r\n");
            cssl_putstring(SerialBb,comStringOut);

            COSMOS_USLEEP(200000);
            BbBusy = false;
        }
    }

    return(NULL);
}
#endif

// *****************************************************************
// Set the position
// *****************************************************************
#ifdef __unix__
void* runRotationA(void *)
#else
DWORD WINAPI runRotationA(LPVOID pParam)
#endif
{
    //char comString[256];    //, buffer[256];
    //char bufferPos[256];
    //int length;
    //, nBytes=255;
    /*

    // Keep checking the slide if it is not open.
    while( !RotationAPortOpen && !ExitProgram) {
        COSMOS_USLEEP(1000000);
    }


    while(!ExitProgram) {

        sem_wait (&MovePosition_count);
        if(ExitProgram) break;

        if(RotationAPortOpen) {

            // Wait if position monitor is getting the position (put a timeout here)
            while(RotationABusy) {
                COSMOS_USLEEP(100000);
            }
            RotationABusy = true;

            //COSMOS_USLEEP(500000);

            // Get the position since it is waiting.
            memset (comString, 0, 40);
            cssl_getdata(SerialRotationA, (uint8_t*)comString, nBytes);

            //Print the position to the main window.
            if( strlen(comString)>1 ) {
                strcpy(bufferPos, &comString[3]);
                length = strlen( bufferPos ) - 2;
                bufferPos[length] = '\0';
                PresentPosition = atof(bufferPos);	//SMC100
            }
            else {
                PresentPosition = 9999;
            }
            //ui->lineEditPresentPositionABench->setText(bufferPos); //(have not used the position value)

            //COSMOS_USLEEP(2000000);

            // Set the speed.
            sprintf (comString, "1VA%.1f\r\n", DesiredSpeed );	//SMC100
            cssl_putstring(SerialRotationA, comString);


            //Move to position.
            sprintf (comString, "1PA%.1f\r\n", DesiredPosition );	//SMC100 angle
            cssl_putstring(SerialRotationA, comString);

            / *COSMOS_USLEEP(200000);
            // Read reply
            nBytes = 1;
            cssl_getdata(SerialRotationA, (uint8_t*)comString, nBytes);
            * /

            //Send command to get RotationA Position  (So it will be ready for next request.)
            strcpy(comString, "1TP\r\n");	//SMC100 get position string
            cssl_putstring(SerialRotationA,comString);


            COSMOS_USLEEP(200000);
            RotationABusy = false;

        }
    }
*/
    return(NULL);
}


// *****************************************************************
// Acquire frames at position
// *****************************************************************
void AcquireAtPosition(float Position)
{
    // Move to BlackBody
    DesiredSpeedRotation = m_nFastSpeedRotation;
    DesiredPositionRotation = Position;
#ifdef __unix__
    sem_post (&MovePosition_count);
#else
    SetEvent (MovePosition_count);
#endif
    // Wait till in position
    while( fabs(PresentPositionRotation-DesiredPositionRotation) > .1 )
        COSMOS_USLEEP(100000);


    NumFrames = NumCalFrames;
#ifdef __unix__
    // Start acquire
    sem_post (&SaveStart_count);
    // Wait till done
    sem_wait (&AcquireFinished_count, INFINITE);
#else
    // Start acquire
    SetEvent (&SaveStart_count);
    // Wait till done
    WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif
}

#ifdef NOTUSED
// *****************************************************************
// Do a stare or scan on one component (item)
// *****************************************************************
void AcquireWithMotion(void)
{
    // This moves the stage while collecting data.  Is used for staring or scanning/rotating
    // Add timeouts!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Move to Start
    DesiredSpeedRotation = m_nFastSpeedRotation;
    DesiredPositionRotation = StartPositionRotation - RotationHome;
#ifdef __unix__
    sem_post (&MovePosition_count);
#else
    SetEvent (MovePosition_count);
#endif
    // Wait till in position
    while( fabs(PresentPositionRotation-DesiredPositionRotation) > .1 )
        COSMOS_USLEEP(200000);

    // Start Motion (Move to End)
    DesiredSpeedRotation = SlowSpeedRotation;
    DesiredPositionRotation = StopPositionRotation - RotationHome;

#ifdef __unix__
    sem_post (&MovePosition_count);
    NumFrames = NumTargetFrames;
    // Start acquire
    sem_post (&SaveStart_count);
    // Wait till done
    sem_wait (&AcquireFinished_count);
#else
    SetEvent (MovePosition_count);
    NumFrames = NumTargetFrames;
    // Start acquire
    SetEvent (SaveStart_count);
    // Wait till done
    WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif

    if(ExitProgram) return;

    while( fabs(PresentPositionRotation-DesiredPositionRotation) > .1 )
        COSMOS_USLEEP(200000);
}
#endif

// *****************************************************************
// Acquire CAL frames without moving anything (for WaveCal and BB's)
// *****************************************************************
#ifdef __unix__
void *runCalAcq(void *args)
#else
DWORD WINAPI runCalAcq(LPVOID pParam)
#endif
{
    // ***** Allow access to UI.
    //    Ui::MainWindow *ui;
    //    ui = (Ui::MainWindow*)args; // Let this "friend function" access ImgData

    while(!ExitProgram) {
        //TircisStatus = IDLE;  // The progress bar takes care of this, so we don't need to delay.

        //Wait for scan message.
#ifdef __unix__
        sem_wait (&RunCalAcq_count);
#else
        WaitForSingleObject (RunCalAcq_count, INFINITE);
#endif
        if(ExitProgram) break;

#ifdef __unix__
        // Start acquire
        sem_post (&SaveStart_count);
        // Wait till done
        sem_wait (&AcquireFinished_count);
#else
        // Start acquire
        SetEvent (SaveStart_count);
        // Wait till done
        WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif

        //TircisStatus = IDLETASK;
        //COSMOS_COSMOS_USLEEP(1000000); // Delay a second so the progress bar can finish
        AcqBusy = false;
    }
    return(NULL);
}


// This routine is identical to the above routine, if they dont change, remove one (runCalAcq).

// *****************************************************************
// Acquire frames without moving anything (for manual acquire)
// *****************************************************************
#ifdef __unix__
void *runManualAcq(void *args)
#else
DWORD WINAPI runManualAcq(LPVOID pParam)
#endif
{
    char FileExtension[8];
    char LongComment[512];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};

    strcpy( FileExtension, ".bsq" );
    strcpy( LongComment, "Tircis manual acquire on Windows System" );



    // ***** Allow access to UI.
    //    Ui::MainWindow *ui;
    //    ui = (Ui::MainWindow*)args; // Let this "friend function" access ImgData

    while(!ExitProgram) {
        //TircisStatus = IDLE;  // The progress bar takes care of this, so we don't need to delay.

        //Wait for scan message.
#ifdef __unix__
        sem_wait (&RunManualAcq_count);
#else
        WaitForSingleObject (RunManualAcq_count, INFINITE);
#endif

        TircisStatus = MANUALACQUIRE;   // To tell which progress bar to use
        if(ExitProgram) break;

#ifdef __unix__
        // Start acquire
        sem_post (&SaveStart_count);
        // Wait till done
        sem_wait (&AcquireFinished_count);
#else
        // Start acquire
        SetEvent (SaveStart_count);
        /*mutex.lock();
        keyPressed.wakeAll();
        mutex.unlock();*/

        // Wait till done
        WaitForSingleObject (AcquireFinished_count, INFINITE);
#endif

        // *************  Save Header  *************
        MakeHeader( DataDirectory, FilenameWithExtensionM, NumFramesSaved, DateTimeStampM, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                    FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


        //TircisStatus = IDLETASK;
        //COSMOS_COSMOS_USLEEP(1000000); // Delay a second so the progress bar can finish
        AcqBusy = false;
    }
    return(NULL);
}

// *****************************************************************
// Do a complete scan (cold, scan scene, hot)
// *****************************************************************
#ifdef __unix__
void* runScan(void *args)   // RotationA mode
#else
DWORD WINAPI runScan(LPVOID pParam)
#endif
{
    char FullPath[2048];
    char DateTimeStamp[2048];
    char FilenameCold[2048];
    char FilenameScene[2048];
    char FilenameHot[2048];
    char LongComment[2048];
    //char ScanDirection[8];
    char FileExtension[16];
    char FilenameWithExtensionCold[2064];
    char FilenameWithExtensionScene[2064];
    char FilenameWithExtensionHot[2064];
    float PresentTemperatureCold;
    float PresentTemperatureScene;
    float PresentTemperatureHot;

    //char ProcDirWithFileNum[1024];
    //char ProcCommand[512];

    // ***** Allow access to UI.
    Ui::MainWindow *ui;
#ifdef __unix__
    ui = (Ui::MainWindow*)args; // Let this "friend function" access ImgData
#else
    ui = (Ui::MainWindow*)NULL;
#endif
    //strcpy(ScanDirection, "S");
    strcpy(FileExtension, ".bsq");
    strcpy(LongComment, "Tircis Scan on Linux System");


    while(!ExitProgram) {
Abort1:

        AcqBusy = false;

        //Wait for scan message.
#ifdef __unix__
        sem_wait (&RunScan_count);
#else
        WaitForSingleObject (RunScan_count, INFINITE);
#endif

        if(ExitProgram) break;


        // Set flags
        //TircisStatus = SCANTASK;
        //HotOrCold = !HotOrCold;


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
#else
        switch(CameraType) {
        case PHOTON:
            CameraPhoton.DoFFC();
            COSMOS_USLEEP(3000000);
            break;
        case INO:
            break;
        }
#endif

        // ********  Make Directory for Scan set  *********
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        CreateFilePathString(FullPath, DataDirectory, DateTimeStamp, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
        MakeDirectoryPath( FullPath/*, DateStamp, DataDirectory, DateTimeStamp, bUseDirectoryStructure, ShortComment, bSkipBlackBodies*/ );
        //strcpy( SaveDirectoryArray, FullPath );

#ifdef NOBBS
        // ***********  Make ColdOrHot BB File Name  ************
        //if(HotOrCold == COLDFIRST) {
        sprintf( FilenameCold, "TIR-%s-BB_COLD%02d", DateTimeStamp, (int)(PresentTemperaturePaddleCold+0.5) );
        sprintf( FilenameWithExtensionCold, "%s%s", FilenameCold, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameCold, FileExtension );
        PresentTemperatureCold = PresentTemperaturePaddleCold;
        /*} else {
            sprintf( FilenameHot, "TIR-%s-BB_HOT%02d", DateTimeStamp, (int)(PresentTemperaturePaddleCold+0.5) );
            sprintf( FilenameWithExtensionHot, "%s%s", FilenameHot, FileExtension );
            sprintf( FilePath, "%s%s%s", FullPath, FilenameHot, FileExtension );
            PresentTemperatureHot = PresentTemperatureRotorCold;
        }*/

        // *****************  Get Cold BB  *****************
        AcquireAtPosition(BBRotationPosition);


        if(Abort) goto Abort1;
#endif


        // ***********  Set BB to Hot for next BB **********
        //DesiredTemperatureA = DesiredTemperatureAHot;
        //sem_post (&ChangeTemperatureA_count);



        // ************  Make Scene File Name  *************
        t = time(NULL);
        tm = *localtime(&t);
        sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        sprintf( FilenameScene, "TIR-%s-%s-scan", DateTimeStamp, ShortComment);
        sprintf( FilenameWithExtensionScene, "%s%s", FilenameScene, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameScene, FileExtension );

        // *****************  Scan Scene  *****************
        PresentTemperatureScene = PresentTemperatureRotorCold;
        AcquireWithMotion(ScanStageSlowSpeed, ScanStageStartPosition, ScanStageStopPosition);

        if(Abort) goto Abort1;


#ifdef NOBBS
        // ************  Make ColdOrHot BB File Name  ************
        t = time(NULL);
        tm = *localtime(&t);
        sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        //if(HotOrCold == COLDFIRST) {
        sprintf( FilenameHot, "TIR-%s-BB_HOT%02d", DateTimeStamp, (int)(PresentTemperaturePaddleCold+0.5) );
        sprintf( FilenameWithExtensionHot, "%s%s", FilenameHot, FileExtension );
        sprintf( FilePath, "%s%s%s", FullPath, FilenameHot, FileExtension );
        PresentTemperatureHot = PresentTemperatureRotorCold;
        /*} else {
            sprintf( FilenameCold, "TIR-%s-BB_COLD%02d", DateTimeStamp, (int)(PresentTemperatureRotorCold+0.5) );
            sprintf( FilenameWithExtensionCold, "%s%s", FilenameCold, FileExtension );
            sprintf( FilePath, "%s%s%s", FullPath, FilenameCold, FileExtension );
            PresentTemperatureCold = PresentTemperaturePaddleCold;
        }*/

        // ****************  Take Hot BB  *****************
        AcquireAtPosition(BBRotationPosition);


        if(Abort) goto Abort1;


        // *************  Save Cold BB Header  *************
        MakeHeader( FullPath, FilenameWithExtensionCold, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureCold, 0, 0,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

        // **************  Save Hot BB Header  *************
        MakeHeader( FullPath, FilenameWithExtensionHot,  NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureHot, 0, 0,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


        // *****  Copy files to processing directory  *****
        //sprintf( ProcDirWithFileNum, "%s/%08d", ProcessingDirectory, FileNum);
        //CopyToProcessingDir( FullPath, ProcessingDirectory, FilenameWithExtensionCold, FilenameWithExtensionScene, FilenameWithExtensionHot, FilenameCold, FilenameScene, FilenameHot);
#endif

        // **************  Save Scene Header  ************** (CopyToProcessingDir gets WaveCalFileName)
        MakeHeader( FullPath, FilenameWithExtensionScene,NumTargetFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureScene, 0, 0,
                    FileExtension, SlowSpeedRotation, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );



        // *************  Done, move to Home  **************
        DesiredSpeedRotation = m_nFastSpeedRotation;
        DesiredPositionRotation = StartPositionRotation + (StopPositionRotation-StartPositionRotation)/2.0 - RotationHome;
        //DesiredPosition = -HomePosition +StartPosition + (StopPosition-StartPosition)/2.0;
#ifdef __unix__
        sem_post (&MovePosition_count);
#else
        SetEvent (MovePosition_count);
#endif


#ifdef __unix__
        // Call Mike's Process function (while moving to position)
        //TircisStatus = PROCESSINGTASK;

        //strcpy(FilenameWithExtensionScene, "TIR-140130141825-recon-S-scan.bsq");

        // FileNum
        sprintf(ProcCommand, "%s%s %s%s" , DataDirectory, "ProcessDummy", FullPath, FilenameWithExtensionScene);
        system(ProcCommand);

        //creatre QProcess
        // Process StartDetached
        // Wait for signal
        wait();
        //TircisStatus = IDLETASK;
#endif


        // Wait till in position
        while( fabs(PresentPositionRotation-DesiredPositionRotation) > .1 ) {
            COSMOS_USLEEP(100000);
            if(Abort) goto Abort1;
        }
    }
    return(NULL);
}


void DoAcquireManual(void)
{
    //char SpecificPath[2048];
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

    strcpy( LongComment, "Tircis manual acquire on Windows System" );
    //strcpy( ScanDirection, "M" );
    strcpy( FileExtension, ".bsq" );

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //CreateFilePathString(SpecificPath, /*DateStampScan,*/ DataDirectory, DateTime4Dir, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
    //MakeDirectoryPath( SpecificPath, DateStamp, DataDirectory, DateTime4Dir, bUseDirectoryStructure, ShortComment, bSkipBlackBodies );
    //strcpy( SaveDirectoryArray, SpecificPath );

    //MakeFileName
    sprintf( Filename, "TIR-%s-%s-scan", DateTimeStamp, ShortComment );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    MakeDirectoryPath( DataDirectory );
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );
    qDebug ()<< "Output file is " << FilePath  ;

//    // *************  Save Header  *************
//    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
//                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );


    strcpy(FilenameWithExtensionM, FilenameWithExtension);
    strcpy(DateTimeStampM, DateTimeStamp);

    // Start Saving
#ifdef __unix__
    sem_post (&RunManualAcq_count);
#else
    SetEvent (RunManualAcq_count);
#endif
}

void DoAcquireManual(TProcess *TProc)
{
    //char SpecificPath[2048];
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

    strcpy( LongComment, "Tircis manual acquire on Windows System" );
    //strcpy( ScanDirection, "M" );
    strcpy( FileExtension, ".bsq" );

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //CreateFilePathString(SpecificPath, /*DateStampScan,*/ DataDirectory, DateTime4Dir, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
    //MakeDirectoryPath( SpecificPath, DateStamp, DataDirectory, DateTime4Dir, bUseDirectoryStructure, ShortComment, bSkipBlackBodies );
    //strcpy( SaveDirectoryArray, SpecificPath );

    //MakeFileName
    sprintf( Filename, "TIR-%s-%s-scan", DateTimeStamp, ShortComment );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '/' ) strcat(DataDirectory, "/");
    MakeDirectoryPath( DataDirectory );
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );
    TProc->scanFile =FilePath ;
    qDebug ()<< "Output file is " << FilePath  ;

//    // *************  Save Header  *************
//    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
//                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    strcpy(FilenameWithExtensionM, FilenameWithExtension);
    strcpy(DateTimeStampM, DateTimeStamp);

    // Start Saving
#ifdef __unix__
    sem_post (&RunManualAcq_count);
#else
    SetEvent (RunManualAcq_count);
#endif
}

void DoAcquireManualBB(float temp, bool hotFlag, TProcess *tp)
{
    //char SpecificPath[2048];
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

    strcpy( LongComment, "Tircis BB acquire" );
    //strcpy( ScanDirection, "M" );
    strcpy( FileExtension, ".bsq" );

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //CreateFilePathString(SpecificPath, /*DateStampScan,*/ DataDirectory, DateTime4Dir, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
    //MakeDirectoryPath( SpecificPath, DateStamp, DataDirectory, DateTime4Dir, bUseDirectoryStructure, ShortComment, bSkipBlackBodies );
    //strcpy( SaveDirectoryArray, SpecificPath );

    //MakeFileName
    sprintf( Filename, "TIR-%s-%s-scan", DateTimeStamp, ShortComment );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '/' ) strcat(DataDirectory, "/");
    MakeDirectoryPath( DataDirectory );
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );
    tp->setBlackBody (FilePath, temp, hotFlag);
    qDebug ()<< "Output file is " << FilePath  ;

    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start Saving
#ifdef __unix__
    sem_post (&RunManualAcq_count);
#else
    SetEvent (RunManualAcq_count);
#endif
}




// Is this routine the same as above???  Get rid of DoAcquieWaveCal (unless we get some automation going)

void DoAcquireWaveCal(void)
{
    //char SpecificPath[2048];
    char DateTimeStamp[2048];
    char Filename[2048];
    char LongComment[512]={0};
    char FilenameWithExtension[2064];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};
    char FileExtension[8];
    //char ScanDirection[4];

    // Set Flags
    TircisStatus = WAVECAL;
    Abort = false;

    strcpy( LongComment, "WaveCal on Linux System" );
    //strcpy( ScanDirection, " " );
    strcpy(FileExtension, ".bsq");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    //CreateFilePathString(SpecificPath, /*DateStamp,*/ DataDirectory/*, DateTime4Dir*/, bUseDirectoryStructure/*, ShortComment, bSkipBlackBodies*/);
    //MakeDirectoryPath( SpecificPath, DateStamp, DataDirectory/*, DateTime4Dir*/, bUseDirectoryStructure, ShortComment, bSkipBlackBodies );
    //strcpy( SaveDirectoryArray, SpecificPath );

    //MakeFileName
    sprintf( Filename, "TIR-%s-W_%s", DateTimeStamp, Wavelength );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start saving
#ifdef __unix__
    sem_post (&RunCalAcq_count);
#else
    SetEvent (RunCalAcq_count);
#endif
}

void DoAcquirePaddleCold(void )
{
    //char SpecificPath[2048];
    char DateTimeStamp[2048];
    char Filename[2048];
    char LongComment[512]={0};
    char FilenameWithExtension[2064];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};
    char FileExtension[8];
    //char ScanDirection[4];

    // Set Flags
    TircisStatus = BBCOLD;
    Abort = false;

    strcpy( LongComment, "Cold BB on Linux System" );
    //strcpy( ScanDirection, " " );
    strcpy(FileExtension, ".bsq");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //MakeFileName
    sprintf( Filename, "TIR-%s-BB_COLDF%02d", DateTimeStamp, (int)(PresentTemperaturePaddleCold+0.5) );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperaturePaddleCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start saving
#ifdef __unix__
    sem_post (&RunCalAcq_count);
#else
    SetEvent (RunCalAcq_count);
#endif
}

void DoAcquirePaddleHot(void )
{
    //char SpecificPath[2048];
    char DateTimeStamp[2048];
    char Filename[2048];
    char LongComment[512]={0};
    char FilenameWithExtension[2064];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};
    char FileExtension[8];
    //char ScanDirection[4];

    // Set Flags
    TircisStatus = BBCOLD;
    Abort = false;

    strcpy( LongComment, "Cold BB on Linux System" );
    //strcpy( ScanDirection, " " );
    strcpy(FileExtension, ".bsq");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //MakeFileName
    sprintf( Filename, "TIR-%s-BB_HOTF%02d", DateTimeStamp, (int)(PresentTemperaturePaddleCold+0.5) );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperaturePaddleCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start saving
#ifdef __unix__
    sem_post (&RunCalAcq_count);
#else
    SetEvent (RunCalAcq_count);
#endif
}

void DoAcquireRotorCold(void)
{
    //char SpecificPath[2048];
    char DateTimeStamp[2048];
    char Filename[2048];
    char LongComment[512]={0};
    char FilenameWithExtension[2064];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};
    char FileExtension[7];
    //char ScanDirection[4];

    // Set Flags
    TircisStatus = BBHOT;
    Abort = false;

    strcpy( LongComment, "Hot BB on Linux System" );
    //strcpy( ScanDirection, " " );
    strcpy(FileExtension, ".bsq");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //MakeFileName
    sprintf( Filename, "TIR-%s-BB_COLDR%02d", DateTimeStamp, (int)(PresentTemperatureRotorCold+0.5) );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start saving
#ifdef __unix__
    sem_post (&RunCalAcq_count);
#else
    SetEvent (RunCalAcq_count);
#endif
}

void DoAcquireRotorHot(void)
{
    //char SpecificPath[2048];
    char DateTimeStamp[2048];
    char Filename[2048];
    char LongComment[512]={0};
    char FilenameWithExtension[2064];
    char FilenameWithExtensionHot[8]={0};
    char FilenameWithExtensionCold[8]={0};
    char FilenameWithExtensionScene[8]={0};
    char FileExtension[7];
    //char ScanDirection[4];

    // Set Flags
    TircisStatus = BBHOT;
    Abort = false;

    strcpy( LongComment, "Hot BB on Linux System" );
    //strcpy( ScanDirection, " " );
    strcpy(FileExtension, ".bsq");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(DateTimeStamp, "%2d%02d%02d%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //MakeFileName
    sprintf( Filename, "TIR-%s-BB_HOTR%02d", DateTimeStamp, (int)(PresentTemperatureRotorCold+0.5) );
    sprintf( FilenameWithExtension, "%s%s", Filename, FileExtension );

    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) strcat(DataDirectory, "\\");
    sprintf( FilePath, "%s%s%s", DataDirectory, Filename, FileExtension );


    // *************  Save Header  *************
    MakeHeader( DataDirectory, FilenameWithExtension, NumFrames, DateTimeStamp, 1, FilenameWithExtensionHot, FilenameWithExtensionCold, WaveCalFilename, FilenameWithExtensionScene, PresentTemperatureRotorCold, 0, 0,
                FileExtension, 0, FrameRate, AcqImgWidth, AcqImgHeight, SensorType, Wavelength, LongComment );

    // Start saving
#ifdef __unix__
    sem_post (&RunCalAcq_count);
#else
    SetEvent (RunCalAcq_count);
#endif
}
