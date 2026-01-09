#pragma once

#include "SRC/ResizableDialog.h"
#include "SRC/SortListCtrl.h"

class CAlarmAllDlg : public CResizableDialog {
	// Construction
public:
	void SetupColumn();
	CAlarmAllDlg(CWnd* pParent = NULL);   // standard constructor
	~CAlarmAllDlg();

	// Dialog Data
		//{{AFX_DATA(CAlarmAllDlg)
	enum { IDD = IDD_ALARMALL };
	CSortListCtrl	m_List;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmAllDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlarmAllDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
