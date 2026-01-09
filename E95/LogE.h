// LogE.h: interface for the CLogE class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CLogE {
	FILE* fp;
	CString dir;

	BOOL bDis;	// Disable flag [5/12/2013 user]
	CString msg;
	CCriticalSection CS;

public:
	void Message(CString str, double value);
	void Message(CString str, int value);
	void Message(CString str, float value);
	void NewLog(CString name);
	void Message(CString str);
	CLogE();
	virtual ~CLogE();
};