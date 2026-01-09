// LogFEM.h: interface for the CLogFEM class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SYS/Log.h"

struct CSMHdr;

class CLogFEM : public CLog {
public:
	void LogLocal(CString& msg, short level = 0);
	void Log(CSMHdr& SM);
	void Log(CString& msg, short lvl = 5);
	void Log(CString& msg, short lvl, CString src = "", short id = 0);
	CLogFEM();
	virtual ~CLogFEM();
};
