#ifdef _WIN32
#include <afxwin.h>
#endif
#include <QMainWindow>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>         // open
#include <time.h>
#include <sys/stat.h>      // stat
#include <sys/types.h>     // stat
//:)#include <sys/sendfile.h>  // sendfile
//#include <unistd.h> //for usleep
#include "defines.h"
#include "fileFctns.h"
#include "cmigitsasc.h"

#ifdef __unix__
#include <dirent.h>
#include <unistd.h> //for usleep
#endif
//:O#include "cssl_lib.h"


//extern float m_nIntermediateSpeed;
//extern float m_nFastSpeed;
//extern float TemperatureBb2;
//extern bool bUseBBUnit2;
//extern bool m_bAllPositionsFlagOn;
//extern int PresentTask;
//extern float PresentTemperatureBb2;
//extern char DateStampWave[64];
//extern char DateTimeStamp[64];
//extern bool bSkipBlackBodies;
//extern bool IntSavedCold;
//extern bool IntSavedHot;
//extern bool IntSavedOther;
//extern bool WaveCalSaved;

extern BOOL UseMotionStage;
extern char DataDirectory[4096];
extern char FilePath[2048];
extern char WaveCalFilename[2048];
extern char ShortComment[512];
extern bool bUseDirectoryStructure;
extern float PresentTemperatureBb1;
extern long FileNum;
extern int MotionPortNum;
extern int BbPortNum;
extern int Bb2PortNum;
extern int CalibratorPortNum;
extern int GumstixPortNum;
extern int FocusPortNum;
extern int GpsPortNum;

extern float PresentTemperatureBbExt;
extern float PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot;
extern bool PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient;
extern char PresentPositionRotor[64];
extern float CameraTemperature;
extern float SlowSpeedRotation, SlowSpeedGoniometer;
extern float PresentPositionRotation, PresentPositionGoniometer;
extern float RotationHome, GoniometerHome;
extern float ScanStageSlowSpeed, ScanStageStartPosition, ScanStageStopPosition;

extern CMigitsA Gps;

