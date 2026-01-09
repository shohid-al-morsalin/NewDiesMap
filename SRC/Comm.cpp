#include "stdafx.h"

//#include "SYS/Log.h"
#include "GlobalDeviceContainer.h"

#include "Comm.h"

CComm::CComm() {
	byPort = 1;
	wBaudRate = 9600;
	bBitSize = 8;
	bParity = NOPARITY;
	bStopBits = 1;
	bFlowCtrl = 0;
	bConnected = FALSE;
	//fConnected = FALSE;
	//@@Vijay
	ZeroMemory(&osRead, sizeof(osRead));
	ZeroMemory(&osPost, sizeof(osPost));
	ZeroMemory(&osWrite, sizeof(osPost));
	//...Vijay
	ComDev = INVALID_HANDLE_VALUE;
	osRead.hEvent = NULL;
	osPost.hEvent = NULL;
	osWrite.hEvent = NULL;

	ErrorStr = "";
}

CComm::~CComm() {
	if (ComDev != INVALID_HANDLE_VALUE) Close();
	if (osRead.hEvent) CloseHandle(osRead.hEvent);
	if (osWrite.hEvent) CloseHandle(osWrite.hEvent);
	if (osPost.hEvent) CloseHandle(osPost.hEvent);
}

BOOL CComm::Open() {
	char szPort[26];
	COMMTIMEOUTS CommTimeOuts;

	wsprintf(szPort, "\\\\.\\COM%d", byPort);

	ComDev = CreateFile(szPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (ComDev == INVALID_HANDLE_VALUE) {
		CString msg;
		DWORD Err = GetLastError();
		msg.Format("Fail to open comm port %d", byPort);
		Log(msg, 0);
		return FALSE;
	}

	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts(ComDev, &CommTimeOuts);

	CString msg;
	if (SetupConnection()) {
		if (SetupComm(ComDev, 16000, 16000)) {
			SetCommMask(ComDev, EV_RXCHAR);
			bConnected = TRUE;
			msg.Format("Connected to port %d", byPort);
			Log(msg, 2);
			return TRUE;
		}
	}
	msg.Format("Fail to setup connection for port %d", byPort);
	Log(msg, 0);
	CloseHandle(ComDev);
	ComDev = INVALID_HANDLE_VALUE;
	return FALSE;
}

BOOL CComm::Close() {
	CString msg;
	if (ComDev != INVALID_HANDLE_VALUE) {
		SetCommMask(ComDev, 0);
		PurgeComm(ComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(ComDev);
		ComDev = INVALID_HANDLE_VALUE;
		bConnected = FALSE;
		msg.Format("Comm port %d closed", byPort);
		Log(msg, 2);
		return TRUE;
	}
	else return FALSE;
}

BOOL CComm::SetupConnection() {
	BYTE bSet;
	BOOL ret;
	DCB dcb;
	char dcbstr[256];
	char parity[] = "NOE";
	memset(&dcb, 0, sizeof(dcb));
	sprintf(dcbstr,
		"baud=%d parity=%c data=%d stop=%d",
		//		"COM%d: baud=%d parity=%c data=%d stop=%d", byPort,
		wBaudRate, parity[bParity], bBitSize, bStopBits);
	Log(dcbstr, 2);
	if (BuildCommDCB(dcbstr, &dcb) < 0) return FALSE;

	bSet = (BYTE)((bFlowCtrl & FC_DTRDSR) != 0);
	dcb.fOutxDsrFlow = bSet;
	if (bSet) dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	else dcb.fDtrControl = DTR_CONTROL_ENABLE;

	bSet = (BYTE)((bFlowCtrl & FC_RTSCTS) != 0);
	dcb.fOutxCtsFlow = bSet;
	if (bSet) dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	else dcb.fRtsControl = RTS_CONTROL_ENABLE;

	bSet = (BYTE)((bFlowCtrl & FC_XONXOFF) != 0);
	dcb.fInX = dcb.fOutX = bSet;
	if (bSet) {
		dcb.XonChar = ASCII_XON;
		dcb.XoffChar = ASCII_XOFF;
		dcb.XonLim = 100;
		dcb.XoffLim = 100;
	}

	ret = SetCommState(ComDev, &dcb);
	if (!ret) {
		long error = GetLastError();
	}
	return ret;
}

BOOL CComm::SetParam(BYTE Port, WORD BaudRate, BYTE BitSize, BYTE Parity, BYTE StopBits, BYTE FlowCtrl) {
	byPort = Port;
	wBaudRate = BaudRate;
	bBitSize = BitSize;
	bParity = Parity;
	bStopBits = StopBits;
	bFlowCtrl = FlowCtrl;

	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osRead.hEvent == NULL) return FALSE;
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == osWrite.hEvent) {
		CloseHandle(osRead.hEvent);
		return FALSE;
	}
	osPost.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (NULL == osPost.hEvent) {
		CloseHandle(osRead.hEvent);
		CloseHandle(osWrite.hEvent);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////
/*
DWORD CommWatchProc(LPSTR lpData)
{
	DWORD dwEvtMask,dwTransfer;
	CComm *com =(CComm *)lpData;
	OVERLAPPED  os ;

	 os.Internal=os.InternalHigh=os.Offset=os.OffsetHigh=0;
	// create I/O event used for overlapped read
	if (NULL==(os.hEvent=CreateEvent(NULL,  // no security
									 TRUE,  // explicit reset req
									 FALSE, // initial event reset
									 NULL))) // no name
		return (FALSE);

	if (!SetCommMask(com->ComDev,EV_RXCHAR)) return (FALSE);
	while (com->fConnected) {
		dwEvtMask=0;
		if (!WaitCommEvent(com->ComDev,&dwEvtMask,&os)) {
			if (ERROR_IO_PENDING==GetLastError()) {
				GetOverlappedResult(com->ComDev,&os,&dwTransfer,TRUE);
				os.Offset += dwTransfer;
			}
		}
		if ((dwEvtMask&EV_RXCHAR)==EV_RXCHAR)
			{
			// wait for "posted notification" flag to clear
			WaitForSingleObject(com->osPost.hEvent,60000);
			// reset event
			ResetEvent(com->osPost.hEvent);
			// last message was processed,O.K. to post
			PostMessage(com->hWnd,WM_COMMNOTIFY,  (WPARAM)com->ComDev, MAKELONG(CN_EVENT,0));
			}
		}
	// get rid of event handle
	CloseHandle(os.hEvent);
	return TRUE;
}
*/

// Yuen: Do not lock [2002/11/03  11:33]
int CComm::_Read(char* lpszBlock, int nMaxLength) {
	COMSTAT ComStat;
	DWORD dwErrorFlags, dwLength;
	char szError[10];

	BOOL ret = ClearCommError(ComDev, &dwErrorFlags, &ComStat);
	if (!ret) {
		long error = GetLastError();
	}
	if (dwErrorFlags > 0) {
		wsprintf(szError, "<CE-%u>", dwErrorFlags);
	}
	//	dwLength=min((DWORD)nMaxLength,ComStat.cbInQue);
	dwLength = nMaxLength;
	if (dwLength > 0) {
		/*int */ret = ReadBlock(lpszBlock, dwLength);
		if (ret) {
			lpszBlock[ret] = 0;
			return ret;
		}
	}
	return 0;
}

BOOL CComm::WriteCh(char* ch) {
	DWORD dwBytesWritten;
	BOOL fWriteStat;

	if (ComDev == INVALID_HANDLE_VALUE) return FALSE;

	fWriteStat = WriteFile(ComDev, (char*)ch, 1, &dwBytesWritten, &osWrite);
	if (!fWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		// wait for a second for this transmission to complete
		if (WaitForSingleObject(osWrite.hEvent, 1000)) {
			return FALSE;  // time out
		}
		else {
			GetOverlappedResult(ComDev, &osWrite, &dwBytesWritten, TRUE);
			osWrite.Offset += dwBytesWritten;
		}
	}
	return TRUE;
}

// Yuen: Do not lock [2002/11/03  11:33]
int CComm::Write(char* lpByte, int len) {
	DWORD dwBytesWritten;
	BOOL fWriteStat;

	if (ComDev == INVALID_HANDLE_VALUE) return 0;

	fWriteStat = WriteFile(ComDev, (char*)lpByte, len, &dwBytesWritten, &osWrite);
	DWORD Err = GetLastError();
	if (!fWriteStat) {
		if (Err == ERROR_IO_PENDING) {
			// wait for a second for this transmission to complete
			if (WaitForSingleObject(osWrite.hEvent, 60000)) {
				dwBytesWritten = -1;  // time out
			}
			else {
				GetOverlappedResult(ComDev, &osWrite, &dwBytesWritten, TRUE);
				osWrite.Offset += dwBytesWritten;
			}
		}
		else {
			char* lpBuf = NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				Err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpBuf,
				0, NULL);
			//			Log(_T(lpBuf),2);
			LocalFree(lpBuf);
		}
	}
	else {
		// Write was successful ! Do not worry !
	}
	return dwBytesWritten;
}

int CComm::ReadBlock(char* lpBuf, DWORD len) {
	DWORD dwLength;
	BOOL fReadStat;

	if (ComDev == INVALID_HANDLE_VALUE) return 0;
	*lpBuf = 0;
	fReadStat = ReadFile(ComDev, lpBuf, len, &dwLength, &osRead);
	if (!fReadStat) {
		if (GetLastError() == ERROR_IO_PENDING) {
			// wait for a second for this transmission to complete
			if (WaitForSingleObject(osRead.hEvent, 60000)) {
				dwLength = -1;  // time out
			}
			else {
				GetOverlappedResult(ComDev, &osRead, &dwLength, TRUE);
				osRead.Offset += dwLength;
			}
		}
		else {
			dwLength = 0;
		}
	}
	return dwLength;
}

// Yuen: Do not lock [2002/11/03  11:35]
void CComm::ReadStr(char* lpstr, int nMaxLength, BYTE term) {
	BYTE ch;
	char str[8];
	while (1) {
		ch = _Read(str, 1);
		if (ch) {
			if (str[0] == term) {
				*(lpstr) = 0;
				break;
			}
			*(lpstr++) = str[0];
		}
	}
}

void CComm::SetCallnWnd(HWND hWd) {
	hWnd = hWd;
	CString msg;
	msg.Format("Callback window %ld for port %d set", hWnd, byPort);
	Log(msg, 2);
}

BOOL CComm::WaitForRespond() {
	OVERLAPPED  os;
	DWORD dwEvtMask, dwTransfer;
	dwEvtMask = 0;
	SetCommMask(ComDev, EV_RXCHAR);
	os.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!WaitCommEvent(ComDev, &dwEvtMask, &os)) {
		if (ERROR_IO_PENDING == GetLastError()) {
			GetOverlappedResult(ComDev, &os, &dwTransfer, TRUE);
			os.Offset += dwTransfer;
		}
	}
	if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) {
		CloseHandle(os.hEvent);
		return TRUE;
	}
	CloseHandle(os.hEvent);
	return FALSE;
}

