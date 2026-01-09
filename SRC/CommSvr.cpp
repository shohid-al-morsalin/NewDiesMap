#include "stdafx.h"
#include "CommSvr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCommSvr, CWinThread)

CCommSvr::CCommSvr() {
	port = 0;
	buff = "";

	pCom = NULL;
	HWin = NULL;
	Ack = 0;

	bRun = TRUE;
	bSimu = FALSE;
	bStarted = FALSE;
}

CCommSvr::~CCommSvr() {
	StopUnit();
	POSITION pos;

	pos = InStr.GetHeadPosition();
	while (pos) {
		ComMsg* p;
		p = (ComMsg*)InStr.GetNext(pos);
		if (p) {
			delete p;
		}
	}
}

BOOL CCommSvr::InitInstance() {
	m_bAutoDelete = FALSE;
	return TRUE;
}

int CCommSvr::ExitInstance() {
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCommSvr, CWinThread)
	//{{AFX_MSG_MAP(CCommSvr)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommSvr message handlers

BOOL CCommSvr::Stop() {
	bRun = FALSE;
	m_bAutoDelete = FALSE;
	return FALSE;
}

BOOL CCommSvr::WriteList(CString& str) {
	CSingleLock Lock(&CSOut);
	Lock.Lock();
	OutStr.AddTail(str);
	if (HWin) {
		::PostMessage(HWin, WM_COMMAND, Ack, NULL);
	}
	return TRUE;
}

// This function will be called from other thread only
BOOL CCommSvr::WriteString(char* str, int tout) {
	// Do not use critical section
	if (pCom) {
		pCom->SendAck(str, tout);
		strcpy(str, (char*)pCom->resp);
		return TRUE;
	}
	str = "";
	return FALSE;
}

int CCommSvr::Run() {
	BOOL bBusy = FALSE;

	if (!StartUnit()) return ExitInstance();

	if (!pCom) {
		ASSERT(0);
		return ExitInstance();
	}

	int cnt = 0;
	char ch[8];
	while (bRun) {
		if (!bBusy) {
			if (InStr.GetCount()) {
				if (DeviceIsFree()) {
					CSingleLock Lock(&CSOut);
					Lock.Lock();
					ComMsg* str;
					str = (ComMsg*)InStr.RemoveHead();
					if (pCom->SendAck(str->text, 240000)) {
						if (str->HWin) {
							::PostMessage(str->HWin, WM_COMMAND, str->Ack, NULL);
						}
					}
					else {
						if (str->HWin) {
							::PostMessage(str->HWin, WM_COMMAND, str->Nak, NULL);
						}
					}
					delete str;
				}
			}
		}
		CSingleLock Lock(&CSOut);
		Lock.Lock();
		if (cnt > 512) {
			WriteList(buff);
			buff = "";
			cnt = 0;
			bBusy = FALSE;
		}
		while (pCom->_Read((char*)ch, 1) > 0) {
			if (ch[0] == '\r') {
				WriteList(buff);
				buff = "";
				cnt = 0;
				bBusy = FALSE;
			}
			else if (ch[0] != '\n') {
				buff += ch[0];
				cnt++;
				bBusy = TRUE;
			}
			Sleep(1);
		}
		Sleep(50);
	}

	StopUnit();

	return ExitInstance();
}

BOOL CCommSvr::StartUnit() {
	if (!port) {
		ASSERT(0);
		return FALSE;
	}

	if (pCom) {
		ASSERT(0);
		return FALSE;
	}

	if (bSimu) {
		bStarted = TRUE;
		return TRUE;
	}

	pCom = new CComm;
	if (!pCom) {
		ASSERT(0);
		return FALSE;
	}
	pCom->SetParam(unsigned char(port), 9600, 8, 0, 1, 0);
	if (!pCom->Open()) {
		ASSERT(0);
		delete pCom;
		pCom = NULL;
		return FALSE;
	}

	bStarted = TRUE;

	return TRUE;
}

BOOL CCommSvr::StopUnit() {
	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		bStarted = FALSE;
		bRun = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CCommSvr::ReadString(CString& msg) {
	CSingleLock Lock(&CSOut);
	Lock.Lock();
	if (OutStr.GetCount()) {
		msg = OutStr.RemoveHead();
		return TRUE;
	}
	return FALSE;
}

BOOL CCommSvr::SendString(ComMsg* msg) {
	CSingleLock Lock(&CSIn);
	Lock.Lock();
	InStr.AddTail(msg);
	return TRUE;
}

BOOL CCommSvr::DeviceIsFree() {
	if (bSimu) return TRUE;

	char msg[512];
	if (pCom->RequestFoup("FSR FC=0", (char*)msg, 255, 240000)) {
		if (strstr(msg, "READY=TRUE")) return TRUE;
	}
	return FALSE;
}