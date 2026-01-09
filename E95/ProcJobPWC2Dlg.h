#pragma once

#include "ProcJobCommon.h"
#include "InfoPanelMain.h"
#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWC2Dlg dialog

class CProcJobPWC2Dlg : public CResizableDialog
	, public CProcJobCommon {
	// Construction
public:
	// 	void LoadSPCParam();
	// 	void SetContentMap();
	short Step;

	CProcJobPWC2Dlg(CWnd* pParent = NULL);   // standard constructor
	~CProcJobPWC2Dlg();

	// Dialog Data
		//{{AFX_DATA(CProcJobPWC2Dlg)
	enum { IDD = IDD_PROCJOBPWC2DLG };
	CString	m_Message;
	CString	m_ContentMap;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobPWC2Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobPWC2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpFoupopenok();
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnComEndopen();
	afx_msg void OnComAbortopen();
	afx_msg void OnComExithome();
	afx_msg long OnComSlotMap(WPARAM wP, LPARAM lP);
	afx_msg long OnComSlotMapFail(WPARAM wP, LPARAM lP);
	afx_msg long OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg long OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP);
	afx_msg void OnCancelcarrier();
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
	int TPErrorRecovery(short errCode, BOOL bCarrierPresent);
	long OnSvrCancelcarrier(WPARAM wP, LPARAM lP);
	long OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	void Log(CString msg, short lvl = 5);
	void OpenFoup();
	CInfoPanelMain* GetParentWnd();

public:

	void CancelAndQuit();

public:
	void SlotmapVerification();

protected:
};