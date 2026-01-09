// SetupProp83Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp83Dlg.h"
#include "413App.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "XTabCtrl.h"
#include "..\413\MotorS2.h"
#include "HelpTrim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp83Dlg dialog

CSetupProp83Dlg::CSetupProp83Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp83Dlg::IDD, pParent), m_FindBevelThres(0)
{
	//{{AFX_DATA_INIT(CSetupProp83Dlg)
	m_Spacing = 8;
	m_NLines = 5;
	m_Exposure1 = 2000;
	m_Exposure2 = 200;
	m_Exposure3 = 100;
	m_TrimWidthOuterRadius = 0.0f;
	m_ZPos = 0.0f;
	m_TrimHeightDistanceInner = 0.0f;
	m_TrimHeightDistanceInnerB = 0.0f;
	m_TrimHeightDistanceOuter1 = 0.0f;
	m_TrimHeightDistanceOuter2 = 0.0f;
	m_TrimHeightZOffset = 0.0f;
	m_NMeasureOut = 1;
	m_Smooth = 1;
	m_TrimWidthInnerRadius = 0.0f;
	m_NTrial = 0;
	m_SearchSpacing = 0.0f;
	m_AutoExposeIn = 0;
	m_AutoExposeOut = 0;
	m_NMeasureIn = 1;
	m_FindTrimThres = 0.0f;
	m_FindBevelThres = 3.5f;
	m_ContrastLevel = 0.0f;
	m_SamplePerPoint = 0;
	m_THExposure = 0;
	m_TrimWidthOuterRadius2 = 0.0f;
	m_EstNTrimWidth = 0.0f;
	//}}AFX_DATA_INIT
}

void CSetupProp83Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp83Dlg)
	DDX_Control(pDX, IDC_SAVEVIDEO2, m_cSaveVideo);
	DDX_Text(pDX, IDC_SPACING, m_Spacing);
	DDX_Text(pDX, IDC_NUMBEROFLINES, m_NLines);
	DDX_Text(pDX, IDC_EXPOSURE, m_Exposure1);
	DDX_Text(pDX, IDC_EXPOSURE3, m_Exposure2);
	DDX_Text(pDX, IDC_EXPOSURE5, m_Exposure3);
	DDX_Text(pDX, IDC_TRIMWIDTHRADIUS, m_TrimWidthOuterRadius);
	DDX_Text(pDX, IDC_ZPOSITION, m_ZPos);
	DDX_Text(pDX, IDC_TRIMHEIGHTDISTANCE, m_TrimHeightDistanceInner);
	DDX_Text(pDX, IDC_TRIMHEIGHTDISTANCE4, m_TrimHeightDistanceInnerB);
	DDX_Text(pDX, IDC_TRIMHEIGHTDISTANCE2, m_TrimHeightDistanceOuter1);
	DDX_Text(pDX, IDC_TRIMHEIGHTDISTANCE3, m_TrimHeightDistanceOuter2);
	DDX_Text(pDX, IDC_TRIMHEIGHTZOFFSET, m_TrimHeightZOffset);
	DDX_Text(pDX, IDC_NMEASUREMENT, m_NMeasureOut);
	DDV_MinMaxInt(pDX, m_NMeasureOut, 1, 16);
	DDX_Text(pDX, IDC_SMOOTH, m_Smooth);
	DDX_Text(pDX, IDC_TRIMWIDTHINNERRADIUS, m_TrimWidthInnerRadius);
	DDX_Text(pDX, IDC_NTRIAL, m_NTrial);
	DDX_Text(pDX, IDC_SEARCH_SPACING, m_SearchSpacing);
	DDX_Text(pDX, IDC_AUTOEXPOSUREIN, m_AutoExposeIn);
	DDX_Text(pDX, IDC_AUTOEXPOSUREOUT, m_AutoExposeOut);
	DDX_Text(pDX, IDC_NMEASUREMENT2, m_NMeasureIn);
	DDV_MinMaxInt(pDX, m_NMeasureIn, 1, 16);
	DDX_Text(pDX, IDC_THRESHOLD2, m_FindTrimThres);
	DDX_Text(pDX, IDC_CONTRAST_LEVEL, m_ContrastLevel); //20210329
	DDX_Text(pDX, IDC_SAMPLEPERPOINT, m_SamplePerPoint);
	DDX_Text(pDX, IDC_THExposure, m_THExposure);
	DDX_Text(pDX, IDC_TRIMWIDTHRADIUS2, m_TrimWidthOuterRadius2);
	DDX_Text(pDX, IDC_ESTWTRIMWIDTH, m_EstNTrimWidth);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_THRESHOLD3, m_FindBevelThres);
}

