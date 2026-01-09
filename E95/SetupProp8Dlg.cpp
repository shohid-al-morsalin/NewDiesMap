// SetupProp8Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "SetupProp8Dlg.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "413App.h"

#include "..\SYS\CUMMsg.h"
#include "CrossHair.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp8Dlg dialog

CSetupProp8Dlg::CSetupProp8Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp8Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp8Dlg)
	TFdX = 0.0f;
	ECHdX = 0.0f;
	TFdY = 0.0f;
	ECHdY = 0.0f;
	m_MLocX1 = 0.0f;
	m_MLocX2 = 0.0f;
	m_MLocY1 = 0.0f;
	m_MLocY2 = 0.0f;
	m_ASizeThinFilm = 0.0f;
	m_ASizeEcho = 0.0f;
	TFmX = 0.0f;
	ECHmX = 0.0f;
	TFmY = 0.0f;
	ECHmY = 0.0f;
	//}}AFX_DATA_INIT

	dx = 0.001f;

	GLocX1 = GLocY1 = 0.5f;
	RLocX1E = RLocY1E = 0.5f;
	RLocX1T = RLocY1T = 0.5f;
	m_cVideo.Name = "Setup8";
}

void CSetupProp8Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp8Dlg)
	DDX_Control(pDX, IDC_MESSAGE3, m_cMessage2);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_CAMERASETTING, m_cCameraSetting);
	DDX_Control(pDX, IDC_SPEED, m_cSpeed);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Text(pDX, IDC_OFFSETX, TFdX);
	DDX_Text(pDX, IDC_OFFSETX2, ECHdX);
	DDX_Text(pDX, IDC_OFFSETY, TFdY);
	DDX_Text(pDX, IDC_OFFSETY2, ECHdY);
	DDX_Text(pDX, IDC_MLOCX1, m_MLocX1);
	DDX_Text(pDX, IDC_MLOCX2, m_MLocX2);
	DDX_Text(pDX, IDC_MLOCY1, m_MLocY1);
	DDX_Text(pDX, IDC_MLOCY2, m_MLocY2);
	DDX_Text(pDX, IDC_ASIZETHINFILM, m_ASizeThinFilm);
	DDX_Text(pDX, IDC_ASIZEECHO, m_ASizeEcho);
	DDX_Text(pDX, IDC_OFFSETX3, TFmX);
	DDX_Text(pDX, IDC_OFFSETX4, ECHmX);
	DDX_Text(pDX, IDC_OFFSETY3, TFmY);
	DDX_Text(pDX, IDC_OFFSETY4, ECHmY);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp8Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp8Dlg)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL, OnMotioncontrol)
	ON_BN_CLICKED(IDC_FIRSTLEFT, OnFirstleft)
	ON_BN_CLICKED(IDC_FIRSTRIGHT, OnFirstright)
	ON_BN_CLICKED(IDC_SECONDLEFT, OnSecondleft)
	ON_BN_CLICKED(IDC_SECONDRIGHT, OnSecondright)
	ON_BN_CLICKED(IDC_FIRSTTOP, OnFirsttop)
	ON_BN_CLICKED(IDC_FIRSTBOTTOM, OnFirstbottom)
	ON_BN_CLICKED(IDC_SECONDTOP, OnSecondtop)
	ON_BN_CLICKED(IDC_SECONDBOTTOM, OnSecondbottom)
	ON_LBN_KILLFOCUS(IDC_SPEED, OnKillfocusSpeed)
	ON_BN_CLICKED(IDC_SETSCREEN2MMOTOR, OnSetscreen2motor)
	ON_BN_CLICKED(IDC_SETPIXEL2MICRONX, OnSetpixel2micron)
	ON_BN_CLICKED(IDC_MARKRED, OnMarkred)
	ON_BN_CLICKED(IDC_MARKGREEN, OnMarkgreen)
	ON_BN_CLICKED(IDC_SETPIXEL2MICRONY, OnSetpixel2microny)
	ON_EN_KILLFOCUS(IDC_OFFSETX2, OnKillfocusOffsetx2)
	ON_EN_KILLFOCUS(IDC_OFFSETY2, OnKillfocusOffsety2)
	ON_EN_KILLFOCUS(IDC_OFFSETX, OnKillfocusOffsetx)
	ON_EN_KILLFOCUS(IDC_OFFSETY, OnKillfocusOffsety)
	ON_BN_CLICKED(IDC_CROSSHAIR, OnCrosshair)
	ON_BN_CLICKED(IDC_SETSCREEN2MMOTOR2, OnSetscreen2mmotor2)
	ON_BN_CLICKED(IDC_GOTOZEROXY, OnGotozeroxy)
	ON_BN_CLICKED(IDC_TAKEPICTURE, OnTakepicture)
	ON_BN_CLICKED(IDC_THIRDTOP, OnThirdtop)
	ON_BN_CLICKED(IDC_THIRDBOTTOM, OnThirdbottom)
	ON_BN_CLICKED(IDC_THIRDLEFT, OnThirdleft)
	ON_BN_CLICKED(IDC_THIRDRIGHT, OnThirdright)
	ON_BN_CLICKED(IDC_SETPTMECHO, OnSetptmecho)
	ON_BN_CLICKED(IDC_SETPTMTF, OnSetptmtf)
	ON_BN_CLICKED(IDC_REVERSEY, OnReversey)
	ON_BN_CLICKED(IDC_REVERSEYTF, OnReverseytf)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp8Dlg message handlers
