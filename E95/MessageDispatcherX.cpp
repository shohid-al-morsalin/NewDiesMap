// MessageDispatcher.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "..\SYS\CUMMsg.h"
//#include "..\SERIAL\BEME95.h"
#include "..\SERIAL\MsgItem.h"

#include "..\SRC\BMake.h"
#include "..\SRC\DosUtil.h"
#include "XTabCtrl.h"
// #include "..\IPC\Alrm.h"
#include "OLnSvrObj.h"
#include "OnlineMgr.h"
#include "..\FEM\RBT\RbtMgr.h"
#include "..\TCP\StreamSocket.h"

#include "MessageDispatcher.h"
 #include "ProcJob413ProcessingDlg.h"
// #include "ProcJobRTProcessingDlg.h"
// #include "ProcJobHTProcessingDlg.h"

#include "FFUFailDlg.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#define IDT_START_TCP			9999
#define IDT_CONNECT_TIMEOUT		9998
#define IDT_AREYOUTHERE_TIMEOUT	9997
#define IDT_RECONNECT			9996
#define IDT_RECONNECT_TIMEOUT	9995
#define IDT_STATUSMONITOR		9994
#define IDT_AREYOUTHERE			9993
#define IDT_AREYOUTHERE_TOUT	9992
#define IDT_ROBOTCMD			9991

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDispatcher dialog

//IMPLEMENT_DYNAMIC(CMessageDispatcher, CResizableDialog)

CMessageDispatcher::CMessageDispatcher(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CMessageDispatcher::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDispatcher)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	if (!pGDev || !pGDev->pGEMObjs)
	{
		ASSERT(0);
	}
	
	bOnScreen = FALSE;
	pFFUFailDlg = NULL;

	ID = DSPCRNONE;
//	pE95BEM = NULL;
	CallerAck = 0;
	CallerNak = 0;
	CallerWnd = 0;

	bFirst = TRUE;

	m_pWinSock = NULL;
	m_pStream = NULL;
	ConnectStatus = NOTCONNECTED;

	sScanData = "Scan data";
	sRobotCmd = "Robot Cmd: ";
	sEventData = "<- Event Data";

	tOUT = 8000;
	
	memset(m_pszServer,0,sizeof(m_pszServer));
}

CMessageDispatcher::~CMessageDispatcher()
{
}

void CMessageDispatcher::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDispatcher)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDispatcher, CResizableDialog)
	//{{AFX_MSG_MAP(CMessageDispatcher)
	ON_COMMAND(ID_RBT_XFEROK, OnRbtXferok)
	ON_COMMAND(ID_RBT_XFERFAIL, OnRbtXferfail)
	ON_WM_TIMER()
	ON_MESSAGE(WM_XTABSELECTED,OnTabSelected)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_AREYOUTHERE, OnAreyouthere)
	ON_COMMAND(ID_IPC_HELLOACK, OnIpcHelloack)
	ON_COMMAND(ID_IPC_HELLONAK, OnIpcHellonak)
	ON_COMMAND(ID_IPC_HELLOACK1, OnIpcHelloack1)
	ON_COMMAND(ID_IPC_HELLONAK1, OnIpcHellonak1)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_STREAM, OnStream)
	ON_MESSAGE(UM_DSPCHRMESSAGEINQUE, OnUMDspchrMsgInQue)
//	ON_MESSAGE(UM_128MESSAGEINQUE, OnUM128MsgInQue)
//	ON_MESSAGE(UM_900MESSAGEINQUE, OnUM900MsgInQue)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDispatcher message handlers

BOOL CMessageDispatcher::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_CONNECT, TOP_RIGHT);
	AddAnchor(IDC_AREYOUTHERE, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT,BOTTOM_LEFT);

