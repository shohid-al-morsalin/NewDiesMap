// SetupProp51Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "SetupProp51Dlg.h"
#include "XTabCtrl.h"
#include "413App.h"
#include "MotionControl.h"
#include "SelectMeasurementDlg.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "..\SYS\CUMMsg.h"

#include "CrossHair.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp51Dlg dialog

CSetupProp51Dlg::CSetupProp51Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp51Dlg::IDD, pParent), CSetupPropComm(pParent) {
	//{{AFX_DATA_INIT(CSetupProp51Dlg)
	m_STIOffset = 0.0f;
	m_STIOffsetY = 0.0f;
	m_ODOffset = 0.0f;
	m_ODOffsetY = 0.0f;
	m_CX = 0.0f;
	m_CY = 0.0f;
	m_CR = 0.0f;
	Bevel = 0.0f;
	//}}AFX_DATA_INIT

	nSel = 0;
	m_cVideo.Name = "Setup51";
}

void CSetupProp51Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp51Dlg)
	DDX_Control(pDX, IDC_FASTMATCHING, m_cFastMatching);
	DDX_Control(pDX, IDC_MATCHATOFFSET, m_cMatchAtOffset);
	DDX_Control(pDX, IDC_MATCH7, m_cMatch7);
	DDX_Control(pDX, IDC_MATCH6, m_cMatch6);
	DDX_Control(pDX, IDC_PASET, m_cPaSet);
	DDX_Control(pDX, IDC_SAVEVIDEO2, m_cSaveVideo);
	DDX_Control(pDX, IDC_MATCH5, m_cMatch5);
	DDX_Control(pDX, IDC_MATCH4, m_cMatch4);
	DDX_Control(pDX, IDC_FINDCXCY, m_cFindCxCy);
	DDX_Control(pDX, IDC_OPTIMIZE, m_cOptimize);
	DDX_Control(pDX, IDC_GOTOALIGNMENTPOS, m_cGotoAlgnPos);
	DDX_Control(pDX, IDC_ALIGNMENTPOS, m_cAlignPos);
	DDX_Control(pDX, IDC_MATCH2, m_cMatch2);
	DDX_Control(pDX, IDC_MESSAGE, m_cList);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_INITIALEXPOSUREAFTERFOCUS2, m_cInitialExposureAfterFocus2);
	DDX_Control(pDX, IDC_INITIALFOCUS2, m_cInitialFocus2);
	DDX_Control(pDX, IDC_INITIALEXPOSUREAFTERFOCUS, m_InitialExposureAfterFocus);
	DDX_Control(pDX, IDC_INITIALFOCUS, m_cInitialFocus);
	DDX_Control(pDX, IDC_INITIALEXPOSUREBEFOREFOCUS, m_cInitialExposeB4Focus);
	DDX_Control(pDX, IDC_SELECTEDPT, m_cSelectedPt);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_MATCH, m_cMatch);
	DDX_Control(pDX, IDC_POSITIONTHINFILM1, m_cPositionThinFilm);
	DDX_Control(pDX, IDC_POSITIONECHO, m_cPositionEcho);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraB);
	DDX_Control(pDX, IDC_IMAGE, m_cImgWnd);
	DDX_Control(pDX, IDC_PATTERNB1, m_cMark2);
	DDX_Control(pDX, IDC_PATTERNA1, m_cMark1);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Text(pDX, IDC_STIOFFSET, m_STIOffset);
	DDX_Text(pDX, IDC_STIOFFSETY, m_STIOffsetY);
	DDX_Text(pDX, IDC_ODOFFSET, m_ODOffset);
	DDX_Text(pDX, IDC_ODOFFSETY, m_ODOffsetY);
	DDX_Text(pDX, IDC_CX, m_CX);
	DDX_Text(pDX, IDC_CY, m_CY);
	DDX_Text(pDX, IDC_CR, m_CR);
	DDX_Text(pDX, IDC_BEVEL, Bevel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp51Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp51Dlg)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL, OnMotioncontrol)
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_POSITIONECHO, OnPositionecho)
	ON_BN_CLICKED(IDC_POSITIONTHINFILM1, OnPositionthinfilm)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_MATCH2, OnMatch2)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREBEFOREFOCUS, OnInitialexposurebeforefocus)
	ON_BN_CLICKED(IDC_INITIALFOCUS, OnInitialfocus)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREAFTERFOCUS, OnInitialexposureafterfocus)
	ON_BN_CLICKED(IDC_INITIALFOCUS2, OnInitialfocus2)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREAFTERFOCUS2, OnInitialexposureafterfocus2)
	ON_COMMAND(IDC_MARK, OnMark)
	ON_COMMAND(IDC_MATCHPATTERN, OnMatchpattern)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_BN_CLICKED(IDC_ALIGNMENTPOS, OnAlignmentpos)
	ON_BN_CLICKED(IDC_GOTOALIGNMENTPOS, OnGotoalignmentpos)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTON3, OnMicroscopelighton)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTOFF1, OnMicroscopelightoff)
	ON_BN_CLICKED(IDC_PATSET1, OnPatset1)
	ON_BN_CLICKED(IDC_PATSET2, OnPatset2)
	ON_BN_CLICKED(IDC_PATSET3, OnPatset3)
	ON_BN_CLICKED(IDC_PATSET4, OnPatset4)
	ON_BN_CLICKED(IDC_PATSET5, OnPatset5)
	ON_BN_CLICKED(IDC_PATSET6, OnPatset6)
	ON_BN_CLICKED(IDC_PATSET7, OnPatset7)
	ON_BN_CLICKED(IDC_PATSET8, OnPatset8)
	ON_BN_CLICKED(IDC_PATSET9, OnPatset9)
	ON_BN_CLICKED(IDC_IMG1, OnImg1)
	ON_BN_CLICKED(IDC_IMG2, OnImg2)
	ON_BN_CLICKED(IDC_IMG3, OnImg3)
	ON_BN_CLICKED(IDC_IMG4, OnImg4)
	ON_BN_CLICKED(IDC_IMG5, OnImg5)
	ON_BN_CLICKED(IDC_IMG6, OnImg6)
	ON_EN_KILLFOCUS(IDC_ODOFFSET, OnKillfocusOdoffset)
	ON_EN_KILLFOCUS(IDC_ODOFFSETY, OnKillfocusOdoffsety)
	ON_EN_KILLFOCUS(IDC_STIOFFSET, OnKillfocusStioffset)
	ON_EN_KILLFOCUS(IDC_STIOFFSETY, OnKillfocusStioffsety)
	ON_BN_CLICKED(IDC_CLEARLIST, OnClearlist)
	ON_BN_CLICKED(IDC_MATCH4, OnMatch4)
	ON_BN_CLICKED(IDC_MATCH5, OnTestRun)
	ON_BN_CLICKED(IDC_FINDCXCY, OnFindcxcy)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnOptimize)
	ON_BN_CLICKED(IDC_SAVEVIDEO2, OnSavevideo2)
	ON_BN_CLICKED(IDC_FOCUS, OnFocus)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_SET11, OnSet11)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DN, OnDn)
	ON_BN_CLICKED(IDC_HERE, OnHere)
	ON_BN_CLICKED(IDC_ADDZOFFSET, OnAddzoffset)
	ON_BN_CLICKED(IDC_UPDATEPT, OnUpdatept)
	ON_BN_CLICKED(IDC_MATCH6, OnMatch6)
	ON_BN_CLICKED(IDC_MATCH7, OnMatch7)
	ON_BN_CLICKED(IDC_MATCHATOFFSET, OnMatchatoffset)
	ON_BN_CLICKED(IDC_PATSSETA, OnPatsseta)
	ON_BN_CLICKED(IDC_PATSSETB, OnPatssetb)
	ON_BN_CLICKED(IDC_PATSSETC, OnPatssetc)
	ON_BN_CLICKED(IDC_PATSSETD, OnPatssetd)
	ON_BN_CLICKED(IDC_FASTMATCHING, OnFastmatching)
	//}}AFX_MSG_MAP
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)
	ON_MESSAGE(ID_RELOCATETOTOP, OnRelocateToTop)
	ON_MESSAGE(ID_RELOCATETOBACK, OnRelocateToBack)
	ON_MESSAGE(IDC_EDITPOINT, OnEditpoint)
	ON_MESSAGE(IDC_ADDPOINT, OnAddpoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp51Dlg message handlers
long CSetupProp51Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
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
			m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
			m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][0];
		}

		m_cWaferMap.pRcp = pRcp;

		RecipeToLocal();

		pRcp->UpdateControl(m_cPoint);
		m_cVideo.pWnd = this;
		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp51Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}

		LocalToRecipe();
	}

	bTabSelected = FALSE;
	return 0;
}

