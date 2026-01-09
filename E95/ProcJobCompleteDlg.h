#pragma once

#include "SRC/ResizableDialog.h"
#include "CLPXChDat.h"

class CInfoPanelMain;

class CProcJobCompleteDlg : public CResizableDialog {
public:
	CInfoPanelMain* m_Parent;

	/**
	* Indicate which process job
	*/
	short ID;
	BOOL bOK;
	CLPXChDat Xch;

	// Construction
public:
	void Unclamp();
	void opAbort();
	void opProceed();
	void CloseFoup();
	CProcJobCompleteDlg(CWnd* pParent = NULL);   // standard constructor
	~CProcJobCompleteDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobCompleteDlg)
	enum { IDD = IDD_PROCJOBCOMPLETE };
	CListBox	m_cList;
	CButton	m_cUnload;
	//CButton	m_cProceed;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobCompleteDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobCompleteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnComButtonpushed();
	afx_msg void OnLpUnclampok();
	afx_msg void OnLpUnclampfail();
	afx_msg void OnComReleasecmpl();
	afx_msg void OnComCarrierremoved();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpFoupcloseok();
	afx_msg void OnLpFoupclosefail();
	afx_msg void OnComAbortclose();
	afx_msg void OnComEndClose();
	afx_msg void OnComClosenoreleasecmpl();
	afx_msg void OnComAbortaphdrelease();
	afx_msg void OnUnload();
	afx_msg void OnUnloadComplete();
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Refresh();
	void Message(CString msg, short lvl = 5);
	int TPErrorRecovery(short errCode, BOOL bCarrierPresent);
	BOOL NoOtherProcJobRuning();
	long OnComSlotMap(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	void Log(CString msg, short lvl = 5);
	void EnableButtonReport();
};
