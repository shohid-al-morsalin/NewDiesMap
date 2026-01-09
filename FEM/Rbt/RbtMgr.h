#pragma once

#include "IPC/SvrThread.h"
#include "SERIAL/MsgItem.h"

class CRbtBase;

/////////////////////////////////////////////////////////////////////////////
// CRbtMgr thread
class CRbtMgr : public CSvrThread {
	// Attributes
	BOOL bAlarmed;

	BOOL bIdle;
public:;

	  CRbtBase* pRbtSvr;

	  enum RBTTYP {
		  BROOKS, PRI, SIMU, LOGOSOLBRK
	  };

	  RBTTYP RbtType;

	  enum RBTSVRSTATUS {
		  NOTSTARTED, STARTOK, STARTFAIL
	  };
	  RBTSVRSTATUS RbtSvrState;

public:
	BOOL IsIdle();
	int GetDIOIN();
	void STSDestination(CMsgItem::STSLOC STSLoc, short Stn);
	void STSSource(CMsgItem::STSLOC STSLoc, short Stn);
	BOOL IsWaferOnAligner(short arm);
	BOOL IsWaferOnAligner();
	BOOL IsWaferOnArm(short arm);
	BOOL IsWaferOnArm();
	BOOL ExitInstance();

	// Implementation
	void Log(CString msg, short lvl = 5);

	void CleanUp();

	BOOL Reply(CSMOL& SM, BOOL ret);

	BOOL ExecPrcsCmd(CSMOL& SM);

	BOOL ProcessCmd(CSMOL& SM);

	CRbtMgr(); // protected constructor used by dynamic creation

	virtual ~CRbtMgr();
};

/////////////////////////////////////////////////////////////////////////////
