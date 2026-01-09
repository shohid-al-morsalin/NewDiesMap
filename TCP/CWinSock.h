// WinSock.h: interface for the CWinSock class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
enum ERR {
	CWINSOCK_PROGRAMMING_ERROR = -7,
	CWINSOCK_WINDOWS_ERROR,
	CWINSOCK_WINSOCK_ERROR,
	CWINSOCK_YOU_ARE_CONNECTED,
	CWINSOCK_LOST_CONNECTION,
	CWINSOCK_ERROR_READING,
	CWINSOCK_ERROR_WRITING,
	CWINSOCK_NOERROR = 0,
	CWINSOCK_DONE_READING,
	CWINSOCK_DONE_WRITING,
	CWINSOCK_READY_TO_ACCEPT_CONNECTION
};

class CWinSock {
private:
	WORD m_wVersionRequired; // WinSock version required by application
	int m_nLastError; // last WinSock error
	WSADATA m_WSAData; // WinSock information
public:
	CWinSock(WORD wVersionRequired = MAKEWORD(1, 1));
	virtual ~CWinSock();
	int Startup();
	int Shutdown();
	void Information(LPWSADATA pwsaData);
	int LastError() { return m_nLastError; }
};
