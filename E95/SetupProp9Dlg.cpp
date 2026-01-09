#include "stdafx.h"
#include "e95.h"

#include "413/MotorS2.h"
#include "413App.h"
#include "Configuration.h"
#include "FEM/IDR/IDRMgr.h"
#include "FEM/IDR/IDRSvr.h"
#include "FEM/IDR/IDRUnit.h"
#include "FEM/Ldp/LPMgr.h"
#include "FEM/Ldp/LPSvr.h"
#include "FEM/Rbt/RbtBase.h"
#include "FEM/Rbt/RbtMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "SelectVCamera.h"
#include "SYS/CUMMsg.h"
#include "XTabCtrl.h"

#include "SetupProp9Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp9Dlg dialog

CSetupProp9Dlg::CSetupProp9Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp9Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp9Dlg)
	m_EchoTopZPos = 0.0f;
	m_ThinFilmZPos = 0.0f;
	m_TFScalingFactor = 0.0f;
	m_EchoAESteps = 0;
	m_TFilmAESteps = 0;
	m_TFilmAFSteps = 0.0f;
	m_EchoAFSteps = 0.0f;
	m_ScaleEcho = 0.0f;
	m_ScaleTF = 0.0f;
	m_Count = 0;
	m_BaseExpoEcho = 0;
	m_BaseExpoTF = 0;
	m_ResetFreq = 0.0f;
	m_LevelTop = 0;
	m_LevelBot = 0;
	m_WidthTop = 0;
	m_WidthBot = 0;
	m_GainEchoCam = 0;
	m_GainThinFCam = 0;
	m_OffsetEchoCam = 0;
	m_OffsetThinFCam = 0;
	//}}AFX_DATA_INIT
	m_FFUPressure = 0.0f;
	g = NULL;
	pRcp = NULL;
	m_cVideo.Name = "Setup9";
}