// Creates a header file on the disk
void MakeHeader( char *Directory, char *Filename, int NumFrames, char *DateTimeStamp, int BkitIndex, char *HotFile, char *ColdFile, char *WaveFile,
                 char *ScanFile, float Temperature, float TemperatureHot, float TemperatureCold, char *FileExtension,
                 float DesiredSpeed, int FrameRate, int ImageWidth, int ImageHeight, char *SensorType, char *Wavelength, char *buffer )
{
    char HeaderFilePath[2048];
    char HeaderString[2048L+1024L+512L+10000L+10000L];
    char CollectNotesComment[16] = {0};
    char LastTenCharsOfFilename[11];
    char DateStamp[16];
    int FileNameLength;
    int PosInStr;
    FILE* FH;


    if(strlen(Directory) > 150) {
        //MessageBox("Directory name is larger than 150 characters. Setting Directory name to \"/\"", "Error", MB_OK);
        strcpy(Directory, "\\");
    }


    if( Directory[strlen(Directory)-1] != '/' ) {
        strcat(Directory, "\\");
    }


    //strncpy_s(FirstFourCharsOfFilename, Filename, 4);
    FileNameLength = strlen(Filename);
    PosInStr = FileNameLength-10;
    strncpy(LastTenCharsOfFilename, &Filename[PosInStr], 10);
    LastTenCharsOfFilename[10] = 0;

    //strncpy(FirstTwoCharsOfFilename, Filename, 2);
    strncpy(DateStamp, DateTimeStamp, 6);
    DateStamp[6] = 0;
    //DateStamp[6] = 0;    // Null Terminate

    //Get the comment
    //GetDlgItemText( IDC_SHORTCOMMENT, ShortComment, 2048 );

    //Get the comment
    //GetDlgItemText( IDC_LONGCOMMENT, buffer, 2048 );
    //AddCR(buffer);	//Add CR's if longer than 50 chars

    //Get the comment
    //GetDlgItemText( IDC_COLLECTNOTES, CollectNotesComment, 2048 );
    //AddCR(CollectNotesComment);	//Add CR's if longer than 50 chars


    // Get target name
    //GetDlgItemText( IDC_TARGETNAME, TargetName, 2048 );
    // Get position name
    //GetDlgItemText( IDC_POSITIONNAME, PositionName, 2048 );


    //*** Write Header
    //strcpy(HeaderFilePath, FilePath);
    sprintf(HeaderFilePath, "%s%s", Directory, Filename);

    FileNameLength = strlen(HeaderFilePath);
    PosInStr = FileNameLength-4;
    HeaderFilePath[PosInStr] = 0;	// Chop off extension
    strcat(HeaderFilePath, ".hdr");

    if ((FH = fopen( HeaderFilePath, "wt" )) == NULL ) {
        QMessageBox::warning(0, "File Error", "Can't open file, Check that the data directory exists." );
        return;//sprintf(buf, "Error opening header file:\n%s" , HeaderFilePath);
        //AfxMessageBox(buf);
    }

    //Write the data
    //if( (strcmp(FirstFourCharsOfFilename, "SCAN")==0) || (strcmp(FirstTwoCharsOfFilename, "W_")==0) ) {
    char Last10[11];	// Added 2 to the extension
    sprintf(Last10, "scan%s", FileExtension);

    if( strcmp(LastTenCharsOfFilename, Last10)==0 ) {
        sprintf( HeaderString, "ENVI\r\n\
description = { \r\n\r\n\
Date =          %s\r\n\
File Name =     %s\r\n\r\n\
Collect Notes = %s\r\n\r\n\
Long Comment =  %s\r\n}\r\n\r\n\
; Scan rate\r\n\
Rotation Scan Rate =   %f\r\n\
Goniometer Scan Rate = %f\r\n\
; Camera Frequency\r\n\
FPS =              %d\r\n\
; Pixel Columns\r\n\
samples =          %d\r\n\
; Pixel Rows\r\n\
lines =            %d\r\n\
; Number of frames in image cube\r\n\
bands =            %d\r\n\
; Number of bytes of embeded header information\r\n\
header offset =    0\r\n\
; File type recognized by ENVI\r\n\
file type =        ENVI Standard\r\n\
; Data representation where 12 = 16-bit unsigned integer\r\n\
data type =        12\r\n\
; Band Sequential (BSQ), Band Interleaved by Pixel (BIP), Band Interleaved by Line (BIL)\r\n\
interleave =       BSQ\r\n\
; Name of HSI and camera system\r\n\
sensor type =      %s\r\n\
; Order of bytes, 0=LSF (DEC and MS-DOS systems), 1=MSF (SUN, SGI, IBM, HP, DG, etc.)\r\n\
byte order =       0\r\n\
Cal Wavelength =          %s\r\n\
Cold Paddle Temperature = %f\r\n\
Hot Paddle Temperature =  %f\r\n\
Cold Rotor Temperature =  %f\r\n\
Hot Rotor Temperature =   %f\r\n\
External BB Temperature = %f\r\n\
Camera Temperature =      %f\r\n\
Cold Paddle Position =    %d\r\n\
Hot Paddle Position =     %d\r\n\
Ambient Paddle Position = %d\r\n\
Rotor Position =          %s\r\n\
Rotation Stage =          %2.3f\r\n\
Goniometer Stage =        %2.3f\r\n\
" , DateStamp, Filename, CollectNotesComment, buffer,
                //DesiredSpeed,
                SlowSpeedRotation, SlowSpeedGoniometer,
                FrameRate, ImageWidth, ImageHeight, NumFrames, SensorType,
                Wavelength,
                //TemperatureHot, TemperatureCold, HotFile, ColdFile, WaveFile, ScanFile
                PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot,
                PresentTemperatureBbExt, CameraTemperature,
                PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient,
                PresentPositionRotor, PresentPositionRotation-RotationHome, PresentPositionGoniometer-GoniometerHome);
    }
    else {
        sprintf( HeaderString, "ENVI\r\n\
description = { \r\n\r\n\
Date =          %s\r\n\
File Name =     %s\r\n\r\n\
Collect Notes = %s\r\n\r\n\
Long Comment =  %s\r\n}\r\n\
; Scan rate\r\n\
Scan Rate =        %f\r\n\
; Camera Frequency\r\n\
FPS =              %d\r\n\
; Pixel Columns\r\n\
samples =          %d\r\n\
; Pixel Rows\r\n\
lines =            %d\r\n\
; Number of frames in image cube\r\n\
bands =            %d\r\n\
; Number of bytes of embeded header information\r\n\
header offset =    0\r\n\
; File type recognized by ENVI\r\n\
file type =        ENVI Standard\r\n\
; Data representation where 12 = 16-bit unsigned integer\r\n\
data type =        12\r\n\
; Band Sequential (BSQ), Band Interleaved by Pixel (BIP), Band Interleaved by Line (BIL)\r\n\
interleave =       BSQ\r\n\
; Name of HSI and camera system\r\n\
sensor type =      %s\r\n\
byte order =       0\r\n\
Cal Wavelength =   %s\r\n\
Temperature =      %f\r\n\
Hot Blackbody =    %s\r\n\
Cold Blackbody =   %s\r\n\
Wavelength File =  %s\r\n\
Scan Files =       %s\r\n\
" , DateStamp, Filename, CollectNotesComment, buffer,
DesiredSpeed, FrameRate, ImageWidth, ImageHeight, NumFrames, SensorType,
Wavelength, Temperature, HotFile, ColdFile, WaveFile, ScanFile);
    }

    fwrite( HeaderString, 1, strlen(HeaderString), FH );

    // Close the file.
    if (fclose(FH)) {
        ;//AfxMessageBox("Error when closing header file.");
    }
}


