#pragma once
// selport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelPort dialog

class CSelPort : public CDialog {
	// Construction
public:
	short Port;
	CSelPort(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelPort)
	enum { IDD = IDD_SELPORT };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPort)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelPort)
	afx_msg void OnPort1();
	afx_msg void OnPort2();
	afx_msg void OnPort3();
	afx_msg void OnPort4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
