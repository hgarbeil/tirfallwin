#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef GLOBAL
#define EXT extern
#else
#define EXT
#endif

#define LALA
#ifdef LALA

// CMIGITS
//EXT BOOL m_bGPSBackwards;

EXT BOOL UseMotionStage;

EXT int MouseX, MouseY;
EXT long MouseIntensity;

// Calibrator
EXT float TemperaturePaddleCold, TemperaturePaddleHot, TemperaturePaddleAmbient;
EXT float TemperatureRotorCold, TemperatureRotorHot, TemperatureRotorAmbient;
EXT float TemperatureSensor[16];

// Misc
EXT long TircisStatus;
EXT bool ExitProgram;
EXT bool Abort;
EXT int FrameRate;
EXT float PresentFPS;
EXT int NumFrames;
EXT int NumTargetFrames;
EXT int NumCalFrames;

EXT char FilePath[2048];
EXT char DataDirectory[4096];
EXT char ProcessingDirectory[4096];
EXT char WaveCalFilename[2048];
EXT char ShortComment[512];
EXT char SensorType[1024];
EXT char Wavelength[64];
EXT bool bUseDirectoryStructure;
EXT bool AcqBusy;
EXT int FFTMaxVal;
EXT float SawToothWave[256];
EXT long FileNum;
EXT char ScriptFile[4096];

EXT int MinValPixel;
EXT int MaxValPixel;
EXT int MinValFft;
EXT int MaxValFft;
EXT uInt32 AcqImgWidth;
EXT uInt32 AcqImgHeight;
EXT uInt32 AcqImgSize;

// Global image arrays
EXT QImage *QImageCamera;
EXT uchar *ImgCamera;
EXT QImage *QImageFall;
EXT uchar *ImgFall;
EXT QImage *QImagePlotFft;
EXT uchar *ImgPlotFft;
EXT QImage *QImagePlotPixel;
EXT uchar *ImgPlotPixel;

EXT int PixelData[128];
EXT double FftData[128];
EXT bool ScriptMessageDone;

// Camera Class
#ifdef __unix__
EXT CAcqMc320 CameraMc320;
EXT CAcqTau CameraTau;
EXT CAcqPv640 CameraPv640;
#endif

EXT CAcqDummy320x240 CameraDummy320;
EXT CAcqDummy640x512 CameraDummy640;
EXT CAcqPhoton CameraPhoton;
EXT CAcqINO CameraINO;

// Gumstix globals
//:OEXT cssl_t *SerialGumstix;
EXT bool GumstixPortOpen;
//EXT int TircisStatus;


// Eat your cerial
#ifdef __unix__
EXT cssl_t *SerialBb;
EXT cssl_t *SerialRotation;
EXT cssl_t *SerialGonometer;
EXT cssl_t *SerialFocus;
#else
//EXT COMDevice SerialBb;
//EXT COMDevice SerialRotation;
//EXT COMDevice SerialGonometer;
//EXT COMDevice SerialFocus;
#endif


// BB globals
EXT bool BbPortOpen;
EXT bool BbBusy;
//EXT float DesiredTemperatureA;  // This is the value that A will be set to.  It is made equal to either DesiredTemperatureACold, or DesiredTemperatureAHot
//EXT float DesiredTemperatureB;
EXT float DesiredTemperatureBbExt, DesiredTemperatureBbExt2;
EXT float PresentTemperatureBbExt, PresentTemperatureBbExt2;
EXT float DesiredTemperaturePaddleCold, DesiredTemperaturePaddleHot, DesiredTemperatureRotorCold, DesiredTemperatureRotorHot;
EXT float PresentTemperaturePaddleCold, PresentTemperaturePaddleHot, PresentTemperatureRotorCold, PresentTemperatureRotorHot;
EXT bool PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient;
EXT char PresentPositionRotor[64];
//EXT float PresentFPS;
EXT float CameraTemperature;

// Rotation Globals
//EXT bool MotionPortOpen;
//EXT bool RotationBusy;
EXT float PresentPositionRotation;
EXT float DesiredPositionRotation;
EXT float DesiredSpeedRotation;
EXT float SlowSpeedRotation;
EXT float m_nIntermediateSpeedRotation;
EXT float m_nFastSpeedRotation;
EXT float RotationHome;

// Remove these when fixing read config
EXT float StartPositionRotation;
EXT float StopPositionRotation;
EXT float BBRotationPosition;

EXT float ScanStageSlowSpeed;
EXT float ScanStageStartPosition;
EXT float ScanStageStopPosition;

// Goniometer Globals
//EXT bool GonometerPortOpen;
//EXT bool GoniometerBusy;
EXT float PresentPositionGoniometer;
EXT float DesiredPositionGoniometer;
EXT float DesiredSpeedGoniometer;
EXT float SlowSpeedGoniometer;
EXT float m_nIntermediateSpeedGoniometer;
EXT float m_nFastSpeedGoniometer;
//EXT float StartGonometerPosition;
//EXT float StopGonometerPosition;
EXT float GoniometerHome;
//EXT float BBPosition;


/*// Focus Globals
EXT bool AutoFocusOn;
EXT bool FocusPortOpen;
EXT bool FocusBusy;
EXT long PresentFocus;
EXT int FocusSpeed;
//EXT long DesiredFocus;
EXT float MinFocus;
EXT float MaxFocus;
EXT double FinalDifferentialSummation;
//EXT float PresentFocusPosition;
EXT long DesiredFocusPosition;
//EXT float FocusSpeed;
*/

// Semaphores.
#ifdef __unix__
EXT sem_t SaveStart_count;
//EXT sem_t ChangeTemperatureA_count;
EXT sem_t ChangeTemperatureB_count;
EXT sem_t ChangeTemperatureExt_count;
EXT sem_t MovePosition_count;
EXT sem_t RunCalAcq_count;
EXT sem_t RunManualAcq_count;
EXT sem_t RunScan_count;
EXT sem_t RunAutoscan_count;

//EXT sem_t RunStare_count;
EXT sem_t ViewRange_count;
EXT sem_t AcquireFinished_count;
//EXT sem_t RunStareBB_count;
//EXT sem_t RunStareScene_count;
//EXT sem_t MoveFocus_count;
//EXT sem_t RunAutoFocus_count;
EXT sem_t RunScript_count;
EXT sem_t RunFfc_count;
#else
EXT HANDLE SaveStart_count;
EXT HANDLE ChangeTemperatureB_count;
EXT HANDLE ChangeTemperatureExt_count;
EXT HANDLE MovePosition_count;
EXT HANDLE RunCalAcq_count;
EXT HANDLE RunManualAcq_count;
EXT HANDLE RunScan_count;
EXT HANDLE RunAutoscan_count;
EXT HANDLE ViewRange_count;
EXT HANDLE AcquireFinished_count;
EXT HANDLE RunScript_count;
EXT HANDLE RunFfc_count;
#endif

EXT double LastBestValue;

#endif
#endif // GLOBAL_H