void CreateFilePathString(char *SpecificPath, char* DataDirectory, char *DateTimeStamp, bool bUseDirectoryStructure/*, char *ShortComment, bool bSkipBlackBodies*/)
{
    char DateStamp[64];


    // Get directory and add "\" if neccessary
    //GetDlgItemText( IDC_DATADIRECTORY, DataDirectory, 1024);
    if( DataDirectory[strlen(DataDirectory)-1] != '\\' ) {
        strcat(DataDirectory, "\\");
    }

    /*  //Get the short comment
    //GetDlgItemText( IDC_SHORTCOMMENT, ShortComment, 2048 );
    if( strlen(ShortComment) != 0 ) {
        strcat(ShortComment, "-");
    }*/


    strncpy(DateStamp, DateTimeStamp, 6);
    DateStamp[6] = 0;    // Null Terminate

    // Make complete directory path on hard drive for scan (add DataDirectory + SpecificScanPath)
    if( bUseDirectoryStructure) {
        sprintf( SpecificPath, "%s%s\\%s\\", DataDirectory, DateStamp, DateTimeStamp );
    }
    else{
        sprintf( SpecificPath, "%s%s\\", DataDirectory, DateStamp);
    }
}


// FullDirectoryPath is something like this:	X:\test\data
void MakeDirectoryPath( char *FullDirectoryPath/*, char *DateStampScan, char *DataDirectory*//*, char *DateTimeStamp*//*, bool bUseDirectoryStructure, char *ShortComment, bool bSkipBlackBodies*/ )
{
    //int retval;
    int SlashPos;
    char *ptrSlash;
    char TempPath[1024];
    wchar_t PathToCreate[1024];


    strcpy(TempPath, FullDirectoryPath);

    if( (FullDirectoryPath[1]==':') || (FullDirectoryPath[2]=='/') ) {
        ptrSlash = strstr( TempPath, "\\" );
        *ptrSlash = 20;
    }

    //
    while( strstr(TempPath, "\\") != NULL ) {
        ptrSlash = strstr( TempPath, "\\" );
        *ptrSlash = 20;
        SlashPos = ptrSlash-TempPath;

        if( ptrSlash!=NULL ) {
            //strcpy(PathToCreate, FullDirectoryPath);
            mbstowcs (PathToCreate, FullDirectoryPath, SlashPos);
            PathToCreate[SlashPos] = 0;
#ifdef __unix__
            retval = mkdir(PathToCreate, 0777);
#else
            //retval = mkdir(PathToCreate);
            CreateDirectory((wchar_t *)PathToCreate, NULL);
#endif
        }
    }
}

#ifdef FORLATERQ
void GetLatestWaveCalFile( char *DataDirectory )
{
    long NewestTime;
    char WavecalPath[256];
    struct dirent *epdf;
    struct stat attrib;
#ifdef __unix__
    DIR *dpdf;

    WaveCalFilename[0] = 0;	//Clear Wavecal Filename


    // Append '/' to end if none there.
    if( DataDirectory[strlen(DataDirectory)-1] != '/' ) {
        strcat(DataDirectory, "\\");
    }

    dpdf = opendir(DataDirectory);
    if (dpdf != NULL){
        NewestTime = 0;

        while( (epdf = readdir(dpdf)) ) {
            if( strlen(epdf->d_name) > 6 ) {    // Don't check if less than 7 chars.

                // Only check files ending in "bsq".
                if( strcmp(&epdf->d_name[strlen(epdf->d_name)-6], ".bsq")==0 ) {
                    sprintf(WavecalPath, "%s%s" , DataDirectory, epdf->d_name);
                    stat(WavecalPath, &attrib);

                    if( NewestTime < attrib.st_mtime ) {
                        NewestTime=attrib.st_mtime;
                        strcpy( WaveCalFilename, epdf->d_name );
                    }
                }
            }
        }
    }
#endif
}
#endif

#ifdef __unix__
void CopyFile(char *SrcFilename, char *DestFilename)
{
    if (SrcFilename && DestFilename) {
        int source = open(SrcFilename, O_RDONLY, 0);
        int dest = open(DestFilename, O_WRONLY | O_CREAT /*| O_TRUNC*/, 0644);

        // struct required, rationale: function stat() exists also
        struct stat stat_source;
        fstat(source, &stat_source);

        sendfile(dest, source, 0, stat_source.st_size);

        close(source);
        close(dest);
    }
}
#endif

