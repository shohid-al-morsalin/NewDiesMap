// StreamSocket.cpp : implementation file
//

#include "stdafx.h"
#include "StreamSocket.h"
#include "..\SRC\BMake.h"

#include "GlobalDeviceContainer.h"

#define CWINSOCK_EVENT_NOTIFICATION WM_USER + 999

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket

///////////////////////////////////////////////////////////////////////////////
// CStreamSocket constructor()
//
CStreamSocket::CStreamSocket(CWnd* pParentWnd, UINT uMsg) {
	m_pParentWnd = pParentWnd;
	ASSERT(m_pParentWnd != NULL);
	m_uMsg = uMsg;
	ASSERT(m_uMsg != 0);
	bReadErrorSend = FALSE;
	InitVars();
}

CStreamSocket::~CStreamSocket() {}

BEGIN_MESSAGE_MAP(CStreamSocket, CWnd)
	//{{AFX_MSG_MAP(CStreamSocket)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(CWINSOCK_EVENT_NOTIFICATION, OnWinSockEvent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket message handlers
/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::InitVars()
//
// Initialize class member variables.
//
void CStreamSocket::InitVars(BOOL bInitLastError/*= TRUE*/) {
	if (bInitLastError) {
		m_nLastError = 0;
	}
	m_s = INVALID_SOCKET;
	memset(&m_sinLocal, 0, sizeof(SOCKADDR_IN));
	memset(&m_sinRemote, 0, sizeof(SOCKADDR_IN));
	m_bServer = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::CreateSocket()
//
// Create a hidden window that will receive asynchronous messages
// from WinSock. Also creates a socket and optionally binds it to
// a name if the socket is a server socket.
//
// This version of the CreateSocket() function takes a
// port number, in host order, as input. A port number
// should only be specified if the socket is to be bound
// to a certain port. If you don’t care which port is
// assigned to the socket, just call CreateSocket() without
// any parameter, causing CreateSocket(NULL) to be called.
//
int CStreamSocket::CreateSocket(int nLocalPort) {
	// if this version of the function is being called,
	// a valid port number must be specified
	if (nLocalPort <= 0) {
		return CWINSOCK_PROGRAMMING_ERROR;
	}
	// convert the port number into a string and
	// call the version of CreateSocket() which
	// accepts a string
	char pszLocalService[18];
	_itoa(nLocalPort, pszLocalService, 10);
	return CreateSocket(pszLocalService);
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::CreateSocket()
//
// Create a hidden window that will receive asynchronous messages
// from WinSock. Also creates a socket and optionally binds it to
// a name if the socket is a server socket.
//
// This version of the CreateSocket() function takes a
// string containing a service name or port number.
// A parameter should only be specified if the socket is to be
// bound to a certain port. If you don’t care which port is
// assigned to the socket, just call CreateSocket() without
// any parameter, causing CreateSocket(NULL) to be called.
//
int CStreamSocket::CreateSocket(LPSTR pszLocalService/*= NULL*/) {
	int nStatus = CWINSOCK_NOERROR;
	while (1) {
		// Make sure the socket isn’t already created.
		// If the socket handle is valid, return from this
		// function right away so the existing parameters of
		// the object are not tampered with.
		if (m_s != INVALID_SOCKET)
			return CWINSOCK_PROGRAMMING_ERROR;
		InitVars();
		// create the hidden window
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = 100;
		rect.bottom = 100;
		if (Create(NULL, NULL, WS_OVERLAPPEDWINDOW, rect, m_pParentWnd, 0) == 0) {
			nStatus = CWINSOCK_WINDOWS_ERROR;
			break;
		}
		// create the socket
		m_s = socket(PF_INET, SOCK_STREAM, 0);
		if (m_s == INVALID_SOCKET) {
			m_nLastError = WSAGetLastError();
			nStatus = CWINSOCK_WINSOCK_ERROR;
			DestroyWindow();
			break;
		}
		// If pszLocalService is not NULL, this is a server socket
		// that will accept data on the specified port.
		if (pszLocalService != NULL) {
			// this socket is bound to a port number
			// so set the server flag
			m_bServer = TRUE;
			// assign the address family
			m_sinLocal.sin_family = AF_INET;
			// assign the service port (may have to do a database lookup
			// if a service port number was not specified)
			m_sinLocal.sin_port = htons(atoi(pszLocalService));
			if (m_sinLocal.sin_port == 0) {
				LPSERVENT pSent = getservbyname(pszLocalService, "tcp");
				if (pSent == NULL) {
					m_nLastError = WSAGetLastError();
					nStatus = CWINSOCK_WINSOCK_ERROR;
					closesocket(m_s);
					DestroyWindow();
					break;
				}
				m_sinLocal.sin_port = pSent->s_port;
			}
			// assign the IP address
			m_sinLocal.sin_addr.s_addr = htonl(INADDR_ANY);
			// bind the server socket to the name containing the port
			if (bind(m_s, (LPSOCKADDR)&m_sinLocal, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
				m_nLastError = WSAGetLastError();
				nStatus = CWINSOCK_WINSOCK_ERROR;
				closesocket(m_s);
				DestroyWindow();
				break;
			}
		}
		// start asynchronous event notification
		long lEvent;
		if (m_bServer) {
			lEvent = FD_READ | FD_WRITE | FD_ACCEPT | FD_CLOSE;
		}
		else {
			lEvent = FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE;
		}
		if (WSAAsyncSelect(m_s, m_hWnd, CWINSOCK_EVENT_NOTIFICATION, lEvent) ==
			SOCKET_ERROR) {
			m_nLastError = WSAGetLastError();
			nStatus = CWINSOCK_WINSOCK_ERROR;
			closesocket(m_s);
			DestroySocket();
			break;
		}
		// if this is a server, listen for client connections
		if (m_bServer) {
			if (listen(m_s, 3) == SOCKET_ERROR) {
				m_nLastError = WSAGetLastError();
				nStatus = CWINSOCK_WINSOCK_ERROR;
				closesocket(m_s);
				DestroySocket();
				break;
			}
		}
		break;
	}
	// if anything failed in this function, set the
	// socket variables appropriately
	if (nStatus != CWINSOCK_NOERROR) {
		InitVars(FALSE);
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::Connect()
//
// Connect the client socket to a server specified by the name and port.
//
// This version of the Connect() function takes a pointer to a
// string representing the host name to send the data to and
// an integer representing the port number to connect to.
//
int CStreamSocket::Connect(LPSTR pszRemoteName, int nRemotePort) {
	// convert the port number into a string and
	// call the version of Connect() which accepts
	// a string service name or number
	char pszRemoteService[18];
	_itoa(nRemotePort, pszRemoteService, 10);
	return Connect(pszRemoteName, pszRemoteService);
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::Connect()
//
// Connect the client socket to a server specified by the name and
// service name or port.
//
// This version of the Connect() function takes a pointer to a
// string representing the host name to send the data to and
// an integer representing the service name or port number to
// connect to.
//
int CStreamSocket::Connect(LPSTR pszRemoteName, LPSTR pszRemoteService) {
	int nStatus = CWINSOCK_NOERROR; // error status
	LPHOSTENT pHent; // pointer to host entry structure
	LPSERVENT pSent; // pointer to service entry structure
	SOCKADDR_IN sinRemote; // Internet address of destination
	while (1) {
		// assign the address family
		sinRemote.sin_family = AF_INET;
		// assign the service port (may have to do a database lookup
		// if a service port number was not specified)
		sinRemote.sin_port = htons(atoi(pszRemoteService));
		if (sinRemote.sin_port == 0) {
			pSent = getservbyname(pszRemoteService, "tcp");
			if (pSent == NULL) {
				m_nLastError = WSAGetLastError();
				nStatus = CWINSOCK_WINSOCK_ERROR;
				break;
			}
			sinRemote.sin_port = pSent->s_port;
		}
		// assign the IP address (may have to do a database lookup
		// if a dotted decimal IP address was not specified)
		sinRemote.sin_addr.s_addr = inet_addr(pszRemoteName);
		if (sinRemote.sin_addr.s_addr == INADDR_NONE) {
			pHent = gethostbyname(pszRemoteName);
			if (pHent == NULL) {
				m_nLastError = WSAGetLastError();
				nStatus = CWINSOCK_WINSOCK_ERROR;
				break;
			}
			sinRemote.sin_addr.s_addr = *(u_long*)pHent->h_addr;
		}
		// call the version of Connect() that takes an
		// Internet address structure
		return Connect(&sinRemote);
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::Connect()
//
// Connect the client socket to a server specified by the
// Internet address.
//
// This version of the Connect() function takes a pointer
// to an Internet address structure to connect to.
//
int CStreamSocket::Connect(LPSOCKADDR_IN psinRemote) {
	int nStatus = CWINSOCK_NOERROR;
	while (1) {
		// only clients should call connect
		if (m_bServer) {
			nStatus = CWINSOCK_PROGRAMMING_ERROR;
			break;
		}
		// copy the Internet address of the remote server to connect to
		memcpy(&m_sinRemote, psinRemote, sizeof(SOCKADDR_IN));
		// attempt the asynchronous connect
		if (connect(m_s, (LPSOCKADDR)&m_sinRemote, sizeof(SOCKADDR_IN)) ==
			SOCKET_ERROR) {
			m_nLastError = WSAGetLastError();
			if (m_nLastError == WSAEWOULDBLOCK) {
				m_nLastError = 0;
			}
			else {
				nStatus = CWINSOCK_WINSOCK_ERROR;
			}
			break;
		}
		break;
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::Accept()
//
// Accept a connection request from a client.
//
// This function takes a pointer to a CStreamSocket object. This
// pointer will become the newly connected socket.
//
int CStreamSocket::Accept(CStreamSocket* pStreamSocket) {
	int nStatus = CWINSOCK_NOERROR;
	while (1) {
		// must have valid CStreamSocket object pointer passed in
		if (pStreamSocket == NULL) {
			ASSERT(0);
			nStatus = CWINSOCK_PROGRAMMING_ERROR;
			break;
		}
		// only servers should call accept
		if (!m_bServer) {
			nStatus = CWINSOCK_PROGRAMMING_ERROR;
			break;
		}
		// Make sure the socket isn’t already created.
		// If the socket handle is valid, return from this
		// function right away so the existing parameters of
		// the object are not tampered with.
		if (pStreamSocket->m_s != INVALID_SOCKET)
			return CWINSOCK_PROGRAMMING_ERROR;
		pStreamSocket->InitVars();
		// create the hidden window
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = 100;
		rect.bottom = 100;
		if (pStreamSocket->Create(NULL, NULL, WS_OVERLAPPEDWINDOW, rect, pStreamSocket->m_pParentWnd, 0) == 0) {
			nStatus = CWINSOCK_WINDOWS_ERROR;
			break;
		}
		// accept the client connection
		pStreamSocket->m_s = accept(m_s, NULL, NULL);
		if (pStreamSocket->m_s == INVALID_SOCKET) {
			m_nLastError = WSAGetLastError();
			nStatus = CWINSOCK_WINSOCK_ERROR;
			pStreamSocket->DestroyWindow();
			break;
		}
		// start asynchronous event notification
		long lEvent;
		lEvent = FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE;
		if (WSAAsyncSelect(pStreamSocket->m_s, pStreamSocket->m_hWnd,
			CWINSOCK_EVENT_NOTIFICATION, lEvent) == SOCKET_ERROR) {
			m_nLastError = WSAGetLastError();
			nStatus = CWINSOCK_WINSOCK_ERROR;
			closesocket(pStreamSocket->m_s);
			pStreamSocket->DestroySocket();
			break;
		}
		break;
	}
	// if anything failed in this function, set the
	// socket variables appropriately
	if (nStatus == CWINSOCK_WINSOCK_ERROR) {
		pStreamSocket->InitVars(FALSE);
	}
	else if (nStatus == CWINSOCK_NOERROR) {
		// notify the parent if the connection was accepted successfully
		pStreamSocket->m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_YOU_ARE_CONNECTED);
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::GetPeerName()
//
// Copies the Internet address of the other end of the socket
// connection into the pointer provided.
// Useful for server’s to use after an Accept().
//
int CStreamSocket::GetPeerName(LPSOCKADDR_IN psinRemote) {
	int nStatus = CWINSOCK_NOERROR;
	int nLen = sizeof(SOCKADDR_IN);
	// make sure the listening socket doesn’t call this function
	if (m_bServer) {
		nStatus = CWINSOCK_PROGRAMMING_ERROR;
	}
	else if (getpeername(m_s, (LPSOCKADDR)psinRemote, &nLen) == SOCKET_ERROR) {
		m_nLastError = WSAGetLastError();
		nStatus = CWINSOCK_WINSOCK_ERROR;
	}
	return (nStatus);
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::OnWinSockEvent()
//
// Called when there is an asynchronous event on the socket.
//
LONG CStreamSocket::OnWinSockEvent(WPARAM wParam, LPARAM lParam) {
	// check for an error
	if (WSAGETSELECTERROR(lParam) != 0) {
		return 0L;
	}
	// what event are we being notified of?
	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_READ:
		return HandleRead(wParam, lParam);
		break;
	case FD_WRITE:
		return HandleWrite(wParam, lParam);
		break;
	case FD_ACCEPT:
		// tell the parent window that a client would like to connect
		// to the server socket
		m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_READY_TO_ACCEPT_CONNECTION, 128);
		break;
	case FD_CONNECT:
		// tell the parent window that the socket has connected
		m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_YOU_ARE_CONNECTED);
		break;
	case FD_CLOSE:
		// check for more data queued on the socket
		// (don’t tell the application that the socket is closed
		// until all data has been read and notification has been posted)
		if (HandleRead(wParam, lParam)) {
			// fake the close event to try again
			PostMessage(CWINSOCK_EVENT_NOTIFICATION, wParam, lParam);
			break;
		}
		// tell the parent window that the socket is closed
		m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_LOST_CONNECTION);
		break;
	default:
		// this should never happen
		ASSERT(0);
		break;
	}
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::HandleRead()
//
// Called when there is an asynchronous read event on the socket.
//
// If the read was successful, the data and its length are stored
// in the read queue. Upon a successful read, the application
// window using this object is then notified with the m_uMsg message
// (wParam set to CWINSOCK_DONE_READING; lParam set to the number of
// data chunks in the read queue). At this point, the application
// should call Read(). If the read fails for some reason, the m_uMsg
// is sent with wParam set to CWINSOCK_ERROR_READING.
//
LONG CStreamSocket::HandleRead(WPARAM wParam, LPARAM lParam) {
	//	Sleep(5); // wait till sender catch up
	while (1) {
		//		Sleep(15); // wait till sender catch up
				// receive data
		int nLen = 0;
		int nBytesRead = recv(m_s, (LPSTR)&nLen, sizeof(int), MSG_PEEK);
		//		TRACE2("RHdr (%d)-(%d)\n",nBytesRead,nLen);
		if (nBytesRead == SOCKET_ERROR) {
			// if the error is just that the read would block,
			// don’t do anything; we’ll get another FD_READ soon
			m_nLastError = WSAGetLastError();
			if (m_nLastError == WSAEWOULDBLOCK) {
				m_nLastError = 0;
			}
			else {
				// tell the parent that a data read failed
				char* pData = new char[4096];
				while (recv(m_s, (LPSTR)pData, 4096, 0) > 0); // flush all in the buffer
				delete[] pData;
				if (!bReadErrorSend) {
					bReadErrorSend = TRUE;
					m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
				}
			}
			break;
		}
		// make sure some data was read
		if (nBytesRead == 0) {
			break;
		}
		if (nBytesRead != sizeof(int)) {
			break;
		}
		if (nLen < 1) {
			// tell the parent that a data read failed
			char* pData = new char[4096];
			while (recv(m_s, (LPSTR)pData, 4096, 0) > 0); // flush all in the buffer
			delete[] pData;
			if (!bReadErrorSend) {
				bReadErrorSend = TRUE;
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			//			ASSERT(0);
			break;
		}
		//		TRACE1("LEN %d\n",nLen);
		char* pData = new char[nLen + 16];
		if (!pData) {
			// tell the parent that a data read failed
			if (!bReadErrorSend) {
				bReadErrorSend = TRUE;
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			//			ASSERT(0);
			break;
		}
		char* pDataRemaining = pData;
		int nLenRemaining = nLen;
		short nOK = -1;
		int tout = 0;
		while (1) {
			//			TRACE2("Rmn (%d)_(%d)\n",nLen,nLenRemaining);
			//			Sleep(30);  // wait till sender catch up
			nBytesRead = recv(m_s, (LPSTR)pDataRemaining, nLenRemaining, 0);
			if (nBytesRead == SOCKET_ERROR) {
				m_nLastError = WSAGetLastError();
				if (m_nLastError == WSAEWOULDBLOCK) {
					tout++;
					if (tout < 1500) {
						Sleep(10);
						continue;
					}
					else {
						// free memory for incoming data
						if (!bReadErrorSend) {
							bReadErrorSend = TRUE;
							m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
						}
						//						ASSERT(0);
						break;
					}
				}
				// if the error is just that the read would block,
				// don’t do anything; we’ll get another FD_READ soon
				break;
			}
			// make sure some data was read
			if (nBytesRead == 0) {
				Sleep(10); // wait a while then try again
				continue;
			}
			if (nBytesRead == nLenRemaining) {
				nOK = 0;
				break;
			}
			else {
				nLenRemaining -= nBytesRead;
				pDataRemaining += nBytesRead;
			}
		}
		if (nOK < 0) {
			delete[] pData;
			// tell the parent that a data read failed
			if (!bReadErrorSend) {
				bReadErrorSend = TRUE;
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			break;
		}

		// successful read
		// allocate memory for incoming data
		STREAMDATA* pStreamData = new STREAMDATA;
		if (pStreamData == NULL) {
			// tell the parent that a possible data read failed
			if (!bReadErrorSend) {
				bReadErrorSend = TRUE;
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			// fake the event to try again
			PostMessage(CWINSOCK_EVENT_NOTIFICATION, m_s, WSAMAKESELECTREPLY(FD_READ, 0));
			break;
		}
		// add the data to the list
		char* p = pData;
		CMake::Conv(pStreamData->nLen, &p);
		pStreamData->pData = new char[pStreamData->nLen + 16];
		memcpy(pStreamData->pData, p, pStreamData->nLen - sizeof(int));
		delete[] pData;

		CSingleLock Lock(&CS);
		Lock.Lock();
		TRY
		{
			m_listRead.AddTail(pStreamData);
		}
			CATCH(CMemoryException, e) {
			delete pStreamData; // Delete this will also delete pData
			pStreamData = NULL;
			// tell the parent that a data read failed
			if (!bReadErrorSend) {
				bReadErrorSend = TRUE;
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			break;
		}
		END_CATCH
			// tell the parent that data has been read
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_DONE_READING, (LPARAM)m_listRead.GetCount());
		// 1 is returned if there is data so CStreamSocket::OnWinSockEvent()’s
		// FD_CLOSE handler will know when the socket can really be closed
		bReadErrorSend = FALSE;
	}
	return 0L;
}

/*
LONG CStreamSocket::HandleRead(WPARAM wParam, LPARAM lParam)
{
//	Sleep(5); // wait till sender catch up
	while (1)
	{
//		Sleep(15); // wait till sender catch up
		// receive data
		int nLen=0;
		int nBytesRead = recv(m_s, (LPSTR)&nLen, sizeof(int), MSG_PEEK);
//		TRACE2("RHdr (%d)-(%d)\n",nBytesRead,nLen);
		if (nBytesRead == SOCKET_ERROR)
		{
			// if the error is just that the read would block,
			// don’t do anything; we’ll get another FD_READ soon
			m_nLastError = WSAGetLastError();
			if (m_nLastError == WSAEWOULDBLOCK) {
				m_nLastError = 0;
			}
			else {
				// tell the parent that a data read failed
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			}
			break;
		}
		// make sure some data was read
		if (nBytesRead == 0)
		{
			break;
		}
		if (nBytesRead != sizeof(int)) {
			break;
		}
		if (nLen < 1)
		{
			// tell the parent that a data read failed
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			ASSERT(0);
			break;
		}
//		TRACE1("LEN %d\n",nLen);
		char * pData = new char [nLen+16];
		if (!pData)
		{
			// tell the parent that a data read failed
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			break;
		}
		char * pDataRemaining = pData;
		int nLenRemaining = nLen;
		short nOK = -1;
		while (1)
		{
//			TRACE2("Rmn (%d)_(%d)\n",nLen,nLenRemaining);
//			Sleep(30);  // wait till sender catch up
			nBytesRead = recv(m_s,(LPSTR)pDataRemaining,nLenRemaining,0);
			if (nBytesRead == SOCKET_ERROR)
			{
				// free memory for incoming data
				delete [] pData;
				pData = NULL;
				// if the error is just that the read would block,
				// don’t do anything; we’ll get another FD_READ soon
				m_nLastError = WSAGetLastError();
				if (m_nLastError == WSAEWOULDBLOCK) {
					m_nLastError = 0;
					nOK = 1;
				}
				else {
					// other errors
				}
				break;
			}
			// make sure some data was read
			if (nBytesRead == 0)
			{
				// free memory for incoming data
				delete [] pData;
				pData = NULL;
				ASSERT(0);
				break;
			}
			if (nBytesRead == nLenRemaining) {
				nOK = 0;
				break;
			}
			else {
				nLenRemaining -= nBytesRead;
				pDataRemaining += nBytesRead;
			}
		}
		if (nOK<0)
		{
			delete [] pData;
			// tell the parent that a data read failed
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			break;
		}
		if (nOK > 0)
		{
			break;
		}

		// successful read
		// allocate memory for incoming data
		STREAMDATA *pStreamData = new STREAMDATA;
		if (pStreamData == NULL)
		{
			// tell the parent that a possible data read failed
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
			// fake the event to try again
			PostMessage(CWINSOCK_EVENT_NOTIFICATION, m_s, WSAMAKESELECTREPLY(FD_READ, 0));
			break;
		}
		// add the data to the list
		char *p = pData;
		CMake::Conv(pStreamData->nLen,&p);
		pStreamData->pData = new char [pStreamData->nLen+16];
		memcpy(pStreamData->pData, p,pStreamData->nLen-sizeof(int));
		delete [] pData;

		CSingleLock Lock(&CS);
		Lock.Lock();
		if (Lock.IsLocked()) {
			TRY
			{
				m_listRead.AddTail(pStreamData);
//				TRACE("ADDED\n");
			}
			CATCH (CMemoryException, e)
			{
				delete pStreamData; // Delete this will also delete pData
				pStreamData = NULL;
				// tell the parent that a data read failed
				m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_READING);
				break;
			}
			END_CATCH
			// tell the parent that data has been read
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_DONE_READING, (LPARAM)m_listRead.GetCount());
			// 1 is returned if there is data so CStreamSocket::OnWinSockEvent()’s
			// FD_CLOSE handler will know when the socket can really be closed
		}
	}
	return 0L;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::HandleWrite()
//
// Called when there is an asynchronous write event on the socket.
//
// If there is data in the write queue waiting to be sent,
// a WinSock send is attempted. If the send is successful,
// a m_uMsg message is sent to the application window with
// wParam set to CWINSOCK_DONE_WRITING and lParam set to the
// address of the data that was sent. On send failure,
// wParam is set to CWINSOCK_ERROR_WRITING and lParam set to
// the address of the data which couldn’t be sent. In either
// case, the application may free the pointer pointing to
// the data or reuse that data buffer. It is possible for the
// entire amount of data to not be sent in one call to send().
// In this case, an attempt is made to send the remaining portion
// of that block of data the next time HandleWrite() is invoked.
//
//
LONG CStreamSocket::HandleWrite(WPARAM wParam, LPARAM lParam) {
	STREAMDATA* pStreamData; // pointer to stream data structure
	char* pData = NULL; // pointer to buffer to send
	int nLen = 0; // total length of buffer to send
	static char* pDataRemaining = NULL; // pointer into buffer to send
	static int nLenRemaining = 0; // number of bytes left to send
	CSingleLock Lock(&CS);
	Lock.Lock();
	while (1) {
		// check to see if there is any data to send
		if (m_listWrite.IsEmpty()) {
			break;
		}
		pStreamData = (STREAMDATA*)m_listWrite.GetHead(); // not RemoveHead()
		if (!pStreamData) {
			// nothing to write
			break;
		}

		// send the data
		BOOL bRemove = FALSE; // remove data from queue?

		pData = (char*)pStreamData->pData;
		nLen = pStreamData->nLen;
		pDataRemaining = pData;
		nLenRemaining = nLen;

		while (1) {
			int nBytesSent = send(m_s, (LPCSTR)pDataRemaining, nLenRemaining, 0);
			if (nBytesSent == SOCKET_ERROR) {
				// if the error is just that the send would block,
				// don’t do anything; we’ll get another FD_WRITE soon
				m_nLastError = WSAGetLastError();
				if (m_nLastError == WSAEWOULDBLOCK) {
					m_nLastError = 0;
					Sleep(100);
					continue;
				}
				else {
					bRemove = TRUE;
					m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_WRITING, (LPARAM)pStreamData->pData);
				}
				break;
			}
			else {
				// if data was sent, we must still check to see
				// if all the bytes were sent
				if (nBytesSent == nLenRemaining) {
					bRemove = TRUE;
					m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_DONE_WRITING, (LPARAM)pStreamData->pData);
					break;
				}
				else {
					// the complete buffer was not sent so adjust
					// these values accordingly
					pDataRemaining = (pDataRemaining + nBytesSent);
					nLenRemaining = nLenRemaining - nBytesSent;
					Log("Next TCP transmission", 1);
				}
			}
		}
		// if the data was completely sent or there was
		// a real error, remove the data from the queue
		if (bRemove) {
			//pStreamData->pData will be remove by CWINSOCK_DONE_WRITING
			m_listWrite.RemoveHead();
			delete pStreamData;
			pDataRemaining = NULL;
			nLenRemaining = 0;
		}
		Sleep(200); // Take a break [11/29/2007 YUEN]
	}
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CStreamSocket::DestroySocket()
//
// Close the socket, remove any queued data,
// and destroy the hidden window.
//
int CStreamSocket::DestroySocket() {
	int nStatus = CWINSOCK_NOERROR;
	// make sure the socket is valid
	if (m_s == INVALID_SOCKET) {
		nStatus = CWINSOCK_PROGRAMMING_ERROR;
	}
	else {
		// remove any data in the write queue
		while (!m_listWrite.IsEmpty()) {
			STREAMDATA* pStreamData = (STREAMDATA*)m_listWrite.RemoveHead();
			m_pParentWnd->PostMessage(m_uMsg, CWINSOCK_ERROR_WRITING, (LPARAM)pStreamData->pData);
			delete pStreamData;
		}
		// remove any data in the read queue
		while (!m_listRead.IsEmpty()) {
			STREAMDATA* pStreamData = (STREAMDATA*)m_listRead.RemoveHead();
			delete[] pStreamData->pData;
			delete pStreamData;
		}
		// close the socket and initialize variables
		closesocket(m_s);
		InitVars();
		// destroy the hidden window
		DestroyWindow();
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CWinSockErrorBox
//
void CWinSockErrorBox(int nError, LPSTR pszMessage/*= NULL*/) {
#define ERROR_BUF_LEN (1000)
	char pszError[ERROR_BUF_LEN];
	wsprintf(pszError, "WinSock error %d: ", nError);
	switch (nError) {
	case WSAEINTR:
		lstrcat(pszError, "Interrupted system call");
		break;
	case WSAEBADF:
		lstrcat(pszError, "Bad file number");
		break;
	case WSAEACCES:
		lstrcat(pszError, "Permission denied");
		break;
	case WSAEFAULT:
		lstrcat(pszError, "Bad address");
		break;
	case WSAEINVAL:
		lstrcat(pszError, "Invalid argument");
		break;
	case WSAEMFILE:
		lstrcat(pszError, "Too many open files");
		break;
	case WSAEWOULDBLOCK:
		lstrcat(pszError, "Operation would block");
		break;
	case WSAEINPROGRESS:
		lstrcat(pszError, "Operation now in progress");
		break;
	case WSAEALREADY:
		lstrcat(pszError, "Operation already in progress");
		break;
	case WSAENOTSOCK:
		lstrcat(pszError, "Socket operation on non-socket");
		break;
	case WSAEDESTADDRREQ:
		lstrcat(pszError, "Destination address required");
		break;
	case WSAEMSGSIZE:
		lstrcat(pszError, "Message too long");
		break;
	case WSAEPROTOTYPE:
		lstrcat(pszError, "Protocol wrong type for socket");
		break;
	case WSAENOPROTOOPT:
		lstrcat(pszError, "Protocol not available");
		break;
	case WSAEPROTONOSUPPORT:
		lstrcat(pszError, "Protocol not supported");
		break;
	case WSAESOCKTNOSUPPORT:
		lstrcat(pszError, "Socket type not supported");
		break;
	case WSAEOPNOTSUPP:
		lstrcat(pszError, "Operation not supported on socket");
		break;
	case WSAEPFNOSUPPORT:
		lstrcat(pszError, "Protocol family not supported");
		break;
	case WSAEAFNOSUPPORT:
		lstrcat(pszError, "Address family not supported by protocol family");
		break;
	case WSAEADDRINUSE:
		lstrcat(pszError, "Address already in use");
		break;
	case WSAEADDRNOTAVAIL:
		lstrcat(pszError, "Can’t assign requested address");
		break;
	case WSAENETDOWN:
		lstrcat(pszError, "Network is down");
		break;
	case WSAENETUNREACH:
		lstrcat(pszError, "Network is unreachable");
		break;
	case WSAENETRESET:
		lstrcat(pszError, "Network dropped connection on reset");
		break;
	case WSAECONNABORTED:
		lstrcat(pszError, "Software caused connection abort");
		break;
	case WSAECONNRESET:
		lstrcat(pszError, "Connection reset by peer");
		break;
	case WSAENOBUFS:
		lstrcat(pszError, "No buffer space available");
		break;
	case WSAEISCONN:
		lstrcat(pszError, "Socket is already connected");
		break;
	case WSAENOTCONN:
		lstrcat(pszError, "Socket is not connected");
		break;
	case WSAESHUTDOWN:
		lstrcat(pszError, "Can’t send after socket shutdown");
		break;
	case WSAETOOMANYREFS:
		lstrcat(pszError, "Too many references: can’t splice");
		break;
	case WSAETIMEDOUT:
		lstrcat(pszError, "Connection timed out");
		break;
	case WSAECONNREFUSED:
		lstrcat(pszError, "Connection refused");
		break;
	case WSAELOOP:
		lstrcat(pszError, "Too many levels of symbolic links");
		break;
	case WSAENAMETOOLONG:
		lstrcat(pszError, "File name too long");
		break;
	case WSAEHOSTDOWN:
		lstrcat(pszError, "Host is down");
		break;
	case WSAEHOSTUNREACH:
		lstrcat(pszError, "No route to host");
		break;
	case WSAENOTEMPTY:
		lstrcat(pszError, "Directory not empty");
		break;
	case WSAEPROCLIM:
		lstrcat(pszError, "Too many processes");
		break;
	case WSAEUSERS:
		lstrcat(pszError, "Too many users");
		break;
	case WSAEDQUOT:
		lstrcat(pszError, "Disc quota exceeded");
		break;
	case WSAESTALE:
		lstrcat(pszError, "Stale NFS file handle");
		break;
	case WSAEREMOTE:
		lstrcat(pszError, "Too many levels of remote in path");
		break;
	case WSAEDISCON:
		lstrcat(pszError, "Disconnect");
		break;
	case WSASYSNOTREADY:
		lstrcat(pszError, "Network sub-system is unusable");
		break;
	case WSAVERNOTSUPPORTED:
		lstrcat(pszError, "WinSock DLL cannot support this application");
		break;
	case WSANOTINITIALISED:
		lstrcat(pszError, "WinSock not initialized");
		break;
	case WSAHOST_NOT_FOUND:
		lstrcat(pszError, "Host not found");
		break;
	case WSATRY_AGAIN:
		lstrcat(pszError, "Non-authoritative host not found");
		break;
	case WSANO_RECOVERY:
		lstrcat(pszError, "Non-recoverable error");
		break;
	case WSANO_DATA:
		lstrcat(pszError, "Valid name, no data record of requested type");
		break;
	default:
		lstrcpy(pszError, "Not a WinSock error");
		break;
	}
	lstrcat(pszError, "\n");
	int n = lstrlen(pszError);
	if (pszMessage != NULL) {
		n += lstrlen(pszMessage);
	}
	if ((pszMessage != NULL) && (n < ERROR_BUF_LEN)) {
		lstrcat(pszError, pszMessage);
	}
	AfxMessageBox(pszError);
}

STREAMDATA* CStreamSocket::Read(LPINT pnLen) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	// check to see if there is data to retrieve
	if (!m_listRead.IsEmpty()) {
		// remove the stream data from the list
		return (STREAMDATA*)m_listRead.RemoveHead();
	}
	return NULL;
}

int CStreamSocket::Write(STREAMDATA* pData) {
	int nStatus = CWINSOCK_NOERROR;
	if (!pData) {
		nStatus = CWINSOCK_WINDOWS_ERROR;
		return nStatus;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();
	// add the data to the list
	TRY
	{
		m_listWrite.AddTail(pData);
	}
		CATCH(CMemoryException, e) {
		delete pData;
		nStatus = CWINSOCK_WINDOWS_ERROR;
		return nStatus;
	}
	END_CATCH
		// trigger the FD_WRITE handler to try to send
		PostMessage(CWINSOCK_EVENT_NOTIFICATION, m_s, WSAMAKESELECTREPLY(FD_WRITE, 0));
	return nStatus;
}

void CStreamSocket::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CStreamSocket");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}