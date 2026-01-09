#include "stdafx.h"

#include "IPC/EqpStatus.h"
#include "ONLINE/INC/Secshead.h"
#include "128/GEM/ObjectsContainer.h"
#include "FsmCoreSocket.h"
#include "GlobalDeviceContainer.h"
#include "HsmsParaMeter.h"
#include "Monitor.h"
#include "ONLINE/GEM/GemFactory.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "OnlineMgr.h"
#include "resource.h"

#include "HSMSComThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CHSMSComThread::m_AcceptFlag = FALSE;
CFmsRWUPCoreSocket* m_pClientSocket = NULL;

extern HWND m_gImageHwnd;
extern BOOL bPauseEvents;
extern void ChangeControlStatus(EqpStatus::CONTROLSTATE nNewStatus);

////////////////////////////////////////////////////////////////////////////
// CHSMSComThread
IMPLEMENT_DYNCREATE(CHSMSComThread, CWinThread)

CHSMSComThread::CHSMSComThread() {
	pbRun = NULL;
	m_hWnd = NULL;
	bAbort = FALSE;
	m_AcceptFlag = FALSE;
	m_pListenSocket = NULL;
	m_pClientSocket = NULL;
	pdwRecvString = new BYTE[MAX_RECVSTRING + 1];
	if (!pdwRecvString) {
		ASSERT(0);
		return;
	}
}

CHSMSComThread::CHSMSComThread(HWND hWnd) {
	pbRun = NULL;
	m_hWnd = NULL;
	bAbort = FALSE;
	m_AcceptFlag = FALSE;
	m_pListenSocket = NULL;
	m_pClientSocket = NULL;
	pdwRecvString = new BYTE[MAX_RECVSTRING + 1];
	if (!pdwRecvString) {
		ASSERT(0);
		return;
	}
}

CHSMSComThread::~CHSMSComThread() {
	if (m_pListenSocket != NULL) {
		delete m_pListenSocket;
	}
	if (pdwRecvString != NULL) {
		delete[] pdwRecvString;
	}
}

BOOL CHSMSComThread::InitInstance() {
	return TRUE;
}

int CHSMSComThread::Run() {
	*pbRun = TRUE;
	MainProcedure();
	Sleep(100); // Wait a while for final write to complete [8/13/2012 Administrator]
	if (m_pClientSocket) {
		delete m_pClientSocket;
		m_pClientSocket = NULL;
	}
	*pbRun = FALSE;
	return TRUE;
}

int CHSMSComThread::ExitInstance() {
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CHSMSComThread, CWinThread)
	//{{AFX_MSG_MAP(CHSMSComThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSMSComThread message handlers

void CHSMSComThread::MainProcedure() {
	// Yuen: This thread need to be properly shutdown [2002/10/09  8:38]
	if (m_ConnectionMode == ACTIVE) {
		while (!bAbort) {
			if (CreateSocket()) {
				Progress_ActiveMode();
			}
			else {
				Log("Failed to create a connection socket", 0);
			}
			hmHSMSSS = CHsmsParaMeter::NOTCONNECTED;

			Sleep(CEIDCtlr.m_SecsParam.Param[CSecsParam::spT7TIMER].iValue);// YFL:  [4/8/2005]
		}
	}
	else if (m_ConnectionMode == PASSIVE) {
		while (!bAbort) {
			Progress_PassiveMode();
			Sleep(CEIDCtlr.m_SecsParam.Param[CSecsParam::spT7TIMER].iValue); // YFL:  [4/8/2005]
		}
	}
}

void CHSMSComThread::StopThread() {
	bAbort = TRUE;
}

BOOL CHSMSComThread::Init_ActiveMode() {
	if (m_pListenSocket != NULL) {
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}

	while (!m_pListenSocket) {
		m_pListenSocket = new CFmsRWUPCoreSocket(INVALID_SOCKET);
		if (!m_pListenSocket) {
			Sleep(1000);
			continue;
		}
	}

	return TRUE;
}

