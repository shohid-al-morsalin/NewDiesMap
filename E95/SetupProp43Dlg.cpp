// SetupProp43Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp43Dlg.h"
#include "413App.h"
#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "..\413\OceanOp\OceanBase.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp43Dlg dialog

CSetupProp43Dlg::CSetupProp43Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp43Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp43Dlg)
	m_IntegrationTime = 0;
	m_Boxcar = 0;
	m_Method = 0;
	m_nCycle = 0;
	m_Expand = 0;
	m_Smooth = 0;
	m_ZPos = 0.0f;
	m_NumberOfMappedPoints = 0;
	m_AutoExpose = 0;
	m_Exposure = 0;
	m_Exposure2 = 0;
	m_ODOffset = 0.0f;
	m_ODOffsetY = 0.0f;
	m_STIOffset = 0.0f;
	m_STIOffsetY = 0.0f;
	m_ZOffset = 0;
	m_TimeLimit = 0;
	m_PointTimeout = 0;
	m_FailPoint = 0;
	//}}AFX_DATA_INIT

	for (int i = 0; i < MAXTHKVLU; i++) {
		m_ThkMax[i] = 0.0f;
		m_ThkMin[i] = 0.0f;
		m_RI[i] = 3.69f;
	}
}

void CSetupProp43Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp43Dlg)
	DDX_Control(pDX, IDC_CLEARTHKRANGE2, m_cClearRg2);
	DDX_Control(pDX, IDC_CLEARTHKRANGE, m_cClearRg1);
	DDX_Control(pDX, IDC_CLEARTF2, m_cClearTF2);
	DDX_Control(pDX, IDC_CLEARTF, m_cClearTF1);
	DDX_Control(pDX, IDC_DIANOSTIC, m_cDianostic);
	DDX_Control(pDX, IDC_THKRANGE, m_cThkRange);
	DDX_Control(pDX, IDC_TAKEPICTUREONMEASURE, m_cTakePic);
	DDX_Text(pDX, IDC_FILMMEASTIME, m_IntegrationTime);
	DDX_Text(pDX, IDC_BOXCAR, m_Boxcar);
	DDX_Text(pDX, IDC_METHOD, m_Method);
	DDX_Text(pDX, IDC_NCYCLE, m_nCycle);
	DDX_Text(pDX, IDC_EXPAND, m_Expand);
	DDX_Text(pDX, IDC_SMOOTH, m_Smooth);
	DDX_Text(pDX, IDC_ZPOSITION, m_ZPos);
	DDX_Text(pDX, IDC_NUMBEROFMAPPEDPOINTS, m_NumberOfMappedPoints);
	DDX_Text(pDX, IDC_EXPOSURE3, m_AutoExpose);
	DDX_Text(pDX, IDC_EXPOSURE, m_Exposure);
	DDX_Text(pDX, IDC_EXPOSURE2, m_Exposure2);
	DDX_Text(pDX, IDC_ODOFFSET, m_ODOffset);
	DDX_Text(pDX, IDC_ODOFFSETY, m_ODOffsetY);
	DDX_Text(pDX, IDC_STIOFFSET2, m_STIOffset);
	DDX_Text(pDX, IDC_STIOFFSETY, m_STIOffsetY);
	DDX_Text(pDX, IDC_ZOFFSET, m_ZOffset);
	DDX_Control(pDX, IDC_MESETBOX1, m_cMeSet);
	DDX_Text(pDX, IDC_TIMELIMIT, m_TimeLimit);
	DDX_Text(pDX, IDC_POINTTIMEOUT, m_PointTimeout);
	DDX_Text(pDX, IDC_FAILPOINT, m_FailPoint);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FTHKMAX_SP4, m_ThkMax[0]);
	DDX_Text(pDX, IDC_FTHKMAX_SP5, m_ThkMax[1]);
	DDX_Text(pDX, IDC_FTHKMIN_SP4, m_ThkMin[0]);
	DDX_Text(pDX, IDC_FTHKMIN_SP5, m_ThkMin[1]);
	DDX_Text(pDX, IDC_FTHKMAX_SP6, m_ThkMax[2]);
	DDX_Text(pDX, IDC_FTHKMIN_SP6, m_ThkMin[2]);
	DDX_Text(pDX, IDC_FTHKMAX_SP7, m_ThkMax[3]);
	DDX_Text(pDX, IDC_FTHKMIN_SP7, m_ThkMin[3]);
	DDX_Text(pDX, IDC_FTHKMAX_SP8, m_ThkMax[4]);
	DDX_Text(pDX, IDC_FTHKMIN_SP8, m_ThkMin[4]);
	DDX_Text(pDX, IDC_FTHKMAX_SP9, m_ThkMax[5]);
	DDX_Text(pDX, IDC_FTHKMIN_SP9, m_ThkMin[5]);
	DDX_Text(pDX, IDC_RI1, m_RI[0]);
	DDX_Text(pDX, IDC_RI2, m_RI[1]);
	DDX_Text(pDX, IDC_RI3, m_RI[2]);
	DDX_Text(pDX, IDC_RI4, m_RI[3]);
	DDX_Text(pDX, IDC_RI5, m_RI[4]);
	DDX_Text(pDX, IDC_RI6, m_RI[5]);
}

