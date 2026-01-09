// SetupProp10Dlg.cpp : implementation file
//

#include "stdafx.h"
// #include <math.h>
#include "strSOp.h"
#include "e95.h"

#include "413/MotorS2.h"
#include "413App.h"
#include "128/GEM/ObjectsContainer.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IPC/SMCHdr.h"
#include "ProcJobDlg.h"
#include "ProcJobSMICDlg.h"
#include "SRC/DOSUtil.h"
#include "SRC/LogInDataNew.h"
#include "TitlePanel.h"
#include "XTabCtrl.h"

#include "../FEM/Rbt/RbtMgr.h"
#include "../FEM/Rbt/RbtBase.h"

#include "SetupProp10Dlg.h"

#include "InfoPanelMain.h"
#include "../SYS/CUMMsg.h"
#include "NavigationPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp10Dlg dialog

CSetupProp10Dlg::CSetupProp10Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp10Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp10Dlg)
	//}}AFX_DATA_INIT

	State = s10ALLCLOSE;
	SLP1 = SLP2 = 0;
	ActiveFoup = 0;
	bTransferInProgress = FALSE;
	bResetting = FALSE;
	bRelease = FALSE;

	LRID[0] = IDC_LSLOT1;
	LRID[1] = IDC_LSLOT2;
	LRID[2] = IDC_LSLOT3;
	LRID[3] = IDC_LSLOT4;
	LRID[4] = IDC_LSLOT5;
	LRID[5] = IDC_LSLOT6;
	LRID[6] = IDC_LSLOT7;
	LRID[7] = IDC_LSLOT8;
	LRID[8] = IDC_LSLOT9;
	LRID[9] = IDC_LSLOT10;
	LRID[10] = IDC_LSLOT11;
	LRID[11] = IDC_LSLOT12;
	LRID[12] = IDC_LSLOT13;
	LRID[13] = IDC_LSLOT14;
	LRID[14] = IDC_LSLOT15;
	LRID[15] = IDC_LSLOT16;
	LRID[16] = IDC_LSLOT17;
	LRID[17] = IDC_LSLOT18;
	LRID[18] = IDC_LSLOT19;
	LRID[19] = IDC_LSLOT20;
	LRID[20] = IDC_LSLOT21;
	LRID[21] = IDC_LSLOT22;
	LRID[22] = IDC_LSLOT23;
	LRID[23] = IDC_LSLOT24;
	LRID[24] = IDC_LSLOT25;

	RRID[0] = IDC_RSLOT1;
	RRID[1] = IDC_RSLOT2;
	RRID[2] = IDC_RSLOT3;
	RRID[3] = IDC_RSLOT4;
	RRID[4] = IDC_RSLOT5;
	RRID[5] = IDC_RSLOT6;
	RRID[6] = IDC_RSLOT7;
	RRID[7] = IDC_RSLOT8;
	RRID[8] = IDC_RSLOT9;
	RRID[9] = IDC_RSLOT10;
	RRID[10] = IDC_RSLOT11;
	RRID[11] = IDC_RSLOT12;
	RRID[12] = IDC_RSLOT13;
	RRID[13] = IDC_RSLOT14;
	RRID[14] = IDC_RSLOT15;
	RRID[15] = IDC_RSLOT16;
	RRID[16] = IDC_RSLOT17;
	RRID[17] = IDC_RSLOT18;
	RRID[18] = IDC_RSLOT19;
	RRID[19] = IDC_RSLOT20;
	RRID[20] = IDC_RSLOT21;
	RRID[21] = IDC_RSLOT22;
	RRID[22] = IDC_RSLOT23;
	RRID[23] = IDC_RSLOT24;
	RRID[24] = IDC_RSLOT25;

	m_cVideo.Name = "Setup10";

	hDryRun = NULL;
	bDryRunEnd = TRUE;
}

void CSetupProp10Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp10Dlg)
	DDX_Control(pDX, IDC_UNCLAMP3, m_cUnclamp2);
	DDX_Control(pDX, IDC_UNCLAMP, m_cUnclamp1);
	DDX_Control(pDX, IDC_REVERT, m_cRevert);
	DDX_Control(pDX, IDC_INVERT, m_cInvert);
	DDX_Control(pDX, IDC_IMAGEVIEW, m_cImgWnd);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_FINDCXCY, m_cAlgn);
	DDX_Control(pDX, IDC_RESETXY, m_cResetXY);
	DDX_Control(pDX, IDC_GOTOLOADPOS, m_cGotoOrigin);
	DDX_Control(pDX, IDC_TRANSFERTOSTAGE, m_cTransfer2stage);
	DDX_Control(pDX, IDC_TRANSFERTOLOADPORT, m_cTransfer2loadport);
	DDX_Control(pDX, IDC_OPENFOUP2, m_cOpenFoup2);
	DDX_Control(pDX, IDC_OPENFOUP1, m_cOpenFoup1);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_WAFERREPEAT2, p413App->Global.WaferRepeat);
	DDX_Text(pDX, IDC_CASSETTEREPEAT, p413App->Global.CassetteRepeat);
	DDX_Control(pDX, IDC_LOADPORT1NO, m_LPNum);
	DDX_Control(pDX, IDC_SLOTNUMS, m_SlotNums);
}