#ifdef FORLATERQ
void CopyToProcessingDir(char *SpecificPath, char *ProcessingDirectory,
                         char *FilenameWithExtensionCold, char *FilenameWithExtensionScene, char *FilenameWithExtensionHot,
                         char *FilenameCold, char *FilenameScene, char *FilenameHot)
{
#ifdef __unix__
    char SrcFilename[4096], DestFilename[4096]; //, OldFiles[4096];
    char WaveCalFilenameHdr[4096];
    struct dirent *next_file;
    DIR *theFolder;
    char filepath[256];


    // ***** Remove old files from processing directory *****
    theFolder = opendir(ProcessingDirectory);

    if(theFolder==NULL) {
        // Give a message
        return;
    }

    while ( (next_file = readdir(theFolder)) ) {
        // build the full path for each file in the folder
        sprintf(filepath, "%s/%s", ProcessingDirectory, next_file->d_name);
        remove(filepath);
    }

    // *****  Copy files to processing directory  *****
    sprintf(SrcFilename, "%s%s" , SpecificPath, FilenameWithExtensionCold);
    sprintf(DestFilename, "%s/%s" , ProcessingDirectory, FilenameWithExtensionCold);
    CopyFile(SrcFilename, DestFilename, true);	// Copy Cold

    sprintf(SrcFilename, "%s%s" , SpecificPath, FilenameWithExtensionScene);
    sprintf(DestFilename, "%s/%s" , ProcessingDirectory, FilenameWithExtensionScene);
    CopyFile(SrcFilename, DestFilename);	// Copy Scene

    sprintf(SrcFilename, "%s%s" , SpecificPath, FilenameWithExtensionHot);
    sprintf(DestFilename, "%s/%s" , ProcessingDirectory, FilenameWithExtensionHot);
    CopyFile(SrcFilename, DestFilename);	// Copy Hot

    sprintf(SrcFilename, "%s%s.hdr" , SpecificPath, FilenameCold);
    sprintf(DestFilename, "%s/%s.hdr" , ProcessingDirectory, FilenameCold);
    CopyFile(SrcFilename, DestFilename);	// Copy Cold Header

    sprintf(SrcFilename, "%s%s.hdr" , SpecificPath, FilenameScene);
    sprintf(DestFilename, "%s/%s.hdr" , ProcessingDirectory, FilenameScene);
    CopyFile(SrcFilename, DestFilename);	// Copy Scene Header

    sprintf(SrcFilename, "%s%s.hdr" , SpecificPath, FilenameHot);
    sprintf(DestFilename, "%s/%s.hdr" , ProcessingDirectory, FilenameHot);
    CopyFile(SrcFilename, DestFilename);	// Copy Hot Header


    // ***** Get latest Wavecal and copy *****
    GetLatestWaveCalFile( DataDirectory );

    sprintf(SrcFilename, "%s%s" , DataDirectory, WaveCalFilename);
    sprintf(DestFilename, "%s/%s" , SpecificPath, WaveCalFilename);
    CopyFile(SrcFilename, DestFilename);	// Copy Wavelength

    strncpy( WaveCalFilenameHdr, WaveCalFilename, strlen(WaveCalFilename)-6 );
    sprintf(SrcFilename, "%s%s.hdr" , DataDirectory, WaveCalFilenameHdr);
    sprintf(DestFilename, "%s/%s.hdr" , SpecificPath, WaveCalFilenameHdr);
    CopyFile(SrcFilename, DestFilename);	// Copy Wavelength Header
#endif
}
#endif

void strupper(char *s) {
    while (*s) {
        if(islower(*s)) *s=toupper(*s);
        s++;
    }
}



