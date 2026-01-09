#include "stdafx.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "413App.h"
#include "CLPXChDat.h"
#include "FEM/Rbt/RbtBase.h"
#include "FEM/Rbt/RbtMgr.h"
#include "IPC/SMCHdr.h"
#include "LOG/LogSvr.h"
#include "LTW/LTMgrE95.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "ONLINE/MFC/getpw.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "OnlineMgr.h"
#include "SRC/DOSUtil.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "SystemStatusDlg.h"

#include "DrLockMgr.h"

#include "GlobalDeviceContainer.h"

CGlobalDeviceContainer* pGDev = NULL;

CGlobalDeviceContainer::CGlobalDeviceContainer() {
	SystemModel = FSM413C2C;

	pLPMgr = NULL;
	pIDRMgr = NULL;
	pRbtMgr = NULL;
	pLogSvr = NULL;
	pLTMgr = NULL;
	pDrLockMgr = NULL;
	pGEMObjs = NULL;
	pDataBnk = NULL;

	pOLnClt = NULL;
	pOLnMgr = NULL;

	m_SystemStatusDlg = NULL;

	username = "";

	InterPaneMessage = "";

	MgrStatus.LogSvr = StartLogSvr(); // Must be first one to start so the rest can write to log

	LoadSystemOption();
	SaveSystemOption();
}

CGlobalDeviceContainer::~CGlobalDeviceContainer() {
	if (pRbtMgr || pIDRMgr || pLPMgr || pLTMgr || pDrLockMgr) {
		ASSERT(0);
	}
	if (pOLnClt) {
		delete pOLnClt;
	}
	if (pOLnMgr) {
		pOLnMgr = NULL;
	}
	if (pDataBnk) {
		delete pDataBnk;
	}
	if (pGEMObjs) {
		delete pGEMObjs;
	}

	switch (SystemModel) {
	case FSM413C2C:
	case FSM413MOT:
	case FSM413SA:
		if (p413App) {
			p413App->StopSystem();
			delete p413App;
		}
		break;
	}
	MgrStatus.LogSvr = StopLogSvr();
}

void CGlobalDeviceContainer::Initialize() {
	p413App = new C413App;

	if (!pGEMObjs) {
		pGEMObjs = new CObjectsContainer;
	}
	if (!pDataBnk) {
		// must start after pGEMObjs [7/30/2010 XPMUser]
		pDataBnk = new CDataBank;
	}
	if (!pOLnClt) {
		pOLnClt = new COLnClt;
	}

	if (p413App && p413App->Global.Confg.bSecsMod) {
		// HSMS parameters must be valid before this object can be instantiated [8/1/2010 XPMUser]
		pOLnMgr = new COnlineMgr;
		if (pOLnMgr) {
			MgrStatus.OLnMgr = CMgrStatus::STARTOK;
		}
	}
}

CMgrStatus::STATUSFLAG CGlobalDeviceContainer::StartLogSvr() {
	if (pLogSvr) {
		ASSERT(0);
		return CMgrStatus::ALREADYINSTANTIATED;
	}
	else {
		pLogSvr = new CLogSvr;
		if (!pLogSvr) {
			ASSERT(0);
			return CMgrStatus::OBJECTCREATEFAIL;
		}
		if (!CSMClt::StartSvrThread(pLogSvr)) {
			ASSERT(0);
			return CMgrStatus::STARTTHREADFAIL;
		}
		while (!pLogSvr->bStarted) {
			Sleep(10);
		}
		return CMgrStatus::STARTOK;
	}
	// Leaked
	ASSERT(0);
	return CMgrStatus::FUNCTIONLEAK;
}

CMgrStatus::STATUSFLAG CGlobalDeviceContainer::StopLogSvr() {
	if (pLogSvr) {
		if (CSMClt::StopSvrThread(pLogSvr->SvrName, pLogSvr->type)) {
			pLogSvr = NULL;
			return CMgrStatus::STOPOK;
		}
	}
	return CMgrStatus::STOPTHREADFAIL;
}

void CGlobalDeviceContainer::Log(CString msg, short lvl) {
	LogFile.Log(msg, lvl, "CGlobalDeviceContainer");
	if (lvl <= 1) {
		LogFile.LogLocal(msg);
	}
}

