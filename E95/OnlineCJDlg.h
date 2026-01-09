#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "..\SRC\ResizableDialog.h"

// COnlineCJDlg dialog

class COnlineCJDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(COnlineCJDlg)
public:
	COnlineCJDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineCJDlg();

	// Dialog Data
	enum { IDD = IDD_ONLINECJDLG };

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// List of control job
	CListCtrl m_CjobList;
	//	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonClearCJ();
	virtual BOOL OnInitDialog();
	// Active cjob
	CStatic m_CurCJName;
};
