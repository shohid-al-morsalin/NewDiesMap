// ComThread.cpp : implementation file
//
#include "stdafx.h"
#include "ComThread.h"
#include "ComTDK.h"
#include "ComFL300.h"

//#include "resource.h"
#include "..\Sys\Var.h"
#include "..\IPC\SMOL.h"
#include "..\IPC\SMHdr.h"
#include "..\IPC\SMCHdr.h"
#include "..\Src\DosUtil.h"
#include "ComBase.h"
#include "..\Src\Comm.h"
#include "..\Src\DOSUtil.h"
//#include "c2c.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif

#define IDT_REFRESH	9999
/////////////////////////////////////////////////////////////////////////////
// CComThread

//IMPLEMENT_DYNCREATE(CComThread, CWinThread)

CComThread::CComThread()
{
	pCom = NULL;

    bStarted = FALSE;
	KillName = "ComThreadKill";
}
	
CComThread::~CComThread()
{
	HANDLE hDone; // Server informs client mutex
	hDone =::CreateEvent(NULL, TRUE, FALSE, KillName);
	SetEvent(hDone);
	CloseHandle(hDone);

	if (pCom) {
		delete pCom;
		pCom = NULL;
	}
}

BOOL CComThread::InitInstance()
{
//	if (!SvrName.GetLength())
//	{
//		ASSERT(0);
//	}
	return TRUE;
}

int CComThread::ExitInstance()
{
	Disconnect();
	return 0;
}

BEGIN_MESSAGE_MAP(CComThread, CWinThread)
//{{AFX_MSG_MAP(CComThread)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComThread message handlers
BOOL CComThread::OnIdle(LONG lCount)
{
	bStarted = TRUE;
	//static short cnt = 0;
	CheckComIn();
	/*if (!(cnt % 8))*/ /*CheckQueOut();*/

	// This is CheckQueOut()
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (SMlist.size())
		{
			vector < CSMHdr >::iterator i;
			i = SMlist.begin();
			SendMsg(*i);
			SMlist.erase(i);
		}
	}
//	Sleep(50);
	Sleep(20);
	return TRUE;
}

