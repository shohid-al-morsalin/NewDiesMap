#pragma once

#include "IPC/SvrThread.h"
//#include "CFoup.h"

/////////////////////////////////////////////////////////////////////////////
// CLPMgr thread

class CLPSvr;
class CLPXChDat;

class CLPMgr : public CSvrThread {
	// Attributes
public:

	/**
	 * short nLp;
	 * @supplierCardinality 0..4
	 */
	CLPSvr* pLPSvr[4];

	// Operations
public:
	BOOL ValidUnit(CSMOL& SM);
	BOOL Reply(CSMOL& SM, BOOL ret);
	BOOL Reply(CSMHdr& SM, BOOL ret);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLPMgr)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	BOOL ProcessCmd(CSMOL& SM);
	BOOL PrcsMgrSvc(CSMOL& SM);

	// Implementation
public:
	BOOL GetLPStatus(CLPXChDat& Xch);
	BOOL StartUnit(CSMOL& SM);
	void Log(CString msg, short lvl = 5);
	BOOL IsValidUnit(short unit);
	// Called externally [2/15/2003]
	CLPSvr* GetLPSvr(short n);
	void CleanUp();
	BOOL CarrierActSvc(CSMOL& SM);
	BOOL SendMsg(CLPSvr* pSvr, CSMOL& SM);
	CLPMgr(); // protected constructor used by dynamic creation
	virtual ~CLPMgr();

	// Generated message map functions
	//{{AFX_MSG(CLPMgr)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// extern CLPMgr * p128L;