BEGIN_MESSAGE_MAP(CSetupProp10Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp10Dlg)
	ON_BN_CLICKED(IDC_OPENFOUP1, OnOpenfoup1)
	ON_BN_CLICKED(IDC_OPENFOUP2, OnOpenfoup2)
	ON_BN_CLICKED(IDC_TRANSFERTOSTAGE, OnTransfertostage)
	ON_BN_CLICKED(IDC_TRANSFERTOLOADPORT, OnTransfertoloadport)
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	ON_COMMAND(ID_LP_FOUPOPENOK, OnLpFoupopenok)
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_COMMAND(ID_COM_ABORTOPEN, OnComAbortopen)
	ON_COMMAND(ID_LP_FOUPCLOSEOK, OnLpFoupcloseok)
	ON_COMMAND(ID_LP_FOUPCLOSEFAIL, OnLpFoupclosefail)
	ON_COMMAND(ID_COM_ABORTCLOSE, OnComAbortclose)
	ON_COMMAND(ID_COM_ENDCLOSE, OnComEndclose)
	ON_BN_CLICKED(IDC_GOTOLOADPOS, OnGotoloadpos)
	ON_BN_CLICKED(IDC_LSLOT1, OnLslot1)
	ON_BN_CLICKED(IDC_RSLOT2, OnRslot2)
	ON_BN_CLICKED(IDC_RSLOT3, OnRslot3)
	ON_BN_CLICKED(IDC_RSLOT4, OnRslot4)
	ON_BN_CLICKED(IDC_RSLOT5, OnRslot5)
	ON_BN_CLICKED(IDC_RSLOT6, OnRslot6)
	ON_BN_CLICKED(IDC_RSLOT7, OnRslot7)
	ON_BN_CLICKED(IDC_RSLOT8, OnRslot8)
	ON_BN_CLICKED(IDC_RSLOT9, OnRslot9)
	ON_BN_CLICKED(IDC_RSLOT10, OnRslot10)
	ON_BN_CLICKED(IDC_RSLOT11, OnRslot11)
	ON_BN_CLICKED(IDC_RSLOT12, OnRslot12)
	ON_BN_CLICKED(IDC_RSLOT13, OnRslot13)
	ON_BN_CLICKED(IDC_RSLOT14, OnRslot14)
	ON_BN_CLICKED(IDC_RSLOT15, OnRslot15)
	ON_BN_CLICKED(IDC_RSLOT16, OnRslot16)
	ON_BN_CLICKED(IDC_RSLOT17, OnRslot17)
	ON_BN_CLICKED(IDC_RSLOT18, OnRslot18)
	ON_BN_CLICKED(IDC_RSLOT19, OnRslot19)
	ON_BN_CLICKED(IDC_RSLOT20, OnRslot20)
	ON_BN_CLICKED(IDC_RSLOT21, OnRslot21)
	ON_BN_CLICKED(IDC_RSLOT22, OnRslot22)
	ON_BN_CLICKED(IDC_RSLOT23, OnRslot23)
	ON_BN_CLICKED(IDC_RSLOT24, OnRslot24)
	ON_BN_CLICKED(IDC_RSLOT25, OnRslot25)
	ON_BN_CLICKED(IDC_LSLOT10, OnLslot10)
	ON_BN_CLICKED(IDC_LSLOT11, OnLslot11)
	ON_BN_CLICKED(IDC_LSLOT12, OnLslot12)
	ON_BN_CLICKED(IDC_LSLOT13, OnLslot13)
	ON_BN_CLICKED(IDC_LSLOT14, OnLslot14)
	ON_BN_CLICKED(IDC_LSLOT15, OnLslot15)
	ON_BN_CLICKED(IDC_LSLOT16, OnLslot16)
	ON_BN_CLICKED(IDC_LSLOT17, OnLslot17)
	ON_BN_CLICKED(IDC_LSLOT18, OnLslot18)
	ON_BN_CLICKED(IDC_LSLOT19, OnLslot19)
	ON_BN_CLICKED(IDC_LSLOT2, OnLslot2)
	ON_BN_CLICKED(IDC_LSLOT20, OnLslot20)
	ON_BN_CLICKED(IDC_LSLOT21, OnLslot21)
	ON_BN_CLICKED(IDC_LSLOT22, OnLslot22)
	ON_BN_CLICKED(IDC_LSLOT23, OnLslot23)
	ON_BN_CLICKED(IDC_LSLOT24, OnLslot24)
	ON_BN_CLICKED(IDC_LSLOT25, OnLslot25)
	ON_BN_CLICKED(IDC_LSLOT3, OnLslot3)
	ON_BN_CLICKED(IDC_LSLOT4, OnLslot4)
	ON_BN_CLICKED(IDC_LSLOT5, OnLslot5)
	ON_BN_CLICKED(IDC_LSLOT6, OnLslot6)
	ON_BN_CLICKED(IDC_LSLOT7, OnLslot7)
	ON_BN_CLICKED(IDC_LSLOT8, OnLslot8)
	ON_BN_CLICKED(IDC_LSLOT9, OnLslot9)
	ON_BN_CLICKED(IDC_RSLOT1, OnRslot1)
	ON_BN_CLICKED(IDC_RESETXY, OnResetxy)
	ON_EN_KILLFOCUS(IDC_WAFERREPEAT2, OnKillfocusWaferrepeat2)
	ON_EN_KILLFOCUS(IDC_CASSETTEREPEAT, OnKillfocusCassetterepeat)
	ON_BN_CLICKED(IDC_RELEASE, OnRelease)
	ON_BN_CLICKED(IDC_INITIALIZE, OnInitialize)
	ON_COMMAND(ID_COM_CLOSENORELEASECMPL, OnComClosenoreleasecmpl)
	ON_BN_CLICKED(IDC_UNCLAMP, OnUnclamp)
	ON_BN_CLICKED(IDC_UNCLAMP3, OnUnclamp3)
	ON_BN_CLICKED(IDC_CLEARWINTER, OnClearwinter)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotMapFail)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	ON_BN_CLICKED(IDC_FINDCXCY, OnFindcxcy)
	ON_BN_CLICKED(IDC_DRYRUN, OnDryrun)
	//}}AFX_MSG_MAP
// 	ON_MESSAGE(ID_COM_SLOTMAP,OnComSlotMap)
// 	ON_MESSAGE(ID_COM_SLOTMAPFAIL,OnComSlotMapFail)
ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
ON_BN_CLICKED(IDC_UNCLAMP, CSetupProp10Dlg::OnBnClickedUnclamp)
ON_BN_CLICKED(IDC_UNCLAMP3, CSetupProp10Dlg::OnBnClickedUnclamp3)
ON_BN_CLICKED(IDC_LPNUM, &CSetupProp10Dlg::OnBnClickedLpnum)
ON_BN_CLICKED(IDC_TESTRUN, &CSetupProp10Dlg::OnBnClickedTestrun)
ON_BN_CLICKED(IDC_RESPONSE, &CSetupProp10Dlg::OnBnClickedResponse)
ON_BN_CLICKED(IDC_PRESENCE, &CSetupProp10Dlg::OnBnClickedPresence)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp10Dlg message handlers
long CSetupProp10Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		StopAllCamera(&m_cVideo);
		m_cVideo.pWnd = NULL;
		UpdateData(TRUE);

		for (int i = 0; i < MAX_LPSUPPORTED; i++) {
			if (pUDev->pProcJobGENDlg[i]) {
				pUDev->pProcJobGENDlg[i]->PostMessage(WM_COMMAND, ID_REGISTERLP);
			}
			if (pUDev->pProcJobSMICDlg[i]) {
				pUDev->pProcJobSMICDlg[i]->PostMessage(WM_COMMAND, ID_REGISTERLP);
			}
		}
		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp10Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
		m_pVideo = &m_cVideo;
		m_cVideo.pWnd = this;
		m_cVideo.bShowCrossHair = FALSE;
		m_cVideo.bShowCenterTarget = TRUE;

		bTabSelected = TRUE;
	}
	return 0;
}