void ReadConfig(float *GoniometerZero, float *RotationZero, float *BBPosition, float *SlowSpeed, float *StartPosition, float *StopPosition, int *NumFrames,
                float *DesiredTemperaturePaddleCold, float *DesiredTemperaturePaddleHot,
                float *DesiredTemperatureRotorCold, float *DesiredTemperatureRotorHot,
                float *DesiredTemperatureBbExt, float *DesiredTemperatureBbExt2,
                char *DataDirectory, char *ProcessingDirectory, char *UchisVersion)
{
    char* EqualSignPosition;
    char* EolPosition;
    char line[256];
    FILE *fp;

    if( (fp = fopen("tirfall.ini", "rt")) == NULL ) {
        //if( *SlowSpeed == 0 ) *SlowSpeed = 0.5;
        if( SlowSpeedRotation==0) SlowSpeedRotation = .50;
        if( SlowSpeedGoniometer==0) SlowSpeedGoniometer = 0.30;
        if( *GoniometerZero == 0 ) *GoniometerZero = (float).565;
        if( *RotationZero == 0 ) *RotationZero = (float)-.390;
        if( *BBPosition == 0 ) *BBPosition = 0;
        if( *StartPosition == 0 ) *StartPosition = -3;
        if( *StopPosition == 0 ) *StopPosition = 3;
        if( *NumFrames == 0 ) *NumFrames = 300;
        if( *DesiredTemperaturePaddleCold == 0 ) *DesiredTemperaturePaddleCold = 30;
        if( *DesiredTemperaturePaddleHot == 0 ) *DesiredTemperaturePaddleHot = 50;
        if( *DesiredTemperatureRotorCold == 0 ) *DesiredTemperatureRotorCold = 30;
        if( *DesiredTemperatureRotorHot == 0 ) *DesiredTemperatureRotorHot = 50;
        if( *DesiredTemperatureBbExt == 0 ) *DesiredTemperatureBbExt = 23;
        if( *DesiredTemperatureBbExt2 == 0 ) *DesiredTemperatureBbExt2 = 23;
        //if( *DesiredTemperatureBbExtHot == 0 ) *DesiredTemperatureBbExtHot = 50;
        if( strlen(DataDirectory) == 0 ) strcpy( DataDirectory, "/home/winsys/data" );
        if( strlen(ProcessingDirectory) == 0 ) strcpy( ProcessingDirectory, "/home/winsys/data" );
        if( strlen(UchisVersion) == 0 ) strcpy( UchisVersion, "n/a" );

        if(ScanStageSlowSpeed==0) ScanStageSlowSpeed = 0.30;
        if(ScanStageStartPosition==0) ScanStageStartPosition = -3.00;
        if(ScanStageStopPosition==0) ScanStageStopPosition = 3.00;


        // What happened to the com port numbers??
        if(CalibratorPortNum==0) CalibratorPortNum = 1;
        if(MotionPortNum==0) MotionPortNum = 2;
        if(BbPortNum==0) BbPortNum = 5;
        if(Bb2PortNum==0) Bb2PortNum = 7;
        if(GpsPortNum==0) GpsPortNum = 6;

        Gps.parm.GpsBackwards = FALSE;

        Gps.parm.HeadingInit=0;
        Gps.parm.LatitudeInit=21;
        Gps.parm.LongitudeInit=-158;
        Gps.parm.AltitudeInit=9;

        //if(RotationHome==0) RotationHome = 0.565;
        //if(GoniometerHome==0) GoniometerHome = -0.390;

        strcpy(DataDirectory, "C:\\WaxfallData");
        return;
    }


    *line = 0;
    while ( fgets ( line, sizeof(line), fp ) != NULL ) { /* read a line */
        // Make uppercase
        strupper(line);

        EqualSignPosition = strstr(line, "=");
        if(EqualSignPosition != NULL) {
            *EqualSignPosition=0;
            EqualSignPosition++;

            if( strcmp(line, "ROTATIONSCANSPEED") == 0 )      SlowSpeedRotation = atof(EqualSignPosition);
            if( strcmp(line, "GONIOMETERSCANSPEED") == 0 )    SlowSpeedGoniometer = atof(EqualSignPosition);
            //if( strcmp(line, "NUMBEROFFRAMES") == 0 )       *NumFrames = atoi(EqualSignPosition);
            if( strcmp(line, "GONIOMETERHOME") == 0 )         *GoniometerZero = atof(EqualSignPosition);
            if( strcmp(line, "ROTATIONHOME") == 0 )           *RotationZero = atof(EqualSignPosition);
            if( strcmp(line, "BBPOSITION") == 0 )             *BBPosition = atof(EqualSignPosition);
            if( strcmp(line, "STARTPOSITION") == 0 )          *StartPosition = atof(EqualSignPosition);
            if( strcmp(line, "STOPPOSITION") == 0 )           *StopPosition = atof(EqualSignPosition);
            if( strcmp(line, "NUMBEROFFRAMES") == 0 )         *NumFrames = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREPADDLECOLD") == 0 ) *DesiredTemperaturePaddleCold = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREPADDLEHOT") == 0 )  *DesiredTemperaturePaddleHot = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREROTORCOLD") == 0 )  *DesiredTemperatureRotorCold = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREROTORHOT") == 0 )   *DesiredTemperatureRotorHot = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREEXTERNALBB") == 0 )   *DesiredTemperatureBbExt = atof(EqualSignPosition);
            if( strcmp(line, "DESIREDTEMPERATUREEXTERNALBB2") == 0 )   *DesiredTemperatureBbExt2 = atof(EqualSignPosition);
            if( strcmp(line, "MOTIONCOMPORT") == 0 )  MotionPortNum = atoi(EqualSignPosition);
            if( strcmp(line, "EXTERNALBBCOMPORT") == 0 )  BbPortNum = atoi(EqualSignPosition);
            if( strcmp(line, "EXTERNALBB2COMPORT") == 0 )  Bb2PortNum = atoi(EqualSignPosition);
            if( strcmp(line, "CALIBRATORCOMPORT") == 0 )  CalibratorPortNum = atoi(EqualSignPosition);
            if( strcmp(line, "GPSCOMPORT") == 0 )  GpsPortNum = atoi(EqualSignPosition);
            if( strcmp(line, "LATITUDE") == 0 )  Gps.parm.LatitudeInit = atof(EqualSignPosition);
            if( strcmp(line, "LONGITUDE") == 0 )  Gps.parm.LongitudeInit = atof(EqualSignPosition);
            if( strcmp(line, "ALTITUDE") == 0 )  Gps.parm.AltitudeInit = atof(EqualSignPosition);
            if( strcmp(line, "HEADING") == 0 )  Gps.parm.HeadingInit = atof(EqualSignPosition);

            if( strcmp(line, "SCANSTAGESCANRATE") == 0 )      ScanStageSlowSpeed = atof(EqualSignPosition);
            if( strcmp(line, "SCANSTAGESTARTPOSITION") == 0 )          ScanStageStartPosition = atof(EqualSignPosition);
            if( strcmp(line, "SCANSTAGESTOPPOSITION") == 0 )           ScanStageStopPosition = atof(EqualSignPosition);

            if( strcmp(line, "USEMOTIONSTAGE") == 0 ) {
                EolPosition = strstr(EqualSignPosition, "\r");
                if(EolPosition != NULL) *EolPosition=0;
                EolPosition = strstr(EqualSignPosition, "\n");
                if(EolPosition != NULL) *EolPosition=0;

                if( strcmp(EqualSignPosition, "YES")==0 ) {
                    UseMotionStage=TRUE;
                } else {
                    UseMotionStage=FALSE;
                }
            }

            if( strcmp(line, "GPSBACKWARDS") == 0 ) {
                EolPosition = strstr(EqualSignPosition, "\r");
                if(EolPosition != NULL) *EolPosition=0;
                EolPosition = strstr(EqualSignPosition, "\n");
                if(EolPosition != NULL) *EolPosition=0;

                if( strcmp(EqualSignPosition, "YES")==0 ) {
                    Gps.parm.GpsBackwards=TRUE;
                } else {
                    Gps.parm.GpsBackwards=FALSE;
                }
            }

            if( strcmp(line, "DATADIRECTORY") == 0 )          {
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\r')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\n')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '/')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                strcpy(DataDirectory, EqualSignPosition);
            }

            if( strcmp(line, "PROCESSINGDIRECTORY") == 0 )    {
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\r')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\n')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '/')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                strcpy(ProcessingDirectory, EqualSignPosition);
            }

            if( strcmp(line, "UCHISVERSION") == 0 )          {
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\r')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '\n')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                if( EqualSignPosition[ strlen(EqualSignPosition)-1 ] == '/')
                    EqualSignPosition[ strlen(EqualSignPosition)-1 ] = 0;
                strcpy(UchisVersion, EqualSignPosition);
            }
        }
        *line = 0;
    }

    fclose(fp);


    /**DesiredTemperaturePaddleCold = (float)23;
    *DesiredTemperaturePaddleHot = (float)23;
    *DesiredTemperatureRotorCold = (float)23;
    *DesiredTemperatureRotorHot = (float)23;*/

    // Set default values if not  set.
    // What happened to the com port numbers??
    if(CalibratorPortNum==0) CalibratorPortNum = 1;
    if(MotionPortNum==0) MotionPortNum = 2;
    if(BbPortNum==0) BbPortNum = 5;
    if(Bb2PortNum==0) Bb2PortNum = 7;
    if(GpsPortNum==0) GpsPortNum = 6;

    //if(RotationHome==0) RotationHome = 0.565;
    //if(GoniometerHome==0) GoniometerHome = -0.390;
    if(SlowSpeedRotation==0) SlowSpeedRotation = 10.00;
    if(SlowSpeedGoniometer==0) SlowSpeedGoniometer = 0.30;
    //if(NumberOfFrames==0) NumberOfFrames = 100;
    if(*StartPosition==0) *StartPosition = -3.00;
    if(*StopPosition==0) *StopPosition = 3.00;
    if(*DesiredTemperatureBbExt==0) *DesiredTemperatureBbExt = 24.00;
    if(*DesiredTemperatureBbExt2==0) *DesiredTemperatureBbExt2 = 24.00;

    if(ScanStageSlowSpeed==0) ScanStageSlowSpeed = 1.0;
    if(ScanStageStartPosition==0) ScanStageStartPosition = -3.00;
    if(ScanStageStopPosition==0) ScanStageStopPosition = 3.00;

    //strcpy(DataDirectory, "C:\\WaxfallData");
}


