#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "..\SRC\ResizableDialog.h"

// COnlinePJDlg dialog

class COnlinePJDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(COnlinePJDlg)
	//	void ReqPJData(CString &) ;
	//	void OnPrjobname();
public:
	COnlinePJDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlinePJDlg();

	// Dialog Data
	enum { IDD = IDD_ONLINEPJDLG };

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButtonClearPJ();
	afx_msg void OnButtonRefresh();
	//	afx_msg void OnBnClickedButtonRefresh();
	//	afx_msg void OnNMDblclkListPrjob(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	// List of process jobs
	CListCtrl m_PjList;
	// Active process job
	CStatic m_CurPJName;
};
