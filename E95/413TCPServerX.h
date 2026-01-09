#if !defined(AFX_413TCPSERVER_H__7E440867_9A3A_4653_A852_181A4CBD830D__INCLUDED_)
#define AFX_413TCPSERVER_H__7E440867_9A3A_4653_A852_181A4CBD830D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 413TCPServer.h : header file
//

#include "resource.h"
#include "MsgItemProcessor.h"

/////////////////////////////////////////////////////////////////////////////
// C413TCPServer dialog
class CWinSock;
class CStreamSocket;

class CToolVar;
class C413SVar;
class CNCas;

class C413TCPServer : public CDialog
{
	// Attributes
private:
	CMsgItemProcessor MsgProc;

	CWinSock * m_pWinSock; // WinSock sub-system startup/.shutdown
	CStreamSocket * m_pStreamSrv; // Stream socket to wait for connections on
	CStreamSocket * m_pStream; // Stream socket to receive from
	
	int AckWnd;
	int Ack;
	int Nak;

	WPARAM wP;
	LPARAM lP;

// Construction
public:
	void SubmitProcessJob(CNCas *pCas);
	void Command413(CMsgItem *pMsg);
	BOOL CollectData(CToolVar &SV);
	BOOL CollectData(C413SVar &SV);
	void SendTV(CMsgItem *pM);
	void SendSV(CMsgItem *pMsg);
	void SendSV1(CMsgItem *pM);
	void Message(CString msg, short lvl=5);
	void Log(CString msg, short lvl = 5);
	BOOL InitiateTCPStuff();
	void KillTCPStuff();
	void DispatchMsg(CMsgItem *pMsg);

	C413TCPServer(CWnd* pParent = NULL);   // standard constructor
	~C413TCPServer();

// Dialog Data
	//{{AFX_DATA(C413TCPServer)
	enum { IDD = IDD_413TCPSERVER };
	CListBox	m_cList;
	//}}AFX_DATA

	CWnd * pWnd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C413TCPServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C413TCPServer)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LONG OnStreamSrv(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnStream(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#define WM_USER_STREAMSRV (WM_USER + 1)
#define WM_USER_STREAM (WM_USER + 2)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_413TCPSERVER_H__7E440867_9A3A_4653_A852_181A4CBD830D__INCLUDED_)