BOOL CGlobalDeviceContainer::RegisterWinWithLPMgr(HWND hWnd, short ID) {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::REGISTERWIN;
	SM.Pm.unit = ID;
	SM.HWin = hWnd;
	SM.Ack = UM_ACKNOLEDGE;
	SM.Nak = UM_NACKNOLEDGE;
	CSMClt ipc;
	if (ipc.Start1("LPMgr", sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			CString str;
			str.Format("LP%d registered", ID);
			Log(str);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::RegisterWinWithLPMgrEx(HWND hWnd, short ID) {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::REGISTERWINEX;
	SM.Pm.unit = ID;
	SM.HWin = hWnd;
	SM.Ack = UM_ACKNOLEDGE;
	SM.Nak = UM_NACKNOLEDGE;
	CSMClt ipc;
	if (ipc.Start1("LPMgr", sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			Log("RegisterEx send");
			return TRUE;
		}
	}
	return FALSE;
}

void CGlobalDeviceContainer::UnRegisterWinWithLPMgrEx(HWND hWnd, short ID) {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::UNREGISTERWINEX;
	SM.Pm.unit = ID;
	SM.HWin = hWnd;
	SM.Ack = UM_ACKNOLEDGE;
	SM.Nak = UM_NACKNOLEDGE;
	CSMClt ipc;
	if (ipc.Start1("LPMgr", sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			Log("UnregisterEx send");
		}
	}
}

BOOL CGlobalDeviceContainer::RegisterWinWithIDRMgr(HWND hWnd, short ID) {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::REGISTERWIN;
	SM.Pm.unit = ID;
	SM.HWin = hWnd;
	SM.Ack = UM_ACKNOLEDGE;
	SM.Nak = UM_NACKNOLEDGE;
	CSMClt ipc;
	if (ipc.Start1("CIDRMgr", sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			CString str;
			str.Format("IDR%d registered", ID);
			Log(str);
			return TRUE;
		}
	}
	return FALSE;
}

CMgrStatus::STATUSFLAG CGlobalDeviceContainer::StartE95BemSvr() {
	return CMgrStatus::STARTOK;
}

CMgrStatus::STATUSFLAG CGlobalDeviceContainer::StopE95BemSvr() {
	return CMgrStatus::STOPOK;
}

void CGlobalDeviceContainer::SetMessage(CString msg) {
	InterPaneMessage = msg;
}

CString CGlobalDeviceContainer::GetMessage() {
	return InterPaneMessage;
}

BOOL CGlobalDeviceContainer::RegisterWithLogSvr(HWND hWnd) {
	if (pLogSvr) {
		pLogSvr->hWnd = hWnd;
		return TRUE;
	}
	return FALSE;
}

void CGlobalDeviceContainer::TriggerAlarm(short ALID) {}

void CGlobalDeviceContainer::SetLPState(short unit, PrcsMgrServices::LPBSTATE state) {
	EqpState.SetLPState(unit, state);
	pLTMgr->SetLight();

	if (pGEMObjs) {
		switch (state) {
		case PrcsMgrServices::NOFOUP:
			// Ready to load
			pGEMObjs->SetLPOccupancy(unit, CLPObj::CARRIERNOTPRESENT);
			pGEMObjs->SetLPTransferStatus(unit, CLPObj::READYTOLOAD);
			break;
		case PrcsMgrServices::FOUPPRESENT:
			// Transfer blocked
			pGEMObjs->SetLPOccupancy(unit, CLPObj::CARRIERPRESENT);
			pGEMObjs->SetLPTransferStatus(unit, CLPObj::TRANSFERBLOCKED);
			break;
		case PrcsMgrServices::FOUPERROR:
			pGEMObjs->SetLPTransferStatus(unit, CLPObj::OUTOFSERVICE);
			pGEMObjs->SetLPOccupancy(unit, CLPObj::ocNOSTATE);
			break;
		case PrcsMgrServices::CLAMPPING:
		case PrcsMgrServices::CLAMPPED:
		case PrcsMgrServices::CLAMPFAILED:
			break;
		case PrcsMgrServices::DOCKING:
		case PrcsMgrServices::DOCKED:
		case PrcsMgrServices::DOCKFAILED:
			break;
		case PrcsMgrServices::OPENING:
		case PrcsMgrServices::OPENED:
		case PrcsMgrServices::OPENFAILED:
			break;
		case PrcsMgrServices::UNDOCKING:
			break;
		case PrcsMgrServices::UNDOCKED:
			break;
		case PrcsMgrServices::UNDOCKFAILED:
			break;
		case PrcsMgrServices::UNCLAMPPING:
			break;
		case PrcsMgrServices::UNCLAMPPED:
			pGEMObjs->SetLPTransferStatus(unit, CLPObj::READYTOUNLOAD);
			break;
		case PrcsMgrServices::UNCLAMPFAILED:
			break;
		case PrcsMgrServices::BUTTONPRESSED:
			break;
		default:
			Log("Unhandled SetLPState", 1);
			break;
		}
	}
}

PrcsMgrServices::LPBSTATE CGlobalDeviceContainer::GetLPState(short unit) {
	return EqpState.GetLPState(unit);
}

BOOL CGlobalDeviceContainer::GetLPState(CLPXChDat& Xch) {
	CLPObj Lp;
	if (pGEMObjs->GetLpSM(Xch.ID, Lp)) {
		Xch.m_AccessingStatus = Lp.GetOccupencyStr();
		Xch.m_Association = Lp.GetAssociationStr();
		Xch.m_AccessMode = Lp.GetAccessModeStr();
		Xch.m_Reservation = Lp.GetReservationStr();
		Xch.m_TransferStatus = Lp.GetTransferStatusStr();
	}
	return FALSE;
}

void CGlobalDeviceContainer::ContinueDoorMonitor() {
	if (pDrLockMgr) pDrLockMgr->Continue();
}

void CGlobalDeviceContainer::PauseDoorMonitor() {
	if (pDrLockMgr) pDrLockMgr->Pause();
}

CString CGlobalDeviceContainer::GetIDRStatusStr(short unit) {
	CIDRObj* p = pGEMObjs->GetIDRObj(unit);
	if (p) {
		switch (p->GetServiceStatus()) {
		case CIDRObj::INSERVICE:
			return "In Service";
			break;
		case CIDRObj::OUTOFSERVICE:
			return "Out of Service";
			break;
		}
	}
	return "";
}

CString CGlobalDeviceContainer::GetIDRBypassStr(short unit) {
	CIDRObj* p = pGEMObjs->GetIDRObj(unit);
	if (p) {
		switch (p->GetBypassStatus()) {
		case CIDRObj::INUSE:
			return "In Use";
			break;
		case CIDRObj::BYPASSED:
			return "Bypassed";
			break;
		}
	}
	return "";
}

void CGlobalDeviceContainer::SetAlarm(CAlrmList::ALIDX alidx, CString msg) {
	int alid = CEIDCtlr.m_alarm.GetAlarmID(alidx);
	CEIDCtlr.m_alarm.SetAlarm(alid);
	// [02022024 ZHIMING
	if (SystemModel == NMODEL::FSM413SA) {
		if (pLTMgr) pLTMgr->Set_Alarm();
	}
	// ]
	LogFile.LogLocal(msg);
}

void CGlobalDeviceContainer::ClearAlarm(CAlrmList::ALIDX alidx) {
	int alid = CEIDCtlr.m_alarm.GetAlarmID(alidx);
	CEIDCtlr.m_alarm.ClearAlarm(alid);
	// [02022024 ZHIMING
	if (SystemModel == NMODEL::FSM413SA) {
		if (pLTMgr) pLTMgr->Set_Idle();
	}
	// ]
	LogFile.LogLocal(CEIDCtlr.m_alarm.GetAlarmText(alidx) + " cleared");
}

void CGlobalDeviceContainer::SetARAMSState(SArams::ARAMSSTATE state) {
	if (m_SystemStatusDlg) {
		switch (state) {
		case SArams::ENGINEERING:
			m_SystemStatusDlg->SetEngineering();
			break;
		case SArams::NONSCHEDULED:
			m_SystemStatusDlg->SetNonScheduled();
			break;
		case SArams::STANDBY:
			m_SystemStatusDlg->SetStandby();
			break;
		case SArams::PRODUCTIVE:
			m_SystemStatusDlg->SetProductive();
			break;
		case SArams::SCHEDULEDDOWN:
			m_SystemStatusDlg->SetScheduledDown();
			break;
		case SArams::UNSCHEDULEDDOWN:
			m_SystemStatusDlg->SetUnscheduledDown();
			break;
			//20131007-01 SCOTT ADD ------------------------------------------
		case SArams::NOSTATE:
			p413App->Log("Invalid SetARAMSState Call with NOSTATE");
			break;
		case SArams::INITIALIZE:
			p413App->Log("Invalid SetARAMSState Call with INITIALIZE");
			break;
		case SArams::SETTINGUP:
			p413App->Log("Invalid SetARAMSState Call with SETTINGUP");
			break;
			//-------------------------------------------------------------
		default:
			ASSERT(0);
			break;
		}
	}
}

SArams::ARAMSSTATE CGlobalDeviceContainer::GetARAMSState() {
	if (m_SystemStatusDlg) {
		return m_SystemStatusDlg->GetCurrentState();
	}
	ASSERT(0);
	return SArams::NOSTATE;
}

CString CGlobalDeviceContainer::GetARAMSStateStr() {
	if (m_SystemStatusDlg) {
		switch (m_SystemStatusDlg->GetCurrentState()) {
		case SArams::STANDBY:
			return "IDLE";
			break;
		case SArams::PRODUCTIVE:
			return "WORKING";
			break;
		default:
			return "MAINTENANCE";
			break;
		}
	}
	return "?";
}

int CGlobalDeviceContainer::GGetARAMSState() {
	if (m_SystemStatusDlg) {
		switch (m_SystemStatusDlg->GetCurrentState()) {
		case SArams::STANDBY:
			return 1;
			break;
		case SArams::PRODUCTIVE:
			return 2;
			break;
		default:
			return 3;
			break;
		}
	}
	return 0;
}

void CGlobalDeviceContainer::RegisterSystemStatus(CSystemStatusDlg* pDlg) {
	m_SystemStatusDlg = pDlg;
}

BOOL CGlobalDeviceContainer::AuthenticateUser(short& AuthenLevel, short UILevel) {
	//20230335 if (Var.UIMode <= UILevel) {
		CGetPW pw;
		if (pw.DoModal() == IDOK) {
			pw.m_username = "FSM";
			pw.m_password = "123";
			if (pw.m_username == "123FSM") {
				username = pw.m_username;
				return TRUE;
			}
			CLogInDataNew login;
			if (login.check_logging(pw.m_username, pw.m_password, AuthenLevel)) {
				pGDev->username = pw.m_username;	// 20230512 - Store the username
				return TRUE;
			}
			else {
				AfxMessageBox("User not found");
			}
		}
		return FALSE;
	//20230325 }
	username = "";
	AuthenLevel = CLogInDataNew::FULLACC;
	return TRUE;
}

BOOL CGlobalDeviceContainer::WaferPresentCheck() {
	// LYF Note this is not thread safe [4/25/2006]
	if (pRbtMgr && pRbtMgr->pRbtSvr) {
		return pRbtMgr->pRbtSvr->WaferPresentCheck();
	}
	return TRUE;
}

void CGlobalDeviceContainer::SECSMessage(CString msg) {
	if (pOLnMgr) {
		pOLnMgr->SECSMessage(msg);
	}
}

void CGlobalDeviceContainer::SECSContent(BYTE* data, short len, BOOL bLcl) {
	if (pOLnMgr) {
		pOLnMgr->SECSContent(data, len, bLcl, NULL);
	}
}

CString CGlobalDeviceContainer::GetTimeStamp() {
	CString str;
	str.Format("%s  ", CTime::GetCurrentTime().Format("%y:%m:%d %H-%M-%S"));
	return str;
}

BOOL CGlobalDeviceContainer::IsSA413() {
	return (SystemModel == FSM413C2C) || (SystemModel == FSM413MOT) || (SystemModel == FSM413SA);
}

BOOL CGlobalDeviceContainer::TransferWafer(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn) {
	p413App->Global.bCanClose = FALSE;
	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			CString str;
			//[[20240820/Arup 
				//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
			//p413App->Global.LoadMaxSlot();
			int nSlot = p413App->Global.SlotsPerFOUP;
			switch (nSlot) {
			case 13:
				if (FrSlot > 13) { ASSERT(0); return -1; }
				str.Format("XFER %d SLOT %d %d SLOT %d", FrStn, FrSlot, ToStn, ToSlot);
				break;
			case 25:
				str.Format("XFER %d SLOT %d %d SLOT %d", FrStn, FrSlot, ToStn, ToSlot);
				break;
			default: ASSERT(0); return -1; break;
			}
			//]]
			//str.Format("XFER %d SLOT %d %d SLOT %d", FrStn, FrSlot, ToStn, ToSlot);
			/*if (bAlgn) {
				str += " ALGN";
			}*/
			p413App->Log(str);
			if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
				p413App->Global.bCanClose = TRUE;
				return TRUE;
			}
			else {
				str += " FAILED";
				p413App->Log(str, 0);
			}
		}
	}
	return FALSE;
}