void WriteConfig(float GoniometerZero, float RotationZero,
                 float BBPosition, float SlowSpeed, float StartPosition, float StopPosition, int NumFrames,
                 float DesiredTemperatureACold, float DesiredTemperatureAHot,
                 float DesiredTemperatureBCold, float DesiredTemperatureBHot,
                 float DesiredTemperatureBbExt, float DesiredTemperatureBbExt2,
                 char *DataDirectory, char *ProcessingDirectory, char *UchisVersion)
{
    char line[256];
    FILE *fp;
return;
    if(MotionPortNum==0) {  // What happened to the com port numbers??
        return;
    }

    if( (fp = fopen("tirfall.ini", "wt")) == NULL ) {
        return;
    }

    // Write scan speed
    sprintf( line, "RotationHome=%4.3f\n", RotationZero);
    fwrite( line, strlen(line), 1, fp );

    // Write scan speed
    sprintf( line, "GoniometerHome=%4.3f\n", GoniometerZero);
    fwrite( line, strlen(line), 1, fp );

    // Write scan speed
    sprintf( line, "RotationScanSpeed=%4.2f\n", SlowSpeedRotation);
    fwrite( line, strlen(line), 1, fp );

    sprintf( line, "GoniometerScanSpeed=%4.2f\n", SlowSpeedGoniometer);
    fwrite( line, strlen(line), 1, fp );

    // Write scan speed
    sprintf( line, "NumberOfFrames=%d\n\n", NumFrames);
    fwrite( line, strlen(line), 1, fp );

    // Write BB position
    sprintf( line, "BBPosition=%4.2f\n", BBPosition);
    fwrite( line, strlen(line), 1, fp );

    // Write Start position
    sprintf( line, "StartPosition=%4.2f\n", StartPosition);
    fwrite( line, strlen(line), 1, fp );

    // Write Stop position
    sprintf( line, "StopPosition=%4.2f\n\n", StopPosition);
    fwrite( line, strlen(line), 1, fp );

    // Write Cold
    sprintf( line, "DesiredTemperatureACold=%4.2f\n", DesiredTemperatureACold);
    fwrite( line, strlen(line), 1, fp );

    // Write Hot
    sprintf( line, "DesiredTemperatureAHot=%4.2f\n\n", DesiredTemperatureAHot);
    fwrite( line, strlen(line), 1, fp );

    // Write Cold
    sprintf( line, "DesiredTemperatureBCold=%4.2f\n", DesiredTemperatureBCold);
    fwrite( line, strlen(line), 1, fp );

    // Write Hot
    sprintf( line, "DesiredTemperatureBHot=%4.2f\n\n", DesiredTemperatureBHot);
    fwrite( line, strlen(line), 1, fp );

    // Write Ext
    sprintf( line, "DesiredTemperatureExternalBB=%4.2f\n", DesiredTemperatureBbExt);
    fwrite( line, strlen(line), 1, fp );

    // Write Ext2
    sprintf( line, "DesiredTemperatureExternalBB2=%4.2f\n", DesiredTemperatureBbExt2);
    fwrite( line, strlen(line), 1, fp );

/*    // Write Hot
    sprintf( line, "DesiredTemperatureExtHot=%4.2f\n\n", DesiredTemperatureExtHot);
    fwrite( line, strlen(line), 1, fp );
*/
    // Write default data directory
    sprintf( line, "DataDirectory=%s\n\n", DataDirectory);
    fwrite( line, strlen(line), 1, fp );
#ifdef REMOVETHIS
    // Write directory for Pacific Spectral processing
    sprintf( line, "ProcessingDirectory=%s\n", ProcessingDirectory);
    fwrite( line, strlen(line), 1, fp );

    // Write UCHIS Version
    sprintf( line, "UchisVersion=%s\n\n", UchisVersion);
    fwrite( line, strlen(line), 1, fp );



    // Write serial help info to header
    strcpy( line, "# set these to 98 for UDev assignment in case auto detection fails\r\n\
# udev values should be:\r\n\
# gumstix = /dev/ttyUSB_gumstix\r\n\
# motion stage = /dev/ttyUSB_smc100\r\n\
# black body controller = /dev/ttyUSB_arroyo\r\n" );
    fwrite( line, strlen(line), 1, fp );
#endif
    // Write Focus default port
    sprintf( line, "CalibratorComPort=%d\n", CalibratorPortNum);
    fwrite( line, strlen(line), 1, fp );

    // Write RotationA default port
    sprintf( line, "MotionComPort=%d\n", MotionPortNum);
    fwrite( line, strlen(line), 1, fp );

    // Write Ext Black Body default port
    sprintf( line, "ExternalBBComPort=%d\n", BbPortNum);
    fwrite( line, strlen(line), 1, fp );

    // Write Ext Black Body 2 default port
    sprintf( line, "ExternalBB2ComPort=%d\n", Bb2PortNum);
    fwrite( line, strlen(line), 1, fp );

    fclose(fp);
}