void CSetupProp9Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp9Dlg)
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_WAFERPRESEMT, m_cWaferPresent);
	DDX_Control(pDX, IDC_CURFFUPRESSURE, m_cCurFFUPress);
	DDX_Control(pDX, IDC_FFUPRESSURE, m_cFFUPress);
	DDX_Control(pDX, IDC_SPECTROMETERPOWON, m_cSpecPowerOn);
	DDX_Control(pDX, IDC_SPECTROMETERPOWOFF, m_cSpecPowerOff);
	DDX_Control(pDX, IDC_SPECTROMETERILLUMON, m_cSpecIllumOn);
	DDX_Control(pDX, IDC_SPECTROMETERILLUMOFF, m_cSpecIllumOff);
	DDX_Control(pDX, IDC_CONFIGURATION, m_cConfiguration);
	DDX_Control(pDX, IDC_ZPOS2, m_cThinFilmZPos);
	DDX_Control(pDX, IDC_ZPOS1, m_cEchoTopZPos);
	DDX_Control(pDX, IDC_RECZPOS2, m_cRecPos2);
	DDX_Control(pDX, IDC_RECZPOS1, m_cRecPos1);
	DDX_Control(pDX, IDC_MICROSCOPELIGHTON, m_cMicroscopeLightOn);
	DDX_Control(pDX, IDC_SPECTROMETERLIGHTON, m_cSpectroLightOn);
	DDX_Control(pDX, IDC_SPECTROMETERLIGHTOFF, m_cSpectroLightOff);
	DDX_Control(pDX, IDC_MICROSCOPELIGHTOFF, m_cMicroscopeLightOff);
	DDX_Control(pDX, IDC_REVEERSESIGNAL, m_cReverseSignal);
	DDX_Control(pDX, IDC_REVEERSEAB, m_cReverseAB);
	DDX_Control(pDX, IDC_ECHOMOTOR, m_cEchoMotor);
	DDX_Control(pDX, IDC_LOADPORT2, m_cLoadPort2);
	DDX_Control(pDX, IDC_CIDR2, m_cCIDR2);
	DDX_Control(pDX, IDC_CIDR, m_cCIDR);
	DDX_Control(pDX, IDC_STAGE, m_cStage);
	DDX_Control(pDX, IDC_THINFILM, m_cThinFilm);
	DDX_Control(pDX, IDC_ROBOT, m_cRobot);
	DDX_Control(pDX, IDC_LOADPORT, m_cLoadPort);
	DDX_Control(pDX, IDC_ECHOPROBE, m_cEchoProbe);
	DDX_Text(pDX, IDC_ZPOS1, m_EchoTopZPos);
	DDX_Text(pDX, IDC_ZPOS2, m_ThinFilmZPos);
	DDX_Text(pDX, IDC_ECHOAESTEPS, m_EchoAESteps);
	DDX_Text(pDX, IDC_TFAESTEPS, m_TFilmAESteps);
	DDX_Text(pDX, IDC_TFILMAFSTEPS, m_TFilmAFSteps);
	DDX_Text(pDX, IDC_ECHOAFSTEPS, m_EchoAFSteps);
	DDX_Text(pDX, IDC_SCALEECHO, m_ScaleEcho);
	DDX_Text(pDX, IDC_SCALETF, m_ScaleTF);
	DDX_Text(pDX, IDC_EXPCOUNT, m_Count);
	DDX_Text(pDX, IDC_BASEEXPECHO, m_BaseExpoEcho);
	DDX_Text(pDX, IDC_BASEEXPTHINF, m_BaseExpoTF);
	DDX_Text(pDX, IDC_RESETFREQ, m_ResetFreq);
	DDX_Text(pDX, IDC_LEVELOFFTOP, m_LevelTop);
	DDX_Text(pDX, IDC_LEVELOFFBOT, m_LevelBot);
	DDX_Text(pDX, IDC_WIDTHOFFTOP, m_WidthTop);
	DDX_Text(pDX, IDC_WIDTHOFFBOT, m_WidthBot);
	DDX_Text(pDX, IDC_GAINECHOCAM, m_GainEchoCam);
	DDX_Text(pDX, IDC_GAINTHINFCAM, m_GainThinFCam);
	DDX_Text(pDX, IDC_OFFSETECHOCAM, m_OffsetEchoCam);
	DDX_Text(pDX, IDC_OFFSETTHINFCAM, m_OffsetThinFCam);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FFUPRESSURE, m_FFUPressure);
	DDX_Control(pDX, IDC_LP_AUTO_START_LATCHING, m_AutoStartLatching);
	DDX_Control(pDX, IDC_SYSSETTING_STAGE_CHECK_WAFER_PRESENCE, m_StageCheckWaferPresent);
}

