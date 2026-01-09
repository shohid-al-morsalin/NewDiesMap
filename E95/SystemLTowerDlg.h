#pragma once
// SystemLTowerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemLTowerDlg dialog

class CSystemLTowerDlg : public CDialog {
	// Construction
public:
	CSystemLTowerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemLTowerDlg)
	enum { IDD = IDD_SYSTEMLTOWERDLG };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemLTowerDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemLTowerDlg)
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
