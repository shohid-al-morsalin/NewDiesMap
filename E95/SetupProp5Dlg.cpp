// SetupProp5Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "SetupProp5Dlg.h"
#include "413App.h"
#include "MotionControl.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "SiCamSettingDlg.h"
#include "..\SYS\CUMMsg.h"
#include "CrossHair.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"
#include "..\413\OceanOp\OceanBase.h"
#include "..\413\MotorS2.h"

#include "MPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp5Dlg dialog

CSetupProp5Dlg::CSetupProp5Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp5Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp5Dlg)
	m_TemplateSize = 0;
	m_AutoExpose = 0;
	m_TemplateSize2 = 0;
	m_Enhance = FALSE;
	m_BWTemplate = FALSE;
	//}}AFX_DATA_INIT

	dx = 0.001f;
	// 	fTGtGrnX = fTGtGrnY = 0.5;
	// 	fTGtBluX = fTGtBluY = 0.5;
	m_cVideo.Name = "Setup5";
}

void CSetupProp5Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp5Dlg)
	DDX_Control(pDX, IDC_PATTERNB4, m_cMarkD2);
	DDX_Control(pDX, IDC_PATTERNA4, m_cMarkD1);
	DDX_Control(pDX, IDC_PASET4, m_cPaSet4);
	DDX_Control(pDX, IDC_IMAGE, m_cImgWnd);
	DDX_Control(pDX, IDC_MESSAGE3, m_cList);
	DDX_Control(pDX, IDC_REDTGT2, m_cRed2);
	DDX_Control(pDX, IDC_REDTGT, m_cRed);
	DDX_Control(pDX, IDC_PASET1, m_cPaSet1);
	DDX_Control(pDX, IDC_PASET2, m_cPaSet2);
	DDX_Control(pDX, IDC_PASET3, m_cPaSet3);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_EXPOSURE4, m_ciAutoExpose);
	DDX_Control(pDX, IDC_MATCH, m_cMatch);
	DDX_Control(pDX, IDC_AUTOEXPOSE, m_cAutoExposure);
	DDX_Control(pDX, IDC_AUTOFOCUS1, m_cAutoFocus);
	DDX_Control(pDX, IDC_MEASURETF, m_cMeasure);
	DDX_Control(pDX, IDC_SPEED, m_cSpeed);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_PATTERNA1, m_cMarkA1);
	DDX_Control(pDX, IDC_PATTERNB1, m_cMarkA2);
	DDX_Control(pDX, IDC_PATTERNA2, m_cMarkB1);
	DDX_Control(pDX, IDC_PATTERNB2, m_cMarkB2);
	DDX_Control(pDX, IDC_PATTERNA3, m_cMarkC1);
	DDX_Control(pDX, IDC_PATTERNB3, m_cMarkC2);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Text(pDX, IDC_TEMPLATESIZE, m_TemplateSize);
	DDX_Text(pDX, IDC_EXPOSURE4, m_AutoExpose);
	DDX_Text(pDX, IDC_TEMPLATESIZE2, m_TemplateSize2);
	DDX_Check(pDX, IDC_PPTEMPLATE, m_Enhance);
	DDX_Check(pDX, IDC_BWTEMPLATE, m_BWTemplate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp5Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp5Dlg)
	ON_BN_CLICKED(IDC_MOTIONCONTROL, OnMotioncontrol)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SETA1, OnSeta1)
	ON_BN_CLICKED(IDC_SETA2, OnSeta2)
	ON_BN_CLICKED(IDC_SETA3, OnSeta3)
	ON_BN_CLICKED(IDC_SETB1, OnSetb1)
	ON_BN_CLICKED(IDC_SETB2, OnSetb2)
	ON_BN_CLICKED(IDC_SETB3, OnSetb3)
	ON_BN_CLICKED(IDC_CLEAR1, OnClear1)
	ON_BN_CLICKED(IDC_CLEAR2, OnClear2)
	ON_BN_CLICKED(IDC_CLEAR3, OnClear3)
	ON_BN_CLICKED(IDC_CAMERASETTING3, OnCamerasetting)
	ON_EN_KILLFOCUS(IDC_TEMPLATESIZE, OnKillfocusTemplatesize)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_BN_CLICKED(IDC_FIRSTTOP, OnFirsttop)
	ON_BN_CLICKED(IDC_FIRSTBOTTOM, OnFirstbottom)
	ON_BN_CLICKED(IDC_FIRSTLEFT, OnFirstleft)
	ON_BN_CLICKED(IDC_FIRSTRIGHT, OnFirstright)
	ON_BN_CLICKED(IDC_SECONDTOP, OnSecondtop)
	ON_BN_CLICKED(IDC_SECONDBOTTOM, OnSecondbottom)
	ON_BN_CLICKED(IDC_SECONDLEFT, OnSecondleft)
	ON_BN_CLICKED(IDC_SECONDRIGHT, OnSecondright)
	ON_LBN_KILLFOCUS(IDC_SPEED, OnKillfocusSpeed)
	ON_BN_CLICKED(IDC_MEASURETF, OnMeasuretf)
	ON_BN_CLICKED(IDC_AUTOFOCUS1, OnAutofocus1)
	ON_BN_CLICKED(IDC_AUTOEXPOSE, OnAutoexpose)
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_CROSSHAIR, OnCrosshair)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTON3, OnMicroscopelighton)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTOFF1, OnMicroscopelightoff)
	ON_BN_CLICKED(IDC_THIRDTOP, OnThirdtop)
	ON_BN_CLICKED(IDC_THIRDBOTTOM, OnThirdbottom)
	ON_BN_CLICKED(IDC_THIRDLEFT, OnThirdleft)
	ON_BN_CLICKED(IDC_THIRDRIGHT, OnThirdright)
	ON_BN_CLICKED(IDC_PATSET1, OnPatset1)
	ON_BN_CLICKED(IDC_PATSET2, OnPatset2)
	ON_BN_CLICKED(IDC_PATSET3, OnPatset3)
	ON_BN_CLICKED(IDC_PATSET4, OnPatset4)
	ON_BN_CLICKED(IDC_PATSET5, OnPatset5)
	ON_BN_CLICKED(IDC_PATSET6, OnPatset6)
	ON_BN_CLICKED(IDC_PATSET7, OnPatset7)
	ON_BN_CLICKED(IDC_PATSET8, OnPatset8)
	ON_BN_CLICKED(IDC_PATSET9, OnPatset9)
	ON_EN_KILLFOCUS(IDC_EXPOSURE4, OnKillfocusExposure4)
	ON_EN_KILLFOCUS(IDC_TEMPLATESIZE2, OnKillfocusTemplatesize2)
	ON_BN_CLICKED(IDC_REDTGT, OnRedtgt)
	ON_BN_CLICKED(IDC_PPTEMPLATE, OnPptemplate)
	ON_BN_CLICKED(IDC_BWTEMPLATE, OnBwtemplate)
	ON_BN_CLICKED(IDC_REDTGT2, OnRedtgt2)
	ON_BN_CLICKED(IDC_PATMETHOD1, OnPatmethod1)
	ON_BN_CLICKED(IDC_PATMETHOD2, OnPatmethod2)
	ON_BN_CLICKED(IDC_PATMETHOD4, OnPatmethod4)
	ON_BN_CLICKED(IDC_SETA4, OnSeta4)
	ON_BN_CLICKED(IDC_SETB4, OnSetb4)
	ON_BN_CLICKED(IDC_CLEAR4, OnClear4)
	ON_BN_CLICKED(IDC_REDTGT3, OnRedtgt3)
	ON_BN_CLICKED(IDC_PATSSETA, OnPatsseta)
	ON_BN_CLICKED(IDC_PATSSETB, OnPatssetb)
	ON_BN_CLICKED(IDC_PATSSETC, OnPatssetc)
	ON_BN_CLICKED(IDC_PATSSETD, OnPatssetd)
	ON_BN_CLICKED(IDC_PATMETHOD5, OnPatmethod5)
	ON_BN_CLICKED(IDC_CLEAR11, OnClear11)
	ON_BN_CLICKED(IDC_CLEAR22, OnClear22)
	ON_BN_CLICKED(IDC_CLEAR32, OnClear32)
	ON_BN_CLICKED(IDC_CLEAR42, OnClear42)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(UM_BLUEXHAIR, OnBlueXHair)
	ON_MESSAGE(UM_GREENXHAIR, OnGreenXHair)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp5Dlg message handlers

