#pragma once
#include "SMOL.h"
#include "SMHdr.h"

// SMSnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSMSnd thread

class CSMSnd : public CWinThread {
	DECLARE_DYNCREATE(CSMSnd)

	// Attributes
public:

	// Operations
public:
	long wait;
	long size;
	CSMOL* pSMOL;
	CSMHdr* pSMHdr;
	CString Svr;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSMSnd)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSMSnd();
	CSMSnd();           // protected constructor used by dynamic creation

	// Generated message map functions
	//{{AFX_MSG(CSMSnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
