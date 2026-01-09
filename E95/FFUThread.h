#pragma once
// FFUThread.h : header file
//

#include "CircularBuffer.h"
/////////////////////////////////////////////////////////////////////////////
// CFFUThread thread

class CFFUThread : public CWinThread {
	CCircularBuffer cb;

	DECLARE_DYNCREATE(CFFUThread)
public:
	CFFUThread();           // protected constructor used by dynamic creation

// Attributes
public:
	BOOL bRun, bStop;
	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFFUThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SleepEx(long duration);
	virtual ~CFFUThread();

	// Generated message map functions
	//{{AFX_MSG(CFFUThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

extern float FFUPressNow;

/////////////////////////////////////////////////////////////////////////////
