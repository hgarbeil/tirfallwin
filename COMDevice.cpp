//////////////////////////////////////////////////////////////////////
//
//  COMDevice.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "PhotonFall.h"
#include "COMDevice.h"

//////////////////////////////////////////////////////////////////////

COMDevice::COMDevice()
{
	m_hCOM = INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////

COMDevice::~COMDevice()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

BOOL COMDevice::Open(int nPort, BOOL bRead, BOOL bWrite)
{
    char messageString[256];
    wchar_t wideCOM[8];

	if (nPort < 1 || nPort > 32)
	{
		sprintf (messageString, "Failed to open COM port %d", nPort);
        //AfxMessageBox (messageString);
		return FALSE;
	}

    char szCOM[8];
    sprintf(szCOM, "COM%d", nPort);
    mbstowcs(wideCOM, szCOM, strlen(szCOM));
    wideCOM[strlen(szCOM)] = 0;
	//szCOM[3] = '0' + nPort;

	DWORD dwAccess = 0;
	if (bRead) dwAccess |= GENERIC_READ;
	if (bWrite) dwAccess |= GENERIC_WRITE;

    m_hCOM = ::CreateFile(wideCOM,
						  dwAccess,
						  0,
						  NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if (m_hCOM == INVALID_HANDLE_VALUE)
	{
		//DWORD errorValue = GetLastError();
		//sprintf (messageString, "Failed to open %s with error message %d", szCOM, errorValue);
		//AfxMessageBox (messageString);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

BOOL COMDevice::Close()
{
	char messageString[256];

	if (m_hCOM != INVALID_HANDLE_VALUE)
	{
		if (::CloseHandle(m_hCOM)) {
			m_hCOM = INVALID_HANDLE_VALUE;
			return TRUE;
		}
		
		sprintf (messageString, "Failed to close COM port");
        //AfxMessageBox (messageString);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////

BOOL COMDevice::SetConfiguration(LPCWSTR lpszConfiguration)
{
	DCB dcbState;
	char messageString[256];

//	SetCommMask(m_hCOM, EV_RXCHAR);
//	if (!SetupComm(m_hCOM, 4096, 4096)) {
//		AfxMessageBox ("Couldn't setup COM port with SetupComm");
//		return FALSE;
//	}

    if (!::GetCommState(m_hCOM, &dcbState))
	{
		sprintf (messageString, "Failed to get COM port state");
        //AfxMessageBox (messageString);
		return FALSE;
	}

	if (!::BuildCommDCB(lpszConfiguration, &dcbState))
	{
		sprintf (messageString, "Failed to build COM port device control block");
        //AfxMessageBox (messageString);
		return FALSE;
	}

	// some extra settings
//	dcbState.fOutxDsrFlow = dcbState.fOutxCtsFlow = FALSE;
//	dcbState.fDtrControl = DTR_CONTROL_ENABLE;
//	dcbState.fRtsControl = RTS_CONTROL_ENABLE;
//	dcbState.fInX = dcbState.fOutX = FALSE;
//	dcbState.XonChar = 0x11;
//	dcbState.XoffChar = 0x13;
//	dcbState.XonLim = 100;
//	dcbState.XoffLim = 100;
//	dcbState.Parity = 1;
//	dcbState.fParity = 1;

	if (!::SetCommState(m_hCOM, &dcbState))
	{
		sprintf (messageString, "Failed to set COM port state");
        //AfxMessageBox (messageString);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

BOOL COMDevice::SetTimeouts(DWORD dwReadInterval, DWORD dwReadTotal, DWORD dwWriteTotal)
{
	COMMTIMEOUTS commTimeouts;
	char messageString[256];
	
	commTimeouts.ReadIntervalTimeout = dwReadInterval; 
   commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.ReadTotalTimeoutConstant = dwReadTotal;
   commTimeouts.WriteTotalTimeoutMultiplier = 0;
	commTimeouts.WriteTotalTimeoutConstant = dwWriteTotal;

	if (!::SetCommTimeouts(m_hCOM, &commTimeouts))
	{
		sprintf (messageString, "Failed to set COM port timeouts");
        //AfxMessageBox (messageString);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

BOOL COMDevice::ClearOutputBuffer()
{
	return (::PurgeComm (m_hCOM, PURGE_TXCLEAR));
}

//////////////////////////////////////////////////////////////////////

int COMDevice::ReadPort(LPSTR lpBuffer, int nMaxLength)

{
   COMSTAT    ComStat;
   DWORD      dwErrorFlags, dwLength;

   ClearCommError(m_hCOM, &dwErrorFlags, &ComStat);

   dwLength = min((DWORD) nMaxLength, ComStat.cbInQue);

   if (dwLength > 0)
		return ::ReadFile(m_hCOM, lpBuffer, dwLength, &dwLength, NULL);
   else
		return 0;
}

//////////////////////////////////////////////////////////////////////

void COMDevice::ClearErrors()
{
   COMSTAT    ComStat;
   DWORD      dwErrorFlags;

   ClearCommError(m_hCOM, &dwErrorFlags, &ComStat);
}
