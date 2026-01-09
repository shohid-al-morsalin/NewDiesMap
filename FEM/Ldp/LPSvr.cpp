// LPSvr.cpp : implementation file
//

#include "stdafx.h"
#include "AsystFoup.h"
#include "IPC/SMHdr.h"
#include "IPC/SMSvr.h"
#include "128/GEM/OLnClt.h"
#include "SRC/DOSUtil.h"
#include "GlobalDeviceContainer.h"

#include "LPSvr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLPSvr

CLPSvr::CLPSvr() {
	type = 1;
	SvrName = "";

	Unit = 0;
	Port = 0;

	bSimu = TRUE;
	bComThread = FALSE;

	pLP = NULL;
	pCIDR = NULL;
}

CLPSvr::~CLPSvr() {}

BOOL CLPSvr::InitInstance() {
	CString msg;
	if (!Unit) {
		msg.Format("Unit (%d) out of range", Unit);
		Log(msg, 2);
		ASSERT(Unit);
	}

	bSimu = DosUtil.GetLPUnitSimu(Unit);
	if (!Port) {
		msg.Format("Port (%d) out of range", Port);
		Log(msg, 2);
		bSimu = TRUE;
	}

	SvrName.Format("LPSvr%d", Unit);

	if (bSimu) {
		pLP = new CFoup;
		msg.Format("LP%d is in simulation", Unit);
		Log(msg);
	}
	else {
		pLP = new CAsystFoup;
	}
	if (!pLP) {
		ASSERT(0);
		return TRUE;
	}

	pLP->Caller = SvrName;
	bComThread = pLP->StartComThread(Unit);

	return TRUE;
}

int CLPSvr::ExitInstance() {
	if (pLP) delete pLP;
	pLP = NULL;
	return CSvrThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLPSvr, CSvrThread)
	//{{AFX_MSG_MAP(CLPSvr)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLPSvr message handlers

BOOL CLPSvr::ProcessCmd(CSMOL& SM) {
	switch (SM.cmd) {
	case CSMOL::cmdDEVICE:
		ipc->Done();
		ExecDeviceCmd(SM);
		return FALSE;
	case CSMOL::cmdPRCSMGRSERVICES:
		return ExecPrcsMgrCmd(SM);
		break;
	case CSMOL::cmdCARRIERACTION:
		return ExecCarrActCmd(SM);
		break;
	}
	return TRUE;
}