int CHSMSComThread::Get_GemDataLen(BYTE* nData) {
	int bRet = ((long)nData[0] << 24)
		| ((long)nData[1] << 16)
		| ((long)nData[2] << 8)
		| ((long)nData[3]);
	return bRet;
}

void CHSMSComThread::Set_GemDataLen(BYTE* nData, int nLen) {
	nData[3] = (BYTE)nLen;
	nData[2] = (BYTE)(nLen >> 8);
	nData[1] = (BYTE)(nLen >> 16);
	nData[0] = (BYTE)(nLen >> 24);
}

UINT CHSMSComThread::GetSysByte(BYTE* nData) {
	int bRet = ((long)nData[0] << 24)
		| ((long)nData[1] << 16)
		| ((long)nData[2] << 8)
		| ((long)nData[3]);
	return bRet;
}

#define LOOPWAIT	25

BOOL CHSMSComThread::Progress_PassiveMode() {
	BYTE  dwRecvString[DATALENSIZE + 1];
	int dwRcnt = 0;
	int dwMsg = 0;
	int dwStream = 0;

	while ((CHsmsParaMeter::m_sClientSk != INVALID_SOCKET) && (!bAbort)) {
		ULONGLONG tick = GetTickCount64();
		while (m_AcceptFlag && (!bAbort)) {
			int Len = CHsmsParaMeter::Pass_RecvString(dwRecvString, DATALENSIZE, m_Time);
			if (Len <= 0) {
				Sleep(LOOPWAIT);
				continue;
			}
			if (Len != DATALENSIZE) {
				pGDev->pOLnMgr->SECSAbnormal(pdwRecvString, Len, "Insufficient", m_Time);
				Log("T8 timeout: Not enough char", 2);
				Sleep(LOOPWAIT);
				continue;
			}
			TRACE("MessageArrived ");
			dwRcnt = Get_GemDataLen(dwRecvString);
			if (dwRcnt > sizeof(GEM_HEADER)) {
				Log("Message too long", 2);
				ProcessExcessLen(dwRcnt);
			}
			else {
				if (dwRcnt >= 10) {
					TRACE("NormalMessage\n");
					memcpy(pdwRecvString, dwRecvString, DATALENSIZE);
					ProcessMessage(dwRcnt);
				}
				else {
					TRACE("DiscardMessage\n");
					DiscardMessage(dwRcnt);
				}
			}
			if (GetTickCount64() - tick > 180000) {
				tick = GetTickCount64();
				// Approx every 180 Seconds in Idle condition
				// Do Linktest
				if ((TRUE == CHsmsParaMeter::m_ChgmodePossible) && (-1 == LinkTestReq())) {
					CHsmsParaMeter::m_ChgmodePossible = FALSE;
					::PostMessage(m_hWnd, WM_USER_ASYNC_SELECT, CHsmsParaMeter::m_sClientSk, WSAMAKESELECTREPLY(FD_CLOSE, 0));
				};
			}
			Sleep(LOOPWAIT);
		}
		Sleep(LOOPWAIT);
	}
	return FALSE;
}

int CHSMSComThread::GetSType(BYTE* pData) {
	return (int)pData[5];
}

// pData contains the whole header read from socket [8/9/2012 Administrator]
void CHSMSComThread::LinkTest_Rsp(BYTE* pData) {
	Set_GemDataLen(pData, DATADEFAULTSIZE);
	pData[9] = LINKREP;
	if (m_ConnectionMode == PASSIVE) {
		if (CHsmsParaMeter::Pass_SndString(pData, DATALENSIZE + DATADEFAULTSIZE)) {
		}
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			m_pClientSocket->Socket_WriteString(pData, DATALENSIZE + DATADEFAULTSIZE);
		}
	}
}

