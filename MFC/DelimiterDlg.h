#pragma once

#include "resource.h"

class CDelimiterDlg : public CDialog {
	// Construction
public:
	char term;
	CDelimiterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelimiterDlg)
	enum { IDD = IDD_DELIMITER };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelimiterDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelimiterDlg)
	afx_msg void OnTab();
	afx_msg void OnSpaces();
	afx_msg void OnComma();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