void CSetupProp51Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp51Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp51Dlg::OnClose() {
	StopAllCamera(&m_cVideo);
	CResizableDialog::OnClose();
}

BOOL CSetupProp51Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);

	AddAnchor(IDC_MOTIONCONTROL, TOP_RIGHT);

	AddAnchor(IDC_POSITIONECHO, TOP_RIGHT);
	AddAnchor(IDC_POSITIONTHINFILM, TOP_RIGHT);

	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_MATCH, TOP_RIGHT);
	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);

	AddAnchor(IDC_ALIGNMENTPOS, TOP_RIGHT);
	AddAnchor(IDC_GOTOALIGNMENTPOS, TOP_RIGHT);
	AddAnchor(IDC_MESSAGE, TOP_LEFT, BOTTOM_RIGHT);

	AddAnchor(IDC_OPTIMIZE, TOP_RIGHT);
	AddAnchor(IDC_MATCH5, TOP_RIGHT);

	AddAnchor(IDC_MICROSCOPELIGHTON3, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTOFF1, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cVideo.SetRecipe(pRcp);

	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	m_cImgWnd.bScaled = FALSE;

	m_cMark1.bShowOverlay = FALSE;
	m_cMark2.bShowOverlay = FALSE;
	m_cMark1.bScaled = FALSE;
	m_cMark2.bScaled = FALSE;
	m_cImgWnd.bShowOverlay = FALSE;

	SetupColumn();

	if (pGDev->SystemModel != CGlobalDeviceContainer::FSM413C2C) {
		m_cAlignPos.ShowWindow(SW_SHOW);
		m_cGotoAlgnPos.ShowWindow(SW_SHOW);
	}

	// [02052024 ZHIMING
	if (p413App->Global.Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraA.ShowWindow(SW_HIDE);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp51Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	OnPositionecho();
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp51Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	OnPositionthinfilm();
	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp51Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp51Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp51Dlg::OnPositionecho() {
	m_cPositionEcho.EnableWindow(FALSE);
	p413App->GotoPositionecho();
	m_cPositionEcho.EnableWindow(TRUE);
}

void CSetupProp51Dlg::OnPositionthinfilm() {
	m_cPositionThinFilm.EnableWindow(FALSE);
	p413App->GotoPositionthinfilm();
	m_cPositionThinFilm.EnableWindow(TRUE);
}

void CSetupProp51Dlg::OnTestRun() {
	CString str;

	p413App->rnd1 = 25;
	m_cMatch5.GetWindowText(str);
	if ((str.Find("Stop") != -1)) {
		if (!p413App->Global.bStop) {
			p413App->Global.bStop = TRUE;
			while (p413App->Global.bStop) {
				p413App->Yld(200);
			}
			m_cVideo.Reserved = "";
		}
		m_cMatch5.SetWindowText("Run\nRecipe");
		ReleaseUI();
		return;
	}
	else {
		BlockUI();
		if (AfxMessageBox("Do you want to 'Center Wafer'?", MB_YESNO) == IDYES) {
			OnFindcxcy();
		}

		m_cList.DeleteAllItems();
		//p413App->rnd1 = 39;	// 65-center, 12-left 39-right 23-up 35-down[6/28/2013 Yuen]
		m_cMatch5.SetWindowText("Stop");
		p413App->Global.bStop = FALSE;
		m_cVideo.Reserved = "TestRun";
		CWinThread* pThread = AfxBeginThread(CSetupProp51Dlg::ExecuteTF, this);
	}
}

void CSetupProp51Dlg::OnMatch2() {
	BlockUI();
	m_cList.DeleteAllItems();
	m_cMatch2.EnableWindow(FALSE);
	p413App->rnd1 = 1;	// 65-center, 12-left 39-right 23-up 35-down 90 48 88 25 13 2[6/28/2013 Yuen]

	SDoPattrnMatching Param;

	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = TRUE;
	Param.bMoveCenter = TRUE;

	DoPatternMatching(Param, FALSE);
	m_cMatch2.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp51Dlg::OnMatch4() {
	BlockUI();
	m_cList.DeleteAllItems();
	m_cMatch4.EnableWindow(FALSE);

	SDoPattrnMatching Param;

	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	Param.bMoveCenter = TRUE;

	p413App->bLoadDIB = FALSE;
	DoPatternMatching(Param, FALSE);
	p413App->bLoadDIB = TRUE;

	m_cMatch4.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp51Dlg::OnMatch() {
	BlockUI();
	m_cList.DeleteAllItems();
	m_cMatch.EnableWindow(FALSE);
	UpdateData(TRUE);

	SDoPattrnMatching Param;
	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	Param.bMoveCenter = TRUE;

	DoPatternMatching(Param, FALSE);

	m_cMatch.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp51Dlg::OnLoadrecipe() {
	UpdateData(TRUE);
	RecipeIO();
	// 	if (LoadRcpSetup())
	// 	{
	pRcp->UpdateControl(m_cPoint);
	// 		RecipeToLocal();
	// 	}
}

void CSetupProp51Dlg::RecipeToLocal() {
	m_ODOffset = pRcp->POffsetX[g->CurPaSet - 1][0];
	m_STIOffset = pRcp->POffsetX[g->CurPaSet - 1][1];
	m_ODOffsetY = pRcp->POffsetY[g->CurPaSet - 1][0];
	m_STIOffsetY = pRcp->POffsetY[g->CurPaSet - 1][1];

	UpdateData(FALSE);

	CString str;
	str.Format("Options: PaSet %d", g->CurPaSet);
	m_cPaSet.SetWindowText(str);

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPB4FOCUS) {
		m_cInitialExposeB4Focus.SetCheck(TRUE);
	}
	else {
		m_cInitialExposeB4Focus.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAFOCUS) {
		m_InitialExposureAfterFocus.SetCheck(TRUE);
	}
	else {
		m_InitialExposureAfterFocus.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCB4MATCH) {
		m_cInitialFocus.SetCheck(TRUE);
	}
	else {
		m_cInitialFocus.SetCheck(FALSE);
	}

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAMATCH) {
		m_cInitialExposureAfterFocus2.SetCheck(TRUE);
	}
	else {
		m_cInitialExposureAfterFocus2.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCAMATCH) {
		m_cInitialFocus2.SetCheck(TRUE);
	}
	else {
		m_cInitialFocus2.SetCheck(FALSE);
	}

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		m_cMatchAtOffset.SetCheck(TRUE);
	}
	else {
		m_cMatchAtOffset.SetCheck(FALSE);
	}

	m_cWaferMap.Redraw();

	CPackedDIB* pDIBA = NULL;
	CPackedDIB* pDIBB = NULL;

	switch (nPatSSet) {
	case 0:
		pDIBA = &pRcp->MatchDIBA1[g->CurPaSet - 1];
		pDIBB = &pRcp->MatchDIBB1[g->CurPaSet - 1];
		break;
	case 1:
		pDIBA = &pRcp->MatchDIBA2[g->CurPaSet - 1];
		pDIBB = &pRcp->MatchDIBB2[g->CurPaSet - 1];
		break;
	case 2:
		pDIBA = &pRcp->MatchDIBA3[g->CurPaSet - 1];
		pDIBB = &pRcp->MatchDIBB3[g->CurPaSet - 1];
		break;
	case 3:
		pDIBA = &pRcp->MatchDIBA4[g->CurPaSet - 1];
		pDIBB = &pRcp->MatchDIBB4[g->CurPaSet - 1];
		break;
	}
	m_cMark1.DupStore1(*pDIBA);
	m_cMark2.DupStore1(*pDIBB);

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

	if (pRcp->bSaveVideo) {
		m_cSaveVideo.SetCheck(TRUE);
	}
	else {
		m_cSaveVideo.SetCheck(FALSE);
	}
	if (pRcp->bFastPMatch) {
		m_cFastMatching.SetCheck(TRUE);
	}
	else {
		m_cFastMatching.SetCheck(FALSE);
	}

	m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet];
}

void CSetupProp51Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENU413);
	pPopUp = menu.GetSubMenu(2);
	if (!pPopUp) return;
	ClientToScreen(&point);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
	switch (res) {
	case ID_RELOCATETOBACK:
		if (pMP) {
			RelocateToBack(&pMP->Co);
		}
		break;
	case ID_RELOCATETOTOP:
		if (pMP) {
			RelocateToTop(&pMP->Co);
		}
		break;
	case ID_UPDATEPOINT:
		if (pMP) {
			UpdatePoint(&pMP->Co);
			m_cPoint.EnsureVisible(nSel, FALSE);
			m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		}
		break;
	case ID_MODIFYPOINT:
		break;
	case ID_ENABLEPT:
		if (pMP) {
			DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
		}
		break;
	case ID_DISABLEPT:
		if (pMP) {
			pMP->MeSet = 0;
			pMP->PatSet = 0;
			pRcp->UpdateControl(m_cPoint);
		}
		break;
	case IDC_GOTOPOINT:
		if (pMP) {
			GotoPointB(&m_cVideo, &pMP->Co);
			UpdateSelectionItem(pMP);
		}
		break;
	case IDC_GOTOXYNOZ:
		if (pMP) {
			GotoPointB(&m_cVideo, &pMP->Co, FALSE);
			UpdateSelectionItem(pMP);
		}
		break;
	case IDC_ALIGN:
		OnMatchpattern();
		break;
	case ID_GOTOALGNUPDATE:
		if (pMP) {
			GotoPointB(&m_cVideo, &pMP->Co);
			OnMatchpattern();
			UpdatePoint(&pMP->Co);
		}
		break;
	case ID_ALGNUPDATEALL:
	{
		int n = 0;
		CMPoint* pMP1 = NULL;
		do {
			pMP1 = FindMPoint(n++);
			if (pMP1) {
				GotoPointB(&m_cVideo, &pMP1->Co);
				OnMatchpattern();
				UpdatePoint(&pMP1->Co);
			}
		} while (pMP1);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
	break;
	case ID_ADDZPOSITION:
		Addzposition(pMP);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		break;
	case ID_SETMARKER1:
		pRcp->Patt.MP.SetMarker(pMP, 1);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_SETMARKER2:
		pRcp->Patt.MP.SetMarker(pMP, 2);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_SETMARKER3:
		pRcp->Patt.MP.SetMarker(pMP, 3);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER1:
		pRcp->Patt.MP.ClearMarker(pMP, 1);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER2:
		pRcp->Patt.MP.ClearMarker(pMP, 2);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER3:
		pRcp->Patt.MP.ClearMarker(pMP, 3);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	}

	*pResult = 0;
}

