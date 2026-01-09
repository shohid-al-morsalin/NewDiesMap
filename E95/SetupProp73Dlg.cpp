// SetupProp73Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include <math.h>
#include "413App.h"
#include "SetupProp73Dlg.h"
#include "XTabCtrl.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "EditPoint.h"
#include "..\413\MotorS2.h"

#include "..\SYS\CUMMsg.h"

#include "DiceView.h"
#include "MPoint.h"

#include "StopDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp73Dlg dialog

CSetupProp73Dlg::CSetupProp73Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp73Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp73Dlg)
	m_Radius = 150.0f;
	m_Pos3 = _T("");
	m_Pos2 = _T("");
	m_Pos1 = _T("");
	m_bIncludeOn = TRUE;
	m_ZPos = 0.0f;
	m_AutoExpose = 0;
	//}}AFX_DATA_INIT

	aX = aY = 0;
	dOX = dOY = 0;

	m_MarkerItems[0] = NULL;
	m_MarkerItems[1] = NULL;
	m_MarkerItems[2] = NULL;

	nPatSSet = 0;	// Fix it [5/5/2014 Administrator]
	m_cVideo.Name = "Setup73";
}

void CSetupProp73Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp73Dlg)
	DDX_Control(pDX, IDC_SETP3, m_cSetP3);
	DDX_Control(pDX, IDC_SETP2, m_cSetP2);
	DDX_Control(pDX, IDC_SETP1, m_cSetP1);
	DDX_Control(pDX, IDC_TESTEXPOSURE, m_cTestExposure);
	DDX_Control(pDX, IDC_OPTIMIZE, m_cOptimize);
	DDX_Control(pDX, IDC_CENTER, m_cCenter);
	DDX_Control(pDX, IDC_PATTERNA1, m_cImgWnd1);
	DDX_Control(pDX, IDC_MATCH, m_cMatch);
	DDX_Control(pDX, IDC_POINTSLOC, m_cPointLoc);
	DDX_Control(pDX, IDC_POINTS, m_cPoints);
	DDX_Control(pDX, IDC_CELLS, m_cCells);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Text(pDX, IDC_RADIUS, m_Radius);
	DDX_Text(pDX, IDC_POSITION3, m_Pos3);
	DDX_Text(pDX, IDC_POSITION2, m_Pos2);
	DDX_Text(pDX, IDC_POSITION1, m_Pos1);
	DDX_Check(pDX, IDC_EXCLUDENCOMPLETE, m_bIncludeOn);
	DDX_Text(pDX, IDC_ZPOSITION_SP4, m_ZPos);
	DDX_Text(pDX, IDC_EXPOSURE3, m_AutoExpose);
	DDX_Control(pDX, IDC_DICEWAFER, m_cDiceWafer);
	DDX_Control(pDX, IDC_COOR, m_cCoor);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_POSITION3, m_cPos[2]);
	DDX_Control(pDX, IDC_POSITION2, m_cPos[1]);
	DDX_Control(pDX, IDC_POSITION1, m_cPos[0]);
}

BEGIN_MESSAGE_MAP(CSetupProp73Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp73Dlg)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_CAMERASETTING4, OnCamerasetting4)
	ON_BN_CLICKED(IDC_MOTIONCONTROL2, OnMotioncontrol2)
	ON_BN_CLICKED(IDC_GOTO1, OnGoto1)
	ON_BN_CLICKED(IDC_GOTO2, OnGoto2)
	ON_BN_CLICKED(IDC_GOTO3, OnGoto3)
	ON_BN_CLICKED(IDC_SETP1, OnSetp1)
	ON_BN_CLICKED(IDC_SETP2, OnSetp2)
	ON_BN_CLICKED(IDC_SETP3, OnSetp3)
	ON_BN_CLICKED(IDC_AddP1, OnAddP1)
	ON_BN_CLICKED(IDC_AddP2, OnAddP2)
	ON_BN_CLICKED(IDC_AddP3, OnAddP3)
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_NOTIFY(NM_RCLICK, IDC_CELLS, OnRclickCells)
	ON_BN_CLICKED(IDC_CLEARDICE, OnCleardice)
	ON_NOTIFY(NM_CLICK, IDC_CELLS, OnClickCells)
	ON_NOTIFY(NM_DBLCLK, IDC_POINTS, OnDblclkPoints)
	ON_NOTIFY(NM_RCLICK, IDC_POINTS, OnRclickPoints)
	ON_EN_KILLFOCUS(IDC_RADIUS, OnKillfocusRadius)
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_ADDPOINT, OnAddpoint)
	ON_BN_CLICKED(IDC_CLEARPTS, OnClearpts)
	ON_BN_CLICKED(IDC_ORIGIN, OnOrigin)
	ON_BN_CLICKED(IDC_CAMERAECHO, OnCameraecho)
	ON_BN_CLICKED(IDC_CAMERATHINF, OnCamerathinf)
	ON_BN_CLICKED(IDC_GENMAP, OnGenmap)
	ON_BN_CLICKED(IDC_CLEARMAP, OnClearmap)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_BN_CLICKED(IDC_EXCLUDENCOMPLETE, OnExcludencomplete)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnOptimize)
	ON_EN_KILLFOCUS(IDC_ZPOSITION_SP4, OnKillfocusZpositionSp4)
	ON_BN_CLICKED(IDC_RECZPOS, OnReczpos)
	ON_EN_KILLFOCUS(IDC_EXPOSURE3, OnKillfocusExposure3)
	ON_BN_CLICKED(IDC_TESTEXPOSURE, OnTestexposure)
	ON_BN_CLICKED(IDC_ANCHOR, OnAnchor)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_STN_CLICKED(IDC_DICEWAFER, OnStnClickedDicewafer)
	ON_STN_DBLCLK(IDC_DICEWAFER, OnStnDblclickDicewafer)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp73Dlg message handlers
