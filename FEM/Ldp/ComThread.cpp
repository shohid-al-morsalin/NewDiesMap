#include "stdafx.h"

#include "ComBase.h"
#include "ComFL300.h"
#include "ComTDK.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMCHdr.h"
#include "IPC/SMHdr.h"
#include "SRC/DOSUtil.h"

#include "ComThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif

#define IDT_REFRESH	9999

//IMPLEMENT_DYNCREATE(CComThread, CWinThread)

CComThread::CComThread() {
	pCom = NULL;

	bRun = FALSE;
	bSimu = FALSE;
	bStarted = FALSE;
	KillName = "ComThreadKill";
}

CComThread::~CComThread() {
	HANDLE hDone; // Server informs client mutex
	hDone = ::CreateEvent(NULL, TRUE, FALSE, KillName);
	if (hDone) {
		SetEvent(hDone);
		CloseHandle(hDone);
	}

	if (pCom) {
		delete pCom;
		pCom = NULL;
	}
}

BOOL CComThread::InitInstance() {
	bRun = TRUE;
	return TRUE;
}

int CComThread::ExitInstance() {
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
BOOL CComThread::OnIdle(LONG lCount) {
	bStarted = TRUE;
	CheckComIn();

	// This is CheckQueOut()
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (SMlist.size()) {
		std::vector < CSMHdr >::iterator i;
		i = SMlist.begin();
		SendMsg(*i);
		SMlist.erase(i);
	}
	Sleep(10);
	return TRUE;
}

int CComThread::Run() {
	bStarted = TRUE;
	while (bRun) {
		CheckComIn();

		// This is CheckQueOut()
		CSingleLock Lock(&CS);
		Lock.Lock();
		if (SMlist.size()) {
			std::vector < CSMHdr >::iterator i;
			i = SMlist.begin();
			SendMsg(*i);
			SMlist.erase(i);
		}
		Lock.Unlock();
		Sleep(10);
	}
	bStarted = FALSE;
	return 0;
}

BOOL CComThread::SendMsg(CSMHdr& SM) {
	BOOL ret = FALSE;
	CSMOL SM1;
	switch (SM.cmd) {
	case CSMHdr::CTCONNECT:
		//			SM1.Dv.success = FALSE;
		if (Connect(SM.sub[0])) {
			BOOL bCalib = DosUtil.GetLPUnit("CalibAfterReset", TRUE);
			if (SM.sub[2]) {
				bCalib = FALSE;
			}
			if (Reset(bCalib)) {
				if (SetDefault()) {
					if (Auto()) {
						BOOL bNV = DosUtil.GetLPUnit("NeedVac", FALSE);
						if (bNV) {
							ret = NeedVac();
						}
						else {
							ret = NoVac();
						}
						//						SM1.Dv.success = ret;
					}
				}
			}
			else {
				switch (int(SM.sub[1])) {
				case 1:
					pGDev->SetAlarm(CAlrmList::LP1RESETFAILURE, "LP1 fail to reset");
					break;
				case 2:
					pGDev->SetAlarm(CAlrmList::LP2RESETFAILURE, "LP2 fail to reset");
					break;
				}
			}
		}
		else {
			switch (int(SM.sub[1])) {
			case 1:
				pGDev->SetAlarm(CAlrmList::LP1COMMFAILURE, "LP1 fail to connect");
				break;
			case 2:
				pGDev->SetAlarm(CAlrmList::LP2COMMFAILURE, "LP2 fail to connect");
				break;
			}
		}
		Reply(SM, ret);
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
		Reply(SM, ret);
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
		Reply(SM, ret);
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
		Reply(SM, ret);
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
		Reply(SM, ret);
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
		Reply(SM, ret);
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
		Reply(SM, ret);
		break;
	case CSMHdr::CTOPEN:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Open());
		break;
	case CSMHdr::CTCLOSE:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Close());
		break;
	case CSMHdr::CTSETDEFAULT:
		Reply(SM, SetDefault());
		break;
	case CSMHdr::CTCLOSENORELEASE:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, CloseNoRelease());
		break;
	case CSMHdr::CTCALIB:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Calib());
		break;
	case CSMHdr::CTMAP:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Map());
		break;
	case CSMHdr::CTREGISTER:
		if (pCom) {
			pCom->hWnd = SM.HWin;
		}
		break;
	case CSMHdr::CTREGISTEREX:
		if (pCom) {
			if (pCom->hWndBak) {
				ASSERT(0);	// Programming error
			}
			pCom->hWndBak = pCom->hWnd;
			pCom->hWnd = SM.HWin;
		}
		break;
	case CSMHdr::CTUNREGISTEREX:
		if (pCom) {
			pCom->hWnd = pCom->hWndBak;
			pCom->hWndBak = 0;
		}
		break;
	case CSMHdr::CTSTATUS:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Status());
		break;
	case CSMHdr::CTSTATUSI:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Status());
		break;

	case CSMHdr::CTACCESSMODECHANGE:
	{
		int mode = (int)SM.sub[1];
		Reply(SM, AccessModeChange((CLPObj::ACCESSMODE)mode));
	}
	break;
	case CSMHdr::CTCLAMP:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Clamp());
		break;
	case CSMHdr::CTUNCLAMP:
		if (pCom) {
			if (SM.HWin) pCom->hWnd = SM.HWin;
		}
		Reply(SM, Unclamp());
		break;
		//		case CSMHdr::CTLEDLOADUNLOAD:
		//			Reply(SM, LEDLoadUnload((PrcsMgrServices::LPBSTATE)int(SM.sub[1])));
		//			break;
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
		Reply(SM, EnableHOAvbl());
		break;
	case CSMHdr::CTENABLEBUTTONREPORT:
		Reply(SM, EnableButtonReport());
		break;
	case CSMHdr::CTLEDALARMED:
		Reply(SM, LEDAlarm(SM.sub[0]));
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