BEGIN_MESSAGE_MAP(CSetupProp43Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp43Dlg)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_EN_KILLFOCUS(IDC_FILMMEASTIME, OnKillfocusFilmmeastime)
	ON_EN_KILLFOCUS(IDC_BOXCAR, OnKillfocusBoxcar)
	ON_EN_KILLFOCUS(IDC_METHOD, OnKillfocusMethodSp4)
	ON_EN_KILLFOCUS(IDC_NCYCLE, OnKillfocusNcycle)
	ON_EN_KILLFOCUS(IDC_EXPAND, OnKillfocusExpand)
	ON_EN_KILLFOCUS(IDC_SMOOTH, OnKillfocusSmooth)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio10)
	ON_BN_CLICKED(IDC_RADIO12, OnRadio12)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio7)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio8)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio9)
	ON_BN_CLICKED(IDC_RECZPOS, OnReczpos)
	ON_EN_KILLFOCUS(IDC_ZPOSITION, OnKillfocusZposition)
	ON_EN_KILLFOCUS(IDC_NUMBEROFMAPPEDPOINTS, OnKillfocusNumberofmappedpoints)
	ON_EN_KILLFOCUS(IDC_EXPOSURE3, OnKillfocusExposure3)
	ON_EN_KILLFOCUS(IDC_EXPOSURE, OnKillfocusExposure)
	ON_EN_KILLFOCUS(IDC_EXPOSURE2, OnKillfocusExposure2)
	ON_BN_CLICKED(IDC_TAKEPICTUREONMEASURE, OnTakepictureonmeasure)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_PATSET1, OnPatset1)
	ON_BN_CLICKED(IDC_PATSET2, OnPatset2)
	ON_BN_CLICKED(IDC_PATSET3, OnPatset3)
	ON_BN_CLICKED(IDC_PATSET4, OnPatset4)
	ON_BN_CLICKED(IDC_PATSET5, OnPatset5)
	ON_BN_CLICKED(IDC_PATSET6, OnPatset6)
	ON_BN_CLICKED(IDC_PATSET7, OnPatset7)
	ON_BN_CLICKED(IDC_PATSET8, OnPatset8)
	ON_BN_CLICKED(IDC_PATSET9, OnPatset9)
	ON_EN_KILLFOCUS(IDC_ODOFFSET, OnKillfocusOdoffset)
	ON_EN_KILLFOCUS(IDC_ODOFFSETY, OnKillfocusOdoffsety)
	ON_EN_KILLFOCUS(IDC_STIOFFSET2, OnKillfocusStioffset2)
	ON_EN_KILLFOCUS(IDC_STIOFFSETY, OnKillfocusStioffsety)
	ON_EN_KILLFOCUS(IDC_ZOFFSET, OnKillfocusZoffset)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP4, OnKillfocusFthkminSp4)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP5, OnKillfocusFthkminSp5)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP6, OnKillfocusFthkminSp6)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP7, OnKillfocusFthkminSp7)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP8, OnKillfocusFthkminSp8)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP9, OnKillfocusFthkminSp9)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP4, OnKillfocusFthkmaxSp4)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP5, OnKillfocusFthkmaxSp5)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP6, OnKillfocusFthkmaxSp6)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP7, OnKillfocusFthkmaxSp7)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP8, OnKillfocusFthkmaxSp8)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP9, OnKillfocusFthkmaxSp9)
	ON_EN_KILLFOCUS(IDC_RI1, OnKillfocusRi1)
	ON_EN_KILLFOCUS(IDC_RI2, OnKillfocusRi2)
	ON_EN_KILLFOCUS(IDC_RI3, OnKillfocusRi3)
	ON_EN_KILLFOCUS(IDC_RI4, OnKillfocusRi4)
	ON_EN_KILLFOCUS(IDC_RI5, OnKillfocusRi5)
	ON_EN_KILLFOCUS(IDC_RI6, OnKillfocusRi6)
	ON_BN_CLICKED(IDC_SETEXPOSURE, OnSetexposure)
	ON_BN_CLICKED(IDC_DIANOSTIC, OnDianostic)
	ON_BN_CLICKED(IDC_CLEARTHKRANGE, OnClearthkrange)
	ON_BN_CLICKED(IDC_CLEARTF, OnCleartf)
	ON_BN_CLICKED(IDC_CLEARTHKRANGE2, OnClearthkrange2)
	ON_BN_CLICKED(IDC_CLEARTF2, OnCleartf2)
	ON_EN_KILLFOCUS(IDC_TIMELIMIT, OnKillfocusTimelimit)
	ON_EN_KILLFOCUS(IDC_POINTTIMEOUT, OnKillfocusPointtimeout)
	ON_EN_KILLFOCUS(IDC_FAILPOINT, OnKillfocusFailpoint)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp43Dlg message handlers