LRESULT CSetupProp73Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		if (!m_cVideo.IsReserved()) {
			p413App->StopCamera(&m_cVideo, NULL);
			m_cVideo.pWnd = NULL;
		}
		bTabSelected = FALSE;
	}
	return 0;
}

LRESULT CSetupProp73Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.pWnd = this;
			m_cVideo.bShowCrossHair = TRUE;
			m_cVideo.bShowCenterTarget = FALSE;
			m_cVideo.SetRecipe(&g->RcpSetup);
			m_cVideo.RSpecClear();
			m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
			m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][0];
		}

		RecipeToLocal();
		// XW 11/16/2013 Added Marker Selection Display
		if (m_bRedrawDiceMap)
			m_cDiceWafer.Redraw();
		else
			OnGenerate();
		bTabSelected = TRUE;
	}

	return 0;
}

void CSetupProp73Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp73Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_OPTIMIZE, TOP_RIGHT);
	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL2, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING4, TOP_RIGHT);
	AddAnchor(IDC_CENTER, TOP_RIGHT);
	AddAnchor(IDC_GENMAP, TOP_RIGHT);
	AddAnchor(IDC_CLEARMAP, TOP_RIGHT);
	AddAnchor(IDC_MATCH, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	m_cImgWnd1.bScaled = FALSE;
	m_cImgWnd1.bShowOverlay = FALSE;

	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	char* dwCjName[] =
	{
		"#", "X", "Y"
	};
	int nSize[] = { 32, 82, 82 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cCells.InsertColumn(i, &nListColumn);
		m_cPoints.InsertColumn(i, &nListColumn);
	}
	m_cCells.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_cPoints.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	RecipeToLocal();

	m_bRedrawDiceMap = FALSE;

	// [02052024 ZHIMING
	if (g->Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CAMERATHINF)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ANCHOR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_APPLY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_THINFILM_CAMERA_OFFSET_GB)->ShowWindow(SW_HIDE);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp73Dlg::RecipeToLocal() {
	m_Pos1.Format("(%.3f,%.3f)", pRcp->XYPair[0].x, pRcp->XYPair[0].y);
	m_Pos2.Format("(%.3f,%.3f)", pRcp->XYPair[1].x, pRcp->XYPair[1].y);
	m_Pos3.Format("(%.3f,%.3f)", pRcp->XYPair[2].x, pRcp->XYPair[2].y);

	if (m_cVideo.iCID == CCamBase::THINFCAM) {
		m_AutoExpose = pRcp->AutoExposeTF;
		m_ZPos = pRcp->fThinFilmZ;
	}
	else if (m_cVideo.iCID == CCamBase::ECHOCAM) {
		m_AutoExpose = pRcp->AutoExposeEcho;
		m_ZPos = pRcp->fEchoTopZ;
	}

	UpdateData(FALSE);
	//UpdateTargets(&m_cVideo);
	UpdateMeasurementPoints();
	m_cWaferMap.Redraw();
}

