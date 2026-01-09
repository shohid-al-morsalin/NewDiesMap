// FsmCoreSocket.h: interface for the CFsmCoreSocket class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CFsmCoreSocket {
public:
	CFsmCoreSocket();
	//	CFsmCoreSocket( int nPort );
	CFsmCoreSocket(SOCKET h);

	virtual ~CFsmCoreSocket();

public:
	SOCKET m_hSocket;
	interface XCannotCreate {};

public:
	BOOL Socket_Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	SOCKET Socket_Accept();
	BOOL SetIntOption(int level, int optname, int val);
	void Socket_Close();
	BOOL Socket_Initailization(int nPort);
	BOOL Socket_Create(int nPort);
	void Initialize(SOCKET h);
	SOCKET	H() const { return m_hSocket; }
};

class CFmsRWUPCoreSocket : public CFsmCoreSocket {
public:
	interface x {};
	interface xread :public x {};
	interface xwrite :public x {};

public:
	BOOL bConnDropped;

	CFmsRWUPCoreSocket(SOCKET h);

	short Socket_QcntChk();

	int  Socket_readchar(BYTE* pData, int nLen);
	int  Socket_ReadString(BYTE* lpszString, int nMaxLen);
	int  Socket_WriteChar(BYTE* pData, int nLen);
	int  Socket_WriteString(void* pData, int nLen);
};