BEGIN_MESSAGE_MAP(CSetupProp83Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp83Dlg)
	ON_EN_KILLFOCUS(IDC_SPACING, OnKillfocusSpacing)
	ON_EN_KILLFOCUS(IDC_NUMBEROFLINES, OnKillfocusNumberoflines)
	ON_EN_KILLFOCUS(IDC_EXPOSURE, OnKillfocusExposure)
	ON_EN_KILLFOCUS(IDC_EXPOSURE3, OnKillfocusExposure3)
	ON_EN_KILLFOCUS(IDC_EXPOSURE5, OnKillfocusExposure5)
	ON_EN_KILLFOCUS(IDC_TRIMWIDTHRADIUS, OnKillfocusTrimwidthradius)
	ON_EN_KILLFOCUS(IDC_ZPOSITION, OnKillfocusZposition)
	ON_BN_CLICKED(IDC_RECZPOS, OnReczpos)
	ON_EN_KILLFOCUS(IDC_TRIMHEIGHTDISTANCE, OnKillfocusTrimheightdistance)
	ON_EN_KILLFOCUS(IDC_TRIMHEIGHTDISTANCE2, OnKillfocusTrimheightdistance2)
	ON_EN_KILLFOCUS(IDC_TRIMHEIGHTDISTANCE3, OnKillfocusTrimheightdistance3)
	ON_EN_KILLFOCUS(IDC_TRIMHEIGHTZOFFSET, OnKillfocusTrimheightzoffset)
	ON_EN_KILLFOCUS(IDC_NMEASUREMENT, OnKillfocusNmeasurement)
	ON_EN_KILLFOCUS(IDC_SMOOTH, OnKillfocusSmooth)
	ON_EN_KILLFOCUS(IDC_TRIMWIDTHINNERRADIUS, OnKillfocusTrimwidthinnerradius)
	ON_EN_KILLFOCUS(IDC_NTRIAL, OnKillfocusNtrial)
	ON_EN_KILLFOCUS(IDC_SEARCH_SPACING, OnKillfocusSearchSpacing)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_HELPS, OnHelps)
	ON_BN_CLICKED(IDC_MEASUREOUTER, OnMeasureouter)
	ON_EN_KILLFOCUS(IDC_AUTOEXPOSUREIN, OnKillfocusAutoexposurein)
	ON_EN_KILLFOCUS(IDC_AUTOEXPOSUREOUT, OnKillfocusAutoexposureout)
	ON_BN_CLICKED(IDC_SAVEVIDEO2, OnSavevideo2)
	ON_EN_KILLFOCUS(IDC_NMEASUREMENT2, OnKillfocusNmeasurement2)
	ON_EN_KILLFOCUS(IDC_THRESHOLD2, OnKillfocusThreshold2)
	ON_EN_KILLFOCUS(IDC_SAMPLEPERPOINT, OnKillfocusSampleperpoint)
	ON_BN_CLICKED(IDC_METHOD1, OnMethod1)
	ON_BN_CLICKED(IDC_METHOD2, OnMethod2)
	ON_BN_CLICKED(IDC_LOCATEINNEREDGE, OnLocateinneredge)
	ON_EN_KILLFOCUS(IDC_THExposure, OnKillfocusTHExposure)
	ON_EN_KILLFOCUS(IDC_TRIMHEIGHTDISTANCE4, OnKillfocusTrimheightdistanceB)
	ON_EN_KILLFOCUS(IDC_TRIMWIDTHRADIUS2, OnKillfocusTrimwidthradius2)
	ON_EN_KILLFOCUS(IDC_ESTWTRIMWIDTH, OnKillfocusEstwtrimwidth)
	ON_BN_CLICKED(IDC_LOCATEINNEREDGE2, OnLocateinneredge2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_EN_KILLFOCUS(IDC_CONTRAST_LEVEL, &CSetupProp83Dlg::OnKillfocusContrastLevel)
	ON_EN_KILLFOCUS(IDC_THRESHOLD3, &CSetupProp83Dlg::OnEnKillfocusThreshold3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp83Dlg message handlers

long CSetupProp83Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp83Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		RecipeToLocal();
		bTabSelected = TRUE;
	}

	return 0;
}