void CSetupProp73Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	if (m_cVideo.iCID == CCamBase::THINFCAM) {
		pRcp->fThinFilmZ = m_ZPos;
		pRcp->AutoExposeTF = m_AutoExpose;
	}
	else if (m_cVideo.iCID == CCamBase::ECHOCAM) {
		pRcp->fEchoTopZ = m_ZPos;
		pRcp->AutoExposeEcho = m_AutoExpose;
	}
}

void CSetupProp73Dlg::OnSaverecipe() {
	m_cWaferMap.Redraw();
	RecipeIO();
	OnGenerate();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp73Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		OnGenerate();
// 		RecipeToLocal();
// 	}
// }

long CSetupProp73Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

void CSetupProp73Dlg::Renumber() {
	pRcp->Renumber();
	m_cWaferMap.Redraw();
}

void CSetupProp73Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	p413App->StopCamera(&m_cVideo, NULL);

	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

void CSetupProp73Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);

	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

BOOL CSetupProp73Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp73Dlg::OnCamerasetting4() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp73Dlg::OnMotioncontrol2() {
	ShowMotionControl(this);
}

void CSetupProp73Dlg::UpdatePointPos(short n) {
	CString str;
	pRcp->XYPair[n].Get(str);
	m_cPos[n].SetWindowText(str);
}

void CSetupProp73Dlg::OnGoto1() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(pRcp->XYPair[0].x, pRcp->XYPair[0].y);
}

void CSetupProp73Dlg::OnGoto2() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(pRcp->XYPair[1].x, pRcp->XYPair[1].y);
}

void CSetupProp73Dlg::OnGoto3() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(pRcp->XYPair[2].x, pRcp->XYPair[2].y);
}

void CSetupProp73Dlg::OnSetp1() {
	SDoPattrnMatching Param;
	//	Param.pRcp = pRcp;
	Param.SearchDepth = 1;
	//	Param.ActiveCamera = g->ActiveCamera;
	Param.PaSet = g->CurPaSet;
	Param.pVideo = &m_cVideo;
	Param.pImgWnd = &m_cImgWnd1;
	Param.pList = NULL;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}

	m_cSetP1.EnableWindow(FALSE);
	if (!GetPatLocation(Param)) {
		AfxMessageBox("Fail: Check match template");
		m_cSetP1.EnableWindow(TRUE);
		return;
	}

	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&pRcp->XYPair[0].x, &pRcp->XYPair[0].y);
		pRcp->XYPair[0].x += Param.PatOffX;
		pRcp->XYPair[0].y += Param.PatOffY;
	}
	UpdatePointPos(0);
	OnGenerate();

	m_cSetP1.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnSetp2() {
	SDoPattrnMatching Param;
	//	Param.pRcp = pRcp;
	Param.SearchDepth = 1;
	//	Param.ActiveCamera = g->ActiveCamera;
	Param.PaSet = g->CurPaSet;
	Param.pVideo = &m_cVideo;
	Param.pImgWnd = &m_cImgWnd1;
	Param.pList = NULL;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}

	m_cSetP2.EnableWindow(FALSE);
	if (!GetPatLocation(Param)) {
		AfxMessageBox("Fail: Check match template");
		m_cSetP2.EnableWindow(TRUE);
		return;
	}

	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&pRcp->XYPair[1].x, &pRcp->XYPair[1].y);
		pRcp->XYPair[1].x += Param.PatOffX;
		pRcp->XYPair[1].y += Param.PatOffY;
	}
	UpdatePointPos(1);
	OnGenerate();
	m_cSetP2.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnSetp3() {
	SDoPattrnMatching Param;
	//	Param.pRcp = pRcp;
	Param.SearchDepth = 1;
	//	Param.ActiveCamera = g->ActiveCamera;
	Param.PaSet = g->CurPaSet;
	Param.pVideo = &m_cVideo;
	Param.pImgWnd = &m_cImgWnd1;
	Param.pList = NULL;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}

	m_cSetP3.EnableWindow(FALSE);
	if (!GetPatLocation(Param)) {
		AfxMessageBox("Fail: Check match template");
		m_cSetP3.EnableWindow(TRUE);
		return;
	}

	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&pRcp->XYPair[2].x, &pRcp->XYPair[2].y);
		pRcp->XYPair[2].x += Param.PatOffX;
		pRcp->XYPair[2].y += Param.PatOffY;
	}
	UpdatePointPos(2);
	OnGenerate();
	SelectMarker();

	m_cSetP3.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnAddP1() {
	CEditPoint dlg;
	dlg.m_XCoor = pRcp->XYPair[0].x;
	dlg.m_YCoor = pRcp->XYPair[0].y;
	if (dlg.DoModal() == IDOK) {
		pRcp->XYPair[0].x = dlg.m_XCoor;
		pRcp->XYPair[0].y = dlg.m_YCoor;

		CalcdXdY();
		UpdatePointPos(0);
		OnGenerate();
	}
}