void CSetupProp10Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp10Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_FINDCXCY, TOP_RIGHT);
	AddAnchor(IDC_INVERT, TOP_RIGHT);
	AddAnchor(IDC_REVERT, TOP_RIGHT);

	hWndPC = m_hWnd;

	if (Var.UIMode > 1) {
		m_cInvert.ShowWindow(SW_SHOW);
		m_cRevert.ShowWindow(SW_SHOW);
	}

	if (p413App->Global.SlotsPerFOUP == 13) {
		for (int i = 13; i < 25; i++) {
			((CButton*)GetDlgItem(LRID[i]))->ShowWindow(FALSE);
			((CButton*)GetDlgItem(RRID[i]))->ShowWindow(FALSE);
		}
	}

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	SlotMap = "1111111111111111111111111";

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp10Dlg::OnOpenfoup1() {
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}//]
	BlockUI();
	switch (State) {
	case s10ALLCLOSE:
		m_cOpenFoup1.EnableWindow(FALSE);
		m_cOpenFoup2.EnableWindow(FALSE);
		EnableButton(FALSE);
		if (OpenFoup(1)) {
			return;
		}
		break;
	case s10FOUPOPENED:
		if (!bTransferInProgress) {
			m_cOpenFoup1.EnableWindow(FALSE);
			m_cOpenFoup2.EnableWindow(FALSE);
			EnableButton(FALSE);
			ClearAllSlots();
			if (CloseFoup(1)) {
				return;
			}
		}
		break;
	}
	m_cOpenFoup1.EnableWindow(TRUE);
	m_cOpenFoup2.EnableWindow(TRUE);
	EnableButton(FALSE);
	ReleaseUI();
}

void CSetupProp10Dlg::OnOpenfoup2() {
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}//]
	BlockUI();
	switch (State) {
	case s10ALLCLOSE:
		m_cOpenFoup1.EnableWindow(FALSE);
		m_cOpenFoup2.EnableWindow(FALSE);
		EnableButton(FALSE);
		if (OpenFoup(2)) {
			return;
		}
		break;
	case s10FOUPOPENED:
		if (!bTransferInProgress) {
			m_cOpenFoup1.EnableWindow(FALSE);
			m_cOpenFoup2.EnableWindow(FALSE);
			EnableButton(FALSE);
			ClearAllSlots();
			if (CloseFoup(2)) {
				return;
			}
		}
		break;
	}
	m_cOpenFoup1.EnableWindow(TRUE);
	m_cOpenFoup2.EnableWindow(TRUE);
	EnableButton(FALSE);
	ReleaseUI();
}

BOOL CSetupProp10Dlg::OpenFoup(short Port) {
	if (!pGDev->pLPMgr) {
		return FALSE;
	}
	ActiveFoup = Port;
	State = s10FOUPOPENING;

	// Open Foup
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPOPENOK;
	SM.Nak = ID_LP_FOUPOPENFAIL;
	SM.Pm.unit = Port;
	SM.Pm.nunit = Port;
	SM.Pm.act = PrcsMgrServices::OPENFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return TRUE;
		}
	}
	State = s10ALLCLOSE;
	PostMessage(WM_COMMAND, SM.Nak, NULL);
	return FALSE;
}

BOOL CSetupProp10Dlg::CloseFoup(short Port) {
	if (!pGDev->pLPMgr) {
		return FALSE;
	}
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = Port;
	SM.Pm.nunit = Port;
	if (DosUtil.GetClampState()) {
		SM.Pm.act = PrcsMgrServices::CLOSENORELEASE;
	}
	else {
		SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	}
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return TRUE;
		}
	}
	PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
	return FALSE;
}