BOOL CComThread::SendMsg(CSMHdr & SM)
{
	BOOL ret;
	CSMOL SM1;
	switch (SM.cmd)
	{
		case CSMHdr::CTCONNECT:
			SM1.Dv.success = FALSE;
			if (Connect(SM.sub[0])) {
				if (Reset(DosUtil.GetLPUnit("CalibAfterReset",TRUE))) {
					if (Auto()) {
						BOOL bNV = DosUtil.GetLPUnit("NeedVac",FALSE);
						DosUtil.SetLPUnit("NeedVac", bNV);
						if (bNV) {
							ret = NeedVac();
						}
						else {
							ret = NoVac();
						}
						SM1.Dv.success = ret;
					}
				}
			}
			//SM1.cmd = CSMOL::cmdDEVICE;
			//SM1.Dv.Preset();
			//SM1.Dv.act = DeviceCmd::CONNECT;
			//SM1.Dv.success = ret;
			//SM1.HWin = SM.HWin;
			//SM1.Ack = SM.Ack;
			//SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTDISCONNECT:
			ret = Disconnect();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::DISCONNECT;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTRESET:
			ret = Reset();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::RESET;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTABORT:
			ret = Abort();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::ABORT;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTAUTO:
			ret = Auto();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::AUTO;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTNOVAC:
			ret = NoVac();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::NOVAC;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTNEEDVAC:
			ret = NeedVac();
			SM1.cmd = CSMOL::cmdDEVICE;
			SM1.Dv.Preset();
			SM1.Dv.act = DeviceCmd::NEEDVAC;
			SM1.Dv.success = ret;
			SM1.HWin = SM.HWin;
			SM1.Ack = SM.Ack;
			SM1.Nak = SM.Nak;
			Reply(SM,ret);
			//Reply1(SM.Caller, SM1);
			break;
		case CSMHdr::CTOPEN:
			Reply(SM, Open());
			break;
		case CSMHdr::CTCLOSE:
			Reply(SM, Close());
			break;
		case CSMHdr::CTSETDEFAULT:
			Reply(SM, SetDefault());
			break;
		case CSMHdr::CTCLOSENORELEASE:
			Reply(SM, CloseNoRelease());
			break;
		case CSMHdr::CTCALIB:
			Reply(SM, Calib());
			break;
		case CSMHdr::CTMAP:
			Reply(SM,Map());
			break;
		case CSMHdr::CTREGISTER:
			if (pCom) {
				if (SM.Ack == 0) {
					pCom->hWnd = SM.HWin;
				}
				else {
					pCom->hWnd = pCom->hWndLPBar = SM.HWin;
				}
			}
			break;
		case CSMHdr::CTSTATUS:
			Reply(SM, Status());
			break;
		case CSMHdr::CTSTATUSI:
			Reply(SM,Status());
			break;

		case CSMHdr::CTACCESSMODECHANGE:
			{
				int mode = (int)SM.sub[1];
				Reply(SM, AccessModeChange((LoadPortObj::ACCESSMODE) mode));
			}
			break;
		case CSMHdr::CTCLAMP:
			Reply(SM, Clamp());
			break;
		case CSMHdr::CTUNCLAMP:
			Reply(SM, Unclamp());
			break;
		case CSMHdr::CTLEDLOADUNLOAD:
			Reply(SM, LEDLoadUnload((PrcsMgrServices::LPBSTATE)int(SM.sub[1])));
			break;
		case CSMHdr::CTCLEARALLLED:
			Reply(SM, ClearAllLED());
			break;
		case CSMHdr::CTLEDRESERVED:
			Reply(SM, LEDReserved(SM.sub[0]));
			break;
		case CSMHdr::CTLEDMANUAL:
			Reply(SM, LEDManual(SM.sub[0]));
			break;
		case CSMHdr::CTENABLEHOAVBL:
			Reply(SM,EnableHOAvbl(SM.sub[0]));
			break;
		case CSMHdr::CTLEDALARMED:
			Reply(SM,LEDAlarm(SM.sub[0]));
			break;
		default:
			ASSERT(0);
			break;
	}
	return TRUE;
}

// Called from other thread only. Will stall if call from same thread [5/20/2002]
//void CComThread::PushButton2()
//{
//	if (hWnd)
//	{
//		::PostMessage(hWnd, WM_COMMAND, 32806, NULL);
//	}

void CComThread::Log(CString msg, short lvl)
{
	CSMClt ipc;
	if (ipc.Start1("LogSvr", sizeof(CSMHdr))) {
		ipc.LogMsg("CommThread", "", msg,lvl);
	}
}

void CComThread::Reply1(CString Caller, CSMOL &SM)
{
	CSMClt ipc;
	if (ipc.Start1(Caller,sizeof(CSMOL))) {
		ipc.Send(&SM);
	}
}
BOOL CComThread::StartCom()
{
	CString str;
	str = DosUtil.GetLPUnit("Type","TDK");
	str.MakeUpper();
	if (str == "TDK") {
		pCom = new CComTDK;
        pCom->bSimu = bSimu;
	}
	else {
		pCom = new CComFL300;
        pCom->bSimu = bSimu;
	}
	return TRUE;
}
CString CComThread::GetSlotMap()
{
    if (pCom) 
	{
        return pCom->Slotmap;
	}
    return "000000000000000000000000000";
}

BOOL CComThread::GetStatus(BOOL & bRdy, BOOL & bInP, BOOL & bAlr)
{
    if (!pCom) {
        bRdy = FALSE;
        bInP = FALSE;
        bAlr = FALSE;
        return FALSE;
    }
    bRdy = pCom->bReady;
	bInP = pCom->bInPlace;
	bAlr = pCom->bAlarmed;
    return TRUE;
}

void CComThread::WaitTillStart()
{
	while (!bStarted)
	{
		Sleep(100);
	}
}

