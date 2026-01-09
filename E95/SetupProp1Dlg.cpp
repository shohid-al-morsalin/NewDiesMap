// SetupProp1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "413App.h"
#include "XTabCtrl.h"
#include "SetupProp1Dlg.h"
#include "..\SRC\DOSUtil.h"
#include "..\SYS\CUMMsg.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp1Dlg dialog

CSetupProp1Dlg::CSetupProp1Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp1Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp1Dlg)
	//}}AFX_DATA_INIT
	m_cVideo.Name = "Setup1";
}

void CSetupProp1Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp1Dlg)
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp1Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp1Dlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp1Dlg message handlers

LRESULT CSetupProp1Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		g = &p413App->Global;
		pRcp = &g->RcpSetup;

		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
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

long CSetupProp1Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
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

BOOL CSetupProp1Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp1Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp1Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp1Dlg::RecipeToLocal() {}

void CSetupProp1Dlg::LocalToRecipe() {}

long CSetupProp1Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

long CSetupProp1Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}