void CSetupProp51Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp51Dlg::OnInitialexposurebeforefocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPB4FOCUS) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPB4FOCUS;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPB4FOCUS;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::OnInitialfocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCB4MATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::FOCB4MATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::FOCB4MATCH;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::OnInitialexposureafterfocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAFOCUS) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPAFOCUS;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPAFOCUS;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::OnInitialfocus2() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCAMATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::FOCAMATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::FOCAMATCH;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::OnInitialexposureafterfocus2() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAMATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPAMATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPAMATCH;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::OnMatchatoffset() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::MATCHATOFFSET;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::MATCHATOFFSET;
	}
	RecipeToLocal();
}

void CSetupProp51Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp51Dlg::OnMark() {
	if (MarkThisPoint()) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp51Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp51Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp51Dlg::OnMatchpattern() {
	BlockUI();

	short PaSet = g->CurPaSet - 1;

	SDoPattrnMatching Param;

	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = TRUE;
	Param.bMoveCenter = TRUE;

	DoPatternMatching(Param, FALSE);

	ReleaseUI();
}

void CSetupProp51Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
		UpdateSelectionItem(pMP);
	}
	*pResult = 0;
}

void CSetupProp51Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

long CSetupProp51Dlg::OnDeletepoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp51Dlg::OnEditpoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp51Dlg::OnAddpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp51Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp51Dlg::OnRelocateToBack(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToBack(p);
	return 0;
}

