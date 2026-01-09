// EquipEvtThread.cpp : implementation file
//
#include "stdafx.h"
#include "EquipEvtThread.h"
#include "..\INC\Event.h"
#include "..\TCPIP\HsmsParaMeter.h"
#include "..\TCPIP\monitor.h"
#include "..\GEM\GEMFactory.h"

#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND m_gImageHwnd;
BOOL bPauseEvents = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CEquipEvtThread
IMPLEMENT_DYNCREATE(CEquipEvtThread, CWinThread);

CEquipEvtThread::CEquipEvtThread() {
	EVTMSG = EV_WAIT;
	bAbort = FALSE;
	bDone = FALSE;
	pbRun = NULL;
	pdwBuf = new BYTE[MAX_RECVSTRING + sizeof(WaitMsg) + 100];
	if (!pdwBuf) {
		ASSERT(0);
	}
	pnGemhead = new GEM_HEADER;
	if (!pnGemhead) {
		ASSERT(0);
	}
}

CEquipEvtThread::~CEquipEvtThread() {
	Clear();
}

class CFmsRWUPCoreSocket;

BOOL CEquipEvtThread::InitInstance() {
	if (m_pFactory) {
		m_pFactory->Initial_ListContrll();
		return TRUE;
	}
	ASSERT(0);
	return TRUE;
}

int CEquipEvtThread::Run() {
	if (OpenEvtQue("EVTMSG")) {
		*pbRun = TRUE;
		if (m_ConnectionMode == PASSIVE) {
			while ((CHsmsParaMeter::m_sClientSk == INVALID_SOCKET)) {
				Sleep(1000);
				// YFL:  [4/6/2005]
				if (bAbort) {
					Clear();
					*pbRun = FALSE;
					return -1;
				}
			}
		}
		MainProcedure();
	}
	Clear();
	*pbRun = FALSE;
	return 0;
}

void CEquipEvtThread::StopThread() {
	bAbort = TRUE;
}

int CEquipEvtThread::ExitInstance() {
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CEquipEvtThread, CWinThread)
	//{{AFX_MSG_MAP(CEquipEvtThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP();

/////////////////////////////////////////////////////////////////////////////
// CEquipEvtThread message handlers
void CEquipEvtThread::MainProcedure() {
	while (!bAbort) {
		EquipEvtProcedure();
		Sleep(50);
	}
	bDone = TRUE;
}

BOOL CEquipEvtThread::EquipEvtProcedure() {
	CEvt* pEvt = NULL;
	long dwSysByte = 0l;
	CSharedMemQue EvtQue;
	BOOL bPause = FALSE;

	short evtphase = 0, retc = 0;
	CString dd("");
	while (!bAbort) {
		if (EVTMSG == EV_WAIT) // Waiting for event
		{
			if (pEvt) {
				delete pEvt;
				pEvt = NULL;
			}
			if ((!bPauseEvents) && (pEvt = Get_Event())) {
				evtphase = 0;
				if (pEvt->eventid == EVT_ID10 && pEvt->make == EVT_MAKE10) {
				}
				if (pEvt->eventid == EVT_TRANSACTION && pEvt->make == EVT_CLEAR_WATT_LIST) {
					if (WaitMsgLst.GetCount()) {
						WaitMsgLst.RemoveAll();
					}
					EVTMSG = EV_PROC;
				}
				else if (pEvt->eventid == EVT_TRANSACTION && pEvt->make == EVT_ABORT_TRANSACTION) {
					EVTMSG = EV_ABORT;
				}
				else {
					EVTMSG = EV_PROC; // Pass to the ev_proc
				}
			}
			else { // No pending events. Wait more
				if (WaitMsgLst.GetCount()) {
					EVTMSG = EV_RESULT;
				}
				else {
					EVTMSG = EV_WAIT;
				}
			}
		}
		else if (EVTMSG == EV_PROC) {
			ZeroMemory(pnGemhead, sizeof GEM_HEADER);
			// Format responses and generate return events which will be pick up by EV_RESULT
			// if it is primary equipment and need reply set retc > 0
			retc = -1;
			if (m_pFactory) {
				retc = m_pFactory->Event_Proc(evtphase, pEvt, *pnGemhead);

				if (retc > 0) {
					retc = 1;
				}
				else {
					retc = 0;
				}
			}
			if (retc == 0) { // Indicates that no need to wait for response & Processing will not enter second time
				EVTMSG = EV_WAIT;
			}
			else if (retc == 1) {
				if (evtphase == 0) {
					// Keep message for later reference
					int wbit = pnGemhead->m_Stream & 0x80;
					if (wbit) {
						WaitMsg* Temp = new WaitMsg;
						memcpy(&Temp->Header, pnGemhead, sizeof(GEM_HEADONLY));
						::GetLocalTime(&Temp->MsgSentTime);
						Temp->CopyEvt(pEvt);
						pEvt = NULL;	// given to Temp
						WaitMsgLst.Add(Temp);
						EVTMSG = EV_RESULT; ///???
					}
					else {
						EVTMSG = EV_WAIT;
					}
				}
				else {
					evtphase = 0;
					EVTMSG = EV_WAIT;
				}
			}
			else {
				ASSERT(0);
			}
		}
		else if (EVTMSG == EV_RESULT) {
			memset(pdwBuf, 0, MAX_RECVSTRING);
			if (EventResultCedure(pdwBuf, dwSysByte, &pEvt)) {
				int dwLen = Get_MsgLen(pdwBuf);
				memcpy(pnGemhead, pdwBuf, dwLen + 4);
				// Reprocess the reply
				evtphase = 1;
				EVTMSG = EV_PROC;
			}
			else if (CheckTimeOut(pdwBuf)) {
				CEvt* pTmpEvt = new CEvt;
				ASSERT(pTmpEvt);
				pTmpEvt->Set(EVT_LINEMODE, EVT_REPLY_TIMEOUT);
				if (pTmpEvt->SetMsg(pdwBuf + 4, 10)) {
					EvtQue.OpenMemory("EVTMSG");
					EvtQue.WriteQue(pTmpEvt);
					pTmpEvt = NULL;	// Invalidated
					EvtQue.CloseMemory();
				}
				else {
					delete pTmpEvt;
				}
				EVTMSG = EV_WAIT;
			}
			else {
				EVTMSG = EV_WAIT;
			}
		}
		else if (EVTMSG == EV_ABORT) {
			if (pEvt) {
				AbortTrans(pEvt->SysByte);
			}
			else {
				ASSERT(0);
			}
			EVTMSG = EV_WAIT;
		}
		Sleep(20);
	}

	if (pEvt) {
		delete pEvt;
		pEvt = NULL;
	}
	return TRUE;
}

