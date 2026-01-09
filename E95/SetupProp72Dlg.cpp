// SetupProp72Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp72Dlg.h"
#include "SetupProp3Dlg.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "413App.h"
#include "InfoPanelSetup.h"
#include "..\413\DMCMotorBase.h"
#include "..\413\MotorS2.h"
#include "PeakControl.h"
#include "DefineSimuPeaks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp72Dlg dialog

CSetupProp72Dlg::CSetupProp72Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp72Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupProp72Dlg)
	m_SamplingFreq = 0;
	m_InterestedDataSet = 0;
	m_MotorFile = _T("");
	m_StepSize = 0.0f;
	m_SearchN = 0;
	m_TimeLimt = 0;
	m_PointTimeout = 0;
	m_FailPoint = 0;
	m_BumpOffsetX = 0.0f;
	m_BumpOffsetY = 0.0f;
	m_EchoTopZPos = 0.0f;
	m_ZPosOffset = 0.0f;
	m_Exposure = 0;
	//}}AFX_DATA_INIT

	m_Threshold[0] = m_Threshold[1] = 10.0f;
	m_Level[0] = m_Level[1] = 20;
	m_PeakWidth[0] = m_PeakWidth[1] = 80;
	m_PeakNear[0] = m_PeakNear[1] = 32;
	m_PeakSmooth[0] = m_PeakSmooth[1] = 0;
	m_PeakMinimum[0] = m_PeakMinimum[1] = 0;

	// [01312024/yukchiu
	m_settlingTime = 0;
	// [05152024 ZHIMING
	m_EchoBottomZPos = 0;
	// ]
}

void CSetupProp72Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp72Dlg)
	DDX_Control(pDX, IDC_DIANOSTIC, m_cDianostic);
	DDX_Control(pDX, IDC_DEFINEPEAK, m_cDefinePeaks);
	DDX_Control(pDX, IDC_SAVEVIDEO2, m_cSaveVideo);
	DDX_Control(pDX, IDC_USEWAFERALIGN, m_cUseWaferAlign);
	DDX_Control(pDX, IDC_USE, m_cUse);
	DDX_Control(pDX, IDC_USEMOTORFILE, m_cUseMotorFile);
	DDX_Text(pDX, IDC_SAMPLINGFREG, m_SamplingFreq);
	DDX_Text(pDX, IDC_INTERESTEDDATASET, m_InterestedDataSet);
	DDX_Text(pDX, IDC_MOTORFILE, m_MotorFile);
	DDX_Text(pDX, IDC_RCP_SEARCH_STEPSIZE, m_StepSize);
	DDX_Text(pDX, IDC_RCP_SEARCH_AROUND, m_SearchN);
	DDX_Text(pDX, IDC_TIMELIMIT, m_TimeLimt);
	DDX_Text(pDX, IDC_POINTTIMEOUT, m_PointTimeout);
	DDX_Text(pDX, IDC_FAILPOINT, m_FailPoint);
	DDX_Text(pDX, IDC_BUMPOFFSETX, m_BumpOffsetX);
	DDX_Text(pDX, IDC_BUMPOFFSETY, m_BumpOffsetY);
	DDX_Text(pDX, IDC_ZPOSITION, m_EchoTopZPos);
	DDX_Text(pDX, IDC_ZPOSITIONOFF, m_ZPosOffset);
	DDX_Text(pDX, IDC_EXPOSURE, m_Exposure);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_THRESHOLDFACTOR1, m_Threshold[0]);
	DDX_Text(pDX, IDC_THRESHOLDFACTOR2, m_Threshold[1]);
	DDX_Text(pDX, IDC_RCP_LEVEL1, m_Level[0]);
	DDX_Text(pDX, IDC_RCP_LEVEL2, m_Level[1]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH1, m_PeakWidth[0]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH2, m_PeakWidth[1]);
	DDX_Text(pDX, IDC_RCP_PEAKNEAR1, m_PeakNear[0]);
	DDX_Text(pDX, IDC_RCP_PEAKNEAR2, m_PeakNear[1]);
	DDX_Text(pDX, IDC_RCP_PEAKSMOOTH1, m_PeakSmooth[0]);
	DDX_Text(pDX, IDC_RCP_PEAKSMOOTH2, m_PeakSmooth[1]);
	DDX_Text(pDX, IDC_RCP_PEAKMINIMUM1, m_PeakMinimum[0]);
	DDX_Text(pDX, IDC_RCP_PEAKMINIMUM2, m_PeakMinimum[1]);
	//01312024/yukchiu
	DDX_Text(pDX, IDC_SETTLINGTIME, m_settlingTime);
	// [05152024 ZHIMING
	DDX_Text(pDX, IDC_ECHOBOTTOMZPOSITION, m_EchoBottomZPos);
	// ]
}

