// Navigation.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "413App.h"
#include "MainFrm.h"
#include "NavigationPanel.h"
#include ".\navigationpanel.h"

#include "..\SYS\CUMMsg.h"
#include "..\ONLINE\MFC\getpw.h"
#include "..\SRC\LogInDataNew.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "TitlePanel.h"
#include "ProcJob413ProcessingDlg.h"

// CNavigation

IMPLEMENT_DYNCREATE(CNavigationPanel, CResizableFormView)

CNavigationPanel::CNavigationPanel()
	: CResizableFormView(CNavigationPanel::IDD) {
	//{{AFX_DATA_INIT(CNavigationPanel)
	//}}AFX_DATA_INIT

	if (!pUDev) {
		ASSERT(0);
		return;
	}
	pUDev->pNavigationPanel = this;
	ActiveButton = acNONE;
}

CNavigationPanel::~CNavigationPanel() {
	if (pUDev) {
		pUDev->pNavigationPanel = NULL;
	}
}

void CNavigationPanel::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNavigationPanel)
	DDX_Control(pDX, IDC_EQPMESSAGE, m_cEqpMessage);
	DDX_Control(pDX, IDC_HOSTMESSAGE, m_cHostMessage);
	DDX_Control(pDX, IDC_VIEW, m_cView);
	DDX_Control(pDX, IDC_SYSTEM, m_cSystem);
	DDX_Control(pDX, IDC_SETUP, m_cSetup);
	DDX_Control(pDX, IDC_RECIPES, m_cRecipes);
	DDX_Control(pDX, IDC_NAVIHELP, m_cNaviHelp);
	DDX_Control(pDX, IDC_LOGS, m_cLogs);
	DDX_Control(pDX, IDC_JOBS, m_cJobs);
	DDX_Control(pDX, IDC_ALARMS, m_cAlarms);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNavigationPanel, CResizableFormView)
	//{{AFX_MSG_MAP(CNavigationPanel)
	ON_BN_CLICKED(IDC_SETUP, OnBnClickedSetup)
	ON_BN_CLICKED(IDC_JOBS, OnBnClickedJobs)
	ON_BN_CLICKED(IDC_SYSTEM, OnBnClickedSystem)
	ON_BN_CLICKED(IDC_RECIPES, OnBnClickedRecipes)
	ON_BN_CLICKED(IDC_LOGS, OnBnClickedLogs)
	ON_BN_CLICKED(IDC_ALARMS, OnBnClickedAlarms)
	ON_BN_CLICKED(IDC_NAVIHELP, OnBnClickedHelps)
	ON_BN_CLICKED(IDC_VIEW, OnBnClickedView)
	ON_COMMAND(ID_E95_BLINKALARMBUTTON, OnE95Blinkalarmbutton)
	ON_COMMAND(ID_E95_BLINKJOBBUTTON, OnE95Blinkjobbutton)
	ON_COMMAND(ID_E95_STOPBLINKJOBBUTTON, OnE95StopBlinkjobbutton)
	ON_BN_CLICKED(IDC_HOSTMESSAGE, OnHostmessage)
	ON_COMMAND(ID_E95_BLINKHOSTMESSAGE, OnE95Blinkhostmessage)
	ON_BN_CLICKED(IDC_EQPMESSAGE, OnEqpMessage)
	ON_COMMAND(ID_E95_BLINKEQPMESSAGE, OnE95Blinkeqpmessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CNavigation diagnostics

#ifdef _DEBUG
void CNavigationPanel::AssertValid() const {
	CFormView::AssertValid();
}

void CNavigationPanel::Dump(CDumpContext& dc) const {
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CNavigation message handlers

void CNavigationPanel::OnInitialUpdate() {
	AddResizedControl(IDC_NAVIHELP, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_ALARMS, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_EQPMESSAGE, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_HOSTMESSAGE, TOP_RIGHT, TOP_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();

	ResizeParentToFit();

	bWnd = TRUE;
	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	short off = -16;
	m_cAlarms.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cEqpMessage.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cHostMessage.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cJobs.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cLogs.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cNaviHelp.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cRecipes.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cSystem.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cSetup.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);
	m_cView.OffsetColor(CButtonST1::BTNST_COLOR_BK_FOCUS, off);

	// 	pUDev->pNavigationPanel = this;
}

void CNavigationPanel::OnBnClickedJobs() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (p413App->bUIBlocked) {
		return;
	}
	if (ActiveButton == JOBS) {
		return;
	}
	ActiveButton = JOBS;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(9);
}

void CNavigationPanel::OnBnClickedSystem() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (p413App->bUIBlocked) {
		return;
	}
	if (ActiveButton == SYSTEM) {
		return;
	}
	if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
		ActiveButton = SYSTEM;
		((CMainFrame*)AfxGetMainWnd())->SwitchPane(8);
	}
	else {
		pUDev->pTitlePanel->AskLogin();
		if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
			ActiveButton = SYSTEM;
			((CMainFrame*)AfxGetMainWnd())->SwitchPane(8);
		}
	}
}

