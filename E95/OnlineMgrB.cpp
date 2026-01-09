// OnlineMgrB.cpp: implementation of the COnlineMgrB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OnlineMgrB.h"
#include "OLnSvrObj.h"

#include "..\ONLINE\INC\Define.h"
#include "..\ONLINE\INC\Event.h"
#include "..\ONLINE\TCPIP\SelectMode.h"
#include "..\ONLINE\TCPIP\HsmsParaMeter.h"
#include "..\ONLINE\TCPIP\HSMSComThread.h"
#include "..\ONLINE\MFC\EquipEvtThread.h"

#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COnlineMgrB::COnlineMgrB() {
	pOLnSvrObj = NULL;
	pEVTThread = NULL;
	pHSMSThread = NULL;

	EVTHandle = NULL;
	HSMSHandle = NULL;

	bOnlineSelected = FALSE;
}

COnlineMgrB::~COnlineMgrB() {}

BOOL COnlineMgrB::StartThreads() {
	if (m_pFactory) {
		ASSERT(0);
	}

	m_pFactory = new CGemFactory;
	if (!m_pFactory) {
		ShowMsg("GEMFactory create fail");
		ASSERT(0);
	}

	StartHSMSThread();
	StartEVTThread();
	StartOLnSvrThread();
	return TRUE;
}

void COnlineMgrB::StopThreads() {
	StopOLnSvrThread();
	StopEVTThread();
	StopHSMSThread();

	if (m_pFactory) {
		delete m_pFactory;
		m_pFactory = NULL;
	}
}

BOOL COnlineMgrB::InitSocket() {
	BOOL   ret = TRUE;
	char   pszMessage[100];
	//DWORD Err =0;

	m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (m_sListen == INVALID_SOCKET) {
		SocketError();
		Log("<Socket> Create error", 1);
	}
	else {
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(m_SocketPortNo);
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(m_sListen, (LPSOCKADDR)&m_addr, sizeof(m_addr)) == SOCKET_ERROR) {
			SocketError();
			Log("<Socket> Bind error", 1);
		}
		else {
			SOCKADDR_IN addr;
			int nAddrLen = sizeof(addr);
			if (getsockname(m_sListen, (LPSOCKADDR)&addr, &nAddrLen) == SOCKET_ERROR) {
				SocketError();
				Log("<Socket> Could not get host name", 1);
			}
			else {
				wsprintf(pszMessage, "%d", ntohs(addr.sin_port));
				if (WSAAsyncSelect(m_sListen, GetHWnd(), WM_USER_ASYNC_SELECT, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
					SocketError();
					Log("<Socket> Assynch select error", 1);
				}
				else {
					if (listen(m_sListen, 3) == SOCKET_ERROR) {
						SocketError();
						Log("<Socket> Socket could not be put in listening state", 1);
					}
				}
			}
		}
	}
	return ret;
}

void COnlineMgrB::SocketError() {
	DWORD Err = ::WSAGetLastError();
	CString ErrStr;
	switch (Err) {
	case WSANOTINITIALISED:
		ErrStr = "A successful WSAStartup call must occur before using this function.";
		break;
	case WSAENETDOWN:
		ErrStr = "The network subsystem or the associated service provider has failed.";
		break;
	case WSAEAFNOSUPPORT:
		ErrStr = "The specified address family is not supported.";
		break;
	case WSAEINPROGRESS:
		ErrStr = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
		break;
	case WSAEMFILE:
		ErrStr = "No more socket descriptors are available.";
		break;
	case WSAENOBUFS:
		ErrStr = "No buffer space is available. The socket cannot be created.";
		break;
	case WSAEPROTONOSUPPORT:
		ErrStr = "The specified protocol is not supported.";
		break;
	case WSAEPROTOTYPE:
		ErrStr = "The specified protocol is the wrong type for this socket.";
		break;
	case WSAESOCKTNOSUPPORT:
		ErrStr = "The specified socket type is not supported in this address family.";
		break;
	default:
		ErrStr.Format("<Socket> %d", Err);
		break;
	}
	Log(ErrStr, 2);
}

