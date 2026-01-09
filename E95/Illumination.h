#pragma once
// Illumination.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIllumination dialog

class CIllumination : public CDialog {
	// Construction
public:
	CIllumination(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIllumination)
	enum { IDD = IDD_ILLUMINATION };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIllumination)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIllumination)
	afx_msg void OnEchoon();
	afx_msg void OnEchooff();
	afx_msg void OnTfilmon();
	afx_msg void OnTfilmoff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
