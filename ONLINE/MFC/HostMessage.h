#pragma once

#include "resource.h"

class CHostMessage : public CDialog {
public:
	CHostMessage(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_CONSOLE_MSG };
	CEdit	m_HostMessage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAck();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	BOOL bAck;
	BOOL bSending;
	CString strMsg;
};