// OnlineMgrB.h: interface for the COnlineMgrB class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class COLnSvrObj;
class CEquipEvtThread;
class CHSMSComThread;

class COnlineMgrB {
	// 	HANDLE OLnHandle;
	HANDLE EVTHandle;
	HANDLE HSMSHandle;
	CEquipEvtThread* pEVTThread;
	CHSMSComThread* pHSMSThread;
	SOCKADDR_IN m_addr;
	SOCKET m_sListen;
	BOOL bOnlineSelected;
	BOOL bRunEVT, bRunHSMS;

protected:
	void StopThreads();
	BOOL StartThreads();

	void Log(CString msg, short lvl = 5);

public:
	void SendSelectReq();
	void SendSeparateReq();
	virtual void ShowMsg(CString msg, short lvl = 5) = 0;
	CString tmpStr;
	virtual void Message(CString msg, short lvl = 5) = 0;
	virtual CWnd* GetCWnd() = 0;
	COLnSvrObj* pOLnSvrObj;

	void FDCloseSocket();
	void FDAcceptSocket();
	SOCKET GetListenSocket();
	BOOL StopEVTThread();
	BOOL StartEVTThread();
	BOOL StopOLnSvrThread();
	BOOL StartOLnSvrThread();
	BOOL StopHSMSThread();
	BOOL StartHSMSThread();
	virtual HWND GetHWnd() = 0;
	BOOL InitSocket();
	void SocketError();
	COnlineMgrB();
	virtual ~COnlineMgrB();
};