// Yuen: Do not lock [2002/11/03  11:33]
void CComm::ClearRx() {
	PurgeComm(ComDev, PURGE_RXABORT | PURGE_RXCLEAR);
}

// Yuen: Do not lock [2002/11/03  11:33]
void CComm::ClearTx() {
	PurgeComm(ComDev, PURGE_TXABORT | PURGE_TXCLEAR);
}

// Yuen: Do not lock [2002/11/03  11:33]
void CComm::FlushTx() {
	FlushFileBuffers(ComDev);
}

BOOL CComm::SendScl(char* msg, short max) {
	ErrorStr = "";
	CSingleLock Lock(&CS);
	Lock.Lock();
	while (max--) {
		if (Write(msg, 1) <= 0) {
			ErrorStr.Format("SendScl: Fail to send %s", msg);
			Log(ErrorStr, 0);
			return FALSE;
		}
		Sleep(20);
		msg++;
	}
	return TRUE;
}

//LYF: Note this command should only be use for short duration serial exchange
BOOL CComm::GetRespondCR(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				// Char timeout occured [11/2/2002]
				*msg = 0;
				return FALSE;
			}
			// YFL: Require to solve buffer overrun during LP read slotmap [3/3/2005]
			Sleep(20);  // change from 1 to 20 [12/2/2007 YUEN]
