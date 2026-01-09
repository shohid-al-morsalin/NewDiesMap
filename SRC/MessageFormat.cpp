// MessageFormat.cpp: implementation of the CMessageFormat class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MessageFormat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageFormat::CMessageFormat() {
	level = 10;
	LogTime = 0;
	Source = "";
	Message = "";
}

CMessageFormat::~CMessageFormat() {}

void CMessageFormat::Set(CString& msg, CString& src, short lvl) {
	level = lvl;
	Source = src;
	Message = msg;
	LogTime.GetCurrentTime();
}