void CSetupProp73Dlg::OnAddP2() {
	CEditPoint dlg;
	dlg.m_XCoor = pRcp->XYPair[1].x;
	dlg.m_YCoor = pRcp->XYPair[1].y;
	if (dlg.DoModal() == IDOK) {
		pRcp->XYPair[1].x = dlg.m_XCoor;
		pRcp->XYPair[1].y = dlg.m_YCoor;

		CalcdXdY();
		UpdatePointPos(1);
		OnGenerate();
	}
}

void CSetupProp73Dlg::OnAddP3() {
	CEditPoint dlg;
	dlg.m_XCoor = pRcp->XYPair[2].x;
	dlg.m_YCoor = pRcp->XYPair[2].y;
	if (dlg.DoModal() == IDOK) {
		pRcp->XYPair[2].x = dlg.m_XCoor;
		pRcp->XYPair[2].y = dlg.m_YCoor;

		CalcdXdY();
		UpdatePointPos(2);
		OnGenerate();
	}
}

void CSetupProp73Dlg::OnGenerate() {
	UpdateData(TRUE);

	m_cDiceWafer.Clear();
	m_cCells.DeleteAllItems();
	m_MarkerItems[0] = NULL;
	m_MarkerItems[1] = NULL;
	m_MarkerItems[2] = NULL;
	GenerateMap();
}

void CSetupProp73Dlg::OnCleardice() {
	m_cCells.DeleteAllItems();
	m_cDiceWafer.Clear();
	m_cDiceWafer.Redraw();
}

void CSetupProp73Dlg::OnRclickCells(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cCells.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CString msg;
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENU413);
	pPopUp = menu.GetSubMenu(3);
	if (!pPopUp) return;
	ClientToScreen(&point);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
	switch (res) {
	case ID_USELOCATION:
		// 		bSel = TRUE;
		// 		m_cCells.GetItemText(nSel,0,str,128);
		// 		nCell = atof(str);
		// 		m_cCells.GetItemText(nSel,1,str,128);
		// 		O.x = atof(str);
		// 		m_cCells.GetItemText(nSel,2,str,128);
		// 		O.y = atof(str);

		// 		msg.Format("%.3f,%.3f",O.x,O.y);
		// 		m_cCoor.SetWindowText(msg);

		// 		msg.Format("Measurement Points (cell %d)", nCell);
		// 		m_cPointLoc.SetWindowText(msg);
		break;
	case ID_DELETEDICE:
		// 		m_cCells.DeleteItem(nSel);
		// 		ReloadDiceWafer();
		break;
	}

	*pResult = 0;
}

void CSetupProp73Dlg::OnClickCells(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	char buf[32];
	int nSel = m_cCells.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSel != -1) {
		float X, Y;
		m_cCells.GetItemText(nSel, 1, buf, 32);
		X = atof(buf);
		m_cCells.GetItemText(nSel, 2, buf, 32);
		Y = atof(buf);
		m_cDiceWafer.Locate(X, Y);
	}
	*pResult = 0;
}

void CSetupProp73Dlg::ReloadDiceWafer() {
	m_cDiceWafer.Clear();
	m_MarkerItems[0] = NULL;
	m_MarkerItems[1] = NULL;
	m_MarkerItems[2] = NULL;
	char buf[32];
	float X, Y;
	for (int i = 0; i < m_cCells.GetItemCount(); i++) {
		m_cCells.GetItemText(i, 1, buf, 32);
		X = atof(buf);
		m_cCells.GetItemText(i, 2, buf, 32);
		Y = atof(buf);
		m_cDiceWafer.AddCell(X, Y, dX.x, dY.y, i + 1);
	}
	m_cDiceWafer.Redraw();
}

void CSetupProp73Dlg::OnStnClickedDicewafer() {
	float fX, fY;
	GetCursorPos(&coord);
	m_cDiceWafer.GetCoor(coord, fX, fY);
	m_cDiceWafer.Locate(fX, fY);
	// 	m_cDiceWafer.Redraw();

	CString str;
	str.Format("(%d,%d)  (%.3f,%.3f)", coord.x, coord.y, fX, fY);
	m_cCoor.SetWindowText(str);

	// XW 11/16/2013 Commented out the following to keep three marker site at the beginning of the list
	//pRcp->Patt.MP.Clear();
	OnGenmap();
}