long CSetupProp51Dlg::OnRelocateToTop(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToTop(p);
	return 0;
}

// Use by MOT only [11/11/2012 Yuen]
void CSetupProp51Dlg::OnAlignmentpos() {
	if (AfxMessageBox("Warning: Internal calibration will be changed.\nAre you sure?", MB_YESNO) != IDYES) {
		return;
	}
	float x, y, z;
	p413App->pMtr->GetXYPos(&x, &y);
	switch (g->ActiveCamera) {
	default:
		p413App->pMtr->GetEchoTopZPosDev(z);
		break;
	case CCamBase::THINFCAM:
		p413App->pMtr->GetThinFilmZPosDev(z);
		break;
	// [05072024 ZHIMING
	case CCamBase::CWL:
		p413App->pMtr->GetCWLTopZPosDev(z);
		break;
	case CCamBase::ROUGH:
		p413App->pMtr->GetRoughZPosDev(z);
		break;
	// ]
	}
	pRcp->AlgnXPos = x;
	pRcp->AlgnYPos = y;
	pRcp->AlgnZPos = z;
}

// Use by MOT only [11/11/2012 Yuen]
void CSetupProp51Dlg::OnGotoalignmentpos() {
	p413App->pMtr->GotoXY(pRcp->AlgnXPos, pRcp->AlgnYPos);
	switch (g->ActiveCamera) {
	default:
		p413App->pMtr->MoveEchoTopZ(pRcp->AlgnZPos);
		break;
	case CCamBase::THINFCAM:
		p413App->pMtr->MoveThinFilmZ(pRcp->AlgnZPos);
		break;
	// [05072024 ZHIMING
	case CCamBase::CWL:
		p413App->pMtr->MoveCWLTopZ(pRcp->AlgnZPos);
		break;
	case CCamBase::ROUGH:
		p413App->pMtr->MoveRoughZ(pRcp->AlgnZPos);
		break;
	// ]
	}
}