long CSetupProp5Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);

			m_pVideo = &m_cVideo;
			m_cVideo.pWnd = this;
			m_cVideo.bAllowMoveXHair = TRUE;
			m_cVideo.bDisableScaling = FALSE;
			m_cVideo.LoadStockDIB("BSI\\CENTER1.BMP");
		}
		RecipeToLocal();
		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp5Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}
		bTabSelected = FALSE;
	}
	return 0;
}

void CSetupProp5Dlg::LocalToRecipe() {
	UpdateData(TRUE);
	if (g->ActiveCamera == CCamBase::THINFCAM) {
		pRcp->AutoExposeTF = m_AutoExpose;
	}
	else {
		pRcp->AutoExposeEcho = m_AutoExpose;
	}
}

void CSetupProp5Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp5Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp5Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_MOTIONCONTROL, TOP_RIGHT);

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);

	//	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING3, TOP_RIGHT);
	AddAnchor(IDC_MEASURETF, TOP_RIGHT);

	AddAnchor(IDC_CROSSHAIR, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTON3, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTOFF1, TOP_RIGHT);

	AddAnchor(IDC_AUTOFOCUS1, TOP_RIGHT);
	AddAnchor(IDC_MATCH, TOP_RIGHT);

	m_cSpeed.AddString("1");
	m_cSpeed.AddString("2");
	m_cSpeed.AddString("3");
	m_cSpeed.AddString("4");
	m_cSpeed.AddString("5");
	m_cSpeed.AddString("6");

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (g->Confg.bThinFilmMicroscopeMod) {
		if (g->Confg.bThinFilmMotorMod) {
			m_cAutoFocus.ShowWindow(SW_SHOW);
		}
		m_cAutoExposure.ShowWindow(SW_SHOW);
		m_ciAutoExpose.ShowWindow(SW_SHOW);
	}

	m_cMarkA1.bShowOverlay = FALSE;
	m_cMarkA2.bShowOverlay = FALSE;
	m_cMarkB1.bShowOverlay = FALSE;
	m_cMarkB2.bShowOverlay = FALSE;
	m_cMarkC1.bShowOverlay = FALSE;
	m_cMarkC2.bShowOverlay = FALSE;
	m_cMarkD1.bShowOverlay = FALSE;
	m_cMarkD2.bShowOverlay = FALSE;
	m_cMarkA1.bScaled = FALSE;
	m_cMarkA2.bScaled = FALSE;
	m_cMarkB1.bScaled = FALSE;
	m_cMarkB2.bScaled = FALSE;
	m_cMarkC1.bScaled = FALSE;
	m_cMarkC2.bScaled = FALSE;
	m_cMarkD1.bScaled = FALSE;
	m_cMarkD2.bScaled = FALSE;

	m_cVideo.SetRecipe(pRcp);

	m_cImgWnd.bScaled = FALSE;
	m_cImgWnd.bShowScale = FALSE;

	int idx = 0;

	// Insert a column. This override is the most convenient.
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

	RecipeToLocal();

	UpdateData(FALSE);

	// [02052024 ZHIMING
	if (p413App->Global.Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp5Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);

	PositionEcho();

	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;

	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