void CSetupProp73Dlg::SelectDice(float fX, float fY) {
	float X, Y;
	char buf[32];
	for (int i = 0; i < m_cCells.GetItemCount(); i++) {
		m_cCells.GetItemText(i, 1, buf, 32);
		X = atof(buf);
		m_cCells.GetItemText(i, 2, buf, 32);
		Y = atof(buf);

		if (IsInDice(fX, fY, X, Y)) {
			Sel = i;
			m_cDiceWafer.Locate(X, Y);
			break;
		}
	}
}

bool CSetupProp73Dlg::IsInDice(float fX, float fY, float X, float Y) {
	if ((fX >= X) && (fX <= (X + dX.x))) {
		if ((fY >= Y) && (fY <= (Y + dY.y))) {
			return true;
		}
	}
	return false;
}

void CSetupProp73Dlg::OnStnDblclickDicewafer() {
	// 	m_cCells.DeleteItem(Sel);
	// 	ReloadDiceWafer();
}

void CSetupProp73Dlg::OnDblclkPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CSetupProp73Dlg::OnRclickPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CSetupProp73Dlg::SetCellItem(short idx, float x, float y) {
	CString str;
	short i = 0, n = 1;
	str.Format("%d", idx);
	int ii = m_cCells.InsertItem(idx + 1, str);
	str.Format("%.3f", x);
	m_cCells.SetItemText(ii, n++, str);
	str.Format("%.3f", y);
	m_cCells.SetItemText(ii, n++, str);
}

void CSetupProp73Dlg::OnKillfocusRadius() {
	UpdateData(TRUE);
	OnGenerate();
}

void CSetupProp73Dlg::OnMatch() {
	m_cMatch.EnableWindow(FALSE);

	SDoPattrnMatching Param;

	short MatchAlgo = pRcp->MatchAlgorithm[g->CurPaSet - 1];
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 0;
	p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}
	Param.Set(&m_cVideo, &m_cImgWnd1, NULL);

	p413App->bLoadDIB = FALSE;
	switch (p413App->DoPattrnMatching(Param)) {
	case C413AppPatt::pmOK:
		break;
	default:	// All other cases consider failure [2/25/2013 Administrator]
		p413App->bLoadDIB = TRUE;
		AfxMessageBox("Not Found");
		pRcp->MatchAlgorithm[g->CurPaSet - 1] = MatchAlgo;
		m_cMatch.EnableWindow(TRUE);
		return;
		break;
	}
	p413App->bLoadDIB = TRUE;
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = MatchAlgo;
	m_cMatch.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnCenter() {
	float Cx, Cy, Cr;
	m_cCenter.EnableWindow(FALSE);
	m_cImgWnd1.ClearTextInfo();
	BOOL bFE = FALSE;
	SFINDCENTER SFC;
	SFC.pCXOffset = &Cx;
	SFC.pCYOffset = &Cy;
	SFC.pCRadius = &Cr;
	SFC.pRcp = pRcp;
	SFC.pVideo = &m_cVideo;
	SFC.pImgWnd = &m_cImgWnd1;
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
		m_cCenter.EnableWindow(TRUE);
		return;
	}
	//	p413App->FindWaferCenterUsingEdgeDetection(Cx,Cy,Cr,&g->RcpSetup,&m_cVideo,&m_cImgWnd1,NULL);
	m_cCenter.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnAddpoint() {
	// Only 2 points are allowed [6/22/2013 Yuen]
	if (m_cPoints.GetItemCount() >= 2) {
		return;
	}
	float X, Y;
	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&X, &Y);
		short idx = m_cPoints.GetItemCount();
		X -= pRcp->XYPair[0].x;
		Y -= pRcp->XYPair[0].y;
		pRcp->XYPairOffset[pRcp->XYPairCount].x = X;
		pRcp->XYPairOffset[pRcp->XYPairCount].y = Y;
		pRcp->XYPairCount++;

		CString str;
		short i = 0, n = 1;
		str.Format("%d", idx + 1);
		int ii = m_cPoints.InsertItem(idx + 1, str);
		str.Format("%.3f", X);
		m_cPoints.SetItemText(ii, n++, str);
		str.Format("%.3f", Y);
		m_cPoints.SetItemText(ii, n++, str);
	}
}

