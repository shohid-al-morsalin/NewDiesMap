#pragma once
// OnlineECIDDlg.h : header file
//

#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// COnlineECIDDlg dialog

class COnlineECIDDlg : public CResizableDialog {
	// Construction
public:
	COnlineECIDDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnlineECIDDlg)
	enum { IDD = IDD_ONLINEECID };
	CListCtrl	m_ECIDList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineECIDDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineECIDDlg)
	afx_msg void OnButtonRefresh();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListEcid(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