BEGIN_MESSAGE_MAP(CSetupProp9Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp9Dlg)
	ON_BN_CLICKED(IDC_CHUCKVACON, OnChuckvacon)
	ON_BN_CLICKED(IDC_CHUCKVACOFF, OnChuckvacoff)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTON, OnMicroscopelighton)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTOFF, OnMicroscopelightoff)
	ON_BN_CLICKED(IDC_SPECTROMETERLIGHTON, OnSpectrometerlighton)
	ON_BN_CLICKED(IDC_SPECTROMETERLIGHTOFF, OnSpectrometerlightoff)
	ON_BN_CLICKED(IDC_ECHOPROBE, OnEchoprobe)
	ON_BN_CLICKED(IDC_THINFILM, OnThinfilm)
	ON_BN_CLICKED(IDC_STAGE, OnStage)
	ON_BN_CLICKED(IDC_ROBOT, OnRobot)
	ON_BN_CLICKED(IDC_LOADPORT, OnLoadport)
	ON_BN_CLICKED(IDC_CIDR, OnCidr)
	ON_BN_CLICKED(IDC_CIDR2, OnCidr2)
	ON_BN_CLICKED(IDC_ECHOMOTOR, OnEchomotor)
	ON_BN_CLICKED(IDC_LOADPORT2, OnLoadport2)
	ON_BN_CLICKED(IDC_REVEERSEAB, OnReveerseab)
	ON_BN_CLICKED(IDC_REVEERSESIGNAL, OnReveersesignal)
	ON_EN_KILLFOCUS(IDC_ZPOS1, OnKillfocusZpos1)
	ON_EN_KILLFOCUS(IDC_ZPOS2, OnKillfocusZpos2)
	ON_BN_CLICKED(IDC_RECZPOS1, OnReczpos1)
	ON_BN_CLICKED(IDC_RECZPOS2, OnReczpos2)
	ON_BN_CLICKED(IDC_CONFIGURATION, OnConfiguration)
	ON_BN_CLICKED(IDC_SPECTROMETERILLUMON, OnSpectrometerillumon)
	ON_BN_CLICKED(IDC_SPECTROMETERILLUMOFF, OnSpectrometerillumoff)
	ON_BN_CLICKED(IDC_SPECTROMETERPOWON, OnSpectrometerpowon)
	ON_BN_CLICKED(IDC_SPECTROMETERPOWOFF, OnSpectrometerpowoff)
	ON_BN_CLICKED(IDC_FFUREAD, OnFfuread)
	ON_BN_CLICKED(IDC_SAVEMARKER, OnSavemarker)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_EN_KILLFOCUS(IDC_TFAESTEPS, OnKillfocusTfaesteps)
	ON_EN_KILLFOCUS(IDC_ECHOAESTEPS, OnKillfocusEchoaesteps)
	ON_EN_KILLFOCUS(IDC_TFILMAFSTEPS, OnKillfocusTfilmafsteps)
	ON_EN_KILLFOCUS(IDC_ECHOAFSTEPS, OnKillfocusEchoafsteps)
	ON_BN_CLICKED(IDC_RESETRND, OnResetrnd)
	ON_BN_CLICKED(IDC_MOTIONCONTROL6, OnMotioncontrol6)
	ON_BN_CLICKED(IDC_CAMERASETTING6, OnCamerasetting6)
	ON_EN_KILLFOCUS(IDC_SCALEECHO, OnKillfocusScaleecho)
	ON_EN_KILLFOCUS(IDC_SCALETF, OnKillfocusScaletf)
	ON_BN_CLICKED(IDC_VIDEOCAMERA, OnVideocamera)
	ON_BN_CLICKED(IDC_GETCOUNT, OnGetcount)
	ON_EN_KILLFOCUS(IDC_BASEEXPECHO, OnKillfocusBaseexpecho)
	ON_EN_KILLFOCUS(IDC_BASEEXPTHINF, OnKillfocusBaseexpthinf)
	ON_EN_KILLFOCUS(IDC_RESETFREQ, OnKillfocusResetfreq)
	ON_EN_KILLFOCUS(IDC_LEVELOFFBOT, OnKillfocusLeveloffbot)
	ON_EN_KILLFOCUS(IDC_LEVELOFFTOP, OnKillfocusLevelofftop)
	ON_EN_KILLFOCUS(IDC_WIDTHOFFTOP, OnKillfocusWidthofftop)
	ON_EN_KILLFOCUS(IDC_WIDTHOFFBOT, OnKillfocusWidthoffbot)
	ON_EN_KILLFOCUS(IDC_GAINECHOCAM, OnKillfocusGainechocam)
	ON_EN_KILLFOCUS(IDC_GAINTHINFCAM, OnKillfocusGainthinfcam)
	ON_EN_KILLFOCUS(IDC_OFFSETECHOCAM, OnKillfocusOffsetechocam)
	ON_EN_KILLFOCUS(IDC_OFFSETTHINFCAM, OnKillfocusOffsetthinfcam)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_REDXHAIR, OnRedXHair)
	ON_BN_CLICKED(IDC_LP_AUTO_START_LATCHING, &CSetupProp9Dlg::OnBnClickedLpAutoStartLatching)
	ON_BN_CLICKED(IDC_SYSSETTING_STAGE_CHECK_WAFER_PRESENCE, &CSetupProp9Dlg::OnBnClickedSyssettingStageCheckWaferPresence)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp9Dlg message handlers

