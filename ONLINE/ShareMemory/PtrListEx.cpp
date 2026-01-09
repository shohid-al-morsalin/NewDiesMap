// PtrListEx.cpp: implementation of the CPtrListEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PtrListEx.h"
class CQueMsgs;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPtrListEx::CPtrListEx() {}

CPtrListEx::~CPtrListEx() {
	Clear();
}

void CPtrListEx::Clear() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = QueList.GetHeadPosition();
	while (pos) {
		void* p = QueList.GetNext(pos);
		if (p) {
			Dealloc(p);
		}
	}
	QueList.RemoveAll();
}

void* CPtrListEx::RemoveHead() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return QueList.RemoveHead();
}

int CPtrListEx::GetCount() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return QueList.GetCount();
}

BOOL CPtrListEx::IsEmpty() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (QueList.GetCount()) {
		return FALSE;
	}
	return TRUE;
}

void CPtrListEx::AddTail(void* pMsg) {
	if (!pMsg) {
		ASSERT(0);
		return;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();
	QueList.AddTail(pMsg);
}