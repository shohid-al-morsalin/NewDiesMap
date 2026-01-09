#pragma once
// OnlineECIDDlg.h : header file
//#include "afxcmn.h"

#include "..\SRC\SortListCtrl.h"
#include "..\SRC\ResizableDialog.h"

class COnlineCEIDDlg : public CResizableDialog {
	void Refresh();
public:
	COnlineCEIDDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineCEIDDlg();

	// Dialog Data
		//{{AFX_DATA(COnlineCEIDDlg)
	enum { IDD = IDD_ONLINE_CEID_DLG };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineCEIDDlg)
public:
	virtual BOOL DestroyWindow();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineCEIDDlg)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedRefresh();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkListCeid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnableallceid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Log(CString msg, short lvl = 5);
	// Holds all CEIDs
	CSortListCtrl m_List;
};