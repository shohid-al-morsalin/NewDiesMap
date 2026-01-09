#pragma once
#include <afxmt.h>
#include "resource.h"

// ErrorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrorDlg dialog

class CErrorDlg : public CDialog {
	CCriticalSection CS;
	CStringList Messages;

	// Construction
public:
	CString Title;

	void Update();
	void Message(CString msg);

	CErrorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CErrorDlg)
	enum { IDD = IDD_ERROR };
	CListCtrl	m_clist;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CErrorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
