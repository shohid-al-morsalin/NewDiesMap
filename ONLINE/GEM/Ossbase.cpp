// OSSBASE.cpp: implementation of the COSSBASE class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OSSBASE.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COSSBASE::COSSBASE() {}

COSSBASE::~COSSBASE() {}

char* COSSBASE::GetType() {
	return m_ObjType;
}

BOOL COSSBASE::GetAttr(CString service, void* buf) {
	if (service == "ObjType") {
		strcpy((char*)buf, m_ObjType);
		return  TRUE;
	}
	else if (service == "ObjID") {
		strcpy((char*)buf, m_ObjID);
		return  TRUE;
	}
	return FALSE;
}

BOOL COSSBASE::SetAttr(CString service, void* val) {
	return FALSE;
}