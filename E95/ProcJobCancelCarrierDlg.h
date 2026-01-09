#pragma once

#include "SRC/ResizableDialog.h"
#include "CLPXChDat.h"

// CProcJobCancelCarrierDlg dialog
class CInfoPanelMain;

class CProcJobCancelCarrierDlg : public CResizableDialog {
public:
	CInfoPanelMain* m_Parent;

	/**
	* Indicate which process job
	*/
	short ID;

	CLPXChDat Xch;

	CProcJobCancelCarrierDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobCancelCarrierDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobCancelCarrierDlg)
	enum { IDD = IDD_PROCJOBCANCELCARRIERDLG };
	CListBox	m_cList;
	CButton	m_cUnload;
	CButton	m_cRecover;
	CButton	m_cClose;
	CButton	m_cCancel;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobCancelCarrierDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CProcJobCancelCarrierDlg)
	afx_msg void OnProceed();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnComButtonpushed();
	afx_msg void OnLpUnclampok();
	afx_msg void OnLpUnclampfail();
	afx_msg void OnComReleasecmpl();
	afx_msg void OnComCarrierremoved();
	afx_msg void OnComNonHSMSCarrierremoved();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpFoupcloseok();
	afx_msg void OnLpFoupclosefail();
	afx_msg void OnComAbortclose();
	afx_msg void OnComEndClose();
	afx_msg void OnComClosenoreleasecmpl();
	afx_msg LRESULT OnComSlotMap(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnClose();
	afx_msg void OnRecovery();
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
	void Unclamp();
	void Refresh();
	void ProcessCarrierRemoved();
	int TPErrorRecovery(short errCode, BOOL bCarrierPresent);
	void Message(CString msg, short lvl = 5);
	void EnableButtonReport();
	void Log(CString msg, short lvl = 5);
	// 	void CloseFoupUnlatch();
	void CloseFoup();
	BOOL NoOtherProcJobRuning();
	UINT Next;
};
