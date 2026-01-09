#pragma once
#include "xtabctrl.h"

// CProcJobManageDlg dialog
class COnlineCJDlg;
class COnlinePJDlg;

class CProcJobManageDlg : public CDialog {
	DECLARE_DYNAMIC(CProcJobManageDlg)

public:
	CProcJobManageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobManageDlg();

	// Dialog Data
	//{{AFX_DATA(CProcJobManageDlg)
	enum { IDD = IDD_PROCJOBMANAGEDLG };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobManageDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CProcJobManageDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CXTabCtrl m_tabctrl;
	COnlineCJDlg* m_CJDlg;
	COnlinePJDlg* m_PJDlg;
};