long CSetupProp43Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp43Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		RecipeToLocal();
		bTabSelected = TRUE;
	}

	return 0;
}

BOOL CSetupProp43Dlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CSetupProp43Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp43Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (Var.UIMode > 1) {
		m_cClearRg1.ShowWindow(SW_SHOW);
		m_cClearRg2.ShowWindow(SW_SHOW);
		m_cClearTF1.ShowWindow(SW_SHOW);
		m_cClearTF2.ShowWindow(SW_SHOW);
	}
	if (g->Confg.bThinFilmMicroscopeMod) {
		m_cTakePic.ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp43Dlg::OnSaverecipe() {
	RecipeIO();
	// 	LocalToRecipe();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp43Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp43Dlg::RecipeToLocal() {
	CString str;
	short MeS = g->CurMeSet;
	short PaS = g->CurPaSet;

	str.Format("Thickness Range (um): MeSet %d", g->CurMeSet);
	m_cThkRange.SetWindowText(str);

	CMeParam* p = &pRcp->MeParam[MeS - 1];

	for (int i = 0; i < MAXTHKVLU; i++) {
		m_ThkMax[i] = p->Ta.ThkMax[i];
		m_ThkMin[i] = p->Ta.ThkMin[i];
		m_RI[i] = p->Ta.RI[i];
	}

	m_IntegrationTime = p->Ta.IntegTime;
	m_Boxcar = p->Ta.Boxcar;
	m_Method = p->Ta.Method;
	m_nCycle = pRcp->nCycle;
	m_Smooth = pRcp->NSmooth;
	m_Expand = pRcp->nExpand;
	m_NumberOfMappedPoints = pRcp->nMappedPoints;
	if (p413App->pOceanOp) p413App->pOceanOp->nFFTSP = m_NumberOfMappedPoints / p413App->pOceanOp->nFFTFac;

	m_ZPos = pRcp->fThinFilmZ;

	m_ODOffset = pRcp->POffsetX[PaS - 1][0];
	m_STIOffset = pRcp->POffsetX[PaS - 1][1];
	m_ODOffsetY = pRcp->POffsetY[PaS - 1][0];
	m_STIOffsetY = pRcp->POffsetY[PaS - 1][1];
	m_ZOffset = pRcp->TFZOffset;

	m_Exposure = pRcp->TFNormalExposure;
	m_Exposure2 = pRcp->TFTakeSpecExposure;
	m_AutoExpose = pRcp->AutoExposeTF;

	m_FailPoint = pRcp->FailCountTimeOut;
	m_TimeLimit = pRcp->WaferTimeOut;
	m_PointTimeout = pRcp->PointTimeOut;
	m_cTakePic.SetCheck(pRcp->bSaveVideo);

	str.Format("Measurement Set (MeSet) %d", MeS);
	m_cMeSet.SetWindowText(str);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(FALSE);
	switch (pRcp->FFTFilter[MeS - 1]) {
	case CSpec::NONE:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		break;
	case CSpec::HANN:
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(TRUE);
		break;
	case CSpec::HAMMING:
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(TRUE);
		break;
	case CSpec::GAUSIAN:
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(TRUE);
		break;
	case CSpec::EXPONENT:
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(TRUE);
		break;
	case CSpec::BARTLETT:
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
		break;
	case CSpec::BLACKMAN:
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(TRUE);
		break;
	}

	if (pRcp->bEchoPeakMonitor) {
		m_cDianostic.SetCheck(TRUE);
	}
	else {
		m_cDianostic.SetCheck(FALSE);
	}

	((CButton*)GetDlgItem(IDC_PATSET1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET2))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET5))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET8))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET9))->SetCheck(FALSE);
	switch (g->CurPaSet) {
	case 1:
		((CButton*)GetDlgItem(IDC_PATSET1))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_PATSET2))->SetCheck(TRUE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATSET3))->SetCheck(TRUE);
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_PATSET4))->SetCheck(TRUE);
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_PATSET5))->SetCheck(TRUE);
		break;
	case 6:
		((CButton*)GetDlgItem(IDC_PATSET6))->SetCheck(TRUE);
		break;
	case 7:
		((CButton*)GetDlgItem(IDC_PATSET7))->SetCheck(TRUE);
		break;
	case 8:
		((CButton*)GetDlgItem(IDC_PATSET8))->SetCheck(TRUE);
		break;
	case 9:
		((CButton*)GetDlgItem(IDC_PATSET9))->SetCheck(TRUE);
		break;
	}

	CButton* pButton;
	switch (g->CurMeSet) {
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_SET1);
		pButton->SetCheck(TRUE);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_SET2);
		pButton->SetCheck(TRUE);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_SET3);
		pButton->SetCheck(TRUE);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_SET4);
		pButton->SetCheck(TRUE);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_SET5);
		pButton->SetCheck(TRUE);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_SET6);
		pButton->SetCheck(TRUE);
		break;
	case 7:
		pButton = (CButton*)GetDlgItem(IDC_SET7);
		pButton->SetCheck(TRUE);
		break;
	case 8:
		pButton = (CButton*)GetDlgItem(IDC_SET8);
		pButton->SetCheck(TRUE);
		break;
	case 9:
		pButton = (CButton*)GetDlgItem(IDC_SET9);
		pButton->SetCheck(TRUE);
		break;
	}

	UpdateData(FALSE);
}

