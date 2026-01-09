// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CSMClt;
struct CSMHdr;

class CLog {
protected:
	CSMClt* pIPC;

public:
	virtual void Log(CSMHdr& SM);
	virtual void LogLocal(CString& msg, short level = 0);
	virtual void Log(CString& msg, short lvl = 5) = 0;
	virtual void Log(CString& msg, short lvl, CString src = "", short id = 0) = 0;
	CLog();
	virtual ~CLog();
};

// extern CLog *pLog;