void CSetupProp73Dlg::OnClearpts() {
	m_cPoints.DeleteAllItems();
	m_cPointLoc.SetWindowText("Measurement Points");

	pRcp->XYPairCount = 0;
	for (int i = 0; i < MAXXYPAIROFFSET; i++) {
		pRcp->XYPairOffset[i].x = 0;
		pRcp->XYPairOffset[i].y = 0;
	}
}

void CSetupProp73Dlg::OnOrigin() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(pRcp->XYPair[0].x, pRcp->XYPair[0].y);
}

void CSetupProp73Dlg::OnCameraecho() {
	float x, y;
	p413App->pMtr->GetXYPos(&x, &y);
	//StopAllCamera(&m_cVideo);
	p413App->StopCamera(&m_cVideo, NULL);
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	//UpdateTargets(&m_cVideo);
	p413App->pMtr->GotoXY(x, y);
}

void CSetupProp73Dlg::OnCamerathinf() {
	float x, y;
	p413App->pMtr->GetXYPos(&x, &y);
	//StopAllCamera(&m_cVideo);
	p413App->StopCamera(&m_cVideo, NULL);
	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	//UpdateTargets(&m_cVideo);
	p413App->pMtr->GotoXY(x + dOX, y + dOY);
}

long CSetupProp73Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp73Dlg::OnGenmap() {
	short n = 1;
	C413Global* g = &p413App->Global;
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	BOOL markerVerified = FALSE;
	if (pMPLst->GetCount() >= 3) {
		markerVerified = TRUE;
	}
	pRcp->Patt.MP.Clear();
	pMPLst = &pRcp->Patt.MP.MPLst;
	if (markerVerified) {
		for (int i = 0; i < 3; i++) {
			if (m_MarkerItems[i] != NULL) {
				CMPoint* pMP = new CMPoint;
				pMP->Co.SetXYZ(n++, m_MarkerItems[i]->X, m_MarkerItems[i]->Y, 0);
				pMP->Co.p = i + 1;
				pMPLst->AddTail(pMP);
			}
		}
	}

	POSITION pos = m_cDiceWafer.Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)m_cDiceWafer.Sites.GetNext(pos);
		if (p) {
			if ((p->bSel) &&
				(!markerVerified ||
					((p != m_MarkerItems[0]) &&
						(p != m_MarkerItems[1]) &&
						(p != m_MarkerItems[2])))) {
				CMPoint* pMP = new CMPoint;
				pMP->Co.SetXYZ(n++, p->X, p->Y, 0);
				pMPLst->AddTail(pMP);
			}
		}
	}

	char buf[36];
	for (int i = 0; i < m_cPoints.GetItemCount(); i++) {
		switch (i) {
		case 0:
			m_cPoints.GetItemText(i, 1, buf, 32);
			pRcp->POffsetX[g->CurPaSet - 1][0] = atof(buf);
			m_cPoints.GetItemText(i, 2, buf, 32);
			pRcp->POffsetY[g->CurPaSet - 1][0] = atof(buf);
			break;
		case 1:
			m_cPoints.GetItemText(i, 1, buf, 32);
			pRcp->POffsetX[g->CurPaSet - 1][1] = atof(buf);
			m_cPoints.GetItemText(i, 2, buf, 32);
			pRcp->POffsetY[g->CurPaSet - 1][1] = atof(buf);
			break;
		}
	}

	m_cWaferMap.Redraw();
}

void CSetupProp73Dlg::OnClearmap() {
	pRcp->Patt.MP.Clear();
	m_cWaferMap.Redraw();
}

void CSetupProp73Dlg::OnSelectall() {
	m_cDiceWafer.SetSelectAll();
	pRcp->Patt.MP.Clear();
	OnGenmap();
}

void CSetupProp73Dlg::OnClearall() {
	m_cDiceWafer.ClearSelectAll();
	pRcp->Patt.MP.Clear();
	m_cWaferMap.Redraw();
}

void CSetupProp73Dlg::UpdateMeasurementPoints() {
	m_cPoints.DeleteAllItems();
	short idx = m_cPoints.GetItemCount();
	for (int i = 0; i < pRcp->XYPairCount; i++) {
		CString str;
		short n = 1;
		str.Format("%d", i + 1);
		int ii = m_cPoints.InsertItem(i + 1, str);
		str.Format("%.3f", pRcp->XYPairOffset[i].x);
		m_cPoints.SetItemText(ii, n++, str);
		str.Format("%.3f", pRcp->XYPairOffset[i].y);
		m_cPoints.SetItemText(ii, n++, str);
	}
}

