// RecoveryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"

#include "413/MotorS2.h"
#include "413App.h"
#include "128/GEM/ObjectsContainer.h"
#include "FEM/Ldp/LPMgr.h"
#include "FEM/Rbt/RbtBase.h"
#include "FEM/Rbt/RbtMgr.h"
#include "FEM/Rbt/selport.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "InfoPanelSystem.h"
#include "IPC/SMCHdr.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "XTabCtrl.h"

#include "RecoveryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_SLOTMAP_TIMEOUT		9998
#define IDT_COMMAND_TIMEOUT		9997
#define IDT_NEXTSTEP			9996
#define IDT_WAFERCHECJ_TIMEOUT	9994

/////////////////////////////////////////////////////////////////////////////
// CRecoveryDlg dialog

CRecoveryDlg::CRecoveryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecoveryDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CRecoveryDlg)
	//}}AFX_DATA_INIT

	m_ViewName = "Recovery";
	m_ParentMain = NULL;
	m_ParentSystem = NULL;
	Port = 0;
	Slot = 0;
	type = 0;
	bStageHome = FALSE;
	// 	Empty = 0;

	bIsOpenOp = FALSE;

	bAutoClose = FALSE;
	bRecoverOK = FALSE;

	AckMsg = NULL;
	bEnable = FALSE;

	WaferLoc = NOTDEFINE;
	isHomeRobotDone = FALSE;
	isHomeAlignerDone = FALSE;
	isRestStageDone = FALSE;
	isGotoLoadDone = FALSE;

	ID = -1;  // Generate assertion if calling function did not set it properly [10/10/2012 Yuen]
}

CRecoveryDlg::~CRecoveryDlg() {}

void CRecoveryDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecoveryDlg)
	DDX_Control(pDX, IDC_OPEN, m_cOpen);
	//DDX_Control(pDX, IDC_LOADPOSITION, m_cLoadPos);
	DDX_Control(pDX, IDC_RESETROBOT, m_cHomeRobot);
	DDX_Control(pDX, IDC_RESETSTAGE, m_cResetAll);
	DDX_Control(pDX, IDC_CLOSEFOUP, m_cCloseFoup);
	DDX_Control(pDX, IDC_CLOSETAB, m_cCloseTab);
	DDX_Control(pDX, IDC_MAP, m_cMap);
	DDX_Control(pDX, IDC_SCANNER, m_cScanner);
	DDX_Control(pDX, IDC_ARM, m_cArm);
	DDX_Control(pDX, IDC_ALIGNER, m_cAligner);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_RECOVER, m_cRecover);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SLT1, m_csl[0]);
	DDX_Control(pDX, IDC_SLT2, m_csl[1]);
	DDX_Control(pDX, IDC_SLT3, m_csl[2]);
	DDX_Control(pDX, IDC_SLT4, m_csl[3]);
	DDX_Control(pDX, IDC_SLT5, m_csl[4]);
	DDX_Control(pDX, IDC_SLT6, m_csl[5]);
	DDX_Control(pDX, IDC_SLT7, m_csl[6]);
	DDX_Control(pDX, IDC_SLT8, m_csl[7]);
	DDX_Control(pDX, IDC_SLT9, m_csl[8]);
	DDX_Control(pDX, IDC_SLT10, m_csl[9]);
	DDX_Control(pDX, IDC_SLT11, m_csl[10]);
	DDX_Control(pDX, IDC_SLT12, m_csl[11]);
	DDX_Control(pDX, IDC_SLT13, m_csl[12]);
	DDX_Control(pDX, IDC_SLT14, m_csl[13]);
	DDX_Control(pDX, IDC_SLT15, m_csl[14]);
	DDX_Control(pDX, IDC_SLT16, m_csl[15]);
	DDX_Control(pDX, IDC_SLT17, m_csl[16]);
	DDX_Control(pDX, IDC_SLT18, m_csl[17]);
	DDX_Control(pDX, IDC_SLT19, m_csl[18]);
	DDX_Control(pDX, IDC_SLT20, m_csl[19]);
	DDX_Control(pDX, IDC_SLT21, m_csl[20]);
	DDX_Control(pDX, IDC_SLT22, m_csl[21]);
	DDX_Control(pDX, IDC_SLT23, m_csl[22]);
	DDX_Control(pDX, IDC_SLT24, m_csl[23]);
	DDX_Control(pDX, IDC_SLT25, m_csl[24]);
	//DDX_Control(pDX, IDC_RESETALIGNER, m_cResetAligner);
}

