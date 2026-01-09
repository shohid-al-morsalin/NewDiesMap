#include "stdafx.h"

#include "CFoup.h"
#include "CLPXChDat.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMHdr.h"
#include "IPC/SMCHdr.h"
#include "LPSvr.h"
#include "SRC/DOSUtil.h"

#include "LPMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLPMgr

CLPMgr::CLPMgr() {
	type = 1;
	SvrName = "LPMgr";
	for (int i = 0; i < 4; i++) {
		pLPSvr[i] = NULL;
	}
	// LYF Obsolete: Not use in multi-computer system [4/28/2006]
//	DosUtil.SetLPLight("NoFoup",DosUtil.GetLPLight("NoFoup","101"));
//	DosUtil.SetLPLight("FoupPresent",DosUtil.GetLPLight("FoupPresent","002"));
//	DosUtil.SetLPLight("ButtonPressed1",DosUtil.GetLPLight("ButtonPressed1","000"));
//	DosUtil.SetLPLight("Clampped",DosUtil.GetLPLight("Clampped","000"));
//	DosUtil.SetLPLight("Docking",DosUtil.GetLPLight("Docking","000"));
//	DosUtil.SetLPLight("Docked",DosUtil.GetLPLight("Docked","000"));
//	DosUtil.SetLPLight("Undocking",DosUtil.GetLPLight("Undocking","000"));
//	DosUtil.SetLPLight("Undocked",DosUtil.GetLPLight("Undocked","022"));
//	DosUtil.SetLPLight("ButtonPressed2",DosUtil.GetLPLight("ButtonPressed2","022"));
//	DosUtil.SetLPLight("Unclampped",DosUtil.GetLPLight("Unclamped","020"));
//	DosUtil.SetLPLight("Unclamping",DosUtil.GetLPLight("Unclamping","022"));
//	DosUtil.SetLPLight("Clamping",DosUtil.GetLPLight("Clamping","002"));

// 	DosUtil.WriteBCRMode(DosUtil.GetBCRMode(TRUE));

	DosUtil.SetLPUnit("PushButton", DosUtil.GetLPUnit("PushButton", 2));
	DosUtil.SetLPUnit("RTLLed", DosUtil.GetLPUnit("RTLLed", 1));
	DosUtil.SetLPUnit("RTULed", DosUtil.GetLPUnit("RTULed", 2));
	DosUtil.SetLPUnit("MANUALLed", DosUtil.GetLPUnit("MANUALLed", 3));
	DosUtil.SetLPUnit("AUTOLed", DosUtil.GetLPUnit("AUTOLed", 4));
	DosUtil.SetLPUnit("DOOROPENLed", DosUtil.GetLPUnit("DOOROPENLed", 0));
	DosUtil.SetLPUnit("ACCESSLed", DosUtil.GetLPUnit("ACCESSLed", 10));
	DosUtil.SetLPUnit("RESERVEDLed", DosUtil.GetLPUnit("RESERVEDLed", 5));
	DosUtil.SetLPUnit("ALARMEDLed", DosUtil.GetLPUnit("ALARMEDLed", 6));

	DosUtil.SetLPUnit("Type", DosUtil.GetLPUnit("Type", "ASYST"));
	DosUtil.SetLPUnit("NeedVac", DosUtil.GetLPUnit("NeedVac", FALSE));
}

CLPMgr::~CLPMgr() {}

BOOL CLPMgr::InitInstance() {
	return CSvrThread::InitInstance();
}

int CLPMgr::ExitInstance() {
	for (int i = 0; i < Var.nLoadPort; i++) {
		// stop Lp server here
		if (pLPSvr[i]) {
			CSMClt::StopSvrThread(pLPSvr[i]->SvrName, pLPSvr[i]->type);
			CLPSvr* p = (CLPSvr*)pLPSvr[i];
			pLPSvr[i] = NULL;
		}
	}
	return CSvrThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLPMgr, CSvrThread)
	//{{AFX_MSG_MAP(CLPMgr)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP();

