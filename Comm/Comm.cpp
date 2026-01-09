// Comm.cpp: implementation of the CComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Comm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD CommWatchProc(LPSTR lpData);


CComm::CComm()
{
	bPort = 1;
	wBaudRate = 9600;
	bBitSize = 8;
	bParity = NOPARITY;
	bStopBits = 1;
	bFlowCtrl = 0;
	fConnected = FALSE;
	ComDev = INVALID_HANDLE_VALUE;
	osRead.hEvent = NULL;
	osPost.hEvent = NULL;
	osWrite.hEvent = NULL;
}

CComm::~CComm()
{
	if (ComDev != INVALID_HANDLE_VALUE) Close ();
	if (osRead.hEvent) CloseHandle(osRead.hEvent);
	if (osWrite.hEvent) CloseHandle(osWrite.hEvent);
	if (osPost.hEvent) CloseHandle(osPost.hEvent);
}

BOOL CComm::Open()
{
	char szPort[8];
	COMMTIMEOUTS CommTimeOuts;

	wsprintf( szPort, "COM%d", bPort) ; 

	ComDev=CreateFile(szPort,GENERIC_READ | GENERIC_WRITE,
						0,						
						NULL,					
						OPEN_EXISTING,
						/*FILE_ATTRIBUTE_NORMAL |*/ FILE_FLAG_OVERLAPPED,
						NULL);
	if (ComDev == INVALID_HANDLE_VALUE) {
		//AfxMessageBox ("Cannot open comm port");
		return FALSE;
	}
	else {
		SetCommMask(ComDev,EV_RXCHAR);
		SetupComm(ComDev,1024,1024);
		CommTimeOuts.ReadIntervalTimeout=0xFFFFFFFF;
		CommTimeOuts.ReadTotalTimeoutMultiplier=0;
		CommTimeOuts.ReadTotalTimeoutConstant=0;
		CommTimeOuts.WriteTotalTimeoutMultiplier=0;
		CommTimeOuts.WriteTotalTimeoutConstant=5000;
		SetCommTimeouts(ComDev,&CommTimeOuts);
		if (SetupConnection()) {
			//if ((hCommWatchThread=CreateThread((LPSECURITY_ATTRIBUTES)NULL,
			//								 0,(LPTHREAD_START_ROUTINE)CommWatchProc,
			//								 (LPVOID)this,0,&dwThreadID))==NULL) {
			//	CloseHandle(ComDev);
			//	ComDev = INVALID_HANDLE_VALUE;
			//	return FALSE;
			//}
			//else {
			//	SetThreadPriority(hCommWatchThread,THREAD_PRIORITY_BELOW_NORMAL);
			//	ResumeThread(hCommWatchThread);
			//	EscapeCommFunction(ComDev,SETDTR);
			//}

		}
		else {
			CloseHandle(ComDev);
			ComDev = INVALID_HANDLE_VALUE;
			return FALSE;
		}

		return TRUE;
	}
}