BEGIN_MESSAGE_MAP(CRecoveryDlg, CDialog)
	//{{AFX_MSG_MAP(CRecoveryDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_BN_CLICKED(IDC_CLOSETAB, OnClosetab)
	ON_BN_CLICKED(IDC_MAP, OnMap)
	ON_BN_CLICKED(IDC_CLOSEFOUP, OnClosefoup)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotmap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotmapfail)
	ON_BN_CLICKED(IDC_SLT1, OnSlot1)
	ON_BN_CLICKED(IDC_SLT10, OnSlot10)
	ON_BN_CLICKED(IDC_SLT11, OnSlot11)
	ON_BN_CLICKED(IDC_SLT12, OnSlot12)
	ON_BN_CLICKED(IDC_SLT13, OnSlot13)
	ON_BN_CLICKED(IDC_SLT14, OnSlot14)
	ON_BN_CLICKED(IDC_SLT15, OnSlot15)
	ON_BN_CLICKED(IDC_SLT16, OnSlot16)
	ON_BN_CLICKED(IDC_SLT17, OnSlot17)
	ON_BN_CLICKED(IDC_SLT18, OnSlot18)
	ON_BN_CLICKED(IDC_SLT19, OnSlot19)
	ON_BN_CLICKED(IDC_SLT2, OnSlot2)
	ON_BN_CLICKED(IDC_SLT20, OnSlot20)
	ON_BN_CLICKED(IDC_SLT21, OnSlot21)
	ON_BN_CLICKED(IDC_SLT22, OnSlot22)
	ON_BN_CLICKED(IDC_SLT23, OnSlot23)
	ON_BN_CLICKED(IDC_SLT24, OnSlot24)
	ON_BN_CLICKED(IDC_SLT25, OnSlot25)
	ON_BN_CLICKED(IDC_SLT3, OnSlot3)
	ON_BN_CLICKED(IDC_SLT4, OnSlot4)
	ON_BN_CLICKED(IDC_SLT5, OnSlot5)
	ON_BN_CLICKED(IDC_SLT6, OnSlot6)
	ON_BN_CLICKED(IDC_SLT7, OnSlot7)
	ON_BN_CLICKED(IDC_SLT8, OnSlot8)
	ON_BN_CLICKED(IDC_SLT9, OnSlot9)
	ON_WM_TIMER()
	ON_COMMAND(ID_LP_MAPOK, OnLpMapok)
	ON_COMMAND(ID_LP_MAPFAILED, OnLpMapfailed)
	ON_COMMAND(ID_COM_ENDCLOSE, OnComEndclose)
	ON_BN_CLICKED(IDC_RECOVER, OnRecover)
	ON_COMMAND(ID_LP_FOUPCLOSEOK, OnLpFoupcloseok)
	ON_BN_CLICKED(IDC_RESETSTAGE, OnResetstage)
	ON_BN_CLICKED(IDC_RESETROBOT, OnResetrobot)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio7)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio8)
	//ON_BN_CLICKED(IDC_LOADPOSITION, OnLoadposition)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	//ON_BN_CLICKED(IDC_RESETALIGNER, &CRecoveryDlg::OnBnClickedResetaligner)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecoveryDlg message handlers

