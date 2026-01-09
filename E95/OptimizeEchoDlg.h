#pragma once
// OptimizeEchoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptimizeEchoDlg dialog

class COptimizeEchoDlg : public CDialog {
	// Construction
public:
	COptimizeEchoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptimizeEchoDlg)
	enum { IDD = IDD_OPTIMIZEECHOMOTOR };
	CListCtrl	m_cList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptimizeEchoDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptimizeEchoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