void CSetupProp10Dlg::OnLpFoupopenfail() {
	EnableButton(FALSE);
	m_cOpenFoup1.EnableWindow(TRUE);
	m_cOpenFoup2.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp10Dlg::OnLpFoupopenok() {}

void CSetupProp10Dlg::OnComEndopen() {
	switch (ActiveFoup) {
	case 1:
		m_cOpenFoup1.SetWindowText("Close");
		m_cOpenFoup1.EnableWindow(TRUE);
		EnableButton(TRUE);
		if (g->WaferOnStage) {
			m_cTransfer2stage.EnableWindow(FALSE);
		}
		else {
			m_cTransfer2loadport.EnableWindow(FALSE);
		}
		State = s10FOUPOPENED;
		pGDev->pGEMObjs->Cms[ActiveFoup - 1].LPObj.CarrierLoc = CLPObj::clDOCKED;
		break;
	case 2:
		m_cOpenFoup2.SetWindowText("Close");
		m_cOpenFoup2.EnableWindow(TRUE);
		EnableButton(TRUE);
		if (g->WaferOnStage) {
			m_cTransfer2stage.EnableWindow(FALSE);
		}
		else {
			m_cTransfer2loadport.EnableWindow(FALSE);
		}
		State = s10FOUPOPENED;
		pGDev->pGEMObjs->Cms[ActiveFoup - 1].LPObj.CarrierLoc = CLPObj::clDOCKED;
		break;
	}
	ReleaseUI();
}

void CSetupProp10Dlg::OnComAbortopen() {
	OnLpFoupopenfail();
}

void CSetupProp10Dlg::OnLpFoupcloseok() {}

void CSetupProp10Dlg::OnLpFoupclosefail() {
	m_cOpenFoup1.EnableWindow(TRUE);
	m_cOpenFoup2.EnableWindow(TRUE);
	EnableButton(FALSE);
	ReleaseUI();
}

void CSetupProp10Dlg::OnComAbortclose() {
	OnLpFoupclosefail();
}

void CSetupProp10Dlg::OnComEndclose() {
	switch (ActiveFoup) {
	case 1:
		m_cOpenFoup1.SetWindowText("Open");
		State = s10ALLCLOSE;
		ClearChecks(ActiveFoup);
		break;
	case 2:
		m_cOpenFoup2.SetWindowText("Open");
		State = s10ALLCLOSE;
		ClearChecks(ActiveFoup);
		break;
	}
	if (State == s10ALLCLOSE) {
		m_cOpenFoup1.EnableWindow(TRUE);
		m_cOpenFoup2.EnableWindow(TRUE);
		EnableButton(FALSE);
	}
	ReleaseUI();
}

BOOL CSetupProp10Dlg::TransferCas2Stage(short slot, BOOL bVacOn) {
	int ID = ActiveFoup - 1;
	if ((ID < 0) || (slot < 1) || (slot > 25)) {
		return FALSE;
	}
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return FALSE;
	}
	C413Global* g = &p413App->Global;
	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
		AfxMessageBox("Move Stage to Load Position returned ERROR. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (!IsStageInLoadPosition(g->LoadPos.PX, g->LoadPos.PY)) {
		AfxMessageBox("Stage is not at Load Position. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (p413App->CanTransfer2Stage(ID + 1, slot)) {
		g->WaferOnStage = ID + 1; // Temporary set it to true [10/7/2012 Yuen]
		DosUtil.SetWaferOnStage(g->WaferOnStage);
		pMtr->SetChuckVacuum(FALSE);
		if (pGDev->TransferWafer1(g->LPNo[ID], slot, 3, 1, TRUE)) {
			if (bVacOn) {
				pMtr->SetChuckVacuum(TRUE);
			}
			return TRUE;
		}
		else {
			pMtr->SetChuckVacuum(FALSE);
		}
	}
	else {
		AfxMessageBox("One wafer maybe on stage");
	}
	return FALSE;
}

BOOL CSetupProp10Dlg::TransferStage2Cas(short slot) {
	int ID = ActiveFoup - 1;
	if (ID < 0) {
		return FALSE;
	}
	C413Global* g = &p413App->Global;
	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
		AfxMessageBox("Move Stage to Load Position returned ERROR. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (!IsStageInLoadPosition(g->LoadPos.PX, g->LoadPos.PY)) {
		AfxMessageBox("Stage is not at Load Position. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (p413App->CanTransfer2Cas(ID + 1, slot)) {
		CMotorS2* pMtr = p413App->pMtr;
		if (pMtr) {
			pMtr->SetChuckVacuum(FALSE);
			if (pGDev->TransferWafer(3, 1, g->LPNo[ID] + 1, slot, FALSE)) {
				pMtr->SetChuckVacuum(FALSE);
				g->WaferOnStage = 0;	// No more wafer on stage [10/7/2012 Yuen]
				DosUtil.SetWaferOnStage(g->WaferOnStage);
				DosUtil.ForgetPrJob(g->LPNo[ID] + 1);
				return TRUE;
			}
			else {
				p413App->Log("Transfer failed");
				pMtr->SetChuckVacuum(FALSE);
			}
		}
	}
	else {
		AfxMessageBox("No wafer on stage");
	}
	p413App->Log("Cannot transfer");
	return FALSE;
}

void CSetupProp10Dlg::OnGotoloadpos() {
	if (bResetting) {
		AfxMessageBox("Cannot perform.\nResetting");
		return;
	}
	if (bTransferInProgress) {
		AfxMessageBox("Can not perform\nTransfer in progress");
		return;
	}

	m_cGotoOrigin.EnableWindow(FALSE);
	m_cResetXY.EnableWindow(FALSE);
	C413Global* g = &p413App->Global;
	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) // 20140223 SCOTT ADD : CHECK RETURN VALUE
	{
		p413App->StageMoveError();
	}
	m_cGotoOrigin.EnableWindow(TRUE);
	m_cResetXY.EnableWindow(TRUE);
}

void CSetupProp10Dlg::OnLslot1() {
	SetSlot(1, 1);
}

void CSetupProp10Dlg::OnRslot2() {
	SetSlot(2, 2);
}

void CSetupProp10Dlg::OnRslot3() {
	SetSlot(2, 3);
}

void CSetupProp10Dlg::OnRslot4() {
	SetSlot(2, 4);
}

void CSetupProp10Dlg::OnRslot5() {
	SetSlot(2, 5);
}

void CSetupProp10Dlg::OnRslot6() {
	SetSlot(2, 6);
}

void CSetupProp10Dlg::OnRslot7() {
	SetSlot(2, 7);
}

void CSetupProp10Dlg::OnRslot8() {
	SetSlot(2, 8);
}

void CSetupProp10Dlg::OnRslot9() {
	SetSlot(2, 9);
}

void CSetupProp10Dlg::OnRslot10() {
	SetSlot(2, 10);
}

void CSetupProp10Dlg::OnRslot11() {
	SetSlot(2, 11);
}

void CSetupProp10Dlg::OnRslot12() {
	SetSlot(2, 12);
}

void CSetupProp10Dlg::OnRslot13() {
	SetSlot(2, 13);
}

void CSetupProp10Dlg::OnRslot14() {
	SetSlot(2, 14);
}

void CSetupProp10Dlg::OnRslot15() {
	SetSlot(2, 15);
}

void CSetupProp10Dlg::OnRslot16() {
	SetSlot(2, 16);
}

void CSetupProp10Dlg::OnRslot17() {
	SetSlot(2, 17);
}

void CSetupProp10Dlg::OnRslot18() {
	SetSlot(2, 18);
}

void CSetupProp10Dlg::OnRslot19() {
	SetSlot(2, 19);
}

void CSetupProp10Dlg::OnRslot20() {
	SetSlot(2, 20);
}

void CSetupProp10Dlg::OnRslot21() {
	SetSlot(2, 21);
}

void CSetupProp10Dlg::OnRslot22() {
	SetSlot(2, 22);
}

void CSetupProp10Dlg::OnRslot23() {
	SetSlot(2, 23);
}

void CSetupProp10Dlg::OnRslot24() {
	SetSlot(2, 24);
}

void CSetupProp10Dlg::OnRslot25() {
	SetSlot(2, 25);
}

void CSetupProp10Dlg::OnLslot10() {
	SetSlot(1, 10);
}

void CSetupProp10Dlg::OnLslot11() {
	SetSlot(1, 11);
}

void CSetupProp10Dlg::OnLslot12() {
	SetSlot(1, 12);
}

void CSetupProp10Dlg::OnLslot13() {
	SetSlot(1, 13);
}

void CSetupProp10Dlg::OnLslot14() {
	SetSlot(1, 14);
}

void CSetupProp10Dlg::OnLslot15() {
	SetSlot(1, 15);
}

void CSetupProp10Dlg::OnLslot16() {
	SetSlot(1, 16);
}

void CSetupProp10Dlg::OnLslot17() {
	SetSlot(1, 17);
}

void CSetupProp10Dlg::OnLslot18() {
	SetSlot(1, 18);
}

void CSetupProp10Dlg::OnLslot19() {
	SetSlot(1, 19);
}

void CSetupProp10Dlg::OnLslot2() {
	SetSlot(1, 2);
}

void CSetupProp10Dlg::OnLslot20() {
	SetSlot(1, 20);
}

void CSetupProp10Dlg::OnLslot21() {
	SetSlot(1, 21);
}

void CSetupProp10Dlg::OnLslot22() {
	SetSlot(1, 22);
}

void CSetupProp10Dlg::OnLslot23() {
	SetSlot(1, 23);
}

void CSetupProp10Dlg::OnLslot24() {
	SetSlot(1, 24);
}

void CSetupProp10Dlg::OnLslot25() {
	SetSlot(1, 25);
}

void CSetupProp10Dlg::OnLslot3() {
	SetSlot(1, 3);
}

void CSetupProp10Dlg::OnLslot4() {
	SetSlot(1, 4);
}

void CSetupProp10Dlg::OnLslot5() {
	SetSlot(1, 5);
}

void CSetupProp10Dlg::OnLslot6() {
	SetSlot(1, 6);
}

void CSetupProp10Dlg::OnLslot7() {
	SetSlot(1, 7);
}

void CSetupProp10Dlg::OnLslot8() {
	SetSlot(1, 8);
}

void CSetupProp10Dlg::OnLslot9() {
	SetSlot(1, 9);
}

void CSetupProp10Dlg::OnRslot1() {
	SetSlot(2, 1);
}

void CSetupProp10Dlg::SetSlot(short port, short slot) {
	if (State != s10FOUPOPENED) {
		return;
	}
	if (port != ActiveFoup) {
		return;
	}
	switch (port) {
	case 1:
		SLP1 = slot;
		break;
	case 2:
		SLP2 = slot;
		break;
	}
}

void CSetupProp10Dlg::OnResetxy() {
	if (bTransferInProgress) {
		AfxMessageBox("Can not perform\nTransfer in progress");
		return;
	}
	BlockUI();
	bResetting = TRUE;
	m_cResetXY.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->ResetXY();
	m_cResetXY.EnableWindow(TRUE);
	bResetting = FALSE;
	ReleaseUI();
}

void CSetupProp10Dlg::OnKillfocusWaferrepeat2() {
	UpdateData(TRUE);
}

BOOL CSetupProp10Dlg::TransferToStage() {
	BlockUI();
	bTransferInProgress = TRUE;
	m_cOpenFoup1.EnableWindow(FALSE);
	m_cOpenFoup2.EnableWindow(FALSE);
	EnableButton(FALSE);
	m_cGotoOrigin.EnableWindow(FALSE);
	m_cResetXY.EnableWindow(FALSE);

	BOOL bRet = FALSE;
	switch (ActiveFoup) {
	case 1:
		bRet = TransferCas2Stage(SLP1, FALSE);
		if (bRet) {
			SlotMap.SetAt(SLP1 - 1, '1');
			EnableSlots(ActiveFoup, SlotMap, '9', SLP1);
		}
		break;
	case 2:
		bRet = TransferCas2Stage(SLP2, FALSE);
		if (bRet) {
			SlotMap.SetAt(SLP2 - 1, '1');
			EnableSlots(ActiveFoup, SlotMap, '9', SLP2);
		}
		break;
	}

	bTransferInProgress = FALSE;
	if (bRet) {
		if (p413App->pMtr) {
			p413App->pMtr->SetChuckVacuum(TRUE);
		}
		State = s10WAFERLOADED;
	}
	else {
		bTransferInProgress = FALSE;
		m_cOpenFoup1.EnableWindow(TRUE);
		m_cOpenFoup2.EnableWindow(TRUE);
	}

	m_cGotoOrigin.EnableWindow(TRUE);
	m_cResetXY.EnableWindow(TRUE);
	EnableButton(TRUE);
	ReleaseUI();
	return bRet;
}

BOOL CSetupProp10Dlg::TransferToCassette() {
	switch (ActiveFoup) {
	case 1:
		if ((SLP1 < 1) || (SLP1 > 25)) {
			return FALSE;
		}
		break;
	default:
		if ((SLP2 < 1) || (SLP2 > 25)) {
			return FALSE;
		}
		break;
	}
	BlockUI();
	bTransferInProgress = TRUE;
	EnableButton(FALSE);
	m_cGotoOrigin.EnableWindow(FALSE);
	m_cResetXY.EnableWindow(FALSE);

	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(FALSE);
	}

	BOOL bRet = FALSE;
	switch (ActiveFoup) {
	case 1:
		bRet = TransferStage2Cas(SLP1);
		if (bRet) {
			SlotMap.SetAt(SLP1 - 1, '3');
			EnableSlots(ActiveFoup, SlotMap, '3', SLP1);
		}
		m_cOpenFoup1.EnableWindow(TRUE);
		break;
	case 2:
		bRet = TransferStage2Cas(SLP2);
		if (bRet) {
			SlotMap.SetAt(SLP2 - 1, '3');
			EnableSlots(ActiveFoup, SlotMap, '3', SLP2);
		}
		m_cOpenFoup2.EnableWindow(TRUE);
		break;
	}

	bTransferInProgress = FALSE;

	if (bRet) {
		EnableButton(TRUE);
		m_cGotoOrigin.EnableWindow(TRUE);
		m_cResetXY.EnableWindow(TRUE);
		State = s10FOUPOPENED;
	}
	else {
		EnableButton(TRUE);
		m_cGotoOrigin.EnableWindow(TRUE);
		m_cResetXY.EnableWindow(TRUE);
	}
	ReleaseUI();
	return bRet;
}

