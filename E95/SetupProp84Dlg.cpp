// SetupProp84Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp84Dlg.h"

#include "413App.h"
#include "XTabCtrl.h"

#include "..\SYS\CUMMsg.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "..\413\MotorS2.h"
#include "TitlePanel.h"
#include "..\SRC\LogInDataNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp84Dlg dialog

CSetupProp84Dlg::CSetupProp84Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp84Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp84Dlg)
	m_Bevel = 0.0f;
	m_CR = 0.0f;
	m_CX = 0.0f;
	m_CY = 0.0f;
	m_Exposure = 0;
	m_ZPos = 0.0f;
	m_Method = 1;
	m_Thres = 0.0f;
	m_WaferSize = 0.0f;
	//}}AFX_DATA_INIT
	m_cVideo.Name = "Setup84";
}

void CSetupProp84Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp84Dlg)
	DDX_Control(pDX, IDC_USECENTERING, m_cUseWaferAlign);
	DDX_Control(pDX, IDC_FINDCXCY, m_cAlgn);
	DDX_Control(pDX, IDC_IMAGEVIEW, m_cImgWnd);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Text(pDX, IDC_BEVEL, m_Bevel);
	DDX_Text(pDX, IDC_CR, m_CR);
	DDX_Text(pDX, IDC_CX, m_CX);
	DDX_Text(pDX, IDC_CY, m_CY);
	DDX_Text(pDX, IDC_WALIGNEXPOSURE, m_Exposure);
	DDX_Text(pDX, IDC_ZPOS1, m_ZPos);
	DDX_Text(pDX, IDC_METHOD, m_Method);
	DDV_MinMaxInt(pDX, m_Method, 1, 2);
	DDX_Text(pDX, IDC_THRESHOLD, m_Thres);
	DDX_Text(pDX, IDC_WAFERSIZE, m_WaferSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp84Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp84Dlg)
	ON_BN_CLICKED(IDC_FINDCXCY, OnFindcxcy)
	ON_BN_CLICKED(IDC_CLEAROFFSET, OnClearoffset)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL1, OnMotioncontrol1)
	ON_EN_KILLFOCUS(IDC_WALIGNEXPOSURE, OnKillfocusWalignexposure)
	ON_EN_KILLFOCUS(IDC_ZPOS1, OnKillfocusZpos1)
	ON_BN_CLICKED(IDC_ZPOS, OnZpos)
	ON_BN_CLICKED(IDC_EXPOSURE, OnExposure)
	ON_EN_KILLFOCUS(IDC_WAFERSIZE, OnKillfocusWafersize)
	ON_BN_CLICKED(IDC_ADJLOADPOS, OnAdjloadpos)
	ON_EN_KILLFOCUS(IDC_METHOD, OnKillfocusMethod)
	ON_EN_KILLFOCUS(IDC_THRESHOLD, OnKillfocusThreshold)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_USECENTERING, OnUsecentering)
	ON_BN_CLICKED(IDC_DARKF, OnDarkf)
	ON_BN_CLICKED(IDC_DARKB, OnDarkb)
	ON_BN_CLICKED(IDC_BRIGHTF, OnBrightf)
	ON_BN_CLICKED(IDC_BRIGHTB, OnBrightb)
	ON_BN_CLICKED(IDC_TESTRSL, OnTestrsl)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp84Dlg message handlers

long CSetupProp84Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StartCamera(CCamBase::ECHOCAM);
			m_cVideo.pWnd = this;
			m_cVideo.bShowCrossHair = FALSE;
			m_cVideo.bShowCenterTarget = TRUE;
		}

		RecipeToLocal();

		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp84Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}

		UpdateData(TRUE);
		pRcp->FindEdgeWaferSize = m_WaferSize;
		g->FindEdgeThres = m_Thres;
		pRcp->bUseWaferAlign = m_cUseWaferAlign.GetCheck();
	}

	bTabSelected = FALSE;

	return 0;
}