void CSetupProp43Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	short MeS = g->CurMeSet;
	short PaS = g->CurPaSet;

	CMeParam* p = &pRcp->MeParam[MeS - 1];
	for (int i = 0; i < MAXTHKVLU; i++) {
		p->Ta.RI[i] = m_RI[i];
		p->Ta.ThkMax[i] = m_ThkMax[i];
		p->Ta.ThkMin[i] = m_ThkMin[i];
	}

	p->Ta.IntegTime = m_IntegrationTime;
	p->Ta.Boxcar = m_Boxcar;
	p->Ta.Method = m_Method;
	pRcp->nCycle = m_nCycle;
	pRcp->NSmooth = m_Smooth;
	pRcp->nExpand = m_Expand;
	pRcp->nMappedPoints = m_NumberOfMappedPoints;
	pRcp->fThinFilmZ = m_ZPos;

	pRcp->POffsetX[PaS - 1][0] = m_ODOffset;
	pRcp->POffsetX[PaS - 1][1] = m_STIOffset;
	pRcp->POffsetY[PaS - 1][0] = m_ODOffsetY;
	pRcp->POffsetY[PaS - 1][1] = m_STIOffsetY;
	pRcp->TFZOffset = m_ZOffset;

	pRcp->TFNormalExposure = m_Exposure;
	pRcp->TFTakeSpecExposure = m_Exposure2;
	pRcp->AutoExposeTF = m_AutoExpose;

	pRcp->WaferTimeOut = m_TimeLimit;
	pRcp->FailCountTimeOut = m_FailPoint;
	pRcp->PointTimeOut = m_PointTimeout;
}