void CSetupProp10Dlg::OnTransfertostage() {
	
	if (bResetting) {
		AfxMessageBox("Cannot perform.\nResetting");
	}
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}//]

	if (TransferToStage()) {
		m_cTransfer2stage.EnableWindow(FALSE);
	}
}

void CSetupProp10Dlg::OnTransfertoloadport() {
	if (bResetting) {
		AfxMessageBox("Cannot perform.\nResetting");
		return;
	}
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}
	//]

	if (TransferToCassette()) {
		m_cTransfer2loadport.EnableWindow(FALSE);
	}
}

void CSetupProp10Dlg::OnKillfocusCassetterepeat() {
	UpdateData(TRUE);
}

void CSetupProp10Dlg::OnRelease() {
	if (bRelease) {
		bRelease = FALSE;
	}
	else {
		bRelease = TRUE;
	}
	if (bRelease) {
		m_cOpenFoup1.EnableWindow(TRUE);
		m_cOpenFoup2.EnableWindow(TRUE);
		EnableButton(TRUE);
	}
	else {
	}
}

void CSetupProp10Dlg::OnInitialize() {
	// Sense load port 1 & 2 open status [6/17/2012 FSM 413 C2C]

	// Sense wafer present on stage [6/17/2012 FSM 413 C2C]
}