// YFL:  [3/3/2005]
//			Sleep (20);
		}
		tick = GetTickCount64();
		if (*msg == '\r') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		if (*msg != '\n') {
			msg++;
		}
	}
	*msg = 0;
	return FALSE;
}

//LYF: Note this command should only be use for short duration serial exchange
BOOL CComm::GetRespondLF(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				*msg = 0;
				return FALSE;
			}
			// YFL: Require to solve buffer overrun during LP read slotmap [3/3/2005]
			Sleep(20); // Change from 1 to 20 [12/2/2007 YUEN]
// YFL:  [3/3/2005]
//			Sleep (20);
		}
		tick = GetTickCount64();
		if (*msg == '\n') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		if (*msg != '\r') {
			msg++;
		}
	}
	*msg = 0;
	return FALSE;
}

void CComm::Wait(DWORD ms) {
	ULONGLONG t0 = GetTickCount64();
	while (1) {
		if ((GetTickCount64() - t0) > ms) return;
	}
}

void CComm::SetFlowControl(short ctrl) {
	bFlowCtrl = char(ctrl);
}

BOOL CComm::GetResMitutoyo(char* str, short max) {
	char message[RESPLEN];
	char* msg = &message[0];

	CSingleLock Lock(&CS);
	Lock.Lock();
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				*msg = 0;
				return FALSE;
			}
			Sleep(20);
		}
		tick = GetTickCount64();
		if (*msg == '\r') {
			msg++; *msg = 0;
			char* p = (char*)strchr((char*)message, '+');
			if (p) {
				p++;
				strcpy((char*)str, (char*)p);
				return TRUE;
			}
		}
		msg++;
	}
	return FALSE;
}