BEGIN_MESSAGE_MAP(CSetupProp72Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp72Dlg)
	ON_EN_KILLFOCUS(IDC_MOTORFILE, OnKillfocusMotorfile)
	ON_BN_CLICKED(IDC_USEMOTORFILE, OnUsemotorfile)
	ON_BN_CLICKED(IDC_USE, OnUse)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_KILLFOCUS(IDC_RCP_SEARCH_STEPSIZE, OnKillfocusRcpSearchStepsize)
	ON_EN_KILLFOCUS(IDC_RCP_SEARCH_AROUND, OnKillfocusRcpSearchAround)
	ON_EN_KILLFOCUS(IDC_TIMELIMIT, OnKillfocusTimelimit)
	ON_EN_KILLFOCUS(IDC_POINTTIMEOUT, OnKillfocusPointtimeout)
	ON_EN_KILLFOCUS(IDC_FAILPOINT, OnKillfocusFailpoint)
	ON_EN_KILLFOCUS(IDC_BUMPOFFSETX, OnKillfocusBumpoffsetx)
	ON_EN_KILLFOCUS(IDC_BUMPOFFSETY, OnKillfocusBumpoffsety)
	ON_BN_CLICKED(IDC_USEWAFERALIGN, OnUsewaferalign)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_RECZPOS, OnReczpos)
	ON_EN_KILLFOCUS(IDC_ZPOSITION, OnKillfocusZposition)
	ON_BN_CLICKED(IDC_RECZPOSOFF, OnReczposoff)
	ON_EN_KILLFOCUS(IDC_ZPOSITIONOFF, OnKillfocusZpositionoff)
	ON_EN_KILLFOCUS(IDC_EXPOSURE, OnKillfocusExposure)
	ON_BN_CLICKED(IDC_SAVEVIDEO2, OnSavevideo2)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_PEAKCONTROL, OnPeakcontrol)
	ON_BN_CLICKED(IDC_DEFINEPEAK, OnDefinepeak)
	ON_BN_CLICKED(IDC_DIANOSTIC, OnDianostic)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_EN_KILLFOCUS(IDC_SETTLINGTIME, OnKillfocusSettlingtime)
	ON_BN_CLICKED(IDC_RECECHOBOTTOMZPOS, &CSetupProp72Dlg::OnBnClickedRecechobottomzpos)
	ON_EN_KILLFOCUS(IDC_ECHOBOTTOMZPOSITION, &CSetupProp72Dlg::OnKillfocusEchobottomzposition)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp72Dlg message handlers

long CSetupProp72Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		RecipeToLocal();
		bTabSelected = TRUE;

		if (p413App->pEcho->bSimu) {
			m_cDefinePeaks.ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

long CSetupProp72Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

BOOL CSetupProp72Dlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CSetupProp72Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSetupProp72Dlg::RecipeToLocal() {
	m_SamplingFreq = pRcp->SamplingFreq;
	m_InterestedDataSet = pRcp->NumberOfInterestedData;
	m_MotorFile = pRcp->MotorFile;
	m_cUseMotorFile.SetCheck(pRcp->bUseMotorFile);

	m_SearchN = pRcp->nTrialEcho;
	m_StepSize = pRcp->fStepSizeEcho;

	m_FailPoint = pRcp->FailCountTimeOut;
	m_TimeLimt = pRcp->WaferTimeOut;
	m_PointTimeout = pRcp->PointTimeOut;

	m_BumpOffsetX = pRcp->BumpOffsetX;
	m_BumpOffsetY = pRcp->BumpOffsetY;

	m_cUseWaferAlign.SetCheck(pRcp->bUseWaferAlign);
	m_cDianostic.SetCheck(pRcp->bEchoPeakMonitor);
	m_cSaveVideo.SetCheck(pRcp->bSaveVideo);

	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		m_Threshold[i] = pRcp->MeParam[MeS].PSG[i].Thres;
		m_Level[i] = pRcp->MeParam[MeS].PSG[i].Level;
		m_PeakWidth[i] = pRcp->MeParam[MeS].PSG[i].Width;
		m_PeakNear[i] = pRcp->MeParam[MeS].PSG[i].Near;
		m_PeakSmooth[i] = pRcp->MeParam[MeS].PSG[i].Smooth;
		m_PeakMinimum[i] = pRcp->MeParam[MeS].PSG[i].Minimum;
	}
	m_EchoTopZPos = pRcp->fEchoTopZ;
	m_ZPosOffset = pRcp->TFZOffset;
	m_Exposure = pRcp->EchoNormalExposure;

	//01312024/yukchiu
	m_settlingTime = pRcp->settlingTime;
	// [05152024 ZHIMING
	//m_EchoBottomZPos = pRcp->fEchoBottomZ;
	// ]
	UpdateData(FALSE);
}

