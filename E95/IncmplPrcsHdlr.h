#pragma once
// IncmplPrcsHdlr.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CIncmplPrcsHdlr dialog

class CIncmplPrcsHdlr : public CDialog {
	// Construction
public:
	CIncmplPrcsHdlr(CWnd* pParent = NULL);   // standard constructor

	short unit;
	CString CarrierID;

	enum ACTION { NOACTION, FORGETANDCLOSEFOUP, DONOTCLOSEFOUP, STARTRECOVERY };
	ACTION Action;

	// Dialog Data
		//{{AFX_DATA(CIncmplPrcsHdlr)
	enum { IDD = IDD_INCMPLPRCSHDLR };
	CButton	m_Action2;
	CButton	m_Action3;
	CString	m_Prompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIncmplPrcsHdlr)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIncmplPrcsHdlr)
	afx_msg void OnAction1();
	afx_msg void OnAction2();
	afx_msg void OnAction3();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