void CSetupProp84Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp84Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_FINDCXCY, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL1, TOP_RIGHT);
	AddAnchor(IDC_ADJLOADPOS, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	RecipeToLocal();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp84Dlg::StartCamera(CCamBase::CAMERAID No) {
	p413App->StartCamera(&m_cVideo, No, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	switch (No) {
	default:
		m_cVideo.XX = 3;
		break;
	case CCamBase::THINFCAM:
		m_cVideo.XX = 4;
		break;
	}
	if (m_cVideo.IsSimu()) {
		m_cVideo.LoadSimuDIB(m_cVideo.XX);
	}
}

void CSetupProp84Dlg::RecipeToLocal() {
	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Get(m_CX, m_CY);
		//m_CX = g->CXOffsetEcho;
		//m_CY = g->CYOffsetEcho;
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Get(m_CX, m_CY);
		//m_CX = g->CXOffsetTF;
		//m_CY = g->CYOffsetTF;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->COffCWL.Get(m_CX, m_CY);
		break;
	case CCamBase::ROUGH:
		g->COffRough.Get(m_CX, m_CY);
		break;
	// ]
	}
	m_Bevel = g->BevelWidth;
	m_Exposure = g->AlgnExposure;
	m_ZPos = g->fAlignZEcho;
	m_WaferSize = pRcp->FindEdgeWaferSize;
	m_Method = pRcp->FindEdgeMethod;
	m_Thres = g->FindEdgeThres;

	m_cUseWaferAlign.SetCheck(pRcp->bUseWaferAlign);

	((CButton*)GetDlgItem(IDC_DARKF))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_DARKB))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_BRIGHTF))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_BRIGHTB))->SetCheck(FALSE);
	switch (pRcp->FindEdgeType) {
	case CImgWnd::DARKFWD:
		((CButton*)GetDlgItem(IDC_DARKF))->SetCheck(TRUE);
		break;
	case CImgWnd::DARKBWD:
		((CButton*)GetDlgItem(IDC_DARKB))->SetCheck(TRUE);
		break;
	case CImgWnd::BRITFWD:
		((CButton*)GetDlgItem(IDC_BRIGHTF))->SetCheck(TRUE);
		break;
	case CImgWnd::BRITBWD:
		((CButton*)GetDlgItem(IDC_BRIGHTB))->SetCheck(TRUE);
		break;
	}
	((CButton*)GetDlgItem(IDC_TESTRSL))->SetCheck(pRcp->bTestFindEdgeResult);

	UpdateData(FALSE);
}

void CSetupProp84Dlg::OnFindcxcy() {
	BlockUI();
	m_cAlgn.EnableWindow(FALSE);

	m_cImgWnd.ClearTextInfo();
	SFINDCENTER SFC;
	SFC.pCXOffset = &m_CX;
	SFC.pCYOffset = &m_CY;
	SFC.pCRadius = &m_CR;
	SFC.pRcp = pRcp;
	SFC.pVideo = &m_cVideo;
	SFC.pImgWnd = &m_cImgWnd;
	SFC.pList = NULL;
	SFC.Method = pRcp->FindEdgeType;
	SFC.bTest = FALSE;
	switch (pRcp->FindEdgeMethod) {
	case 2:
		if (p413App->FindWaferCenterUsingEdgeDetectionFast(SFC)) {
		}
		break;
	default:
		if (p413App->FindWaferCenterUsingEdgeDetection(SFC)) {
		}
		break;
	}
	RecipeToLocal();
	m_cAlgn.EnableWindow(TRUE);
	ReleaseUI();
}

BOOL CSetupProp84Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp84Dlg::OnClearoffset() {
	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Clear();
		//g->CXOffsetEcho = 0;
		//g->CYOffsetEcho = 0;
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Clear();
		//g->CXOffsetTF = 0;
		//g->CYOffsetTF = 0;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->COffCWL.Clear();
		break;
	case CCamBase::ROUGH:
		g->COffRough.Clear();
		break;
	// ]
	}
	m_CR = 0;
	g->BevelWidth = 0;

	g->SaveMotorOption();

	RecipeToLocal();
}

void CSetupProp84Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
	m_Exposure = g->AlgnExposure = m_cVideo.GetExposure();
	g->SaveVideoOption();
	UpdateData(FALSE);
}

void CSetupProp84Dlg::OnMotioncontrol1() {
	ShowMotionControl(this);
}

void CSetupProp84Dlg::OnKillfocusWalignexposure() {
	UpdateData(TRUE);
	g->AlgnExposure = m_Exposure;
	g->SaveVideoOption();
}

