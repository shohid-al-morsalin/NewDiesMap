#pragma once
// InputCarrierID.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CInputCarrierID dialog

class CInputCarrierID : public CDialog {
	// Construction
public:
	CInputCarrierID(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputCarrierID)
	enum { IDD = IDD_INPUTCARRIERID };
	CString	m_CarrierID;
	CString	m_Prompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputCarrierID)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputCarrierID)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};