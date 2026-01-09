#pragma once
// IssueAlarmDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIssueAlarmDlg dialog

class CIssueAlarmDlg : public CDialog {
	// Construction
public:
	void SetAlarm(int alID);
	void RefreshList();
	CIssueAlarmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIssueAlarmDlg)
	enum { IDD = IDD_ISSUEALARM };
	CListCtrl	m_List;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIssueAlarmDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIssueAlarmDlg)
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
