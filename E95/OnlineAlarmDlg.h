#pragma once
// OnlineAlarmDlg.h : header file
//
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// COnlineAlarmDlg dialog

class COnlineAlarmDlg : public CResizableDialog {
	// Construction
public:
	COnlineAlarmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnlineAlarmDlg)
	enum { IDD = IDD_ONLINE_ALARM };
	CListCtrl	m_AlarmList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineAlarmDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineAlarmDlg)
	afx_msg void OnButtonRefresh();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnableallalarms();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
