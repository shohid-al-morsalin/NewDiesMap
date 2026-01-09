// SetupProp41Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "SetupProp41Dlg.h"

#include "413App.h"
#include "XTabCtrl.h"

#include "..\SYS\CUMMsg.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp41Dlg dialog

CSetupProp41Dlg::CSetupProp41Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp41Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp41Dlg)
	m_MinimumStep = 0.0f;
	m_Range = 0.0f;
	m_XOff = 0;
	m_YOff = 0;
	m_Range2 = 0.0f;
	m_MinimunStep2 = 0.0f;
	//}}AFX_DATA_INIT
	m_cVideo.Name = "Setup41";
}

void CSetupProp41Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp41Dlg)
	DDX_Control(pDX, IDC_SEARCHAROUND, m_cSearch);
	DDX_Control(pDX, IDC_SNAP, m_cSnap);
	DDX_Control(pDX, IDC_FOCUS, m_cFocus);
	DDX_Control(pDX, IDC_EXPOSURE1, m_cExposure);
	DDX_Control(pDX, IDC_LIST, m_cList);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_IMAGEVIEW, m_cImgWnd1);
	DDX_Control(pDX, IDC_IMAGEVIEW2, m_cImgWnd2);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Text(pDX, IDC_MINIMUMSTEP, m_MinimumStep);
	DDX_Text(pDX, IDC_RANGE, m_Range);
	DDX_Text(pDX, IDC_XOFF, m_XOff);
	DDX_Text(pDX, IDC_YOFF, m_YOff);
	DDX_Text(pDX, IDC_RANGE2, m_Range2);
	DDX_Text(pDX, IDC_MINIMUMSTEP2, m_MinimunStep2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp41Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp41Dlg)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_FOCUS, OnFocus)
	ON_BN_CLICKED(IDC_SCORE, OnScore)
	ON_BN_CLICKED(IDC_CLEARLIST, OnClearlist)
	ON_EN_KILLFOCUS(IDC_MINIMUMSTEP, OnKillfocusMinimumstep)
	ON_EN_KILLFOCUS(IDC_RANGE, OnKillfocusRange)
	ON_EN_KILLFOCUS(IDC_XOFF, OnKillfocusXoff)
	ON_EN_KILLFOCUS(IDC_YOFF, OnKillfocusYoff)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL, OnMotioncontrol)
	ON_BN_CLICKED(IDC_EXPOSURE1, OnExposure)
	ON_BN_CLICKED(IDC_CONTRAST, OnContrast)
	ON_BN_CLICKED(IDC_GREY, OnGrey)
	ON_BN_CLICKED(IDC_CONTRAST2, OnContrast2)
	ON_BN_CLICKED(IDC_BINARIZE, OnBinarize)
	ON_BN_CLICKED(IDC_GAUSSIAN, OnGaussian)
	ON_BN_CLICKED(IDC_BINARIZEH, OnBinarizeh)
	ON_BN_CLICKED(IDC_BINARIZEL, OnBinarizel)
	ON_BN_CLICKED(IDC_CONTRAST4, OnContrast4)
	ON_BN_CLICKED(IDC_CONTRAST5, OnContrast5)
	ON_BN_CLICKED(IDC_SNAP, OnSnap)
	ON_BN_CLICKED(IDC_SEARCHAROUND, OnSearcharound)
	ON_EN_KILLFOCUS(IDC_RANGE2, OnKillfocusRange2)
	ON_EN_KILLFOCUS(IDC_MINIMUMSTEP2, OnKillfocusMinimumstep2)
	ON_BN_CLICKED(IDC_AVEHUE, OnAvehue)
	ON_BN_CLICKED(IDC_AVESAT, OnAvesat)
	ON_BN_CLICKED(IDC_DOUBLESNAP, OnDoublesnap)
	ON_BN_CLICKED(IDC_RESETRND1, OnResetrnd1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp41Dlg message handlers

long CSetupProp41Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
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
			m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
			m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][0];
		}

		RecipeToLocal();

		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp41Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}
		UpdateData(TRUE);

		bTabSelected = FALSE;
	}

	return 0;
}

