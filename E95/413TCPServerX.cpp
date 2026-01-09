// 413TCPServer.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "413App.h"
#include "strSOp.h"
#include "413TCPServer.h"
#include "..\TCP\StreamSocket.h"

#include "..\serial\MsgItem.h"
#include "..\IPC\Toolvar.h"
#include "413SVar.h"

#include "..\Sys\Var.h"
#include "..\Sys\Tool.h"

#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C413TCPServer dialog

C413TCPServer::C413TCPServer(CWnd* pParent /*=NULL*/)
	: CDialog(C413TCPServer::IDD, pParent)
{
	//{{AFX_DATA_INIT(C413TCPServer)
	//}}AFX_DATA_INIT

	m_pWinSock = NULL;
	m_pStreamSrv = NULL;
	m_pStream = NULL;
	pWnd = NULL;
	wP = 0;
	lP = NULL;
}

C413TCPServer::~C413TCPServer()
{
	KillTCPStuff();
}

void C413TCPServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C413TCPServer)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C413TCPServer, CDialog)
	//{{AFX_MSG_MAP(C413TCPServer)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_STREAMSRV, OnStreamSrv)
	ON_MESSAGE(WM_USER_STREAM, OnStream)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C413TCPServer message handlers

BOOL C413TCPServer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitiateTCPStuff();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void C413TCPServer::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void C413TCPServer::OnDestroy() 
{
	KillTCPStuff();
	CDialog::OnDestroy();
}

void C413TCPServer::KillTCPStuff()
{
	if (m_pStreamSrv)
	{
		m_pStreamSrv->DestroySocket();
		delete m_pStreamSrv;
		m_pStreamSrv = NULL;
	}
	if (m_pStream)
	{
		m_pStream->DestroySocket();
		delete m_pStream;
		m_pStream = NULL;
		MsgProc.m_pStream = m_pStream;
	}
	if (m_pWinSock)
	{
		m_pWinSock->Shutdown();
		delete m_pWinSock;
		m_pWinSock = NULL;
	}
}