void CSetupProp10Dlg::OnComClosenoreleasecmpl() {
	OnComEndclose();
}

void CSetupProp10Dlg::OnBnClickedUnclamp() {
	Unclamp(1);
}

void CSetupProp10Dlg::OnBnClickedUnclamp3() {
	Unclamp(2);
}

void CSetupProp10Dlg::Unclamp(short Port) {
	if (!pGDev->pLPMgr) {
		return;
	}
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = Port;
	SM.Pm.nunit = Port;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CSetupProp10Dlg::OnUnclamp() {
	if (!pGDev->pLPMgr) {
		return;
	}
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_UNCLAMPOK;
	SM.Nak = ID_LP_UNCLAMPFAIL;
	SM.Pm.unit = 1;
	SM.Pm.nunit = 1;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
		return;
	}
}

void CSetupProp10Dlg::OnUnclamp3() {
	if (!pGDev->pLPMgr) {
		return;
	}
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_UNCLAMPOK;
	SM.Nak = ID_LP_UNCLAMPFAIL;
	SM.Pm.unit = 2;
	SM.Pm.nunit = 2;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
		return;
	}
}

BOOL CSetupProp10Dlg::DestroyWindow() {
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp10Dlg::OnClearwinter() {
	if (pUDev->pTitlePanel->AuthenticateLevel != CLogInDataNew::FULLACC) {
		pUDev->pTitlePanel->AskLogin();
	}

	if (AfxMessageBox("Warning: Protection will be released.\nAre you sure?", MB_YESNO) == IDYES) {
		if (pUDev->pTitlePanel->AuthenticateLevel == CLogInDataNew::FULLACC) {
			DosUtil.ForgetPrJob(1);
			DosUtil.ForgetPrJob(2);
			p413App->Global.WaferOnStage = FALSE;
			DosUtil.SetWaferOnStage(p413App->Global.WaferOnStage);
			AfxMessageBox("Done");
		}
	}
}

void CSetupProp10Dlg::EnableButton(BOOL bEna) {
	m_cTransfer2loadport.EnableWindow(bEna);
	m_cTransfer2stage.EnableWindow(bEna);
	if (g->WaferOnStage) m_cAlgn.EnableWindow(TRUE);
	else m_cAlgn.EnableWindow(FALSE);
	m_cUnclamp1.EnableWindow(!bEna);
	m_cUnclamp2.EnableWindow(!bEna);
}

LRESULT CSetupProp10Dlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	SlotMap = (char*)(lP);

	if ((SlotMap.Find('2') != -1) ||
		(SlotMap.Find('4') != -1) ||
		(SlotMap.Find('5') != -1)) {
		OnComSlotMapFail(wP, lP);
		return 0;
	}

	CString txt;
	switch (wP) {
	case 1:
		m_cOpenFoup1.GetWindowText(txt);
		if (txt == "Close") {
			EnableSlots(wP, SlotMap, '3', 0);
			if (g->WaferOnStage) {
				OnInvert();
			}
		}
		break;
	case 2:
		m_cOpenFoup2.GetWindowText(txt);
		if (txt == "Close") {
			EnableSlots(wP, SlotMap, '3', 0);
			if (g->WaferOnStage) {
				OnInvert();
			}
		}
		break;
	}
	return 0;
}

LRESULT CSetupProp10Dlg::OnComSlotMapFail(WPARAM wP, LPARAM lP) {
	return 0;
}

