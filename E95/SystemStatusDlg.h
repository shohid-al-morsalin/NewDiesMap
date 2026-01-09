#pragma once

#include "SRC/Arams.h"
#include "SRC/ResizableDialog.h"

class CSystemStatusDlg : public CResizableDialog {
	// Construction
public:
	SArams::ARAMSSTATE GetCurrentState();
	void SetStandby();
	void SetUnscheduledDown();
	void SetScheduledDown();
	void SetNonScheduled();
	void SetEngineering();
	void SetProductive();
	short numLP;
	SArams::ARAMSSTATE PrevState;
	void GetAramStatus();
	CSystemStatusDlg(CWnd* pParent = NULL);   // standard constructor
	~CSystemStatusDlg();

	// Dialog Data
		//{{AFX_DATA(CSystemStatusDlg)
	enum { IDD = IDD_SYSTEMSTATUSDLG };
	CStatic	m_cUnscheduled;
	CStatic	m_cStandby;
	CStatic	m_cScheduleDown;
	CStatic	m_cProductive;
	CStatic	m_cNonSchedule;
	CStatic	m_cEngineering;
	CButton	m_cStatus;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemStatusDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemStatusDlg)
	afx_msg void OnAramsProductive();
	afx_msg void OnAramsEngineering();
	afx_msg void OnAramsNonscheduled();
	afx_msg void OnAramsScheduleddown();
	afx_msg void OnAramsUnscheduleddown();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnClearabort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
