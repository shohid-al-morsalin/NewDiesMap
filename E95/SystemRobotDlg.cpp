// SystemRobotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "FEM/Rbt/RbtBase.h"
#include "FEM/Rbt/RbtMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelSystem.h"
#include "ONLINE/MFC/getpw.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "TitlePanel.h"
#include "XTabCtrl.h"

#include "SystemRobotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemRobotDlg dialog

#define IDT_REFRESH		9999

CSystemRobotDlg::CSystemRobotDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSystemRobotDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSystemRobotDlg)
	m_Service = _T("");
	//}}AFX_DATA_INIT

	m_Parent = NULL;

	if (!pGDev || !pGDev->pGEMObjs) {
		ASSERT(0);
	}
}

void CSystemRobotDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemRobotDlg)
	DDX_Control(pDX, IDC_HOME, m_cHome);
	DDX_Control(pDX, IDC_RECOVERY3, m_cRecovery3);
	DDX_Control(pDX, IDC_SERVICE, m_cService);
	DDX_Text(pDX, IDC_SERVICESTATUS, m_Service);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSystemRobotDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSystemRobotDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_BN_CLICKED(IDC_SERVICE, OnService)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RECOVERY, OnRecovery)
	ON_BN_CLICKED(IDC_RECOVERY2, OnRecovery2)
	ON_BN_CLICKED(IDC_HOME, OnHome)
	ON_BN_CLICKED(IDC_RECOVERY3, OnRecovery3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemRobotDlg message handlers

LRESULT CSystemRobotDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("System\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	return 0;
}

void CSystemRobotDlg::OnService() {
	CRbtObj* p = pGDev->pGEMObjs->StMd.GetRbtObj();
	if (p->Service == CRbtObj::INSERVICE) {
		// 		if (pGDev->AuthenticateUser())
		// 		{
		p->Service = CRbtObj::OUTOFSERVICE;
		m_cService.SetWindowText("In Service");
		// 		}
	}
	else {
		p->Service = CRbtObj::INSERVICE;
		m_cService.SetWindowText("Out of Service");
	}
	pGDev->pGEMObjs->StMd.SetRbtServ(p->Service);
	m_Service = pGDev->pGEMObjs->StMd.GetRbtServStr();
	UpdateData(FALSE);
}

BOOL CSystemRobotDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_SERVICE, TOP_RIGHT);
	AddAnchor(IDC_HOME, TOP_RIGHT);
	AddAnchor(IDC_RECOVERY, BOTTOM_RIGHT);
	AddAnchor(IDC_RECOVERY2, BOTTOM_RIGHT);
	AddAnchor(IDC_RECOVERY3, BOTTOM_RIGHT);

	Refresh();

	if (Var.UIMode > 2) {
		m_cRecovery3.ShowWindow(SW_SHOW);
	}

	SetTimer(IDT_REFRESH, 3000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemRobotDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		Refresh();
		SetTimer(IDT_REFRESH, 2000, NULL);
	}
}

void CSystemRobotDlg::Refresh() {
	CRbtObj* p = pGDev->pGEMObjs->StMd.GetRbtObj();
	if (p->Service == CRbtObj::INSERVICE) {
		m_cService.SetWindowText("Out of Service");
	}
	else {
		m_cService.SetWindowText("In Service");
	}

	m_Service = pGDev->pGEMObjs->StMd.GetRbtServStr();
	UpdateData(FALSE);
}

// LYF 128 module recovery [4/30/2006]
void CSystemRobotDlg::OnRecovery() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	if (pGDev->AramsE95.GetCurrentState() == SArams::PRODUCTIVE) {
		// continue is impossible
		if (AfxMessageBox("System is in PROCESSING state\nClick YES to proceed only if you are sure of your action", MB_YESNO) != IDYES) {
			return;
		}
	}
	if (m_Parent) {
		ASSERT(0); // Need further action [10/10/2012 Yuen]
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_WAFERRECOVERY, 1, 0);
	}
}

void CSystemRobotDlg::OnRecovery2() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	if (pGDev->AramsE95.GetCurrentState() == SArams::PRODUCTIVE) {
		AfxMessageBox("System is processing\nTry again when system is idle");
		return;
	}
	if (m_Parent) {
		ASSERT(0); // Need further action [10/10/2012 Yuen]
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_WAFERRECOVERY, 2, 0);
	}
}

void CSystemRobotDlg::OnHome() {
	m_cHome.EnableWindow(FALSE);
	if (pGDev->pRbtMgr && pGDev->pRbtMgr->pRbtSvr) {
		if (pGDev->pRbtMgr->pRbtSvr->HomeRobot(1)) {
			m_cHome.EnableWindow(TRUE);
			return;
		}
	}
	m_cHome.EnableWindow(TRUE);
}

void CSystemRobotDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSystemRobotDlg::OnRecovery3() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	if (pGDev->AramsE95.GetCurrentState() == SArams::PRODUCTIVE) {
		// continue is impossible
		if (AfxMessageBox("System is in PROCESSING state\nClick YES to proceed only if you are sure of your action", MB_YESNO) != IDYES) {
			return;
		}
	}
	if (m_Parent) {
		// 		ASSERT(0); // Need further action [10/10/2012 Yuen]
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_WAFERRECOVERY, 3, 0);	// 413 recovery [1/1/2013 Administrator]
	}
}