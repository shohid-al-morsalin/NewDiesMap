#pragma once

#include "ResizableFormView.h"

class CInfoPanelWelcome : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelWelcome)

	BOOL bStartFault;

protected:
	CInfoPanelWelcome();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelWelcome();

public:
	BOOL AllStarted();
	DWORD StartBegin;
	CWinThread* pThread;
	void Log(CString msg, short lvl = 5);
	void StopSystem();
	static UINT  StartSystem(LPVOID pObj);

	CMgrStatus::STATUSFLAG StartDoorLockMgr();
	CMgrStatus::STATUSFLAG StopDoorLockMgr();
	CMgrStatus::STATUSFLAG StopLTWMgr();
	CMgrStatus::STATUSFLAG StartLTWMgr();
	CMgrStatus::STATUSFLAG StartIDRMgr();
	CMgrStatus::STATUSFLAG StopIDRMgr();
	CMgrStatus::STATUSFLAG StartIDRUnits();
	CMgrStatus::STATUSFLAG StopE95BemSvr();
	CMgrStatus::STATUSFLAG StartE95BemSvr();
	CMgrStatus::STATUSFLAG StartRbtMgr();
	CMgrStatus::STATUSFLAG StopRbtMgr();
	CMgrStatus::STATUSFLAG StartRbtUnit();
	CMgrStatus::STATUSFLAG StartLpMgr(HWND hWnd);
	CMgrStatus::STATUSFLAG StopLpMgr();
	CMgrStatus::STATUSFLAG StartLpUnits(HWND hWnd);
	void StopMgrs();
	void StartMgrs(HWND hWnd, CListBox* pMessage);
	void StartHardwareSystem(CListBox* pMessage);
	//{{AFX_DATA(CInfoPanelWelcome)
	enum { IDD = IDD_INFOPANELWELCOME };
	CListBox	m_cMessage;
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInfoPanelWelcome)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpStartfail1();
	afx_msg void OnLpStartfail2();
	afx_msg void OnCidStartfailed1();
	afx_msg void OnCidStartfailed2();
	afx_msg void OnRbtStartfail();
	afx_msg void OnRbtStartok();
	afx_msg void OnCidStarted1();
	afx_msg void OnCidStarted2();
	afx_msg void OnLpStartok1();
	afx_msg void OnLpStartok2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};