void CHSMSComThread::PostMessageDlg(int msg, BOOL flag) {
	if (m_gImageHwnd) {
		switch (msg) {
		case 4:
			if (flag)
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 4, 1);
			else::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 4, 0);
			break;
		case 5:
			if (flag)
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 5, 1);
			else::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 5, 0);
			break;
		case 6:
			if (flag)
				::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 6, 1);
			else::PostMessage(m_gImageHwnd, UWM_USER_BMPCHANGE, 6, 0);
			break;
		}
	}
}

int CHSMSComThread::LinkTestReq() {
	BYTE pData[15] = { 0 };
	BYTE dwLinkData[15];
	int retval = 0;
	memset(dwLinkData, 0, sizeof dwLinkData);

	Set_GemDataLen(dwLinkData, DATADEFAULTSIZE);
	pData[0] = ~0;
	pData[1] = ~0;
	pData[5] = LINKREQ;
	memcpy(&(pData[6]), &m_SystemByte, 4);
	m_SystemByte += m_SysByteInc;
	ByteSwap(&pData[6], 4);
	memcpy(&dwLinkData[4], pData, DATADEFAULTSIZE);

	if (m_ConnectionMode == PASSIVE) {
		retval = CHsmsParaMeter::Pass_SndString(dwLinkData, DATALENSIZE + DATADEFAULTSIZE);
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			retval = m_pClientSocket->Socket_WriteString(dwLinkData, DATALENSIZE + DATADEFAULTSIZE);
		}
		else {
			retval = 0;
		}
	}
	return retval;
}

void CHSMSComThread::ByteSwap(void* pVoid, int nNumBytes) {
	BYTE* pByte = (BYTE*)pVoid;
	int mid = nNumBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumBytes - 1 - nIndex];
		pByte[nNumBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
}

void CHSMSComThread::AbortTrans(UINT SysByte, int Stream, int Function) {
	CSharedMemQue	pQue;
	pQue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	pEvt->Set(EVT_TRANSACTION, EVT_ABORT_TRANSACTION, SysByte);
	pQue.WriteQue(pEvt);
	pQue.CloseMemory();
}