/*
void SaveHeader(char * FilePath, int ImageWidth, int ImageHeight,
                char *Directory, int NumFrames, char* DateTime, int BkitIndex, char *HotFile, char *ColdFile, char *WaveFile,
                char*ScanFile, float Temperature, float TemperatureHot, float TemperatureCold, char *FileExtension, char *ScanDirection,
                float DesiredSpeed, int FrameRate, char *SensorType, char *Wavelength, char *buffer, char *DataDirectory )
{
    char DateStampScan[64];
    char *WPos;
    //SYSTEMTIME sysTime;   // Win32 time information
    char SpecificPath[1024];
    char Filename[1024];

    //int NumFrames;
    char Date[32];


    //NumFrames = GetDlgItemInt(IDC_NUMCALFRAMES, &ReturnValue, FALSE);
    //NumFrames = 100;

    switch( PresentTask ) {
    case INTCOLDTASK:
        break;
    case INTHOTTASK:
        break;
    case INTOTHERTASK:
        break;
    case WAVECALTASK:

        //Re insert the Wavelength in case user changed it between change of state and save.
        //GetDlgItemText(IDC_WAVELENGTH, buffer, 256 );
        //strcpy( Wavelength, buffer);

        //Replace period with underscore
        WPos = strstr(buffer, ".");
        if( WPos!=NULL ){
            memmove( &WPos[0], "_", 1 );
        }

        switch( strlen(buffer) ) {
        case 0:
            break;
        case 1:
            strcat(buffer, "_00");
            break;
        case 2:
            strcat(buffer, "_0");
            break;
        case 3:
            strcat(buffer, "0");
            break;
        case 4:
            break;
        default:
            buffer[4] = 0;
        }

        if( strlen(buffer) != 0 ) {
            WPos = strstr(FilePath, "/w_");
            if( WPos!=NULL	 ) {
                memmove( &WPos[3], buffer, 4 );
            }
        }

        break;
    case SCANTASK:
        break;
    default:
        break;
    }



    if( (PresentTask == WAVECALTASK) || (*ScanDirection == 'S') || !m_bAllPositionsFlagOn / *|| (strcmp(FileExtension, ".bsqmn")==0)* / ) {

        if( PresentTask == WAVECALTASK ) {
            MakeHeader( DataDirectory, WaveCalFilename, NumFrames, DateStampWave, 1, " ", " ", WaveCalFilename, " ", PresentTemperatureBb1, 0, 0,
                        FileExtension, ScanDirection, DesiredSpeed, FrameRate, ImageWidth, ImageHeight, SensorType, Wavelength, buffer );
        }
        else {

            if(m_bAllPositionsFlagOn) {
                CreateFilePathString(SpecificPath, / *DateStampScan,* / DataDirectory, DateTimeStamp, bUseDirectoryStructure/ *, ShortComment, bSkipBlackBodies* /);
                sprintf( Filename, "SPI-%s-%s%s-scan%s", DateStampScan, ShortComment, ScanDirection, FileExtension);

                MakeHeader( SpecificPath, Filename, NumFrames, DateStampScan, 1, " ", " ", WaveCalFilename, " ", PresentTemperatureBb1, 0, 0,
                            FileExtension, ScanDirection, DesiredSpeed, FrameRate, ImageWidth, ImageHeight, SensorType, Wavelength, buffer );
            }
            else {
                MakeHeader( "", FilePath, NumFrames, DateStampScan, 1, " ", " ", WaveCalFilename, " ", PresentTemperatureBb1, 0, 0,
                            FileExtension, ScanDirection, DesiredSpeed, FrameRate, ImageWidth, ImageHeight, SensorType, Wavelength, buffer );
            }
        }
    }

    PresentTask = 0;
}
*/

