#pragma once


// CSocketWndInvisible dialog

class CSocketWndInvisible : public CDialog
{
	DECLARE_DYNAMIC(CSocketWndInvisible)
	SOCKADDR_IN m_addr;
	SOCKET m_s;
	BOOL bOnlineSelected;
//	void Log(CString &Msg);
	void Log(const CString &Msg,short LogLvl = 5);
	BOOL Socket_Initialization();
public:
	CSocketWndInvisible(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSocketWndInvisible();
	afx_msg LONG OnHostMsg( WPARAM wParam, LPARAM lParam);
	afx_msg LONG UpdateAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnAsyncSelect( WPARAM wParam, LPARAM lParam);
    afx_msg LONG OnChangeMode( WPARAM wParam , LPARAM lParam);
	afx_msg LONG OnDoneRBHome( WPARAM wParam , LPARAM lParam );

// Dialog Data
	enum { IDD = IDD_ONLINE_SOCK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
