#pragma once
#include "afxcmn.h"
#include <afxmt.h>

#include "..\SRC\ResizableDialog.h"

// CInfoPanelLogsMainDlg dialog

class CInfoPanelLogsMainDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(CInfoPanelLogsMainDlg)

public:
	CInfoPanelLogsMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoPanelLogsMainDlg();

	// Dialog Data
		//{{AFX_DATA(CInfoPanelLogsMainDlg)
	enum { IDD = IDD_INFOPANELLOGSMAINDLG };
	//}}AFX_DATA
	CCriticalSection CS;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelLogsMainDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:

	enum BEHAVIOR { MAIN = 0, C128, C900 };
	BEHAVIOR Behavior;  // 0 - main, 1 - 128, 2 - 900, etc ...
	CListCtrl m_List;
	short level; // loglevel

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelLogsMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnLogRefresh(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// 	short RefreshLog(void);
	void ClearList();
	void SetupColumn(void);
};