void CSetupProp43Dlg::OnKillfocusFilmmeastime() {
	if (!bTabSelected) {
		return;
	}
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.IntegTime = m_IntegrationTime;
	if (p413App->pOceanOp) p413App->pOceanOp->nMeastime = m_IntegrationTime;
}

void CSetupProp43Dlg::OnKillfocusBoxcar() {
	if (!bTabSelected) {
		return;
	}
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.Boxcar = m_Boxcar;
	if (p413App->pOceanOp) p413App->pOceanOp->boxcar = m_Boxcar;
}

void CSetupProp43Dlg::OnKillfocusMethodSp4() {
	if (!bTabSelected) {
		return;
	}
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.Method = m_Method;
}

void CSetupProp43Dlg::OnKillfocusNcycle() {
	UpdateData(TRUE);
	if (m_nCycle < 1) {
		m_nCycle = 1;
		UpdateData(FALSE);
	}
	pRcp->nCycle = m_nCycle;
}

void CSetupProp43Dlg::OnKillfocusExpand() {
	UpdateData(TRUE);
	if (m_Expand < 1) {
		m_Expand = 1;
		UpdateData(FALSE);
	}
	pRcp->nExpand = m_Expand;
}

void CSetupProp43Dlg::OnKillfocusSmooth() {
	UpdateData(TRUE);
	if (m_Smooth < 1) {
		m_Smooth = 1;
		UpdateData(FALSE);
	}
	pRcp->NSmooth = m_Smooth;
}

void CSetupProp43Dlg::OnRadio1() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::NONE;
}

void CSetupProp43Dlg::OnRadio10() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::EXPONENT;
}

void CSetupProp43Dlg::OnRadio12() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::BLACKMAN;
}

void CSetupProp43Dlg::OnRadio3() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::BARTLETT;
}

void CSetupProp43Dlg::OnRadio7() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::HANN;
}

void CSetupProp43Dlg::OnRadio8() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::HAMMING;
}

void CSetupProp43Dlg::OnRadio9() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::GAUSIAN;
}

void CSetupProp43Dlg::OnReczpos() {
	UpdateData(TRUE);
	if (p413App->pMtr) p413App->pMtr->GetThinFilmZPosDev(m_ZPos);
	pRcp->fThinFilmZ = m_ZPos;
	UpdateData(FALSE);
}

void CSetupProp43Dlg::OnKillfocusZposition() {
	UpdateData(TRUE);
	pRcp->fThinFilmZ = m_ZPos;
}

