// SocketWndInvisible.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "SocketWndInvisible.h"
#include "AfxWin.h"
#include "stdafx.h"

#include "../online/TCPIP\HSMSComThread.h"
#include "../online/TCPIP\HsmsParaMeter.h"
#include "../online/TCPIP\SelectMode.h"
#include "../online/MFC\EquipEvtThread.h"
#include "../online/TCPIP\monitor.h"
#include "../online/TCPIP\ComLog.h"
#include "../online/INC\Event.h"
#include "../online/ShareMemory\ShareMemory.h"
//#include "../online/MFC\alarmlistdlg.h"
//#include "../online/GEM\AlaramList.h"
#include "../online/OLn\OLnSvr.h"
#include "../online/MFC\PropertyFrame.h"
//#include "../online/MFC\HostMessage.h"
#include "../online/MFC\SpoolMan.h"
//#include "../online/MFC\AlarmDlg.h"
//#include "../online/MFC\RecipeDlg.h"
//#include "../online/MFC\GemParaPage1.h"
//#include "MFC\ur_resource.h"

#include "../online/GEM\PRJob.h"
#include "../online/GEM\PRJobMgr.h"
#include "../online/GEM\CtrlJobMgr.h"
#include "..\IPC\SMOL.h"
#include "..\Src\DosUtil.h"
#include ".\socketwndinvisible.h"

//void ChangeControlStatus(EqpStatus::CONTROLSTATE nNewStatus);

// CSocketWndInvisible dialog
BOOL bDisabled = FALSE;
#define PASSIVE		0
#define ACTIVE		1
#define MAX_CONSOLE_MSG 310
#define IDT_TIMER1	(100)

char    m_gTerminal1[MAX_CONSOLE_MSG +100];
char    m_gTerminal2[150];
HWND    m_gHwnd;
CRect   m_gRect;
BOOL    m_COMMODE = FALSE;
//CAlaramList    m_AlarmArry;
BOOL	m_gConnectFlag = FALSE;
HDC		m_hDC;


BufMon				m_BufMon;
CComLog				m_Log;
CShareMemory        m_ShareMemory; 

CHSMSComThread* pThread = NULL;
CEquipEvtThread* pEvtThread= NULL;
COLnSvr* pSthread= NULL;
void DelThrds();
IMPLEMENT_DYNAMIC(CSocketWndInvisible, CDialog)
CSocketWndInvisible::CSocketWndInvisible(CWnd* pParent /*=NULL*/)
	: CDialog(CSocketWndInvisible::IDD, pParent)
{
	bOnlineSelected = FALSE;
}

CSocketWndInvisible::~CSocketWndInvisible()
{
	m_ShareMemory.CloseSharedMemory();
}

void CSocketWndInvisible::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

