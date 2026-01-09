// LogE.cpp: implementation of the CLogE class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "LogE.h"
#include "..\SRC\DOSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogE::CLogE() {
	dir = "";

	bDis = TRUE;
}

CLogE::~CLogE() {}

void CLogE::Message(CString str) {
	if (bDis) {
		return;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (dir.GetLength() > 0) {
		FILE* fp = fopen(dir, "ab");
		if (fp) {
			str += "\r\n";
			fwrite(str.GetBuffer(str.GetLength()), sizeof(char), str.GetLength(), fp);
			fclose(fp);
		}
	}
}

void CLogE::NewLog(CString name) {
	if (bDis) {
		return;
	}

	DosUtil.GetProgramDir(dir);
	dir += "LOGE\\";
	int nTemp = ::GetFileAttributes(dir);
	if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
		if (::CreateDirectory(dir, NULL)) {
			return;
		}
	}
	dir += CTime::GetCurrentTime().Format("%H%M%S");
	dir += ".LOG";
	msg.Format("Recipe %s", name);
	Message(msg);
}

void CLogE::Message(CString str, int value) {
	if (bDis) {
		return;
	}
	msg.Format("%s: %d", str, value);
	Message(msg);
}

void CLogE::Message(CString str, float value) {
	if (bDis) {
		return;
	}
	msg.Format("%s: %.4f", str, value);
	Message(msg);
}

void CLogE::Message(CString str, double value) {
	if (bDis) {
		return;
	}
	msg.Format("%s: %.4lf", str, value);
	Message(msg);
}