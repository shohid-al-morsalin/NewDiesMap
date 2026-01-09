// Serial.cpp
#include "stdafx.h"
#include "Serial.h"
BOOL CSerialReal::ClearRx()
{
	if (m_hIDComDev) {
		PurgeComm (m_hIDComDev, PURGE_RXABORT | PURGE_RXCLEAR);
		return TRUE;
	}
	return FALSE;
}

BOOL CSerialReal::Flush()
{
	if (m_hIDComDev) {
		FlushFileBuffers (m_hIDComDev);
		return TRUE;
	}
	return FALSE;
}

CSerialReal::CSerialReal()
{
	memset(& m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(& m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

CSerialReal::~CSerialReal()
{
	Close();
}

BOOL CSerialReal::Open(int nPort, int nBaud, char nPa, int nBit, int /*nStop*/)
{
	if (m_bOpened)
	{
		return (TRUE);
	}
	char szPort[15];
	DCB dcb;
	wsprintf(szPort, "COM%d", nPort);
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
	   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hIDComDev == NULL)
	{
		return (FALSE);
	}
	memset(& m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(& m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout =MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 5;//0;
//	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 5000;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 5;//0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(m_hIDComDev, & CommTimeOuts);
	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, & dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = nBit;
	//0-4=no,odd,even,mark,space
	switch (tolower(nPa))
	{
		case 'n':
			dcb.Parity = 0;
			break;
		case 'o':
			dcb.Parity = 1;
			break;
		case 'e':
			dcb.Parity = 2;
			break;
		case 'm':
			dcb.Parity = 3;
			break;
		case 's':
			dcb.Parity = 4;
			break;
	}
//	unsigned char ucSet;
//	ucSet = (unsigned char) ((FC_RTSCTS & FC_DTRDSR) != 0);
//	ucSet = (unsigned char) ((FC_RTSCTS & FC_RTSCTS) != 0);
//	ucSet = (unsigned char) ((FC_RTSCTS & FC_XONXOFF) != 0);
	if (!SetCommState(m_hIDComDev, & dcb) || !SetupComm(m_hIDComDev, 10000, 10000) || m_OverlappedRead.hEvent == NULL ||
	   m_OverlappedWrite.hEvent == NULL)
	   {
		   DWORD dwError = GetLastError();
		   if (m_OverlappedRead.hEvent != NULL)
		   {
			   CloseHandle(m_OverlappedRead.hEvent);
		   }
		   if (m_OverlappedWrite.hEvent != NULL)
		   {
			   CloseHandle(m_OverlappedWrite.hEvent);
		   }
		   CloseHandle(m_hIDComDev);
		   return (FALSE);
	}
	m_bOpened = TRUE;
	return (m_bOpened);
}

BOOL CSerialReal::Close(void)
{
	if (!m_bOpened || m_hIDComDev == NULL)
	{
		return (TRUE);
	}
	if (m_OverlappedRead.hEvent != NULL)
	{
		CloseHandle(m_OverlappedRead.hEvent);
	}
	if (m_OverlappedWrite.hEvent != NULL)
	{
		CloseHandle(m_OverlappedWrite.hEvent);
	}
	CloseHandle(m_hIDComDev);
	m_bOpened = FALSE;
	m_hIDComDev = NULL;
	return (TRUE);
}

BOOL CSerialReal::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	bWriteStat = WriteFile(m_hIDComDev, (LPSTR) & ucByte, 1, & dwBytesWritten, & m_OverlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_hIDComDev, & m_OverlappedWrite, & dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}
	return (TRUE);
}

int CSerialReal::SendData(const char * buffer, int size)
{
	if (!m_bOpened || m_hIDComDev == NULL)
	{
		return (0);
	}
	DWORD dwBytesWritten = 0;
	int i;
	for (i = 0; i < size; i++)
	{
		WriteCommByte(buffer[i]);
		Sleep(5);
		dwBytesWritten++;
	}
//	CString Str;
//	Str.Format("Send <%s> :%d\n",buffer,size);
//	TRACE(Str.GetBuffer(0));
	::FlushFileBuffers(m_hIDComDev);
	return ((int)dwBytesWritten);
}

int CSerialReal::ReadDataWaiting(void)
{
	if (!m_bOpened || m_hIDComDev == NULL)
	{
		return (0);
	}
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError(m_hIDComDev, & dwErrorFlags, & ComStat);
	return ((int)ComStat.cbInQue);
}

int CSerialReal::ReadData(void * buffer, int limit)
{
	if (!m_bOpened || m_hIDComDev == NULL)
	{
		return (0);
	}
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError(m_hIDComDev, & dwErrorFlags, & ComStat);
	if (!ComStat.cbInQue)
	{
		return (0);
	}
	dwBytesRead = (DWORD)ComStat.cbInQue;
	if (limit < (int)dwBytesRead)
	{
		dwBytesRead = (DWORD)limit;
	}
	bReadStatus = ReadFile(m_hIDComDev, buffer, dwBytesRead, & dwBytesRead, & m_OverlappedRead);
	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
			return ((int)dwBytesRead);
		}
		return (0);
	}
	return ((int)dwBytesRead);
}

BOOL CSerialReal::ReadByte(BYTE & by)
{
	if (!m_bOpened || m_hIDComDev == NULL)
	{
		return (0);
	}
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError(m_hIDComDev, & dwErrorFlags, & ComStat);
	if (!ComStat.cbInQue)
	{
		return (FALSE);
	}
	dwBytesRead = (DWORD)1;
	bReadStatus = ReadFile(m_hIDComDev, & by, dwBytesRead, & dwBytesRead, & m_OverlappedRead);
	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
		}
	}
	if (dwBytesRead)
	{
		return TRUE;
	};
	return FALSE;
}

int CSerialReal::ReadBlock(unsigned char * pBuf, int len)
{
	ASSERT(0); //TODO
	return 0;
}