// Note: Only valid in the direction of FOUP to measuring station
BOOL CGlobalDeviceContainer::TransferWafer1(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn) {
	p413App->Global.bCanClose = FALSE;
	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			// [6/4/2020] ============================================
			CString inifile;
			DosUtil.GetLocalCfgFile(inifile);
			int twiceAlign = GetPrivateProfileInt("RobotUnit", "TwiceAlign", 1, inifile);
			if (twiceAlign) {
				CString str;
				
				//[[20240820/Arup 
				//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
				//p413App->Global.LoadMaxSlot();
				int nSlot = p413App->Global.SlotsPerFOUP;
				switch (nSlot) {
				case 13:
					if (FrSlot > 13) { ASSERT(0); return -1; }
					str.Format("XFER %d SLOT %d 1", FrStn, FrSlot); //pick and place 
					break;
				case 25:
					str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
					break;
				default: ASSERT(0); return -1; break;
				}
				//]]
				p413App->Log(str);
				if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
					//pRbtMgr->pRbtSvr->IsWaferOnAlgn();//just for checking
					str = "ALIGN";
					if (!pRbtMgr->pRbtSvr->Align()) {
						if (!pRbtMgr->pRbtSvr->Align()) {
							str += " FALIED";
							p413App->Log(str, 0);
						}
					}
					str = "PICK 1";
					p413App->Log(str);
					if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {//Arup
						str = "PLACE 1";
						if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
							str = "ALIGN";
							if (!pRbtMgr->pRbtSvr->Align()) {
								if (!pRbtMgr->pRbtSvr->Align()) {
									str += " FALIED";
									p413App->Log(str, 0);
								}
							}
							//pRbtMgr->pRbtSvr->IsWaferOnAlgn();//just for checking
							str = "PICK 1";
							if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
								str = "PLACE 3";
								p413App->Log(str);
								if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
									p413App->Global.bCanClose = TRUE;
									return TRUE;
								}
								else {
									str += " FALIED";
									p413App->Log(str, 0);
								}
							}
						}
					}
					else {
						str += " FALIED";
						p413App->Log(str, 0);
					}
				}
			}
			else {
				CString str;
				//[[20240820/Arup 
				//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
				//p413App->Global.LoadMaxSlot();
				int nSlot = p413App->Global.SlotsPerFOUP;
				switch (nSlot) {
				case 13:
					if (FrSlot > 13) { ASSERT(0); return -1; }
					str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
					break;
				case 25:
					str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
					break;
				default: ASSERT(0); return -1; break;
				}
				//]]
				
				p413App->Log(str);
				if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
					str = "ALIGN";
					if (!pRbtMgr->pRbtSvr->Align()) {
						if (!pRbtMgr->pRbtSvr->Align()) {
							str += " FALIED";
							p413App->Log(str, 0);
						}
					}
					str = "PICK 1";
					p413App->Log(str);
					if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
						str = "PLACE 3";
						p413App->Log(str);
						if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
							p413App->Global.bCanClose = TRUE;
							return TRUE;
						}
						else {
							str += " FALIED";
							p413App->Log(str, 0);
						}
					}
					else {
						str += " FALIED";
						p413App->Log(str, 0);
					}
				}
			}
		}
		// ==========================================================
	}
	return FALSE;
}