void CHSMSComThread::ProcessMessage(UINT DataLength) {
	UINT Len = CHsmsParaMeter::Pass_RecvString(&pdwRecvString[4], DataLength, m_Time, TRUE);
	if ((Len > 0) && (Len != DataLength)) {
		pGDev->pOLnMgr->SECSAbnormal(pdwRecvString, Len + DATALENSIZE, "Insufficient", m_Time);
		Log("Insufficient data received", 1);
		return;
	}
	if (Len > 0) {
		UINT SType = GetSType(&pdwRecvString[4]);
		GEM_HEADER* pGEMHeader = (GEM_HEADER*)pdwRecvString;
		//pGDev->pOLnMgr->SECSAbnormal(pdwRecvString,14,"TestOnly");
		pGDev->pOLnMgr->SECSContent(pGEMHeader->m_SessionId, Len, FALSE, m_Time);
		if (SType == DATAMSG) {
			UINT dwStream = pGEMHeader->m_Stream & 0x7F;
			if ((pGEMHeader->m_Function == 0)) {	// Abort Transaction
				TRACE3("Abort Transaction: S%dF%d (%ld)\n", dwStream, pGEMHeader->m_Function, GetSysByte(pGEMHeader->m_Sysbyte));
				AbortTrans(GetSysByte(pGEMHeader->m_Sysbyte), dwStream, pGEMHeader->m_Function);
			}
			else if (0 == (pGEMHeader->m_Function % 2)) {	// Secondary Host Message
				TRACE3("Secondary Host Message: S%dF%d (%ld)\n", dwStream, pGEMHeader->m_Function, GetSysByte(pGEMHeader->m_Sysbyte));
				SYSTEMTIME LocalTime;
				::GetLocalTime(&LocalTime);	// Time stamp !
				m_BufMon.put_event(pdwRecvString, DataLength + DATALENSIZE, LocalTime);
			}
			else {	// Primary host message
				PauseEvents();
				if (m_pFactory) {
					TRACE3("Primary Host Message: S%dF%d (%ld)\n", dwStream, pGEMHeader->m_Function, GetSysByte(pGEMHeader->m_Sysbyte));
					m_pFactory->Message_Seperation(pGEMHeader);
				}
				else {
					ASSERT(0);
				}
				ResumeEvents();
			}
		}
		else if (SType == SEPERATEREQ) {
			TRACE("SEPERATEREQ\n");
			// Passive mode separate request
			CHsmsParaMeter::m_ChgmodePossible = FALSE;
			hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
			hmCommState = CHsmsParaMeter::DISABLED;
			if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
				ChangeControlStatus(EqpStatus::OFFLINEHOST);
			}
			::PostMessage(m_hWnd, WM_USER_ASYNC_SELECT, CHsmsParaMeter::m_sClientSk, WSAMAKESELECTREPLY(FD_CLOSE, 0));
			m_AcceptFlag = FALSE;
		}
		else if (SType == LINKREQ) {
			TRACE("LINKREQ\n");
			LinkTest_Rsp(pdwRecvString);	// Note pdwRecvString[9] modified on return [8/9/2012 Administrator]
			pGDev->pOLnMgr->SECSContent(&pdwRecvString[4], Len, TRUE, NULL);
		}
		else if (SType == SELECTREQ) {
			TRACE("SELECTREQ\n");
			Select_Rsp(pdwRecvString);	// Note pdwRecvString[9] modified on return [8/9/2012 Administrator]
			pGDev->pOLnMgr->SECSContent(&pdwRecvString[4], Len, TRUE, NULL);
			hmHSMSSS = CHsmsParaMeter::SELECTED;
			if (pGDev->pDataBnk->bAutoConnect) {
				CWnd* pWnd = pGDev->pOLnMgr->GetCWnd();
				if (pWnd) {
					pWnd->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM, NULL);
				}
				else {
					AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM);
				}
			}
		}
		else if (SType == SELECTRSP) {
			hmHSMSSS = CHsmsParaMeter::SELECTED;
			if (pGDev->pDataBnk->bAutoConnect) {
				CWnd* pWnd = pGDev->pOLnMgr->GetCWnd();
				if (pWnd) {
					pWnd->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM, NULL);
				}
				else {
					AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM);
				}
			}
		}
		else if (SType == LINKREP) {
			TRACE("LINKREP\n");
		}
		else if (SType == REJECTREQ) {
			TRACE("REJECTREQ\n");
		}
		else if (SType == DESELECTREQ) {
			TRACE("DESELECTREQ\n");
			Deselect_Rsp(pdwRecvString);
			pGDev->pOLnMgr->SECSContent(&pdwRecvString[4], Len, TRUE, NULL);
			// Passive mode separate request
			CHsmsParaMeter::m_ChgmodePossible = FALSE;
			hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
			hmCommState = CHsmsParaMeter::DISABLED;
			if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
				ChangeControlStatus(EqpStatus::OFFLINEHOST);
			}
			::PostMessage(m_hWnd, WM_USER_ASYNC_SELECT, CHsmsParaMeter::m_sClientSk, WSAMAKESELECTREPLY(FD_CLOSE, 0));
			m_AcceptFlag = FALSE;
		}
		else if (SType == DESELECTRSP) {
			TRACE("DESELECTRSP\n");
		}
		else {
			TRACE("UNKNOWN\n");
			CString Msg;
			Msg.Format("Unknown SType %d", SType);
			Log(Msg, 1);
		}
	}
}