LRESULT CRecoveryDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("System\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	p413App->Log("Enter Recovery Mode", 1);

	// wafer presence check [7/4/2020]
	if (CheckWaferOnArm()) {
		WaferLoc = ROBOTARM;
		CheckRadioButton(IDC_RADIO1, IDC_RADIO8, IDC_RADIO1);
		msg.Format("Detected wafer present on Arm");
		Message(msg);
		/*m_cResetAligner.EnableWindow(FALSE);
		m_cLoadPos.EnableWindow(FALSE);
		m_cMap.EnableWindow(FALSE);
		m_cResetAll.EnableWindow(FALSE);*/
		m_cOpen.EnableWindow(FALSE);
		//m_cLoadPos.EnableWindow(FALSE);
		m_cResetAll.EnableWindow(FALSE);
		m_cCloseFoup.EnableWindow(FALSE);
		m_cCloseTab.EnableWindow(FALSE);
		m_cMap.EnableWindow(FALSE);
		AfxMessageBox("Step 1: Do Home Robot");
		return 0;
	}

	if (CheckWaferOnAligner()) {
		WaferLoc = ALIGNER;
		CheckRadioButton(IDC_RADIO1, IDC_RADIO8, IDC_RADIO7);
		msg.Format("Detected wafer present on Aligner");
		Message(msg);
		//m_cResetAligner.EnableWindow(FALSE);
		//m_cLoadPos.EnableWindow(FALSE);
		//m_cMap.EnableWindow(FALSE);
		//m_cResetAll.EnableWindow(FALSE);
		m_cOpen.EnableWindow(FALSE);
		//m_cLoadPos.EnableWindow(FALSE);
		m_cResetAll.EnableWindow(FALSE);
		m_cCloseFoup.EnableWindow(FALSE);
		m_cCloseTab.EnableWindow(FALSE);
		m_cMap.EnableWindow(FALSE);
		AfxMessageBox("Step 1: Do Home Robot");
		return 0;
	}

	p413App->pMtr->pSiMc->SetChuckVacuum(TRUE);
	Sleep(1000);
	bool isWaferPresent = false;
	if (p413App->WaferOnStageCheck(&p413App->Global.RcpSetup, &isWaferPresent)) {
		if (isWaferPresent) {
			WaferLoc = STAGE;
			CheckRadioButton(IDC_RADIO1, IDC_RADIO8, IDC_RADIO8);
			msg.Format("Detected wafer present on Stage");
			Message(msg);
			//m_cResetAligner.EnableWindow(FALSE);
			//m_cLoadPos.EnableWindow(FALSE);
			//m_cMap.EnableWindow(FALSE);
			//m_cResetAll.EnableWindow(FALSE);
			m_cOpen.EnableWindow(FALSE);
			//m_cLoadPos.EnableWindow(FALSE);
			m_cResetAll.EnableWindow(FALSE);
			m_cCloseFoup.EnableWindow(FALSE);
			m_cCloseTab.EnableWindow(FALSE);
			m_cMap.EnableWindow(FALSE);
			AfxMessageBox("Step 1: Do Home Robot");
			return 0;
		}
	}
	// =======================================

	return 0;
}

void CRecoveryDlg::OnClosetab() {
	if ((Port >= 1) && (Port <= 4)) {
		pGDev->UnRegisterWinWithLPMgrEx(m_hWnd, Port);
		Sleep(1000);

		if (m_ParentMain) {
			if (AckMsg) {
				if (bRecoverOK) {
					DosUtil.ForgetPrJob(Port);
					ConfirmNoWaferOnStage();
				}
				else {
					if (AfxMessageBox("Erase Job Information", MB_YESNO) == IDYES) {
						if (Var.UIMode <= 1) {
							//  [10/27/2008 Yuen]
							short AuthenLevel;
							if (pGDev->AuthenticateUser(AuthenLevel)) {
								if (AuthenLevel < CLogInDataNew::CHGSETTING) {
									AfxMessageBox("Access deny", MB_OK);
									return;
								}
								DosUtil.ForgetPrJob(Port);
								ConfirmNoWaferOnStage();
							}
							else {
								// fail authorization, will not forget PrJob, will not exit recovery [10/27/2008 Yuen]
								return;
							}
						}
						else {
							DosUtil.ForgetPrJob(Port);
							ConfirmNoWaferOnStage();
						}
					}
				}
				p413App->bUIEnabled = TRUE;
				m_ParentMain->m_tabctrl.bUIBlocked = FALSE;
				Sleep(11340); //20130828-3 SCOTT ADD
				if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
					m_ParentMain->PostMessage(UM_TO_PROCJOBDLG, Port - 1, 0);
				}
				else {
					m_ParentMain->PostMessage(UM_TO_PROCJOBMANUALDLG, Port - 1, 0);
				}
			}
			else {
				DosUtil.ForgetPrJob(Port);
				ConfirmNoWaferOnStage();
				m_ParentMain->PostMessage(UM_CLOSETAB1, 0, (long)this);
			}
		}
	}
	if (m_ParentSystem) {
		m_ParentSystem->PostMessage(UM_CLOSETAB, 0, (long)this);
	}
	p413App->Global.SetRecoveryStatus(0);//09092024
}