long CSetupProp8Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}

		// Save working cursor [1/15/2013 Administrator]
		GLocX1 = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
		GLocY1 = pRcp->fTgtGRnY[g->CurPaSet - 1][0];

		RLocX1T = g->fTgtRedTF.X;
		RLocY1T = g->fTgtRedTF.Y;
		RLocX1E = g->fTgtRedECHO.X;
		RLocY1E = g->fTgtRedECHO.Y;

		// Restore original cursor [1/15/2013 Administrator]
		pRcp->fTgtGRnX[g->CurPaSet - 1][0] = GLocX;
		pRcp->fTgtGRnY[g->CurPaSet - 1][0] = GLocY;

		g->fTgtRedTF.X = RLocXT;
		g->fTgtRedTF.Y = RLocYT;
		g->fTgtRedECHO.X = RLocXE;
		g->fTgtRedECHO.Y = RLocYE;

		g->ScrnCalibTF.dX = TFdX;
		g->ScrnCalibTF.dY = TFdY;

		g->ScrnCalibECH.dX = ECHdX;
		g->ScrnCalibECH.dY = ECHdY;

		g->ScrnCalibTF.ppmX = TFmX;
		g->ScrnCalibTF.ppmY = TFmY;

		g->ScrnCalibECH.ppmX = ECHmX;
		g->ScrnCalibECH.ppmY = ECHmY;
	}

	bTabSelected = FALSE;

	return 0;
}

long CSetupProp8Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	// Save original cursor [1/15/2013 Administrator]
	GLocX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
	GLocY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];

	RLocXT = g->fTgtRedTF.X;
	RLocYT = g->fTgtRedTF.Y;
	RLocXE = g->fTgtRedECHO.X;
	RLocYE = g->fTgtRedECHO.Y;

	// Load working cursor [1/15/2013 Administrator]
	pRcp->fTgtGRnX[g->CurPaSet - 1][0] = GLocX1;
	pRcp->fTgtGRnY[g->CurPaSet - 1][0] = GLocY1;

	g->fTgtRedTF.X = RLocX1T;
	g->fTgtRedTF.Y = RLocY1T;
	g->fTgtRedECHO.X = RLocX1E;
	g->fTgtRedECHO.Y = RLocY1E;

	if (!m_cVideo.IsReserved()) {
		p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
		m_pVideo = &m_cVideo;
		m_cVideo.pWnd = this;
		m_cVideo.bDisableScaling = FALSE;
		m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
		m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];
		m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][0];
		m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][0];
	}

	//UpdateTargets(&m_cVideo);

	TFdX = g->ScrnCalibTF.dX;
	TFdY = g->ScrnCalibTF.dY;

	ECHdX = g->ScrnCalibECH.dX;
	ECHdY = g->ScrnCalibECH.dY;

	TFmX = g->ScrnCalibTF.ppmX;
	TFmY = g->ScrnCalibTF.ppmY;

	ECHmX = g->ScrnCalibECH.ppmX;
	ECHmY = g->ScrnCalibECH.ppmY;

	UpdateData(FALSE);

	bTabSelected = TRUE;
	return 0;
}

void CSetupProp8Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp8Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