/////////////////////////////////////////////////////////////////////////////
// CLPMgr message handlers
BOOL CLPMgr::ProcessCmd(CSMOL& SM) {
	BOOL ret;
	switch (SM.cmd) {
	case CSMOL::cmdPRCSMGRSERVICES:
		ret = PrcsMgrSvc(SM);
		break;
	case CSMOL::cmdCARRIERACTION:
		ret = CarrierActSvc(SM);
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CLPMgr::PrcsMgrSvc(CSMOL& SM) {
	if (!ValidUnit(SM)) {
		ASSERT(0);
		return TRUE;
	}
	CString msg;
	switch (SM.Pm.act) {
	case PrcsMgrServices::RESET:
	case PrcsMgrServices::CLEARALARM:
		msg.Format("Clear alarm on LP Unit #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::REGISTERWIN:
		msg.Format("Register LP Unit #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::REGISTERWINEX:
		msg.Format("RegisterEx LP Unit #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::UNREGISTERWINEX:
		msg.Format("UnRegisterEx LP Unit #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::MAP:
		msg.Format("Map Foup #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::LATCHFOUP:
		msg.Format("Latch Foup #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::UNLATCHFOUP:
		msg.Format("Unlatch Foup #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::OPENFOUP:
		msg.Format("Open foup %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::CLOSEFOUP:
		msg.Format("Closing foup %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::CLOSENORELEASE:
		msg.Format("Closing no release %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::LEDCLEARALL:
		msg.Format("LED clear all %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
		//		case PrcsMgrServices::LEDLOADUNLOAD:
		//			SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		//			break;
	case PrcsMgrServices::ENABLEHOAVBL:
		msg.Format("Enable HO_AVBL for port %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::ENABLEBUTTONREPORT:
		msg.Format("Enable Button Report for port %d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::STARTUNIT:
		if (!StartUnit(SM)) {
			Reply(SM, FALSE);
		}
		break;
	case PrcsMgrServices::GETLPISTATUS:
		if (SM.exe == CSMHdr::REQ) {
			ASSERT(0);
		}
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::GETLPSTATUS:
		if (SM.exe == CSMHdr::REQ) {
			ASSERT(0);
		}
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::ACCESSMODEAUTO:
		msg.Format("Change Access Mode Auto Unit %d", SM.Pm.unit);
		Log(msg, 3);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::ACCESSMODEMANUAL:
		msg.Format("Change Access Mode Manual Unit %d", SM.Pm.unit);
		Log(msg, 3);
		SendMsg(pLPSvr[SM.Pm.unit - 1], SM);
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CLPMgr::ValidUnit(CSMOL& SM) {
	if (SM.Pm.act == PrcsMgrServices::STARTUNIT) {
		return TRUE;
	}
	short unit;
	switch (SM.cmd) {
	case CSMOL::cmdPRCSMGRSERVICES:
		unit = SM.Pm.unit;
		break;
	case CSMOL::cmdCARRIERACTION:
		unit = SM.Ca.PortNo;
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	if (unit == 9) {
		unit = SM.Pm.nunit;
	}
	BOOL ret = IsValidUnit(unit);
	// Do not reply YFL [3/17/2003]
	//Reply(SM, ret);
	return ret;
}

BOOL CLPMgr::IsValidUnit(short unit) {
	short nLP = Var.nLoadPort;
	if (unit > 0 && unit <= nLP) {
		if (pLPSvr[unit - 1]) {
			return TRUE;
		}
	}
	ASSERT(0);
	return FALSE;
}

BOOL CLPMgr::SendMsg(CLPSvr* pSvr, CSMOL& SM) {
	if (pSvr) {
		CSMClt ipc;
		if (ipc.Start1(pSvr->SvrName, sizeof(CSMOL))) {
			if (ipc.Send(&SM)) {
				return TRUE;
			}
		}
	}
	ASSERT(0);
	Reply(SM, FALSE);
	return FALSE;
}

BOOL CLPMgr::CarrierActSvc(CSMOL& SM) {
	if (!ValidUnit(SM)) {
		ASSERT(0);
		return FALSE;
	}
	CString msg;
	switch (SM.Ca.act) {
	case CarrierAction::SETCARRIERARRIVED:
		msg.Format("Carrier arrived on LP Unit #%d", SM.Ca.PortNo);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Ca.PortNo - 1], SM);
		break;
	case CarrierAction::CLEARCARRIERARRIVED:
		msg.Format("Carrier depart from LP Unit #%d", SM.Ca.PortNo);
		Log(msg, 10);
		SendMsg(pLPSvr[SM.Ca.PortNo - 1], SM);
		break;
	}
	return TRUE;
}

void CLPMgr::CleanUp() {}

CLPSvr* CLPMgr::GetLPSvr(short unit) {
	if (IsValidUnit(unit)) {
		return pLPSvr[unit - 1];
	}
	return NULL;
}

void CLPMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CLPMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CLPMgr::Reply(CSMOL& SM, BOOL ret) {
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

BOOL CLPMgr::Reply(CSMHdr& SM, BOOL ret) {
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

BOOL CLPMgr::StartUnit(CSMOL& SM) {
	CString msg;
	CLPSvr* pSvr;
	msg.Format("Starting LP unit %d", SM.Pm.unit);
	Log(msg, 10);
	pSvr = pLPSvr[SM.Pm.unit - 1];
	if (pSvr) {
		msg.Format("LP Unit %d already started", SM.Pm.unit);
		Log(msg, 2);
		//Reply(SM, TRUE);
		return TRUE;
	}
	pLPSvr[SM.Pm.unit - 1] = new CLPSvr;
	pSvr = pLPSvr[SM.Pm.unit - 1];
	if (!pSvr) {
		msg.Format("Unit (%d) alloc error", SM.Pm.unit);
		Log(msg, 2);
		//Reply(SM, FALSE);
		return FALSE;
	}
	pSvr->Unit = SM.Pm.unit;
	pSvr->Port = SM.Pm.port;
	if (CSMClt::StartSvrThread(pSvr)) {
		while (!pSvr->bStarted) {
			Sleep(10);
		}
		RPLY rpy;
		BOOL bNoClose = FALSE;
		if (strcmp(SM.Pm.text, "NOCLOSE") == 0) {
			bNoClose = TRUE;
		}
		rpy.Set(SM.HWin, SM.Ack, SM.Nak);
		if (!pSvr->StartUnit(rpy, bNoClose)) {
			msg.Format("Unit (%d) fail to start", SM.Pm.unit);
			Log(msg, 0);
			//Reply(SM, FALSE);
			return FALSE;
		}
		return TRUE;
	}
	msg.Format("Unit (%d) thread fail to instantiate", SM.Pm.unit);
	Log(msg, 0);
	//Reply(SM, FALSE);
	return FALSE;
}

BOOL CLPMgr::GetLPStatus(CLPXChDat& Xch) {
	CLPSvr* p = GetLPSvr(Xch.ID);
	if (p) {
		return p->GetLPStatus(Xch.bRdy, Xch.bPst, Xch.bErr);
	}
	return FALSE;
}