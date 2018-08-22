// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MICROXCAM384I_CAPI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MICROXCAM384I_CAPI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MICROXCAM384I_CAPI_EXPORTS
#define MICROXCAM384I_CAPI_API __declspec(dllexport)
#else
#define MICROXCAM384I_CAPI_API __declspec(dllimport)
#endif

#include "GlobalDef.h"

extern "C" MICROXCAM384I_CAPI_API  int    fn_Initialize(bool bVirtual,char* pstrPath);
extern "C" MICROXCAM384I_CAPI_API  int    fn_DeInitialize();
extern "C" MICROXCAM384I_CAPI_API  char*  fn_ErrorToText(int iError);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetImageSize(int &iWidth,int &iHeight);

extern "C" MICROXCAM384I_CAPI_API  int    fn_GetFPATemp(double &dVal);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetExternalTrigger(int &iVal);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetExternalTrigger(int iValSet,int &iValRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetShutterPosition(int &iVal);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetShutterPosition(int iValSet,int &iValRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetGain(int &iVal);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetGain(int iValSet,int &iValRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetLsyncPw(int &iVal);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetLsyncPw(int iValSet,int &iValRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetSHCDSPWAndStart(int &iValPW,int &iValStart);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetSHCDSPWAndStart(int iValPWSet, int iValStartSet, int &iValPWRet, int &iValStartRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetSHVIDPWAndStart(int &iValPW,int &iValStart);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetSHVIDPWAndStart(int iValPWSet, int iValStartSet, int &iValPWRet, int &iValStartRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetFPABias(int &iValVRES_HIGH,int &iValVCSH,int &iValVCSL,int &iValVREF,int &iValVCM1,int &iValVCM2,int &iValVCM3 );
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetFPABias(int iValVRES_HIGHSet,int iValVCSHSet,int iValVCSLSet,int iValVREFSet,int iValVCM1Set,int iValVCM2Set,int iValVCM3Set,
                                                        int &iValVRES_HIGHRet,int &iValVCSHRet,int &iValVCSLRet,int &iValVREFRet,int &iValVCM1Ret,int &iValVCM2Ret,int &iValVCM3Ret);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SaveFactoryParam();
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetDACFixedValueAndState(int &iValFix,int &iValState);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetDACFixedValueAndState(int iValFixSet, int iValStateSet, int &iValFixRet, int &iValStateRet);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetStatusAndError(int &iValTecTempWarning, int &iValReadout, int &iPLL);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetFactoryInfo(char *pstrManufacturerName,int iManufacturerNameSize,
                                                            char *pstrModelName,int iModelNameSize,
                                                            char *pstrDeviceVersion,int iDeviceVersionSize,
                                                            char *pstrManufacturerSpecInfo,int iManufacturerSpecInfoSize,
                                                            char *pstrSN,int iSNSize);
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetSoftwareInfo(char *ptrMBFirmNumber,int iMBFirmNumberSize,
                                                             char *pstrMBFirmRev,int iMBFirmRevSize,
                                                             char *pstrMBFirmDC,int iMBFirmDCSize,
                                                             char *pstrMBFirmTC,int iMBFirmTCSize,
                                                             char *pstrFPGAFirmNumber,int iFPGAFirmNumberSize,
                                                             char *pstrFPGAFirmRev,int iFPGAFirmRevSize);
extern "C" MICROXCAM384I_CAPI_API  int    fn_SetRAWProcessOption(int iOpt);                           //See GlobalDef.h for valid process options
extern "C" MICROXCAM384I_CAPI_API  int    fn_TakeOffset(int iNbrFrame, bool bUseInternalShutter, bool bVerbose);
extern "C" MICROXCAM384I_CAPI_API  int    fn_LoadRPLFile(char* pstrFile);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Need to allocate a Array of unsigned short with a size of cameraWidth*cameraHeight
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetCameraImage(unsigned short aImageData[],int iArraySize);             
extern "C" MICROXCAM384I_CAPI_API  int    fn_GetCameraImageOffset(unsigned short aImageOffsetData[],int iArraySize);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" MICROXCAM384I_CAPI_API  int    fn_SetCameraCallback(pCameraCallback pCallback,void *pThis);             








