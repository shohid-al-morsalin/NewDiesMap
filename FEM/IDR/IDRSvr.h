#pragma once

// #include "IDRAsyst.h"
// #include "IDRUnitKey.h"	// Added by ClassView
// #include "IDRUnitOmron.h"	// Added by ClassView
#include "IPC/SvrThread.h"

class CIDRUnit;

/////////////////////////////////////////////////////////////////////////////
// CIDRSvr thread

class CIDRSvr : public CSvrThread {
	//	DECLARE_DYNCREATE(CIDRSvr)

	// Attributes
public:
	CString ErrorStr;

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CIDRSvr)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	// 	BOOL bSimuX;
	BOOL StartUnit(CSMOL& SM);
	//	void Log(CString msg, short lvl);
	void CleanUp();
	BOOL Reply(CSMOL& SM, BOOL ret);
	BOOL ExecPrcsMgrCmd(CSMOL& SM);
	//CSMOL SM;
	BOOL ProcessCmd(CSMOL& SM);
	short Port;
	short Unit;
	void SetReply(CSMHdr& SM1, CSMOL& SM);
	//CIDRUnitKey CIDRUnit;		// Elpida Hiroshima
	CIDRUnit* IDRUnit;	// TTI Katsuta

	CIDRSvr();           // protected constructor used by dynamic creation
	virtual ~CIDRSvr();

	// Generated message map functions
	//{{AFX_MSG(CIDRSvr)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