LONG CSocketWndInvisible::OnAsyncSelect(WPARAM wPram, LPARAM lParam)
{
	char         pszMessage[100];
	//static char  pBuf[101];
	//static int   nTotalBytesToSend;  
	//static int   nBytesToSend = 0;
	int          nError;
	static       SOCKADDR_IN addrForm;
	static int   nAddrFormLen = sizeof(addrForm);
	static       IN_ADDR inForm;
	//int          i = 0; 
	CSelectMode  m_cSelectMode;
	//int			 LineMode = 100;
	//int			 nSocketError = 0;
	
	// if (! m_gConnectFlag) return 0L;
	
	// if (WSAGETSELECTERROR(lParam) != 0) return 0L;
	
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT :
			if (bOnlineSelected)
			{ 
				SOCKET Temp = accept(m_s , (LPSOCKADDR)&addrForm , &nAddrFormLen);
				nSocketError =shutdown(Temp, 3);
				closesocket(Temp);
				return 0L;
			}
			CHSMSComThread::m_AcceptFlag = FALSE;
			if (CHsmsParaMeter::m_sClientSk !=  INVALID_SOCKET)
			{
				closesocket(CHsmsParaMeter::m_sClientSk);
				CHsmsParaMeter::m_sClientSk = INVALID_SOCKET;
			}
			
			if (CHsmsParaMeter::m_sClientSk == INVALID_SOCKET) 
			{
				CHsmsParaMeter::m_sClientSk = accept(m_s , (LPSOCKADDR)&addrForm ,
															  &nAddrFormLen);
				if (CHsmsParaMeter::m_sClientSk == INVALID_SOCKET)
				{
					nError = WSAGetLastError();
					if (nError != WSAEWOULDBLOCK)
					{
						wsprintf(pszMessage , "ERROR %d Accepting connection", nError);
						AfxMessageBox(pszMessage);
					}
				}
				else
				{
					memcpy(&inForm, &addrForm.sin_addr.s_addr, 4);
					wsprintf(pszMessage , "CONNECTION ACCEPT FROM %s, %d\n" , 
						inet_ntoa(inForm), ntohs(addrForm.sin_port));
				}
            
				if (! m_cSelectMode.Pass_SelectCedure(CHsmsParaMeter::m_T7Timer))
				{
					closesocket(CHsmsParaMeter::m_sClientSk);
					CHsmsParaMeter::m_sClientSk = INVALID_SOCKET; 
				}
				else
				{
					CHSMSComThread::m_AcceptFlag = TRUE;           // Communication Thread Running
//					CGemParaPage1::m_ChgmodePossible = TRUE;      // Line mode change possible 
					CHsmsParaMeter::m_ChgmodePossible = TRUE;      // Line mode change possible 
//LYF9
//					LineMode = AfxGetApp()->GetProfileInt("ControlState", "LineMode", EqpStatus::OFFLINEHOST);
//					if ((LineMode < EqpStatus::OFFLINEHOST) || (LineMode > EqpStatus::REMOTE))
//					{
//						LineMode = EqpStatus::OFFLINEHOST;
//					}
//					CEvt *pEvt = new CEvt;
//					ASSERT(pEvt);
//					LineMode = CHsmsParaMeter::GGetDefOflMode();
//					switch (LineMode)
//					{
//					case EqpStatus::LOCAL:
//						{
//							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_LOCALEVT); 
//							break;
//						}
//					case EqpStatus::REMOTE:
//						{ 
//							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE); 
//							break;
//						}
//					case EqpStatus::OFFLINEHOST:
//						{	
//							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_HOSTOFF); 
//							break;
//						}
//					case EqpStatus::OFFLINEEQP:
//						{	
//							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_OFFLINE); 
//							break;
//						}
//					}
//					CSharedMemQue EvtQue;
//					EvtQue.OpenMemory("EVTMSG");
//					EvtQue.WriteQue(pEvt);
//					EvtQue.CloseMemory();
//					bOnlineSelected = TRUE;
//					
//					m_gDataBank.m_EStatus.LineMode = (EqpStatus::CONTROLSTATE) LineMode;
//					PostMessage(UWM_USER_ONLINEMODE, 1, m_gDataBank.m_EStatus.LineMode);
				}
			}
			else
			{
				closesocket(CHsmsParaMeter::m_sClientSk);
				CHsmsParaMeter::m_sClientSk = INVALID_SOCKET; 
			}
			break;
			
		case FD_CLOSE : 
			{
				CSharedMemQue EvtQue;
				EvtQue.OpenMemory("EVTMSG");
				CEvt *pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_INIT_DATA);
				EvtQue.WriteQue(pEvt);
				EvtQue.CloseMemory();	
				//ChangeControlStatus(CHsmsParaMeter::GGetDefOflMode());	// Yuen:  [2002/10/30  3:29]
				//LYF9 ChangeControlStatus(EqpStatus::OFFLINEEQP);	// Yuen:  [2002/10/30  3:29]

	//			CGemParaPage1::m_ChgmodePossible = FALSE;
				CHsmsParaMeter::m_ChgmodePossible = FALSE;
				nSocketError =shutdown(CHsmsParaMeter::m_sClientSk, 3);
				closesocket(CHsmsParaMeter::m_sClientSk);
				CHsmsParaMeter::m_sClientSk = INVALID_SOCKET; 
				bOnlineSelected = FALSE;						
			}
			break;
		case FD_WRITE : 
			break;
		case FD_ROUTING_INTERFACE_CHANGE: 
			break;
		case FD_ADDRESS_LIST_CHANGE: 
			break;
		default    :
			break;
	}
	return 0L;
}

LONG CSocketWndInvisible::OnHostMsg(WPARAM wParam, LPARAM lParam)
{
	int nMsgLen = wParam;
	//if ((0 == nMsgLen)|| (500 < nMsgLen))
	//{
		// hide the button
		//m_ColorButtonLst[2].ShowWindow(SW_HIDE);
	//}
	//else
	//{
		// show button
		//m_ColorButtonLst[2].ShowWindow(SW_SHOW);
	//}
	return wParam;
}

LONG CSocketWndInvisible::UpdateAlarm(WPARAM wParam, LPARAM lParam)
{
	//int Alcnt  = m_AlarmArry.AlarmSetCount();
	//m_ColorButtonLst[1].ShowWindow((Alcnt)?SW_SHOW:SW_HIDE);
	return wParam;
}


