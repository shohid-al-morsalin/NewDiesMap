// HdrList.cpp: implementation of the CHdrList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogMsgList.h"
#include "CLogMsgItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogMsgList::CLogMsgList() {}

CLogMsgList::~CLogMsgList() {
	Clear();
}

void CLogMsgList::Clear() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = Ptr.GetHeadPosition();
	while (pos) {
		CLogMsgItem* p = (CLogMsgItem*)Ptr.GetNext(pos);
		if (p) {
			delete p;
		}
	}
	Ptr.RemoveAll();
}

CLogMsgItem* CLogMsgList::GetTail() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (Ptr.GetCount()) {
		return (CLogMsgItem*)Ptr.RemoveTail();
	}
	return NULL;
}

void CLogMsgList::Set(CLogMsgItem* pMsg) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pMsg) {
		Ptr.AddHead(pMsg);
		// Running buffer of MAX_LOGMSGITEM entries
		if (Ptr.GetCount() > MAX_LOGMSGITEM) {
			CLogMsgItem* p = (CLogMsgItem*)Ptr.RemoveTail();
			if (p) {
				delete p;
			}
		}
	}
}