long CSetupProp51Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

long CSetupProp51Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp51Dlg::SetupColumn() {
	char* dwCjName[] =
	{
		"#", "Position", /*"Me",*/ "Pat"
	};
	int nSize[] = { 32, 160, /*32,*/ 32 };
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
}

void CSetupProp51Dlg::OnMicroscopelighton() {
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

void CSetupProp51Dlg::OnMicroscopelightoff() {
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

void CSetupProp51Dlg::OnPatset1() {
	g->CurPaSet = 1;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset2() {
	g->CurPaSet = 2;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset3() {
	g->CurPaSet = 3;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset4() {
	g->CurPaSet = 4;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset5() {
	g->CurPaSet = 5;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset6() {
	g->CurPaSet = 6;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset7() {
	g->CurPaSet = 7;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset8() {
	g->CurPaSet = 8;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatset9() {
	g->CurPaSet = 9;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnImg1() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(16);
	}
}

void CSetupProp51Dlg::OnImg2() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(17);
	}
}

void CSetupProp51Dlg::OnImg3() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(15);
	}
}

void CSetupProp51Dlg::OnImg4() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(18);
	}
}

void CSetupProp51Dlg::OnImg5() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(19);
	}
}

void CSetupProp51Dlg::OnImg6() {
	if (m_cVideo.IsSimu()) {
		m_cVideo.SimuDIBFilename = "";
		m_cVideo.LoadSimuDIB(20);
	}
}

// void CSetupProp51Dlg::OnSaverecipe()
// {
// 	UpdateData(TRUE);
// 	if (p413App->ValidCheckRcp(g->RcpSetup))
// 	{
// 		pRcp->SaveRecipe();
// 	}
// }

void CSetupProp51Dlg::OnKillfocusOdoffset() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][0] = m_ODOffset;
}