void CComThread::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CComThread");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CComThread::Reply1(CString Caller, CSMOL& SM) {
	CSMClt ipc;
	if (ipc.Start1(Caller, sizeof(CSMOL))) {
		ipc.Send(&SM);
	}
}
BOOL CComThread::StartCom(short unit) {
	CString str;
	str = DosUtil.GetLPUnit("Type", "ASYST");
	str.MakeUpper();
	if (str == "TDK") {
		pCom = new CComTDK;
	}
	else {
		pCom = new CComFL300;
	}
	if (pCom) {
		pCom->unit = unit;
		return TRUE;
	}
	return FALSE;
}
CString CComThread::GetSlotMap() {
	if (pCom) {
		return pCom->SlotMap;
	}
	return "000000000000000000000000000";
}

BOOL CComThread::GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr) {
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

void CComThread::WaitTillStart() {
	while (!bStarted) {
		Sleep(100);
	}
}

BOOL CComThread::Connect(short port) {
	if (pCom) {
		return pCom->Connect(port);
	}
	return FALSE;
}

BOOL CComThread::Disconnect() {
	if (pCom) {
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

void CComThread::CheckComIn() {
	if (pCom) {
		pCom->CheckComIn();
	}
}

BOOL CComThread::NeedVac() {
	if (pCom) {
		return pCom->NeedVac();
	}
	return FALSE;
}

BOOL CComThread::Auto() {
	if (pCom) {
		return pCom->Auto();
	}
	return FALSE;
}

BOOL CComThread::Reset(BOOL bCalib) {
	if (pCom) {
		return pCom->Reset(bCalib);
	}
	return FALSE;
}

BOOL CComThread::Calib() {
	if (pCom) {
		return pCom->Calib();
	}
	return FALSE;
}

BOOL CComThread::NoVac() {
	if (pCom) {
		return pCom->NoVac();
	}
	return FALSE;
}

BOOL CComThread::Reply(CSMOL& SM, BOOL ret) {
	if (pCom) {
		return pCom->Reply(SM, ret);
	}
	return FALSE;
}

BOOL CComThread::Reply(CSMHdr& SM, BOOL ret) {
	//	if (pCom) {
	//		return pCom->Reply(SM, ret);
	//	}
	//	return FALSE;
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
	}
	return TRUE;
}

BOOL CComThread::Open() {
	if (pCom) {
		return pCom->Open();
	}
	return FALSE;
}

BOOL CComThread::Close() {
	if (pCom) {
		return pCom->Close();
	}
	return FALSE;
}
BOOL CComThread::SetDefault() {
	if (pCom) {
		return pCom->SetDefault();
	}
	return FALSE;
}

BOOL CComThread::CloseNoRelease() {
	if (pCom) {
		return pCom->CloseNoRelease();
	}
	return FALSE;
}
BOOL CComThread::Map() {
	if (pCom) {
		return pCom->Map();
	}
	return FALSE;
}

BOOL CComThread::Unclamp() {
	if (pCom) {
		return pCom->Unclamp();
	}
	return FALSE;
}

BOOL CComThread::Clamp() {
	if (pCom) {
		return pCom->Clamp();
	}
	return FALSE;
}

BOOL CComThread::Status() {
	if (pCom) {
		return pCom->Status();
	}
	return FALSE;
}

BOOL CComThread::AccessModeChange(CLPObj::ACCESSMODE mode) {
	if (pCom) {
		return pCom->AccessModeChange(mode);
	}
	return FALSE;
}

//BOOL CComThread::LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState)
//{
//	if (pCom) {
//		return pCom->LEDLoadUnload(LpState);
//	}
//	return FALSE;
//}

BOOL CComThread::ClearAllLED() {
	if (pCom) {
		return pCom->ClearAllLED();
	}
	return FALSE;
}

BOOL CComThread::LEDReserved(BOOL on) {
	if (pCom) {
		return pCom->LEDReserved(on);
	}
	return FALSE;
}

BOOL CComThread::LEDManual(BOOL on) {
	if (pCom) {
		return pCom->LEDManual(on);
	}
	return FALSE;
}

BOOL CComThread::EnableHOAvbl() {
	if (pCom) {
		return pCom->EnableHOAvbl();
	}
	return FALSE;
}

BOOL CComThread::EnableButtonReport() {
	if (pCom) {
		return pCom->EnableButtonReport();
	}
	return FALSE;
}

BOOL CComThread::LEDAlarm(BOOL on) {
	if (pCom) {
		return pCom->LEDAlarm(on);
	}
	return FALSE;
}

BOOL CComThread::AddToSMList(CSMHdr& SM) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	SMlist.push_back(SM);
	return TRUE;
}

BOOL CComThread::Abort() {
	if (pCom) {
		return pCom->Abort();
	}
	return FALSE;
}