BOOL CComm::Close ()
{
	if (ComDev != INVALID_HANDLE_VALUE) {
		SetCommMask(ComDev,0);
		PurgeComm(ComDev,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(ComDev);
		ComDev = INVALID_HANDLE_VALUE;
		return TRUE;
	}
	else return FALSE;
}


BOOL CComm::SetupConnection()
{
	BYTE bSet ;
	BOOL ret;
/*
	DCB		  dcb ;

	memset (&dcb, 0, sizeof (DCB));
	dcb.DCBlength=sizeof(DCB);
	dcb.BaudRate = wBaudRate;
	dcb.ByteSize = bBitSize;
	dcb.Parity = bParity;
	dcb.StopBits =bStopBits;

	bSet = (BYTE) ((bFlowCtrl & FC_DTRDSR) != 0) ;
	dcb.fOutxDsrFlow=bSet;
	if (bSet) dcb.fDtrControl=DTR_CONTROL_HANDSHAKE;
	else dcb.fDtrControl=DTR_CONTROL_ENABLE;

	bSet = (BYTE) ((bFlowCtrl & FC_RTSCTS) != 0) ;
	dcb.fOutxCtsFlow=bSet;
	if (bSet) dcb.fRtsControl=RTS_CONTROL_HANDSHAKE;
	else dcb.fRtsControl=RTS_CONTROL_ENABLE;

	bSet = (BYTE) ((bFlowCtrl & FC_XONXOFF) != 0) ;
	dcb.fInX = dcb.fOutX = bSet ;
	if (bSet) {
		dcb.XonChar = ASCII_XON ;
		dcb.XoffChar = ASCII_XOFF ;
		dcb.XonLim = 100 ;
		dcb.XoffLim = 100 ;
	}
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
*/

	DCB dcb ;
	char dcbstr[256];
	char parity[] ="NOE";
	memset (&dcb, 0, sizeof (dcb));
	sprintf_s (dcbstr, 
		"COM%d: baud=%d parity=%c data=%d stop=%d", bPort,
		wBaudRate,parity[bParity],bBitSize,bStopBits);
	if (BuildCommDCB(dcbstr, &dcb) < 0) return FALSE;

	bSet = (BYTE) ((bFlowCtrl & FC_DTRDSR) != 0) ;
	dcb.fOutxDsrFlow=bSet;
	if (bSet) dcb.fDtrControl=DTR_CONTROL_HANDSHAKE;
	else dcb.fDtrControl=DTR_CONTROL_ENABLE;

	bSet = (BYTE) ((bFlowCtrl & FC_RTSCTS) != 0) ;
	dcb.fOutxCtsFlow=bSet;
	if (bSet) dcb.fRtsControl=RTS_CONTROL_HANDSHAKE;
	else dcb.fRtsControl=RTS_CONTROL_ENABLE;

	bSet = (BYTE) ((bFlowCtrl & FC_XONXOFF) != 0) ;
	dcb.fInX = dcb.fOutX = bSet ;
	if (bSet) {
		dcb.XonChar = ASCII_XON ;
		dcb.XoffChar = ASCII_XOFF ;
		dcb.XonLim = 100 ;
		dcb.XoffLim = 100 ;
	}

	ret = SetCommState(ComDev, &dcb);
	if (!ret) {
		long error = GetLastError();
	}
	else {
		fConnected = TRUE;
	}
	return ret;
}


BOOL CComm::SetParam(BYTE Port, WORD BaudRate, BYTE BitSize, BYTE Parity, BYTE StopBits, BYTE FlowCtrl)
{
	bPort = Port;
	wBaudRate = BaudRate;
	bBitSize = BitSize;
	bParity = Parity;
	bStopBits = StopBits;
	bFlowCtrl= FlowCtrl;

	osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (osRead.hEvent==NULL) return FALSE;
	osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (NULL==osWrite.hEvent) {
		CloseHandle(osRead.hEvent);
		return FALSE;
	}
	osPost.hEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
	if (NULL==osPost.hEvent) {
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
			PostMessage(com->hWnd,WM_COMMNOTIFY,
							 (WPARAM)com->ComDev,
							 MAKELONG(CN_EVENT,0));
			}
		}
	// get rid of event handle
	CloseHandle(os.hEvent);
	return TRUE;
}



int CComm::Read (unsigned char * lpszBlock, int nMaxLength)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags,dwLength;
	char szError[10];

	BOOL ret = ClearCommError(ComDev,&dwErrorFlags,&ComStat);
	if (!ret) {
		long error = GetLastError();
	}
	if (dwErrorFlags>0) {
		wsprintf(szError,"<CE-%u>",dwErrorFlags);
		//return 0;
	} 
	dwLength=min((DWORD)nMaxLength,ComStat.cbInQue);
	if (dwLength>0) {
		int ret = ReadBlock (lpszBlock,dwLength);
		if (ret) {
			lpszBlock[ret] = 0;
			return ret;
		}
	}
	return 0;
}

BOOL CComm::WriteCh(unsigned char *ch)
{
	DWORD dwBytesWritten;
	BOOL fWriteStat;

	if (ComDev == INVALID_HANDLE_VALUE) return FALSE;

	fWriteStat=WriteFile(ComDev,(char *)ch, 1, &dwBytesWritten, &osWrite);
	if (!fWriteStat&&(GetLastError()==ERROR_IO_PENDING)) {
		// wait for a second for this transmission to complete
		if (WaitForSingleObject(osWrite.hEvent,1000)) {
			return FALSE ;  // time out
		}
		else {
			GetOverlappedResult(ComDev, &osWrite, &dwBytesWritten,TRUE);
			osWrite.Offset += dwBytesWritten ;
		}
	}		
	return TRUE; 
}

int CComm::Write (unsigned char * lpByte,int len)
{
	DWORD dwBytesWritten;
	BOOL fWriteStat;

	if (ComDev == INVALID_HANDLE_VALUE) return 0;

	fWriteStat=WriteFile(ComDev,(char *)lpByte,len, &dwBytesWritten,&osWrite);
	if (!fWriteStat&&(GetLastError()==ERROR_IO_PENDING)) {
		// wait for a second for this transmission to complete
		if (WaitForSingleObject(osWrite.hEvent,60000)) {
			dwBytesWritten = -1 ;  // time out
		}
		else {
			GetOverlappedResult(ComDev, &osWrite, &dwBytesWritten,TRUE);
			osWrite.Offset += dwBytesWritten ;
		}
	}		
	return dwBytesWritten; 
}