// Append CRNL and flush buffers.
BOOL CComm::_Send(CString& msg) {
	// Do not use critical section
	msg += "\r\n";
	short len = msg.GetLength();
	if (Write((char*)msg.GetBuffer(len), len) <= 0) {
		return FALSE;
	}
	//	FlushTx();
	//	PurgeComm (ComDev, PURGE_TXABORT | PURGE_TXCLEAR);
	//	TRACE("->");
	//	TRACE(msg);
	return TRUE;
}

// Append CRNL and flush buffers.
BOOL CComm::_Send2(CString msg) {
	// Do not use critical section
	msg += "\r\n";
	short len = msg.GetLength();
	if (Write((char*)msg.GetBuffer(len), len) <= 0) {
		return FALSE;
	}
	//	FlushTx();
	//	PurgeComm (ComDev, PURGE_TXABORT | PURGE_TXCLEAR);
	//	TRACE(msg);
	return TRUE;
}

BOOL CComm::SendBrk(/*char **/CString text, DWORD tout) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespBrk(&resp[0], RESPLEN, tout)) {
			if (strstr((char*)resp, "_ERR")) return FALSE;
			if (strstr((char*)resp, "Error")) return FALSE; //20120924 SCOTT ADD
			if (strstr((char*)resp, "error")) return FALSE; //20120924 SCOTT ADD
			return TRUE;
		}
		else {
			strcpy((char*)&resp[0], "Reply timeout");
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}
//[Tushar 21082024
BOOL CComm::SendLogosol(/*char **/CString text) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespLogosol(&resp[0], RESPLEN)) {
			if (strstr((char*)resp, "?")) return FALSE;
			return TRUE;
		}
		else {
			strcpy((char*)&resp[0], "Reply timeout");
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}

