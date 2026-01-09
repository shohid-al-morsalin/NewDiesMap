// MessageBoard.cpp: implementation of the CMessageBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageBoard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageBoard MsgBrd;

CMessageBoard::CMessageBoard() {
	MaxEntry = 256;
}

CMessageBoard::~CMessageBoard() {}

void CMessageBoard::Write(CString& msg) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (strlist.GetCount() < 256) strlist.AddTail(msg);
	else {
		strlist.RemoveHead();
		strlist.AddTail(msg);
	}
}

BOOL CMessageBoard::Read(CString& msg) {
	if (!strlist.GetCount()) return FALSE;

	CString str = "";
	CSingleLock Lock(&CS);
	Lock.Lock();
	msg = strlist.RemoveHead();
	return TRUE;
}

void CMessageBoard::Write(char* msg) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (strlist.GetCount() < MaxEntry) {
		strlist.AddTail(msg);
	}
	else {
		strlist.RemoveHead();
		strlist.AddTail(msg);
	}
}

void CMessageBoard::SetMaxEntry(short max) {
	MaxEntry = max;
}