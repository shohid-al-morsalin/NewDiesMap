// LogItem.h: interface for the CLogItem class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CLogItem {
	short mLen;
	BYTE* message;

public:
	void Record(BYTE* pData, short len, CString& LogFile, char* pMsg, short* pTime);
	void Header(std::string& HDR);
	BOOL Append(BYTE* buf, short len, CString& fname, BOOL bLcl, short* pTime);
	// 	BOOL Append(CString &fname);
	BOOL SML(std::string& XML);
	// 	char * GetMsg();
	void Set(BYTE* buf, short len);
	CLogItem();
	virtual ~CLogItem();
};
