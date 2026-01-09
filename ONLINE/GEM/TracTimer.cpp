// TracTimer.cpp : implementation file
//

#include "stdafx.h"
#include "TracTimer.h"
#include "..\Inc\Event.h"
#include "..\ShareMemory\Evt.h"
#include "..\ShareMemory\SharedMemQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern HWND   m_gHwnd;
CPtrList m_TimerList;

/////////////////////////////////////////////////////////////////////////////
// CTracTimer
CTimer::CTimer() {}

CTimer::~CTimer() {
	m_TimerList.RemoveAll();
}

void CALLBACK CTimer::StdTimerProc(HWND hwnd, UINT uMsg, UINT nIDEvent, DWORD dwTime) {
	NODE* nNode = NULL;

	POSITION pos = m_TimerList.GetHeadPosition();

	nNode = (NODE*)m_TimerList.GetHead();
	while (pos != NULL) {
		if (nNode->m_Trid == nIDEvent) {
			if (nNode->m_Totsmp > 1) {
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->Set(161, nIDEvent);
				pEvt->no = 1;
				nNode->m_Totsmp--;
				CSharedMemQue	m_gEvtQue;
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(pEvt);
				m_gEvtQue.CloseMemory();
			}
			else {
				//				KillTimer( m_gHwnd , nNode->m_Trid );
				m_TimerList.RemoveAt(pos);
				delete nNode;
			}
			break;
		}
		m_TimerList.GetNext(pos);
		if (pos) {
			nNode = (NODE*)m_TimerList.GetAt(pos);
		}
	}
}

CTraceTracker::CTraceTracker() {}

CTraceTracker::CTraceTracker(UINT nTrid, UINT nTotsmp) {
	m_trid = nTrid;
	m_Totsmp = nTotsmp;
	m_Smpln = 1;
}

CTraceTracker::~CTraceTracker() {}

void CTraceTracker::StartTrace(UINT nIDEvent, UINT uElapse, UINT uKillCounter) {
	pNode = new NODE;
	pNode->m_Trid = nIDEvent;
	pNode->m_Dsper = uElapse;
	pNode->m_Totsmp = uKillCounter + 1;
	pNode->m_Currentsmp = 0;

	m_TimerList.AddHead(pNode);

	//	::SetTimer( m_gHwnd , nIDEvent , uElapse , StdTimerProc );
}