int CComm::ReadBlock (unsigned char * lpBuf,DWORD len)
{
	DWORD dwLength;
	BOOL fReadStat;
					
	if (ComDev == INVALID_HANDLE_VALUE) return 0;
	*lpBuf = 0;
	fReadStat=ReadFile(ComDev,lpBuf,len,&dwLength,&osRead);
	if (!fReadStat) {
		if (GetLastError() == ERROR_IO_PENDING) {
				// wait for a second for this transmission to complete
			if (WaitForSingleObject(osRead.hEvent, 60000)) {
				dwLength = -1 ;  // time out
			}
			else {
				GetOverlappedResult(ComDev, &osRead, &dwLength, TRUE);
				osRead.Offset+=dwLength ;
			}
		}
		else {
			dwLength = 0 ;
		}
	}
	return dwLength;
}			  

void CComm::ReadStr (unsigned char * lpstr, int nMaxLength, BYTE term)
{
	BYTE ch;
	unsigned char str[8];
	while (1) {
		ch = Read (str, 1);
		if (ch) {
			if (str[0] == term) { *(lpstr) = 0; break; }
			*(lpstr++) = str[0];
		}
	}
}

void CComm::SetCallnWnd(HWND hWd)
{
	hWnd = hWd;
}


BOOL CComm::WaitForRespond()
{
	OVERLAPPED  os ;
	DWORD dwEvtMask, dwTransfer;
	dwEvtMask=0;
	SetCommMask (ComDev, EV_RXCHAR);
	os.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (!WaitCommEvent(ComDev,&dwEvtMask,&os)) {
		if (ERROR_IO_PENDING==GetLastError()) {
			GetOverlappedResult(ComDev,&os,&dwTransfer,TRUE);
			os.Offset += dwTransfer;
		}
	}
	if ((dwEvtMask&EV_RXCHAR)==EV_RXCHAR) {
		CloseHandle(os.hEvent);
		return TRUE;
	}
	CloseHandle(os.hEvent);
	return FALSE;
}


void CComm::ClearRx()
{
	PurgeComm (ComDev, PURGE_RXABORT | PURGE_RXCLEAR);
}

void CComm::ClearTx()
{
	PurgeComm (ComDev, PURGE_TXABORT | PURGE_TXCLEAR);
}

void CComm::FlushTx()
{
	FlushFileBuffers (ComDev);
}

BOOL CComm::SendCommand(unsigned char *msg, int max)
{
	while (max--) 
	{
		if (Write (msg, 1) <= 0) 
		{
			return FALSE;
		}
		Sleep(20);
		msg++;
	}
	return TRUE;
}

BOOL CComm::GetRespondCR(unsigned char *msg, int max)
{
	max--;
	int timeout = 0;
	while (max--) {
		while (Read (msg, 1) < 1) {
			timeout++;
			if (timeout > 80) { *msg = 0; return FALSE; }
			Sleep (20);
		}
		timeout = 0;
		if (*msg == '\r') { msg++; *msg = 0; return TRUE; }
		msg++;
	}
	return FALSE;
}

BOOL CComm::GetRespondLF(unsigned char *msg, int max)
{
	max--;
	int timeout = 0;
	while (max--) {
		while (Read (msg, 1) < 1) {
			timeout++;
			if (timeout > 80) { *msg = 0; return FALSE; }
			Sleep (20);
		}
		timeout = 0;
		if (*msg == '\n') { msg++; *msg = 0; return TRUE; }
		msg++;
	}
	return FALSE;
}

void CComm::Wait (DWORD ms)
{
	DWORD t0 = GetTickCount();
	while (1) {
		if ((GetTickCount()-t0) > ms) return;
	}
}

void CComm::SetFlowControl(int ctrl)
{
	bFlowCtrl = unsigned char (ctrl);
}

BOOL CComm::GetResMitutoyo(unsigned char *str, int max)
{
	unsigned char massage[256];
	unsigned char *msg;

	msg = massage;
	max--;
	int timeout = 0;
	while (max--) {
		while (Read (msg, 1) < 1) {
			timeout++;
			if (timeout > 80) {
				// *(msg++)= '?'; *msg = 0;
				return FALSE;
			}
			Sleep (10);
		}
		if (*msg == '\r') { 
			msg++; *msg = 0;
			unsigned char *p = (unsigned char *)strchr((char *)massage, '+');
			if (p) {
				p++;
				strcpy((char *)str, (char *)p);
				return TRUE; 
			}
		}
		msg++;
	}
	return FALSE;
}


// Append CRNL and flush buffers.
BOOL CComm::Send(CString msg)
{
	msg += "\r\n";
	int len = msg.GetLength();
	if (Write ((unsigned char *)msg.GetBuffer(len), len) <= 0) return FALSE;
	FlushTx();
	return TRUE;
}