// 	// This dialog runs in main thread, thus main thread will assume this thread priority [12/3/2007 YUEN]
// 	CWinThread * pThread ;
// 	pThread = AfxGetThread ();
// 	pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	if (pGDev)
	{
		if (pGDev->pGEMObjs)
		{
			switch(ID)
			{
			case DSPCR900:
// 				pGDev->pGEMObjs->StMd.Set900OpStatus(C900Obj::OFFLINE);
				pGDev->pGEMObjs->StMd.C900.Status = C900Obj::oOFFLINE;
				break;
			case DSPCR128:
// 				pGDev->pGEMObjs->StMd.Set128OpStatus(C128Obj::OFFLINE);
				pGDev->pGEMObjs->StMd.C128.Status = C128Obj::oOFFLINE;
				break;
			case DSPCR413:
				pGDev->pGEMObjs->StMd.C413.Status = C413Obj::oOFFLINE;
				break;
			default:
				ASSERT(0);
				break;
			}
		}
	}
	// initialize the WinSock object
	m_pWinSock = new CWinSock;
	if (m_pWinSock->Startup() == CWINSOCK_NOERROR)
	{
		Message("WinSock initialized");
		SetTimer(IDT_START_TCP,200,NULL);
	}
	else
	{
		Message("WinSock initialization failed");
		delete m_pWinSock;
		m_pWinSock = NULL;
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CMessageDispatcher::OnTabSelected(WPARAM wP, LPARAM lP)
{
	switch (ID)
	{
	case DSPCR128:
		pUDev->SetCurrentView("Message Dispatcher\n128 unit");
		break;
	case DSPCR900:
		pUDev->SetCurrentView("Message Dispatcher\n900 unit");
		break;
	case DSPCR413:
		pUDev->SetCurrentView("Message Dispatcher\n413 unit");
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

//BOOL CMessageDispatcher::Start900()
//{
//	ID = DSPCR900;
//	if (pE95BEM) {
//		ASSERT(0);
//		delete pE95BEM;
//	}
//	SetWindowText("E95 to 900");
////	pE95BEM = new CBEME95;
//	if (pE95BEM) {
//		pE95BEM->StartE95To900();
//		pE95BEM->RegisterWin(this);
//	}
//	return TRUE;
//}

//BOOL CMessageDispatcher::Start128()
//{
//	ID = DSPCR128;
//	if (pE95BEM) {
//		ASSERT(0);
//		delete pE95BEM;
//	}
//	SetWindowText("E95 to 128");
////	pE95BEM = new CBEME95;
//	if (pE95BEM) {
//		pE95BEM->StartE95To128();
//		pE95BEM->RegisterWin(this);
//	}
//	return TRUE;
//}

BOOL CMessageDispatcher::Send(CMsgItem *pMsg)
{
//	if (pE95BEM) {
//		if (pE95BEM->Send(pMsg) == CRtnCode::NOERR) {
//			Message("-> Send");
//			return TRUE;
//		}
//	}
	if (m_pStream)
	{
		STREAMDATA *pStreamData = new STREAMDATA;
		if (pStreamData)
		{
			pStreamData->nLen = pMsg->GetLength()+sizeof(int);
			pStreamData->pData = new char [pStreamData->nLen+16];
			if (pStreamData->pData)
			{
				char *p = pStreamData->pData;
				CMake::Binary(pStreamData->nLen,&p);	// LYF This line must come first [11/10/2005]
				pMsg->Binary(p);
				if (m_pStream->Write(pStreamData) == CWINSOCK_NOERROR)
				{
					delete pMsg; pMsg = NULL;
					return TRUE;
				}
				delete [] pStreamData->pData;
			}
			delete pStreamData;
		}
		if (pMsg && (pMsg->AckWnd))
		{
//			if (pMsg)
//			{
				::PostMessage((HWND)pMsg->AckWnd,WM_COMMAND,pMsg->Nak,NULL);
				delete pMsg;
				pMsg = NULL;
//			}
//			else
//			{
//				// Should not come here
//				ASSERT(0);
//			}
		}
		return TRUE; // must return TRUE here
	}
	// sink the message if unable to send
//	TRACE("Message sinked\n");
	if (pMsg) delete pMsg;
	return FALSE;
}

//
//CMsgItem * CMessageDispatcher::GetMsgItem(int ID)
//{
//	if (pE95BEM) {
//		return pE95BEM->GetMsgItem(ID);
//	}
//	ASSERT(0);
//	return NULL;
//}

LRESULT CMessageDispatcher::OnUMDspchrMsgInQue(WPARAM wP, LPARAM lP)
{
	//short Unit=-1;
	CMsgItem *pMsg = (CMsgItem *)lP;
	DispatchMsg(pMsg);	
	return 0;
}


// Note: more than one thread will call this function.
void CMessageDispatcher::ProcessRbtCmd(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		ASSERT(0);
		return ;
	}
	CSMOL SM;
	CString cmd;
	cmd = pMsg->pMsg;
	// Watch out for second call to this function when first call is executing
	if (CallerWnd != 0)
	{
		Message("Command rejected: " + cmd);
		RejectRbtCmd(pMsg);
//		ASSERT(0);
		return ;
	}
	if (!CanTransfer(pMsg))
	{
		Message("Cannot perform: " + cmd);
		RejectRbtCmd(pMsg);
		return;
	}
	Message(sRobotCmd+cmd);
	CallerAck = pMsg->Ack;
	CallerNak = pMsg->Nak;
	CallerWnd = pMsg->AckWnd;
	SM.Pm.Ret = pMsg->Ret;
	// Send Command To RbtMgr. The reply will come in via window message
	SM.cmd		= CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act	= PrcsMgrServices::XFER1;
	SM.Pm.STSTo = pMsg->STSLocTo;
	SM.Pm.STSFrom = pMsg->STSLocFrom;
	SM.HWin		= m_hWnd;
	SM.Ack		= ID_RBT_XFEROK;
	SM.Nak		= ID_RBT_XFERFAIL;
	strcpy(SM.Pm.text, cmd.GetBuffer(cmd.GetLength()));

	delete pMsg;
	
	CSMClt ipc;
	if (ipc.Start1(pGDev->pRbtMgr->SvrName, sizeof(CSMOL))) 
	{
		if (!ipc.Send(& SM))
		{
			cmd.Format("Fail to send '%s'", cmd);
			PostMessage(WM_COMMAND,CallerNak,NULL);
			return;
		}
	}
}

void CMessageDispatcher::ProcessScanData(CMsgItem *pMsg)
{
	ASSERT(0);  // ?? [8/16/2011 FSMT]

	if (!pMsg)
	{
		return ;
	}
	
	int Unit = pMsg->Unit;
	if ((Unit <1) || (Unit>MAX_LPSUPPORTED))
	{
		delete pMsg;
		ASSERT(0);
		return ;
	}
	if (Unit == -1)
	{
		delete pMsg;
		ASSERT(0);
		return ;
	}
	if (pMsg->MsgSrc == CMsgItem::MSB413)
	{
		CProcJob413ProcessingDlg *p = pUDev->pProcJob413ProcessingDlg[Unit-1];
		if (p && (!p->bDone))
		{
			p->PostMessage(UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
			// Do not delete pMsg here, deleted by called function
			return ;
		}
		else
		{
			delete pMsg;
		}
	}
// 	else if (pMsg->MsgSrc == CMsgItem::MSB128){
// 		CProcJobRTProcessingDlg *p = pGDev->m_ProcJobRTProcessingDlg[Unit-1];
// 		if (p && (!p->bDone)) {
// 			Message(sScanData);
// 			p->PostMessage(UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
// 			// Do not delete pMsg here, deleted by called function
// 			return ;
// 		}
// 		else {
// 			delete pMsg;
// 		}
// 	}
// 	else if (pMsg->MsgSrc == CMsgItem::MSB900) {
// 		CProcJobHTProcessingDlg *p = pGDev->m_ProcJobHTProcessingDlg[Unit-1];
// 		if (p && (!p->bDone)) {
// 			Message(sScanData);
// 			p->PostMessage(UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
// 			// Do not delete pMsg here, deleted by called function
// 			return ;
// 		}
// 		else {
// 			delete pMsg;
// 		}
// 	}
	else
	{
		// message not delivered
		delete pMsg;
		ASSERT(0);
	}
	return ;
}

// CString CMessageDispatcher::GetTimeStamp()
// {
// 	CString str;
// 	str.Format("%s  ",CTime::GetCurrentTime().Format("%y:%m:%d %H-%M-%S"));
// 	return str;
// }

void CMessageDispatcher::ProcessEventData(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		return ;
	}

	switch (pMsg->DataID)
	{
	case CMsgItem::HELLOACK:
		ProcessHelloAck(pMsg);
		break;
	case CMsgItem::FFUFAIL:
		if (!bOnScreen)
		{
			if (pFFUFailDlg)
			{
				pFFUFailDlg->DestroyWindow();
			}
			pFFUFailDlg = new CFFUFailDlg;
			if (pFFUFailDlg)
			{
				pFFUFailDlg->bOnScreen = &bOnScreen;
				pFFUFailDlg->Create(CFFUFailDlg::IDD);
				pFFUFailDlg->ShowWindow(SW_SHOW);
			}
		}
		break;
	case CMsgItem::PROCESSCOMPLETE:
		Sleep(5000);  // wait a while for C2C to clean up [2/8/2010 Yuen]
		// fall through intended (no break) [2/8/2010 Yuen]
	case CMsgItem::PROCESSFAILED:
	case CMsgItem::PROCESSHALT:
		if (ProcessComplete(pMsg))
		{
			// do not delete pMsg
			return;
		}
		break;
	default:
		delete pMsg;
		ASSERT(0);
		break;
	}
}

void CMessageDispatcher::OnRbtXferok() 
{
	CMsgItem *p = new CMsgItem;
	if (p)
	{
		p->DataTyp = CMsgItem::ROBOTEVENT;
		p->Ack = CallerAck;
		p->Nak = 0;
		p->AckWnd = CallerWnd;
		CallerWnd = 0; // LYF Reset to detect multiple entry [4/17/2006]
		Message("->RbtXfer OK");
		Send(p);
		SetTimer(IDT_ROBOTCMD,500,NULL);
	}
	else
	{
		// Memory crashed
		ASSERT(0);
	}
}

void CMessageDispatcher::OnRbtXferfail() 
{
	CMsgItem *p = new CMsgItem;
	if (p)
	{
		p->DataTyp = CMsgItem::ROBOTEVENT;
		p->Ack = 0;
		p->Nak = CallerNak;
		p->AckWnd = CallerWnd;
		CallerWnd = 0; // LYF Reset to detect multiple entry [4/17/2006]
		Message("->RbtXfer fail",0);
		Send(p);
// 		SetTimer(IDT_ROBOTCMD,500,NULL);
	}
	else
	{
		// Memory crashed
		ASSERT(0);
	}
}

void CMessageDispatcher::Message(CString msg, short lvl)
{
	int n = m_cList.GetCount();
	if (n > 32)
	{
		for (int i = n; i >= 24; i--)
		{
			m_cList.DeleteString(i);
		}
	}
	Log(msg,lvl);
	msg = pGDev->GetTimeStamp() + msg; 
	m_cList.InsertString(0,msg);
}

void CMessageDispatcher::Log(CString msg, short lvl)
{
	pGDev->LogFile.Log(msg,lvl,"CMessageDispatcher");
	if (lvl <= 1)
	{
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CMessageDispatcher::ProcessComplete(CMsgItem *pMsg)
{
	ASSERT(0); // ?? [8/16/2011 FSMT]

	if (!pMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	int Unit = pMsg->Unit;
	if ((Unit < 1) || (Unit > MAX_LPSUPPORTED))
	{
		// Communication error
		delete pMsg;
		ASSERT(0);
		return FALSE;
	}
	if (pMsg->MsgSrc == CMsgItem::MSB413)
	{
		CProcJob413ProcessingDlg *p = pUDev->pProcJob413ProcessingDlg[Unit-1];
		if (p && (!p->bDone))
		{
			::PostMessage(p->m_hWnd, UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
			// Do not delete pMsg here, deleted by called function
			return TRUE;
		}
		else
		{
			delete pMsg;
		}
	}
// 	else if (pMsg->MsgSrc == CMsgItem::MSB128){
// 		CProcJobRTProcessingDlg *p = pGDev->m_ProcJobRTProcessingDlg[Unit-1];
// 		if (p && (!p->bDone)) {
// 			Message(sEventData);
// 			::PostMessage(p->m_hWnd, UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
// 			// Do not delete pMsg here, deleted by called function
// 			return TRUE;
// 		}
// 		else {
// 			delete pMsg;
// 		}
// 	}
// 	else if (pMsg->MsgSrc == CMsgItem::MSB900) {
// 		CProcJobHTProcessingDlg *p = pGDev->m_ProcJobHTProcessingDlg[Unit-1];
// 		if (p && (!p->bDone)) {
// 			Message(sEventData);
// 			::PostMessage(p->m_hWnd, UM_GENMESSAGEINQUE,pMsg->IDIdx,(LPARAM)pMsg);
// 			// Do not delete pMsg here, deleted by called function
// 			return TRUE;
// 		}
// 		else {
// 			delete pMsg;
// 		}
// 	}
	else
	{
		delete pMsg;
		ASSERT(0);
	}
	return FALSE;
}

void CMessageDispatcher::ProcessHelloAck(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		ASSERT(0);
		return ;
	}
	if (pMsg->AckWnd)
	{
		::PostMessage((HWND)pMsg->AckWnd, WM_COMMAND,pMsg->Ack,NULL);
	}
	delete pMsg;
}


void CMessageDispatcher::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_START_TCP)
	{
		KillTimer(IDT_START_TCP);
		TCPConnect();
		SetTimer(IDT_ROBOTCMD, 2500, NULL);
		return ;
	}
	if (nIDEvent == IDT_ROBOTCMD)
	{
		KillTimer(IDT_ROBOTCMD);
		if (ConnectStatus == CONNECTED)
		{
			if (pGDev && pGDev->pRbtMgr && (CallerWnd == 0) && (pGDev->pRbtMgr->IsIdle()))
			{
				if (RbtCmdQue.GetCount() > 0)
				{
					ProcessRbtCmd(RbtCmdQue.RemoveHead());
// 					SetTimer(IDT_ROBOTCMD,500,NULL);  //  [1/2/2008 YUEN]
					return ;
				}
			}
		}
		SetTimer(IDT_ROBOTCMD,500,NULL);
		return ;
	}
	if (nIDEvent == IDT_CONNECT_TIMEOUT)
	{
		KillTimer(IDT_CONNECT_TIMEOUT);
// 			switch(ID) {
// 			case DSPCR128:
// 				pGDev->SetAlarm(CAlrmList::C128NOTONLINE,"Fail to connect, 128 unit not online");
// 				break;
// 			case DSPCR900:
// 				pGDev->SetAlarm(CAlrmList::C900NOTONLINE,"Fail to connect, 900 unit not online");
// 				break;
// 			case DSPCR413:
// 				pGDev->SetAlarm(CAlrmList::C413NOTONLINE,"Fail to connect, 413 unit not online");
// 				break;
// 			default:
// 				ASSERT(0);
// 				break;
// 			}
		SetTimer(IDT_RECONNECT,200,NULL);
		return ;
	}
	if (nIDEvent == IDT_RECONNECT)
	{
		KillTimer(IDT_RECONNECT);
		bFirst = TRUE;
		ConnectStatus = NOTCONNECTED;
		TCPReconnect();
		return ;
	}
	if (nIDEvent == IDT_RECONNECT_TIMEOUT)
	{
		KillTimer(IDT_RECONNECT);
		SetTimer(IDT_RECONNECT,5000,NULL);
		return ;
	}
	if (nIDEvent == IDT_AREYOUTHERE_TIMEOUT)
	{
		KillTimer(IDT_AREYOUTHERE_TIMEOUT);
		Message("Fail to recieve acknowledge from server");
		return ;
	}
	if (nIDEvent == IDT_STATUSMONITOR)
	{
		KillTimer(IDT_STATUSMONITOR);
		CString str;
		switch(ID)
		{
		case DSPCR128:	{
			C128Obj *p = pGDev->pGEMObjs->StMd.Get128Obj();
			if (p->Service == C128Obj::OUTOFSERVICE)
			{
				if (m_pStream)
				{
					m_pStream->DestroySocket();
					Message("Socket destroyed");
					delete m_pStream;
					m_pStream = NULL;
				}
				ConnectStatus = NOTCONNECTED;
				str = "128 unit Out of service set, connection closed";
				pGDev->LogFile.LogLocal(str);
				Log(str,5);
				SetTimer(IDT_START_TCP,5000,NULL);
			}
			else
			{
				SetTimer(IDT_STATUSMONITOR,5000,NULL);
			}
					   }
			break;
		case DSPCR900: {
			C900Obj *p = pGDev->pGEMObjs->StMd.Get900Obj();
			if (p->Service == C900Obj::OUTOFSERVICE)
			{
				if (m_pStream)
				{
					m_pStream->DestroySocket();
					Message("Socket destroyed");
					delete m_pStream;
					m_pStream = NULL;
				}
				ConnectStatus = NOTCONNECTED;
				str = "900 unit Out of service set, connection closed";
				pGDev->LogFile.LogLocal(str);
				Log(str,5);
				SetTimer(IDT_START_TCP,5000,NULL);
			}
			else
			{
				SetTimer(IDT_STATUSMONITOR,5000,NULL);
			}
					   }
			break;
		case DSPCR413: {
			C413Obj *p = pGDev->pGEMObjs->StMd.Get413Obj();
			if (p->Service == C413Obj::OUTOFSERVICE)
			{
				if (m_pStream)
				{
					m_pStream->DestroySocket();
					Message("Socket destroyed");
					delete m_pStream;
					m_pStream = NULL;
				}
				ConnectStatus = NOTCONNECTED;
				str = "413 unit Out of service set, connection closed";
				pGDev->LogFile.LogLocal(str);
				Log(str,5);
				SetTimer(IDT_START_TCP,5000,NULL);
			}
			else
			{
				SetTimer(IDT_STATUSMONITOR,5000,NULL);
			}
					   }
			break;
		default:
			ASSERT(0);
			break;
		}
		return ;
	}
	if (nIDEvent == IDT_AREYOUTHERE) 
	{
		KillTimer(IDT_AREYOUTHERE);
		CMsgItem *pMsg = new CMsgItem;
		if (pMsg)
		{
			pMsg->AckWnd = (int)m_hWnd;
			pMsg->Ack = ID_IPC_HELLOACK1;
			pMsg->Nak = ID_IPC_HELLONAK1;
			pMsg->DataTyp = CMsgItem::EVENTDATA;
			pMsg->DataID = CMsgItem::HELLO;
			if (TCPSend(pMsg))
			{
				SetTimer(IDT_AREYOUTHERE_TOUT,tOUT-2000,NULL);
			}
		}
		return ;
	}
	if (nIDEvent == IDT_AREYOUTHERE_TOUT)
	{
		KillTimer(IDT_AREYOUTHERE_TOUT);
		AttemptReconnection();
		return ;
	}
}

BOOL CMessageDispatcher::TCPConnect()
{
	
	if (m_pStream && (ConnectStatus == CONNECTED))
	{
		Message("Already connected");
		return FALSE;
	}
	// initialize the stream socket object
	if (!m_pStream)
	{
		m_pStream = new CStreamSocket(this, WM_USER_STREAM);
		if (m_pStream->CreateSocket() == CWINSOCK_NOERROR)
		{
			Message("Socket created");
		}
		else
		{
			Message("Socket creation failed");
			delete m_pStream;
			m_pStream = NULL;
			return FALSE;
		}
	}

	CString Address="";
	int m_TCPPort=0;
	switch(ID)
	{
	case DSPCR128:
		m_TCPPort = 128;
		Address = DosUtil.GetIPAddr(1);
		break;
	case DSPCR900:
		m_TCPPort = 900;
		Address = DosUtil.GetIPAddr(2);
		break;
	case DSPCR413:
		m_TCPPort = 413;
		Address = DosUtil.GetIPAddr(3);
		break;
	default:
		ASSERT(0);
		break;
	}
	strcpy(m_pszServer,Address.GetBuffer(0));

//	BOOL bConct = TRUE;
//	if (Address.GetLength() < 3)
//	{
//		bConct = FALSE;
//	}

// 	switch(ID) {
// 	case DSPCR128:
// 		{
// 		if (!bConct) {
// 			Log("No 128 Module");
// 			pGDev->pGEMObjs->StMd.Set128Serv(C128Obj::OUTOFSERVICE);
// 			return TRUE;
// 		}
// 		C128Obj *p = pGDev->pGEMObjs->StMd.Get128Obj();
// 		if (p->Service == C128Obj::INSERVICE)
// 		{
// 			AttemptConnection(Address, m_TCPPort);
// 		}
// 		else {
// 			if (bFirst) {
// 				pGDev->SetAlarm(CAlrmList::C128OUTOFSERVICE, "Not connecting: 128 Unit is out of service");
// 				bFirst = FALSE;
// 			}
// 			SetTimer(IDT_START_TCP,5000,NULL);
// 		}
// 				   }
// 		break;
// 	case DSPCR900: {
// 		if (!bConct) {
// 			Log("No 900 Module");
// 			pGDev->pGEMObjs->StMd.Set900Serv(C900Obj::OUTOFSERVICE);
// 			return TRUE;
// 		}
// 		C900Obj *p = pGDev->pGEMObjs->StMd.Get900Obj();
// 		if (p->Service == C900Obj::INSERVICE)
// 		{
// 			AttemptConnection(Address, m_TCPPort);
// 		}
// 		else {
// 			if (bFirst) {
// 				pGDev->SetAlarm(CAlrmList::C900OUTOFSERVICE, "Not connecting: 900 Unit is out of service");
// 				bFirst = FALSE;
// 			}
// 			SetTimer(IDT_START_TCP,5000,NULL);
// 		}
// 				   }
// 		break;
// 	case DSPCR413: {
// 		if (!bConct) {
// 			Log("No 413 Module");
// 			pGDev->pGEMObjs->StMd.Set413Serv(C413Obj::OUTOFSERVICE);
// 			return TRUE;
// 		}
// 		C413Obj *p = pGDev->pGEMObjs->StMd.Get413Obj();
// 		if (p->Service == C413Obj::INSERVICE)
// 		{
// 			AttemptConnection(Address, m_TCPPort);
// 		}
// 		else {
// 			if (bFirst) {
// 				pGDev->SetAlarm(CAlrmList::C413OUTOFSERVICE, "Not connecting: 413 Unit is out of service");
// 				bFirst = FALSE;
// 			}
// 			SetTimer(IDT_START_TCP,5000,NULL);
// 		}
// 				   }
// 		break;
// 	default:
// 		ASSERT(0);
// 		break;
// 	}
	return TRUE;
}

void CMessageDispatcher::AttemptConnection(CString Addrs, short TCPPort)
{
	// connect the client to the server
	if (!m_pStream)
	{
		ASSERT(0);
		return ;
	}
	if (m_pStream->Connect(Addrs.GetBuffer(0), TCPPort) == CWINSOCK_NOERROR)
	{
		Message("Stream connect attempt made");
		ConnectStatus = ATTEMPTCONNECT;
		SetTimer(IDT_CONNECT_TIMEOUT,5000,NULL);
	}
	else
	{
		Message("Socket connect attempt failed");
		SetTimer(IDT_START_TCP,10000,NULL);
	}
}

BOOL CMessageDispatcher::TCPReconnect()
{
	// free the stream and WinSock objects
	if (m_pStream)
	{
		m_pStream->DestroySocket();
		delete m_pStream;
		m_pStream = NULL;
	}
	
	// initialize the stream socket object
	m_pStream = new CStreamSocket(this, WM_USER_STREAM);
	if (!m_pStream)
	{
		ASSERT(0);
		return FALSE;
	}
	if (m_pStream->CreateSocket() == CWINSOCK_NOERROR)
	{
	}
	else
	{
		m_pStream->DestroySocket();
		Message("Socket destroyed");
		delete m_pStream;
		m_pStream = NULL;
		return FALSE;
	}

	CString Addr="";
	int m_TCPPort=0;
	switch(ID)
	{
	case DSPCR128:
		m_TCPPort = 128;
		Addr = DosUtil.GetIPAddr(1);
		break;
	case DSPCR900:
		m_TCPPort = 900;
		Addr = DosUtil.GetIPAddr(2);
		break;
	case DSPCR413:
		m_TCPPort = 413;
		Addr = DosUtil.GetIPAddr(3);
		break;
	default:
		ASSERT(0);
		break;
	}
	// connect the client to the server
	if (m_pStream->Connect(Addr.GetBuffer(0), m_TCPPort) == CWINSOCK_NOERROR)
	{
		ConnectStatus = ATTEMPTCONNECT;
		SetTimer(IDT_RECONNECT_TIMEOUT,5000,NULL);
	}
	else
	{
		m_pStream->DestroySocket();
		Message("Socket destroyed");
		delete m_pStream;
		m_pStream = NULL;
	}
	return TRUE;
}

// Receives messages from the stream object.
//
LONG CMessageDispatcher::OnStream(WPARAM wParam, LPARAM lParam)
{
	int nLen; // length

	if (!m_pStream)
	{
		ASSERT(0);
		return -1L;
	}
	// get pointer to list box used for status messages
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
				Message(str);
			}
// 			else {
// 				str.Format("Sent (%d)",MsgItem.MsgDes);
// 				Message(str);
// 			}
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
		while (1)
		{
			STREAMDATA *pDataRead;
			pDataRead = m_pStream->Read(&nLen);
			if (!pDataRead)
			{
				// No more messages
				break;
			}
			if (pDataRead->pData)
			{
				CMsgItem *pMsgItem = new CMsgItem;
				if (pMsgItem)
				{
					pMsgItem->Conv(pDataRead->pData);
					delete [] pDataRead->pData;
					CString str;
					if (pMsgItem->pMsg)
					{
						str.Format("Received (%d) %s %d",pMsgItem->MsgSrc, pMsgItem->pMsg, pMsgItem->IDIdx);
						Message(str);
					}
// 					else {
// 						str.Format("Received (%d)",pMsgItem->MsgSrc);
// 						Message(str);
// 					}
					DispatchMsg(pMsgItem);
				}
				else
				{
					Message("Alloc error");
				}
			}
			delete pDataRead;
		}
		break;
	case CWINSOCK_ERROR_READING:
// 		switch(ID) {
// 		case DSPCR128:
// 			pGDev->SetAlarm(CAlrmList::C128NOTONLINE,"Read Error: 128 Module");
// 			Log("128 Module: TCPIP Read error",1);
// 			break;
// 		case DSPCR900:
// 			pGDev->SetAlarm(CAlrmList::C900NOTONLINE,"Read Error: 900 Module");
// 			Log("900 Module: TCPIP Read error",1);
// 			break;
// 		case DSPCR413:
// 			pGDev->SetAlarm(CAlrmList::C413NOTONLINE,"Read Error: 413 Module");
// 			Log("413 Module: TCPIP Read error",1);
// 			break;
// 		default:
// 			ASSERT(0);
// 			break;
// 		}
		break;
	case CWINSOCK_YOU_ARE_CONNECTED:
		ConnectStatus = CONNECTED;
		bFirst = TRUE;
		KillTimer(IDT_RECONNECT_TIMEOUT);
		KillTimer(IDT_CONNECT_TIMEOUT);
		if (pGDev)
		{
			if (pGDev->pGEMObjs)
			{
				switch(ID)
				{
				case DSPCR128:
					pGDev->pGEMObjs->StMd.C128.Status=(C128Obj::oIDLE);
					break;
				case DSPCR900:
					pGDev->pGEMObjs->StMd.C900.Status=(C900Obj::oIDLE);
					break;
				case DSPCR413:
					pGDev->pGEMObjs->StMd.C413.Status=(C413Obj::oIDLE);
					break;
				default:
					ASSERT(0);
					break;
				}
			}
		}
		Message("Connected to server");
		SetTimer(IDT_STATUSMONITOR,1000, NULL);
		//SetTimer(IDT_AREYOUTHERE,tOUT,NULL);
		break;
	case CWINSOCK_LOST_CONNECTION:
		KillTimer(IDT_AREYOUTHERE_TIMEOUT);
		KillTimer(IDT_AREYOUTHERE_TOUT);
		// server closed the connection
		Message("Server closed connection");		
		ConnectStatus = NOTCONNECTED;
		m_pStream->DestroySocket();
		Message("Socket destroyed");
		delete m_pStream;
		m_pStream = NULL;
		if (pGDev)
		{
			if (pGDev->pGEMObjs)
			{
// 				switch(ID) {
// 				case DSPCR128:
// 					pGDev->pGEMObjs->StMd.C128.Status=(C128Obj::OFFLINE);
// 					pGDev->SetAlarm(CAlrmList::C128NOTONLINE, "128 unit not online");
// 					break;
// 				case DSPCR900:
// 					pGDev->pGEMObjs->StMd.C900.Status=(C900Obj::OFFLINE);
// 					pGDev->SetAlarm(CAlrmList::C900NOTONLINE, "900 unit not online");
// 					break;
// 				case DSPCR413:
// 					pGDev->pGEMObjs->StMd.C413.Status=(C413Obj::OFFLINE);
// 					pGDev->SetAlarm(CAlrmList::C900NOTONLINE, "413 unit not online");
// 					break;
// 				default:
// 					ASSERT(0);
// 					break;
// 				}
			}
		}
		TCPReconnect();
		break;
	case CWINSOCK_WINDOWS_ERROR:
		Message("Winsock Windows Error");
		ASSERT(0);
		break;
	case CWINSOCK_WINSOCK_ERROR:
		Message("Winsock Error");
		ASSERT(0);
		break;
	default:
		break;
	}
	return 0L;
}

BOOL CMessageDispatcher::TCPDisconnect()
{
	if (ConnectStatus == ATTEMPTCONNECT)
	{
		KillTimer(IDT_CONNECT_TIMEOUT);
		KillTimer(IDT_RECONNECT_TIMEOUT);
	}
	if (ConnectStatus != NOTCONNECTED)
	{
		// free the stream and WinSock objects
		if (m_pStream)
		{
			ConnectStatus = NOTCONNECTED;
			m_pStream->DestroySocket();
			Message("Socket destroyed");
			delete m_pStream;
			m_pStream = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMessageDispatcher::TCPSend(CMsgItem *pMsg)
{
	if (m_pStream && pMsg)
	{
		STREAMDATA *pStreamData = new STREAMDATA;
		if (pStreamData)
		{
			pStreamData->nLen = pMsg->GetLength()+sizeof(int);
			pStreamData->pData = new char [pStreamData->nLen+16];
			if (pStreamData->pData)
			{
				char *p = pStreamData->pData;
				CMake::Binary(pStreamData->nLen,&p);
				pMsg->Binary(p);
				delete pMsg;
				if (m_pStream->Write(pStreamData) == CWINSOCK_NOERROR)
				{
					//Message("Data submitted");
					return TRUE;
				}
				delete [] pStreamData->pData;
			}
			delete pStreamData;
		}
	}
	delete pMsg;
	Message("Fail to send data");
	return FALSE;
}


BOOL CMessageDispatcher::DispatchMsg(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		ASSERT(0);
		return FALSE;
	}
	if (pMsg->DataTyp == CMsgItem::SCANDATA)
	{
		ProcessScanData(pMsg);
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::EVENTDATA)
	{
		ProcessEventData(pMsg);
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::ROBOTCMD)
	{
		RbtCmdQue.AddT(pMsg); //  [12/2/2007 YUEN]
		//ProcessRbtCmd(pMsg); //  [12/2/2007 YUEN]
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::DATAREQACK)
	{
		ProcessDataReqAck(pMsg);
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::COMMANDACK)
	{
		ProcessCommandAck(pMsg);
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::SMOLMSG)
	{
		ProcessSMOLMsg(pMsg);
		return TRUE;
	}
	else if (pMsg->DataTyp == CMsgItem::EQPMSG)
	{
		ProcessEqpMsg(pMsg);
		return TRUE;
	}
	else
	{
		// Wrong routing
		delete pMsg;
		ASSERT(0);
	}
	return FALSE;
}

void CMessageDispatcher::OnConnect() 
{
	if (ConnectStatus == CONNECTED)
	{
		if (AfxMessageBox("Current connection will be lost\nProceed?",MB_YESNO) != IDYES)
		{
			return ;
		}
	}
	TCPReconnect();
}

void CMessageDispatcher::OnAreyouthere() 
{
	CMsgItem *pMsg = new CMsgItem;
	if (pMsg)
	{
		pMsg->AckWnd = (int)m_hWnd;
		pMsg->Ack = ID_IPC_HELLOACK;
		pMsg->Nak = ID_IPC_HELLONAK;
		pMsg->DataTyp = CMsgItem::EVENTDATA;
		pMsg->DataID = CMsgItem::HELLO;
		if (TCPSend(pMsg))
		{
			Message("Are You There send");
			SetTimer(IDT_AREYOUTHERE_TIMEOUT,3000,NULL);
		}
	}
}

void CMessageDispatcher::OnIpcHelloack() 
{
	KillTimer(IDT_AREYOUTHERE_TIMEOUT);	
	Message("Recieve Are You There acknowledge");
}

void CMessageDispatcher::OnIpcHellonak() 
{
	KillTimer(IDT_AREYOUTHERE_TIMEOUT);	
	Message("Could not send message");
}


void CMessageDispatcher::OnIpcHelloack1() 
{
	KillTimer(IDT_AREYOUTHERE_TOUT);	
	SetTimer(IDT_AREYOUTHERE,tOUT,NULL);
}

void CMessageDispatcher::OnIpcHellonak1() 
{
	KillTimer(IDT_AREYOUTHERE_TOUT);	
	if ((ConnectStatus == NOTCONNECTED) || (ConnectStatus == ATTEMPTCONNECT))
	{
	}
	else 
	{
		AttemptReconnection();
		//SetTimer(IDT_AREYOUTHERE,tOUT,NULL);
	}
}

// pMsg memory allocation will be taken care of by receiving function
void CMessageDispatcher::ProcessDataReqAck(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		return ;
	}

	if (pMsg->AckWnd)
	{
		if (pMsg->Ret)
		{
			::PostMessage((HWND)pMsg->AckWnd,pMsg->Ack,999,(LPARAM)pMsg);
		}
		else
		{
			::PostMessage((HWND)pMsg->AckWnd,pMsg->Nak,999, NULL);
			delete pMsg;
		}
		return ;
	}
	delete pMsg;
}

void CMessageDispatcher::ProcessCommandAck(CMsgItem *pMsg)
{
	if (!pMsg)
	{
		return ;
	}

	if (pMsg->AckWnd)
	{
		if (pMsg->Ret)
		{
			::PostMessage((HWND)pMsg->AckWnd,pMsg->Ack,999,(LPARAM)pMsg);
		}
		else
		{
			::PostMessage((HWND)pMsg->AckWnd,pMsg->Nak,999, NULL);
			delete pMsg;
		}
		return ;
	}
	delete pMsg;
}

void CMessageDispatcher::ProcessSMOLMsg(CMsgItem *pMsg)
{
	CSMOL *pSM = (CSMOL *)pMsg->pMsg;

	if (pGDev->pOLnMgr && pGDev->pOLnMgr->pOLnSvrObj)
	{
		pGDev->pOLnMgr->pOLnSvrObj->ProcessCmd(*pSM);
	}

	delete pMsg;
}


void CMessageDispatcher::RejectRbtCmd(CMsgItem *pMsg)
{
	CMsgItem *p = new CMsgItem;
	if (p)
	{
		p->DataTyp = CMsgItem::ROBOTEVENT;
		p->Ack = 0;
		p->Nak = pMsg->Nak;
		p->AckWnd = pMsg->AckWnd;
		Message("->RbtXfer fail",0);
		Send(p);
	}
}

BOOL CMessageDispatcher::CanTransfer(CMsgItem *pMsg)
{
	char ch;
	char dmy[128];
	short Stn1, Stn2,Slt1, Slt2;
	CString str = pMsg->pMsg;
	sscanf(pMsg->pMsg,"%s %s %c %d %s %d %d %s %d", dmy, dmy, &ch, &Stn1, dmy, &Slt1, &Stn2, dmy, &Slt2 );
	if (str.Find("XFER")!=-1)
	{
		switch(Stn1)
		{
		case 4:
		case 5:
		case 8:
		case 9:
			// LYF Could not transfer involving foup because it is not opened [4/17/2006]
			if (pGDev->GetLPState(1) != PrcsMgrServices::OPENED)
			{
				return FALSE;
			}
			break;
		case 6:
		case 7:
		case 10:
		case 11:
			// LYF Could not transfer involving foup because it is not opened [4/17/2006]
			if (pGDev->GetLPState(2) != PrcsMgrServices::OPENED)
			{
				return FALSE;
			}
			break;
		default:
			break;
		}
		switch(Stn2)
		{
		case 4:
		case 5:
		case 8:
		case 9:
			// LYF Could not transfer involving foup because it is not opened [4/17/2006]
			if (pGDev->GetLPState(1) != PrcsMgrServices::OPENED)
			{
				return FALSE;
			}
			break;
		case 6:
		case 7:
		case 10:
		case 11:
			// LYF Could not transfer involving foup because it is not opened [4/17/2006]
			if (pGDev->GetLPState(2) != PrcsMgrServices::OPENED)
			{
				return FALSE;
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}

void CMessageDispatcher::ProcessEqpMsg(CMsgItem *pMsg)
{
	if (pMsg)
	{
		CString msg;
		pMsg->Get(msg);
		pGDev->LogFile.LogLocal(msg);
		delete pMsg;
	}
}

void CMessageDispatcher::AttemptReconnection()
{
	Message("Server closed connection");		
	ConnectStatus = NOTCONNECTED;
	m_pStream->DestroySocket();
	Message("Socket destroyed");
	delete m_pStream;
	m_pStream = NULL;
	TCPReconnect();
}

void CMessageDispatcher::PostNcDestroy() 
{
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CMessageDispatcher::OnDestroy() 
{
	if (pFFUFailDlg)
	{
		pFFUFailDlg->DestroyWindow();
		pFFUFailDlg = NULL;
	}
	// free the stream and WinSock objects
	if (m_pStream)
	{
		m_pStream->DestroySocket();
		delete m_pStream;
		m_pStream = NULL;
	}
	if (m_pWinSock)
	{
		m_pWinSock->Shutdown();
		delete m_pWinSock;
		m_pWinSock = NULL;
	}
//	if (pE95BEM) {
//		delete pE95BEM;
//	}
	CResizableDialog::OnDestroy();
}
