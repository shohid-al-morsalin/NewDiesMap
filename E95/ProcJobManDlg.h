#pragma once
// ProcJobManDlg.h : header file
//

#include "StatusReq.h"
#include "ProcJobCommon.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CProcJobManDlg dialog

class CProcJobManDlg : public CResizableDialog,
	public CStatusRequest, public CProcJobCommon {
	// Construction
public:
	void SetDataReqTimeOut(long dur);
	int GetHWnd();
	void Log(CString msg, short lvl);
	CProcJobManDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobManDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobManDlg)
	enum { IDD = IDD_PRCSJOBMAN };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobManDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobManDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
