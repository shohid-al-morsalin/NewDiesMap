#pragma once

#include "..\SRC\SortListCtrl.h"
#include "..\SRC\ResizableDialog.h"
// COnlineEventsDlg dialog

class COnlineEventsDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(COnlineEventsDlg)
	void Refresh();

public:
	COnlineEventsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineEventsDlg();

	// Dialog Data
		//{{AFX_DATA(COnlineEventsDlg)
	enum { IDD = IDD_ONLINEEVENTSDLG };
	//}}AFX_DATA

	// Implementation
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineEventsDlg)
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	// Holds all CEIDs
	CSortListCtrl m_List;

	// Generated message map functions
	//{{AFX_MSG(COnlineEventsDlg)
	afx_msg void OnDestroy();
	afx_msg void OnEnableallceid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Log(CString msg, short lvl = 5);
	afx_msg void OnLvnItemchangedListCeid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnNMDblclkListCeid(NMHDR* pNMHDR, LRESULT* pResult);
};