long CSetupProp9Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			m_cVideo.pWnd = NULL;
			m_cVideo.RSpecClear();
			p413App->StopCamera(&m_cVideo, NULL);
		}

		LocalToRecipe();

		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp9Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		m_FFUPressure = g->FFUPressure;

		RecipeToLocal();

		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.SetRecipe(pRcp);
			m_cVideo.bShowCrossHair = TRUE;

			m_cVideo.pWnd = this;
			m_cVideo.bSetRedXHair = TRUE;
			m_cVideo.RSpecClear();
		}

		bTabSelected = TRUE;
	}

	return 0;
}

void CSetupProp9Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp9Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp9Dlg::OnChuckvacon() {
	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(TRUE);
		// 		CString str,str2;
		// 		Sleep(1000);
		// 		if (p413App->pMtr->IsWaferPresent())
		// 		{
		// 			str = "";
		// 		}
		// 		else
		// 		{
		// 			str = "Not ";
		// 		}
		// 		str2.Format("Wafer %sPresent On Chuck",str);
		// 		m_cWaferPresent.SetWindowText(str2);
	}
}

void CSetupProp9Dlg::OnChuckvacoff() {
	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(FALSE);
		m_cWaferPresent.SetWindowText("");
	}
}

void CSetupProp9Dlg::OnMicroscopelighton() {
	if (p413App->pMtr) p413App->pMtr->SetMicroscopeLight(TRUE);
}

void CSetupProp9Dlg::OnMicroscopelightoff() {
	if (p413App->pMtr) p413App->pMtr->SetMicroscopeLight(FALSE);
}

void CSetupProp9Dlg::OnSpectrometerlighton() {
	//	if (p413App->pMtr) p413App->pMtr->SetSpectroPower(TRUE);
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(TRUE);
}

void CSetupProp9Dlg::OnSpectrometerlightoff() {
	//	if (p413App->pMtr) p413App->pMtr->SetSpectroPower(FALSE);
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(FALSE);
}