void CHSMSComThread::ProcessExcessLen(UINT Len) {
	pGDev->pOLnMgr->SECSAbnormal(pdwRecvString, 14, "ExcessLength", NULL);
	// Read & Discard all the message
	int ll = Len;
	// Make sure do not allocate too much memory [8/10/2012 Administrator]
	if (ll > 128) {
		ll = 128;
	}
	BYTE* pByte = new BYTE[ll];
	CHsmsParaMeter::Pass_RecvString(pByte, ll, NULL);
	CHsmsParaMeter::Pass_DescardRevString(Len - ll);	// Throw away the rest [8/10/2012 Administrator]

	CSharedMemQue	pQue;
	pQue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	// Copy the Header information
	pEvt->SetMsg(pByte, 10);
	pEvt->Set(EVT_TRANSACTION, EVT_EXCESS_MESSAGE_LENGTH, Len);
	pQue.WriteQue(pEvt);
	pQue.CloseMemory();
	delete[]pByte;
}

void CHSMSComThread::PauseEvents() {
	::bPauseEvents = TRUE;
}

void CHSMSComThread::ResumeEvents() {
	::bPauseEvents = FALSE;
}

// There is no handler for this event at this time
void CHSMSComThread::DiscardMessage(UINT Len) {
	BYTE pByte[100];
	CHsmsParaMeter::Pass_RecvString(pByte, Len, NULL);

	CSharedMemQue	pQue;
	pQue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	pEvt->SetMsg(pByte, Len);
	pEvt->Set(EVT_TRANSACTION, EVT_EXCESS_LESS_LENGTH, Len);
	pQue.WriteQue(pEvt);
	pQue.CloseMemory();
}

void CHSMSComThread::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CHSMSComThread");
	// 	if (lvl <= 1)
	// 	{
	pGDev->LogFile.LogLocal(msg);
	// 	}
}

// BOOL CHSMSComThread::ActiveConnect()
// {
// 	hmHSMSSS = CHsmsParaMeter::NOTCONNECTED;
// 	ULONGLONG tick = GetTickCount64();
// 	while((GetTickCount64() - tick) < 5000)
// 	{
// 		if (!m_pSocket->Socket_Connect(m_cActIP , m_ActPort))
// 		{
// 		}
// 		else
// 		{
// 			hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
// 			return TRUE;
// 			break;
// 		}
// 		Sleep(1000);
// 	}
// 	return FALSE;
// }