void CSetupProp72Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	pRcp->SamplingFreq = m_SamplingFreq;
	pRcp->NumberOfInterestedData = m_InterestedDataSet;
	pRcp->MotorFile;
	pRcp->bUseMotorFile = m_cUseMotorFile.GetCheck();
	pRcp->bUseWaferAlign = m_cUseWaferAlign.GetCheck();
	pRcp->nTrialEcho = m_SearchN;
	pRcp->fStepSizeEcho = m_StepSize;
	pRcp->WaferTimeOut = m_TimeLimt;
	pRcp->FailCountTimeOut = m_FailPoint;
	pRcp->PointTimeOut = m_PointTimeout;
	pRcp->BumpOffsetX = m_BumpOffsetX;
	pRcp->BumpOffsetY = m_BumpOffsetY;

	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_Threshold[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
		pRcp->MeParam[MeS].PSG[i].Near = m_PeakNear[i];
		pRcp->MeParam[MeS].PSG[i].Smooth = m_PeakSmooth[i];
		pRcp->MeParam[MeS].PSG[i].Minimum = m_PeakMinimum[i];
	}
	pRcp->fEchoTopZ = m_EchoTopZPos;
	pRcp->TFZOffset = m_ZPosOffset;
	pRcp->EchoNormalExposure = m_Exposure;

	//01312024/yukchiu
	pRcp->settlingTime = m_settlingTime;
	// [05152024 ZHIMING
	//pRcp->fEchoBottomZ = m_EchoBottomZPos;
	// ]
}

BOOL CSetupProp72Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	RecipeToLocal();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp72Dlg::OnKillfocusMotorfile() {
	UpdateData(TRUE);
	if (pRcp) pRcp->MotorFile = m_MotorFile;
}

void CSetupProp72Dlg::OnUsemotorfile() {
	if (pRcp->bUseMotorFile) {
		pRcp->bUseMotorFile = FALSE;
	}
	else {
		pRcp->bUseMotorFile = TRUE;
	}
}

void CSetupProp72Dlg::OnUse() {
	m_cUse.EnableWindow(FALSE);
	if (pUDev->pInfoPanelSetup && pUDev->pInfoPanelSetup->m_Prop3Dlg) {
		BOOL bRun = p413App->pDMC->bRun;
		pUDev->pInfoPanelSetup->m_Prop3Dlg->StoppingEcho();
		UpdateData(TRUE);

		pRcp->SamplingFreq = m_SamplingFreq;
		pRcp->NumberOfInterestedData = m_InterestedDataSet;
		pRcp->MotorFile = m_MotorFile;
		pRcp->bUseMotorFile = TRUE;
		m_cUseMotorFile.SetCheck(TRUE);
		p413App->ResetEchoSamplingProperty(pRcp, NULL);

		if (bRun) {
			pUDev->pInfoPanelSetup->m_Prop3Dlg->StartingEcho();
		}
	}
	m_cUse.EnableWindow(TRUE);
}