void CSetupProp43Dlg::OnKillfocusNumberofmappedpoints() {
	UpdateData(TRUE);

	if (m_NumberOfMappedPoints > 65536) {
		m_NumberOfMappedPoints = 65536;
	}
	else if (m_NumberOfMappedPoints > 32768) {
		m_NumberOfMappedPoints = 32768;
	}
	else if (m_NumberOfMappedPoints > 16384) {
		m_NumberOfMappedPoints = 16384;
	}
	else if (m_NumberOfMappedPoints > 8192) {
		m_NumberOfMappedPoints = 8192;
	}
	else if (m_NumberOfMappedPoints > 4096) {
		m_NumberOfMappedPoints = 4096;
	}
	else if (m_NumberOfMappedPoints > 2048) {
		m_NumberOfMappedPoints = 2048;
	}
	else if (m_NumberOfMappedPoints > 1024) {
		m_NumberOfMappedPoints = 1024;
	}
	else {
		m_NumberOfMappedPoints = 512;
	}
	UpdateData(FALSE);

	pRcp->nMappedPoints = m_NumberOfMappedPoints;
	if (p413App->pOceanOp) p413App->pOceanOp->nFFTSP = m_NumberOfMappedPoints / p413App->pOceanOp->nFFTFac;
}

void CSetupProp43Dlg::OnKillfocusExposure3() {
	UpdateData(TRUE);
	pRcp->AutoExposeTF = m_AutoExpose;
}

void CSetupProp43Dlg::OnSetexposure() {
	m_Exposure = pRcp->TFNormalExposure = g->Exposure[1];
	UpdateData(FALSE);
}

void CSetupProp43Dlg::OnKillfocusExposure() {
	UpdateData(TRUE);
	pRcp->TFNormalExposure = m_Exposure;
}

void CSetupProp43Dlg::OnKillfocusExposure2() {
	UpdateData(TRUE);
	pRcp->TFTakeSpecExposure = m_Exposure2;
}

void CSetupProp43Dlg::OnTakepictureonmeasure() {
	if (pRcp->bSaveVideo) {
		pRcp->bSaveVideo = FALSE;
	}
	else {
		pRcp->bSaveVideo = TRUE;
	}
}

void CSetupProp43Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp43Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp43Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp43Dlg::OnSet4() {
	SwapMeSet(4);
}

void CSetupProp43Dlg::OnSet5() {
	SwapMeSet(5);
}

void CSetupProp43Dlg::OnSet6() {
	SwapMeSet(6);
}

void CSetupProp43Dlg::OnSet7() {
	SwapMeSet(7);
}

void CSetupProp43Dlg::OnSet8() {
	SwapMeSet(8);
}

void CSetupProp43Dlg::OnSet9() {
	SwapMeSet(9);
}

void CSetupProp43Dlg::OnPatset1() {
	g->CurPaSet = 1;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset2() {
	g->CurPaSet = 2;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset3() {
	g->CurPaSet = 3;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset4() {
	g->CurPaSet = 4;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset5() {
	g->CurPaSet = 5;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset6() {
	g->CurPaSet = 6;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset7() {
	g->CurPaSet = 7;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset8() {
	g->CurPaSet = 8;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnPatset9() {
	g->CurPaSet = 9;
	RecipeToLocal();
}

void CSetupProp43Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}

void CSetupProp43Dlg::OnKillfocusOdoffset() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][0] = m_ODOffset;
}

void CSetupProp43Dlg::OnKillfocusOdoffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][0] = m_ODOffsetY;
}

void CSetupProp43Dlg::OnKillfocusStioffset2() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][1] = m_STIOffset;
}

void CSetupProp43Dlg::OnKillfocusStioffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][1] = m_STIOffsetY;
}

void CSetupProp43Dlg::OnKillfocusZoffset() {
	UpdateData(TRUE);
	pRcp->TFZOffset = m_ZOffset;
}

