// SetupProp12Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp12Dlg.h"
#include "GlobalUIContainer.h"
#include "413App.h"
#include "XTabCtrl.h"
#include "..\SYS\CUMMsg.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp12Dlg dialog

CSetupProp12Dlg::CSetupProp12Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp12Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp12Dlg)
	m_Exposure = 0;
	m_TemplateSize1 = 0;
	m_TemplateSize2 = 0;
	//}}AFX_DATA_INIT
	m_cVideo.Name = "Setup12";
}

void CSetupProp12Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp12Dlg)
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_IMAGE, m_cImgWnd);
	DDX_Control(pDX, IDC_REPORT, m_cList);
	DDX_Control(pDX, IDC_MATCH, m_cMatch);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_PATTERNB1, m_cMarkB);
	DDX_Control(pDX, IDC_PATTERNA1, m_cMarkA);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Text(pDX, IDC_EDIT1, m_Exposure);
	DDX_Text(pDX, IDC_TEMPLATESIZE, m_TemplateSize1);
	DDX_Text(pDX, IDC_TEMPLATESIZE2, m_TemplateSize2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp12Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp12Dlg)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_CLEAR2, OnClear2)
	ON_BN_CLICKED(IDC_CLEAR1, OnClear1)
	ON_BN_CLICKED(IDC_GOTOLOCATION, OnGotolocation)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL3, OnMotioncontrol3)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio7)
	ON_EN_KILLFOCUS(IDC_TEMPLATESIZE, OnKillfocusTemplatesize)
	ON_EN_KILLFOCUS(IDC_TEMPLATESIZE2, OnKillfocusTemplatesize2)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(UM_BLUEXHAIR, OnBlueXHair)
	ON_MESSAGE(UM_GREENXHAIR, OnGreenXHair)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp12Dlg message handlers

long CSetupProp12Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
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

long CSetupProp12Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		RecipeToLocal();
		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.SetRecipe(pRcp);
			m_cVideo.bShowCrossHair = TRUE;
			m_cVideo.bAllowMoveXHair = TRUE;

			m_cVideo.pWnd = this;
			m_cVideo.RSpecClear();
			m_cVideo.LoadStockDIB("BSI\\CENTER1.BMP");
		}
		bTabSelected = TRUE;
	}
	return 0;
}

BOOL CSetupProp12Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp12Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSetupProp12Dlg::RecipeToLocal() {
	m_cVideo.fTGtGrnX = g->fLPosGRnX;
	m_cVideo.fTGtGrnY = g->fLPosGRnY;
	m_cVideo.fTGtBluX = g->fLPosBLuX;
	m_cVideo.fTGtBluY = g->fLPosBLuY;

	m_TemplateSize1 = g->MWdw1;
	m_TemplateSize2 = g->MWdw2;

	m_cMarkA.DupStore1(g->MatchDIBA);
	m_cMarkB.DupStore1(g->MatchDIBB);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	switch (g->LoadPosConfType) {
	case C413Global::USEPATTMATCHING:
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
		break;
	case C413Global::USEECHOPROBE:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		break;
	default:
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(TRUE);
		break;
	}
	m_Exposure = g->fLExposure;

	UpdateData(FALSE);
}

void CSetupProp12Dlg::LocalToRecipe() {
	UpdateData(TRUE);
	g->fLExposure = m_Exposure;
}

void CSetupProp12Dlg::OnRadio2() {
	g->LoadPosConfType = C413Global::USEPATTMATCHING;
	g->SaveModule();
}

void CSetupProp12Dlg::OnRadio1() {
	g->LoadPosConfType = C413Global::USEECHOPROBE;
	g->SaveModule();
}

void CSetupProp12Dlg::OnRadio7() {
	g->LoadPosConfType = C413Global::USENONE;
	g->SaveModule();
}

BOOL CSetupProp12Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_MATCH, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL3, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cMarkA.bShowOverlay = FALSE;
	m_cMarkA.bScaled = FALSE;
	m_cMarkB.bShowOverlay = FALSE;
	m_cMarkB.bScaled = FALSE;

	m_cImgWnd.bScaled = FALSE;

	// Insert a column. This override is the most convenient.
	short idx = 0;
	m_cList.InsertColumn(idx, _T("Time       "), LVCFMT_LEFT);
	m_cList.SetColumnWidth(idx++, 96);

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("Message");
	m_cList.InsertColumn(idx, &col);
	m_cList.SetColumnWidth(idx++, 128 * 16);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

