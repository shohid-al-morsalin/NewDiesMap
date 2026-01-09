#pragma once
// EchoTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEchoTest dialog

class CEchoTest : public CDialog {
	// Construction
public:
	CEchoTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEchoTest)
	enum { IDD = IDD_ECHOTEST };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEchoTest)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	short cnt;
	float P, I, D;
	float dP, dI, dD;

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEchoTest)
	afx_msg void OnStart();
	afx_msg void OnNext();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