void CSetupProp51Dlg::OnKillfocusOdoffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][0] = m_ODOffsetY;
}

void CSetupProp51Dlg::OnKillfocusStioffset() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][1] = m_STIOffset;
}

void CSetupProp51Dlg::OnKillfocusStioffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][1] = m_STIOffsetY;
}

void CSetupProp51Dlg::OnClearlist() {
	m_cList.DeleteAllItems();
}

void CSetupProp51Dlg::OnFindcxcy() {
	m_cFindCxCy.EnableWindow(FALSE);
	BlockUI();
	m_cImgWnd.ClearTextInfo();
	BOOL bFE = FALSE;
	SFINDCENTER SFC;
	SFC.pCXOffset = &m_CX;
	SFC.pCYOffset = &m_CY;
	SFC.pCRadius = &m_CR;
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
	ReleaseUI();
	m_cFindCxCy.EnableWindow(TRUE);
	if (!bFE) {
		return;
	}
	Bevel = g->BevelWidth;
	UpdateData(FALSE);
}

void CSetupProp51Dlg::OnOptimize() {
	BlockUI();
	CString str;
	m_cOptimize.GetWindowText(str);
	if (str.Find("Optimize") != -1) {
		m_cOptimize.SetWindowText("Stop");
		OptimizeRecipe(pRcp);
	}
	else {
		g->bAborting = TRUE;
		m_cOptimize.SetWindowText("Optimize");
	}
	ReleaseUI();
}

