// ShareQue.cpp: implementation of the CShareQue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Evt.h"
#include "ShareQue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShareQue::CShareQue() {
	MaxQCnt = 127;
}

CShareQue::~CShareQue() {
	Clear();
}

void CShareQue::Dealloc(void* p) {
	if (p) {
		CEvt* q = (CEvt*)p;
		delete q;
	}
}

BOOL CShareQue::Add(CEvt* Evt) {
	if (GetCount() > MaxQCnt) {
		delete Evt;
		//		AfxMessageBox("QUE BUFFER FULL");
		return FALSE;
	}
	AddTail(Evt);
	return TRUE;
}

CEvt* CShareQue::Remove() {
	if (GetCount()) {
		return (CEvt*)RemoveHead();
	}
	return NULL;
}