void CGlobalDeviceContainer::LoadSystemOption() {
	char str[256];
	CString winini;
	CString st, sect;

	DosUtil.GetLocalSysFile(winini);
	sect = "FSM";
	GetPrivateProfileString(sect, "Model", "GENERIC", str, 255, winini);
	st = str;
	if (st.Find("FSM413C2C") != -1) {
		SystemModel = FSM413C2C;
	}
	else if (st.Find("FSM413SA") != -1) {
		SystemModel = FSM413SA;
	}
	else if (st.Find("FSM413MOT") != -1) {
		SystemModel = FSM413MOT;
	}
	else {
		SystemModel = MGENERIC;
	}
	GetPrivateProfileString(sect, "SubModel", "GENERIC", str, 255, winini);
	st = str;
	if (st.Find("MDLS") != -1)	// SMIC [6/18/2012 FSM 413 C2C]
	{
		SubModel = SMIC;
	}
	//20120921 SCOTT EDIT if (st.Find("MDLU") != -1)	// UMC [6/18/2012 FSM 413 C2C]
	else if (st.Find("MDLU") != -1)	// UMC [6/18/2012 FSM 413 C2C]
	{
		SubModel = UMC;
		bAutoStartLatching = TRUE;	//04262022 ZHIMING //UMC USER NEED TO AUTO-RUN WHEN FOUP LOADED
	}
	else if (st.Find("MDLT") != -1)	// TSMC [6/18/2012 FSM 413 C2C]
	{
		SubModel = TSMC;
	}
	else if (st.Find("MDLA") != -1)	// ASE [6/18/2012 FSM 413 C2C]
	{
		SubModel = ASE;
	}
	else {
		SubModel = SGENERIC;
	}
}