BOOL CSetupProp83Dlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CSetupProp83Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp83Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);
	AddAnchor(IDC_HELPS, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp83Dlg::OnKillfocusSpacing() {
	UpdateData(TRUE);
	pRcp->TrimWidthSpacing = m_Spacing;
}

void CSetupProp83Dlg::RecipeToLocal() {
	m_Spacing = pRcp->TrimWidthSpacing;
	m_NLines = pRcp->TrimWidthLines * 2 + 1;
	m_Exposure1 = pRcp->TWExposureInner;
	m_Exposure2 = pRcp->TWExposureOuter;
	m_Exposure3 = pRcp->TWExposureBevel;
	m_AutoExposeIn = pRcp->AutoExposeInner;
	m_AutoExposeOut = pRcp->AutoExposeOuter;
	m_TrimHeightDistanceInner = pRcp->TrimHeightDistanceInner1;
	m_TrimHeightDistanceInnerB = pRcp->TrimHeightDistanceInner2;
	m_TrimHeightDistanceOuter1 = pRcp->TrimHeightDistanceOuter1;
	m_TrimHeightDistanceOuter2 = pRcp->TrimHeightDistanceOuter2;
	m_TrimHeightZOffset = pRcp->TrimHeightZOffset;
	m_ZPos = pRcp->fEchoTopZ;
	m_NMeasureIn = pRcp->NMeasureIn;
	m_NMeasureOut = pRcp->NMeasureOut;
	m_Smooth = pRcp->TrimSmooth;
	m_TrimWidthOuterRadius = pRcp->TrimWidthOuterRadius;
	m_TrimWidthInnerRadius = m_TrimWidthOuterRadius2 = pRcp->TrimWidthInnerRadius;
	m_EstNTrimWidth = pRcp->EstNTrimWidth;
	m_NTrial = pRcp->nTrialTrim;
	m_SearchSpacing = pRcp->fStepSizeTrim;
	m_FindTrimThres = pRcp->FindTrimThres;
	m_FindBevelThres = pRcp->FindBevelThres;
	m_SamplePerPoint = pRcp->SamplePerPoint;
	m_THExposure = pRcp->THExposure;
	m_ContrastLevel = pRcp->ContrastLevel;	//[04222021] ZHIMING

	((CButton*)GetDlgItem(IDC_MEASUREOUTER))->SetCheck(pRcp->bMeasureOuterdge);

	((CButton*)GetDlgItem(IDC_LOCATEINNEREDGE))->SetCheck(pRcp->bLocateEdgeStep);
	((CButton*)GetDlgItem(IDC_LOCATEINNEREDGE2))->SetCheck(pRcp->bLocateEdgeWidth);

	if (pRcp->bSaveVideo) {
		m_cSaveVideo.SetCheck(TRUE);
	}
	else {
		m_cSaveVideo.SetCheck(FALSE);
	}
	((CButton*)GetDlgItem(IDC_METHOD1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_METHOD2))->SetCheck(FALSE);
	switch (pRcp->nTrimMethod) {
	case 1:
		((CButton*)GetDlgItem(IDC_METHOD2))->SetCheck(TRUE);
		break;
	default:
		((CButton*)GetDlgItem(IDC_METHOD1))->SetCheck(TRUE);
		break;
	}

	UpdateData(FALSE);
}

void CSetupProp83Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	pRcp->TrimWidthSpacing = m_Spacing;
	pRcp->TrimWidthLines = m_NLines / 2;
	pRcp->TWExposureInner = m_Exposure1;
	pRcp->TWExposureOuter = m_Exposure2;
	pRcp->TWExposureBevel = m_Exposure3;
	pRcp->AutoExposeInner = m_AutoExposeIn;
	pRcp->AutoExposeOuter = m_AutoExposeOut;
	pRcp->TrimHeightDistanceInner1 = m_TrimHeightDistanceInner;
	pRcp->TrimHeightDistanceInner2 = m_TrimHeightDistanceInnerB;
	pRcp->TrimHeightDistanceOuter1 = m_TrimHeightDistanceOuter1;
	pRcp->TrimHeightDistanceOuter2 = m_TrimHeightDistanceOuter2;
	pRcp->TrimHeightZOffset = m_TrimHeightZOffset;
	pRcp->fEchoTopZ = m_ZPos;
	pRcp->NMeasureIn = m_NMeasureIn;
	pRcp->NMeasureOut = m_NMeasureOut;
	// 	pRcp->CXOffset = m_CXOffset;
	// 	pRcp->CYOffset = m_CYOffset;
	pRcp->TrimSmooth = m_Smooth;
	pRcp->TrimWidthOuterRadius = m_TrimWidthOuterRadius;
	pRcp->TrimWidthInnerRadius = m_TrimWidthInnerRadius;
	pRcp->EstNTrimWidth = m_EstNTrimWidth;
	pRcp->nTrialTrim = m_NTrial;
	pRcp->fStepSizeTrim = m_SearchSpacing;
	pRcp->FindTrimThres = m_FindTrimThres;
	pRcp->FindBevelThres = m_FindBevelThres;
	pRcp->ContrastLevel = m_ContrastLevel;	//[04222021] ZHIMING
}

