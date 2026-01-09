#pragma once

#include "StatusReq.h"
#include "ProcJobCommon.h"
#include "SRC/ResizableDialog.h"
#include "SRC/SortListCtrl.h"
#include "CLPXChDat.h"

/////////////////////////////////////////////////////////////////////////////
// CProcJobDlg dialog

class CXTabCtrl;
class CInfoPanelMain;

class CProcJobDlg : public CResizableDialog,
	public CStatusRequest, public CProcJobCommon {
	// Construction
public:
	CProcJobDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobDlg)
	enum { IDD = IDD_PROCESSJOB };
	CButton	m_cRecover;
	CButton	m_cLoad;
	CButton	m_cAbort;
	CListBox	m_cList;
	CButton	m_cLoadPos;
	CButton	m_cIcon;
	CButton	m_cProceed;
	CButton	m_cCancel;
	CEdit	m_cCarrierID;
	CString	m_RecipeName;
	//}}AFX_DATA

	BOOL bIncomAlrm;

	CLPXChDat Xch;
	CXTabCtrl* pXTab;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CProcJobDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CProcJobDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnComLoadcompleted();
	afx_msg void OnComCarrierarrived();
	afx_msg void OnComButtonpushed();
	afx_msg void OnComCarrierremoved();
	afx_msg void OnComCIDReadBypass();
	afx_msg void OnComCIDReadFailed();
	afx_msg void OnLatchFoupOK();
	afx_msg void OnLatchFoupFail();
	afx_msg void OnCidReadok();
	afx_msg void OnComNonhsmscarrierarrived();
	afx_msg void OnComEqpoutofservice();
	afx_msg void OnComLpoutofservice();
	afx_msg void OnBnClickedProceed();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnComNonhsmscarrierremoved();
	afx_msg void OnComAbortaphdlatch();
	afx_msg void OnClear();
	afx_msg void OnChangeCarrierid();
	afx_msg void OnLoad();
	afx_msg void OnLoadpos();
	afx_msg void OnLpFoupopenok();
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnComEndopen();
	afx_msg void OnComAbortopen();
	afx_msg void OnComExithome();
	afx_msg void OnCancel();
	afx_msg void OnProceed();
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
	afx_msg void OnIpcHelloack();
	afx_msg LRESULT OnIpcDatareqack(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnIpcDatareqnak(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComSlotMap(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComSlotMapFail(WPARAM wP, LPARAM lP);
	//afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrProceedwithcarrierid(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrPjdefine(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrCjdefine(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrCancelcarrier(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP);
	afx_msg void OnAbort();
	afx_msg void RegisterLP();
	afx_msg void OnComLatchfail();
	afx_msg void OnComNopod();
	afx_msg void OnRecover();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnCarrierRemoved(WPARAM wP, LPARAM lP);
	long OnCarrierArrived(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()

public:
	void Message(CString msg, short lvl = 5);
	void StartProcessRequest();
	void SetDataReqTimeOut(long dur);
	int GetHWnd();

public:
	void SlotmapVerification();
	void Log(CString msg, short lvl = 5);
	void OnLatchCmpl();
	long OnComCIDReadOK(WPARAM wP, LPARAM lP);

	void opCIDReadBypass();
	void opCIDReadOK(char* carrierID);
	void opCIDReadFail();
	void OpCarrierremoved();

	void LatchFoup();

	CInfoPanelMain* GetParentWnd();

public:
	void OnHoavbl();
	void Refresh();
	int TPErrorRecovery(short errCode, BOOL bCarrierPresent);
	void WaitingForPJTimeOut();
	void SMWaitingForHostTimeout();
	void IDWaitingForHostTimeout();
	void IDVerificationFail();
	void AlarmAndCancelCarrier(BOOL bCancelCarrier);
	void ProceedProcessJobQueue();
	void ProceedToOpenFOUP();
	BOOL IsActiveJob(short port);
	void SetupLPStatus();

	void OnRightViewFolderSelected(CString strPath, UINT index);
	void opAbortAphdLatch();

	void CancelAndQuit();
	BOOL AreYouThere();
};