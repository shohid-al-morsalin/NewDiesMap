#pragma once
#include <afxmt.h>
#include "resource.h"

// WarningDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg dialog

class CWarningDlg : public CDialog {
	CCriticalSection CS;

	// Construction
public:
	CString Title;
	void Update();

	CStringList Messages;

	void Message(CString msg);
	CWarningDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWarningDlg)
	enum { IDD = IDD_WARNING };
	CListCtrl	m_clist;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarningDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWarningDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
