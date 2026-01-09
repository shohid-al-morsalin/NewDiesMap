#pragma once
// TelnetClientSocket.h : header file
//

#include <afxsock.h>

#define ioBuffSize 1024
#define CIRNORMALTEXTLEN	64

const	unsigned char IAC = 255;
const	unsigned char DO = 253;
const	unsigned char DONT = 254;
const	unsigned char WILL = 251;
const	unsigned char WONT = 252;
const	unsigned char SB = 250;
const	unsigned char SE = 240;
const	unsigned char IS = '0';
const	unsigned char SEND = '1';
const	unsigned char INFO = '2';
const	unsigned char VAR = '0';
const	unsigned char VALUE = '1';
const	unsigned char ESC = '2';
const	unsigned char USERVAR = '3';

//typedef void (CALLBACK* lpfnMessageProc)(int, LPVOID);
typedef void (CALLBACK* lpfnMessageProc)(int, LPVOID);

/////////////////////////////////////////////////////////////////////////////
// CTelnetClientSocket command target

class CTelnetClientSocket : public CAsyncSocket {
	// Attributes
public:
	enum MESSAGETYPE {
		NORMALMSG, SOCCLOSE, SOCCONNECT
	};
	lpfnMessageProc m_lpfnMessageProc;
	int idx;
	BOOL bConnected;
	CString m_strResp;
	CString m_strLine;
	CString m_strNormalText;
	CString m_strOptions;
	CStringList m_ListOptions;
	unsigned char m_bBuf[ioBuffSize + 1];
	// Operations
public:
	CTelnetClientSocket();
	virtual ~CTelnetClientSocket();

	// Overrides
public:
	void DispatchMessage(CString strText);
	void ArrangeReply(CString strOption);
	void RespondToOptions();
	void MessageReceived(LPCSTR pText);
	BOOL GetLine(unsigned char* bytes, int nBytes, int& ndx);
	void ProcessOptions();
	void SetCallBack(lpfnMessageProc pFunc = NULL);

public:
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
};

/////////////////////////////////////////////////////////////////////////////
