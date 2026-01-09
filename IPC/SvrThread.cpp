#include "stdafx.h"

#include "SMSvr.h"
#include "SMOL.h"
#include "SMHdr.h"

#include "SvrThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif
/////////////////////////////////////////////////////////////////////////////
// CSvrThread
//IMPLEMENT_DYNCREATE(CSvrThread, CWinThread)
CSvrThread::CSvrThread() {
	type = 0;
	pWnd = NULL;
	pWinT = NULL;
	UIMode = 5;
	mileage = 0;
	nThreadID = 0;
	SvrName = "";
	ipc = new CSMSvr;
	loop = TRUE;
	bStarted = FALSE;
}

CSvrThread::~CSvrThread() {
	if (ipc) {
		delete ipc;
	}
}

BOOL CSvrThread::InitInstance() {
	return TRUE;
}

int CSvrThread::ExitInstance() {
	HANDLE hDone; // Server informs client mutex
	hDone = ::CreateEvent(NULL, TRUE, FALSE, SvrName + "Kill");
	if (hDone) {
		SetEvent(hDone);
		CloseHandle(hDone);
	}
	if (ipc) {
		delete ipc;
		ipc = NULL;
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CSvrThread, CWinThread)
	//{{AFX_MSG_MAP(CSvrThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSvrThread message handlers
int CSvrThread::Run() {
	int ret;
	ASSERT(type);
	ASSERT(SvrName.GetLength());
	switch (type) {
	case 1:
		ret = RunSMOL();
		break;
	default:
		ret = RunSMHdr();
		break;
	}
	ExitInstance();
	return ret;
}

BOOL CSvrThread::ProcessCmd(CSMOL& SM) {
	if (SM.exe == CSMHdr::REQ) {
		if (ipc) {
			ipc->WriteSM(&SM);
		}
		else {
			ASSERT(0);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CSvrThread::ProcessCmd(CSMHdr& SM) {
	if (SM.exe == CSMHdr::REQ) {
		if (ipc) {
			ipc->WriteSM(&SM);
		}
		else {
			ASSERT(0);
			return FALSE;
		}
	}
	return TRUE;
}

//void CSvrThread::Log(CString msg, short lvl)
//{
//	CSMClt ipcLog;
//	if (ipcLog.Start1("LogSvr",sizeof(CSMHdr),FALSE)) {
//		ipcLog.LogMsg(SvrName, "", msg, lvl);
//	}
//	Yld();
//}
void CSvrThread::Yld() {
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetThread()->PumpMessage()) {
			break;
		}
	}
}

int CSvrThread::RunSMOL() {
	if (!ipc) {
		ASSERT(0);
		return -1;
	}
	ipc->Start1(SvrName, sizeof(CSMOL));
	CSMOL SM;
	bStarted = TRUE;
	while (loop) {
		if (ipc->RdMsg(&SM, INFINITE)) {
			switch (SM.cmd) {
			case 0:
				loop = FALSE;
				CleanUp();
				break;
			default:
				mileage++;
				if (ProcessCmd(SM)) {
					ipc->Done();
				}
				break;
			}
		}
		//		MSG msg;
		//		while (::PeekMessage(& msg, NULL, 0, 0, PM_NOREMOVE))
		//		{
		//			if (!AfxGetThread()->PumpMessage())
		//			{
		//				break;
		//			}
		//		}
	}
	bStarted = FALSE;
	ipc->Quit();
	ipc->Destroy();
	return 0;
}

int CSvrThread::RunSMHdr() {
	if (!ipc) {
		ASSERT(0);
		return -1;
	}
	ipc->Start1(SvrName, sizeof(CSMHdr));
	bStarted = TRUE;
	CSMHdr SM;
	while (loop) {
		if (ipc->RdMsg(&SM, INFINITE)) {
			switch (SM.cmd) {
			case 0:
				CleanUp();
				loop = FALSE;
				break;
			default:
				mileage++;
				if (ProcessCmd(SM)) {
					ipc->Done();
				}
				break;
			}
		}
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!AfxGetThread()->PumpMessage()) {
				break;
			}
		}
	}
	bStarted = FALSE;
	ipc->Quit();
	ipc->Destroy();
	return 0;
}

BOOL CSvrThread::Reply(CSMOL& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSvrThread::Reply(CSMHdr& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

long CSvrThread::GetMileage() {
	return mileage;
}

void CSvrThread::CleanUp() {}