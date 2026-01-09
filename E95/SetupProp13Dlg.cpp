// SetupProp13Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "413App.h"
#include "XTabCtrl.h"
#include "..\SYS\CUMMsg.h"
#include "SetupProp13Dlg.h"
#include "GlobalUIContainer.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp13Dlg dialog

CSetupProp13Dlg::CSetupProp13Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp13Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp13Dlg)
	m_Result = _T("");
	m_THExposure = 0;
	//}}AFX_DATA_INIT

	bInner = TRUE;
	m_cVideo.Name = "Setup13";
}

void CSetupProp13Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp13Dlg)
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraa);
	DDX_Control(pDX, IDC_REPORT, m_cList);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_IMAGE, m_cImgWnd);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Text(pDX, IDC_RESULT, m_Result);
	DDX_Text(pDX, IDC_THExposure, m_THExposure);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp13Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp13Dlg)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL3, OnMotioncontrol3)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_EN_KILLFOCUS(IDC_THExposure, OnKillfocusTHExposure)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp13Dlg message handlers

long CSetupProp13Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
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

long CSetupProp13Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
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
			m_cVideo.LoadStockDIB("TRIM-IN\\IE67.5.BMP");
		}
		bTabSelected = TRUE;
	}

	return 0;
}

BOOL CSetupProp13Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp13Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSetupProp13Dlg::RecipeToLocal() {
	m_THExposure = pRcp->THExposure;
	UpdateData(FALSE);
}

void CSetupProp13Dlg::LocalToRecipe() {}

BOOL CSetupProp13Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_FIND, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL3, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cImgWnd.bScaled = TRUE;

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

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(bInner);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp13Dlg::OnCameraa() {
	m_cCameraa.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	p413App->GotoPositionecho();
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraa.EnableWindow(TRUE);
}

void CSetupProp13Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
	UpdateData(FALSE);
	g->Save();
}

void CSetupProp13Dlg::OnMotioncontrol3() {
	ShowMotionControl(this);
}

long CSetupProp13Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

long CSetupProp13Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

void CSetupProp13Dlg::OnClear() {
	m_cList.DeleteAllItems();
}

void CSetupProp13Dlg::OnFind() {
	float x, y, radius, angle;

	p413App->pMtr->GetXYPos(&x, &y);
	g->Cartesian2Polar(x, y, radius, angle);
	//radius = pRcp->TrimWidthInnerRadius;		// Set radius to outer radius [4/18/2013 Administrator]
	//g->Polar2Cartesian(radius,angle,x,y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
	p413App->pMtr->GotoXY(x, y);
	m_cVideo.SnapOneFrame(pRcp->THExposure);
	m_cImgWnd.DupStore1(g->StoredDIB);

	float dR = p413App->GetEdgeOffset(pRcp->TrimWidthLines, pRcp->TrimWidthSpacing, angle, bInner, &m_cImgWnd);

	m_Result.Format("Initial R= %.3f, dR= %.3f, Final R= %.3f", radius, dR, radius + dR);
	UpdateData(FALSE);
}

void CSetupProp13Dlg::OnRadio1() {
	bInner = TRUE;
}

void CSetupProp13Dlg::OnRadio2() {
	bInner = FALSE;
}

void CSetupProp13Dlg::OnKillfocusTHExposure() {
	UpdateData(TRUE);
	pRcp->THExposure = m_THExposure;
}