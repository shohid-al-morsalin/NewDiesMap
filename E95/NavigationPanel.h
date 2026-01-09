#pragma once

#include "..\SRC\BtnST1.h"
#include "ResizableFormView.h"

// CNavigation form view

class CNavigationPanel : public CResizableFormView {
	DECLARE_DYNCREATE(CNavigationPanel)

protected:
	CNavigationPanel();           // protected constructor used by dynamic creation
	virtual ~CNavigationPanel();

public:
	//{{AFX_DATA(CNavigationPanel)
	enum { IDD = IDD_NAVIGATIONPANEL };
	CButtonST1	m_cEqpMessage;
	CButtonST1	m_cHostMessage;
	CButtonST1	m_cView;
	CButtonST1	m_cSystem;
	CButtonST1	m_cSetup;
	CButtonST1	m_cRecipes;
	CButtonST1	m_cNaviHelp;
	CButtonST1	m_cLogs;
	CButtonST1	m_cJobs;
	CButtonST1	m_cAlarms;
	//}}AFX_DATA

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavigationPanel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:

	//{{AFX_MSG(CNavigationPanel)
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedSetup();
	afx_msg void OnBnClickedJobs();
	afx_msg void OnBnClickedSystem();
	afx_msg void OnBnClickedRecipes();
	afx_msg void OnBnClickedLogs();
	afx_msg void OnBnClickedAlarms();
	afx_msg void OnBnClickedHelps();
	afx_msg void OnBnClickedView();
	afx_msg void OnE95Blinkalarmbutton();
	afx_msg void OnE95Blinkjobbutton();
	afx_msg void OnE95StopBlinkjobbutton();
	afx_msg void OnHostmessage();
	afx_msg void OnE95Blinkhostmessage();
	afx_msg void OnEqpMessage();
	afx_msg void OnE95Blinkeqpmessage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	enum ACTNBUT { acNONE, JOBS, RECIPES, SYSTEM, SETUP, LOGS, ALARMS, HELPS, VIEW, HOSTMESSAGE, EQPMESSAGE };
	ACTNBUT ActiveButton;
};
