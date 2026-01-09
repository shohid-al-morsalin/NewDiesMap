#pragma once

#include "ProcJobCommon.h"

// ProcJobPWCRDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWCRDlg dialog

class CProcJobPWCRDlg : public CDialog,
	public CProcJobCommon {
	// Construction
public:
	CProcJobPWCRDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcJobPWCRDlg)
	enum { IDD = IDD_PROCJOBPWC1RDLG };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobPWCRDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobPWCRDlg)
	afx_msg void OnCancelcarrier();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
