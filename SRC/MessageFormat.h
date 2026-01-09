// MessageFormat.h: interface for the CMessageFormat class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CMessageFormat {
public:
	void Set(CString& msg, CString& src, short lvl);
	short level;
	CTime LogTime;
	CString Source;
	CString Message;

public:
	CMessageFormat();
	virtual ~CMessageFormat();
};