void CSetupProp10Dlg::EnableSlots(short unit, CString slotmap, char bEn, short slt) {
	int i;
	CString str;
	//[[20240820/Arup
	//p413App->Global.LoadMaxSlot();
	int nSlot = p413App->Global.SlotsPerFOUP;
	if(nSlot == 25){
		switch (unit) {
		case 1:
			for (i = 0; i < 25; i++) {
				char ch = slotmap.GetAt(i);
				if ((ch == bEn) || (slt == i + 1)) {
					((CButton*)GetDlgItem(LRID[i]))->EnableWindow(TRUE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
					if (slt == i + 1) {
						str.Format("Slot %2d %s", i + 1, ch == '1' ? "Empty" : "Occupied");
					}
				}
				else {
					((CButton*)GetDlgItem(LRID[i]))->EnableWindow(FALSE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
				}
				((CButton*)GetDlgItem(LRID[i]))->SetWindowText(str);
			}
			break;
		case 2:
			for (i = 0; i < 25; i++) {
				char ch = slotmap.GetAt(i);
				if ((ch == bEn) || (slt == i + 1)) {
					((CButton*)GetDlgItem(RRID[i]))->EnableWindow(TRUE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
					if (slt == i + 1) {
						str.Format("Slot %2d %s", i + 1, ch == '1' ? "Empty" : "Occupied");
					}
				}
				else {
					((CButton*)GetDlgItem(RRID[i]))->EnableWindow(FALSE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
				}
				((CButton*)GetDlgItem(RRID[i]))->SetWindowText(str);
			}

			break;
		}

	}
	//[20243008/Masum
	else if (nSlot == 13)
	{

		switch (unit) {
		case 1:
			for (i = 0; i < 13; i++) {
				/*if (i != 0)
				{
					if (i % 2 != 0) continue;
				}*/
				char ch = slotmap.GetAt(i);
				if ((ch == bEn) || (slt == i + 1)) {
					((CButton*)GetDlgItem(LRID[i]))->EnableWindow(TRUE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
					if (slt == i + 1) {
						str.Format("Slot %2d %s", i + 1, ch == '1' ? "Empty" : "Occupied");
					}
				}
				else {
					((CButton*)GetDlgItem(LRID[i]))->EnableWindow(FALSE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
				}
				((CButton*)GetDlgItem(LRID[i]))->SetWindowText(str);
			}
			break;
		case 2:
			for (i = 0; i < 13; i++) {
				/*if (i != 0)
				{
					if (i % 2 != 0) continue;
				}*/
				char ch = slotmap.GetAt(i);
				if ((ch == bEn) || (slt == i + 1)) {
					((CButton*)GetDlgItem(RRID[i]))->EnableWindow(TRUE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
					if (slt == i + 1) {
						str.Format("Slot %2d %s", i + 1, ch == '1' ? "Empty" : "Occupied");
					}
				}
				else {
					((CButton*)GetDlgItem(RRID[i]))->EnableWindow(FALSE);
					str.Format("Slot %2d %s", i + 1, ch == '3' ? "Occupied" : "Empty");
				}
				((CButton*)GetDlgItem(RRID[i]))->SetWindowText(str);
			}

			break;
		}
	}

}

void CSetupProp10Dlg::ClearAllSlots(BOOL bEn) {
	int i;
	CString str;
	for (i = 0; i < 25; i++) {
		str.Format("slot %d", i + 1);
		((CButton*)GetDlgItem(LRID[i]))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(LRID[i]))->SetWindowText(str);
		((CButton*)GetDlgItem(RRID[i]))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(RRID[i]))->SetWindowText(str);
	}
}

void CSetupProp10Dlg::OnInvert() {
	EnableSlots(ActiveFoup, SlotMap, '1', 0);
}

void CSetupProp10Dlg::OnRevert() {
	EnableSlots(ActiveFoup, SlotMap, '3', 0);
}

void CSetupProp10Dlg::OnFindcxcy() {
	float CX, CY, CR;

	BlockUI();
	m_cAlgn.EnableWindow(FALSE);
	m_cImgWnd.ClearTextInfo();
	BOOL bFE = FALSE;
	SFINDCENTER SFC;
	SFC.pCXOffset = &CX;
	SFC.pCYOffset = &CY;
	SFC.pCRadius = &CR;
	SFC.pRcp = pRcp;
	SFC.pVideo = &m_cVideo;
	SFC.pImgWnd = &m_cImgWnd;
	SFC.pList = NULL;
	SFC.Method = pRcp->FindEdgeType;
	switch (pRcp->FindEdgeMethod) {
	case 2:
		bFE = p413App->FindWaferCenterUsingEdgeDetectionFast(SFC);
		break;
	default:
		bFE = p413App->FindWaferCenterUsingEdgeDetection(SFC);
		break;
	}
	if (!bFE) {
		m_cAlgn.EnableWindow(TRUE);
		return;
	}
	// 	if (p413App->FindWaferCenterUsingEdgeDetection(CX,CY,CR,pRcp,&m_cVideo,&m_cImgWnd,NULL))
	// 	{
	// 	}
	m_cAlgn.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp10Dlg::ClearChecks(short unit) {
	int i;
	switch (unit) {
	case 1:
		for (i = 0; i < 25; i++) {
			((CButton*)GetDlgItem(LRID[i]))->SetCheck(FALSE);
		}
		break;
	default:
		for (i = 0; i < 25; i++) {
			((CButton*)GetDlgItem(RRID[i]))->SetCheck(FALSE);
		}
		break;
	}
}

void CSetupProp10Dlg::SetCheck(short unit, short slt) {
	switch (unit) {
	case 1:
		((CButton*)GetDlgItem(LRID[slt - 1]))->SetCheck(FALSE);
		break;
	default:
		((CButton*)GetDlgItem(RRID[slt - 1]))->SetCheck(FALSE);
		break;
	}
}

BOOL CSetupProp10Dlg::IsStageInLoadPosition(float fX, float fY) {
	BOOL result = FALSE;

	CMotorS2* pMtr = p413App->pMtr;
	float xPos;
	float yPos;
	pMtr->GetXYPos(&xPos, &yPos);
	if ((fabs(xPos - fX) < 1.0f) && (fabs(yPos - fY) < 1.0f))
		result = TRUE;
	else
		result = FALSE;
	return result;
}

// [5/27/2020 Zhiming]
UINT CSetupProp10Dlg::StartDryRunProc(LPVOID p) {
	CSetupProp10Dlg* d = (CSetupProp10Dlg*)p;

	// open FOUP
	d->OpenFoup(1);

	while (!d->bDryRunEnd) {
		// transfer wafer load port to stage
		if (d->SLP1 == 0) {
			AfxMessageBox("Slot Number is 0");
			d->bDryRunEnd = TRUE;
			continue;
		}
		d->TransferCas2Stage(d->SLP1, FALSE);
		if (d->bDryRunEnd == TRUE) continue;

		// go to origin
		p413App->pMtr->GotoXY(0, 0);
		if (d->bDryRunEnd == TRUE) continue;

		p413App->pMtr->GotoXY(p413App->Global.DPPos.X, p413App->Global.DPPos.Y);

		p413App->pMtr->GotoXY(p413App->Global.TFPos.X, p413App->Global.TFPos.Y);

		// transfer wafer stage to load port
		if (d->SLP1 == 0) {
			AfxMessageBox("Slot Number is 0");
			d->bDryRunEnd = TRUE;
			continue;
		}
		d->TransferStage2Cas(d->SLP1);
		if (d->bDryRunEnd == TRUE) continue;

		// go origin
		p413App->GotoXYS(0, 0, CCamBase::ECHOCAM);
	}

	// close FOUP
	d->CloseFoup(1);

	return 1;
}

void CSetupProp10Dlg::OnDryrun() {
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}
	//]
	if (bDryRunEnd == TRUE) {
		DWORD dwid = 0;

		bDryRunEnd = FALSE;

		hDryRun = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartDryRunProc, this, 0, &dwid);
		m_cDryRun.SetWindowText("Stop");
	}
	else {
		bDryRunEnd = TRUE;

		m_cDryRun.SetWindowText("Run");
	}
}

//[ Tushar /// testing purpose //02092024
void CSetupProp10Dlg::OnBnClickedLpnum()
{
	m_LPNum.GetWindowText(sLPNum);
	iLPNum = _ttoi(sLPNum);

	if (iLPNum == 1) {
		OnOpenfoup1();
	}
	else if (iLPNum == 2) {
		OnOpenfoup2();
	}
	
	return;

}


void CSetupProp10Dlg::OnBnClickedTestrun()
{	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}//]
	if (!bRun) {
		m_SlotNums.GetWindowText(sSlotNums);
		slots.clear();
		string SlotNum(sSlotNums);
		string tmp = "";
		for (int i = 0; i < SlotNum.size();i++)
		{
			if (SlotNum[i] == ',') {
				slots.push_back(stoi(tmp));
				tmp = "";
			}
			else tmp += SlotNum[i];
		}
		if (tmp.size())
			slots.push_back(stoi(tmp));

		// input done
		bRun = true;
		AfxBeginThread(TransferPrep, this);
	}
	else {
		bStop = true;
		while (bRun) {
			p413App->Yld(20);
		}
		bStop = false;
	}
}

UINT CSetupProp10Dlg::TransferPrep(LPVOID pdlg) {

	CSetupProp10Dlg* pd = (CSetupProp10Dlg*)pdlg;
	BOOL bRet = FALSE;
	short slt;

	int i = 0;
	int cycle_count = 0;
	CString str;
	while (!pd->bStop && i < pd->slots.size())
	{
		str.Format(_T("Repeat #%d"), cycle_count);
		p413App->Log(str);
		slt = pd->slots[i];
		pd->SetSlot(pd->iLPNum, slt);
		//TransferFull(slt, FALSE);

		str.Format(_T("Transferring LP #%d slot %d to stage"), pd->iLPNum, slt);
		p413App->Log(str);

		if (pd->bStop) break;

		pd->TransferToStage();

		if (pd->bStop) break;

		Sleep(1000);

		str.Format(_T("Transferring stage to LP #%d slot %d"), pd->iLPNum, slt);
		p413App->Log(str);
		pd->TransferToCassette();

		if (pd->bStop) break;
		
		i++;
		if (i == pd->slots.size())
		{
			cycle_count++;
			i = 0;
		}

	}
	pd->bRun = false;
	return TRUE;
}

BOOL CSetupProp10Dlg::TransferFull(short slot, BOOL bVacOn) {
	int ID = iLPNum - 1;
	if ((ID < 0) || (slot < 1) || (slot > 25)) {
		return FALSE;
	}
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return FALSE;
	}
	C413Global* g = &p413App->Global;
	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
		AfxMessageBox("Move Stage to Load Position returned ERROR. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (!IsStageInLoadPosition(g->LoadPos.PX, g->LoadPos.PY)) {
		AfxMessageBox("Stage is not at Load Position. Please press ResetXY first, then Press Goto Load Position");
		return FALSE;
	}
	if (p413App->CanTransfer2Stage(ID + 1, slot)) {
		g->WaferOnStage = ID + 1; // Temporary set it to true [10/7/2012 Yuen]
		DosUtil.SetWaferOnStage(g->WaferOnStage);
		pMtr->SetChuckVacuum(FALSE);
		if (pGDev->TransferTest(g->LPNo[ID], slot, 3, 1, TRUE)) {
			SlotMap.SetAt(slot - 1, '1');
			EnableSlots(iLPNum, SlotMap, '9', slot);
			State = s10WAFERLOADED;
			/*if (bVacOn) {
				pMtr->SetChuckVacuum(TRUE);
			}*/
			//return TRUE;
			pMtr->SetChuckVacuum(FALSE);
			if (pGDev->TransferTest2(3, 1, g->LPNo[ID], slot, TRUE)) {
				SlotMap.SetAt(slot - 1, '3');
				EnableSlots(ActiveFoup, SlotMap, '3', slot);
				State = s10FOUPOPENED;
				return TRUE;

			}
		}
		else {
			pMtr->SetChuckVacuum(FALSE);
		}
	}
	else {
		AfxMessageBox("One wafer maybe on stage");
	}
	return FALSE;
}
//]

void CSetupProp10Dlg::OnBnClickedResponse()
{
	//pGDev->pRbtMgr->pRbtSvr->HomeRobot(1);
	//[//09092024
	if (g->CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}
	//]
}


void CSetupProp10Dlg::OnBnClickedPresence()
{
	CString str = _T("");
	// robot arm wafer present check
	str.AppendFormat(_T("RobotArm: Wafer is %s\n"), pGDev->pRbtMgr->IsWaferOnArm() ? _T("Present") : _T("Not Present"));

	// aligner wafer present check
	str.AppendFormat(_T("Aligner: Wafer is %s\n"), pGDev->pRbtMgr->IsWaferOnAligner() ? _T("Present") : _T("Not Present"));

	// stage wafer present check
	bool isWaferPresent = false;
	p413App->WaferOnStageCheck(pRcp, &isWaferPresent, &m_cVideo);
	str.AppendFormat(_T("Stage: Wafer is %s\n"), isWaferPresent ? _T("Present") : _T("Not Present"));
	AfxMessageBox(str);
}
