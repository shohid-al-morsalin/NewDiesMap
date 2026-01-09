// SetupProp15Dlg.cpp : implementation file
//
#include "stdafx.h"
#include "e95.h"
#include "SetupProp15Dlg.h"


#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "413/MotorS2.h"
#include "RoughProbe.h"
#include "Data.h"
#include "MPoint.h"
#include "afxcoll.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//[ define from thred to measure roughness
#define CHECKTIME 500
#define IDT_CHECKSIGNALS 9999
//]

// CSetupProp15Dlg dialog -- Added By Mahedi Kamal - 03202024

CSetupProp15Dlg::CSetupProp15Dlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(CSetupProp15Dlg::IDD, pParent)
{
	bTimer = FALSE;
}


CSetupProp15Dlg::~CSetupProp15Dlg() {}

void CSetupProp15Dlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap); // waferView
	DDX_Control(pDX, IDC_LIST1, m_cPoint); // data point view
	//[ for showing data in roughness tab
	if (p413App->pRough) {
		DDX_Text(pDX, IDC_NMEAS, p413App->pRough->HWPar.Nmeas);
		DDV_MinMaxInt(pDX, p413App->pRough->HWPar.Nmeas, 1, 10000);
		DDX_Text(pDX, IDC_OFFSETCALIBR, p413App->pRough->HWPar.fRdOffset);
		DDV_MinMaxDouble(pDX, p413App->pRough->HWPar.fRdOffset, -10., 10.);
		DDX_Text(pDX, IDC_OFFSETCALIBR2, p413App->pRough->HWPar.fRsOffset);
		DDV_MinMaxDouble(pDX, p413App->pRough->HWPar.fRsOffset, -10., 10.);
		DDX_Text(pDX, IDC_SIGMACALIBR, p413App->pRough->HWPar.SigmaCalibr1);
		DDV_MinMaxDouble(pDX, p413App->pRough->HWPar.SigmaCalibr1, 1., 100000.);
		DDX_Text(pDX, IDC_SIGMACALIBR2, p413App->pRough->HWPar.SigmaCalibr2);
		DDV_MinMaxDouble(pDX, p413App->pRough->HWPar.SigmaCalibr2, 1., 100000.);
		DDX_Text(pDX, IDC_K1, p413App->pRough->HWPar.kRefl1);
		DDX_Text(pDX, IDC_K2, p413App->pRough->HWPar.kRefl2);
	}
	DDX_Text(pDX, IDC_ROUGHNESS_Z_POS, fRoughZPos);
	//]
}


BEGIN_MESSAGE_MAP(CSetupProp15Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp15Dlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_MEASURE_ROUGHNESS, &CSetupProp15Dlg::OnBnClickedMeasureRoughness)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_BN_CLICKED(IDC_READ, &CSetupProp15Dlg::OnBnClickedRead)
	ON_BN_CLICKED(IDC_ROUGHNESS_TIMER, &CSetupProp15Dlg::OnRoughnessTimer)
	//Updating with UI change
	ON_EN_KILLFOCUS(IDC_NMEAS, OnKillfocusNmeas)
	ON_EN_KILLFOCUS(IDC_OFFSETCALIBR, OnKillfocusfReflOffset)
	ON_EN_KILLFOCUS(IDC_OFFSETCALIBR2, OnKillfocusfScatOffset)
	ON_EN_KILLFOCUS(IDC_SIGMACALIBR, OnKillfocusSigmacalibr)
	ON_EN_KILLFOCUS(IDC_SIGMACALIBR2, OnKillfocusSigmacalibr2)
	//]
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ROUGH_CALIBRATE, &CSetupProp15Dlg::OnCalibration)
	ON_BN_CLICKED(IDC_ROUGH_CALIBRATE2, &CSetupProp15Dlg::OnCalibration2)
	ON_BN_CLICKED(IDC_15DLG_RA, &CSetupProp15Dlg::OnBnClicked15dlgRa)
	ON_BN_CLICKED(IDC_15DLG_RMS, &CSetupProp15Dlg::OnBnClicked15dlgRms)
//	ON_WM_NCDESTROY()
ON_BN_CLICKED(IDC_MOTIONCONTROL1, &CSetupProp15Dlg::OnBnClickedMotioncontrol1)
ON_BN_CLICKED(IDC_ROUGH_ZPOS, &CSetupProp15Dlg::OnBnClickedRoughZpos)
ON_EN_KILLFOCUS(IDC_ROUGHNESS_Z_POS, &CSetupProp15Dlg::OnKillfocusRoughnessZPos)
ON_BN_CLICKED(IDC_ROUGHNESS_RECIPE, &CSetupProp15Dlg::OnBnClickedRoughnessRecipe)
ON_BN_CLICKED(IDC_ROUGHNESS_RESET, &CSetupProp15Dlg::OnBnClickedRoughnessReset)
END_MESSAGE_MAP()


