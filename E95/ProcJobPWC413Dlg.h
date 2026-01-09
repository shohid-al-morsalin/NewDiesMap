#pragma once

#include "ProcJobPWC1DlgBase.h"

class CProcJobPWC413Dlg : public CProcJobPWC1DlgBase {
	// Construction
public:
	BOOL IsStress(CRecipe413* pRcp);
	void ShowStressButton(BOOL bShow);
	BOOL bRecipeSelected;

	void SetSlotMapInfo(char* p);
	void SendToQueue();
	void ProceedOK();
	void UserSelectionCheck();
	void Log(CString msg, short lvl = 5);
	BOOL CanProcess();
	// 	BOOL LoadRecipe();
	CProcJobPWC413Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcJobPWC413Dlg)
	enum { IDD = IDD_PROCJOBPWC413DLG };
	CButton	m_cProceed;
	CButton	m_cScanMore;
	CButton	m_cScanSecond;
	CButton	m_cScanFirst;
	CStatic	m_cCarrierID;
	CString	m_Recipe;
	CEdit	m_cLotID;
	CEdit	m_cOperator;
	BOOL	m_SelectAll;
	CButton	m_cSelectAll;
	//}}AFX_DATA
	BOOL	m_sl[25];
	BOOL	m_slEn[25];
	CButton	m_csl[25];
	CEdit	m_cID[25];

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CProcJobPWC413Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobPWC413Dlg)
	afx_msg void OnCancel();
	afx_msg void OnBrowserecipe();
	afx_msg void OnInspecrecipe();
	afx_msg void OnProcessing();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelectall();
	afx_msg void OnScanfirst();
	afx_msg void OnScansecond();
	afx_msg void OnScanmore();
	afx_msg void OnKillfocusWaferrepeat();
	afx_msg void OnKillfocusCassetterepeat();
	//}}AFX_MSG
	afx_msg LRESULT OnIpcTvdataack(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnIpcTvdatanak(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
