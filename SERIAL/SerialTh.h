#pragma once
#include "NThread.h"
#include "RtnCode.h"
#include "Links.h"

class CSerialICC;

struct SSDT // meaningless name
{
	short Rpy;
	int IDIdx;
	int DataTyp;
	int DataID;
	int MsgSrc;
	int MsgDes;
	int MsgEvent;
	int Unit;
	int AckWnd;
	int Ack;
	int Nak;
};

/** Specials to inter-computer communication involving a simple protocol. */
class CSerialTh : public NThread {
public:
	/** 0 - Text version 1 - Binary version Byte stream <L4 size> <B size> */
	enum PROTOCL {
		TEXTFORMAT, MSGITEMFORMAT
	};

	PROTOCL Protocol;
public:
	CSerialTh(void);
	~CSerialTh(void);
	static CSerialTh* Start(CSerialTh* pThread, int port);
	UINT ThreadMain(void);

private:
	/** Text version that response to CR */
	int ProcessWrite();

	/** Binary version that implement a size protocol. */
	int ProcessWriteB();

	/** Text version that response to CR */
	int ProcessRead();

	/** Binary version that implement a size protocol. */
	int ProcessReadB();
	int Initialize(void);
public:
	void SelectProtocol(CSerialTh::PROTOCL proto);
	CRtnCode::RCODE Put(char* pMsg, short bRpy = 0);
public:
	BOOL bSimu;
	CSerialICC* pCom;
	short cnt;
	short port, ytime;
	char ReadBuff[64000];
public:
	short MessageDone(int len, SSDT& Dt);
	BOOL GetItemOverComm(short& val);
	DWORD CTOut, CTOut1;
	BOOL GetItemOverComm(int& val);
	int hMsgInQue;
	void SetMessageEventHandler(int hMsgInQue);
	short IDSrc;  // Sender ID
	CLinks Que;
	short cdelay;
	// Notification of incoming event
	CWnd* pWnd; // Window to accept the notification
};