void CSetupProp84Dlg::OnKillfocusZpos1() {
	UpdateData(TRUE);
	g->fAlignZEcho = m_ZPos;
	g->SaveVideoOption();
}

void CSetupProp84Dlg::OnZpos() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		pMtr->GetEchoTopZPosDev(g->fAlignZEcho);
		m_ZPos = g->fAlignZEcho;
		g->SaveVideoOption();
		UpdateData(FALSE);
	}
}

void CSetupProp84Dlg::OnExposure() {
	m_Exposure = g->AlgnExposure = m_cVideo.GetExposure();
	UpdateData(FALSE);
}

void CSetupProp84Dlg::OnKillfocusWafersize() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		pUDev->pTitlePanel->AskLogin();
		if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	UpdateData(TRUE);
	pRcp->FindEdgeWaferSize = m_WaferSize;
	g->SaveModule();
}

void CSetupProp84Dlg::OnAdjloadpos() {
	if (AfxMessageBox("WARNING\nWrong 'Load Pos' can potentially break wafer\nClick 'Yes' to modify 'Load Pos'base on\ncalculated wafer center", MB_YESNO) == IDYES) {
		CString str;
		p413App->Log("User accepted option to change 'Load Pos'", 2);
		str.Format("Initial LoadPos: X=%.3f, Y=%.3f", g->LoadPos.PX, g->LoadPos.PY);
		p413App->Log(str, 2);
		UpdateData(TRUE);
		switch (g->ActiveCamera) {
		default:
			g->LoadPos.PX -= g->COffEcho.X;
			g->LoadPos.PY -= g->COffEcho.Y;
			OnClearoffset();
			break;
		case CCamBase::THINFCAM:
			g->LoadPos.PX -= g->COffTF.X;
			g->LoadPos.PY -= g->COffTF.Y;
			OnClearoffset();
			break;
		// [05082024 ZHIMING
		case CCamBase::CWL:
			g->LoadPos.PX -= g->COffCWL.X;
			g->LoadPos.PY -= g->COffCWL.Y;
			OnClearoffset();
			break;
		case CCamBase::ROUGH:
			g->LoadPos.PX -= g->COffRough.X;
			g->LoadPos.PY -= g->COffRough.Y;
			OnClearoffset();
			break;
		// ]
		}

		str.Format("New LoadPos: X=%.3f, Y=%.3f", g->LoadPos.PX, g->LoadPos.PY);
		p413App->Log(str, 2);
	}
}

void CSetupProp84Dlg::OnKillfocusMethod() {
	UpdateData(TRUE);
	pRcp->FindEdgeMethod = m_Method;
	g->SaveMeasurementOption();
}

void CSetupProp84Dlg::OnKillfocusThreshold() {
	UpdateData(TRUE);
	g->FindEdgeThres = m_Thres;
	g->SaveVideoOption();
}

void CSetupProp84Dlg::OnSaverecipe() {
	LocalToRecipe();
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp84Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp84Dlg::OnUsecentering() {
	if (pRcp->bUseWaferAlign) {
		pRcp->bUseWaferAlign = FALSE;
	}
	else {
		pRcp->bUseWaferAlign = TRUE;
	}
}

void CSetupProp84Dlg::OnDarkf() {
	pRcp->FindEdgeType = CImgWnd::DARKFWD;
	//	RecipeToLocal();
}

void CSetupProp84Dlg::OnDarkb() {
	pRcp->FindEdgeType = CImgWnd::DARKBWD;
	//	RecipeToLocal();
}

void CSetupProp84Dlg::OnBrightf() {
	pRcp->FindEdgeType = CImgWnd::BRITFWD;
	//	RecipeToLocal();
}

void CSetupProp84Dlg::OnBrightb() {
	pRcp->FindEdgeType = CImgWnd::BRITBWD;
	//	RecipeToLocal();
}

void CSetupProp84Dlg::OnTestrsl() {
	if (pRcp->bTestFindEdgeResult) {
		pRcp->bTestFindEdgeResult = FALSE;
	}
	else {
		pRcp->bTestFindEdgeResult = TRUE;
	}
	RecipeToLocal();
}