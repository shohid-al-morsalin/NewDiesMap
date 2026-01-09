// WaitMsgList.cpp: implementation of the CWaitMsgList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaitMsgList.h"
#include "..\GEM\CediListCtroller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaitMsgList::CWaitMsgList() {}

CWaitMsgList::~CWaitMsgList() {
	Clear();
}

void CWaitMsgList::Dealloc(void* p) {
	WaitMsg* q = (WaitMsg*)p;
	if (q) {
		delete q;
	}
}

void CWaitMsgList::Add(WaitMsg* p) {
	AddTail(p);
}

WaitMsg* CWaitMsgList::Remove() {
	if (GetCount()) {
		return (WaitMsg*)RemoveHead();
	}
	return NULL;
}

WaitMsg* CWaitMsgList::Remove(long SysByte) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION opos;
	POSITION pos = QueList.GetHeadPosition();
	while (pos) {
		opos = pos;
		WaitMsg* p = (WaitMsg*)QueList.GetNext(pos);
		if (p) {
			if (Get_SysByte(p->Header.m_Sysbyte) == SysByte) {
				QueList.RemoveAt(opos);
				return p;
			}
		}
	}
	return NULL;
}

WaitMsg* CWaitMsgList::Get(long SysByte) {
	CSingleLock Lock(&CS);
	Lock.Lock();

	POSITION pos = QueList.GetHeadPosition();
	while (pos) {
		WaitMsg* p = (WaitMsg*)QueList.GetNext(pos);
		if (p) {
			long SBy = Get_SysByte(p->Header.m_Sysbyte);
			if (SBy == SysByte) {
				TRACE3("WaitMsg Get: S%dF%d (%ld)\n", p->Header.m_Stream & 0x7F, p->Header.m_Function, SBy);
				return p;
			}
			TRACE1("WaitMsg: %ld\n", SBy);
		}
	}
	return NULL;
}

long CWaitMsgList::Get_SysByte(BYTE* pBuf) {
	return ((long)pBuf[0] << 24) + ((long)pBuf[1] << 16) + ((long)pBuf[2] << 8) + ((long)pBuf[3]);
}

WaitMsg* CWaitMsgList::GetTimeoutMsg(CTime& CurrentTime) {
	CSingleLock Lock(&CS);
	Lock.Lock();

	//	short cnt = QueList.GetCount();

	POSITION opos;
	POSITION pos = QueList.GetHeadPosition();
	while (pos) {
		opos = pos;
		WaitMsg* p = (WaitMsg*)QueList.GetNext(pos);
		if (p) {
			CTime MsgSendTime(p->MsgSentTime);
			if (CurrentTime > MsgSendTime + CEIDCtlr.m_SecsParam.Param[CSecsParam::spT3TIMER].iValue) {
				QueList.RemoveAt(opos);
				return p;
			}
		}
	}
	return NULL;
}

void CWaitMsgList::RemoveAll() {
	while (GetCount()) {
		delete (WaitMsg*)RemoveHead();
	}
}