void CSetupProp51Dlg::OptimizeRecipe(CRecipe413* pRcp) {
	CString str;
	CString MsgStr;
	SDoPattrnMatching Param;
	CMotorS2* pMtr = p413App->pMtr;

	UpdateData(TRUE);

	pRcp->UpdateControl(m_cPoint);

	MsgStr = "Clear Z info";
	p413App->Message(&m_cList, MsgStr);

	//	Param.pRcp = pRcp;
	Param.SearchDepth = 12;
	Param.pVideo = &m_cVideo;
	Param.pImgWnd = &m_cImgWnd;
	Param.pList = &m_cList;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}

	// Retrieving points database [9/20/2012 Administrator]
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		if (g->bAborting) {
			g->bAborting = FALSE;
			return;
		}
		CMPoint* pMP = (CMPoint*)pMPLst->GetNext(pos);
		if (pMP) {
			str.Format("%d", pMP->Co.n);
			m_cSelectedPt.SetWindowText(str);

			GotoXYZ(&pMP->Co, TRUE);

			// Do Auto exposure here [6/10/2013 Yuen]
			MsgStr = "Do auto exposure";
			p413App->Message(&m_cList, MsgStr);

			short AutoExpose = pRcp->AutoExposeEcho;
			if (g->ActiveCamera == CCamBase::THINFCAM) {
				AutoExpose = pRcp->AutoExposeTF;
			}
			m_cVideo.Level = AutoExpose;
			m_cVideo.AutoAdjust();

			Param.MeSet = pMP->MeSet;
			Param.PaSet = pMP->PatSet;
			// Data has not being link [7/10/2013 FSM413]
			if (!Param.MeSet || !Param.PaSet) {
				return;
			}
			if (Param.MeSet < 1) {
				Param.MeSet = 1;
			}
			if (Param.PaSet < 1) {
				Param.PaSet = 1;
			}
			for (int i = 0; i < 6; i++) {
				if (g->bAborting) {
					break;
				}
				MsgStr.Format("Find match pattern #%d", i);
				p413App->Message(&m_cList, MsgStr);

				switch (p413App->DoMatchingB(Param)) {
				case C413AppPatt::pmOK:
				{
					i = 6;
					float dX, dY;
					switch (g->ActiveCamera) {
					default:
						dX = g->ScrnCalibECH.dX * (Param.MoveDist.nX) * g->ScrnCalibECH.ScrnX / float(Param.pVideo->ImgWd);
						dY = g->ScrnCalibECH.dY * (Param.MoveDist.nY) * g->ScrnCalibECH.ScrnY / float(Param.pVideo->ImgHt);
						break;
					case CCamBase::THINFCAM:
						dX = g->ScrnCalibTF.dX * (Param.MoveDist.nX) * g->ScrnCalibTF.ScrnX / float(Param.pVideo->ImgWd);
						dY = g->ScrnCalibTF.dY * (Param.MoveDist.nY) * g->ScrnCalibTF.ScrnY / float(Param.pVideo->ImgHt);
						break;
					}
					MsgStr.Format("Goto new position, dX=%.3f  dY=%.3f", dX, dY);
					p413App->Message(&m_cList, MsgStr);

					pMtr->GotoXY(pMP->Co.x + dX, pMP->Co.y + dY);

					// Done, do auto focus here [6/10/2013 Yuen]
					MsgStr = "Do auto focus";
					p413App->Message(&m_cList, MsgStr);
					p413App->AutoFocusVideo(g->ActiveCamera, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd, NULL);

					MsgStr = "Record new position";
					p413App->Message(&m_cList, MsgStr);
					pMtr->GetXYPos(&pMP->Co.x, &pMP->Co.y);
					switch (g->ActiveCamera) {
					default:
						pMtr->GetEchoTopZPosDev(pMP->Co.z);
						pMP->Co.z -= pRcp->fEchoTopZ;
						break;
					case CCamBase::THINFCAM:
						pMtr->GetThinFilmZPosDev(pMP->Co.z);
						pMP->Co.z -= pRcp->fThinFilmZ;
						break;
					// [05072024 ZHIMING
					case CCamBase::CWL:
						pMtr->GetCWLTopZPosDev(pMP->Co.z);
						pMP->Co.z -= pRcp->fCWLTopZ;
						break;
					case CCamBase::ROUGH:
						pMtr->GetRoughZPosDev(pMP->Co.z);
						pMP->Co.z -= pRcp->fRoughZ;
						break;
					// ]
					}
					pRcp->UpdateControl(m_cPoint);
					continue;
				}
				break;
				case C413AppPatt::pmMOVE:
					p413App->MoveMatchPatCenter(Param);
					break;
				default:
					// Fail patt match, try readjust focus [7/10/2013 FSM413]
					MsgStr = "Do auto focus";
					p413App->Message(&m_cList, MsgStr);
					p413App->AutoFocusVideo(g->ActiveCamera, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd, NULL);
					break;
				}
			}
			Sleep(1000);
		}
	}
}

void CSetupProp51Dlg::GotoXYZ(CCoor* p, BOOL bZ) {
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return;
	}
	switch (g->ActiveCamera) {
	default:
		if (bZ) pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + p->z, FALSE);
		pMtr->GotoXY(p->x, p->y);
		if (bZ) pMtr->WaitEchoTopZ();
		break;
	case CCamBase::THINFCAM:
		if (bZ) pMtr->MoveThinFilmZ(pRcp->fThinFilmZ + p->z, FALSE);
		pMtr->GotoXY(p->x, p->y);
		if (bZ) pMtr->WaitThinFilmZ();
		break;
	// [05072024 ZHIMING
	case CCamBase::CWL:
		if (bZ) pMtr->MoveCWLTopZ(pRcp->fCWLTopZ + p->z, FALSE);
		pMtr->GotoXY(p->x, p->y);
		if (bZ) pMtr->WaitCWLTopZ();
		break;
	case CCamBase::ROUGH:
		if (bZ) pMtr->MoveRoughZ(pRcp->fRoughZ + p->z, FALSE);
		pMtr->GotoXY(p->x, p->y);
		if (bZ) pMtr->WaitRoughZ();
		break;
	// ]
	}
	m_cVideo.Index = p->n;
}

UINT CSetupProp51Dlg::ExecuteTF(LPVOID pObj) {
	CSetupProp51Dlg* pDlg = (CSetupProp51Dlg*)pObj;
	if (pDlg) {
		pDlg->DoMeasurement();
		pDlg->m_cMatch5.SetWindowText("Run\nRecipe");
		pDlg->m_cVideo.Reserved = "";
		pDlg->ReleaseUI();
		AfxEndThread(0);
	}
	return TRUE;
}

