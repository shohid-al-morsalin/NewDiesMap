#pragma once

// CInfoPanelLogs form view

#include "ResizableFormView.h"
#include "xtabctrl.h"
#include "afxwin.h"
#include "afxcmn.h"

// class CMessageDispatcher;
class CInfoPanelLogsMainDlg;

class CInfoPanelLogs : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelLogs)

protected:
	CInfoPanelLogs();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelLogs();

public:
	//{{AFX_DATA(CInfoPanelLogs)
	enum { IDD = IDD_INFOPANELLOGS };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelLogs)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CInfoPanelLogs)
	afx_msg LRESULT OnSTSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CXTabCtrl m_tabctrl;
	CInfoPanelLogsMainDlg* m_InfoPanelLogsMainDlg;
};