BOOL CComm::SendLogosolTest(/*char **/CString text) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespLogosolTest(&resp[0], RESPLEN, text)) {
			if (strstr((char*)resp, "?")) return FALSE;
			return TRUE;
		}
		else {
			strcpy((char*)&resp[0], "Reply timeout");
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}

BOOL CComm::SendLogosolAction(/*char **/CString text) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespLogosol_Action(&resp[0], RESPLEN)) {
			return TRUE;
		}
		else {
			strcpy((char*)&resp[0], strcat(text.GetBuffer(0), " command error"));
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}

BOOL CComm::GetRespLogosol(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				*msg = 0;
				return FALSE;
			}
			Sleep(2);
		}
		tick = GetTickCount64();
		if (*msg == '>') {
			max++;
			*msg = 0;
			return TRUE;
		}
		msg++;
	}
	*msg = 0;
	return FALSE;
}

//BOOL CComm::GetRespLogosolTest(char* msg, short max) {
//	CString response = "";
//	max--;  // Decrement max to leave space for null terminator.
//	ULONGLONG tick = GetTickCount64();
//	bool foundStart = false;  // Flag to check if '>' has been found.
//	int charsAfterStart = 0;  // Counter for characters after '>'
//
//	char* start = msg;  // Keep track of the start of the buffer.
//
//	while (max--) {
//		while (_Read(msg, 1) < 1) {  // Try to read a single character.
//			
//			if ((GetTickCount64() - tick) > 240000) {  // Check for timeout (240 seconds).
//				
//				*msg = 0;  // Null-terminate the string.
//				return FALSE;  // Timeout occurred.
//			}
//			TRACE("The Response is: %d\n", msg);
//			Sleep(2);  // Sleep to prevent busy-waiting.
//		}
//		TRACE("The Response is: %d\n", msg);
//		tick = GetTickCount64();  // Reset tick for the next character read.
//
//		if (*msg == '>') {
//			foundStart = true;  // Mark that the starting character '>' has been found.
//			charsAfterStart = 0;  // Reset counter after finding '>'.
//			response += *msg;  // Start the response CString with '>'.
//		}
//
//		else if (*msg == '?') {
//			return false;
//		}
//		/*if (*msg == '\r') {
//			max++;
//			continue;
//		}
//		else if (*msg == '\n') {
//			max++;
//			continue;
//		}*/
//
//		//if (*msg != '?') {
//		//	foundStart = TRUE;  // Mark that the starting character '>' has been found.
//		//	charsAfterStart = 0;  // Reset counter after finding '>'.
//		//	response += *msg;  // Start the response CString with '>'.
//		//}
//		//else {
//		//	return FALSE;
//		//}
//		
//
//		if (foundStart) {
//			// If '>' is found, store characters in response CString.
//			if (charsAfterStart < 6) {  // Read the next 4 characters after '>'.
//				response += *msg;  // Append character to the response.
//				charsAfterStart++;
//			}
//
//			if (charsAfterStart == 6) {  // After reading '>' + 4 characters, stop.
//				msg++;  // Move to the next position for null-terminating.
//				break;
//			}
//		}
//
//		msg++;  // Move to the next position in the buffer.
//	}
//
//	*msg = 0;  // Null-terminate the string.
//	TRACE("The Response is: %s\n", response);
//	return foundStart;  // Return TRUE if '>' was found, FALSE otherwise.
//}