long CSetupProp15Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp15Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	/* default recipe to show on wafermap*/
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!p413App->pRough->HWPar.bCalibType) {
		GetDlgItem(IDC_SIGMACALIBR2)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROUGH_CALIBRATE2)->EnableWindow(FALSE);
	}

	if (!bTabSelected) {
		g->ActiveCamera = CCamBase::ROUGH;
		m_cWaferMap.pRcp = pRcp;
		RecipeToLocal();
		bTabSelected = TRUE;
	}

	UpdateData(FALSE);

	return 0;
}

BOOL CSetupProp15Dlg::DestroyWindow() {

	return CResizableDialog::DestroyWindow();
}


BOOL CSetupProp15Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	//[ setting position
	AddAnchor(IDC_READ, TOP_RIGHT);
	AddAnchor(IDC_MEASURE_ROUGHNESS, TOP_RIGHT);
	AddAnchor(IDC_ROUGHNESS_TIMER, TOP_RIGHT);
	AddAnchor(IDC_WAFERMAP, TOP_LEFT); 
	AddAnchor(IDC_MOTIONCONTROL1, TOP_RIGHT);
	AddAnchor(IDC_ROUGHNESS_RECIPE, TOP_RIGHT);
	//]

	g = &p413App->Global;
	pRcp = &g->RcpSetup;
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE;
	m_cWaferMap.pParent = this;


	//[ for list control
	char* dwCjName[] =
	{
		"#", "Position", "Me", "Pat"
	};
	int nSize[] = { 32, 160, 32, 32 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cPoint.InsertColumn(i, &nListColumn);
	}
	m_cPoint.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	//]


	//RenewParams(); // for showing initil values of roughness tab 

	if (p413App->bIsRa == false) {
		CButton* pBtn = (CButton*)GetDlgItem(IDC_15DLG_RMS);
		pBtn->SetCheck(1);
	}
	else {
		CButton* pBtn = (CButton*)GetDlgItem(IDC_15DLG_RA);
		pBtn->SetCheck(1);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSetupProp15Dlg::RenewParams() {
	if (p413App->pRough) {
		RH = p413App->pRough->Read(p413App->pRough->HWPar.Nmeas);
		_Vf1 = p413App->pRough->VRs;
		_Vf2 = p413App->pRough->VRd;
		UpdateSigmaRoughness();
	}
}

void CSetupProp15Dlg::UpdateSigmaRoughness() {
	CString str;
	str.Format("%.5f", _Vf1);
	GetDlgItem(IDC_VREFLECTED1)->SetWindowText(str.GetBuffer());
	str.Format("%.5f", _Vf2);
	GetDlgItem(IDC_VSCATTERED1)->SetWindowText(str.GetBuffer());
	str.Format("%.4f", RH);
	GetDlgItem(IDC_15DLG_ROUGH_READING)->SetWindowText(str.GetBuffer());
}

void CSetupProp15Dlg::LocalToRecipe() {
	if (pRcp) {
		pRcp->fRoughZ = fRoughZPos;
	}
}
void CSetupProp15Dlg::RecipeToLocal() {
	if (pRcp) {
		fRoughZPos = pRcp->fRoughZ;
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
		//[ 07102024 -> Mahedi Kamal
		p413App->bIsRa = pRcp->bIsRa_rcp; 
		if (p413App->bIsRa)
			CheckRadioButton(IDC_15DLG_RMS, IDC_15DLG_RA, IDC_15DLG_RA);
		else
			CheckRadioButton(IDC_15DLG_RMS, IDC_15DLG_RA, IDC_15DLG_RMS);
		//]
	}
	UpdateData(FALSE);
}




/* measure roughness for recipe*/
void CSetupProp15Dlg::OnBnClickedMeasureRoughness()
{
	/*
	Algorithm:
		If Roughness recipe is set: 
			If roughness probe is Not Started try to start:(connect with port)
				Perform Roughness Calibration()
				Measure Roughness For All Points()
			else Show error
		else Show error Message
		Stop Roughness probe
	*/
	CString mgs;
	p413App->PerformRoughnessCalibration(pRcp);
	//CObList MPList; // = pRcp->MPList
	CPtrList * pMPList = &pRcp->Patt.MP.MPLst;
	//MeasureRoughMPList(pRcp, pMPList);
	p413App->MeasureRoughMPList(pRcp, pMPList);

}

long CSetupProp15Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

void CSetupProp15Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

// Measure Roughness One time on button click
void CSetupProp15Dlg::OnBnClickedRead()
{
	if (!p413App->pRough) {
		ASSERT(0); return;
	}
	RH = p413App->pRough->Read(p413App->pRough->HWPar.Nmeas);
	_Vf1 = p413App->pRough->VRs; _Vf2 = p413App->pRough->VRd;
	UpdateSigmaRoughness();
}

// Start measuring roughness continuously once button is clicked
void CSetupProp15Dlg::OnRoughnessTimer()
{
	if (bTimer) {
		KillTimer(IDT_CHECKSIGNALS);
		SetDlgItemText(IDC_ROUGHNESS_TIMER, "Start");
	}
	else {
		SetTimer(IDT_CHECKSIGNALS, CHECKTIME, NULL);
		SetDlgItemText(IDC_ROUGHNESS_TIMER, "Stop");
	}
	bTimer = !bTimer;
}
void CSetupProp15Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_CHECKSIGNALS) {
		KillTimer(IDT_CHECKSIGNALS);
		RenewParams();
		UpdateData(false);
		SetTimer(IDT_CHECKSIGNALS, CHECKTIME, NULL);
	}
}

