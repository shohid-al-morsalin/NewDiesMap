#pragma once
// StopDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStopDlg dialog

class CStopDlg : public CDialog {
	// Construction
public:
	CStopDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStopDlg)
	enum { IDD = IDD_STOP };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStopDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStopDlg)
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