LONG CSocketWndInvisible::OnChangeMode(WPARAM wParam , LPARAM lParam)
{
	if ((int)wParam == 1)
	{
		CString str;
		switch (lParam)
		{
			case EqpStatus::OFFLINEHOST: 
				str = "Offline Host";
				break;
			case EqpStatus::OFFLINEEQP: 
				str = "Offline Eqp";
				break;
			case EqpStatus::REMOTE:str = "Online Remote";
				break;
			case EqpStatus::LOCAL:str = "Online Local";
				break;
			default:	
				return FALSE;
		}
		//m_ColorButton.Change_Caption(str);
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(CSocketWndInvisible, CDialog)
    ON_MESSAGE(WM_USER_ASYNC_SELECT, OnAsyncSelect)
    ON_MESSAGE(WM_HOST_MESSAGE_RECD, OnHostMsg)
    ON_MESSAGE(WM_ALARM_DATA_CHANGED, UpdateAlarm)	
    ON_MESSAGE(UWM_USER_ONLINEMODE , OnChangeMode)
	ON_MESSAGE(UWM_USER_HOMEFINISHED , OnDoneRBHome)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSocketWndInvisible message handlers

BOOL CSocketWndInvisible::OnInitDialog()
{
	CDialog::OnInitDialog();
	::pSthread = new COLnSvr();
	if (! pSthread)
		AfxMessageBox("Thread alloc error");
	else
	{
		if (! pSthread->CreateThread(CREATE_SUSPENDED))
			AfxMessageBox("CreateThread error");
		else
		{
			pSthread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			VERIFY(pSthread->ResumeThread());
		}
	}
	
	::pEvtThread = new CEquipEvtThread();
	if (! pEvtThread)
		AfxMessageBox("Thread alloc error");
	else
	{
		if (! pEvtThread->CreateThread(CREATE_SUSPENDED))
			AfxMessageBox("CreateThread error");
		else
		{
			pEvtThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			VERIFY(pEvtThread->ResumeThread());
		}
	}
//	::PostMessage(m_gHwnd, UWM_USER_HOMEFINISHED, 0, 0);
	atexit(DelThrds);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DelThrds()
{
	delete pThread;
	delete pEvtThread;
	delete pSthread;
}

LONG CSocketWndInvisible::OnDoneRBHome(WPARAM wParam , LPARAM lParam)
{
//	Log("Initializing Sockets",3);

	memset(m_gTerminal1 , ' ' , /*sizeof m_gTerminal1*/MAX_CONSOLE_MSG);
	
	if (CHsmsParaMeter::m_ConnectionMode == PASSIVE)
	{
		Log("Passive Connection",3);
		Socket_Initialization();
	}
	else 
	{
		Log("Active Connection",3);
	}
	
	::pThread = new CHSMSComThread(GetSafeHwnd());
	if (! pThread)
	{
		ASSERT(0);
		AfxMessageBox("IDS_THREADERROR1");
	}
	else
	{
		if (! pThread->CreateThread(CREATE_SUSPENDED))
		{
			ASSERT(0);
			AfxMessageBox("IDS_THREADERROR1");
		}
		else
		{
			pThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			VERIFY(pThread->ResumeThread());
		}
	}
	return 0l;
}

//void CSocketWndInvisible::Log(CString &msg)
//{
//	CSMClt ipcLog;
//	if (ipcLog.Start1("LogSvrE95", sizeof(CSMHdr))) 
//	{
//		ipcLog.LogMsg("OnlineView", "", msg, 3);
//	}
//}


void CSocketWndInvisible::Log(const CString &msg,short LogLvl)
{
//	CSMClt ipcLog;
//	if (ipcLog.Start1("LogSvrE95", sizeof(CSMHdr))) 
//	{
//		ipcLog.LogMsg("OnlineView", "", msg, LogLvl);
//	}
	if (pLog) {
		pLog->Log(msg,LogLvl,"SocketWndInvisible");
	}
}

BOOL CSocketWndInvisible::Socket_Initialization()
{
	BOOL   ret  = TRUE;
	char   pszMessage[100];
	m_s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (m_s == INVALID_SOCKET) 
	{
		//DWORD Err = GetLastError();
		AfxMessageBox("SOCKET FAIL");
	}
	else
	{
		m_addr.sin_family = AF_INET;
		m_addr.sin_port =  htons(CHsmsParaMeter::m_SocketPortNo);
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		if (bind(m_s, (LPSOCKADDR)&m_addr, sizeof(m_addr)) == SOCKET_ERROR) 
		{
			AfxMessageBox("BIND FAIL");
		}
		else
		{
			SOCKADDR_IN addr;
			int nAddrLen = sizeof(addr);
			if (getsockname(m_s , (LPSOCKADDR)&addr, &nAddrLen) == SOCKET_ERROR)
				AfxMessageBox("COULD NOT GET PORT");
			else
			{
				wsprintf(pszMessage, "%d", ntohs(addr.sin_port));
				if (WSAAsyncSelect(m_s, m_hWnd, WM_USER_ASYNC_SELECT, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) 
				{
					AfxMessageBox("STREAM ECHO SEVER COULD NOT DO ASYNC SELECT");
				}
				else
				{
					if (listen(m_s, 3) == SOCKET_ERROR)
					{
						AfxMessageBox("STREM ECHO SEVER COUNLD NOT LISTEN");
					}
				}
			}
		}
	}
	return ret;
}


void CSocketWndInvisible::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