void CSetupProp15Dlg::OnKillfocusNmeas() {
	UpdateData(TRUE);
	p413App->pRough->HWPar.SaveParam();
}

void CSetupProp15Dlg::OnKillfocusfReflOffset() {
	UpdateData(TRUE);
	p413App->pRough->HWPar.SaveParam();
}

void CSetupProp15Dlg::OnKillfocusfScatOffset() {
	UpdateData(TRUE);
	p413App->pRough->HWPar.SaveParam();
}

void CSetupProp15Dlg::OnKillfocusSigmacalibr() {
	UpdateData(TRUE);
	p413App->pRough->HWPar.SaveParam();
}

void CSetupProp15Dlg::OnKillfocusSigmacalibr2() {
	UpdateData(TRUE);
	p413App->pRough->HWPar.SaveParam();
}

void CSetupProp15Dlg::OnCalibration(){
	CString msg;
	p413App->pRough->CalibrateRH(32, msg);
	UpdateData(FALSE);
	GetDlgItem(IDC_MESSAGE)->SetWindowText(msg);
}


void CSetupProp15Dlg::OnCalibration2()
{
	double lambda = 0.67, theta = 0, pi = 3.14159265358, a;
	CRouPar* pHWPar = &p413App->pRough->HWPar;
	a = 4 * pi * cos(theta) * pHWPar->SigmaCalibr2 / lambda;
	if ((_Vf1 > 0) && (_Vf2 > 0) && (a > 0))
		pHWPar->kRefl2 = _Vf1 / _Vf2 * a * a;
}


void CSetupProp15Dlg::OnBnClicked15dlgRa()
{
	p413App->bIsRa = true;
	pRcp->bIsRa_rcp = p413App->bIsRa; // 07102024 -> Mahedi Kamal
}


void CSetupProp15Dlg::OnBnClicked15dlgRms()
{
	p413App->bIsRa = false;
	pRcp->bIsRa_rcp = p413App->bIsRa; // 07102024 -> Mahedi Kamal
}

void CSetupProp15Dlg::PostNcDestroy()
{
	CResizableDialog::PostNcDestroy();
	delete this;
}


void CSetupProp15Dlg::OnBnClickedMotioncontrol1()
{
	ShowMotionControl(this);
}


void CSetupProp15Dlg::OnBnClickedRoughZpos()
{
	if (p413App->pMtr) p413App->pMtr->GetRoughZPosDev(fRoughZPos);
	pRcp->fRoughZ = fRoughZPos;
	UpdateData(FALSE);
}


void CSetupProp15Dlg::OnKillfocusRoughnessZPos()
{
	UpdateData(TRUE);
	pRcp->fRoughZ = fRoughZPos;
}


void CSetupProp15Dlg::OnBnClickedRoughnessRecipe()
{
	RecipeIO();
}


void CSetupProp15Dlg::OnBnClickedRoughnessReset()
{
	if (!p413App->pRough) return;
	CRouPar* pHWPar = &p413App->pRough->HWPar;
	if (!pHWPar) return;
	pHWPar->kRefl1 = 1.0f;
	pHWPar->kRefl2 = 1.0f;
}
