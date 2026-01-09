#include "stdafx.h"
#include "RbtMgr.h"
#include "BrkUnit.h"
#include "IPC/SMSvr.h"
#include "SRC/DOSUtil.h"
#include "LogosolBrkUnit.h"
#include "GlobalDeviceContainer.h"

/////////////////////////////////////////////////////////////////////////////
// CRbtMgr
//
// Service Provided via SM.Pm (SMOL)
// STARTUNIT
//	SM.Pm.port:
// HOMEALGN
// HOMERBT
// XFER
//	SM.Pm.text
// WAFERCHECK
// TINYEYECHECK
// WAFERONARM
// New Commands
//	LP2HC, HC2LP, LP2SCL, SCL2SCN, SCN2LP
// port:LP, unit:Slot

CRbtMgr::CRbtMgr() {
	type = 1;
	SvrName = "RbtMgr";
	pRbtSvr = NULL;
	bIdle = TRUE;
	bAlarmed = FALSE;
	RbtSvrState = NOTSTARTED;

	CString str = DosUtil.GetRbtType();
	DosUtil.SetRbtType(str);

	str.MakeUpper();
	if (str == "BROOKS") {
		RbtType = BROOKS;
	}
	// [Tushar 21082024
	else if (str == "LOGOSOLBRK") {
		RbtType = LOGOSOLBRK;
	}
	//]
	else {
		RbtType = SIMU;
	}
}

CRbtMgr::~CRbtMgr() {}

BOOL CRbtMgr::ProcessCmd(CSMOL& SM) {
	switch (SM.cmd) {
	case CSMOL::cmdPRCSMGRSERVICES:
		return ExecPrcsCmd(SM);
		break;
	}
	return TRUE;
}

BOOL CRbtMgr::ExecPrcsCmd(CSMOL& SM) {//t123
	BOOL ret;
	CString str;
	switch (SM.Pm.act) {
	case PrcsMgrServices::STARTUNIT:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			Reply(SM, FALSE);
			return TRUE;
		}
		ipc->Done();
		bIdle = FALSE;
		if (SM.Pm.port == 0) {
			RbtType = SIMU;
		}
		switch (RbtType) {
		case BROOKS:
			pRbtSvr = new CBrkUnit;
			break;
		case LOGOSOLBRK:
			pRbtSvr = new CLogosolBrkUnit; //Tushar 21082024
			break;
		default:
			pRbtSvr = new CRbtBase;
			break;
		}
		if (!pRbtSvr) {
			Reply(SM, FALSE);
			return FALSE;
		}
		ret = pRbtSvr->Start(SM.Pm.port);
		Log(pRbtSvr->GetLastError(), 3);
		if (ret) {
			// set communication response to MON [6/3/2020]
			ret = pRbtSvr->SetComMon();
			if (ret) {
				Log(pRbtSvr->GetLastError(), 3);
			}
			else {
				pGDev->SetAlarm(CAlrmList::ROBOTHOMEFAILURE, pRbtSvr->GetLastError());
				Reply(SM, ret);
				RbtSvrState = STARTFAIL;
				return FALSE;
			}
			ret = pRbtSvr->HomeRobot(1);
			if (ret) {
				Log(pRbtSvr->GetLastError(), 3);
			}
			else {
				pGDev->SetAlarm(CAlrmList::ROBOTHOMEFAILURE, pRbtSvr->GetLastError());
				Reply(SM, ret);
				RbtSvrState = STARTFAIL;
				return FALSE;
			}
		}
		else {
			pGDev->SetAlarm(CAlrmList::ROBOTCOMMFAILURE);
			Reply(SM, ret);
			RbtSvrState = STARTFAIL;
			return FALSE;
		}
		if (pRbtSvr->IsWaferOnArm()) {
			pGDev->SetAlarm(CAlrmList::WAFERONARM, "Wafer on robot arm! Recovery needed");
			ret = FALSE;
		}
		Reply(SM, ret);
		bIdle = TRUE;
		RbtSvrState = STARTOK;
		return FALSE;
		break;
	case PrcsMgrServices::HOMEALGNER:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ipc->Done();
		bIdle = FALSE;
		ret = pRbtSvr->HomeAlign(SM.Pm.Ret);
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		bIdle = TRUE;
		return FALSE;
		break;
	case PrcsMgrServices::HOMEROBOT:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ipc->Done();
		bIdle = FALSE;
		ret = pRbtSvr->HomeRobot(SM.Pm.Ret);
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		bIdle = TRUE;
		return FALSE;
		break;
	case PrcsMgrServices::XFER:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ipc->Done();
		bIdle = FALSE;
		ret = pRbtSvr->Xfer(SM.Pm.text);
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		bIdle = TRUE;
		return FALSE;
		break;
	case PrcsMgrServices::XFER1:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ipc->Done();
		ret = FALSE;
		bIdle = FALSE;
		str = SM.Pm.text;
		{
			char ch;
			char dmy[64];
			short Stn1, Stn2, Slt1, Slt2;
			sscanf(SM.Pm.text, "%s %s %c %hd %s %hd %hd %s %hd", dmy, dmy, &ch, &Stn1, dmy, &Slt1, &Stn2, dmy, &Slt2);
			if (str.Find("XFER ARM A") != -1) {
				if (Stn2 == 1) {
					str += " ALGN";
				}
			}
			if (str.Find("HOME") != -1) {
				ret = pRbtSvr->HomeRobot(SM.Pm.Ret);
			}
			else {
				STSSource((CMsgItem::STSLOC)SM.Pm.STSFrom, Stn1);
				ret = pRbtSvr->Xfer(str.GetBuffer(0));
				if (ret) {
					STSDestination((CMsgItem::STSLOC)SM.Pm.STSTo, Stn2);
				}
			}
		}
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		bIdle = TRUE;
		return FALSE;
		break;
	case PrcsMgrServices::WAFERCHECK:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ret = pRbtSvr->WaferPresentCheck();
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		break;
	case PrcsMgrServices::TINYEYECHECK:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ret = pRbtSvr->GetTinyEye();
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		break;
	case PrcsMgrServices::WAFERONARM:
		if (SM.exe == CSMOL::REQ) {
			ASSERT(0);
		}
		if (!bIdle) {
			// 				ASSERT(0);
			Reply(SM, FALSE);
			return TRUE;
		}
		ret = pRbtSvr->IsWaferOnArm();
		Log(pRbtSvr->GetLastError(), 3);
		Reply(SM, ret);
		break;
	}
	return TRUE;
}

