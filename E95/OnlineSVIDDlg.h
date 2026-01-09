#pragma once
#include "afxcmn.h"

#include "..\SRC\SortListCtrl.h"
#include "..\SRC\ResizableDialog.h"

// COnlineSVIDDlg dialog

class COnlineSVIDDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(COnlineSVIDDlg)

	std::string GetParamType(int);
	void GetParamType(int, std::string&);

public:
	COnlineSVIDDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineSVIDDlg();

	// Dialog Data
	enum { IDD = IDD_ONLINESVIDDLG };

	// Implementation
protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COnlineSVIDDlg)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Refresh();
	void Log(CString msg, short lvl = 5);
	afx_msg void OnBnClickedButtonSaveVid();
	virtual BOOL OnInitDialog();
	// List of all parameters
	CSortListCtrl m_List;
	afx_msg void OnNMDblclkListVarIds(NMHDR* pNMHDR, LRESULT* pResult);
};