void CRecoveryDlg::OnMap() {
	//CSelPort dlg;
	//if (/*(Port == 0) && (*/dlg.DoModal() == IDOK/*)*/) {
	//	Port = dlg.Port;
	//	ID = Port - 1;
	//	CString str;
	//	str.Format("LP%d selected", Port);
	//	Message(str);
	//}
	//else {
	//	return;
	//}

	if (p413App->Global.CheckRecovery()) {
		CSelPort dlg;
		if (dlg.DoModal() == IDOK) {
			Port = dlg.Port;
			ID = Port - 1;
			CString str;
			str.Format("LP%d selected", Port);
			Message(str);
		}
	}
	else {
		ID = Port - 1;
	}

	//ID = Port - 1;
	CString str;
	str.Format("LP%d selected", Port);

	if ((Port >= 1) && (Port <= 4)) {
		pGDev->RegisterWinWithLPMgrEx(m_hWnd, Port);
		Sleep(1000);

		CSMOL SM;
		SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
		SM.HWin = m_hWnd;
		SM.Ack = ID_LP_MAPOK;
		SM.Nak = ID_LP_MAPFAILED;
		SM.Pm.unit = Port;
		SM.Pm.nunit = 1;
		SM.Pm.act = PrcsMgrServices::MAP;
		if (pGDev && pGDev->pLPMgr) {
			CSMClt ipc;
			if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
				if (ipc.Send(&SM)) {
					bIsOpenOp = TRUE;
					m_cMap.ShowWindow(SW_HIDE);
					m_cCloseFoup.ShowWindow(SW_HIDE);
					m_cCloseTab.ShowWindow(SW_HIDE);

					m_cOpen.EnableWindow(FALSE);
					//m_cLoadPos.EnableWindow(FALSE);
					m_cResetAll.EnableWindow(FALSE);
					m_cCloseFoup.EnableWindow(FALSE);
					m_cCloseTab.EnableWindow(FALSE);
					//m_cMap.EnableWindow(FALSE);
					m_cHomeRobot.EnableWindow(FALSE);



					SetTimer(IDT_SLOTMAP_TIMEOUT, 15000, NULL);
					//m_cMap.EnableWindow(TRUE);
					m_cOpen.EnableWindow(FALSE);
					AfxMessageBox("Next step: Recover");
					return;
				}
				m_cOpen.EnableWindow(FALSE);
			}
			m_cOpen.EnableWindow(FALSE);
		}
		m_cOpen.EnableWindow(FALSE);
		ASSERT(0);
		return;
	}
}

void CRecoveryDlg::OnClosefoup() {
	/*if (pGDev->pRbtMgr && pGDev->pRbtMgr->pRbtSvr) {
		if (pGDev->pRbtMgr->pRbtSvr->HomeRobot(1)) {
			Message("Robot home OK");
		}
	}
	CloseFoup();
	return;*/

	CloseFoup();
	p413App->bUIEnabled = TRUE;
	m_ParentMain->m_tabctrl.bUIBlocked = FALSE;
	return;
}

BOOL CRecoveryDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	bEnable = FALSE;

	if (type == 0) {
		ASSERT(0);
	}
	else if (type == 3) {
	}

	for (int i = 0; i < 25; i++) {
		m_csl[i].EnableWindow(bEnable);
	}

	SetTimer(IDT_NEXTSTEP, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CRecoveryDlg::OnComSlotmap(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_SLOTMAP_TIMEOUT);

	if (bIsOpenOp) {
		m_cRecover.ShowWindow(SW_SHOW);
		m_cCloseTab.ShowWindow(SW_SHOW);
		bIsOpenOp = FALSE;
	}

	if (!lP) {
		ASSERT(0);
		return -1;
	}

	CString slotmap = (char*)lP;

	Message(slotmap);

	for (int i = 0; i < 25; i++) {
		if (slotmap.GetAt(i) == CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED)) {
			m_csl[i].EnableWindow(FALSE);
		}
		else {
			m_csl[i].EnableWindow(TRUE);
		}
	}
	UpdateData(FALSE);

	return 0;
}

LRESULT CRecoveryDlg::OnComSlotmapfail(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_SLOTMAP_TIMEOUT);
	return 0;
}

