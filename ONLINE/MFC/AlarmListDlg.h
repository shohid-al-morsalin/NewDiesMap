#pragma once

// #include "resource.h"
#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDlg dialog

class CAlarmListDlg : public CResizableDialog {
public:
	CFont m_Font;
	CBrush m_Brush;

	// Construction
public:
	void ClearContent();
	CAlarmListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlarmListDlg)
	enum { IDD = IDD_ALARM };
	CListBox	m_AlarmDisplay;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmListDlg)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlarmListDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOk();
	afx_msg void OnClear();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