BOOL CComm::GetRespLogosolTest(char* msg, short max, CString cmd) {
	max--;
	ULONGLONG tick = GetTickCount64();
	std::string responseString;
	while (max--) {
		if (cmd != "DOC\r\n") {
			ULONGLONG readStartTime = GetTickCount64();
			while (_Read(msg, 1) < 1) {
				if ((GetTickCount64() - tick) > 240000) {
					*msg = 0;
					return FALSE;
				}
				Sleep(2);
			}
			tick = GetTickCount64();
			if (*msg == '?') { // If response is '?'
				*msg = 0;
				return FALSE;
			}
			if (*msg == '>') { // If response is '>'
				max++;
				*msg = 0;
				// Check 10 consecutive responses
				char responses[10];
				bool zeroDetected = false;
				for (int i = 0; i < 10; i++) {
					readStartTime = GetTickCount64();
					while (_Read(&responses[i], 1) < 1) {
						if ((GetTickCount64() - readStartTime) > 240000) {  // 60-second timeout for _Read()
							return FALSE;
						}
						Sleep(2);
					}
					if (responses[i] == '0') {
						zeroDetected = true;
						i=9;
					} 
				}
				if (zeroDetected) {
					// Check next 3 characters after zero detection
					char next3[4];
					for (int i = 0; i < 3; i++) {
						readStartTime = GetTickCount64();
						while (_Read(&next3[i], 1) < 1) {
							if ((GetTickCount64() - readStartTime) > 60000) {  // 60-second timeout for _Read()
								return FALSE;
							}
							Sleep(2);
						}
					}
					// If all three are '0', return TRUE (command executed successfully)
					if (next3[0] == '0' && next3[1] == '0' && next3[2] == '0') {
						return TRUE;
					}
					else { // If any of them are non-zero, return FALSE
						return FALSE;
					}
				}
				return TRUE; // If no '0' was detected
			}
			if (*msg == '\r' || *msg == '\n') { // If response is '\r' or '\n'
				max++;
				continue;
			}
		}
		else if (cmd == "DOC\r\n") {
			ULONGLONG readStartTime = GetTickCount64();
			while (_Read(msg, 1) < 1) {
				if ((GetTickCount64() - readStartTime) > 60000) {  // 60-second timeout for _Read()
					*msg = 0;
					return FALSE;
				}
				Sleep(2);
			}
			if (*msg == '1') {
				return TRUE;
			}
			else if (*msg == '0') {
				return FALSE;
			}
			else {
				max++;
				continue;
			}
		}
		// Store input in a string until '\r' or '\n' is found
		responseString += *msg;
		msg++;
	}
	*msg = 0;
	return FALSE;
}

BOOL CComm::GetRespLogosol_Action(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 60000) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 60000) {
				*msg = 0;
				return FALSE;
			}
			Sleep(2);
		}
		tick = GetTickCount64();
		if (strstr(msg, "0000")) {
			return TRUE;
		}
		msg++;
	}
	return FALSE;

	
}

//]

BOOL CComm::SendPri(/*char **/CString text) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespPri(&resp[0], RESPLEN)) {
			if (strstr((char*)resp, "0:")) return TRUE;
			return FALSE;
		}
		else {
			strcpy((char*)&resp[0], "Reply timeout");
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}

BOOL CComm::SendPri2(/*char **/CString text) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(text)) {
		if (GetRespPri2(&resp[0], RESPLEN)) {
			if (strstr((char*)resp, "0:")) return TRUE;
			return FALSE;
		}
		else {
			strcpy((char*)&resp[0], "Reply timeout");
		}
	}
	else {
		strcpy((char*)&resp[0], "Fail to send command");
	}
	return FALSE;
}

// Yuen: Do not lock [2002/11/03  11:34]
BOOL CComm::GetRespBrk(char* msg, short max, DWORD tout) {
	char* p = msg;
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > tout) {
				*msg = 0;
				return FALSE;
			}
			Sleep(20); // Change from 1 to 20. Purpose is to allow other thread has more time to run, may eliminate 'Server busy' issue [12/2/2007 YUEN]
// YFL:  [3/3/2005]
//			Sleep (20);
		}
		tick = GetTickCount64();
		if (*msg == '\r') {
			max++;
			continue;
		}
		else if (*msg == '\n') {
			max++;
			continue;
		}
		if (*msg == ':') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		msg++;
	}
	*msg = 0;
	return FALSE;
}