BOOL COnlineMgrB::StartHSMSThread() {
	CString ErrMsg;

	ShowMsg("Initializing Sockets");

	if (m_ConnectionMode == PASSIVE) {
		ShowMsg("Passive Connection");
		InitSocket();
	}
	else {
		ShowMsg("Active Connection");
	}

	pHSMSThread = new CHSMSComThread(GetHWnd());
	if (!pHSMSThread) {
		ShowMsg("<HSMS> Thread alloc failure");
		ASSERT(0);
	}
	else {
		pHSMSThread->pbRun = &bRunHSMS;
		if (!pHSMSThread->CreateThread(CREATE_SUSPENDED)) {
			ShowMsg("<HSMS> Thread start failure", 1);
			ASSERT(0);
		}
		else {
			pHSMSThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			VERIFY(pHSMSThread->ResumeThread());
			HSMSHandle = pHSMSThread->m_hThread;
			ShowMsg("HSMSThread started");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL COnlineMgrB::StopHSMSThread() {
	if (pHSMSThread) {
		pHSMSThread->StopThread();
		while (bRunHSMS) {
			Sleep(100);
		}
		pHSMSThread = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL COnlineMgrB::StartOLnSvrThread() {
	CString ErrMsg;

	pOLnSvrObj = new COLnSvrObj();
	if (!pOLnSvrObj) {
		//AfxMessageBox("Thread alloc error");
		// Should not continue after this pont
		ErrMsg.Format("Memory alloc error [COLnSvrObj] <%d>", ::GetLastError());
		ShowMsg(ErrMsg, 0);
		ShowMsg("OLnSvrObj create fail");
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

BOOL COnlineMgrB::StopOLnSvrThread() {
	if (pOLnSvrObj) {
		delete pOLnSvrObj;
		return TRUE;
	}
	return FALSE;
}

BOOL COnlineMgrB::StartEVTThread() {
	CString ErrMsg;

	pEVTThread = new CEquipEvtThread();
	if (!pEVTThread) {
		// Should not continue after this pont
		ErrMsg.Format("Event thread mem alloc error<%d>", ::GetLastError());
		ShowMsg(ErrMsg, 0);
		ASSERT(0);
		return FALSE;
	}
	pEVTThread->pbRun = &bRunEVT;
	if (!pEVTThread->CreateThread(CREATE_SUSPENDED)) {
		ErrMsg.Format("Event thread start error<%d>", ::GetLastError());
		ShowMsg(ErrMsg, 0);
		return FALSE;
	}
	pEVTThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	VERIFY(pEVTThread->ResumeThread());
	EVTHandle = pEVTThread->m_hThread;
	ShowMsg("EquipEvtThread started");
	return TRUE;
}

BOOL COnlineMgrB::StopEVTThread() {
	if (pEVTThread) {
		pEVTThread->StopThread();
		while (bRunEVT) {
			Sleep(100);
		}
		pEVTThread = NULL;
		return TRUE;
	}
	return FALSE;
}

SOCKET COnlineMgrB::GetListenSocket() {
	return m_sListen;
}

void COnlineMgrB::FDAcceptSocket() {
	static       SOCKADDR_IN addrForm;
	static int   nAddrFormLen = sizeof(addrForm);
	int			 nSocketError = 0;
	char         pszMessage[128];
	static       IN_ADDR inForm;

	if (bOnlineSelected) {
		SOCKET Temp = accept(GetListenSocket(), (LPSOCKADDR)&addrForm, &nAddrFormLen);
		nSocketError = shutdown(Temp, 3);
		closesocket(Temp);
		Log("New accept request ignored", 3);
		return;
	}
	CHSMSComThread::m_AcceptFlag = FALSE;
	CHsmsParaMeter::m_ChgmodePossible = TRUE;      // Line mode change possible
	if (CHsmsParaMeter::m_sClientSk != INVALID_SOCKET) {
		closesocket(CHsmsParaMeter::m_sClientSk);
		CHsmsParaMeter::m_sClientSk = INVALID_SOCKET;
		Log("Closing old socket", 3);
	}

	if (CHsmsParaMeter::m_sClientSk == INVALID_SOCKET) {
		CHsmsParaMeter::m_sClientSk = accept(m_sListen, (LPSOCKADDR)&addrForm, &nAddrFormLen);
		if (CHsmsParaMeter::m_sClientSk == INVALID_SOCKET) {
			nSocketError = WSAGetLastError();
			if (nSocketError != WSAEWOULDBLOCK) {
				SocketError();
				wsprintf(pszMessage, "ERROR %d Accepting connection", nSocketError);
				Log(pszMessage, 1);
			}
		}
		else {
			memcpy(&inForm, &addrForm.sin_addr.s_addr, 4);
			wsprintf(pszMessage, "CONNECTION ACCEPTED FROM %s, %d\n", inet_ntoa(inForm), ntohs(addrForm.sin_port));
			Log(pszMessage, 5);
		}

		//  [8/12/2012 Administrator]
		CHSMSComThread::m_AcceptFlag = TRUE;           // Communication Thread Running
	}
	else {
		closesocket(CHsmsParaMeter::m_sClientSk);
		Log("Close socket (2)", 3);
		CHsmsParaMeter::m_sClientSk = INVALID_SOCKET;
	}
}

void COnlineMgrB::FDCloseSocket() {
	CSharedMemQue EvtQue;
	EvtQue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_INIT_DATA);
	EvtQue.WriteQue(pEvt);
	EvtQue.CloseMemory();

	bOnlineSelected = FALSE;
	CHSMSComThread::m_AcceptFlag = FALSE;
	CHsmsParaMeter::m_ChgmodePossible = FALSE;
	shutdown(CHsmsParaMeter::m_sClientSk, 3);
	closesocket(CHsmsParaMeter::m_sClientSk);
	Log("Socket close (3)", 3);
	CHsmsParaMeter::m_sClientSk = INVALID_SOCKET;
}

void COnlineMgrB::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "OnlMgrB");
	pGDev->LogFile.LogLocal(msg);
}

void COnlineMgrB::SendSeparateReq() {
	if (pHSMSThread) {
		pHSMSThread->SendSeparateReq();
	}
}

void COnlineMgrB::SendSelectReq() {
	if (pHSMSThread) {
		pHSMSThread->SendSelectReq();
	}
}