void CSetupProp83Dlg::OnKillfocusNumberoflines() {
	UpdateData(TRUE);
	pRcp->TrimWidthLines = m_NLines / 2;
}

void CSetupProp83Dlg::OnKillfocusExposure() {
	UpdateData(TRUE);
	pRcp->TWExposureInner = m_Exposure1;
}

void CSetupProp83Dlg::OnKillfocusExposure3() {
	UpdateData(TRUE);
	pRcp->TWExposureOuter = m_Exposure2;
}

void CSetupProp83Dlg::OnKillfocusExposure5() {
	UpdateData(TRUE);
	pRcp->TWExposureBevel = m_Exposure3;
}

void CSetupProp83Dlg::OnKillfocusTrimwidthradius() {
	UpdateData(TRUE);
	pRcp->TrimWidthOuterRadius = m_TrimWidthOuterRadius;
	UpdateData(FALSE);
}

void CSetupProp83Dlg::OnKillfocusZposition() {
	UpdateData(TRUE);
	pRcp->fEchoTopZ = m_ZPos;
}

void CSetupProp83Dlg::OnReczpos() {
	if (p413App->pMtr) p413App->pMtr->GetEchoTopZPosDev(m_ZPos);
	pRcp->fEchoTopZ = m_ZPos;
	UpdateData(FALSE);
}

void CSetupProp83Dlg::OnKillfocusTrimheightdistance() {
	UpdateData(TRUE);
	pRcp->TrimHeightDistanceInner1 = m_TrimHeightDistanceInner;
}

void CSetupProp83Dlg::OnKillfocusTrimheightdistance2() {
	UpdateData(TRUE);
	pRcp->TrimHeightDistanceOuter1 = m_TrimHeightDistanceOuter1;
}

void CSetupProp83Dlg::OnKillfocusTrimheightdistance3() {
	UpdateData(TRUE);
	pRcp->TrimHeightDistanceOuter2 = m_TrimHeightDistanceOuter2;
}

void CSetupProp83Dlg::OnKillfocusTrimheightzoffset() {
	UpdateData(TRUE);
	pRcp->TrimHeightZOffset = m_TrimHeightZOffset;
}

void CSetupProp83Dlg::OnKillfocusNmeasurement() {
	UpdateData(TRUE);
	pRcp->NMeasureOut = m_NMeasureOut;
}

// void CSetupProp83Dlg::OnKillfocusCxoffset()
// {
// 	UpdateData(TRUE);
// 	g->CXOffsetEcho = m_CXOffset;
// 	if (AfxMessageBox("Save to INI",MB_YESNO) == IDYES)
// 	{
// 		g->SaveMotorOption();
// 	}
// }
//
// void CSetupProp83Dlg::OnKillfocusCyoffset()
// {
// 	UpdateData(TRUE);
// 	g->CYOffsetEcho = m_CYOffset;
// 	if (AfxMessageBox("Save to INI",MB_YESNO) == IDYES)
// 	{
// 		g->SaveMotorOption();
// 	}
// }

void CSetupProp83Dlg::OnKillfocusSmooth() {
	UpdateData(TRUE);
	pRcp->TrimSmooth = m_Smooth;
}

