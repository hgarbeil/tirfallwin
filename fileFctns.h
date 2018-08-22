#ifndef FILEFCTNS_H
#define FILEFCTNS_H

void strupper(char *s);
#ifdef __unix__
void CopyFile(char *SrcFilename, char *DestFilename);
#endif
//void GetLatestWaveCalFile( char *DataDirectory );
void CreateFilePathString(char *SpecificPath, char* DataDirectory, char *DateTimeStamp, bool bUseDirectoryStructure/*, char *ShortComment, bool bSkipBlackBodies*/);
void MakeDirectoryPath( char *FullDirectoryPath/*, char *TimeStampScan, char *DataDirectory, char *TimeStampForDirectory, bool bUseDirectoryStructure, char *ShortComment, bool bSkipBlackBodies*/ );

void SaveHeader(char *FilePath, int ImageWidth, int ImageHeight,
                char *Directory, int NumFrames, char *DateTime, int BkitIndex, char *HotFile, char *ColdFile, char *WaveFile,
                char *ScanFile, float Temperature, float TemperatureHot, float TemperatureCold, char *FileExtension, char *ScanDirection,
                float DesiredSpeed, int FrameRate, char *SensorType, char *Wavelength, char *buffer, char *DataDirectory );

void MakeHeader( char *Directory, char *Filename, int NumFrames, char *DateTimeStamp, int BkitIndex, char *HotFile, char *ColdFile, char*WaveFile,
                 char*ScanFile, float Temperature, float TemperatureHot, float TemperatureCold, char *FileExtension,
                 float DesiredSpeed, int FrameRate, int ImageWidth, int ImageHeight, char *SensorType, char *Wavelength, char *buffer );

/*void CopyToProcessingDir(char *SpecificPath, char *ProcessingPath,
                         char *FilenameWithExtensionCold, char *FilenameWithExtensionScene, char *FilenameWithExtensionHot,
                         char *FilenameCold, char *FilenameScene, char *FilenameHot);
*/
void ReadConfig(float *GoniometerOffset, float *RotationOffset,
                float *BBPosition, float *SlowSpeed, float *StartPosition, float *StopPosition, int *NumFrames,
                float *DesiredTemperaturePaddleCold, float *DesiredTemperaturePaddleHot,
                float *DesiredTemperatureRotorCold, float *DesiredTemperatureRotorHot,
                float *DesiredTemperatureBbExt, float *DesiredTemperatureBbExt2,
                char *DataDirectory, char *ProcessingDirectory, char *UchisVersion);
void WriteConfig(float GoniometerOffset, float RotationOffset,
                 float BBPosition, float SlowSpeed, float StartPosition, float StopPosition, int NumFrames,
                 float DesiredTemperaturePaddleCold, float DesiredTemperaturePaddleHot,
                 float DesiredTemperatureRotorCold, float DesiredTemperatureRotorHot,
                 float DesiredTemperatureBbExt, float DesiredTemperatureBbExt2,
                 char *DataDirectory, char *ProcessingDirectory, char *UchisVersion);
bool ReadTircisConfig(char *wavefile, char *segfile, int *npixInterf, float *xoff, float *yoff ) ;
#endif // FILEFCTNS_H