void CGlobalDeviceContainer::SaveSystemOption() {
	CString str;
	CString sect;
	CString winini;

	DosUtil.GetLocalSysFile(winini);
	sect = "FSM";
	switch (SystemModel) {
	default:
		WritePrivateProfileString(sect, "Model", "GENERIC", winini);
		break;
	case FSM413C2C:
		WritePrivateProfileString(sect, "Model", "FSM413C2C", winini);
		break;
	case FSM413SA:
		WritePrivateProfileString(sect, "Model", "FSM413SA", winini);
		break;
	case FSM413MOT:
		WritePrivateProfileString(sect, "Model", "FSM413MOT", winini);
		break;
	}
	switch (SubModel) {
	case TSMC:
		WritePrivateProfileString(sect, "SubModel", "MDLT", winini);	// TSMC [6/18/2012 FSM 413 C2C]
		break;
	case ASE:
		WritePrivateProfileString(sect, "SubModel", "MDLA", winini);	// ASE [6/18/2012 FSM 413 C2C]
		break;
	case SMIC:
		WritePrivateProfileString(sect, "SubModel", "MDLS", winini);	// SMIC [6/18/2012 FSM 413 C2C]
		break;
		//20120921 SCOTT ADD ------------------------------------------------------------------------------
	case UMC:
		WritePrivateProfileString(sect, "SubModel", "MDLU", winini);	// SMIC [6/18/2012 FSM 413 C2C]
		break;
		//-------------------------------------------------------------------------------------------------
	default:
		WritePrivateProfileString(sect, "SubModel", "GENERIC", winini);
		break;
	}
}