BOOL CSetupProp41Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cVideo.XX = 5;
	RecipeToLocal();

	// [02052024 ZHIMING
	if (g->Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp41Dlg::StartCamera(CCamBase::CAMERAID No) {
	p413App->StartCamera(&m_cVideo, No, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cVideo.bShowCrossHair = FALSE;
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

void CSetupProp41Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	StartCamera(CCamBase::ECHOCAM);
	m_cVideo.pWnd = this;
	m_cVideo.bShowCrossHair = FALSE;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp41Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	StartCamera(CCamBase::THINFCAM);
	m_cVideo.pWnd = this;
	m_cVideo.bShowCrossHair = FALSE;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
}

void CSetupProp41Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp41Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp41Dlg::RecipeToLocal() {
	UpdateData(FALSE);
}

void CSetupProp41Dlg::OnScore() {
	ClearTextInfoAll();
	CString str;
	double scoreA, scoreB;
	if (m_cVideo.SnapOneFrame()) {
		scoreA = p413App->GetScore(g->StoredDIB.pDIB, DIB.pDIB);
		CPackedDIB SobDIB;
		p413App->StoreDib1(p413App->Global.StoredDIB, SobDIB, 256, 256, pRcp->fTgtGRnX[0][0], pRcp->fTgtGRnY[0][0], m_XOff, m_YOff);	// @@@ [11/3/2012 Yuen]
		SobDIB.Sobel();
		m_cImgWnd2.DupStore1(SobDIB);

		scoreB = SobDIB.GetIntensityCount();
		str.Format("Score: %.4f", scoreB);
		Message(str);
	}
}

void CSetupProp41Dlg::OnClearlist() {
	m_cList.ResetContent();
}

void CSetupProp41Dlg::OnKillfocusMinimumstep() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::OnKillfocusRange() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::OnKillfocusXoff() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::OnKillfocusYoff() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp41Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp41Dlg::OnExposure() {
	ClearTextInfoAll();
	int wSiz = g->MWdw1;
	g->MWdw1 = 200;
	DoExposure();
	g->MWdw1 = wSiz;
}

void CSetupProp41Dlg::OnFocus() {
	ClearTextInfoAll();
	int wSiz = g->MWdw1;
	g->MWdw1 = 128;
	DoFocus();
	g->MWdw1 = wSiz;
}

void CSetupProp41Dlg::DoExposure() {
	m_cExposure.EnableWindow(FALSE);
	short AutoExpose = pRcp->AutoExposeEcho;
	if (g->ActiveCamera == CCamBase::THINFCAM) {
		AutoExpose = pRcp->AutoExposeTF;
	}
	m_cVideo.Level = AutoExpose;
	m_cVideo.AutoAdjust();
	m_cExposure.EnableWindow(TRUE);
}

void CSetupProp41Dlg::DoFocus() {
	m_cFocus.EnableWindow(FALSE);
	p413App->AutoFocusVideo(g->ActiveCamera, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo, &m_cImgWnd2, &m_cList);
	m_cFocus.EnableWindow(TRUE);
}

void CSetupProp41Dlg::Message(CString str) {
	m_cList.InsertString(0, str);
	if (m_cList.GetCount() > 256) {
		for (int i = 0; i < 64; i++) {
			m_cList.DeleteString(m_cList.GetCount());
		}
	}
}

void CSetupProp41Dlg::OnContrast() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastBW();
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnGrey() {
	CString str;
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.GreyScale();
		m_cImgWnd2.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnContrast2() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastBW();
		m_cImgWnd2.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnBinarize() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastBW();
		g->StoredDIB.Binarize(128);
		m_cImgWnd2.DupStore1(g->StoredDIB);
	}
}

long CSetupProp41Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp41Dlg::OnGaussian() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.Contrast3Level();
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnBinarizeh() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastBW();
		g->StoredDIB.Binarize(224);
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnBinarizel() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastBW();
		g->StoredDIB.Binarize(32);
		m_cImgWnd2.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnContrast4() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastColor();
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnContrast5() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		g->StoredDIB.ContrastStretchColor(0, 0.25);
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

// Snap 1 pictures each at 22.5 deg angle with radius at 150mm [6/4/2013 Yuen]
void CSetupProp41Dlg::OnSnap() {
	float x, y;
	float ang = 0;
	float radius = 143.02f;
	CString tm, filedir, filebase;

	m_cSnap.EnableWindow(FALSE);
	tm = CTime::GetCurrentTime().Format("%H%M");
	filedir.Format("C:\\TEMP\\SNAP-%s-%s-%.2f\\", CTime::GetCurrentTime().Format("%m%d"), tm, radius);
	for (int i = 0; i < 16; i++) {
		p413App->Global.Polar2Cartesian(radius, ang, x, y);
		p413App->pMtr->GotoXY(x, y);
		m_cVideo.SnapOneFrame();
		filebase.Format("%.2f.BMP", ang);
		if (!p413App->Global.StoredDIB.Save(filedir + filebase)) {
			break;
		}
		ang += 22.5f;
	}
	m_cSnap.EnableWindow(TRUE);
}

void CSetupProp41Dlg::OnSearcharound() {
	float x, y;
	p413App->pMtr->GetXYPos(&x, &y);
	CString tm, filedir, filebase;

	m_cSearch.EnableWindow(FALSE);
	tm = CTime::GetCurrentTime().Format("%H%M");
	filedir.Format("C:\\TEMP\\SRCH-%s-%s-%.3f-%.3f\\", CTime::GetCurrentTime().Format("%m%d"), tm, x, y);
	for (int i = 0; i < pRcp->nTrialEcho; i++) {
		p413App->pMtr->GotoXY(x + gPX[i] * pRcp->fStepSizeEcho, y + gPY[i] * pRcp->fStepSizeEcho);
		if (m_cVideo.SnapOneFrame()) {
			filebase.Format("%.3f-%.3f.BMP", x + gPX[i] * pRcp->fStepSizeEcho, y + gPY[i] * pRcp->fStepSizeEcho);
			if (!p413App->Global.StoredDIB.Save(filedir + filebase)) {
				break;
			}
		}
	}
	m_cSearch.EnableWindow(TRUE);
}

void CSetupProp41Dlg::OnKillfocusRange2() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::OnKillfocusMinimumstep2() {
	UpdateData(TRUE);
}

void CSetupProp41Dlg::ClearTextInfoAll() {
	m_cImgWnd1.ClearTextInfo();
	m_cImgWnd2.ClearTextInfo();
}

void CSetupProp41Dlg::OnAvehue() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		CString str;
		str.Format("Average Hue: %.4f", g->StoredDIB.AverageHue());
		Message(str);
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnAvesat() {
	ClearTextInfoAll();
	if (m_cVideo.SnapOneFrame()) {
		CString str;
		str.Format("Average Saturation: %.4f", g->StoredDIB.AverageSat());
		Message(str);
		m_cImgWnd1.DupStore1(g->StoredDIB);
	}
}

void CSetupProp41Dlg::OnDoublesnap() {
	m_cVideo.DoubleSnapOneFrame();
	CString str;
	str.Format("SnapResult: %d (%d)", m_cVideo.SnapRsl, m_cVideo.SnapLimit);
	Message(str);
}

void CSetupProp41Dlg::OnResetrnd1() {
	p413App->rnd1 = 1;
}