BOOL CHSMSComThread::Progress_ActiveMode() {
	BOOL bRet = TRUE;
	int nLen = 0;
	int SType = 0;
	int dwStream = 0;
	int dwFunction = 0;
	if (m_pClientSocket) {
		delete m_pClientSocket;
		m_pClientSocket = NULL;
	}

	if (AttemptConnect()) {
		m_pClientSocket = new CFmsRWUPCoreSocket(m_pListenSocket->H());

		if (!m_pClientSocket) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}

	CHsmsParaMeter::m_ActiveSocket = TRUE;

	DWORD timeoutC = 1000;
	long SelectTick = 0;
	BOOL bLinkTestIssued = FALSE;
	ULONGLONG tick = GetTickCount64();
	while ((CHsmsParaMeter::m_ActiveSocket) && (!bAbort)) {
		if (hmHSMSSS == CHsmsParaMeter::COMMUNICATING) {
			if ((GetTickCount64() - tick) > 180000) {
				// perform a link test
				if (!bLinkTestIssued) {
					LinkTestReq();
					bLinkTestIssued = TRUE;
					tick = GetTickCount64();
					// TODO: Set link test timeout timer [8/14/2012 Administrator]
				}
			}
		}
		nLen = m_pClientSocket->Socket_ReadString(pdwRecvString, DATALENSIZE);
		if (nLen == DATALENSIZE) {
			nLen = Get_GemDataLen(pdwRecvString);
			if (m_pClientSocket->Socket_ReadString(&pdwRecvString[4], nLen) > 0) {
				GEM_HEADER* pGEMHeader = (GEM_HEADER*)pdwRecvString;
				pGDev->pOLnMgr->SECSContent(pGEMHeader->m_SessionId, nLen, FALSE, NULL);
				SType = GetSType(&pdwRecvString[4]);
				if (SType == DATAMSG) {
					dwStream = pGEMHeader->m_Stream & 0x7f;
					dwFunction = pGEMHeader->m_Function;

					int pLen = Get_GemDataLen(pGEMHeader->m_DataLen);
					if ((pGEMHeader->m_Function == 0)) {	// Abort Transaction
						AbortTrans(GetSysByte(pGEMHeader->m_Sysbyte), dwStream, pGEMHeader->m_Function);
					}
					// PRIMARY message from host
					else if (dwFunction % 2) {
						PauseEvents();
						if (m_pFactory) {
							m_pFactory->Message_Seperation(pGEMHeader);
						}
						else {
							ASSERT(0);
						}
						ResumeEvents();
					}
					// Secondary Message from host
					else {
						SYSTEMTIME time;
						::GetLocalTime(&time);	// Time stamp !
						m_BufMon.put_event(pdwRecvString, nLen + DATALENSIZE, time);
					}
				}
				else if (SType == LINKREQ) {
					LinkTest_Rsp(pdwRecvString);
					pGDev->pOLnMgr->SECSContent(&pdwRecvString[4], nLen, TRUE, NULL);
				}
				else if (SType == SEPERATEREQ) {
					if (hmHSMSSS == CHsmsParaMeter::SELECTED) {
						CHsmsParaMeter::m_ChgmodePossible = FALSE;      // Line mode change possible
						hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
						hmCommState = CHsmsParaMeter::DISABLED;
						if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
							ChangeControlStatus(EqpStatus::OFFLINEHOST);
						}
					}
					CHsmsParaMeter::m_ActiveSocket = FALSE;
				}
				else if (SType == SELECTREQ) {
					TRACE("SELECTREQ\n");
					Select_Rsp(pdwRecvString);	// Note pdwRecvString[9] modified on return [8/9/2012 Administrator]
					pGDev->pOLnMgr->SECSContent(&pdwRecvString[4], nLen, TRUE, NULL);
					hmHSMSSS = CHsmsParaMeter::SELECTED;
					if (pGDev->pDataBnk->bAutoConnect) {
						CWnd* pWnd = pGDev->pOLnMgr->GetCWnd();
						if (pWnd) {
							pWnd->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM, NULL);
						}
						else {
							AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM);
						}
					}
				}
				else if (SType == SELECTRSP) {
					if (hmHSMSSS != CHsmsParaMeter::SELECTED) {
						hmHSMSSS = CHsmsParaMeter::SELECTED;
						SelectTick = 0;
						CHsmsParaMeter::m_ChgmodePossible = TRUE;      // Line mode change possible	hmHSMSSS = CHsmsParaMeter::SELECTED;
						hmCommState = CHsmsParaMeter::ENABLED;
						if (pGDev->pDataBnk->bAutoConnect) {
							if (m_hWnd) {
								::PostMessage(m_hWnd, WM_COMMAND, ID_MSG_ESTCOMM, NULL);
							}
						}
					}
				}
				else if (SType == LINKREP) {
					bLinkTestIssued = FALSE;
				}
				else if (SType == REJECTREQ) {
					//TRACE("Reject Request\n");
				}
				else if (SType == DESELECTREQ) {
					//TRACE("Deselect Request\n");
				}
				else if (SType == DESELECTRSP) {
					//TRACE("Deselect Response\n");
				}
				else {
					CString Msg;
					Msg.Format("Unknown Message %d", SType);
					Log(Msg, 1);
				}
			}
		}
		else if (nLen == 0) {
			if (hmHSMSSS == CHsmsParaMeter::NOTSELECTED) {
				static int count = 0;
				if (!SelectTick) {
					count = 0;
					SelectTick = GetTickCount64();
					SelectRequest();
				}
				else if ((GetTickCount64() - SelectTick) > timeoutC) {
					if (count > 15) {
						SelectTick = 0;
						CHsmsParaMeter::m_ActiveSocket = FALSE;
					}
					else {
						SelectTick = GetTickCount64();
						SelectRequest();
					}
					count++;
				}
			}
			if (m_pClientSocket->bConnDropped) {
				CHsmsParaMeter::m_ActiveSocket = FALSE;
				CHsmsParaMeter::m_ChgmodePossible = FALSE;      // Line mode change possible
				hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
				hmCommState = CHsmsParaMeter::DISABLED;
				if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
					ChangeControlStatus(EqpStatus::OFFLINEHOST);
				}
				Log("Connection dropped (1)", 0);
				return FALSE;
			}
		}
		else {	// YFL: Expect network error here and should close connection [4/8/2005]
			CHsmsParaMeter::m_ActiveSocket = FALSE;
			CHsmsParaMeter::m_ChgmodePossible = FALSE;      // Line mode change possible
			hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
			hmCommState = CHsmsParaMeter::DISABLED;
			if (pGDev->pGEMObjs->GetLineMode() /*pGDev->pDataBank->m_EStatus.LineMode*/ != EqpStatus::OFFLINEEQP) {
				ChangeControlStatus(EqpStatus::OFFLINEHOST);
			}
			Log("Connection dropped (2)", 0);
			return FALSE;
		}
		Sleep(LOOPWAIT);
	}
	if (bAbort) {
		SeparateReq();
	}
	return FALSE;
}

