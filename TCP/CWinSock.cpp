// WinSock.cpp: implementation of the CWinSock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CWinSock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CWinSock constructor
//
// Constructs the CWinSock object. Initializes member variables
//
CWinSock::CWinSock(WORD wVersionRequired/*= MAKEWORD(1, 1)*/) {
	// initialize member variables
	m_wVersionRequired = wVersionRequired;
	m_nLastError = 0;
}

CWinSock::~CWinSock() {}

/////////////////////////////////////////////////////////////////////////////
// CWinSock::Startup()
//
// Start the WinSock sub–system.
//
int CWinSock::Startup() {
	int nStatus = CWINSOCK_NOERROR;
	m_nLastError = WSAStartup(m_wVersionRequired, &m_WSAData);
	if (m_nLastError != 0)
		nStatus = CWINSOCK_WINSOCK_ERROR;
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CWinSock::Shutdown()
//
// Shutdown the WinSock sub–system.
//
int CWinSock::Shutdown() {
	int nStatus = CWINSOCK_NOERROR;
	if (WSACleanup() != 0) {
		m_nLastError = WSAGetLastError();
		nStatus = CWINSOCK_WINSOCK_ERROR;
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CWinSock::Information()
//
// Copy the WinSock information structure.
//
void CWinSock::Information(LPWSADATA pwsaData) {
	memcpy(pwsaData, &m_WSAData, sizeof(WSADATA));
}