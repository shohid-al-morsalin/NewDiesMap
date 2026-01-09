// SetupProp11Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "413App.h"
#include "XTabCtrl.h"
#include "SetupProp11Dlg.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "..\413\MotorS2.h"
#include "..\SYS\CUMMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp11Dlg dialog

CSetupProp11Dlg::CSetupProp11Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp11Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp11Dlg)
	m_PosX1 = 0.0f;
	m_PosX2 = 0.0f;
	m_PosY1 = 0.0f;
	m_PosY2 = 0.0f;
	m_PostPos = 0;
	m_OffsetPos = 0;
	m_Angle = 0.0f;
	m_Result = _T("");
	//}}AFX_DATA_INIT

	m_cVideo.Name = "Setup11";
}

void CSetupProp11Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp11Dlg)
	DDX_Control(pDX, IDC_GOPOS2, m_cGoPos2);
	DDX_Control(pDX, IDC_GOPOS1, m_cGoPos1);
	DDX_Control(pDX, IDC_ALIGN, m_cAlgn);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Text(pDX, IDC_POSX1, m_PosX1);
	DDX_Text(pDX, IDC_POSX2, m_PosX2);
	DDX_Text(pDX, IDC_POSY1, m_PosY1);
	DDX_Text(pDX, IDC_POSY2, m_PosY2);
	DDX_Text(pDX, IDC_POSTPOS, m_PostPos);
	DDX_Text(pDX, IDC_OFFSETPOS, m_OffsetPos);
	DDX_Text(pDX, IDC_ANGLE, m_Angle);
	DDX_Text(pDX, IDC_CHGE_RBTXY, m_Result);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp11Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp11Dlg)
	ON_BN_CLICKED(IDC_CAMERASETTING5, OnCamerasetting5)
	ON_BN_CLICKED(IDC_MOTIONCONTROL5, OnMotioncontrol5)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_SETPOS1, OnSetpos1)
	ON_BN_CLICKED(IDC_SETPOS2, OnSetpos2)
	ON_BN_CLICKED(IDC_GOPOS1, OnGopos1)
	ON_BN_CLICKED(IDC_GOPOS2, OnGopos2)
	ON_BN_CLICKED(IDC_ALIGN, OnAlign)
	ON_BN_CLICKED(IDC_SETALIGN, OnSetalign)
	ON_BN_CLICKED(IDC_FINDANGLE, OnFindangle)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_FINDCENTR, OnFindcentr)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp11Dlg message handlers

long CSetupProp11Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		m_cVideo.pWnd = NULL;
		//m_cVideo.RSpecClear();
		p413App->StopCamera(&m_cVideo, NULL);
		bTabSelected = FALSE;
	}

	return 0;
}

long CSetupProp11Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
		m_pVideo = &m_cVideo;
		m_cVideo.SetRecipe(pRcp);
		//m_cVideo.SetAve(&Ave[0]);
		m_cVideo.pWnd = this;
		//m_cVideo.RSpecClear();
		//m_cVideo.bShowCrossHair = FALSE;
		bTabSelected = TRUE;
	}
	return 0;
}

void CSetupProp11Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp11Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_CAMERASETTING5, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL5, TOP_RIGHT);
	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_PostPos = DosUtil.GetAlignerPostPos(1); // LYF Convert to robot unit [1/14/2005]
	m_OffsetPos = DosUtil.GetAlignerOffsetPos(1); // LYF Convert to robot unit [1/14/2005]

	// [02052024 ZHIMING
	if (p413App->Global.Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
	}
	// ]

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSetupProp11Dlg::DestroyWindow() {
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp11Dlg::OnCamerasetting5() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp11Dlg::OnMotioncontrol5() {
	ShowMotionControl(this);
}

void CSetupProp11Dlg::OnCameraa() {
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

void CSetupProp11Dlg::OnCamerab() {
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

void CSetupProp11Dlg::OnSetpos1() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		pMtr->GetXYPos(&m_PosX1, &m_PosY1);
		UpdateData(FALSE);
	}
}

void CSetupProp11Dlg::OnSetpos2() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		pMtr->GetXYPos(&m_PosX2, &m_PosY2);
		UpdateData(FALSE);
	}
}

void CSetupProp11Dlg::OnGopos1() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		m_cGoPos1.EnableWindow(FALSE);
		pMtr->GotoXY(m_PosX1, m_PosY1);
		m_cGoPos1.EnableWindow(TRUE);
	}
}

void CSetupProp11Dlg::OnGopos2() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		m_cGoPos2.EnableWindow(FALSE);
		pMtr->GotoXY(m_PosX2, m_PosY2);
		m_cGoPos2.EnableWindow(TRUE);
	}
}

void CSetupProp11Dlg::OnAlign() {
	UpdateData(TRUE);
	m_cAlgn.EnableWindow(FALSE);

	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		if (pGDev->SetPostPos(1, m_PostPos, m_OffsetPos)) {
			if (p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
				pMtr->SetChuckVacuum(FALSE);
				if (pGDev->TransferWafer1(3, 1, 3, 1, TRUE)) {
					pMtr->SetChuckVacuum(TRUE);
				}
				else {
					pMtr->SetChuckVacuum(FALSE);
				}
			}
		}
	}
	m_cAlgn.EnableWindow(TRUE);
}

void CSetupProp11Dlg::OnSetalign() {
	if (AfxMessageBox("Are you sure ?", MB_YESNO) == IDYES) {
		UpdateData(TRUE);
		DosUtil.SetAlignerPostPos(1, m_PostPos);
		DosUtil.SetAlignerOffsetPos(1, m_OffsetPos);
	}
}

void CSetupProp11Dlg::OnFindangle() {
	float dx = m_PosX2 - m_PosX1;
	float dy = m_PosY2 - m_PosY1;

	if (dx == 0.0f) {
		dx = 1.0e-9f;	// a very small number [3/26/2013 FSMT]
	}
	m_Angle = atan((dy) / (dx)) * 180.0f / 3.141592653f;
	m_Angle *= 1000.0f;
	UpdateData(FALSE);
}

long CSetupProp11Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp11Dlg::OnSet1() {
	Pos1.Clear();
	p413App->pMtr->GetXYPos(&Pos1.X, &Pos1.Y);
}

void CSetupProp11Dlg::OnSet2() {
	Pos2.Clear();
	p413App->pMtr->GetXYPos(&Pos2.X, &Pos2.Y);
}

void CSetupProp11Dlg::OnSet3() {
	Pos3.Clear();
	p413App->pMtr->GetXYPos(&Pos3.X, &Pos3.Y);
}

void CSetupProp11Dlg::OnFindcentr() {
	sABC L1, L2;	// the two lines [6/5/2013 Yuen]
	p413App->FindABC(L1, Pos1.X, Pos1.Y, Pos2.X, Pos2.Y);
	p413App->FindABC(L2, Pos2.X, Pos2.Y, Pos3.X, Pos3.Y);
	sABC PL1, PL2;	// Perpendicular bisector of L1 and L2 [6/5/2013 Yuen]
	p413App->PerpendicularLine(PL1, L1, Pos1.X, Pos1.Y, Pos2.X, Pos2.Y);
	p413App->PerpendicularLine(PL2, L2, Pos2.X, Pos2.Y, Pos3.X, Pos3.Y);
	float Cx, Cy;
	p413App->Intersec(PL1, PL2, Cx, Cy);

	m_Result.Format("T:%.0f   R:%.0f um", 0 * Cx * 1000, Cy * 1000);
	UpdateData(FALSE);
}