void CSetupProp72Dlg::OnBrowse() {
	CFileDialog dlg(TRUE,
		"TXT",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Text files|*.TXT|");
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	dlg.m_ofn.lpstrInitialDir = mdir.GetBuffer(0);
	if (dlg.DoModal() == IDOK) {
		m_MotorFile = dlg.GetFileName();
		UpdateData(FALSE);
	}
}

void CSetupProp72Dlg::OnKillfocusRcpSearchStepsize() {
	UpdateData(TRUE);
	pRcp->fStepSizeEcho = m_StepSize;
}

void CSetupProp72Dlg::OnKillfocusRcpSearchAround() {
	UpdateData(TRUE);
	pRcp->nTrialEcho = m_SearchN;
}

void CSetupProp72Dlg::OnKillfocusTimelimit() {
	UpdateData(TRUE);
	pRcp->WaferTimeOut = m_TimeLimt;
}

void CSetupProp72Dlg::OnKillfocusPointtimeout() {
	UpdateData(TRUE);
	pRcp->PointTimeOut = m_PointTimeout;
}

void CSetupProp72Dlg::OnKillfocusFailpoint() {
	UpdateData(TRUE);
	pRcp->FailCountTimeOut = m_FailPoint;
}

void CSetupProp72Dlg::OnKillfocusBumpoffsetx() {
	UpdateData(TRUE);
	pRcp->BumpOffsetX = m_BumpOffsetX;
}

void CSetupProp72Dlg::OnKillfocusBumpoffsety() {
	UpdateData(TRUE);
	pRcp->BumpOffsetY = m_BumpOffsetY;
}

void CSetupProp72Dlg::OnUsewaferalign() {
	if (pRcp->bUseWaferAlign) {
		pRcp->bUseWaferAlign = FALSE;
	}
	else {
		pRcp->bUseWaferAlign = TRUE;
	}
}

void CSetupProp72Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp72Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp72Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp72Dlg::OnSet4() {
	SwapMeSet(4);
}

void CSetupProp72Dlg::OnSet5() {
	SwapMeSet(5);
}

void CSetupProp72Dlg::OnSet6() {
	SwapMeSet(6);
}

void CSetupProp72Dlg::OnSet7() {
	SwapMeSet(7);
}

void CSetupProp72Dlg::OnSet8() {
	SwapMeSet(8);
}

void CSetupProp72Dlg::OnSet9() {
	SwapMeSet(9);
}

void CSetupProp72Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}

void CSetupProp72Dlg::OnReczpos() {
	if (p413App->pMtr) p413App->pMtr->GetEchoTopZPosDev(m_EchoTopZPos);
	pRcp->fEchoTopZ = m_EchoTopZPos;
	UpdateData(FALSE);
}

void CSetupProp72Dlg::OnKillfocusZposition() {
	UpdateData(TRUE);
	pRcp->fEchoTopZ = m_EchoTopZPos;
}

void CSetupProp72Dlg::OnReczposoff() {
	float zPos;
	p413App->pMtr->GetEchoTopZPosDev(zPos);
	pRcp->TFZOffset = zPos - pRcp->fEchoTopZ;
	m_ZPosOffset = pRcp->TFZOffset;
	UpdateData(FALSE);
}

void CSetupProp72Dlg::OnKillfocusZpositionoff() {
	UpdateData(TRUE);
	pRcp->TFZOffset = m_ZPosOffset;
}

void CSetupProp72Dlg::OnKillfocusExposure() {
	UpdateData(TRUE);
	pRcp->EchoNormalExposure = m_Exposure;
}

void CSetupProp72Dlg::OnSavevideo2() {
	if (pRcp->bSaveVideo) {
		pRcp->bSaveVideo = FALSE;
		m_cSaveVideo.SetCheck(FALSE);
	}
	else {
		pRcp->bSaveVideo = TRUE;
		m_cSaveVideo.SetCheck(TRUE);
	}
}

void CSetupProp72Dlg::OnSaverecipe() {
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(*pRcp))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp72Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

// void CSetupProp72Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp72Dlg::OnPeakcontrol() {
	CPeakControl dlg;
	dlg.pRcp = pRcp;
	dlg.DoModal();
}

void CSetupProp72Dlg::OnDefinepeak() {
	CDefineSimuPeaks dlg;

	dlg.DoModal();
}

void CSetupProp72Dlg::OnDianostic() {
	if (pRcp->bEchoPeakMonitor) {
		pRcp->bEchoPeakMonitor = FALSE;
		m_cDianostic.SetCheck(FALSE);
	}
	else {
		pRcp->bEchoPeakMonitor = TRUE;
		m_cDianostic.SetCheck(TRUE);
	}
}

//01312024/yukchiu
void CSetupProp72Dlg::OnKillfocusSettlingtime() {
	UpdateData(TRUE);
}

void CSetupProp72Dlg::OnBnClickedRecechobottomzpos()
{
	if (p413App->pMtr) p413App->pMtr->GetEchoBottomZPosDev(m_EchoBottomZPos);
	//pRcp->fEchoBottomZ = m_EchoBottomZPos;
	UpdateData(FALSE);
}


void CSetupProp72Dlg::OnKillfocusEchobottomzposition()
{
	UpdateData(TRUE);
	//pRcp->fEchoBottomZ = m_EchoBottomZPos;
}
