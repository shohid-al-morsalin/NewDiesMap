#pragma once

#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSystemRobotDlg dialog
class CInfoPanelSystem;

class CSystemRobotDlg : public CResizableDialog {
	// Construction
public:
	CInfoPanelSystem* m_Parent;
	void Refresh();
	CSystemRobotDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemRobotDlg)
	enum { IDD = IDD_SYSTEMROBOTDLG };
	CButton	m_cHome;
	CButton	m_cRecovery3;
	CButton	m_cService;
	CString	m_Service;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemRobotDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemRobotDlg)
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnService();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRecovery();
	afx_msg void OnRecovery2();
	afx_msg void OnHome();
	afx_msg void OnRecovery3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
