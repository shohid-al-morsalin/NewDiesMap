// TitlePanel.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "TitlePanel.h"
#include "413App.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "..\src\LogInDataNew.h"

#define IDT_TIMETICK		9999
#define IDT_LOGOUT			9998
//#define IDT_UPDATELTWR		9998

// CTitlePanel

IMPLEMENT_DYNCREATE(CTitlePanel, CResizableFormView)

CTitlePanel::CTitlePanel()
	: CResizableFormView(CTitlePanel::IDD) {
	//{{AFX_DATA_INIT(CTitlePanel)
	m_DateTime = _T("");
	m_CommState = _T("");
	m_EquipState = _T("");
	m_Message = _T("");
	m_OnTime = _T("");
	//}}AFX_DATA_INIT
	if (pUDev) {
		pUDev->pTitlePanel = this;
	}
	AuthenticateLevel = AuthenLevelBak = CLogInDataNew::READDATA;
}

CTitlePanel::~CTitlePanel() {
	if (pUDev) {
		pUDev->pTitlePanel = NULL;
	}
}

void CTitlePanel::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTitlePanel)
	DDX_Control(pDX, IDC_BLUE, m_Blue);
	DDX_Control(pDX, IDC_AMBER, m_Amber);
	DDX_Control(pDX, IDC_GREEN, m_Green);
	DDX_Control(pDX, IDC_RED, m_Red);
	DDX_Control(pDX, IDC_VIEWNAME, m_cViewName);
	DDX_Text(pDX, IDC_DATETIME, m_DateTime);
	DDX_Text(pDX, IDC_COMMSTATUS, m_CommState);
	DDX_Text(pDX, IDC_STATUS, m_EquipState);
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_ONTIME, m_OnTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTitlePanel, CResizableFormView)
	//{{AFX_MSG_MAP(CTitlePanel)
	ON_BN_CLICKED(IDC_LOGIN, OnBnClickedLogin)
	ON_WM_TIMER()
	ON_MESSAGE(ID_LTW_SHOW, OnLtwShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CTitlePanel diagnostics

#ifdef _DEBUG
void CTitlePanel::AssertValid() const {
	CFormView::AssertValid();
}

void CTitlePanel::Dump(CDumpContext& dc) const {
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CTitlePanel message handlers

void CTitlePanel::OnInitialUpdate() {
	AddResizedControl(IDC_RED, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_AMBER, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_GREEN, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_BLUE, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_PARAMETERS, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_LOGIN, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_MESSAGE, TOP_LEFT, TOP_RIGHT);
	AddResizedControl(IDC_VIEWNAME, TOP_LEFT, TOP_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	bWnd = TRUE;
	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	m_Red.SetSkin(IDB_REDOFF, IDB_REDON, IDB_REDON, IDB_REDOFF, 0, IDB_MASK, 0, 0, 0);
	m_Green.SetSkin(IDB_GREENOFF, IDB_GREENON, IDB_GREENON, IDB_GREENOFF, 0, IDB_MASK, 0, 0, 0);
	m_Amber.SetSkin(IDB_AMBEROFF, IDB_AMBERON, IDB_AMBERON, IDB_AMBEROFF, 0, IDB_MASK, 0, 0, 0);
	m_Blue.SetSkin(IDB_BLUEOFF, IDB_BLUEON, IDB_BLUEON, IDB_BLUEOFF, 0, IDB_MASK, 0, 0, 0);
	m_Blue.SetState(TRUE);

	// 20230512 - Activate Login for all UIMode, no full access untill login
	/*if (Var.UIMode > 1) {
		AuthenticateLevel = CLogInDataNew::FULLACC;
	}*/
	SetTimer(IDT_TIMETICK, 1000, NULL);
}

void CTitlePanel::OnBnClickedLogin() {
	if (AuthenticateLevel == CLogInDataNew::READDATA) {
		if (pGDev->AuthenticateUser(AuthenticateLevel)) {
			if (Var.UIMode <= 1) {
				SetTimer(IDT_LOGOUT, 15 * 60000, NULL);
			}

			// 20230512 - LOG THE LOGIN USER INFORMATION
			if (AuthenticateLevel != AuthenLevelBak)
			{
				LogUserInfo(AuthenLevelBak, CLogInDataNew::LOGOUT);
				LogUserInfo(AuthenticateLevel, CLogInDataNew::LOGIN);
			}
		}
		else {
			AuthenticateLevel = AuthenLevelBak;
		}
	}
	else {
		LogUserInfo(AuthenticateLevel, CLogInDataNew::LOGOUT);	// 20230512
		AuthenticateLevel = CLogInDataNew::READDATA;
		KillTimer(IDT_LOGOUT);
		AfxMessageBox("You are logged out");
	}
}

void CTitlePanel::SetCurrentView(CString ViewName) {
	m_cViewName.SetWindowText(ViewName);
}

CString MtrReset[] = { "'Stage Not Reset'", "","" };
CString WOnStage[] = { "", "'Wafer Loaded'","" };
CString WCamera[] = { "'Camera:Echo'", "'Camera:TFilm'","","","" };

void CTitlePanel::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_TIMETICK) {
		C413Global* g = &p413App->Global;
		CTime time = CTime::GetCurrentTime();
		m_DateTime = time.Format("%y-%m-%d\n%H:%M:%S");
		m_OnTime.Format("%.1f", clock() / 3600000.0f);
		m_Message.Format("[%d %d] %s  %s  %s  Me:%d  Pa:%d  FFU: %.2f  Repeat:%d  %d  EM:%d  Vac:%d EL:%d SL:%d SS:%d SP:%d ",
			g->bAborting, g->bStop, MtrReset[g->bStageReset], WCamera[g->ActiveCamera], WOnStage[g->WaferOnStage == 1],
			g->CurMeSet, g->CurPaSet, g->FFUPressure, g->WaferRepeat, g->CassetteRepeat, p413App->GetDMCStatus(),
			g->bChuckVac, g->bEchoLight, g->bSpectLight, g->bSpecSource, g->bSpecPower
		);
		UpdateData(FALSE);
	}
	if (nIDEvent == IDT_LOGOUT) {
		KillTimer(IDT_LOGOUT);
		AuthenticateLevel = CLogInDataNew::READDATA;
	}
	//	if (nIDEvent == IDT_UPDATELTWR)
	//	{
	//		KillTimer(IDT_UPDATELTWR);
	//		UpdateLTower(LTstate);
	//	}
}

// state format: RGAB
void CTitlePanel::UpdateLTower(char* state) {
	if (!state) {
		ASSERT(0);
		return;
	}

	// Red
	switch (*state) {
	case 'R':
		m_Red.Blink(FALSE);
		m_Red.SetState(TRUE);
		break;
	case 'r':
		m_Red.Blink(FALSE);
		m_Red.SetState(FALSE);
		break;
	case 'E':
		m_Red.Blink(TRUE);
		break;
	case 'e':
		m_Red.Blink(FALSE);
		break;
	}

	// Green
	switch (*(state + 1)) {
	case 'G':
		m_Green.Blink(FALSE);
		m_Green.SetState(TRUE);
		break;
	case 'g':
		m_Green.Blink(FALSE);
		m_Green.SetState(FALSE);
		break;
	case 'N':
		m_Green.Blink(TRUE);
		break;
	case 'n':
		m_Green.Blink(FALSE);
		break;
	}

	// Amber
	switch (*(state + 2)) {
	case 'O':
		m_Amber.Blink(FALSE);
		m_Amber.SetState(TRUE);
		break;
	case 'o':
		m_Amber.Blink(FALSE);
		m_Amber.SetState(FALSE);
		break;
	case 'Y':
		m_Amber.Blink(TRUE);
		break;
	case 'y':
		m_Amber.Blink(FALSE);
		break;
	}

	// Blue
	switch (*(state + 3)) {
	case 'B':
		m_Blue.Blink(FALSE);
		m_Blue.SetState(TRUE);
		break;
	case 'b':
		m_Blue.Blink(FALSE);
		m_Blue.SetState(FALSE);
		break;
	case 'L':
		m_Blue.Blink(TRUE);
		break;
	case 'l':
		m_Blue.Blink(FALSE);
		break;
	}

	Invalidate(TRUE);
}

void CTitlePanel::SetCommState(CString state) {
	m_CommState = state;
}

void CTitlePanel::SetEquipmentState(CString state) {
	m_EquipState = state;
}

void CTitlePanel::SetMessage(CString msg) {
	m_Message = msg;
}

LRESULT CTitlePanel::OnLtwShow(WPARAM wP, LPARAM lP) {
	UpdateLTower((char*)lP);
	return 0;
}

void CTitlePanel::AskLogin() {
	AuthenLevelBak = AuthenticateLevel;
	AuthenticateLevel = CLogInDataNew::READDATA;
	OnBnClickedLogin();
}

////////////////////////////////////////////////
// 20230512 - Log the user details
////////////////////////////////////////////////
void CTitlePanel::LogUserInfo(int Authentication, int status)
{
	CString msg = "";
	if (Authentication == CLogInDataNew::RUNRCP)
	{
		msg = "Operator";
	}
	else if (Authentication == CLogInDataNew::CREATERCP)
	{
		msg = "Engineer";
	}
	else if (Authentication == CLogInDataNew::CHGSETTING)
	{
		msg = "Super Engineer";
	}

	if (msg != "")
	{
		if (status == CLogInDataNew::LOGIN)
		{
			msg = "LogIn: " + msg + " -> " + pGDev->username;
			GetDlgItem(IDC_LOGIN)->SetWindowTextA("Logout");
		}
		else if (status == CLogInDataNew::LOGOUT)
		{
			msg = "LogOut: " + msg;
			GetDlgItem(IDC_LOGIN)->SetWindowTextA("Login");
		}
		pGDev->LogFile.Log(msg, 5, "TitlePanel");
	}
}