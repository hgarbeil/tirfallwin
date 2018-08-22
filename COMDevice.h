//////////////////////////////////////////////////////////////////////
//
//  COMDevice.h - Interface
//
//////////////////////////////////////////////////////////////////////

#pragma once

class COMDevice 
{
public:
	COMDevice();
	virtual ~COMDevice();

	BOOL Open(int nPort, BOOL bRead, BOOL bWrite);
	BOOL Close();

    BOOL SetConfiguration(LPCWSTR lpszConfiguration);
	BOOL SetTimeouts(DWORD dwReadInterval, DWORD dwReadTotal, DWORD dwWriteTotal);
	BOOL COMDevice::ClearOutputBuffer();
	int ReadPort(LPSTR lpszBlock, int nMaxLength);
	void ClearErrors();

	BOOL Read(LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesRead)
	{
		return ::ReadFile(m_hCOM, lpBuffer, dwBytesToRead, lpdwBytesRead, NULL);
	}

	BOOL Write(LPCVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten)
	{
		return ::WriteFile(m_hCOM, lpBuffer, dwBytesToWrite, lpdwBytesWritten, NULL);
	}

private:
	HANDLE m_hCOM;
};

//////////////////////////////////////////////////////////////////////