void CSetupProp43Dlg::OnKillfocusFthkminSp4() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[0] = m_ThkMin[0];
}

void CSetupProp43Dlg::OnKillfocusFthkminSp5() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[1] = m_ThkMin[1];
}

void CSetupProp43Dlg::OnKillfocusFthkminSp6() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[2] = m_ThkMin[2];
}

void CSetupProp43Dlg::OnKillfocusFthkminSp7() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[3] = m_ThkMin[3];
}

void CSetupProp43Dlg::OnKillfocusFthkminSp8() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[4] = m_ThkMin[4];
}

void CSetupProp43Dlg::OnKillfocusFthkminSp9() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMin[5] = m_ThkMin[5];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp4() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[0] = m_ThkMax[0];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp5() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[1] = m_ThkMax[1];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp6() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[2] = m_ThkMax[2];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp7() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[3] = m_ThkMax[3];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp8() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[4] = m_ThkMax[4];
}

void CSetupProp43Dlg::OnKillfocusFthkmaxSp9() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.ThkMax[5] = m_ThkMax[5];
}

void CSetupProp43Dlg::OnKillfocusRi1() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[0] = m_RI[0];
}

void CSetupProp43Dlg::OnKillfocusRi2() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[1] = m_RI[1];
}

void CSetupProp43Dlg::OnKillfocusRi3() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[2] = m_RI[2];
}

void CSetupProp43Dlg::OnKillfocusRi4() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[3] = m_RI[3];
}

void CSetupProp43Dlg::OnKillfocusRi5() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[4] = m_RI[4];
}

void CSetupProp43Dlg::OnKillfocusRi6() {
	UpdateData(TRUE);
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.RI[5] = m_RI[5];
}

// void CSetupProp43Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp43Dlg::OnDianostic() {
	if (pRcp->bEchoPeakMonitor) {
		pRcp->bEchoPeakMonitor = FALSE;
		m_cDianostic.SetCheck(FALSE);
	}
	else {
		pRcp->bEchoPeakMonitor = TRUE;
		m_cDianostic.SetCheck(TRUE);
	}
}

void CSetupProp43Dlg::OnClearthkrange() {
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	for (int i = 0; i < 6; i++) {
		p->Ta.RI[i] = 0;
		p->Ta.ThkMin[i] = 0;
		p->Ta.ThkMax[i] = 0;
	}
	RecipeToLocal();
}

void CSetupProp43Dlg::OnCleartf() {
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	p->Ta.IntegTime = 500;
	p->Ta.Boxcar = 1;
	p->Ta.Method = 2;

	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::NONE;

	RecipeToLocal();
}

void CSetupProp43Dlg::OnClearthkrange2() {
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* p = &pRcp->MeParam[j];
		for (int i = 0; i < 6; i++) {
			p->Ta.RI[i] = 0;
			p->Ta.ThkMin[i] = 0;
			p->Ta.ThkMax[i] = 0;
		}
	}
	RecipeToLocal();
}

void CSetupProp43Dlg::OnCleartf2() {
	for (int i = 0; i < MAXMEPASET; i++) {
		CMeParam* p = &pRcp->MeParam[i];
		p->Ta.IntegTime = 500;
		p->Ta.Boxcar = 1;
		p->Ta.Method = 2;
		pRcp->FFTFilter[i] = CSpec::NONE;
	}
	RecipeToLocal();
}

void CSetupProp43Dlg::OnKillfocusTimelimit() {
	UpdateData(TRUE);
	pRcp->WaferTimeOut = m_TimeLimit;
}

void CSetupProp43Dlg::OnKillfocusPointtimeout() {
	UpdateData(TRUE);
	pRcp->PointTimeOut = m_PointTimeout;
}

void CSetupProp43Dlg::OnKillfocusFailpoint() {
	UpdateData(TRUE);
	pRcp->FailCountTimeOut = m_FailPoint;
}