/*
// For Black body file names
void MakeFileName(char* Suffix, int DlgControl)
{
    //int Temperature;  Make this global
    //char DateStamp[64];
    char DateStamp[64];
    char SpecificPath[2048];

    CreateFilePathString(SpecificPath, DateStamp, DataDirectory, DateTimeStamp, bUseDirectoryStructure, ShortComment, bSkipBlackBodies);	// SpecificPath is set inside here
    MakeDirectoryPath( SpecificPath, DateStamp, DataDirectory, DateTimeStamp, bUseDirectoryStructure, ShortComment, bSkipBlackBodies );


    //Get Temperature
    if( DlgControl != NULL ) {
        GetDlgItemText(DlgControl, Temperature, 64 );
    } else {
        if(m_bUseBBUnit2) {
            sprintf( Temperature, "%2d", (int)(TemperatureBb2+0.5) );
        }
        else {
            sprintf( Temperature, "%2d", (int)(PresentTemperatureBb1+0.5) );
        }
    }

    if( strcmp(Suffix, "BB_HOT")==0 ) {
        strcpy(DateStampHotArray, DateStamp);
        if(m_bUseBBUnit2) {
            TemperatureHotArray = TemperatureBb2;
        }
        else {
            TemperatureHotArray = PresentTemperatureBb1;
        }
    }
    else {
        strcpy(DateStampColdArray, DateStamp);
        if(m_bUseBBUnit2) {
            TemperatureColdArray = TemperatureBb2;
        }
        else {
            TemperatureColdArray = PresentTemperatureBb1;
        }
    }

    //MakeFileName
#ifdef NIIMAQ_CAMERA
    //sprintf( FilePath, "%s%s_%s_%s%s.bsq", DataDirectory, Prefix, Temperature, ShortComment, DateStamp);
    //sprintf( FilePath, "%s%s_%s_%s.bsq", SpecificPath, Suffix, Temperature, DateStamp);
    sprintf( FilePath, "%sSPI-%s-%s_%s%s", SpecificPath, DateStamp, Suffix, Temperature, FileExtension);
#else
    sprintf( FilePath, "%s%s_%s_%s%s.het", DataDirectory, Prefix, Temperature, ShortComment, DateStamp);
#endif


    if( strcmp(Suffix, "BB_HOT")==0 ) {
        //sprintf( HotFilenameArray, 1024, "%s_%s_%s.bsq", Suffix, Temperature, DateStamp);
        sprintf( HotFilenameArray, 1024, "SPI-%s-%s_%s%s", DateStamp, Suffix, Temperature, FileExtension);
    }
    else {
        //sprintf( ColdFilenameArray, 1024, "%s_%s_%s.bsq", Suffix, Temperature, DateStamp);
        sprintf( ColdFilenameArray, 1024, "SPI-%s-%s_%s%s", DateStamp, Suffix, Temperature, FileExtension);
    }
}
*/


bool ReadTircisConfig(char *wavefile, char *segfile, int *npixInterf, float *xoff, float *yoff)
{
    char* EqualSignPosition, *eq1;
    char line[256], upline[256];
    FILE *fp;

    // open the tirprocess.ini file and read in processing files and parameters
    if( (fp = fopen("tirprocess.ini", "rt")) != NULL )  {

    while (!feof(fp)){
        fgets (line, 256, fp) ;
        EqualSignPosition = strstr(line, "=");

        if(EqualSignPosition != NULL) {
            //*EqualSignPosition=0;
            //EqualSignPosition++;

            // Make uppercase
            strcpy (upline, line) ;
            strupper(upline);
            eq1 = &EqualSignPosition[1] ;
            if( strstr(upline, "WAVECAL FILE") )   strcpy (wavefile, eq1);

            if( strstr(upline, "SEGMENT FILE") )   strcpy (segfile, eq1) ;
            if (strstr (upline, "NUMBER OF RAW"))  *npixInterf = atoi (eq1) ;
            if (strstr (upline, "X OFF")) *xoff = atof (eq1) ;
            if (strstr (upline, "Y OFF")) *yoff = atof (eq1) ;
        }


    }
    }
    else { // tirprocess.ini not found

        QMessageBox::warning(0, "File Missing", "tirprocess.ini not found" );
        return (false) ;
    }
    // remove leading whitespace
    wavefile[strcspn(wavefile,"\r\n")]=0 ;
    segfile[strcspn(segfile,"\r\n")]=0 ;
    return (true) ;
}