void CSetupProp5Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);

	PositionThinFilm();

	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

BOOL CSetupProp5Dlg::DestroyWindow() {
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp5Dlg::OnClose() {
	p413App->StopCamera(&m_cVideo, NULL);
	CResizableDialog::OnClose();
}

// void CSetupProp5Dlg::OnSaverecipe()
// {
// 	LocalToRecipe();
// 	if (p413App->ValidCheckRcp(g->RcpSetup))
// 	{
// 		pRcp->SaveRecipe();
// 	}
// }

// n starts from 0 [11/4/2012 Yuen]
void CSetupProp5Dlg::SetA1(int n) {
	if (m_cVideo.SnapOneFrame()) {
		pRcp->fTgtGRnX[n][0] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[n][0] = m_cVideo.fTGtGrnY;
		pRcp->fTgtBLuX[n][0] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[n][0] = m_cVideo.fTGtBluY;
		p413App->StoreDib1(p413App->Global.StoredDIB, pRcp->MatchDIBA1[n], g->MWdw1, g->MWdw1, m_cVideo.fTGtGrnX, m_cVideo.fTGtGrnY); // @@@ [11/3/2012 Yuen]
		if (m_BWTemplate) pRcp->MatchDIBA1[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBA1[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::SetA2(int n) {
	if (m_cVideo.SnapOneFrame()) {
		float tgtX, tgtY;
		if (pRcp->PatMatOpt[n] & CRecipe413::MATCHATOFFSET) {
			switch (g->ActiveCamera) {
			case CCamBase::ECHOCAM:
				tgtX = g->fTgtRedECHO.X;
				tgtY = g->fTgtRedECHO.Y;
				break;
			// [05082024 ZHIMING
			case CCamBase::CWL:
				tgtX = g->fTgtRedCWL.X;
				tgtY = g->fTgtRedCWL.Y;
				break;
			case CCamBase::ROUGH:
				tgtX = g->fTgtRedRough.X;
				tgtY = g->fTgtRedRough.Y;
				break;
			// ]
			default:
				tgtX = g->fTgtRedTF.X;
				tgtY = g->fTgtRedTF.Y;
				break;
			}
		}
		else {
			tgtX = pRcp->fTgtGRnX[n][1] = m_cVideo.fTGtGrnX;
			tgtY = pRcp->fTgtGRnY[n][1] = m_cVideo.fTGtGrnY;
			pRcp->fTgtBLuX[n][1] = m_cVideo.fTGtBluX;
			pRcp->fTgtBLuY[n][1] = m_cVideo.fTGtBluY;
		}
		p413App->StoreDib1(p413App->Global.StoredDIB, pRcp->MatchDIBA2[n], g->MWdw1, g->MWdw1, tgtX, tgtY);
		if (m_BWTemplate) pRcp->MatchDIBA2[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBA2[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::SetA3(int n) {
	if (m_cVideo.SnapOneFrame()) {
		float tgtX, tgtY;
		if (pRcp->PatMatOpt[n] & CRecipe413::MATCHATOFFSET) {
			switch (g->ActiveCamera) {
			case CCamBase::ECHOCAM:
				tgtX = g->fTgtRedECHO.X;
				tgtY = g->fTgtRedECHO.Y;
				break;
			// [05082024 ZHIMING
			case CCamBase::CWL:
				tgtX = g->fTgtRedCWL.X;
				tgtY = g->fTgtRedCWL.Y;
				break;
			case CCamBase::ROUGH:
				tgtX = g->fTgtRedRough.X;
				tgtY = g->fTgtRedRough.Y;
				break;
			// ]
			default:
				tgtX = g->fTgtRedTF.X;
				tgtY = g->fTgtRedTF.Y;
				break;
			}
		}
		else {
			tgtX = pRcp->fTgtGRnX[n][2] = m_cVideo.fTGtGrnX;
			tgtY = pRcp->fTgtGRnY[n][2] = m_cVideo.fTGtGrnY;
			pRcp->fTgtBLuX[n][2] = m_cVideo.fTGtBluX;
			pRcp->fTgtBLuY[n][2] = m_cVideo.fTGtBluY;
		}
		p413App->StoreDib1(p413App->Global.StoredDIB, pRcp->MatchDIBA3[n], g->MWdw1, g->MWdw1, tgtX, tgtY);
		if (m_BWTemplate) pRcp->MatchDIBA3[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBA3[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::OnSeta1() {
	nPatSSet = 0;
	SetA1(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnSeta2() {
	nPatSSet = 1;
	SetA2(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnSeta3() {
	nPatSSet = 2;
	SetA3(g->CurPaSet - 1);
}

// n starts from 0 [11/4/2012 Yuen]
void CSetupProp5Dlg::SetB1(int n) {
	if (m_cVideo.SnapOneFrame()) {
		pRcp->fTgtGRnX[n][0] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[n][0] = m_cVideo.fTGtGrnY;
		pRcp->fTgtBLuX[n][0] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[n][0] = m_cVideo.fTGtBluY;
		p413App->StoreDIB2(p413App->Global.StoredDIB, pRcp->MatchDIBB1[n], m_cVideo.fTGtBluX, m_cVideo.fTGtBluY, g->MWdw2, g->MWdw2);
		if (m_BWTemplate) pRcp->MatchDIBB1[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBB1[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::SetB2(int n) {
	if (m_cVideo.SnapOneFrame()) {
		pRcp->fTgtGRnX[n][1] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[n][1] = m_cVideo.fTGtGrnY;
		pRcp->fTgtBLuX[n][1] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[n][1] = m_cVideo.fTGtBluY;
		p413App->StoreDIB2(p413App->Global.StoredDIB, pRcp->MatchDIBB2[n], m_cVideo.fTGtBluX, m_cVideo.fTGtBluY, g->MWdw2, g->MWdw2);
		if (m_BWTemplate) pRcp->MatchDIBB2[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBB2[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::SetB3(int n) {
	if (m_cVideo.SnapOneFrame()) {
		pRcp->fTgtGRnX[n][2] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[n][2] = m_cVideo.fTGtGrnY;
		pRcp->fTgtBLuX[n][2] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[n][2] = m_cVideo.fTGtBluY;
		p413App->StoreDIB2(p413App->Global.StoredDIB, pRcp->MatchDIBB3[n], m_cVideo.fTGtBluX, m_cVideo.fTGtBluY, g->MWdw2, g->MWdw2);
		if (m_BWTemplate) pRcp->MatchDIBB3[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBB3[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::OnSetb1() {
	nPatSSet = 0;
	SetB1(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnSetb2() {
	nPatSSet = 1;
	SetB2(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnSetb3() {
	nPatSSet = 2;
	SetB3(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnClear1() {
	m_cMarkA1.ClearDIB();
	//m_cMarkA2.ClearDIB();
	m_cMarkA1.Refresh();
	//m_cMarkA2.Refresh();
	pRcp->MatchDIBA1[g->CurPaSet - 1].ClearDIB();
	//pRcp->MatchDIBB1[g->CurPaSet-1].ClearDIB();
}

void CSetupProp5Dlg::OnClear2() {
	m_cMarkB1.ClearDIB();
	//m_cMarkB2.ClearDIB();
	m_cMarkB1.Refresh();
	//m_cMarkB2.Refresh();
	pRcp->MatchDIBA2[g->CurPaSet - 1].ClearDIB();
	//pRcp->MatchDIBB2[g->CurPaSet-1].ClearDIB();
}

void CSetupProp5Dlg::OnClear3() {
	m_cMarkC1.ClearDIB();
	//m_cMarkC2.ClearDIB();
	m_cMarkC1.Refresh();
	//m_cMarkC2.Refresh();
	pRcp->MatchDIBA3[g->CurPaSet - 1].ClearDIB();
	//pRcp->MatchDIBB3[g->CurPaSet-1].ClearDIB();
}

void CSetupProp5Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp5Dlg::RecipeToLocal() {
	CString str;

	str.Format("Pattern Set %dA", g->CurPaSet);
	m_cPaSet1.SetWindowText(str);

	str.Format("Pattern Set %dB", g->CurPaSet);
	m_cPaSet2.SetWindowText(str);

	str.Format("Pattern Set %dC", g->CurPaSet);
	m_cPaSet3.SetWindowText(str);

	str.Format("Pattern Set %dD", g->CurPaSet);
	m_cPaSet4.SetWindowText(str);

	m_cMarkA1.DupStore1(pRcp->MatchDIBA1[g->CurPaSet - 1]);
	m_cMarkA2.DupStore1(pRcp->MatchDIBB1[g->CurPaSet - 1]);

	m_cMarkB1.DupStore1(pRcp->MatchDIBA2[g->CurPaSet - 1]);
	m_cMarkB2.DupStore1(pRcp->MatchDIBB2[g->CurPaSet - 1]);

	m_cMarkC1.DupStore1(pRcp->MatchDIBA3[g->CurPaSet - 1]);
	m_cMarkC2.DupStore1(pRcp->MatchDIBB3[g->CurPaSet - 1]);

	m_cMarkD1.DupStore1(pRcp->MatchDIBA4[g->CurPaSet - 1]);
	m_cMarkD2.DupStore1(pRcp->MatchDIBB4[g->CurPaSet - 1]);

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

	((CButton*)GetDlgItem(IDC_PATSSETA))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETB))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETD))->SetCheck(FALSE);
	switch (nPatSSet) {
	case 0:
		((CButton*)GetDlgItem(IDC_PATSSETA))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_PATSSETB))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_PATSSETC))->SetCheck(TRUE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATSSETD))->SetCheck(TRUE);
		break;
	}

	((CButton*)GetDlgItem(IDC_PATMETHOD1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD2))->SetCheck(FALSE);
	// 	((CButton *)GetDlgItem(IDC_PATMETHOD3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD5))->SetCheck(FALSE);
	switch (pRcp->MatchAlgorithm[g->CurPaSet - 1]) {
	case 0:
		((CButton*)GetDlgItem(IDC_PATMETHOD1))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_PATMETHOD2))->SetCheck(TRUE);
		break;
		// 	case 2:
		// 		((CButton *)GetDlgItem(IDC_PATMETHOD3))->SetCheck(TRUE);
		// 		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATMETHOD4))->SetCheck(TRUE);
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_PATMETHOD5))->SetCheck(TRUE);
		break;
	}

	m_TemplateSize = g->MWdw1;
	m_TemplateSize2 = g->MWdw2;

	if (g->ActiveCamera == CCamBase::THINFCAM) {
		m_AutoExpose = pRcp->AutoExposeTF;
	}
	else {
		m_AutoExpose = pRcp->AutoExposeEcho;
	}

	m_cRed.SetCheck(FALSE);
	m_cRed2.SetCheck(FALSE);
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		m_cRed.SetCheck(TRUE);
		m_cRed2.SetCheck(TRUE);
	}

	m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet];

	UpdateMessage();

	UpdateData(FALSE);
}

void CSetupProp5Dlg::PositionEcho() {
	p413App->GotoPositionecho();
}

void CSetupProp5Dlg::PositionThinFilm() {
	p413App->GotoPositionthinfilm();
}

long CSetupProp5Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

long CSetupProp5Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp5Dlg::OnLoadrecipe() {
	LocalToRecipe();
	RecipeIO();
	// 	if (LoadRcpSetup())
	// 	{
	// 		RecipeToLocal();
	// 	}
}

void CSetupProp5Dlg::OnFirsttop() {
	switch (g->ActiveCamera) {
	case CCamBase::THINFCAM:
		g->fTgtRedTF.Y -= dx;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->fTgtRedCWL.Y -= dx;
		break;
	case CCamBase::ROUGH:
		g->fTgtRedRough.Y -= dx;
		break;
	// ]
	default:
		g->fTgtRedECHO.Y -= dx;
		break;
	}
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnFirstbottom() {
	switch (g->ActiveCamera) {
	case CCamBase::THINFCAM:
		g->fTgtRedTF.Y += dx;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->fTgtRedCWL.Y += dx;
		break;
	case CCamBase::ROUGH:
		g->fTgtRedRough.Y += dx;
		break;
	// ]
	default:
		g->fTgtRedTF.Y += dx;
		break;
	}
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnFirstleft() {
	switch (g->ActiveCamera) {
	case CCamBase::THINFCAM:
		g->fTgtRedTF.X -= dx;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->fTgtRedCWL.X -= dx;
		break;
	case CCamBase::ROUGH:
		g->fTgtRedRough.X -= dx;
		break;
	// ]
	default:
		g->fTgtRedECHO.X -= dx;
		break;
	}
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnFirstright() {
	switch (g->ActiveCamera) {
	case CCamBase::THINFCAM:
		g->fTgtRedTF.X += dx;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->fTgtRedCWL.X += dx;
		break;
	case CCamBase::ROUGH:
		g->fTgtRedRough.X += dx;
		break;
	// ]
	default:
		g->fTgtRedECHO.X += dx;
		break;
	}
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnSecondtop() {
	m_cVideo.fTGtGrnY -= dx;

	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnSecondbottom() {
	m_cVideo.fTGtGrnY += dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnSecondleft() {
	m_cVideo.fTGtGrnX -= dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnSecondright() {
	m_cVideo.fTGtGrnX += dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnKillfocusSpeed() {
	CString str;
	int i = m_cSpeed.GetCurSel();
	if (i > -1) {
		i++;
		i *= i;
		dx = i * 0.001f;
	}
}
void CSetupProp5Dlg::OnMeasuretf() {
	BlockUI();
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	COceanBase* pOcean = p413App->pOceanOp;
	pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
	m_cMeasure.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->SetSpectroSourceLight(FALSE);
	if (!pOcean->bSimu) {
		Sleep(1000);
	}
	m_cVideo.SetExposure(pRcp->TFTakeSpecExposure + g->BaseExpoTF);
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\SEM06\\%03d-0_1.CSV";
	int temp = pOcean->StartTF(p, dir, 49 * rand() / float(RAND_MAX) + 1);
	if ((temp == 512) || (temp == 2048)) {
		BOOL bTaken = FALSE;
		pOcean->ClearPolyThk(&pOcean->CurrSpectrum);
		pOcean->SetActivatePoly(&pOcean->CurrSpectrum, 0, p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.RI[0]);
		pOcean->Result(&pOcean->CurrSpectrum, &pOcean->ResultSpectrum, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
	}
	m_cMeasure.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp5Dlg::OnAutofocus1() {
	BlockUI();
	m_cAutoFocus.EnableWindow(FALSE);
	p413App->AutoFocusVideo(CCamBase::THINFCAM, m_cVideo.fTGtGrnX, m_cVideo.fTGtGrnY, &m_cVideo);
	m_cAutoFocus.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp5Dlg::OnAutoexpose() {
	m_cAutoExposure.EnableWindow(FALSE);
	short Exposure;
	if (g->ActiveCamera == CCamBase::THINFCAM) {
		Exposure = pRcp->AutoExposeTF + g->BaseExpoEcho;
	}
	else {
		Exposure = pRcp->AutoExposeEcho + g->BaseExpoTF;
	}
	m_cVideo.Level = Exposure;
	m_cVideo.AutoAdjust();
	m_cAutoExposure.EnableWindow(TRUE);
}

void CSetupProp5Dlg::OnMatch() {
	BlockUI();
	m_cMatch.EnableWindow(FALSE);

	m_cList.DeleteAllItems();

	SDoPattrnMatching Param;
	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	Param.bMoveCenter = TRUE;

	p413App->bLoadDIB = FALSE;
	DoPatternMatching(Param, FALSE);
	p413App->bLoadDIB = TRUE;

	m_cMatch.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp5Dlg::OnCrosshair() {
	CCrossHair dlg;
	dlg.DoModal();
	UpdateMessage();
}

void CSetupProp5Dlg::UpdateMessage() {
	CString str;
	float TgtX = g->fTgtRedECHO.X;
	float TgtY = g->fTgtRedECHO.Y;

	if (g->ActiveCamera == CCamBase::THINFCAM) {
		TgtX = g->fTgtRedTF.X;
		TgtY = g->fTgtRedTF.Y;
	}

	str.Format("R(%.3f,%.3f) G(%.3f,%.3f) B(%.3f,%.3f)", m_cVideo.fTGtGrnX, m_cVideo.fTGtGrnY, TgtX, TgtY, m_cVideo.fTGtBluX, m_cVideo.fTGtBluY);
	m_cMessage.SetWindowText(str);
}

void CSetupProp5Dlg::OnMicroscopelighton() {
	if (p413App->pMtr) {
		switch (g->ActiveCamera) {
		default:
			p413App->pMtr->SetMicroscopeLight(TRUE);
			break;
		case CCamBase::THINFCAM:
			p413App->pMtr->SetSpectroIllumLight(TRUE);
			break;
		}
	}
}

void CSetupProp5Dlg::OnMicroscopelightoff() {
	if (p413App->pMtr) {
		switch (g->ActiveCamera) {
		default:
			p413App->pMtr->SetMicroscopeLight(FALSE);
			break;
		case CCamBase::THINFCAM:
			p413App->pMtr->SetSpectroIllumLight(FALSE);
			break;
		}
	}
}

void CSetupProp5Dlg::OnThirdtop() {
	m_cVideo.fTGtBluY -= dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnThirdbottom() {
	m_cVideo.fTGtBluY += dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnThirdleft() {
	m_cVideo.fTGtBluX -= dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnThirdright() {
	m_cVideo.fTGtBluX += dx;
	UpdateMessage();
	UpdateData(FALSE);
}

void CSetupProp5Dlg::OnPatset1() {
	g->CurPaSet = 1;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset2() {
	g->CurPaSet = 2;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset3() {
	g->CurPaSet = 3;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset4() {
	g->CurPaSet = 4;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset5() {
	g->CurPaSet = 5;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset6() {
	g->CurPaSet = 6;
	RecipeToLocal
	();
}

void CSetupProp5Dlg::OnPatset7() {
	g->CurPaSet = 7;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset8() {
	g->CurPaSet = 8;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatset9() {
	g->CurPaSet = 9;
	RecipeToLocal();
}

long CSetupProp5Dlg::OnBlueXHair(WPARAM wP, LPARAM lP) {
	switch (nPatSSet) {
	case 0:
		pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluY;
		SetB1(g->CurPaSet - 1);
		break;
	case 1:
		pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluY;
		SetB2(g->CurPaSet - 1);
		break;
	case 2:
		pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluY;
		SetB3(g->CurPaSet - 1);
		break;
	case 3:
		pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtBluY;
		SetB4(g->CurPaSet - 1);
		break;
	}
	return 0;
}

long CSetupProp5Dlg::OnGreenXHair(WPARAM wP, LPARAM lP) {
	switch (nPatSSet) {
	case 0:
		pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnY;
		SetA1(g->CurPaSet - 1);
		break;
	case 1:
		pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnY;
		SetA2(g->CurPaSet - 1);
		break;
	case 2:
		pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnY;
		SetA3(g->CurPaSet - 1);
		break;
	case 3:
		pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet] = m_cVideo.fTGtGrnY;
		SetA4(g->CurPaSet - 1);
		break;
	}

	return 0;
}

void CSetupProp5Dlg::OnKillfocusExposure4() {
	UpdateData(TRUE);
	if (g->ActiveCamera == CCamBase::THINFCAM) {
		pRcp->AutoExposeTF = m_AutoExpose;
	}
	else {
		pRcp->AutoExposeEcho = m_AutoExpose;
	}
}

void CSetupProp5Dlg::OnKillfocusTemplatesize() {
	UpdateData(TRUE);
	g->MWdw1 = m_TemplateSize;
}

void CSetupProp5Dlg::OnKillfocusTemplatesize2() {
	UpdateData(TRUE);
	g->MWdw2 = m_TemplateSize2;
}

void CSetupProp5Dlg::OnPptemplate() {
	UpdateData(TRUE);
}

void CSetupProp5Dlg::OnBwtemplate() {
	UpdateData(TRUE);
}

void CSetupProp5Dlg::OnRedtgt() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::MATCHATOFFSET;
		ClearRed();
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::MATCHATOFFSET;
		ClearRed();
		SetA2(g->CurPaSet - 1);
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::OnRedtgt2() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::MATCHATOFFSET;
		ClearRed();
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::MATCHATOFFSET;
		ClearRed();
		SetA3(g->CurPaSet - 1);
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatmethod1() {
	// Use primary template [4/30/2014 Administrator]
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 0;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatmethod2() {
	// Use second template [4/30/2014 Administrator]
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 1;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatmethod4() {
	// Use alternate template [4/30/2014 Administrator]
	// Will perform additional matching when target is found, purpose is fine positioning of ROI [4/30/2014 Administrator]
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 3;
	RecipeToLocal();
}

void CSetupProp5Dlg::ClearRed() {
	m_cMarkB1.ClearDIB();
	m_cMarkB1.Refresh();
	m_cMarkB2.ClearDIB();
	m_cMarkB2.Refresh();
	pRcp->MatchDIBA2[g->CurPaSet - 1].ClearDIB();
	pRcp->MatchDIBB2[g->CurPaSet - 1].ClearDIB();

	m_cMarkC1.ClearDIB();
	m_cMarkC1.Refresh();
	m_cMarkC2.ClearDIB();
	m_cMarkC2.Refresh();
	pRcp->MatchDIBA3[g->CurPaSet - 1].ClearDIB();
	pRcp->MatchDIBB3[g->CurPaSet - 1].ClearDIB();
}

void CSetupProp5Dlg::OnSeta4() {
	nPatSSet = 3;
	SetA4(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnSetb4() {
	nPatSSet = 3;
	SetB4(g->CurPaSet - 1);
}

void CSetupProp5Dlg::OnClear4() {
	m_cMarkD1.ClearDIB();
	//m_cMarkD2.ClearDIB();
	m_cMarkD1.Refresh();
	//m_cMarkD2.Refresh();
	pRcp->MatchDIBA4[g->CurPaSet - 1].ClearDIB();
	//pRcp->MatchDIBB4[g->CurPaSet-1].ClearDIB();
}

void CSetupProp5Dlg::OnRedtgt3() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::MATCHATOFFSET;
		ClearRed();
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::MATCHATOFFSET;
		ClearRed();
		SetA4(g->CurPaSet - 1);
	}
	RecipeToLocal();
}

// n starts from 0 [11/4/2012 Yuen]
void CSetupProp5Dlg::SetA4(int n) {
	if (m_cVideo.SnapOneFrame()) {
		float tgtX, tgtY;
		if (pRcp->PatMatOpt[n] & CRecipe413::MATCHATOFFSET) {
			switch (g->ActiveCamera) {
			case CCamBase::ECHOCAM:
				tgtX = g->fTgtRedECHO.X;
				tgtY = g->fTgtRedECHO.Y;
				break;
			// [05082024 ZHIMING
			case CCamBase::CWL:
				tgtX = g->fTgtRedCWL.X;
				tgtY = g->fTgtRedCWL.Y;
				break;
			case CCamBase::ROUGH:
				tgtX = g->fTgtRedRough.X;
				tgtY = g->fTgtRedRough.Y;
				break;
			// ]
			default:
				tgtX = g->fTgtRedTF.X;
				tgtY = g->fTgtRedTF.Y;
				break;
			}
		}
		else {
			tgtX = pRcp->fTgtGRnX[n][3] = m_cVideo.fTGtGrnX;
			tgtY = pRcp->fTgtGRnY[n][3] = m_cVideo.fTGtGrnY;
			pRcp->fTgtBLuX[n][3] = m_cVideo.fTGtBluX;
			pRcp->fTgtBLuY[n][3] = m_cVideo.fTGtBluY;
		}
		p413App->StoreDib1(p413App->Global.StoredDIB, pRcp->MatchDIBA4[n], g->MWdw1, g->MWdw1, tgtX, tgtY); // @@@ [11/3/2012 Yuen]
		if (m_BWTemplate) pRcp->MatchDIBA4[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBA4[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

// n starts from 0 [11/4/2012 Yuen]
void CSetupProp5Dlg::SetB4(int n) {
	if (m_cVideo.SnapOneFrame()) {
		pRcp->fTgtGRnX[n][3] = m_cVideo.fTGtGrnX;
		pRcp->fTgtGRnY[n][3] = m_cVideo.fTGtGrnY;
		pRcp->fTgtBLuX[n][3] = m_cVideo.fTGtBluX;
		pRcp->fTgtBLuY[n][3] = m_cVideo.fTGtBluY;
		p413App->StoreDIB2(p413App->Global.StoredDIB, pRcp->MatchDIBB4[n], m_cVideo.fTGtBluX, m_cVideo.fTGtBluY, g->MWdw2, g->MWdw2);
		if (m_BWTemplate) pRcp->MatchDIBB4[n].GreyScale();
		if (m_Enhance) {
			pRcp->MatchDIBB4[n].ContrastStretchColor(0, 0);
		}
	}
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatsseta() {
	nPatSSet = 0;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatssetb() {
	nPatSSet = 1;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatssetc() {
	nPatSSet = 2;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatssetd() {
	nPatSSet = 3;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnPatmethod5() {
	// Use multi template [4/30/2014 Administrator]
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 4;
	RecipeToLocal();
}

void CSetupProp5Dlg::OnClear11() {
	m_cMarkA2.ClearDIB();
	m_cMarkA2.Refresh();
	pRcp->MatchDIBB1[g->CurPaSet - 1].ClearDIB();
}

void CSetupProp5Dlg::OnClear22() {
	m_cMarkB2.ClearDIB();
	m_cMarkB2.Refresh();
	pRcp->MatchDIBB2[g->CurPaSet - 1].ClearDIB();
}

void CSetupProp5Dlg::OnClear32() {
	m_cMarkC2.ClearDIB();
	m_cMarkC2.Refresh();
	pRcp->MatchDIBB3[g->CurPaSet - 1].ClearDIB();
}

void CSetupProp5Dlg::OnClear42() {
	m_cMarkD2.ClearDIB();
	m_cMarkD2.Refresh();
	pRcp->MatchDIBB4[g->CurPaSet - 1].ClearDIB();
}