BOOL CHSMSComThread::AttemptConnect() {
	if (m_pListenSocket == NULL) {
		return FALSE;
	}

	hmHSMSSS = CHsmsParaMeter::ATTEMPTCONNECT;

	if (m_pListenSocket->Socket_Connect(m_cActIP, m_ActPort)) {
		hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
		CString msg;
		msg.Format("Host %s:%d contacted", m_cActIP, m_ActPort);
		Log(msg, 2);
		return TRUE;
	}
	hmHSMSSS = CHsmsParaMeter::NOTCONNECTED;
	return FALSE;
}

BOOL CHSMSComThread::CreateSocket() {
	hmHSMSSS = CHsmsParaMeter::NOTCONNECTED;

	if (m_pListenSocket) {
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}

	if (!m_pListenSocket) {
		DWORD timeoutB = 3600000;
		long delayB = 1000;

		ULONGLONG tick = GetTickCount64();
		while ((GetTickCount64() - tick) < timeoutB) {
			m_pListenSocket = new CFmsRWUPCoreSocket(INVALID_SOCKET);
			if (m_pListenSocket) {
				return TRUE;
			}
			Sleep(delayB);
			if (bAbort) {
				return FALSE;
			}
		}
		return FALSE;
	}
	return TRUE;
}

void CHSMSComThread::SelectRequest() {
	unsigned char nBuf[20];
	memset(nBuf, 0, sizeof(nBuf));
	Set_GemDataLen(nBuf, 10);
	int MsgLen = 10;
	nBuf[0] = (BYTE)(MsgLen >> 24);
	nBuf[1] = (BYTE)(MsgLen >> 16);
	nBuf[2] = (BYTE)(MsgLen >> 8);
	nBuf[3] = (BYTE)(MsgLen);
	nBuf[4] = (BYTE)(m_DeviceId >> 8);
	nBuf[5] = (BYTE)(m_DeviceId);
	nBuf[6] = 0;
	nBuf[7] = 0;
	nBuf[8] = 0;
	nBuf[9] = SELECTREQ;
	nBuf[10] = BYTE(m_SystemByte >> 24);
	nBuf[11] = BYTE(m_SystemByte >> 16);
	nBuf[12] = BYTE(m_SystemByte >> 8);
	nBuf[13] = BYTE(m_SystemByte);
	m_SystemByte += m_SysByteInc;
	if (m_ConnectionMode == PASSIVE) {
		if (CHsmsParaMeter::Pass_SndString(nBuf, 14)) {
		}
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			m_pClientSocket->Socket_WriteString(nBuf, 14);
		}
	}
	pGDev->pOLnMgr->SECSContent(&nBuf[4], 10, TRUE, NULL);
}