BOOL C413TCPServer::InitiateTCPStuff()
{
	// initialize the WinSock object
	if (m_pWinSock)
	{
		delete m_pWinSock;
	}
	m_pWinSock = new CWinSock;
	if (m_pWinSock->Startup() == CWINSOCK_NOERROR)
	{
		Message("WinSock Initialized");
	}
	else
	{
		delete m_pWinSock;
		m_pWinSock = NULL;
		return TRUE;
	}
	// initialize the stream socket object
	m_pStreamSrv = new CStreamSocket(this, WM_USER_STREAMSRV);
	if (m_pStreamSrv->CreateSocket(413) == CWINSOCK_NOERROR) 
	{
		Message("413 Server Socket Created");
	}
	else
	{
		delete m_pStreamSrv;
		m_pStreamSrv = NULL;
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// C413TCPServer::OnStreamSrv()
//
// Receives messages from the stream server object.
//
LONG C413TCPServer::OnStreamSrv(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CWINSOCK_READY_TO_ACCEPT_CONNECTION:
		// make sure the server is not already servicing a client
		if (m_pStream != NULL)
		{
			m_pStream->DestroySocket();
			delete m_pStream;
			Message("Reconnect");
		}
		// accept the client connection
		int nStatus;
		m_pStream = new CStreamSocket(this, WM_USER_STREAM);
		nStatus = m_pStreamSrv->Accept(m_pStream);
		if (nStatus != CWINSOCK_NOERROR)
		{
			delete m_pStream;
			m_pStream = NULL;
			Message("Error accepting client connection");
			break;
		}
		else
		{
			Message("Accepted client connection");
			MsgProc.m_pStream = m_pStream;
		}
		break;
	default:
		Message("Other StreamSvr Message");
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// C413TCPServer::OnStream()
//
// Receives messages from the connected stream object.
//
LONG C413TCPServer::OnStream(WPARAM wParam, LPARAM lParam)
{
	int nLen; // length
	SOCKADDR_IN sin; // Internet address of client
	IN_ADDR in; // IP address of client
	int nStatus; // error status

	switch (wParam)
	{
	case CWINSOCK_DONE_WRITING:
		if (lParam)
		{
			CMsgItem MsgItem;
			char * p = (char *)lParam + sizeof (int);
			MsgItem.Conv(p);
			delete [] (char *)lParam;
			CString str;
			if (MsgItem.pMsg)
			{
				str.Format("Sent (%d) %s %d",MsgItem.MsgDes,MsgItem.pMsg, MsgItem.IDIdx);
				Message( str);
			}
		}
		break;
	case CWINSOCK_ERROR_WRITING:
		if (lParam)
		{
			CMsgItem MsgItem;
			char * p = (char *)lParam + sizeof (int);
			MsgItem.Conv(p);
			delete [] (char *)lParam;
			CString str;
			if (MsgItem.pMsg)
			{
				str.Format("Send Error (%d) %s %d", MsgItem.MsgDes, MsgItem.pMsg, MsgItem.IDIdx);
			}
			else
			{
				str.Format("Send Error (%d)", MsgItem.MsgDes);
			}
			Message(str);
			if (MsgItem.AckWnd)
			{
				::PostMessage((HWND)MsgItem.AckWnd,WM_COMMAND,MsgItem.Nak,NULL);
			}
		}
		break;
	case CWINSOCK_DONE_READING:
		// lParam = # data chunks in queue
		while (1)
		{
			//  [11/30/2007 YUEN]
			if (!m_pStream)
			{
				break;
			}
			STREAMDATA *pDataRead;
			pDataRead = m_pStream->Read(&nLen);
			if (!pDataRead)
			{
				// No more messages
				break;
			}
			if (pDataRead->pData)
			{
				CMsgItem MsgItem;
				MsgItem.Conv(pDataRead->pData);
				delete [] pDataRead->pData;
				delete pDataRead;
				CString str;
				if (MsgItem.pMsg)
				{
					str.Format("Received (%d) %s %d", MsgItem.MsgSrc, MsgItem.pMsg, MsgItem.IDIdx);
					Message(str);
				}
				DispatchMsg(&MsgItem);
			}
		}
		break;
	case CWINSOCK_ERROR_READING:
		Message("Read Error Detected");
		Message("Problem witt front end unit");
		break;
	case CWINSOCK_YOU_ARE_CONNECTED:
		// print out client information
		if (!m_pStream)
		{
			break;
		}
		nStatus = m_pStream->GetPeerName(&sin);
		if (nStatus == CWINSOCK_NOERROR)
		{
			memcpy(&in, &sin.sin_addr.s_addr, 4);
			CString pszMessage;
			pszMessage.Format("Connected to client %s, %d", inet_ntoa(in), ntohs(sin.sin_port));
			Message(pszMessage);
		}
		else
		{
			Message("Error getting client name");
		}
		break;
	case CWINSOCK_LOST_CONNECTION:
		// client closed the connection
		if (m_pStream)
		{
			m_pStream->DestroySocket();
			delete m_pStream;
			m_pStream = NULL;
			MsgProc.m_pStream = m_pStream;
		}
		Message("Client closed connection");
		break;
	default:
		break;
	}
	return 0L;
}

void C413TCPServer::Log(CString msg, short lvl)
{
	pGDev->LogFile.Log(msg,lvl,"C413TCPServer");
	if (lvl <= 1)
	{
		pGDev->LogFile.LogLocal(msg);
	}
}

void C413TCPServer::Message(CString msg, short lvl)
{
	Log(msg,lvl);

	int n = m_cList.GetCount();
	if (n > 32)
	{
		for (int i = n; i >= 24; i--)
		{
			m_cList.DeleteString(i);
		}
	}
	msg = pGDev->GetTimeStamp() + msg; 
	m_cList.InsertString(0,msg);
}

// LYF *pMsg should not be destroyed [11/9/2005]
// LYF pMsg cleared by calling function [4/16/2006]
void C413TCPServer::DispatchMsg(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		ASSERT(0);
		return ;
	}
	if (pMsg->DataTyp == CMsgItem::CASSETTEDATA)
	{
		CNCas *pCas = new CNCas;
		if (!pCas)
		{
			ASSERT(0);
			return ;
		}
		pCas->Conv(pMsg->pMsg);
		CString str;
		str.Format("Cas: %s", pCas->JobInfo.CarrierID);
		Message(str);
		SubmitProcessJob(pCas);
		Message("Cassette data");
	}
	else if (pMsg->DataTyp == CMsgItem::ROBOTEVENT) 
	{
		CWnd *pWnd = (CWnd *) pMsg->AckWnd;
		Message("Robot Event");
		if (pWnd)
		{
			if (pMsg->Ack)
			{
				::PostMessage(pWnd->m_hWnd,WM_COMMAND,pMsg->Ack,0);
			}
			if (pMsg->Nak)
			{
				::PostMessage(pWnd->m_hWnd,WM_COMMAND,pMsg->Nak,0);
			}
		}
		Message("Robot Events");
	}
	else if (pMsg->DataTyp == CMsgItem::DATAREQ)
	{
		if (pMsg->DataID == CMsgItem::SV)
		{
			Message("Data Req: SV");
			SendSV(pMsg);
		}
		if (pMsg->DataID == CMsgItem::SV1)
		{
			Message("Data Req: SV1");
			SendSV1(pMsg);
		}
		if (pMsg->DataID == CMsgItem::TV)
		{
			Message("Data Req: TV");
			SendTV(pMsg);
		}
	}
	else if (pMsg->DataTyp == CMsgItem::COMMAND) 
	{
		Command413(pMsg);
		Message("Commands");
	}
	else if (pMsg->DataTyp == CMsgItem::EVENTDATA)
	{
		switch(pMsg->DataID)
		{
		case CMsgItem::HELLO: {
			Message("Are You There received");
			CMsgItem *q = new CMsgItem;
			if (q) {
				q->DataTyp = CMsgItem::EVENTDATA;
				q->DataID = CMsgItem::HELLOACK;
				q->AckWnd = pMsg->AckWnd;
				q->Ack = pMsg->Ack;
				q->Nak = pMsg->Nak;
				q->Unit = pMsg->Unit;
				MsgProc.Send(q); // Reply
				Message("Acknowledgment send");
			}
							  }
			break;
		case CMsgItem::LTwr:
			//CommandLTwr(pMsg);
			Message("Light Tower Command");
			break;
		default:
			Message("Unknown Event Data");
			ASSERT(0);
			break;
		}
	}
	else
	
	{
		Message("Unknown Event");
	}
}

// LYF Do not delete pMsg [4/16/2006]
void C413TCPServer::SendTV(CMsgItem *pM)
{
	CToolVar SV;
	CMsgItem *pMsg = new CMsgItem;
	if (pMsg)
	{
		if (CollectData(SV))
		{
			pMsg->Ret = 1;
		}
		else
		{
			pMsg->Ret = 0;
		}
		long sz = SV.GetLength();
		if (pMsg->Alloc(sz+16))
		{
			char *p = pMsg->pMsg;	// p is required because SV.MakeBinary will change the pointer value
			pMsg->Len = SV.Binary(&p);
			pMsg->MsgSrc = CMsgItem::MSB413;
			pMsg->DataTyp = CMsgItem::DATAREQACK;
			pMsg->DataID = CMsgItem::TV;
			pMsg->AckWnd = pM->AckWnd;
			pMsg->Ack = pM->Ack;
			pMsg->Nak = pM->Nak;
			MsgProc.Send(pMsg);
		}
		else
		{
			delete pMsg;
		}
	}
}

BOOL C413TCPServer::CollectData(CToolVar &SV)
{
	SV.OpMode = p413App->AramsE95.GetCurrentState();
	return TRUE;
}

BOOL C413TCPServer::CollectData(C413SVar &SV)
{
	SV.bWaferPresent = FALSE;
	SV.OpMode = p413App->AramsE95.GetCurrentState();
	return TRUE;
}

// LYF Do not delete pMsg [4/16/2006]
void C413TCPServer::SendSV(CMsgItem *pMsg)
{
	AckWnd = pMsg->AckWnd;	// pMsg will be deleted externally, thus need to keep info first
	Ack = pMsg->Ack;
	Nak = pMsg->Nak;
	Message("SV reply");
}

// LYF Do not delete pMsg [4/16/2006]
void C413TCPServer::SendSV1(CMsgItem *pM)
{
	AckWnd = pM->AckWnd;	// pMsg will be deleted externally, thus need to keep info first
	Ack = pM->Ack;
	Nak = pM->Nak;
	
	C413SVar SV;
		
	CMsgItem *pMsg = new CMsgItem;
	if (pMsg)
	{
		if (CollectData(SV))
		{
			pMsg->Ret = 1;
		}
		else
		{
			pMsg->Ret = 0;
		}
		if (p413App->AramsE95.GetCurrentState() == SArams::STANDBY)
		{
			SV.ServiceState = 1;
		}
		else
		{
			SV.ServiceState = 0;
		}
		long sz = SV.GetLength();
		if (pMsg->Alloc(sz+16))
		{
			char *p = pMsg->pMsg;
			pMsg->Len = SV.Binary(&p);
			pMsg->MsgSrc = CMsgItem::MSB413;
			pMsg->DataTyp = CMsgItem::DATAREQACK;
			pMsg->DataID = CMsgItem::SV1;
			pMsg->AckWnd = AckWnd;
			pMsg->Ack = Ack;
			pMsg->Nak = Nak;
			MsgProc.Send(pMsg);
		}
		else
		{
			delete pMsg;
		}
	}
}

void C413TCPServer::Command413(CMsgItem *pMsg)
{
	AckWnd = pMsg->AckWnd;
	Ack = pMsg->Ack;
	Nak = pMsg->Nak;
	
	BOOL ret = FALSE;
	if (pMsg->DataTyp == CMsgItem::COMMAND)
	{
		switch (pMsg->DataID)
		{
		case CMsgItem::STOPPROCESS:
			if (pWnd)
			{
				pWnd->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGSTOP,NULL);
			}
			break;
		case CMsgItem::RESETTABLE:
			Message("Reset Table");
			break;
		case CMsgItem::DONERECOVERY:
			Message("Done Recovery");
			break;
		default:
			Message("Unknown Command");
			break;
		}
	}
}

// This call is used exclusively by back end module [12/17/2007 YUEN]
void C413TCPServer::SubmitProcessJob(CNCas *pCas)
{
	if (!pCas) 
	{
		ASSERT(0);
		return ;
	}
	short unit = pCas->JobInfo.LPUnit;
	if (unit < 1 || (unit > 4)) 
	{
		ASSERT(0);
		return ;
	}
	CCasMgr * casmgr;
	casmgr = Tool.GetCasMgr(unit);
	if (!casmgr)
	{
		ASSERT(0);
		return ;
	}
	DWORD tick = ::GetTickCount();

	BOOL bOK = FALSE;
	if (casmgr->State == CCasMgr::NOTSELECTED) 
	{
		bOK = TRUE;
	}
	if (!bOK) 
	{
		Log("CCasMgr in state other than NOTSELECTED",2);
		ASSERT(0);
		return ;
	}
	
	CString str;
	str.Format("Final Carrier ID: %s", pCas->JobInfo.CarrierID);
	Log(str, 10);

	BOOL bOldFile = FALSE;
	CNCas *cas;
	// Cassette filename is derived from pCas->cassettefile and recipe name and lotid
	CString cassettefile="";
	char *pp = strstr(pCas->Recipe413.RcpeName,"@1");
	if (pp == NULL) 
	{
		if (!casmgr->MakeCassetteName(cassettefile,pCas->Recipe413.RcpeName, "", pCas->JobInfo.CarrierID,FALSE, TRUE))
		{ // Use local disk [12/15/2007 YUEN]
			return ;
		}
	}
	else 
	{
		*pp = 0; *(pp+1) = 0;	// Clear '@1'
		if (!casmgr->MakeCassetteNameOld(cassettefile, pCas->JobInfo.CarrierID/*,Var.DiskSelect*/))
		{ // Use local disk [12/15/2007 YUEN]
			return ;
		}
		bOldFile = TRUE;
	}
	CString tmp;
	if (!casmgr->OpenCassette(cassettefile)) // Load from local disk [12/16/2007 YUEN]
	{
		tmp.Format("Cassette file %s not found. Create New",cassettefile);
		Log(tmp, 2);
		casmgr->CreateNewCassette(cassettefile);
		cas = casmgr->GetCurCas();
		if (!cas)
		{
			ASSERT(0);
			Log("Fail Create New Document", 10);
			return ;
		}
		*cas = *pCas;  // copy content
	}
	else 
	{
		// Found cassette data
		// need to modify some content
		tmp.Format("Cassette file found: %s",cassettefile);
		Log(tmp, 2);
		cas = casmgr->GetCurCas();
		if (cas) 
		{
			cas->JobInfo = pCas->JobInfo;
			for (int i=0; i<25; i++) 
			{
				CWaferInfo *pWI = pCas->winfo.FindWafer(i+1);
				if (pWI) 
				{
					cas->winfo.Add(pWI,TRUE);
				}
				else 
				{
					// unselect from processing
					CWaferInfo *pWI2 = cas->winfo.FindWafer(i+1);
					if (pWI2) 
					{
						pWI2->bSelected = FALSE;
					}
				}
			}
			cas->winfo.enPort = pCas->winfo.enPort;
			cas->winfo.InPort = pCas->winfo.InPort;
			cas->winfo.OutPort = pCas->winfo.OutPort;
			if (bOldFile) 
			{
				strscpy(cas->Recipe413.RcpeName,RCPNAMELEN,pCas->Recipe413.RcpeName);
			}
		}
	}

	if (!cas) 
	{
		// Something wrong, do not proceed
		ASSERT(0);
		return ;
	}

	// Ready to run [7/11/2010 C2C]
	casmgr->State = CCasMgr::SELECTED;
}
