#pragma once
// RunRecipe.h : header file
//

class CSetupProp3Dlg;

/////////////////////////////////////////////////////////////////////////////
// CRunRecipe dialog

class CRunRecipe : public CDialog {
	// Construction
public:
	BOOL IsDone();
	CWinThread* pThread;

	CSetupProp3Dlg* pExe;
	CRunRecipe(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRunRecipe)
	enum { IDD = IDD_RUNRECIPE };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunRecipe)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRunRecipe)
	afx_msg void OnStop();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};