BOOL CComm::GetRespPri(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				*msg = 0;
				return FALSE;
			}
			Sleep(2);
		}
		tick = GetTickCount64();
		if (*msg == '>') {
			max++;
			continue;
		}
		if (*msg == '\r') {
			max++;
			continue;
		}
		else if (*msg == '\n') {
			max++;
			continue;
		}
		if (*msg == ':') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		msg++;
	}
	*msg = 0;
	return FALSE;
}

BOOL CComm::GetRespPri2(char* msg, short max) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				*msg = 0;
				return FALSE;
			}
			Sleep(2);
		}
		tick = GetTickCount64();
		if (*msg == '>') {
			max++;
			*msg = 0;
			return TRUE;
		}
		if (*msg == '\r') {
			max++;
			continue;
		}
		else if (*msg == '\n') {
			max++;
			continue;
		}
		msg++;
	}
	*msg = 0;
	return FALSE;
}

BOOL CComm::SendAck(CString msg, DWORD tout) {
	return SendHCA(msg, tout);
}

// Yuen: Do not lock [2002/11/03  11:34]
BOOL CComm::GetRespLFFoup(char* msg, short max, DWORD tout) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > tout) {
				// inter-char timeout occured
				*msg = 0;
				return FALSE;
			}
			Sleep(20); // change from 1 to 20 [12/2/2007 YUEN]
		}
		tick = GetTickCount64();
		if (*msg == '\n') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		else if (*msg == '\r') {
			// sink this char
//			msg++;
//			*msg = 0;
//			return TRUE;
		}
		msg++;
	}
	// incomplete message
	*msg = 0;
	return FALSE;
}

// Yuen: Do not lock [2002/11/03  11:34]
BOOL CComm::GetRespCR(char* msg, short max, DWORD tout) {
	max--;
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > tout/*240000*/) {
				// char timeout has occured
//				Log("Inter-character timeout",1);
				*msg = 0;
				return FALSE;
			}
			Sleep(20);  // change from 1 to 20 [12/2/2007 YUEN]
// YFL:  [3/3/2005]
//			Sleep (20);
		}
		tick = GetTickCount64();
		if (*msg == '\r') {
			msg++;
			*msg = 0;
			return TRUE;
		}
		if (*msg != '\n') {
			msg++;
		}
	}
	*msg = 0;
	return FALSE;
}

