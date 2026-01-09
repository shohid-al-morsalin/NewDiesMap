#if !defined(AFX_MESSAGEDISPATCHER_H__5997690C_0AC5_453E_AF9E_99FCD45277B4__INCLUDED_)
#define AFX_MESSAGEDISPATCHER_H__5997690C_0AC5_453E_AF9E_99FCD45277B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageDispatcher.h : header file
//

#include "resource.h"
#include "..\SRC\ResizableDialog.h"
#include "..\SERIAL\Queue.h"	// Added by ClassView
#include "MsgItemProcessor.h"

//class CBEME95;
class CMsgItem;

/////////////////////////////////////////////////////////////////////////////
// CMessageDispatcher dialog

class CWinSock;
class CStreamSocket;
class CFFUFailDlg;

class CMessageDispatcher : public CResizableDialog
{
//	DECLARE_DYNAMIC(CMessageDispatcher)
		
private:
	CMsgItemProcessor MsgProc;

	CWinSock * m_pWinSock; // WinSock sub-system startup/.shutdown
	CStreamSocket * m_pStream; // Stream socket to receive from
	char m_pszServer[64]; // host name or IP address of stream server

	BOOL bFirst;
	long tOUT;
	// Attributes
public:
	enum CONNECTSTATUS { NOTCONNECTED, ATTEMPTCONNECT, CONNECTED };
	CONNECTSTATUS ConnectStatus;

	BOOL bOnScreen;
	CQueue RbtCmdQue;
	CFFUFailDlg * pFFUFailDlg;

	CString sEventData;
	CString sRobotCmd;
	CString sScanData;

	int CallerWnd, CallerAck, CallerNak;

	enum DSPCHRID { DSPCRNONE, DSPCR900, DSPCR128, DSPCR413 };
	DSPCHRID ID;

	// Construction
public:
	void AttemptReconnection();
	void ProcessEqpMsg(CMsgItem *pMsg);
	BOOL CanTransfer(CMsgItem *pMsg);
	void RejectRbtCmd(CMsgItem *pMsg);
	void AttemptConnection(CString Addr, short port);
	void ProcessSMOLMsg(CMsgItem *pMsg);
	void ProcessCommandAck(CMsgItem *pMsg);
	void ProcessDataReqAck(CMsgItem *pMsg);
	BOOL DispatchMsg(CMsgItem *pMsg);
	BOOL TCPSend(CMsgItem *pMsg);
	BOOL TCPDisconnect();
	BOOL TCPReconnect();
	BOOL TCPConnect();
	void ProcessHelloAck(CMsgItem *pMsg);
	BOOL ProcessComplete(CMsgItem *pMsg);
	void Log(CString msg, short lvl=5);
	void Message(CString msg, short lvl=5);
	void ProcessEventData(CMsgItem *pMsg);
//	CString GetTimeStamp();
	void ProcessScanData(CMsgItem *pMsg);
//	void ProcessScanData128(WPARAM wP, LPARAM lP);
//	void ProcessScanData900(WPARAM wP, LPARAM lP);
	void ProcessRbtCmd(CMsgItem *pMsg);
//	CMsgItem * GetMsgItem(int ID);
	BOOL Send(CMsgItem *pMsg);
//	BOOL Send128(CMsgItem *pMsg);
//	BOOL Send900(CMsgItem *pMsg);
//	BOOL Start128();
//	BOOL Start900();
//	CBEME95 * pE95BEM;
	CMessageDispatcher(CWnd* pParent = NULL);   // standard constructor
	~CMessageDispatcher();

// Dialog Data
	//{{AFX_DATA(CMessageDispatcher)
	enum { IDD = IDD_DISPATCHER };
	CListBox	m_cList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDispatcher)
	protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageDispatcher)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbtXferok();
	afx_msg void OnRbtXferfail();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnStream(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnConnect();
	afx_msg void OnAreyouthere();
	afx_msg void OnIpcHelloack();
	afx_msg void OnIpcHellonak();
	afx_msg void OnIpcHelloack1();
	afx_msg void OnIpcHellonak1();
	afx_msg LRESULT OnUMDspchrMsgInQue(WPARAM wP, LPARAM lP);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	void OnUM128MsgInQue(WPARAM wP, LPARAM lP);
//	void OnUM900MsgInQue(WPARAM wP, LPARAM lP);
};

#define WM_USER_STREAM (WM_USER + 1)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEDISPATCHER_H__5997690C_0AC5_453E_AF9E_99FCD45277B4__INCLUDED_)
