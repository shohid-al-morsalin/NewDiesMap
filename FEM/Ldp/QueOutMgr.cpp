// QueOutMgr.cpp: implementation of the CQueOutMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QueOutMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueOutMgr::CQueOutMgr() {}

CQueOutMgr::~CQueOutMgr() {}

BOOL CQueOutMgr::Get(CString& msg) {
	if (MSGList.size()) {
		vector <CString>::iterator i;
		i = MSGList.begin();
		msg = *i;
		MSGList.erase(i);
		return TRUE;
	}
	return FALSE;
}

void CQueOutMgr::Add(CString msg) {
	MSGList.push_back(msg);
}