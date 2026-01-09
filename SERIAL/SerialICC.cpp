// Serial.cpp
#include "stdafx.h"
#include "SerialICC.h"

CSerialICC::CSerialICC() {
	isConfocalRS232 = FALSE; // Mahedi Kamal 10172024
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

CSerialICC::~CSerialICC() {
	Close();
}

BOOL CSerialICC::Open(int nPort, int nBaud, char nPa, int nBit, int nStop) {
	if (m_bOpened) {
		return (TRUE);
	}
	char szPort[15];
	DCB dcb;
	wsprintf(szPort, "\\\\.\\COM%d", nPort);
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	// Commented/replaced @@Vijay
/*	if (m_hIDComDev == NULL)
	{
		return (FALSE);
	}*/
	if (m_hIDComDev == INVALID_HANDLE_VALUE) {
		return (FALSE);
	}
	// ... @@Vijay
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);
	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = nBit;
	//0-4=no,odd,even,mark,space
	switch (tolower(nPa)) {
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

	if (isConfocalRS232 == TRUE) { // Mahedi Kamal 10172024, for RS232 connection of confocal
		dcb.StopBits = ONESTOPBIT;
	}
	// 	unsigned char ucSet;
	// 	ucSet = (unsigned char) ((FC_RTSCTS & FC_DTRDSR) != 0);
	// 	ucSet = (unsigned char) ((FC_RTSCTS & FC_RTSCTS) != 0);
	// 	ucSet = (unsigned char) ((FC_RTSCTS & FC_XONXOFF) != 0);
	if (!SetCommState(m_hIDComDev, &dcb) || !SetupComm(m_hIDComDev, 16000, 16000) || m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL) {
		DWORD dwError = GetLastError();
		if (m_OverlappedRead.hEvent != NULL) {
			CloseHandle(m_OverlappedRead.hEvent);
		}
		if (m_OverlappedWrite.hEvent != NULL) {
			CloseHandle(m_OverlappedWrite.hEvent);
		}
		CloseHandle(m_hIDComDev);
		return (FALSE);
	}
	m_bOpened = TRUE;
	return (m_bOpened);
}

BOOL CSerialICC::Close(void) {
	if (!m_bOpened || m_hIDComDev == NULL) {
		return (TRUE);
	}
	if (m_OverlappedRead.hEvent != NULL) {
		CloseHandle(m_OverlappedRead.hEvent);
	}
	if (m_OverlappedWrite.hEvent != NULL) {
		CloseHandle(m_OverlappedWrite.hEvent);
	}
	CloseHandle(m_hIDComDev);
	m_bOpened = FALSE;
	m_hIDComDev = NULL;
	return (TRUE);
}

BOOL CSerialICC::WriteCommByte(unsigned char ucByte) {
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	bWriteStat = WriteFile(m_hIDComDev, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000)) {
			dwBytesWritten = 0;
		}
		else {
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}
	return (TRUE);
}

int CSerialICC::SendData(const char* buffer, int size) {
	if (!m_bOpened || m_hIDComDev == NULL) {
		return (0);
	}
	DWORD dwBytesWritten = 0;
	int i;
	for (i = 0; i < size; i++) {
		WriteCommByte(buffer[i]);
		//		Sleep(1);
		dwBytesWritten++;
	}
	return ((int)dwBytesWritten);
}

int CSerialICC::ReadData(void* buffer, int limit) {
	if (!m_bOpened || m_hIDComDev == NULL) {
		return (0);
	}
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) {
		return (0);
	}
	dwBytesRead = (DWORD)ComStat.cbInQue;
	if (limit < (int)dwBytesRead) {
		dwBytesRead = (DWORD)limit;
	}
	bReadStatus = ReadFile(m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead);
	if (!bReadStatus) {
		if (GetLastError() == ERROR_IO_PENDING) {
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
			return ((int)dwBytesRead);
		}
		return (0);
	}
	return ((int)dwBytesRead);
}

int CSerialICC::ReadDataWaiting(void) {
	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	return((int)ComStat.cbInQue);
}

BOOL CSerialICC::ReadByte(BYTE& by) {
	if (!m_bOpened || m_hIDComDev == NULL) {
		return (0);
	}
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) {
		return (FALSE);
	}
	dwBytesRead = (DWORD)1;
	bReadStatus = ReadFile(m_hIDComDev, &by, dwBytesRead, &dwBytesRead, &m_OverlappedRead);
	if (!bReadStatus) {
		if (GetLastError() == ERROR_IO_PENDING) {
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
		}
	}
	if (dwBytesRead) {
		return TRUE;
	};
	return FALSE;
}

// pMsg must be a valid null terminated string
int CSerialICC::Send(char* pMsg) {
	if (pMsg) {
		return SendData(pMsg, (int)strlen(pMsg));
	}
	return 0;
}