void CSetupProp83Dlg::OnKillfocusTrimwidthinnerradius() {
	UpdateData(TRUE);
	pRcp->TrimWidthInnerRadius = m_TrimWidthOuterRadius2 = m_TrimWidthInnerRadius;
}

void CSetupProp83Dlg::OnKillfocusNtrial() {
	UpdateData(TRUE);
	pRcp->nTrialTrim = m_NTrial;
}

void CSetupProp83Dlg::OnKillfocusSearchSpacing() {
	UpdateData(TRUE);
	pRcp->fStepSizeTrim = m_SearchSpacing;
}

// void CSetupProp83Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp83Dlg::OnSaverecipe() {
	LocalToRecipe();
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(p413App->Global.RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

void CSetupProp83Dlg::OnHelps() {
	CHelpTrim dlg;
	dlg.DoModal();
}

void CSetupProp83Dlg::OnMeasureouter() {
	if (pRcp->bMeasureOuterdge) {
		pRcp->bMeasureOuterdge = FALSE;
	}
	else {
		pRcp->bMeasureOuterdge = TRUE;
	}
}

void CSetupProp83Dlg::OnKillfocusAutoexposurein() {
	UpdateData(TRUE);
	pRcp->AutoExposeInner = m_AutoExposeIn;
}

void CSetupProp83Dlg::OnKillfocusAutoexposureout() {
	UpdateData(TRUE);
	pRcp->AutoExposeOuter = m_AutoExposeOut;
}

// void CSetupProp83Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp83Dlg::OnSavevideo2() {
	if (pRcp->bSaveVideo) {
		pRcp->bSaveVideo = FALSE;
		m_cSaveVideo.SetCheck(FALSE);
	}
	else {
		pRcp->bSaveVideo = TRUE;
		m_cSaveVideo.SetCheck(TRUE);
	}
}

void CSetupProp83Dlg::OnKillfocusNmeasurement2() {
	UpdateData(TRUE);
	pRcp->NMeasureIn = m_NMeasureIn;
}

void CSetupProp83Dlg::OnKillfocusThreshold2() {
	UpdateData(TRUE);
	pRcp->FindTrimThres = m_FindTrimThres;
}

void CSetupProp83Dlg::OnKillfocusSampleperpoint() {
	UpdateData(TRUE);
	pRcp->SamplePerPoint = m_SamplePerPoint;
}

void CSetupProp83Dlg::OnMethod1() {
	pRcp->nTrimMethod = 0;
}

void CSetupProp83Dlg::OnMethod2() {
	pRcp->nTrimMethod = 1;
}

void CSetupProp83Dlg::OnLocateinneredge() {
	if (pRcp->bLocateEdgeStep) {
		pRcp->bLocateEdgeStep = FALSE;
	}
	else {
		pRcp->bLocateEdgeStep = TRUE;
	}
}

void CSetupProp83Dlg::OnKillfocusTHExposure() {
	UpdateData(TRUE);
	pRcp->THExposure = m_THExposure;
}

void CSetupProp83Dlg::OnKillfocusTrimheightdistanceB() {
	UpdateData(TRUE);
	pRcp->TrimHeightDistanceInner2 = m_TrimHeightDistanceInnerB;
}

void CSetupProp83Dlg::OnKillfocusTrimwidthradius2() {
	UpdateData(TRUE);
	pRcp->TrimWidthInnerRadius = m_TrimWidthInnerRadius = m_TrimWidthOuterRadius2;
	UpdateData(FALSE);
}

void CSetupProp83Dlg::OnKillfocusEstwtrimwidth() {
	UpdateData(TRUE);
	pRcp->EstNTrimWidth = m_EstNTrimWidth;
}

void CSetupProp83Dlg::OnLocateinneredge2() {
	if (pRcp->bLocateEdgeWidth) {
		pRcp->bLocateEdgeWidth = FALSE;
	}
	else {
		pRcp->bLocateEdgeWidth = TRUE;
	}
}

void CSetupProp83Dlg::OnKillfocusContrastLevel() {
	UpdateData(TRUE);
	pRcp->ContrastLevel = m_ContrastLevel;
}

void CSetupProp83Dlg::OnEnKillfocusThreshold3()
{
	UpdateData(TRUE);
	pRcp->FindBevelThres = m_FindBevelThres;
}