BOOL CSetupProp8Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_MOTIONCONTROL, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);
	AddAnchor(IDC_GOTOZEROXY, TOP_RIGHT);
	AddAnchor(IDC_CROSSHAIR, TOP_RIGHT);
	AddAnchor(IDC_TAKEPICTURE, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (g->Confg.bEchoMicroscopeMod) {
		m_cCameraB.ShowWindow(SW_SHOW);
		m_cCameraSetting.ShowWindow(SW_SHOW);
	}
	if (g->Confg.bThinFilmMicroscopeMod) {
		m_cCameraA.ShowWindow(SW_SHOW);
		m_cCameraSetting.ShowWindow(SW_SHOW);
	}

	m_cVideo.SetRecipe(&g->RcpSetup);

	m_cSpeed.AddString("1");
	m_cSpeed.AddString("2");
	m_cSpeed.AddString("3");
	m_cSpeed.AddString("4");
	m_cSpeed.AddString("5");
	m_cSpeed.AddString("6");

	RecipeToLocal();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp8Dlg::RecipeToLocal() {
	if (g->bImageFlipYECHO) {
		((CButton*)GetDlgItem(IDC_REVERSEY))->SetCheck(TRUE);
	}
	else {
		((CButton*)GetDlgItem(IDC_REVERSEY))->SetCheck(FALSE);
	}
	if (g->bImageFlipYTFILM) {
		((CButton*)GetDlgItem(IDC_REVERSEYTF))->SetCheck(TRUE);
	}
	else {
		((CButton*)GetDlgItem(IDC_REVERSEYTF))->SetCheck(FALSE);
	}
}

void CSetupProp8Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	//UpdateTargets(&m_cVideo);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp8Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	//UpdateTargets(&m_cVideo);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp8Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp8Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp8Dlg::OnFirstleft() {
	g->fTgtRedTF.X -= dx;
	g->fTgtRedECHO.X -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnFirstright() {
	g->fTgtRedTF.X += dx;
	g->fTgtRedECHO.X += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnFirsttop() {
	g->fTgtRedTF.Y -= dx;
	g->fTgtRedECHO.Y -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnFirstbottom() {
	g->fTgtRedTF.Y += dx;
	g->fTgtRedECHO.Y += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnSecondleft() {
	m_cVideo.fTGtGrnX /*= pRcp->fTgtGRnX[g->CurPaSet-1][0]*/ -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnSecondright() {
	m_cVideo.fTGtGrnX /*= pRcp->fTgtGRnX[g->CurPaSet-1][0]*/ += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnSecondtop() {
	m_cVideo.fTGtGrnY /*= pRcp->fTgtGRnY[g->CurPaSet-1][0]*/ -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnSecondbottom() {
	m_cVideo.fTGtGrnY /*= pRcp->fTgtGRnY[g->CurPaSet-1][0]*/ += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnKillfocusSpeed() {
	CString str;
	int i = m_cSpeed.GetCurSel();
	if (i > -1) {
		i++;
		i *= i;
		dx = i * 0.001;
	}
}

void CSetupProp8Dlg::OnSetscreen2motor() {
	CRect rc;
	float xx1, xx2;

	if (AfxMessageBox("Are you sure?", MB_YESNO) != IDYES) {
		return;
	}

	m_cVideo.GetClientRect(&rc);

	//	int sourceWd = g->ImgWd;
	//	int sourceHt = g->ImgHt;

	switch (g->ActiveCamera) {
	default:
		xx1 = fabs(m_MLocX2 - m_MLocX1);
		xx2 = m_cVideo.ImgWd * fabs(g->fTgtRedECHO.X - m_cVideo.fTGtGrnX);
		g->ScrnCalibECH.dX = ECHdX = xx1 / xx2;
		g->ScrnCalibECH.ScrnX = m_cVideo.ImgWd;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	case CCamBase::THINFCAM:
		xx1 = fabs(m_MLocX2 - m_MLocX1);
		xx2 = m_cVideo.ImgWd * fabs(g->fTgtRedTF.X - m_cVideo.fTGtGrnX);
		g->ScrnCalibTF.dX = TFdX = xx1 / xx2;
		g->ScrnCalibTF.ScrnY = m_cVideo.ImgWd;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	}
}

void CSetupProp8Dlg::OnSetscreen2mmotor2() {
	CRect rc;
	float yy1, yy2;

	if (AfxMessageBox("Are you sure?", MB_YESNO) != IDYES) {
		return;
	}

	m_cVideo.GetClientRect(&rc);

	//	int sourceWd = g->ImgWd;
	//	int sourceHt = g->ImgHt;

	switch (g->ActiveCamera) {
	default:
		yy1 = fabs(m_MLocY2 - m_MLocY1);
		yy2 = m_cVideo.ImgHt * fabs(g->fTgtRedECHO.Y - m_cVideo.fTGtGrnY);
		g->ScrnCalibECH.dY = ECHdY = yy1 / yy2;
		g->ScrnCalibECH.ScrnY = m_cVideo.ImgHt;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	case CCamBase::THINFCAM:
		yy1 = fabs(m_MLocY2 - m_MLocY1);
		yy2 = m_cVideo.ImgHt * fabs(g->fTgtRedTF.Y - m_cVideo.fTGtGrnY);
		g->ScrnCalibTF.dY = TFdY = yy1 / yy2;
		g->ScrnCalibTF.ScrnY = m_cVideo.ImgHt;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	}
}

void CSetupProp8Dlg::OnSetpixel2micron() {
	// Calculate conversion factor, mm per pixel [6/2/2011 XPMUser]
	float xx;
	CRect rc;

	if (AfxMessageBox("Are you sure?", MB_YESNO) != IDYES) {
		return;
	}
	m_cVideo.GetClientRect(&rc);

	//	int sourceWd = g->ImgWd;
	//	int sourceHt = g->ImgHt;

	switch (g->ActiveCamera) {
	default:
		UpdateData(TRUE);
		if (m_ASizeEcho == 0) {
			return;
		}
		xx = m_cVideo.ImgWd * fabs(g->fTgtRedECHO.X - m_cVideo.fTGtGrnX);
		ECHmX = xx / m_ASizeEcho;	// unit: pix per um [4/15/2013 Administrator]
		g->ScrnCalibECH.ppmX = ECHmX;
		g->ScrnCalibECH.ScrnX = m_cVideo.ImgWd;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	case CCamBase::THINFCAM:
		UpdateData(TRUE);
		if (m_ASizeThinFilm == 0) {
			return;
		}
		xx = m_cVideo.ImgWd * fabs(g->fTgtRedTF.X - m_cVideo.fTGtGrnX);
		TFmX = xx / m_ASizeThinFilm;	// unit: pix per um [4/15/2013 Administrator]
		g->ScrnCalibTF.ppmX = TFmX;
		g->ScrnCalibTF.ScrnX = m_cVideo.ImgWd;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	}
}

void CSetupProp8Dlg::OnMarkred() {
	if (p413App->pMtr) p413App->pMtr->GetXYPos(&m_MLocX1, &m_MLocY1);
	UpdateData(FALSE);
}

void CSetupProp8Dlg::OnMarkgreen() {
	if (p413App->pMtr) p413App->pMtr->GetXYPos(&m_MLocX2, &m_MLocY2);
	UpdateData(FALSE);
}

void CSetupProp8Dlg::OnSetpixel2microny() {
	// Calculate conversion factor, mm per pixel [6/2/2011 XPMUser]
	float yy;
	CRect rc;

	if (AfxMessageBox("Are you sure?", MB_YESNO) != IDYES) {
		return;
	}

	m_cVideo.GetClientRect(&rc);
	m_cVideo.SnapOneFrame();

	//	int sourceWd = g->ImgWd;
	//	int sourceHt = g->ImgHt;

	switch (g->ActiveCamera) {
	default:
		UpdateData(TRUE);
		if (m_ASizeEcho == 0) {
			return;
		}
		yy = m_cVideo.ImgHt * fabs(g->fTgtRedECHO.Y - m_cVideo.fTGtGrnY);
		ECHmY = yy / m_ASizeEcho;
		g->ScrnCalibECH.ppmY = ECHmY;
		g->ScrnCalibECH.ScrnY = m_cVideo.ImgHt;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	case CCamBase::THINFCAM:
		UpdateData(TRUE);
		if (m_ASizeThinFilm == 0) {
			return;
		}
		yy = m_cVideo.ImgHt * fabs(g->fTgtRedTF.Y - m_cVideo.fTGtGrnY);
		TFmY = yy / m_ASizeThinFilm;
		g->ScrnCalibTF.ppmY = TFmY;
		g->ScrnCalibTF.ScrnY = m_cVideo.ImgHt;
		g->SaveScrnCalibINI();
		UpdateData(FALSE);
		break;
	}
}

long CSetupProp8Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMessage.SetWindowText(MoMove(wP, lP));

	SIPackedDIB* pDIB = g->StoredDIB.pDIB;
	if (pDIB) {
		m_cMessage2.SetWindowText(MoMove2(wP, lP, pDIB->BmpInfo.biWidth, abs(pDIB->BmpInfo.biHeight)));
	}
	else if (m_cVideo.IsSimu()) {
		pDIB = &m_cVideo.SimuDIB;
		m_cMessage2.SetWindowText(MoMove2(wP, lP, pDIB->BmpInfo.biWidth, abs(pDIB->BmpInfo.biHeight)));
	}
	return 0;
}

long CSetupProp8Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp8Dlg::OnKillfocusOffsetx2() {
	UpdateData(TRUE);
	g->ScrnCalibECH.dX = ECHdX;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::OnKillfocusOffsety2() {
	UpdateData(TRUE);
	g->ScrnCalibECH.dY = ECHdY;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::OnKillfocusOffsetx() {
	UpdateData(TRUE);
	g->ScrnCalibTF.dX = TFdX;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::OnKillfocusOffsety() {
	UpdateData(TRUE);
	g->ScrnCalibTF.dY = TFdY;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::SetupImageFlip() {}

void CSetupProp8Dlg::OnCrosshair() {
	CCrossHair dlg;
	dlg.DoModal();
}

void CSetupProp8Dlg::OnGotozeroxy() {
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(0, 0);
	}
}

void CSetupProp8Dlg::OnTakepicture() {
	m_cVideo.FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d"));
	m_cVideo.FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S"), int(rand() / float(RAND_MAX) * 100));
	m_cVideo.bSaveDIB = TRUE;
	m_cVideo.bSaveVideo = TRUE;
	while (m_cVideo.bSaveDIB) {
		Sleep(80);
	}
}

void CSetupProp8Dlg::OnThirdtop() {
	m_cVideo.fTGtBluY /*= pRcp->fTgtBLuY[g->CurPaSet-1][0]*/ -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnThirdbottom() {
	m_cVideo.fTGtBluY /*= pRcp->fTgtBLuY[g->CurPaSet-1][0]*/ += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnThirdleft() {
	m_cVideo.fTGtBluX /*= pRcp->fTgtBLuX[g->CurPaSet-1][0]*/ -= dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnThirdright() {
	m_cVideo.fTGtBluX /*= pRcp->fTgtBLuX[g->CurPaSet-1][0]*/ += dx;
	//UpdateTargets(&m_cVideo);
}

void CSetupProp8Dlg::OnSetptmecho() {
	UpdateData(TRUE);
	g->ScrnCalibECH.dX = ECHdX;
	g->ScrnCalibECH.dY = ECHdY;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::OnSetptmtf() {
	UpdateData(TRUE);
	g->ScrnCalibTF.dX = TFdX;
	g->ScrnCalibTF.dY = TFdY;
	g->SaveScrnCalibINI();
}

void CSetupProp8Dlg::OnReversey() {
	if (AfxMessageBox("Are you sure ?", MB_YESNO) != IDYES) {
		return;
	}
	if (g->bImageFlipYECHO) {
		g->bImageFlipYECHO = FALSE;
		m_cVideo.bYFlip = g->bImageFlipYECHO;
	}
	else {
		g->bImageFlipYECHO = TRUE;
		m_cVideo.bYFlip = g->bImageFlipYECHO;
	}
	g->SaveVideoOption();
	RecipeToLocal();
}

void CSetupProp8Dlg::OnReverseytf() {
	if (AfxMessageBox("Are you sure ?", MB_YESNO) != IDYES) {
		return;
	}
	if (g->bImageFlipYTFILM) {
		g->bImageFlipYTFILM = FALSE;
		m_cVideo.bYFlip = g->bImageFlipYTFILM;
	}
	else {
		g->bImageFlipYTFILM = TRUE;
		m_cVideo.bYFlip = g->bImageFlipYTFILM;
	}
	g->SaveVideoOption();
	RecipeToLocal();
}