BOOL CRbtMgr::Reply(CSMOL& SM, BOOL ret) {
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

void CRbtMgr::CleanUp() {
	if (pRbtSvr) {
		pRbtSvr->Stop();
		delete pRbtSvr;
	}
}

void CRbtMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CRbtMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CRbtMgr::ExitInstance() {
	//TRACE("CRbtMgr terminated\n");
	return CSvrThread::ExitInstance();
}

// Note: called from other thread, use with care
BOOL CRbtMgr::IsWaferOnArm(short arm) {
	if (pRbtSvr) {
		return pRbtSvr->IsWaferOnArm(arm);
	}
	return TRUE; // return true on purpose, false positive for safety reason
}

BOOL CRbtMgr::IsWaferOnArm() {
	if (pRbtSvr) {
		return pRbtSvr->IsWaferOnArm();
	}
	return TRUE; // return true on purpose, false positive for safety reason
}

BOOL CRbtMgr::IsWaferOnAligner(short arm) {
	if (pRbtSvr) {
		return pRbtSvr->IsWaferOnAlgn(arm);
	}
	return TRUE; // return true on purpose, false positive for safety reason
}

BOOL CRbtMgr::IsWaferOnAligner() {
	if (pRbtSvr) {
		return pRbtSvr->IsWaferOnAlgn();
	}
	return TRUE; // return true on purpose, false positive for safety reason
}

void CRbtMgr::STSSource(CMsgItem::STSLOC STSLoc, short Stn) {
	switch (STSLoc) {
	case CMsgItem::ALGNR1:
		break;
	case CMsgItem::ALGNR2:
		break;
	case CMsgItem::CASIN1:
		break;
	case CMsgItem::CASIN2:
		break;
	case CMsgItem::CASOUT1:
		break;
	case CMsgItem::CASOUT2:
		break;
	case CMsgItem::CHMBR1:
		break;
	case CMsgItem::CHMBR2:
		break;
	case CMsgItem::SCALE:
		break;
	default:
		//		ASSERT(0);
		break;
	}
}

void CRbtMgr::STSDestination(CMsgItem::STSLOC STSLoc, short Stn) {
	switch (STSLoc) {
	case CMsgItem::ALGNR1:
		break;
	case CMsgItem::ALGNR2:
		break;
	case CMsgItem::CASIN1:
		break;
	case CMsgItem::CASIN2:
		break;
	case CMsgItem::CASOUT1:
		break;
	case CMsgItem::CASOUT2:
		break;
	case CMsgItem::CHMBR1:
		break;
	case CMsgItem::CHMBR2:
		break;
	case CMsgItem::SCALE:
		break;
	default:
		//		ASSERT(0);
		break;
	}
}

// LYF Function do not apply to Dual end effector version [5/4/2006]
int CRbtMgr::GetDIOIN() {
	if (pRbtSvr) {
		return pRbtSvr->GetDIOIN();
	}
	return 0xFF; // return 0xFF on purpose, false positive for safety reason
}

BOOL CRbtMgr::IsIdle() {
	return bIdle;
}