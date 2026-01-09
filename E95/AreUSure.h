#pragma once

class CAreUSure : public CDialog {
public:
	BOOL bRet;
	CAreUSure(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_RUSURE };
	CString	m_Message;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
