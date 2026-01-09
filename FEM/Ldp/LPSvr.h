#pragma once

// #include "IPC/Smol.h"
#include "IPC/SvrThread.h"

class CFoup;
class CIDRUnit;

/////////////////////////////////////////////////////////////////////////////
// CLPSvr thread

class CLPSvr : public CSvrThread {
	// Attributes
public:
	CFoup* pLP;
	CIDRUnit* pCIDR;

	BOOL ret;
	BOOL bSimu;
	BOOL bComThread;
	short Port;
	short Unit;
	CString ErrorStr;

	// Operations
public:
	void SetInPlace(BOOL bInP);
	virtual BOOL GetLPStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr);
	virtual CString GetSlotmap();

	BOOL Reply(CSMOL& SM, BOOL ret);
	void SetReply(CSMHdr& SM1, CSMOL& SM);
	BOOL ExecPrcsMgrCmd(CSMOL& SM);
	BOOL ProcessCmd(CSMOL& SM);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CLPSvr)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ExecCarrActCmd(CSMOL& SM);
	BOOL LedClearAll();
	BOOL LedManual(BOOL bMode);

	BOOL LedReserved(BOOL bMode);
	BOOL SetE84AccessMode(CLPObj::ACCESSMODE mode);

	BOOL CheckAlarm(CString msg);

public:
	BOOL StartUnit(RPLY rpy, BOOL bNoClose);
	void ExecDeviceCmd(CSMOL& SM);
	void Log(CString msg, short lvl = 5);
	void CleanUp();

	CLPSvr();           // protected constructor used by dynamic creation
	virtual ~CLPSvr();

	// Generated message map functions
	//{{AFX_MSG(CLPSvr)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
