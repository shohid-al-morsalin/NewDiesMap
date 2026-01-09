#pragma once
// SelectVCamera.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectVCamera dialog

class CSelectVCamera : public CDialog {
	// Construction
public:
	CSelectVCamera(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectVCamera)
	enum { IDD = IDD_VIDEOCAMERA };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectVCamera)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectVCamera)
	afx_msg void OnSimu1();
	afx_msg void OnSimu2();
	afx_msg void OnSiimaging1();
	afx_msg void OnSiimaging2();
	afx_msg void OnLumi1();
	afx_msg void OnLumi2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