long CSetupProp12Dlg::OnBlueXHair(WPARAM wP, LPARAM lP) {
	g->fLPosBLuX = m_cVideo.fTGtBluX;
	g->fLPosBLuY = m_cVideo.fTGtBluY;
	SetB();
	RecipeToLocal();
	return 0;
}

long CSetupProp12Dlg::OnGreenXHair(WPARAM wP, LPARAM lP) {
	g->fLPosGRnX = m_cVideo.fTGtGrnX;
	g->fLPosGRnY = m_cVideo.fTGtGrnY;
	SetA();
	RecipeToLocal();
	return 0;
}

void CSetupProp12Dlg::SetA() {
	if (m_cVideo.SnapOneFrame()) {
		p413App->StoreDib1(p413App->Global.StoredDIB, g->MatchDIBA, g->MWdw1, g->MWdw1, m_cVideo.fTGtGrnX, m_cVideo.fTGtGrnY); // @@@ [11/3/2012 Yuen]
		p413App->pMtr->GetXYPos(&g->fLPosX, &g->fLPosY);
		p413App->pMtr->GetEchoTopZPosDev(g->fLPosZ);
		g->Save();
	}
}

// n starts from 0 [11/4/2012 Yuen]
void CSetupProp12Dlg::SetB() {
	if (m_cVideo.SnapOneFrame()) {
		p413App->StoreDIB2(p413App->Global.StoredDIB, g->MatchDIBB, m_cVideo.fTGtBluX, m_cVideo.fTGtBluY, g->MWdw2, g->MWdw2);
		p413App->pMtr->GetXYPos(&g->fLPosX, &g->fLPosY);
		p413App->pMtr->GetEchoTopZPosDev(g->fLPosZ);
		g->Save();
	}
}

void CSetupProp12Dlg::OnClear2() {
	m_cMarkB.ClearDIB();
	m_cMarkB.Refresh();
	g->MatchDIBB.ClearDIB();
	g->Save();
}

void CSetupProp12Dlg::OnClear1() {
	m_cMarkA.ClearDIB();
	m_cMarkA.Refresh();
	g->MatchDIBA.ClearDIB();
	g->Save();
}

void CSetupProp12Dlg::OnGotolocation() {
	p413App->pMtr->MoveEchoTopZ(g->fLPosZ, FALSE);
	p413App->GotoXYS(g->fLPosX, g->fLPosY, m_cVideo.iCID);
	p413App->pMtr->WaitEchoTopZ();
}

void CSetupProp12Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	p413App->GotoPositionecho();
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
}

void CSetupProp12Dlg::OnMatch() {
	BlockUI();
	m_cMatch.EnableWindow(FALSE);

	m_cImgWnd.ClearOverlay();

	SDoPattrnMatching Param;
	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	Param.MeSet = 0;
	Param.PaSet = 0;
	Param.pMP = NULL;
	Param.pVideo->PaOffX = Param.pVideo->PaOffY = 0;
	Param.SearchDepth = 4;
	Param.bMoveCenter = TRUE;
	Param.Par.dT = 3;	// Use default [6/8/2014 Yuen]
	Param.Par.dI = 5;	// Use default [6/8/2014 Yuen]
	Param.Par.GoF = 0;	// Not use [6/8/2014 Yuen]
	Param.MatchRes = 12;

	if (!p413App->ConfirmLoadPos(Param)) {
		AfxMessageBox("Load position can not be confirm");
	}
	m_cMatch.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp12Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
	g->fLExposure = m_Exposure = g->Exposure[m_cVideo.iCID];
	UpdateData(FALSE);
	g->Save();
}

void CSetupProp12Dlg::OnMotioncontrol3() {
	ShowMotionControl(this);
}

void CSetupProp12Dlg::OnKillfocusEdit1() {
	UpdateData(TRUE);
	g->fLExposure = m_Exposure;
	g->Save();
}

void CSetupProp12Dlg::OnKillfocusTemplatesize() {
	UpdateData(TRUE);
	g->MWdw1 = m_TemplateSize1;
}

void CSetupProp12Dlg::OnKillfocusTemplatesize2() {
	UpdateData(TRUE);
	g->MWdw2 = m_TemplateSize2;
}

long CSetupProp12Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

long CSetupProp12Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

void CSetupProp12Dlg::OnClear() {
	m_cList.DeleteAllItems();
}