void CSetupProp73Dlg::CalcdXdY() {
	dX.x = pRcp->XYPair[2].x - pRcp->XYPair[0].x;//x
	dX.y = pRcp->XYPair[2].y - pRcp->XYPair[0].y;

	dY.x = pRcp->XYPair[1].x - pRcp->XYPair[0].x;
	dY.y = pRcp->XYPair[1].y - pRcp->XYPair[0].y;//y
}

void CSetupProp73Dlg::OnExcludencomplete() {
	UpdateData(TRUE);
	OnGenerate();
}

void CSetupProp73Dlg::OnKillfocusZpositionSp4() {
	UpdateData(TRUE);
	switch (m_cVideo.iCID) {
	default:
		pRcp->fEchoTopZ = m_ZPos;
		break;
	case CCamBase::THINFCAM:
		pRcp->fThinFilmZ = m_ZPos;
		break;
	}
}

void CSetupProp73Dlg::OnReczpos() {
	switch (m_cVideo.iCID) {
	default:
		if (p413App->pMtr) p413App->pMtr->GetEchoTopZPosDev(m_ZPos);
		pRcp->fEchoTopZ = m_ZPos;
		break;
	case CCamBase::THINFCAM:
		if (p413App->pMtr) p413App->pMtr->GetThinFilmZPosDev(m_ZPos);
		pRcp->fThinFilmZ = m_ZPos;
		break;
	}
	UpdateData(FALSE);
}

void CSetupProp73Dlg::OnKillfocusExposure3() {
	UpdateData(TRUE);
	if (m_cVideo.iCID == CCamBase::THINFCAM) {
		pRcp->AutoExposeTF = m_AutoExpose;
	}
	else if (m_cVideo.iCID == CCamBase::ECHOCAM) {
		pRcp->AutoExposeEcho = m_AutoExpose;
	}
}

void CSetupProp73Dlg::OnOptimize() {
	m_cOptimize.EnableWindow(FALSE);
	g->bStop = FALSE;
	g->bAborting = FALSE;

	CStopDlg* pdlg = new CStopDlg;
	if (pdlg) {
		pdlg->Create(CStopDlg::IDD);
		pdlg->ShowWindow(SW_SHOW);
	}

	CMotorS2* pMtr = p413App->pMtr;
	CPtrList* pMPList = &pRcp->Patt.MP.MPLst;

	// 	pMtr->MoveThinFilmZ(pRcp->fThinFilmZ,TRUE);

	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			// goto the point [6/23/2013 Yuen]
			if (g->bStop) break;
			CCoor* pCo = &p->Co;
			pMtr->MoveThinFilmZ(pRcp->fThinFilmZ + pCo->z, FALSE);
			pMtr->GotoXY(pCo->x, pCo->y);
			pMtr->WaitForAll(15000);

			p413App->LoadSimuDIB(&m_cVideo);

			// adjust exposure [6/23/2013 Yuen]
			if (g->bStop) break;
			m_cVideo.Level = m_AutoExpose;
			m_cVideo.AutoAdjust();

			// adjust focus [6/23/2013 Yuen]
			if (g->bStop) break;
			p413App->AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd1, 0);

			// Matching [6/23/2013 Yuen]
			if (g->bStop) break;
			OnMatch();

			// adjust focus [6/23/2013 Yuen]
			if (g->bStop) break;
			p413App->AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd1, 0);

			// update x,y,z positions [6/23/2013 Yuen]
			if (g->bStop) break;
			float zPos;
			pMtr->GetXYPos(&pCo->x, &pCo->y);
			pMtr->GetThinFilmZPosDev(zPos);
			pCo->z = zPos - pRcp->fThinFilmZ;
		}
	}
	if (pdlg) {
		pdlg->DestroyWindow();
		delete pdlg;
	}
	g->bStop = FALSE;
	m_cOptimize.EnableWindow(TRUE);
}

