#pragma once

// CInfoPanelAlarms form view

#include "xtabctrl.h"
#include "ResizableFormView.h"

class CAlarmAllDlg;
class CAlarmListDlg;

class CInfoPanelAlarms : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelAlarms)

protected:
	CInfoPanelAlarms();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelAlarms();

public:
	// Dialog Data
	//{{AFX_DATA(CInfoPanelAlarms)
	enum { IDD = IDD_INFOPANELALARMS };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelAlarms)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelAlarms)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CAlarmAllDlg* m_AlarmAll;
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	CAlarmListDlg* m_AlarmList;
	CXTabCtrl m_tabctrl;
};
