#pragma once

// IDRMgr.h : header file
//

#include "IPC/SvrThread.h"

class CIDRSvr;

/////////////////////////////////////////////////////////////////////////////
// CIDRMgr thread
class CIDRMgr : public CSvrThread {
	//DECLARE_DYNCREATE(CIDRMgr)
	   // Attributes
public:
	CString ErrorStr;
	// Operations
public:
	CIDRSvr* pCIDRSvr[4];
	short nCIDR;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIDRMgr)
	//}}AFX_VIRTUAL
	// Implementation
public:
	BOOL SendMsg(CIDRSvr* pSvr, CSMOL& SM);
	void Log(CString msg, short lvl = 5);
	void CleanUp();
	BOOL Reply(CSMOL& SM, BOOL ret);
	BOOL ValidUnit(CSMOL& SM);
	BOOL ExecPrcsMgrCmd(CSMOL& SM);
	BOOL StopCIDRs();
	BOOL StartCIDRs(CSMOL& SM);
	BOOL ProcessCmd(CSMOL& SM);
	CIDRMgr(); // protected constructor used by dynamic creation
	virtual ~CIDRMgr();
	// Generated message map functions
	//{{AFX_MSG(CIDRMgr)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CIDRMgr* pCIDRMgr;
/////////////////////////////////////////////////////////////////////////////
