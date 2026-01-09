// MessageBoard.h: interface for the CMessageBoard class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <afxmt.h>

class CMessageBoard {
	CCriticalSection CS;
	CStringList strlist;

	short MaxEntry;

public:
	void SetMaxEntry(short max);
	void Write(char* text);
	BOOL Read(CString& msg);
	void Write(CString& msg);

public:
	CMessageBoard();
	virtual ~CMessageBoard();
};

extern CMessageBoard MsgBrd;
