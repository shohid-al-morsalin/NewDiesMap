#pragma once
// getpw.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CGetPW dialog

class CGetPW : public CDialog {
	// Construction
public:
	CGetPW(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetPW)
	enum { IDD = IDD_LOGOUT };
	CString	m_password;
	CString	m_username;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetPW)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPW)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