void CRecoveryDlg::OnSlot1() {
	Slot = 1;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot10() {
	Slot = 10;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot11() {
	Slot = 11;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot12() {
	Slot = 12;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot13() {
	Slot = 13;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot14() {
	Slot = 14;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot15() {
	Slot = 15;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot16() {
	Slot = 16;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot17() {
	Slot = 17;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot18() {
	Slot = 18;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot19() {
	Slot = 19;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot2() {
	Slot = 2;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot20() {
	Slot = 20;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot21() {
	Slot = 21;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot22() {
	Slot = 22;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot23() {
	Slot = 23;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot24() {
	Slot = 24;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot25() {
	Slot = 25;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot3() {
	Slot = 3;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot4() {
	Slot = 4;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot5() {
	Slot = 5;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot6() {
	Slot = 6;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot7() {
	Slot = 7;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot8() {
	Slot = 8;
	DspSlotMessage();
}

void CRecoveryDlg::OnSlot9() {
	Slot = 9;
	DspSlotMessage();
}

void CRecoveryDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_SLOTMAP_TIMEOUT) {
		KillTimer(IDT_SLOTMAP_TIMEOUT);
		//////////////////////////////////////////////////////////////////////////
		bIsOpenOp = FALSE;
		m_cMap.ShowWindow(SW_SHOW);
		m_cCloseFoup.ShowWindow(SW_SHOW);
		m_cCloseTab.ShowWindow(SW_SHOW);
	}
	if (nIDEvent == IDT_WAFERCHECJ_TIMEOUT) {
		KillTimer(IDT_WAFERCHECJ_TIMEOUT);
		switch (type) {
		case 1:
			Message("128 Unit failed to return wafer present information");
			break;
		case 2:
			Message("900 Unit failed to return wafer present information");
			break;
		case 3:
			Message("413 Unit failed to return wafer present information");
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	if (nIDEvent == IDT_COMMAND_TIMEOUT) {
		KillTimer(IDT_COMMAND_TIMEOUT);
		switch (type) {
		case 1:
			Message("128 Unit failed to return slotmap information");
			break;
		case 2:
			Message("900 Unit failed to return slotmap information");
			break;
		case 3:
			Message("413 Unit failed to return slotmap information");
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	if (nIDEvent == IDT_NEXTSTEP) {
		KillTimer(IDT_NEXTSTEP);
		if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
			if (AfxMessageBox("Click YES after make sure that 413 Module has stopped", MB_YESNO) == IDYES) {
				StatusCheck413();
			}
			else {
				OnClosetab();
			}
		}
		else {
			OnClosetab();
		}
	}
}

void CRecoveryDlg::OnLpMapok() {
	m_cMap.ShowWindow(SW_HIDE);
}

void CRecoveryDlg::OnLpMapfailed() {
	Message("Error: Map fail");
	m_cMap.ShowWindow(SW_SHOW);
	m_cCloseFoup.ShowWindow(SW_SHOW);
	m_cCloseTab.ShowWindow(SW_SHOW);
}

void CRecoveryDlg::OnComEndclose() {
	Message("Foup closed");
	if (bAutoClose) {
		AfxMessageBox("Perform recovery again\nif there are other wafers in tool");
		OnClosetab();
	}
}

void CRecoveryDlg::OnRecover() {
	m_cOpen.EnableWindow(FALSE);
	if (Slot == 0) {
		Message("Pease select a slot");
		return;
	}
	if (WaferLoc == NOTDEFINE) {
		Message("Please select the location of wafer");
		return;
	}
	if (!bStageHome) {
		Message("Please reset stage");
		return;
	}
	if (!p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) {
		p413App->StageMoveError();
		Message("Stage may not be at load position");
		return;
	}

	if (MessageBox("Is stage at load position?", "Please confirm", MB_YESNO) != IDYES) {
		Message("Make sure stage is at load position");
		return;
	}

	CString str;
	CString WafLoc[4] = { "Not Define", "Robot Arm", "Aligner", "Stage" };
	str.Format("Wafer will be moved from %s to load port %d slot %d", WafLoc[WaferLoc], Port, Slot);
	if (MessageBox(str, "Please confirm", MB_YESNO) != IDYES) {
		return;
	}

	m_cRecover.ShowWindow(SW_HIDE);
	m_cCloseFoup.ShowWindow(SW_HIDE);
	m_cCloseTab.ShowWindow(SW_HIDE);

	if (!RecoverWafer413()) {
		m_cRecover.ShowWindow(SW_SHOW);

		m_cCloseFoup.ShowWindow(SW_SHOW);
		m_cCloseFoup.EnableWindow(FALSE);
		m_cCloseTab.EnableWindow(FALSE);

	}
	m_cRecover.ShowWindow(SW_SHOW);
	m_cRecover.EnableWindow(FALSE);
	AfxMessageBox("Next step: Close foup");
	m_cCloseFoup.ShowWindow(SW_SHOW);
	m_cCloseFoup.EnableWindow(TRUE);
	m_cCloseTab.ShowWindow(SW_SHOW);
	m_cCloseTab.EnableWindow(FALSE);
	m_cOpen.EnableWindow(FALSE);
}

BOOL CRecoveryDlg::RecoverWafer413() {
	if ((Port == 0) || (Slot == 0)) {
		return FALSE;
	}
	switch (WaferLoc) {
	case ROBOTARM:
		WaferPlace(WaferLoc, Port, Slot);
		break;
	case ALIGNER:
		WaferXfer(WaferLoc, Port, Slot);
		break;
	case STAGE:
		WaferXfer(WaferLoc, Port, Slot);
		break;
	default:
		return FALSE;
	}
	bRecoverOK = TRUE;
	DosUtil.ForgetPrJob(Port);
	// 	CloseFoup();

	return bRecoverOK;
}

BOOL CRecoveryDlg::WaferPlace(WAFERLOC Loc, short Port, short Slt) {
	CString cmd;
	short Pod;

	switch (Port) {
	case 1:
		Pod = DosUtil.GetInCasLoc(1, 4) + 1;
		break;
	case 2:
		Pod = DosUtil.GetInCasLoc(2, 6) + 1;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	switch (Loc) {
	case ROBOTARM:
		cmd.Format("PLACE %d SLOT %d", Pod, Slt);
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	// Actual transfer here [1/1/2013 Administrator]
	Message(cmd);
	return pGDev->ExecuteXFer(cmd);
}

BOOL CRecoveryDlg::WaferXfer(WAFERLOC Loc, short Port, short Slt) {
	CString cmd;
	short Pod;

	switch (Port) {
	case 1:
		Pod = DosUtil.GetInCasLoc(1, 4) + 1;
		break;
	case 2:
		Pod = DosUtil.GetInCasLoc(2, 6) + 1;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	switch (Loc) {
	case ALIGNER:
		cmd.Format("XFER %d SLOT 1 %d SLOT %d", 1, Pod, Slt);
		break;
	case STAGE:
		cmd.Format("XFER %d SLOT 1 %d SLOT %d", 3, Pod, Slt);
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	// Actual transfer here [1/1/2013 Administrator]
	Message(cmd);
	return pGDev->ExecuteXFer(cmd);
}

void CRecoveryDlg::CloseFoup() {
	if (Port == 0) {
		CSelPort dlg;

		if (dlg.DoModal() == IDOK) {
			Port = dlg.Port;
			ID = Port - 1;
		}
		else {
			return;
		}
	}

	pGDev->RegisterWinWithLPMgrEx(m_hWnd, Port);
	Sleep(1000);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = Port;
	SM.Pm.nunit = 1;
	if (DosUtil.GetClampState()) {
		SM.Pm.act = PrcsMgrServices::CLOSENORELEASE;
	}
	else {
		SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	}
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
		msg.Format("Close foup send fail (%d)", Port);
		PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
		return;
	}
	PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
}

void CRecoveryDlg::StatusCheck413() {
	SetWaferStation413();
}

void CRecoveryDlg::SetWaferStation413() {
	m_cMap.ShowWindow(SW_SHOW);
	m_cCloseFoup.ShowWindow(SW_SHOW);
	return;
}

BOOL CRecoveryDlg::CheckWaferOnArm(short arm) {
	if (pGDev->pRbtMgr) {
		return pGDev->pRbtMgr->IsWaferOnArm(arm);
	}
	return FALSE;
}

BOOL CRecoveryDlg::CheckWaferOnArm() {
	if (pGDev->pRbtMgr) {
		return pGDev->pRbtMgr->IsWaferOnArm();
	}
	return FALSE;

}

// used on second aligner which do not have a wafer sensor.
BOOL CRecoveryDlg::CheckWaferOnAligner(short arm) {
	return pGDev->pRbtMgr->IsWaferOnAligner(arm);
}

BOOL CRecoveryDlg::CheckWaferOnAligner() {
	return pGDev->pRbtMgr->IsWaferOnAligner();
}

void CRecoveryDlg::OnLpFoupcloseok() {
	OnClosetab();
}

int CRecoveryDlg::GetWaferPresent() {
	return pGDev->pRbtMgr->GetDIOIN();
}

void CRecoveryDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}

void CRecoveryDlg::ConfirmNoWaferOnStage() {
	if (MessageBox("Please confirm that\nthere is no wafer on stage", "Wafer on Stage", MB_YESNO) == IDYES) {
		p413App->Global.WaferOnStage = 0;
	}
	else {
		if ((ID < 0) || (ID > MAX_LPSUPPORTED)) {
			ASSERT(0);
			return;
		}
		p413App->Global.WaferOnStage = ID + 1;
	}
	DosUtil.SetWaferOnStage(p413App->Global.WaferOnStage);
}

void CRecoveryDlg::OnResetstage() {
	//m_cLoadPos.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		p413App->pMtr->pSiMc->SetChuckVacuum(TRUE, TRUE);
		if (pMtr->ResetAll()) {
			if (!pMtr->GotoXY(0, 0)) {
				p413App->StageMoveError();
				return;
			}
			pMtr->SetHome();
			p413App->Global.ResetLapTime = GetTickCount64();
			//p413App->Global.bConfirmStage = TRUE;
			Message("Stage home OK");
			if (p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) {
				m_cOpen.EnableWindow(TRUE);

				//m_cOpen.EnableWindow(FALSE);
				//m_cLoadPos.EnableWindow(FALSE);
				m_cResetAll.EnableWindow(FALSE);
				m_cCloseFoup.EnableWindow(FALSE);
				m_cCloseTab.EnableWindow(FALSE);
				m_cMap.EnableWindow(FALSE);
				m_cHomeRobot.EnableWindow(FALSE);
				//m_cResetAligner.EnableWindow(FALSE);
				bStageHome = TRUE;
				AfxMessageBox("Next step: Open Foup");
				return;
			}
			else {
				p413App->StageMoveError();
				Message("Stage may not be at load position");
			}
			p413App->pMtr->pSiMc->SetChuckVacuum(FALSE, FALSE);
		}
		else Message("Stage home FAILED");
	}
	//m_cLoadPos.EnableWindow(TRUE);
	m_cResetAll.EnableWindow(TRUE);
}

void CRecoveryDlg::OnResetrobot() {
	m_cHomeRobot.EnableWindow(FALSE);
	if (pGDev->pRbtMgr && pGDev->pRbtMgr->pRbtSvr) {
		if (pGDev->pRbtMgr->pRbtSvr->HomeRobot(1)) {
			Message("Robot home OK");
			m_cHomeRobot.EnableWindow(TRUE);
			//if (WaferLoc == ROBOTARM) {
				//AfxMessageBox("Next step: Do Mapping");
			AfxMessageBox("Next step: Reset Stage");
			//m_cMap.EnableWindow(TRUE);
			m_cResetAll.EnableWindow(TRUE);

			m_cOpen.EnableWindow(FALSE);
			//m_cLoadPos.EnableWindow(FALSE);
			//m_cResetAll.EnableWindow(FALSE);
			m_cCloseFoup.EnableWindow(FALSE);
			m_cCloseTab.EnableWindow(FALSE);
			m_cMap.EnableWindow(FALSE);
			m_cHomeRobot.EnableWindow(FALSE);
			//m_cResetAligner.EnableWindow(FALSE);
			return;
			//}
		}
	}
	Message("Robot home FAILED");
	m_cHomeRobot.EnableWindow(TRUE);
}

void CRecoveryDlg::Message(CString msg) {
	m_cList.InsertString(0, msg);
	m_cList.DeleteString(128);
}

// Robot arm [1/1/2013 Administrator]
void CRecoveryDlg::OnRadio1() {
	WaferLoc = ROBOTARM;
	Message("Robot Arm");
	m_cOpen.EnableWindow(FALSE);
	//m_cLoadPos.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);
	m_cCloseFoup.EnableWindow(FALSE);
	m_cCloseTab.EnableWindow(FALSE);
	m_cMap.EnableWindow(FALSE);
	m_cHomeRobot.EnableWindow(TRUE);
	AfxMessageBox("Step 1: Do Home Robot");
}

// Aligner [1/1/2013 Administrator]
void CRecoveryDlg::OnRadio7() {
	WaferLoc = ALIGNER;
	Message("Aligner");
	/*m_cResetAligner.EnableWindow(FALSE);
	m_cLoadPos.EnableWindow(FALSE);
	m_cMap.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);*/
	m_cOpen.EnableWindow(FALSE);
	//m_cLoadPos.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);
	m_cCloseFoup.EnableWindow(FALSE);
	m_cCloseTab.EnableWindow(FALSE);
	m_cMap.EnableWindow(FALSE);
	m_cHomeRobot.EnableWindow(TRUE);
	AfxMessageBox("Step 1: Do Home Robot");
}

// Stage [1/1/2013 Administrator]
void CRecoveryDlg::OnRadio8() {
	WaferLoc = STAGE;
	Message("Stage");
	/*m_cResetAligner.EnableWindow(FALSE);
	m_cLoadPos.EnableWindow(FALSE);
	m_cMap.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);*/
	m_cOpen.EnableWindow(FALSE);
	//m_cLoadPos.EnableWindow(FALSE);
	m_cResetAll.EnableWindow(FALSE);
	m_cCloseFoup.EnableWindow(FALSE);
	m_cCloseTab.EnableWindow(FALSE);
	m_cMap.EnableWindow(FALSE);
	m_cHomeRobot.EnableWindow(TRUE);
	AfxMessageBox("Step 1: Do Home Robot");
}

void CRecoveryDlg::DspSlotMessage() {
	CString str;

	str.Format("Slot %d selected", Slot);
	Message(str);
}

//void CRecoveryDlg::OnLoadposition() {
//	m_cResetAll.EnableWindow(FALSE);
//	m_cLoadPos.EnableWindow(FALSE);
//	if (p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) {
//		Message("Make sure Stage is at load position");
//		m_cLoadPos.EnableWindow(TRUE);
//		m_cResetAll.EnableWindow(TRUE);
//		m_cMap.EnableWindow(TRUE);
//		AfxMessageBox("Next step: Click Map");
//		return;
//	}
//	else {
//		Message("Stage may not be at load position");
//	}
//	m_cLoadPos.EnableWindow(TRUE);
//	m_cResetAll.EnableWindow(TRUE);
//}

void CRecoveryDlg::OnOpen() {
	CSelPort dlg;
	if (dlg.DoModal() == IDOK) {
		Port = dlg.Port;
		ID = Port - 1;
		CString str;
		str.Format("LP%d selected", Port);
		Message(str);
	}
	else {
		return;
	}

	if ((Port >= 1) && (Port <= 4)) {
		pGDev->RegisterWinWithLPMgrEx(m_hWnd, Port);
		Sleep(1000);

		m_cOpen.EnableWindow(FALSE);

		bIsOpenOp = TRUE;

		CSMOL SM;
		SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
		SM.HWin = m_hWnd;
		SM.Ack = ID_LP_FOUPOPENOK;
		SM.Nak = ID_LP_FOUPOPENFAIL;
		SM.Pm.unit = ID + 1;
		SM.Pm.nunit = ID + 1;
		SM.Pm.act = PrcsMgrServices::OPENFOUP;
		CSMClt ipc;
		if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
			if (ipc.Send(&SM)) {
				m_cOpen.EnableWindow(FALSE);
				//m_cLoadPos.EnableWindow(FALSE);
				m_cResetAll.EnableWindow(FALSE);
				m_cCloseFoup.EnableWindow(FALSE);
				m_cCloseTab.EnableWindow(FALSE);
				//m_cMap.EnableWindow(FALSE);
				m_cHomeRobot.EnableWindow(FALSE);
				m_cMap.EnableWindow(TRUE);
				AfxMessageBox("Next step: Map");
				//m_cResetAligner.EnableWindow(FALSE);
				return;
			}
		}
	}
	m_cOpen.EnableWindow(TRUE);
}

void CRecoveryDlg::OnComEndopen() {
	Message("Open FOUP complete");
	pGDev->pGEMObjs->Cms[ID].LPObj.CarrierLoc = CLPObj::clDOCKED;
	UpdateData(FALSE);
	m_cOpen.EnableWindow(TRUE);
}
void CRecoveryDlg::OnLpFoupopenfail() {
	CString str;
	str.LoadString(IDS_FOUPOPENFAILED);
	Message(str);
	m_cOpen.EnableWindow(TRUE);
}

//void CRecoveryDlg::OnBnClickedResetaligner()
//{
//	m_cResetAligner.EnableWindow(FALSE);
//	if (pGDev->pRbtMgr && pGDev->pRbtMgr->pRbtSvr) {
//		if (pGDev->pRbtMgr->pRbtSvr->HomeAlign(1)) {
//			Message("Aligner home OK");
//			m_cResetAligner.EnableWindow(TRUE);
//			//if (WaferLoc == STAGE) {
//				AfxMessageBox("Next step: Reset stage");
//				//AfxMessageBox("Next step: Home Aligner");
//				m_cResetAll.EnableWindow(TRUE);
//			//}
//			/*if (WaferLoc == ALIGNER) {
//				AfxMessageBox("Next step: Click Map");
//				m_cMap.EnableWindow(TRUE);
//			}*/
//			
//			return;
//		}
//	}
//	Message("Aligner home FAILED");
//	m_cResetAligner.EnableWindow(TRUE);
//}
