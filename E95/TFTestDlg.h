#pragma once
// TFTestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTFTestDlg dialog

class CTFTestDlg : public CDialog {
	// Construction
public:
	CTFTestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTFTestDlg)
	enum { IDD = IDD_TFTEST };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

	short Selection;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTFTestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTFTestDlg)
	afx_msg void OnSinedata();
	afx_msg void OnReferencespec();
	afx_msg void OnDarkspec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
