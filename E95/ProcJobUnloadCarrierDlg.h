#pragma once

class CInfoPanelMain;

class CProcJobUnloadCarrierDlg : public CDialog {
	DECLARE_DYNAMIC(CProcJobUnloadCarrierDlg)

public:
	CInfoPanelMain* m_Parent;

	/**
	* Indicate which process job
	*/
	short ID;

	CProcJobUnloadCarrierDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobUnloadCarrierDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobUnloadCarrierDlg)
	enum { IDD = IDD_PROCJOBUNLOADCARRIERDLG };
	CString	m_Message;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobUnloadCarrierDlg)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	int TPErrorRecovery(short errCode, BOOL bCarrierPresent);
	void Message(CString msg, short lvl = 5);
	void Log(CString msg, short lvl = 5);
	void Unclamp();

	// Generated message map functions
	//{{AFX_MSG(CProcJobUnloadCarrierDlg)
	afx_msg void OnComButtonpushed();
	afx_msg void OnLpUnclampok();
	afx_msg void OnLpUnclampfail();
	afx_msg void OnComReleasecmpl();
	afx_msg void OnComCarrierremoved();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnComNovacuum();
	afx_msg void OnComWaferout();
	afx_msg void OnComVacuumlost();
	afx_msg void OnComSensortrip();
	afx_msg LRESULT OnComTp1timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComTp2timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComTp3timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComTp4timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComTp5timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComTp6timeout(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComLoste84signal(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComUnexpectede84signal(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnCarrierRemoved(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