void CSetupProp51Dlg::DoMeasurement() {
	// Initialize Markers
	p413App->Global.bHasMarker = pRcp->Patt.HasMarkers();
	p413App->Global.DLoc.ClearAll();

	CPtrList* pMPList = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		if (p413App->Global.bStop) {
			p413App->Global.bStop = FALSE;
			return;
		}
		// Execute recipe [7/23/2013 Yuen]
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			p413App->MeasureThinFilm(pRcp, p, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList);
		}
	}
}

void CSetupProp51Dlg::OnSavevideo2() {
	if (pRcp->bSaveVideo) {
		pRcp->bSaveVideo = FALSE;
		m_cSaveVideo.SetCheck(FALSE);
	}
	else {
		pRcp->bSaveVideo = TRUE;
		m_cSaveVideo.SetCheck(TRUE);
	}
}

void CSetupProp51Dlg::OnFocus() {
	p413App->AutoFocusVideo(g->ActiveCamera, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd, NULL);
}

void CSetupProp51Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp51Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp51Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp51Dlg::OnSet4() {
	SwapMeSet(4);
}

void CSetupProp51Dlg::OnSet5() {
	SwapMeSet(5);
}

void CSetupProp51Dlg::OnSet6() {
	SwapMeSet(6);
}

void CSetupProp51Dlg::OnSet8() {
	SwapMeSet(7);
}

void CSetupProp51Dlg::OnSet9() {
	SwapMeSet(8);
}

void CSetupProp51Dlg::OnSet11() {
	SwapMeSet(9);
}

void CSetupProp51Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}

void CSetupProp51Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	pRcp->POffsetX[g->CurPaSet - 1][0] = m_ODOffset;
	pRcp->POffsetX[g->CurPaSet - 1][1] = m_STIOffset;
	pRcp->POffsetY[g->CurPaSet - 1][0] = m_ODOffsetY;
	pRcp->POffsetY[g->CurPaSet - 1][1] = m_STIOffsetY;
}

void CSetupProp51Dlg::OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult) {
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey) {
	case VK_DELETE:
	{
		nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		if (nSel > -1) {
			if (GetAsyncKeyState(VK_LCONTROL)) {
				DeletePoint(nSel);
				m_cPoint.DeleteItem(nSel);
				m_cWaferMap.Redraw();
			}
		}
	}
	break;
	}
	*pResult = 0;
}

void CSetupProp51Dlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		UpdateSelectionItem(pMP);
	}

	*pResult = 0;
}

void CSetupProp51Dlg::OnUp() {
	nSel--;
	if (nSel < 0) {
		nSel = 0;
		return;
	}
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp51Dlg::OnDn() {
	if (nSel >= m_cPoint.GetItemCount()) {
		return;
	}
	nSel++;
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp51Dlg::OnHere() {
	if ((nSel < 1) || (nSel >= m_cPoint.GetItemCount())) {
		return;
	}
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp51Dlg::UpdateSelectionItem(CMPoint* pMP) {
	CString str;
	str.Format("%d", pMP->Co.n);
	m_cSelectedPt.SetWindowText(str);
}

void CSetupProp51Dlg::OnAddzoffset() {
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		Addzposition(pMP);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CSetupProp51Dlg::OnUpdatept() {
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		UpdatePoint(&pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
}

// Match only [7/23/2013 Yuen]
void CSetupProp51Dlg::OnMatch6() {
	BlockUI();
	m_cList.DeleteAllItems();
	m_cMatch6.EnableWindow(FALSE);

	SDoPattrnMatching Param;

	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	Param.bMoveCenter = FALSE;

	p413App->bLoadDIB = FALSE;
	DoPatternMatching(Param, FALSE);
	p413App->bLoadDIB = TRUE;

	m_cMatch6.EnableWindow(TRUE);
	ReleaseUI();
}

// Match and move [7/27/2013 Yuen]
void CSetupProp51Dlg::OnMatch7() {
	BlockUI();
	m_cList.DeleteAllItems();
	m_cMatch7.EnableWindow(FALSE);

	SDoPattrnMatching Param;
	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = TRUE;
	Param.bMoveCenter = FALSE;

	p413App->bLoadDIB = FALSE;
	DoPatternMatching(Param, FALSE);
	p413App->bLoadDIB = TRUE;

	m_cMatch7.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp51Dlg::OnPatsseta() {
	nPatSSet = 0;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatssetb() {
	nPatSSet = 1;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatssetc() {
	nPatSSet = 2;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnPatssetd() {
	nPatSSet = 3;
	RecipeToLocal();
}

void CSetupProp51Dlg::OnFastmatching() {
	if (pRcp->bFastPMatch) {
		pRcp->bFastPMatch = FALSE;
		m_cFastMatching.SetCheck(FALSE);
	}
	else {
		pRcp->bFastPMatch = TRUE;
		m_cFastMatching.SetCheck(TRUE);
	}
}