BOOL CHSMSComThread::OfflineEvent() {
	CSharedMemQue	pQue;
	pQue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_OFFLINE);
	pQue.WriteQue(pEvt);
	pQue.CloseMemory();
	return TRUE;
}

void CHSMSComThread::Deselect_Rsp(BYTE* pData) {
	Set_GemDataLen(pData, DATADEFAULTSIZE);
	pData[3] = 0;	// OK [8/10/2012 Administrator]
	pData[9] = DESELECTRSP;
	if (m_ConnectionMode == PASSIVE) {
		if (CHsmsParaMeter::Pass_SndString(pData, DATALENSIZE + DATADEFAULTSIZE)) {
		}
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			m_pClientSocket->Socket_WriteString(pData, DATALENSIZE + DATADEFAULTSIZE);
		}
	}
}

void CHSMSComThread::Select_Rsp(BYTE* pData) {
	Set_GemDataLen(pData, DATADEFAULTSIZE);
	pData[7] = 0;
	pData[9] = SELECTRSP;
	if (m_ConnectionMode == PASSIVE) {
		if (CHsmsParaMeter::Pass_SndString(pData, DATALENSIZE + DATADEFAULTSIZE)) {
		}
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			m_pClientSocket->Socket_WriteString(pData, DATALENSIZE + DATADEFAULTSIZE);
		}
	}
}

void CHSMSComThread::SeparateReq() {
	unsigned char nBuf[20];
	memset(nBuf, 0, sizeof(nBuf));
	Set_GemDataLen(nBuf, 10);
	int MsgLen = 10;
	nBuf[0] = (BYTE)(MsgLen >> 24);
	nBuf[1] = (BYTE)(MsgLen >> 16);
	nBuf[2] = (BYTE)(MsgLen >> 8);
	nBuf[3] = (BYTE)(MsgLen);
	nBuf[4] = (BYTE)(m_DeviceId >> 8);
	nBuf[5] = (BYTE)(m_DeviceId);
	nBuf[6] = 0;
	nBuf[7] = 0;
	nBuf[8] = 0;
	nBuf[9] = SEPERATEREQ;
	nBuf[10] = BYTE(m_SystemByte >> 24);
	nBuf[11] = BYTE(m_SystemByte >> 16);
	nBuf[12] = BYTE(m_SystemByte >> 8);
	nBuf[13] = BYTE(m_SystemByte);
	m_SystemByte += m_SysByteInc;
	if (m_ConnectionMode == PASSIVE) {
		if (CHsmsParaMeter::Pass_SndString(nBuf, 14)) {
		}
	}
	else if (m_ConnectionMode == ACTIVE) {
		if (m_pClientSocket) {
			m_pClientSocket->Socket_WriteString(nBuf, 14);
		}
	}
	pGDev->pOLnMgr->SECSContent(&nBuf[4], 10, TRUE, NULL);
}

void CHSMSComThread::SendSeparateReq() {
	SeparateReq();
	CHsmsParaMeter::m_ChgmodePossible = FALSE;
	hmHSMSSS = CHsmsParaMeter::NOTSELECTED;
	hmCommState = CHsmsParaMeter::DISABLED;
	if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
		ChangeControlStatus(EqpStatus::OFFLINEHOST);
	}
	::PostMessage(m_hWnd, WM_USER_ASYNC_SELECT, CHsmsParaMeter::m_sClientSk, WSAMAKESELECTREPLY(FD_CLOSE, 0));
	m_AcceptFlag = FALSE;
}

void CHSMSComThread::SendSelectReq() {
	SelectRequest();
}