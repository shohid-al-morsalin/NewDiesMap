#pragma once
#include <afxmt.h>
#include "resource.h"

// noticedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoticeDlg dialog

class CNoticeDlg : public CDialog {
	CCriticalSection CS;
	CStringList Messages;

	// Construction
public:
	CString Title;

	void Update();
	void Message(CString msg);

	CNoticeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNoticeDlg)
	enum { IDD = IDD_NOTICE };
	CListCtrl	m_clist;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoticeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNoticeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
