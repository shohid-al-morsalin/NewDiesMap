#pragma once
// StreamSocket.h : header file
//
#include <afxmt.h>
#include "CWinSock.h"

// structure used for stream socket read/write queue
struct STREAMDATA {
	int nLen;
	char* pData;
};

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket window

class CStreamSocket : public CWnd {
	CCriticalSection CS;
private:
	CWnd* m_pParentWnd; // window to receive event notification
	UINT m_uMsg; // message to send to m_pParentWnd on event
	SOCKET m_s; // socket handle
	SOCKADDR_IN m_sinLocal; // name bound to socket m_s
	SOCKADDR_IN m_sinRemote; // name on other side of m_s
	int m_nLastError; // last WinSock error
	BOOL m_bServer; // TRUE if socket m_s is bound to a name
	CPtrList m_listWrite; // data waiting to be sent
	CPtrList m_listRead; // data read

// Construction
public:
	int CreateSocket(int nLocalPort);
	int CreateSocket(LPSTR pszLocalService = NULL);
	int DestroySocket();
	int Connect(LPSTR pszRemoteName, int nRemotePort);
	int Connect(LPSTR pszRemoteName, LPSTR pszRemoteService);
	int Connect(LPSOCKADDR_IN psinRemote);
	int Accept(CStreamSocket* pStreamSocket);
	int GetPeerName(LPSOCKADDR_IN psinRemote);
	int LastError() { return m_nLastError; }
private:
	void InitVars(BOOL bInitLastError = TRUE);
	LONG HandleRead(WPARAM wParam, LPARAM lParam);
	LONG HandleWrite(WPARAM wParam, LPARAM lParam);

	// Attributes
public:

	// Operations
public:
	STREAMDATA* Read(LPINT pnLen);
	int Write(STREAMDATA* pData);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CStreamSocket)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void Log(CString msg, short lvl);
	BOOL bReadErrorSend;
	CStreamSocket(CWnd* pParentWnd, UINT uMsg);
	virtual ~CStreamSocket();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStreamSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	LONG OnWinSockEvent(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

void CWinSockErrorBox(int nError, LPSTR pszMessage = NULL);