void CNavigationPanel::OnBnClickedRecipes() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (p413App->bUIBlocked) {
		return;
	}
	if (ActiveButton == RECIPES) {
		return;
	}
	if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
		ActiveButton = RECIPES;
		((CMainFrame*)AfxGetMainWnd())->SwitchPane(7);
	}
	else {
		pUDev->pTitlePanel->AskLogin();
		if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
			ActiveButton = RECIPES;
			((CMainFrame*)AfxGetMainWnd())->SwitchPane(7);
		}
	}
}

void CNavigationPanel::OnBnClickedSetup() {
	p413App->Global.setDiceDisplay = false;//10122024
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == SETUP) {
		// Prevent multiple trigger [7/23/2013 Yuen]
		return;
	}
	if (p413App->IsOperating()) {
		AfxMessageBox("Not allow when a measurement is in progress");
		return;
	}
	if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::CREATERCP) {
		ActiveButton = SETUP;
		((CMainFrame*)AfxGetMainWnd())->SwitchPane(6);
	}
	else {
		pUDev->pTitlePanel->AskLogin();
		if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::CREATERCP) {
			ActiveButton = SETUP;
			((CMainFrame*)AfxGetMainWnd())->SwitchPane(6);
		}
	}
}

void CNavigationPanel::OnBnClickedLogs() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == LOGS) {
		return;
	}
	ActiveButton = LOGS;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(5);
}

void CNavigationPanel::OnBnClickedView() {
	p413App->Global.setDiceDisplay = true;//10122024
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == VIEW) {
		return;
	}
	ActiveButton = VIEW;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(4);
}

void CNavigationPanel::OnEqpMessage() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == EQPMESSAGE) {
		return;
	}
	ActiveButton = EQPMESSAGE;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(3);
}

void CNavigationPanel::OnHostmessage() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == HOSTMESSAGE) {
		return;
	}
	ActiveButton = HOSTMESSAGE;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(2);
}

void CNavigationPanel::OnBnClickedAlarms() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == ALARMS) {
		return;
	}
	ActiveButton = ALARMS;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(1);
}

void CNavigationPanel::OnBnClickedHelps() {
	if (!p413App->bUIEnabled) {
		return;
	}
	if (ActiveButton == HELPS) {
		return;
	}
	ActiveButton = HELPS;
	((CMainFrame*)AfxGetMainWnd())->SwitchPane(0);
}

void CNavigationPanel::OnE95Blinkalarmbutton() {
	if (ActiveButton != ALARMS) {
		m_cAlarms.SetBlink(TRUE);
	}
}

void CNavigationPanel::OnE95Blinkjobbutton() {
	if (ActiveButton != JOBS) {
		m_cJobs.SetBlink(TRUE);
	}
}

void CNavigationPanel::OnE95StopBlinkjobbutton() {
	if (ActiveButton != JOBS) {
		m_cJobs.SetBlink(FALSE);
	}
}

void CNavigationPanel::OnE95Blinkhostmessage() {
	if (ActiveButton != HOSTMESSAGE) {
		m_cHostMessage.SetBlink(TRUE);
	}
}

void CNavigationPanel::OnE95Blinkeqpmessage() {
	if (ActiveButton != EQPMESSAGE) {
		m_cEqpMessage.SetBlink(TRUE);
	}
}