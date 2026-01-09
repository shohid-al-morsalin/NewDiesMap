#pragma once
#include "xtabctrl.h"

#include "..\SRC\ResizableDialog.h"

class COnlineSubstrateDlg;
class COnlineAlarmDlg;
class COnlineECIDDlg;
class COnlineCJDlg;
class COnlinePJDlg;
class COnlineLogsDlg;
class COnlineSVIDDlg;
class COnlineModeDlg;
class COnlineHSMSDlg;
class COnlineEventsDlg;

class COnLineDlg : public CResizableDialog {
public:
	COnLineDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnLineDlg();

	void Log(CString Msg, short lvl = 5);
public:
	afx_msg LONG OnHostMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LONG UpdateAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnChangeMode(WPARAM wParam, LPARAM lParam);

	// Dialog Data
		//{{AFX_DATA(COnLineDlg)
	enum { IDD = IDD_ONLINEDLG };
	//}}AFX_DATA

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnLineDlg)
public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	// Generated message map functions
	//{{AFX_MSG(COnLineDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMsgEstcomm();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void ClearAll();
	CXTabCtrl m_tabctrl;
	COnlineModeDlg* m_Mode;
	COnlineSVIDDlg* m_SVIDDlg;
	COnlineEventsDlg* m_EventDlg;
	COnlineCJDlg* m_CJDlg;
	COnlinePJDlg* m_PJDlg;
	COnlineLogsDlg* m_LogsDlg;
	COnlineAlarmDlg* m_AlarmDlg;
	COnlineECIDDlg* m_ECIDDlg;
	COnlineSubstrateDlg* m_SubstDlg;
};