void CGlobalDeviceContainer::SetActivePort(short PortNo) {
	if (pDataBnk) {
		pDataBnk->SetActivePort(PortNo);
	}
}

void CGlobalDeviceContainer::ClearActivePort() {
	if (pDataBnk) {
		pDataBnk->ClearActivePort();
	}
}

int CGlobalDeviceContainer::CanPerformPortAction(int PortID, const string& actn) {
	if (pDataBnk) {
		return pDataBnk->CanPerformPortAction(PortID, actn);
	}
	return 0;
}

void CGlobalDeviceContainer::SetActivePortSubstrate(short PortID, short Slot) {
	if (pDataBnk) {
		pDataBnk->SetActivePortSubstrate(PortID, Slot);
	}
}

void CGlobalDeviceContainer::ClearActivePortSubstrate() {
	if (pDataBnk) {
		pDataBnk->ClearActivePortSubstrate();
	}
}

void CGlobalDeviceContainer::SetActiveCJPJ(const std::string& CtlJobID, const std::string& PrjobID) {
	if (pDataBnk) {
		pDataBnk->SetActiveCJPJ(CtlJobID, PrjobID);
	}
}

BOOL CGlobalDeviceContainer::IsCarrierPresent(const char* Carrier) {
	if (pDataBnk) {
		return pDataBnk->IsCarrierPresent(Carrier);
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::GetCarrierID(const string& SubstID, CString& CarrierID) {
	if (pDataBnk) {
		return pDataBnk->GetCarrierID(SubstID, CarrierID);
	}
	return FALSE;
}

int CGlobalDeviceContainer::CanPerformService(string& Service, string& CarrierID, BYTE PortID, vector<string>& Attributes) {
	if (pDataBnk) {
		return pDataBnk->CanPerformService(Service, CarrierID, PortID, Attributes);
	}
	return 0;
}

int CGlobalDeviceContainer::PerformE87Service(const char* szCarrierID, const char* szAction,
	vector<string>& AttrIDs, vector<SecsItem*>& AttrValues, SecsItemList* pErrorList, BYTE PotID) {
	if (pDataBnk) {
		return pDataBnk->PerformE87Service(szCarrierID, szAction, AttrIDs, AttrValues, pErrorList, PotID);
	}
	return 0;
}

int CGlobalDeviceContainer::GetPortID(const string& CarrierID) {
	if (pDataBnk) {
		return pDataBnk->GetPortID(CarrierID);
	}
	return 0;
}

int CGlobalDeviceContainer::CreateObject(const string& ObjSpec, const string& ObjType, map<string, SecsItem*>& Map, vector<int>& ErrCodes, string& ObCreated) {
	if (pDataBnk) {
		return pDataBnk->CreateObject(ObjSpec, ObjType, Map, ErrCodes, ObCreated);
	}
	return 0;
}

int CGlobalDeviceContainer::DeleteObject(string& ObjID) {
	if (pDataBnk) {
		return pDataBnk->DeleteObject(ObjID);
	}
	return 0;
}

int CGlobalDeviceContainer::GetAssociatedPorts(const string& PGrpName, vector<BYTE>& PortList) {
	if (pDataBnk) {
		return pDataBnk->GetAssociatedPorts(PGrpName, PortList);
	}
	return 0;
}

void CGlobalDeviceContainer::AddPortGroup(PortGroup& PGrp) {
	if (pDataBnk) {
		pDataBnk->AddPortGroup(PGrp);
	}
}

BOOL CGlobalDeviceContainer::IsCarrierPresent(const string& Carrier) {
	if (pDataBnk) {
		return pDataBnk->IsCarrierPresent(Carrier);
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::IsSubstrate(const string& SubstID) {
	if (pDataBnk) {
		return pDataBnk->IsSubstrate(SubstID);
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::IsSubstrateLoc(const string& SubstLocID) {
	if (pDataBnk) {
		return pDataBnk->IsSubstrateLoc(SubstLocID);
	}
	return FALSE;
}

int CGlobalDeviceContainer::GetSubstAttr(const string& SubstID, const string& AttrID, SecsItemList* pList) {
	if (pDataBnk) {
		return pDataBnk->GetSubstAttr(SubstID, AttrID, pList);
	}
	return 0;
}

int CGlobalDeviceContainer::GetSubstLocAttr(const string& SubstLocID, const string& AttrID, SecsItemList* pList) {
	if (pDataBnk) {
		return pDataBnk->GetSubstLocAttr(SubstLocID, AttrID, pList);
	}
	return 0;
}

CPRJob* CGlobalDeviceContainer::GetPrjobObj(CString& PrJobID) {
	if (pDataBnk) {
		return pDataBnk->GetPrjobObj(PrJobID);
	}
	return NULL;
}

void CGlobalDeviceContainer::GetAllPRJobIDs(vector<string>& IdList) {
	if (pDataBnk) {
		pDataBnk->GetAllPRJobIDs(IdList);
	}
}

void CGlobalDeviceContainer::GetAllCJobIDs(vector<string>& IdList) {
	if (pDataBnk) {
		pDataBnk->GetAllCJobIDs(IdList);
	}
}

void CGlobalDeviceContainer::GetCarrierIds(vector<string>& CIds) {
	if (pDataBnk) {
		pDataBnk->GetCarrierIds(CIds);
	}
}

BOOL CGlobalDeviceContainer::GetAllSubstIDS(vector<string>& SubstIds) {
	if (pDataBnk) {
		return pDataBnk->GetAllSubstIDS(SubstIds);
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::GetSubstLocList(vector<string>& Locs) {
	if (pDataBnk) {
		return pDataBnk->GetSubstLocList(Locs);
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::GetCurrentPRJobID(string& Id) {
	if (pDataBnk) {
		return pDataBnk->GetCurrentPRJobID(Id);
	}
	return FALSE;
}

CPRJob* CGlobalDeviceContainer::GetActivePrJob() {
	if (pDataBnk) {
		return pDataBnk->GetActivePrJob();
	}
	return NULL;
}

CCtrlJob* CGlobalDeviceContainer::GetActiveCtrlJob() {
	if (pDataBnk) {
		return pDataBnk->GetActiveCtrlJob();
	}
	return NULL;
}

short CGlobalDeviceContainer::GetActiveSubstrateNo() {
	if (pDataBnk) {
		return pDataBnk->GetActiveSubstrate();
	}
	return NULL;
}

CSubstObj* CGlobalDeviceContainer::GetActiveSubstrateObj() {
	if (pDataBnk) {
		return pDataBnk->GetActiveSubstrateObj();
	}
	return NULL;
}

int CGlobalDeviceContainer::GetActivePort() {
	if (pDataBnk) {
		return pDataBnk->GetActivePort();
	}
	return 0;
}

BOOL CGlobalDeviceContainer::GetCurrentCJobID(string& Id) {
	if (pDataBnk) {
		return pDataBnk->GetCurrentCJobID(Id);
	}
	return FALSE;
}

void CGlobalDeviceContainer::ClearJobs() {
	if (pDataBnk) {
		pDataBnk->ClearJobs();
	}
}

void CGlobalDeviceContainer::SetActivePrJob(const std::string& PrjobID) {
	if (pDataBnk) {
		pDataBnk->SetActivePrJob(PrjobID);
	}
}

void CGlobalDeviceContainer::SetActiveCtrlJob(const std::string& CtlJobID) {
	if (pDataBnk) {
		pDataBnk->SetActiveCtrlJob(CtlJobID);
	}
}

void CGlobalDeviceContainer::SetActiveSubstrate(int SubstNo) {
	if (pDataBnk) {
		pDataBnk->SetActiveSubstrate(SubstNo);
	}
}

void CGlobalDeviceContainer::ClearActiveSubstrate() {
	if (pDataBnk) {
		pDataBnk->ClearActiveSubstrate();
	}
}

// MGENERIC, FSM413MOT, FSM413SA, FSM413C2C
// SGENERIC, TSMC, ASE, SMIC, UMC

CString CGlobalDeviceContainer::GetModelStr() {
	CString str;
	str = "FSM413";
	switch (SystemModel) {
	case FSM413C2C:
		str += "-C2C";
		break;
	case FSM413MOT:
		str += "-MOT";
		break;
	case FSM413SA:
		str += "-SA";
		break;
	}
	switch (SubModel) {
	case SGENERIC:
		str += "-G";
		break;
	case TSMC:
		str += "-T";
		break;
	case ASE:
		str += "-A";
		break;
	case SMIC:
		str += "-S";
		break;
	case UMC:
		str += "-U";
		break;
	}
	return str;
}

CString CGlobalDeviceContainer::GetSubModelStr() {
	CString str;
	str = "FSM";
	switch (SubModel) {
	case SGENERIC:
		str += "-G";
		break;
	case TSMC:
		str += "-T";
		break;
	case ASE:
		str += "-A";
		break;
	case SMIC:
		str += "-S";
		break;
	case UMC:
		str += "-U";
		break;
	}
	return str;
}

BOOL CGlobalDeviceContainer::ExecuteXFer(CString cmd) {
	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			CString str;
			p413App->Log(str);
			if (pRbtMgr->pRbtSvr->Xfer(cmd.GetBuffer(0))) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::SetPostPos(short AlgnNum, long PostPos, long OffsetPos) {
	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			return pRbtMgr->pRbtSvr->SetPostPos(AlgnNum, PostPos, OffsetPos);
		}
	}
	return FALSE;
}

void CGlobalDeviceContainer::justTest() {
	pRbtMgr->pRbtSvr->chkResponse();
}

/// <02092024   >
BOOL CGlobalDeviceContainer::TransferTest(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn) {
	p413App->Global.bCanClose = FALSE;


	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			// [6/4/2020] ============================================
			CString str;

			//[[20240820/Arup 
			//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
			//p413App->Global.LoadMaxSlot();
			int nSlot = p413App->Global.SlotsPerFOUP;
			switch (nSlot) {
			case 13:
				if (FrSlot > 13) { ASSERT(0); return -1; }
				//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
				break;
			case 25:
				//str.Format("XFER %d SLOT %d 1", FrStn, FrSlot);
				break;
			default: ASSERT(0); return -1; break;
			}
			//]]
			//p413App->Log(str);
			str.Format("PICK %d SLOT %d ", FrStn, FrSlot);//Pick wafer from cassette
			if (!pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) { 
				str += " FALIED";
				p413App->Log(str, 0);
			}
			else {
				p413App->Log(str);
				// Do we need to return here???
			}

			str.Format("PLACE %d SLOT %d ", 1, 1);//place to aligner
			if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
				if (!pRbtMgr->pRbtSvr->Align()) {
					if (!pRbtMgr->pRbtSvr->Align()) {
						str += " FALIED";
						p413App->Log(str, 0);
					}
				}
				//ALIGN DONE or FAILED 
				str.Format("PICK %d SLOT %d ", 1, 1);// Pick wafer from aligner
				if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
					str.Format("PLACE %d SLOT %d ", ToStn, ToSlot);//place to stage
					if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
						p413App->Log(str, 0);
						return TRUE;
					}
					else {
						str += " FALIED";
						p413App->Log(str, 0);
					}
					p413App->Log(str, 0);
				}
				else {
					str += " FALIED";
					p413App->Log(str, 0);
				}
			}
			//// back from stage 
			//str.Format("PICK %d SLOT %d ", ToStn, ToSlot);//Pick wafer from stage
			//if (!pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) { 
			//	str += " FALIED";
			//	p413App->Log(str, 0);
			//}
			//else {
			//	p413App->Log(str);
			//}
			//

			//str.Format("PLACE %d SLOT %d ", FrStn, FrSlot);//place to cassette
			//if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
			//	p413App->Log(str, 0);
			//	p413App->Global.bCanClose = TRUE; //need checking
			//	return TRUE;
			//}
			//else {
			//	str += " FALIED";
			//	p413App->Log(str, 0);
			//}

		}
		// ==========================================================
	}
	return FALSE;
}

BOOL CGlobalDeviceContainer::TransferTest2(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn) {
	p413App->Global.bCanClose = FALSE;


	if (pRbtMgr) {
		if (pRbtMgr->pRbtSvr) {
			// [6/4/2020] ============================================
			CString str;
			// back from stage 
			str.Format("PICK %d SLOT %d ", FrStn, FrSlot);//Pick wafer from stage
			if (!pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
				str += " FALIED";
				p413App->Log(str, 0);
			}
			else {
				p413App->Log(str);
			}


			str.Format("PLACE %d SLOT %d ", ToStn, ToSlot);//place to cassette
			if (pRbtMgr->pRbtSvr->Xfer(str.GetBuffer(0))) {
				p413App->Log(str, 0);
				p413App->Global.bCanClose = TRUE; //need checking
				return TRUE;
			}
			else {
				str += " FALIED";
				p413App->Log(str, 0);
			}

		}
		// ==========================================================
	}
	return FALSE;
}
/// </TEST>