BOOL CSetupProp9Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SPECTROMETERLIGHTON, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERLIGHTOFF, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTON, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTOFF, TOP_RIGHT);
	AddAnchor(IDC_CHUCKVACON, TOP_RIGHT);
	AddAnchor(IDC_CHUCKVACOFF, TOP_RIGHT);

	AddAnchor(IDC_SPECTROMETERILLUMON, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERILLUMOFF, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERPOWON, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERPOWOFF, TOP_RIGHT);
	AddAnchor(IDC_CONFIGURATION, BOTTOM_RIGHT);

	AddAnchor(IDC_CAMERASETTING6, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL6, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (g->Confg.bEchoMod) {
		m_cRecPos1.EnableWindow(TRUE);
		m_cEchoTopZPos.EnableWindow(TRUE);
		m_cEchoProbe.EnableWindow(TRUE);
		m_cEchoMotor.EnableWindow(TRUE);
		m_cReverseAB.EnableWindow(TRUE);
		m_cReverseSignal.EnableWindow(TRUE);
	}
	if (g->Confg.bThinFilmMod) {
		m_cRecPos2.EnableWindow(TRUE);
		m_cThinFilmZPos.EnableWindow(TRUE);
		m_cThinFilm.EnableWindow(TRUE);
		m_cSpectroLightOn.EnableWindow(TRUE);
		m_cSpectroLightOff.EnableWindow(TRUE);
		m_cSpecPowerOn.EnableWindow(TRUE);
		m_cSpecPowerOff.EnableWindow(TRUE);
		m_cSpecIllumOn.EnableWindow(TRUE);
		m_cSpecIllumOff.EnableWindow(TRUE);
	}
	if (g->Confg.bEchoMicroscopeMod || g->Confg.bThinFilmMicroscopeMod) {
		//		m_cVideoCam.EnableWindow(TRUE);
		m_cMicroscopeLightOn.EnableWindow(TRUE);
		m_cMicroscopeLightOff.EnableWindow(TRUE);
	}

	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		m_cRobot.EnableWindow(TRUE);
		if (Var.nLoadPort > 0) {
			m_cLoadPort.EnableWindow(TRUE);
			// [ 01172024 ZHIMING
			m_AutoStartLatching.EnableWindow(TRUE);
			// ]
		}
		if (Var.nLoadPort > 1) {
			m_cLoadPort2.EnableWindow(TRUE);
		}
		if (Var.nCIDR > 0) {
			m_cCIDR.EnableWindow(TRUE);
		}
		if (Var.nCIDR > 1) {
			m_cCIDR2.EnableWindow(TRUE);
		}
	}

	if (g->Confg.bAdmin) {
		m_cConfiguration.ShowWindow(SW_SHOW);
	}

	RecipeToLocal();

	// [02052024 ZHIMING
	if (g->Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp9Dlg::LocalToRecipe() {}

void CSetupProp9Dlg::RecipeToLocal() {
	m_EchoTopZPos = g->CalZPosEcho;
	m_ThinFilmZPos = g->CalZPosTF;

	m_EchoAESteps = g->AutoExposeStepEcho;
	m_TFilmAESteps = g->AutoExposeStepTF;

	m_EchoAFSteps = g->AutoFocusStepEcho;
	m_TFilmAFSteps = g->AutoFocusStepTF;

	m_ScaleEcho = g->ScaleEcho;
	m_ScaleTF = g->ScaleTF;

	m_BaseExpoEcho = g->BaseExpoEcho;
	m_BaseExpoTF = g->BaseExpoTF;

	m_ResetFreq = g->ResetFreq;

	m_WidthTop = g->PSWidthOff[0];
	m_WidthBot = g->PSWidthOff[1];

	m_LevelTop = g->PSLevelOff[0];
	m_LevelBot = g->PSLevelOff[1];

	m_GainEchoCam = g->GainEchoCam;
	m_GainThinFCam = g->GainThinFCam;
	m_OffsetEchoCam = g->OffsetEchoCam;
	m_OffsetThinFCam = g->OffsetThinFCam;

	UpdateData(FALSE);

	if (p413App->bSimuEcho) {
		m_cEchoProbe.SetCheck(TRUE);
	}
	else {
		m_cEchoProbe.SetCheck(FALSE);
	}
	if (p413App->bSimuDMC) {
		m_cEchoMotor.SetCheck(TRUE);
	}
	else {
		m_cEchoMotor.SetCheck(FALSE);
	}
	if (p413App->nSpectroType == 0) {
		m_cThinFilm.SetCheck(TRUE);
	}
	else {
		m_cThinFilm.SetCheck(FALSE);
	}
	// 	if (m_cVideo.IsSimu())
	// 	{
	// 		m_cVideoCam.SetCheck(TRUE);
	// 	}
	// 	else
	// 	{
	// 		m_cVideoCam.SetCheck(FALSE);
	// 	}
	if (p413App->bSimuStage) {
		m_cStage.SetCheck(TRUE);
	}
	else {
		m_cStage.SetCheck(FALSE);
	}

	if (pGDev->pRbtMgr) {
		CRbtBase* pRbtSvr = pGDev->pRbtMgr->pRbtSvr;
		if (pRbtSvr && pRbtSvr->bSimu) {
			m_cRobot.SetCheck(TRUE);
		}
		else {
			m_cRobot.SetCheck(FALSE);
		}
	}
	if (pGDev->pLPMgr) {
		CLPSvr* pLPSvr = pGDev->pLPMgr->pLPSvr[0];
		if (pLPSvr && pLPSvr->bSimu) {
			m_cLoadPort.SetCheck(TRUE);
		}
		else {
			m_cLoadPort.SetCheck(FALSE);
		}
		pLPSvr = pGDev->pLPMgr->pLPSvr[1];
		if (pLPSvr && pLPSvr->bSimu) {
			m_cLoadPort2.SetCheck(TRUE);
		}
		else {
			m_cLoadPort2.SetCheck(FALSE);
		}
	}

	if (pGDev->pIDRMgr) {
		CIDRSvr* pCIDRSvr = pGDev->pIDRMgr->pCIDRSvr[0];
		if (pCIDRSvr && pCIDRSvr->IDRUnit->bSimul) {
			m_cCIDR.SetCheck(TRUE);
		}
		else {
			m_cCIDR.SetCheck(FALSE);
		}
		pCIDRSvr = pGDev->pIDRMgr->pCIDRSvr[1];
		if (pCIDRSvr && pCIDRSvr->IDRUnit->bSimul) {
			m_cCIDR2.SetCheck(TRUE);
		}
		else {
			m_cCIDR2.SetCheck(FALSE);
		}
	}

	if (p413App->bReverseAB) {
		m_cReverseAB.SetCheck(TRUE);
	}
	else {
		m_cReverseAB.SetCheck(FALSE);
	}

	if (p413App->bReverseSignal) {
		m_cReverseSignal.SetCheck(TRUE);
	}
	else {
		m_cReverseSignal.SetCheck(FALSE);
	}

	// [04262022 ZHIMING
	if (pGDev->bAutoStartLatching) {
		m_AutoStartLatching.SetCheck(TRUE);
	}
	else {
		m_AutoStartLatching.SetCheck(FALSE);
	}
	// [03152024
	if (g->bCheckWaferPresence) {
		m_StageCheckWaferPresent.SetCheck(TRUE);
	}
	else {
		m_StageCheckWaferPresent.SetCheck(FALSE);
	}
	// ]
}

void CSetupProp9Dlg::OnEchoprobe() {
	if (p413App->bSimuEcho) {
		p413App->bSimuEcho = FALSE;
	}
	else {
		p413App->bSimuEcho = TRUE;
	}
	p413App->SaveEchoOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnThinfilm() {
	if (p413App->nSpectroType) {
		p413App->nSpectroType = 0;
	}
	else {
		p413App->nSpectroType = 3;
	}
	p413App->SaveThinFilmOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnStage() {
	if (p413App->bSimuStage) {
		p413App->bSimuStage = FALSE;
	}
	else {
		p413App->bSimuStage = TRUE;
	}
	p413App->SaveMotorOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnRobot() {
	CRbtBase* pRbtSvr = pGDev->pRbtMgr->pRbtSvr;
	if (pRbtSvr) {
		if (pRbtSvr->bSimu) {
			pRbtSvr->bSimu = FALSE;
		}
		else {
			pRbtSvr->bSimu = TRUE;
		}
		DosUtil.SetRbtUnitSimu(pRbtSvr->bSimu);
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnLoadport() {
	CLPSvr* pLPSvr = pGDev->pLPMgr->pLPSvr[0];
	if (pLPSvr) {
		if (pLPSvr->bSimu) {
			pLPSvr->bSimu = FALSE;
		}
		else {
			pLPSvr->bSimu = TRUE;
		}
		DosUtil.SetLPUnitSimu(pLPSvr->bSimu, 1);
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnCidr() {
	CIDRSvr* pIDRSvr = pGDev->pIDRMgr->pCIDRSvr[0];
	if (pIDRSvr) {
		if (pIDRSvr->IDRUnit->bSimul) {
			pIDRSvr->IDRUnit->bSimul = FALSE;
		}
		else {
			pIDRSvr->IDRUnit->bSimul = TRUE;
		}
		DosUtil.SetCIDRSimu(pIDRSvr->IDRUnit->bSimul, 1);
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnCidr2() {
	CIDRSvr* pIDRSvr = pGDev->pIDRMgr->pCIDRSvr[1];
	if (pIDRSvr) {
		if (pIDRSvr->IDRUnit->bSimul) {
			pIDRSvr->IDRUnit->bSimul = FALSE;
		}
		else {
			pIDRSvr->IDRUnit->bSimul = TRUE;
		}
		DosUtil.SetCIDRSimu(pIDRSvr->IDRUnit->bSimul, 2);
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnEchomotor() {
	if (p413App->bSimuDMC) {
		p413App->bSimuDMC = FALSE;
	}
	else {
		p413App->bSimuDMC = TRUE;
	}
	p413App->SaveEchoOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnLoadport2() {
	CLPSvr* pLPSvr = pGDev->pLPMgr->pLPSvr[1];
	if (pLPSvr) {
		if (pLPSvr->bSimu) {
			pLPSvr->bSimu = FALSE;
		}
		else {
			pLPSvr->bSimu = TRUE;
		}
		DosUtil.SetLPUnitSimu(pLPSvr->bSimu, 2);
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnReveerseab() {
	if (p413App->bReverseAB) {
		p413App->bReverseAB = FALSE;
	}
	else {
		p413App->bReverseAB = TRUE;
	}
	p413App->SaveEchoOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnReveersesignal() {
	if (p413App->bReverseSignal) {
		p413App->bReverseSignal = FALSE;
	}
	else {
		p413App->bReverseSignal = TRUE;
	}
	p413App->SaveEchoOption();
	RecipeToLocal();
}

void CSetupProp9Dlg::OnKillfocusZpos1() {
	UpdateData(TRUE);

	g->CalZPosEcho = m_EchoTopZPos;
	g->SaveEchoOption();
}

void CSetupProp9Dlg::OnKillfocusZpos2() {
	UpdateData(TRUE);

	g->CalZPosTF = m_ThinFilmZPos;
	g->SaveThinFilmOption();
}

void CSetupProp9Dlg::OnReczpos1() {
	if (p413App->pMtr) p413App->pMtr->GetEchoTopZPosDev(m_EchoTopZPos);

	g->CalZPosEcho = m_EchoTopZPos;
	g->SaveEchoOption();

	UpdateData(FALSE);
}

void CSetupProp9Dlg::OnReczpos2() {
	if (p413App->pMtr) p413App->pMtr->GetThinFilmZPosDev(m_ThinFilmZPos);

	g->CalZPosTF = m_ThinFilmZPos;
	g->SaveThinFilmOption();

	UpdateData(FALSE);
}

void CSetupProp9Dlg::OnConfiguration() {
	CConfiguration dlg;

	dlg.DoModal();
}

//void CSetupProp9Dlg::OnKillfocusTfscalefactor()
//{
//	UpdateData(TRUE);
//	p413App->Global.TFScalingFactor = m_TFScalingFactor;
//
//	// Need to save this [6/7/2011 XPMUser]
//	g->SaveThinFilmOption();
//}

// void CSetupProp9Dlg::OnKillfocusCassetterepeat()
// {
// 	UpdateData(TRUE);
// }

void CSetupProp9Dlg::OnSpectrometerillumon() {
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroIllumLight(TRUE);
	}
}

void CSetupProp9Dlg::OnSpectrometerillumoff() {
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroIllumLight(FALSE);
	}
}

void CSetupProp9Dlg::OnSpectrometerpowon() {
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroPower(TRUE);
	}
}

void CSetupProp9Dlg::OnSpectrometerpowoff() {
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroPower(FALSE);
	}
}

void CSetupProp9Dlg::OnFfuread() {
	if (p413App->pMtr) {
		CString str;
		str.Format("%.3f", p413App->pMtr->GetFFUPressure());
		m_cCurFFUPress.SetWindowText(str);
	}
}

void CSetupProp9Dlg::OnSavemarker() {
	// Save video [9/12/2012 Administrator]
	// Save exposure [9/12/2012 Administrator]
	// Save Z1 coordinate [9/12/2012 Administrator]
}

long CSetupProp9Dlg::OnRedXHair(WPARAM wP, LPARAM lP) {
	g->SaveModule();
	return 0;
}

void CSetupProp9Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);

	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp9Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);

	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp9Dlg::OnKillfocusTfaesteps() {
	UpdateData(TRUE);
	g->AutoExposeStepTF = m_TFilmAESteps;
	g->SaveVideoOption();
}

void CSetupProp9Dlg::OnKillfocusEchoaesteps() {
	UpdateData(TRUE);
	g->AutoExposeStepEcho = m_EchoAESteps;
	g->SaveVideoOption();
}

void CSetupProp9Dlg::OnKillfocusTfilmafsteps() {
	UpdateData(TRUE);
	g->AutoFocusStepTF = m_TFilmAFSteps;
	g->SaveVideoOption();
}

void CSetupProp9Dlg::OnKillfocusEchoafsteps() {
	UpdateData(TRUE);
	g->AutoFocusStepEcho = m_EchoAFSteps;
	g->SaveVideoOption();
}

void CSetupProp9Dlg::OnResetrnd() {
	p413App->rnd1 = 1;
}

void CSetupProp9Dlg::OnMotioncontrol6() {
	ShowMotionControl(this);
}

void CSetupProp9Dlg::OnCamerasetting6() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp9Dlg::OnKillfocusScaleecho() {
	UpdateData(TRUE);
	p413App->Global.ScaleEcho = m_ScaleEcho;
	p413App->Global.SaveEchoOption();
}

void CSetupProp9Dlg::OnKillfocusScaletf() {
	UpdateData(TRUE);
	p413App->Global.ScaleTF = m_ScaleTF;
	p413App->Global.SaveThinFilmOption();
}

void CSetupProp9Dlg::OnVideocamera() {
	CSelectVCamera dlg;
	dlg.DoModal();
	// 	if (p413App->bSimuVideo)
	// 	{
	// 		p413App->bSimuVideo = FALSE;
	// 	}
	// 	else
	// 	{
	// 		p413App->bSimuVideo = TRUE;
	// 	}
	// 	p413App->SaveVideoOption();
	// 	RecipeToLocal();
}

void CSetupProp9Dlg::OnGetcount() {
	m_Count = m_cVideo.GetCount();
	UpdateData(FALSE);
}

void CSetupProp9Dlg::OnKillfocusBaseexpecho() {
	UpdateData(TRUE);
	g->BaseExpoEcho = m_BaseExpoEcho;
}

void CSetupProp9Dlg::OnKillfocusBaseexpthinf() {
	UpdateData(TRUE);
	g->BaseExpoTF = m_BaseExpoTF;
}

void CSetupProp9Dlg::OnKillfocusResetfreq() {
	UpdateData(TRUE);
	g->ResetFreq = m_ResetFreq;
	g->SaveModule();
}

void CSetupProp9Dlg::OnKillfocusLeveloffbot() {
	UpdateData(TRUE);
	g->PSLevelOff[1] = m_LevelBot;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusLevelofftop() {
	UpdateData(TRUE);
	g->PSLevelOff[0] = m_LevelTop;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusWidthofftop() {
	UpdateData(TRUE);
	g->PSWidthOff[0] = m_WidthTop;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusWidthoffbot() {
	UpdateData(TRUE);
	g->PSWidthOff[1] = m_WidthBot;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusGainechocam() {
	UpdateData(TRUE);
	g->GainEchoCam = m_GainEchoCam;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusGainthinfcam() {
	UpdateData(TRUE);
	g->GainThinFCam = m_GainThinFCam;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusOffsetechocam() {
	UpdateData(TRUE);
	g->OffsetEchoCam = m_OffsetEchoCam;
	g->Save();
}

void CSetupProp9Dlg::OnKillfocusOffsetthinfcam() {
	UpdateData(TRUE);
	g->OffsetThinFCam = m_OffsetThinFCam;
	g->Save();
}

void CSetupProp9Dlg::OnBnClickedLpAutoStartLatching()
{
	if (pGDev->bAutoStartLatching) {
		pGDev->bAutoStartLatching = FALSE;
	}
	else {
		pGDev->bAutoStartLatching = TRUE;
	}
	RecipeToLocal();
}

void CSetupProp9Dlg::OnBnClickedSyssettingStageCheckWaferPresence()
{
	g->bCheckWaferPresence = !g->bCheckWaferPresence;
	g->SaveModule();
}