CEvt* CEquipEvtThread::Get_Event() {
	if (m_EvtQue.GetSharedQcnt()) {
		return m_EvtQue.ReadQue();
	}
	return NULL;
}

BOOL CEquipEvtThread::OpenEvtQue(char* pName) {
	BOOL bRet = m_EvtQue.CreateMemory(pName, 500, sizeof(CEvt));
	return bRet;
}

BOOL CEquipEvtThread::EventResultCedure(BYTE* szBuf, long sysbyte1, CEvt** pEvt) {
	// Traverse the Wait list and compare with each message in the buffer
	// check for timeout value also

	SYSTEMTIME time;
	BOOL bRes = FALSE, bFound = FALSE;

	for (int nCount = 0; nCount < m_BufMon.count; nCount++) {
		int len;
		m_BufMon.get_event(pdwBuf, len, time);

		GEM_HEADER* pGEMHeader = (GEM_HEADER*)pdwBuf;

		// check system bytes
		WaitMsg* p = WaitMsgLst.Get(Get_SysByte(pGEMHeader->m_Sysbyte));
		if (p) {
			// Found match
			bRes = TRUE;
			// Copy to the output block
			memcpy(szBuf, pGEMHeader, sizeof(GEM_HEADER));
			delete* pEvt;
			*pEvt = p->pEvnt;
			p->pEvnt = NULL;	// moved to pEvt, Invalidated
			// Remove from list
			p = WaitMsgLst.Remove(Get_SysByte(pGEMHeader->m_Sysbyte));
			if (p) {
				delete p;
			}
			return TRUE;
		}
		else {
			m_BufMon.put_event(pdwBuf, len, time);
		}
	}
	return FALSE;
}

long CEquipEvtThread::Get_SysByte(BYTE* pBuf) {
	return ((long)pBuf[0] << 24) + ((long)pBuf[1] << 16) + ((long)pBuf[2] << 8) + (long)pBuf[3];
}

int CEquipEvtThread::Get_MsgLen(BYTE* pBuf) {
	return ((long)pBuf[0] << 24) + ((long)pBuf[1] << 16) + ((long)pBuf[2] << 8) + ((long)pBuf[3]);
}

void CEquipEvtThread::PostMessageDlg(int msg, BOOL flag) {
	if (m_gImageHwnd) {
		switch (msg) {
		case 1:
			if (flag) {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 1, 1);
			}
			else {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 1, 0);
			}
			break;
		case 2:
			if (flag) {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 2, 1);
			}
			else {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 2, 0);
			}
			break;
		case 3:
			if (flag) {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 3, 1);
			}
			else {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 3, 0);
			}
			break;
		case 4:
			if (flag) {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 4, 1);
			}
			else {
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 4, 0);
			}
			break;
		}
	}
}

BOOL CEquipEvtThread::CheckTimeOut(BYTE* szBuf) {
	// Traverse the Wait list and compare with each message in the buffer
	// check for timeout value also

	BOOL bRes = FALSE, bFound = FALSE;
	int min = 0;
	int sec = CEIDCtlr.m_SecsParam.Param[CSecsParam::spT3TIMER].iValue / 1000;
	min = sec / 60;
	sec %= 60;
	CTimeSpan T3Timer(0, 0, min, sec);
	CTime CurrentTime = CTime::GetCurrentTime() - T3Timer;
	while (1) {
		WaitMsg* p = WaitMsgLst.GetTimeoutMsg(CurrentTime);
		if (p) {
			// Timeout has occurred
			memcpy(szBuf, &p->Header, sizeof(GEM_HEADONLY));
			// Copy & remove & exit
			CString str;
			str.Format("T3 Timeout: SysByte %d", Get_SysByte(p->Header.m_Sysbyte));
			Log(str, 1);
			bFound = TRUE;
			delete p;
		}
		else break;
	}
	return bFound;
}

BOOL CEquipEvtThread::AbortTrans(UINT AbortSysByte) {
	// Traverse the Wait list and compare with each message in the buffer
	// check for timeout value also

	BOOL bRes = FALSE, bFound = FALSE;
	// First check in the Wait List

	WaitMsg* p = WaitMsgLst.Remove(AbortSysByte);
	if (p) {
		// Found match
		bRes = TRUE;
		Log("Transaction aborted", 1);
		// Remove from list
		bFound = TRUE;
		delete p;
	}
	return bFound;
}

void CEquipEvtThread::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CEquipEvtThread");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CEquipEvtThread::Clear() {
	if (pnGemhead) {
		delete pnGemhead;
		pnGemhead = NULL;
	}
	if (pdwBuf) {
		delete[] pdwBuf;
		pdwBuf = NULL;
	}
}