BOOL CLPSvr::ExecPrcsMgrCmd(CSMOL& SM) {
	if (!pLP) {
		ASSERT(0);
		Reply(SM, FALSE);
		return TRUE;
	}

	CSMHdr SM1;
	CString str;
	CString sect;
	CString winini;

	switch (SM.Pm.act) {
	case PrcsMgrServices::CLEARALARM:
	{
		SM1.cmd = CSMHdr::CTRESET;
		SetReply(SM1, SM);
		RPLY rpy;
		pLP->Reset(rpy);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
	}
	break;
	case PrcsMgrServices::REGISTERWIN:
		SM1.cmd = CSMHdr::CTREGISTER;
		SetReply(SM1, SM);
		pLP->Register(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::REGISTERWINEX:
		SM1.cmd = CSMHdr::CTREGISTEREX;
		SetReply(SM1, SM);
		pLP->RegisterEx(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::UNREGISTERWINEX:
		SM1.cmd = CSMHdr::CTUNREGISTEREX;
		SetReply(SM1, SM);
		pLP->UnRegisterEx(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::MAP:
		SM1.cmd = CSMHdr::CTMAP;
		SetReply(SM1, SM);
		pLP->Map(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::OPENFOUP:
		SM1.cmd = CSMHdr::CTOPEN;
		SetReply(SM1, SM);
		if (!pLP->Open(SM1)) {
			Reply(SM, FALSE);
		}
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::CLOSEFOUP:
		SM1.cmd = CSMHdr::CTCLOSE;
		SetReply(SM1, SM);
		if (!pLP->Close(SM1)) {
			Reply(SM, FALSE);
		}
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::AUTO:
	{
		SM1.cmd = CSMHdr::CTAUTO;
		SetReply(SM1, SM);
		RPLY rpy;
		pLP->Auto(rpy);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
	}
	break;
	case PrcsMgrServices::SETDEFAULT:
		SM1.cmd = CSMHdr::CTSETDEFAULT;
		SetReply(SM1, SM);
		pLP->SetDefault(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::NOVACUUM:
	{
		SM1.cmd = CSMHdr::CTNOVAC;
		SetReply(SM1, SM);
		RPLY rpy;
		pLP->NoVac(rpy);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
	}
	break;
	case PrcsMgrServices::NEEDVACUUM:
	{
		SM1.cmd = CSMHdr::CTNEEDVAC;
		SetReply(SM1, SM);
		RPLY rpy;
		pLP->NeedVac(rpy);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
	}
	break;
	case PrcsMgrServices::PUSHBUTTON2:
		SM1.cmd = CSMHdr::PUSHBUTTON2;
		SetReply(SM1, SM);
		pLP->PushButton2(SM1);
		break;
	case PrcsMgrServices::GETLPSTATUS:
		SM1.cmd = CSMHdr::CTSTATUS;
		SetReply(SM1, SM);
		pLP->Status(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::GETLPISTATUS:
		SM1.cmd = CSMHdr::CTSTATUSI;
		SetReply(SM1, SM);
		pLP->Status(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::ACCESSMODEAUTO:
		SM1.cmd = CSMHdr::CTACCESSMODECHANGE;
		SM1.sub[1] = CLPObj::AUTO;
		SetReply(SM1, SM);
		pLP->SetE84AccessMode(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::ACCESSMODEMANUAL:
		SM1.cmd = CSMHdr::CTACCESSMODECHANGE;
		SM1.sub[1] = CLPObj::MANUAL;
		SetReply(SM1, SM);
		pLP->SetE84AccessMode(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::LATCHFOUP:
		SM1.cmd = CSMHdr::CTCLAMP;
		SetReply(SM1, SM);
		if (!pLP->Clamp(SM1)) {
			Reply(SM, FALSE);
		}
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::CLOSENORELEASE:
		SM1.cmd = CSMHdr::CTCLOSENORELEASE;
		SetReply(SM1, SM);
		if (!pLP->CloseNoRelease(SM1)) {
			Reply(SM, FALSE);
		}
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::UNLATCHFOUP:
		SM1.cmd = CSMHdr::CTUNCLAMP;
		SetReply(SM1, SM);
		pLP->Unclamp(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
		//	case PrcsMgrServices::LEDLOADUNLOAD:
		//		SM1.cmd = CSMHdr::CTLEDLOADUNLOAD;
		//		SM1.sub[0]=SM.Pm.port;
		//		SM1.sub[1]=float(SM.Pm.LpBState);
		//		SetReply(SM1,SM);
		//		pLP->LEDLoadUnload(SM1);
		//		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		//		break;
	case PrcsMgrServices::ENABLEHOAVBL:
		SM1.cmd = CSMHdr::CTENABLEHOAVBL;
		SM1.sub[0] = SM.Pm.port;
		SetReply(SM1, SM);
		pLP->EnableHOAvbl(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::ENABLEBUTTONREPORT:
		SM1.cmd = CSMHdr::CTENABLEBUTTONREPORT;
		SM1.sub[0] = SM.Pm.port;
		SetReply(SM1, SM);
		pLP->EnableButtonReport(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::RESET:
	{
		RPLY rpy;
		pLP->Reset(rpy);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
	}
	break;
	}
	return TRUE;
}

BOOL CLPSvr::Reply(CSMOL& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CLPSvr::CheckAlarm(CString msg) {
	if (msg.Find("ALARM") != -1) {
		switch (Unit) {
		case 1:
			pGDev->SetAlarm(CAlrmList::LP1ALARMED, "LP1 is in alarmed condition");
			break;
		case 2:
			pGDev->SetAlarm(CAlrmList::LP2ALARMED, "LP2 is in alarmed condition");
			break;
		default:
			ASSERT(0);
			break;
		}
		Log("LP ALARM set", 1);
		return TRUE;
	}
	return FALSE;
}

void CLPSvr::SetReply(CSMHdr& SM1, CSMOL& SM) {
	SM1.Ack = SM.Ack;
	SM1.Nak = SM.Nak;
	SM1.HWin = SM.HWin;
}

BOOL CLPSvr::ExecCarrActCmd(CSMOL& SM) {
	// Simulation use only [5/16/2002]
	switch (SM.Ca.act) {
	case CarrierAction::SETCARRIERARRIVED:
		pLP->bInPlace = TRUE;
		break;
	case CarrierAction::CLEARCARRIERARRIVED:
		pLP->bInPlace = FALSE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CLPSvr::LedClearAll() {
	if (!pLP) {
		ASSERT(0);
		return FALSE;
	}
	return pLP->LedClearAll();
}

BOOL CLPSvr::SetE84AccessMode(CLPObj::ACCESSMODE mode) {
	if (!pLP) {
		ASSERT(0);
		return FALSE;
	}
	CSMHdr SM;
	SM.cmd = CSMHdr::CTACCESSMODECHANGE;
	SM.sub[1] = (float)mode;
	return pLP->SetE84AccessMode(SM);
}

BOOL CLPSvr::LedReserved(BOOL bMode) {
	if (!pLP) {
		ASSERT(0);
		return FALSE;
	}
	CSMHdr SM;
	SM.cmd = CSMHdr::CTLEDRESERVED;
	SM.sub[0] = float(bMode);
	return pLP->LedReserved(SM);
}

BOOL CLPSvr::LedManual(BOOL bMode) {
	if (!pLP) {
		ASSERT(0);
		return FALSE;
	}
	CSMHdr SM;
	SM.cmd = CSMHdr::CTLEDMANUAL;
	SM.sub[0] = float(bMode);
	return pLP->LedManual(SM);
}

void CLPSvr::SetInPlace(BOOL bInP) {
	if (!pLP) {
		ASSERT(0);
		return;
	}
	pLP->SetInP(bInP);
}

BOOL CLPSvr::GetLPStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr) {
	if (!pLP) {
		ASSERT(0);
		bRdy = FALSE;
		bInP = FALSE;
		bAlr = FALSE;
		return FALSE;
	}
	return pLP->GetStatus(bRdy, bInP, bAlr);
}

CString CLPSvr::GetSlotmap() {
	if (!pLP) {
		ASSERT(0);
		return "";
	}
	return pLP->GetSM();
}

void CLPSvr::CleanUp() {}

void CLPSvr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CLPSvr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CLPSvr::ExecDeviceCmd(CSMOL& SM) {
	switch (SM.Dv.act) {
	case DeviceCmd::CONNECT:
		if (SM.Dv.success) {
			Log("LP started", 10);
			Reply(SM, TRUE);
		}
		else {
			Log("LP start fail", 1);
			Reply(SM, FALSE);
		}
		break;

	case DeviceCmd::DISCONNECT:
		if (SM.Dv.success) {
			Log("Disconnect OK", 10);
		}
		else {
			Log("Disconnect failed", 1);
		}
		break;
	case DeviceCmd::RESET:
		if (SM.Dv.success) {
			Log("Reset OK", 10);
			RPLY rpy;
			rpy.HWin = SM.HWin;
			rpy.Ack = SM.Ack;
			rpy.Nak = SM.Nak;
			pLP->Auto(rpy);
		}
		else {
			Log("Reset failed", 1);
		}
		break;
	case DeviceCmd::AUTO:
		if (SM.Dv.success) {
			Log("Auto OK", 10);
			BOOL bNV = DosUtil.GetLPUnit("NeedVac", FALSE);
			RPLY rpy;
			rpy.HWin = SM.HWin;
			rpy.Ack = SM.Ack;
			rpy.Nak = SM.Nak;
			if (bNV) {
				pLP->NeedVac(rpy);
			}
			else {
				pLP->NoVac(rpy);
			}
		}
		else {
			Log("Auto failed", 1);
		}
		break;
	case DeviceCmd::NOVAC:
		if (SM.Dv.success) {
			Log("LP started", 10);
			Reply(SM, TRUE);
		}
		else {
			Log("LP start fail", 1);
			Reply(SM, FALSE);
		}
		break;
	case DeviceCmd::NEEDVAC:
		if (SM.Dv.success) {
			Log("LP started", 10);
			Reply(SM, TRUE);
		}
		else {
			Log("LP start fail", 1);
			Reply(SM, FALSE);
		}
		break;
	}
}

// Called from other thread
BOOL CLPSvr::StartUnit(RPLY rpy, BOOL bNoClose) {
	if (pLP) {
		pLP->bNoClose = bNoClose;
		if (pLP->Connect(rpy, Unit, Port))	// This function is guarantee o success [3/16/2012 Administrator]
		{
			return TRUE;
		}
	}
	return FALSE;
}

//BOOL CLPSvr::LedAlarmed(BOOL bMode)
//{
//	if (!pLP) {
//		ASSERT(0);
//		return FALSE;
//	}
//	CSMHdr SM;
//	SM.cmd = CSMHdr::CTLEDALARMED;
//	SM.sub[0] = float(bMode);
//	return pLP->LedAlarmed(SM);
//}