BOOL CComThread::Connect(short port)
{
	if (pCom) {
		return pCom->Connect(port);
	}
	return FALSE;
}

BOOL CComThread::Disconnect()
{
	if (pCom)
	{
		return pCom->Disconnect();
	}
	return TRUE;
}

//void CComThread::CheckQueOut()
//{
//	if (pCom) {
//		pCom->CheckQueOut();
//	}
//}

void CComThread::CheckComIn()
{
	if (pCom) {
		pCom->CheckComIn();
	}
}


BOOL CComThread::NeedVac()
{
	if (pCom) {
		return pCom->NeedVac();
	}
	return FALSE;
}

BOOL CComThread::Auto()
{
	if (pCom) {
		return pCom->Auto();
	}
	return FALSE;
}

BOOL CComThread::Reset(BOOL bCalib)
{
	if (pCom) {
		return pCom->Reset(bCalib);
	}
	return FALSE;
}

BOOL CComThread::Calib()
{
	if (pCom) {
		return pCom->Calib();
	}
	return FALSE;
}

BOOL CComThread::NoVac()
{
	if (pCom) {
		return pCom->NoVac();
	}
	return FALSE;
}

BOOL CComThread::Reply(CSMOL & SM, BOOL ret)
{
	if (pCom) {
		return pCom->Reply(SM, ret);
	}
	return FALSE;
}

BOOL CComThread::Reply(CSMHdr & SM, BOOL ret)
{
	if (pCom) {
		return pCom->Reply(SM, ret);
	}
	return FALSE;
}

BOOL CComThread::Open()
{
	if (pCom) {
		return pCom->Open();
	}
	return FALSE;
}

BOOL CComThread::Close()
{
	if (pCom) {
		return pCom->Close();
	}
	return FALSE;

}
BOOL CComThread::SetDefault()
{
	if (pCom) {
		return pCom->SetDefault();
	}
	return FALSE;
}

BOOL CComThread::CloseNoRelease()
{
	if (pCom) {
		return pCom->CloseNoRelease();
	}
	return FALSE;
}
BOOL CComThread::Map()
{
	if (pCom) {
		return pCom->Map();
	}
	return FALSE;
}

BOOL CComThread::Unclamp()
{
	if (pCom) {
		return pCom->Unclamp();
	}
	return FALSE;
}

BOOL CComThread::Clamp()
{
	if (pCom) {
		return pCom->Clamp();
	}
	return FALSE;
}

BOOL CComThread::Status()
{
	if (pCom) {
		return pCom->Status();
	}
	return FALSE;
}

BOOL CComThread::AccessModeChange(LoadPortObj::ACCESSMODE mode)
{
	if (pCom) {
		return pCom->AccessModeChange(mode);
	}
	return FALSE;
}

BOOL CComThread::LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState)
{
	if (pCom) {
		return pCom->LEDLoadUnload(LpState);
	}
	return FALSE;
}

BOOL CComThread::ClearAllLED()
{
	if (pCom) {
		return pCom->ClearAllLED();
	}
	return FALSE;
}

BOOL CComThread::LEDReserved(BOOL on)
{
	if (pCom) {
		return pCom->LEDReserved(on);
	}
	return FALSE;
}

BOOL CComThread::LEDManual(BOOL on)
{
	if (pCom) {
		return pCom->LEDManual(on);
	}
	return FALSE;
}

BOOL CComThread::EnableHOAvbl(short port)
{
	if (pCom) {
		return pCom->EnableHOAvbl(port);
	}
	return FALSE;
}

BOOL CComThread::LEDAlarm(BOOL on)
{
	if (pCom) {
		return pCom->LEDAlarm(on);
	}
	return FALSE;
}

BOOL CComThread::AddToSMList(CSMHdr & SM)
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		SMlist.push_back(SM);
		return TRUE;
	}
	return FALSE;
}


BOOL CComThread::Abort()
{
	if (pCom) {
		return pCom->Abort();
	}
	return FALSE;

}