BOOL CComm::RequestTDK(CString msg, char* text, short max) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send2(msg)) {
		if (GetRespCR(text, max, 240000)) {
			strcpy((char*)resp, (char*)text);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CComm::SendHCA(CString msg, DWORD tout) {
	int cnt = 0;
	CSingleLock Lock(&CS);
	Lock.Lock();
AGAIN:
	ClearRx();
	if (_Send(msg)) {
		Sleep(50); // must wait
		if (GetRespLFFoup(&resp[0], RESPLEN, tout)) {
			if (strstr((char*)resp, "HCA RMIDA NO")) return TRUE;
			if (strstr((char*)resp, "HCA NO_POD")) return FALSE;
			if (strstr((char*)resp, "ALARM")) return FALSE;
			if (strstr((char*)resp, "DENIED")) return FALSE;
			if (strstr((char*)resp, "BUSY")) {
				if (cnt > 30) return FALSE;
				Sleep(500);
				cnt++;
				goto AGAIN;
			}
			return TRUE;
		}
		strcpy((char*)resp, "Error: SendHCA No response");
	}
	else {
		strcpy((char*)resp, "Error: SendHCA No send");
	}
	return FALSE;
}

BOOL CComm::SendKEY(CString msg) {
#ifdef _TEST
	return TRUE;
#endif

	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send2(msg)) {
		if (GetRespCR(&resp[0], RESPLEN, 240000)) {
			if (GetRespCR(&resp[0], RESPLEN, 240000)) {
				//	if (strstr((char *)resp,"HCA RMIDA NO")) return TRUE;
				//	if (strstr((char *)resp,"ALARM")) return FALSE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComm::SendOmron(CString msg) {
#ifdef _TEST
	return TRUE;
#endif

	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send2(msg)) {
		if (GetRespCR(&resp[0], RESPLEN, 240000)) {
			return TRUE;
		}
	}
	return FALSE;
}

// Yuen: Do not lock [2002/11/03  11:36]
short CComm::ReadChr(char* ch) {
	return _Read((char*)ch, 1);
}

int CComm::GetComPortNo() {
	return byPort;
}

BOOL CComm::SendFSMSignalTower(char Cmd) {
	CString Message;
	if (!WriteCh((char*)(&Cmd))) {
		Message.Format("Fail to send <%c> to Signal Tower", Cmd);
		Log(Message, 2);
		return FALSE;
	}

	static char Buffer[125] = { 0 };
	ULONGLONG tick = GetTickCount64();
	ULONGLONG ntick = tick;
	char* ch = Buffer;
	while (1) {
		if (!ReadBlock(ch, 1)) {
			ntick = GetTickCount64();
			if (ntick - tick > 100) {
				break;	// intended inter char timeout to flush echo from interface card
			}
			Sleep(1);
			continue;
		}
		ch++; *ch = 0;
		tick = ntick;
	}

	// Safeguard if string is not null terminated
	Buffer[124] = NULL;
	if (Buffer[0] == Cmd) {
		return TRUE;
	}
	return FALSE;
}

BOOL CComm::Send(char* msg, short max) {
	while (max--) {
		if (Write(msg, 1) <= 0) return FALSE;
		Sleep(2);
		msg++;
	}
	return TRUE;
}

BOOL CComm::SendCommandEcho(char* msg, short max) {
	char Ret = 0;
	char Res[128] = { 0 };
	short Cnt = 0;
	//	::FlushFileBuffers(ComDev);
	while (max--) {
		if (Write(msg, 1) <= 0) {
			return FALSE;
		}
		::Sleep(20);
		while (((*msg) != Ret)) {
			if (ReadChr(&Ret)) {
				Res[Cnt++] = Ret;
			}
			else {
				::Sleep(20);
			}
		}
		msg++;
	}
	return TRUE;
}

BOOL CComm::SendTDK(CString msg, int Rpy) {
	msg += "\r";
	//LYF
	CSingleLock Lock(&CS);
	Lock.Lock();
	BOOL ret = FALSE;
	ret = Send((char*)msg.GetBuffer(0), msg.GetLength());
	while (Rpy--) {
		char dummy[512];
		GetRespCR(dummy, 511, 240000);
	}
	return ret;
}

BOOL CComm::ReadResp(char* msg, short max) {
	ULONGLONG tick = GetTickCount64();
	while (max--) {
		while (_Read(msg, 1) < 1) {
			if ((GetTickCount64() - tick) > 240000) {
				// char timeout has occurred
				*msg = 0;
				return FALSE;
			}
			Sleep(20); // Change from 1 to 20  [12/2/2007 YUEN]
		}
		tick = GetTickCount64();
		msg++;
	}
	*msg = 0;
	return TRUE;
}

BOOL CComm::IsOpened() {
	return bConnected;
}

BOOL CComm::RequestFoup(CString msg, char* text, short max, DWORD tout) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(msg)) {
		Sleep(50);
		if (GetRespLFFoup(text, max, tout)) {
			strcpy((char*)resp, (char*)text);
			return TRUE;
		}
	}
	return FALSE;
}

void CComm::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CComm");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CComm::QuerryData(CString& Cmd) {
	if (!SendCommandEcho(Cmd.GetBuffer(0), Cmd.GetLength())) {
		return FALSE;
	}
	Sleep(160);
	char re[32] = { 0 };
	ReadBlock((char*)re, 32);
	Cmd = re;
	return TRUE;
}

BOOL CComm::Request(CString msg, char* text, short max, DWORD tout) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	ClearRx();
	if (_Send(msg)) {
		if (GetRespLFFoup(text, max, tout)) {
			strcpy((char*)resp, (char*)text);
			return TRUE;
		}
	}
	return FALSE;
}