void CSetupProp73Dlg::GenerateMap() {
	int idx = 1;
	float x, y, bx, by, Ax, Ay;

	// Anchor point [6/19/2013 Yuen]
	Ax = pRcp->XYPair[0].x;
	Ay = pRcp->XYPair[0].y;

	dX.x = (pRcp->XYPair[2].x - pRcp->XYPair[0].x);
	dX.y = (pRcp->XYPair[2].y - pRcp->XYPair[0].y);
	dY.y = (pRcp->XYPair[1].y - pRcp->XYPair[0].y);
	dY.x = (pRcp->XYPair[1].x - pRcp->XYPair[0].x);

	if (dX.x <= 2) { // 11102025 MORSALIN
		return;
	}
	if (dY.y <= 2) { // 11102025 MORSALIN
		return;
	}

	short szX = 50;
	short szY = 50;
	for (int j = -szY; j <= szY; j++)  // y direction [3/10/2011 FSMT]
	{
		for (int i = -szX; i <= szX; i++) // x direction [3/10/2011 FSMT]
		{
			x = Ax + i * dX.x + j * dY.x;
			y = Ay + j * dY.y + i * dX.y;
			bx = x; by = y;
			float dist1 = (float)sqrt(x * x + y * y);
			x += dX.x; y += dX.y;
			float dist2 = (float)sqrt(x * x + y * y);
			y += dY.y; x += dY.x;
			float dist3 = (float)sqrt(x * x + y * y);
			x -= dX.x; y -= dX.y;
			float dist4 = (float)sqrt(x * x + y * y);

			short cnt = 0;
			BOOL bFirst = FALSE;
			if (dist1 <= m_Radius) {
				bFirst = TRUE;
				cnt++;
			}
			if (dist2 <= m_Radius) {
				cnt++;
			}
			if (dist3 <= m_Radius) {
				cnt++;
			}
			if (dist4 <= m_Radius) {
				cnt++;
			}

			if (bFirst) {
				if (!m_bIncludeOn && (cnt != 4)) {
					continue;
				}
				SetCellItem(idx, bx, by);
				m_cDiceWafer.AddCell(bx, by, dX.x, dY.y, idx);
				idx++;
			}
		}
	}
}

BOOL CSetupProp73Dlg::GetPatLocation(SDoPattrnMatching& Param) {
	if (!Param.pVideo) {
		return FALSE;
	}
	float dX, dY;
	switch (p413App->DoMatchingB(Param)) {
	case C413AppPatt::pmOK:
		switch (Param.pVideo->iCID) {
		default:
			dX = g->ScrnCalibECH.dX;
			dY = g->ScrnCalibECH.dY;
			break;
		case CCamBase::THINFCAM:
			dX = g->ScrnCalibTF.dX;
			dY = g->ScrnCalibTF.dY;
			break;
		}
		// Current match position at current position
		Param.PatOffX = Param.MoveDist.nX * dX;
		Param.PatOffY = Param.MoveDist.nY * dY;
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}

void CSetupProp73Dlg::OnTestexposure() {
	short Exposure;
	if (m_cVideo.iCID == CCamBase::THINFCAM) {
		Exposure = pRcp->AutoExposeTF;
	}
	else if (m_cVideo.iCID == CCamBase::ECHOCAM) {
		Exposure = pRcp->AutoExposeEcho;
	}
	m_cTestExposure.EnableWindow(FALSE);
	m_cVideo.Level = Exposure;
	m_cVideo.AutoAdjust();
	m_cTestExposure.EnableWindow(TRUE);
}

void CSetupProp73Dlg::OnAnchor() {
	p413App->pMtr->GetXYPos(&aX, &aY);
}

void CSetupProp73Dlg::OnApply() {
	p413App->pMtr->GetXYPos(&dOX, &dOY);
	dOX -= aX;
	dOY -= aY;
}

void CSetupProp73Dlg::SelectMarker() {
	OnClearall();
	short n = 1;
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	for (int i = 0; i < 3; i++) {
		double dist;
		double maxdist = 1000.0F;
		POSITION pos = m_cDiceWafer.Sites.GetHeadPosition();
		CSiteItem* markerDice = NULL;
		float markerx = pRcp->XYPair[i].x;
		float markery = pRcp->XYPair[i].y;
		double dx, dy;
		while (pos) {
			CSiteItem* p = (CSiteItem*)m_cDiceWafer.Sites.GetNext(pos);
			if (p) {
				dx = (double)(p->X - markerx);
				dy = (double)(p->Y - markery);
				dist = sqrt((dx * dx) + (dy * dy));
				if (dist < maxdist) {
					maxdist = dist;
					markerDice = p;
				}
			}
		}
		if (markerDice != NULL) {
			m_MarkerItems[i] = markerDice;
			markerDice->bSel = TRUE;
			CMPoint* pMP = new CMPoint;
			pMP->Co.SetXYZ(n++, markerDice->X, markerDice->Y, 0);
			pMP->Co.p = i + 1;
			pMPLst->AddTail(pMP);
		}
	}

	m_cDiceWafer.Redraw();
	m_cWaferMap.Redraw();
	m_bRedrawDiceMap = TRUE;
}