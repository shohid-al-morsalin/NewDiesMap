#pragma once

#include "CLPXChDat.h"
#include "ProcJobCommon.h"
#include "SRC/ResizableDialog.h"
#include "StatusReq.h"

class CProcJobSMICDlg : public CResizableDialog,
	public CStatusRequest, public CProcJobCommon {
	// Construction
public:

	// 	enum CANSTARTSTATE
	// 	{
	// 		NOTREADY, OKTOSTART, ABORT
	// 	};

	// 	short CJMONTimeoutCounter;
	// 	short AreYouThereRetryCount;
	// 	BOOL bOutOfServiceAlarmSet;

	// Implementation
public:
	void StartProcessRequest();
	BOOL AreYouThere();
	// 	CANSTARTSTATE CanStart();
	void CancelAndQuit();
	void WaitingForPJTimeOut();
	void ProceedProcessJobQueue();
	void SMWaitingForHostTimeout();
	void SlotmapVerification();
	// 	void OpenFoup();
	void AlarmAndCancelCarrier();
	void opAbortAphdLatch();
	void LatchFoup();
	void Message(CString msg, short lvl);
	void SetupLPStatus();
	void Refresh();
	int GetHWnd();
	void SetDataReqTimeOut(long dur);
	CProcJobSMICDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcJobSMICDlg)
	enum { IDD = IDD_PROCJOBSMICDLG };
	CButton	m_cLoad;
	CButton	m_cProceed;
	CListBox	m_cList;
	//}}AFX_DATA

	CLPXChDat Xch;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CProcJobSMICDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobSMICDlg)
	virtual BOOL OnInitDialog();
	afx_msg void RegisterLP();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnComCarrierarrived();
	afx_msg void OnComAbortaphdlatch();
	afx_msg void OnLatchCmpl();
	afx_msg LRESULT OnSvrProceedwithcarrierid(WPARAM wP, LPARAM lP);
	afx_msg void OnLpFoupopenok();
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnComAbortopen();
	afx_msg void OnComExithome();
	afx_msg void OnComLatchfail();
	afx_msg void OnComEndopen();
	afx_msg LRESULT OnComSlotmap(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComSlotmapFail(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrPjdefine(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrCjdefine(WPARAM wP, LPARAM lP);
	afx_msg void OnIpcHelloack();
	afx_msg void OnIpcDatareqack1();
	afx_msg LRESULT OnIpcDatareqack(WPARAM wP, LPARAM lP);
	afx_msg void OnProceed();
	afx_msg void OnComCarrierremoved();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};