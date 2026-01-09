// FsmCoreSocket.cpp: implementation of the CFsmCoreSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FsmCoreSocket.h"
#include "HsmsParaMeter.h"
#include "..\GEM\CediListCtroller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFsmCoreSocket::CFsmCoreSocket() {
	m_hSocket = INVALID_SOCKET;
}

/*
CFsmCoreSocket::CFsmCoreSocket( int nPort )
{
	Initialize( INVALID_SOCKET );

	if( ! Socket_Create( nPort ) )
	{
	   throw XCannotCreate();
	}
}
*/

CFsmCoreSocket::CFsmCoreSocket(SOCKET h) {
	m_hSocket = INVALID_SOCKET;
	Initialize(h);
}

CFsmCoreSocket::~CFsmCoreSocket() {
	if (m_hSocket != INVALID_SOCKET) {
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void CFsmCoreSocket::Initialize(SOCKET h) {
	if (m_hSocket != INVALID_SOCKET) {
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	m_hSocket = h;
}

BOOL CFsmCoreSocket::Socket_Create(int nPort) {
	m_hSocket = ::socket(PF_INET, SOCK_STREAM, 0);
	if (m_hSocket == INVALID_SOCKET) {
		AfxMessageBox("m_hSocket == invalid");
		return FALSE;
	}
	if (!Socket_Initailization(nPort)) {
		AfxMessageBox(" Create initial fail");
		Socket_Close();
		return FALSE;
	}
	return TRUE;
}

BOOL CFsmCoreSocket::Socket_Initailization(int nPort) {
	ASSERT(m_hSocket != INVALID_SOCKET);

	SOCKADDR_IN SockAddr;

	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = INADDR_ANY;
	SockAddr.sin_port = ::htons(nPort);

	int r = ::bind(m_hSocket, (SOCKADDR*)&SockAddr, sizeof(SockAddr));

	if (r == 0) {
		SetIntOption(SOL_SOCKET, SO_DONTLINGER, 1);
		SetIntOption(SOL_SOCKET, SO_KEEPALIVE, 1);
		r = ::listen(m_hSocket, 5);
		if (r == 0) {
			return TRUE;
		}
	}
	else {
		CString dwError("");
		dwError.Format("Socket bind Error ! Error No = %d ", r);
		AfxMessageBox(dwError);
		return FALSE;
	}
	return  FALSE;
}

void CFsmCoreSocket::Socket_Close() {
	shutdown(m_hSocket, SD_BOTH);
	closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;
}

BOOL CFsmCoreSocket::SetIntOption(int level, int optname, int val) {
	return ::setsockopt(m_hSocket, level, optname,
		(char*)&val, sizeof(val));
}

SOCKET CFsmCoreSocket::Socket_Accept() {
	SOCKET h = ::accept(m_hSocket, NULL, NULL);
	return h;
}

BOOL CFsmCoreSocket::Socket_Connect(LPCTSTR lpszHostAddress, UINT nHostPort) {
	ASSERT(lpszHostAddress != NULL);                               // Create ? socket
	if (m_hSocket == INVALID_SOCKET) {
		m_hSocket = ::socket(PF_INET, SOCK_STREAM, 0);
		if (m_hSocket == INVALID_SOCKET) {
			return FALSE;
		}
	}
	SOCKADDR_IN sockAddr;	                                       // Fill address machinery of sockets.
	memset(&sockAddr, 0, sizeof(sockAddr));
#ifdef _UNICODE
	char buff[500];
	WideCharToMultiByte(CP_ACP, 0, lpszHostAddress, -1, buff, 500,
		NULL, NULL);
	LPSTR lpszAscii = buff;
#else
	LPSTR lpszAscii = (LPSTR)lpszHostAddress;
#endif
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
	if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL) {
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		else {
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}
	sockAddr.sin_port = htons((u_short)nHostPort);

	int r = ::connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));                                   // connects to peer
	if (r != SOCKET_ERROR) {
		ASSERT(r == 0);
		return TRUE;
	}

	int e = ::WSAGetLastError();
	ASSERT(e != WSAEWOULDBLOCK);

	return FALSE;
}

//============================================================================//
//  CLASS SEPERATE                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
CFmsRWUPCoreSocket::CFmsRWUPCoreSocket(SOCKET h) : CFsmCoreSocket(h) {
	bConnDropped = FALSE;
}

short CFmsRWUPCoreSocket::Socket_QcntChk() {
	int nError = 0;
	u_long rcnt = 0;
	u_long* p = &rcnt;
	u_long** p1 = &p;

	//	CHAR szBuffer[1] = "";
	//	nError = ::recv( m_hSocket, szBuffer, 1, MSG_PEEK );
	//	if( nError == 0 )
	//	{
	//		INT iError = ::WSAGetLastError();
	//		bConnDropped = ( ( iError == WSAENETRESET ) ||
	//			( iError == WSAECONNABORTED ) ||
	//			( iError == WSAECONNRESET ) ||
	//			( iError == WSAEINVAL ) );
	//	}
	//	return TRUE;

	//	char buf[17];
	//	rcnt = recv(m_hSocket,buf,16,MSG_PEEK);
	//	if (rcnt == SOCKET_ERROR) {
	//		AfxMessageBox("Network down");
	//		return FALSE;
	//	}
	//
	//	if( rcnt <= 0)
	//	{
	//		return FALSE;
	//	}
	//	return TRUE;

	//	fd_set rd;
	//	FD_SET(m_hSocket,&rd);
	//	timeval tm;
	//	tm.tv_sec = 0;
	//	tm.tv_usec = 0;
	//
	//	if (select(0,&rd,NULL,NULL,&tm) == SOCKET_ERROR) {
	//		AfxMessageBox("Socket Error");
	//		return -1;
	//	}

	nError = ioctlsocket(m_hSocket, FIONREAD, *p1);

	if (nError != 0) {
		return -1;
	}
	if (rcnt <= 0) {
		return 0;
	}
	return 1;
}

int CFmsRWUPCoreSocket::Socket_readchar(BYTE* pData, int nLen) {
	int bRet = 0;
	int Cnt = 0;
	int nRsize = nLen;

	Cnt = Socket_QcntChk();
	if ( /*Socket_QcntChk()*/ Cnt > 0) // YFL:  [4/8/2005]
	{
		bRet = ::recv(m_hSocket, (char*)pData, nRsize, 0);

		if (bRet == SOCKET_ERROR) {
			int e = WSAGetLastError();
			ASSERT(e != WSAEWOULDBLOCK);
			return 0;
		}
		else if (bRet == 0) {
			// YFL: connection closed [4/8/2005]
			return 0;
		}
	}
	else if (Cnt < 0) {
		// YFL: network error [4/8/2005]
	}
	return bRet;
}

int CFmsRWUPCoreSocket::Socket_ReadString(BYTE* lpszString, int nMaxLen) {
	int bRet = 0;
	int dwCnt = 0;
	int bufcnt = 0;
	ULONGLONG  dwCurTime = 0;
	ULONGLONG dwStartTime = dwCurTime = ::GetTickCount64();

	dwCnt = Socket_QcntChk(); // YFL: return -1 if network error [4/8/2005]
	if (dwCnt == 0) {
		return 0;
	}
	if (dwCnt > 0) {
		int T8 = CEIDCtlr.m_SecsParam.Param[CSecsParam::spT8TIMER].iValue;
		while (dwCurTime < (T8 + dwStartTime)) {
			bRet = Socket_readchar(&lpszString[bufcnt], nMaxLen - bufcnt);
			bufcnt += bRet;

			if (nMaxLen == bufcnt) {
				return nMaxLen;
			}
			Sleep(1);
			dwCurTime = GetTickCount64();
		}
		// YFL: We have T8 timeout here [4/8/2005]
		return 0;  // do not close socket here, it may recover after congestion
	}
	return -1;	// YFL: Indicate network error [4/8/2005]
}

int CFmsRWUPCoreSocket::Socket_WriteChar(BYTE* pData, int nLen) {
	int bRet = 0;
	return bRet;
}

int CFmsRWUPCoreSocket::Socket_WriteString(void* pData, int nLen) {
	char* pcData = (char*)pData;
	int	n = nLen;
	int dwSdcnt = 0;
	do {
		int r1 = ::send(m_hSocket, &pcData[dwSdcnt], n, 0);
		if (r1 == SOCKET_ERROR) {
			int e = WSAGetLastError();
			ASSERT(e != WSAEWOULDBLOCK);
			if (e == WSAECONNRESET) {
				return -1;
			}
			return 0;
		}
		else if (r1 == 0) {
			return 0;
		}
		else if (r1 < 0) {
			ASSERT(0);
			return 0;
		}
		pcData += r1;
		dwSdcnt += r1;
		n -= r1;
	} while (n > 0);

	ASSERT(n == 0);
	return nLen;
}