#pragma once

#include "SRC/ResizableDialog.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"

class CSocketWndInvisible;

class COnlineModeDlg : public CResizableDialog {
	long tout;
	int preState;
	CSharedMemQue m_gEvtQue;
	void KillAllTimer();
	void UpdateButtons();
	void DisableAllButtons();
	void OnOfflineChangeTimeout();
	void OnLocalChangeTimeout();
	void OnRemoteChangeTimeout();
	void Log(CString msg, short lvl = 5);

public:
	COnlineModeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineModeDlg();

	// Dialog Data
	//{{AFX_DATA(COnlineModeDlg)
	enum { IDD = IDD_ONLINEMODEDLG };
	CButton	m_cSeparate;
	CButton	m_cSelect;
	CButton	m_cLowlevel;
	CListBox	m_cList;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineModeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	CButton m_RemoteButt;
	CButton m_LocalButt;
	CButton m_OffButt;
	CStatic	m_ConnInfo;
	CButton	m_OnlineButt;
	CButton	m_BCR;
	// Generated message map functions
	//{{AFX_MSG(COnlineModeDlg)
	afx_msg void OnBnClickedGoremote();
	afx_msg void OnBnClickedGolocal();
	afx_msg void OnBnClickedGooffline();
	afx_msg void OnBnClickedButtonOnline();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSelect();
	afx_msg void OnSeparate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void Message(CString& msg, short lvl = 5);
	CString msg;
};
