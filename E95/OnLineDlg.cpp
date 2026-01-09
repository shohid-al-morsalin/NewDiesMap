// OnLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "XTabCtrl.h"
#include "OnLineDlg.h"
#include "OnlineCJDlg.h"
#include "OnlinePJDlg.h"
#include "OnlineLogsDlg.h"
#include "OnlineModeDlg.h"
#include "OnlineSVIDDlg.h"
#include "OnlineEventsDlg.h"
#include "OnlineCEIDDlg.h"
#include "OnlineMgr.h"
#include "OnlineAlarmDlg.h"
#include "OnlineECIDDlg.h"
#include "OnlineSubstrateDlg.h"

#include "..\ONLINE\TCPIP\monitor.h"
#include "..\ONLINE\TCPIP\ComLog.h"
#include "..\ONLINE\INC\Event.h"
#include "..\ONLINE\ShareMemory\ShareMemory.h"

#include "..\ONLINE\MFC\SpoolMan.h"

#include "..\ONLINE\MFC\establishcomm.h"

#include "..\ONLINE\GEM\PRJob.h"
#include "..\ONLINE\GEM\PRJobMgr.h"
#include "..\ONLINE\GEM\CtrlJobMgr.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "..\Sys\Var.h"

// COnLineDlg dialog

BOOL bDisabled = FALSE;
#define PASSIVE		0
#define ACTIVE		1
#define IDT_TIMER1	(100)

CRect   m_gRect;
BOOL    m_COMMODE = FALSE;

CShareMemory    m_ShareMemory;

//IMPLEMENT_DYNAMIC(COnLineDlg, CResizableDialog)

COnLineDlg::COnLineDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnLineDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnLineDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Mode = NULL;
	m_SVIDDlg = NULL;
	m_EventDlg = NULL;
	m_CJDlg = NULL;
	m_PJDlg = NULL;
	m_LogsDlg = NULL;
	m_ECIDDlg = NULL;
	m_AlarmDlg = NULL;
	m_SubstDlg = NULL;
}

COnLineDlg::~COnLineDlg() {
	//	ClearAll();
}

void COnLineDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnLineDlg)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnLineDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnLineDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_COMMAND(ID_MSG_ESTCOMM, OnMsgEstcomm)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnLineDlg message handlers

BOOL COnLineDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	// Create the rest if Online system can be started
	int nTab = 0;

	m_Mode = new COnlineModeDlg;
	if (m_Mode) {
		m_Mode->Create(COnlineModeDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_Mode, "Mode", nTab++);
	}

	m_EventDlg = new COnlineEventsDlg;
	if (m_EventDlg) {
		m_EventDlg->Create(COnlineEventsDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_EventDlg, "CEIDs", nTab++);
	}

	m_SVIDDlg = new COnlineSVIDDlg;
	if (m_SVIDDlg) {
		m_SVIDDlg->Create(COnlineSVIDDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_SVIDDlg, "SVIDs", nTab++);
	}

	m_ECIDDlg = new COnlineECIDDlg;
	if (m_ECIDDlg) {
		m_ECIDDlg->Create(COnlineECIDDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_ECIDDlg, "ECIDs", nTab++);
	}

	m_CJDlg = new COnlineCJDlg;
	if (m_CJDlg) {
		m_CJDlg->Create(COnlineCJDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_CJDlg, "Control Jobs", nTab++);
	}

	m_PJDlg = new COnlinePJDlg;
	if (m_PJDlg) {
		m_PJDlg->Create(COnlinePJDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_PJDlg, "Process Jobs", nTab++);
	}

	m_AlarmDlg = new COnlineAlarmDlg;
	if (m_AlarmDlg) {
		m_AlarmDlg->Create(COnlineAlarmDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_AlarmDlg, "Alarm IDs", nTab++);
	}

	//m_SubstDlg = new COnlineSubstrateDlg;
	if (m_SubstDlg) {
		m_SubstDlg->Create(COnlineSubstrateDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_SubstDlg, "Substrates", nTab++);
	}

	if (pGDev->pOLnMgr) {
		pGDev->pOLnMgr->Create(COnlineMgr::IDD, &m_tabctrl);
		m_tabctrl.AddTab(pGDev->pOLnMgr, "Online Logs", nTab++);
	}

	// Temporary disabled
	//	m_LogsDlg = new COnlineLogsDlg;
	if (m_LogsDlg) {
		m_LogsDlg->Create(COnlineLogsDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_LogsDlg, "Logs", nTab++);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnLineDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

LONG COnLineDlg::UpdateAlarm(WPARAM wParam, LPARAM lParam) {
	CEIDCtlr.m_alarm.GetCount(); //  [9/16/2007 LYF]
	return wParam;
}

LONG COnLineDlg::OnChangeMode(WPARAM wParam, LPARAM lParam) {
	if ((int)wParam == 1) {
		CString str;
		switch (lParam) {
		case EqpStatus::OFFLINEHOST:
			str = "Offline Host";
			break;
		case EqpStatus::OFFLINEEQP:
			str = "Offline Eqp";
			break;
		case EqpStatus::REMOTE:str = "Online Remote";
			break;
		case EqpStatus::LOCAL:str = "Online Local";
			break;
		default:
			return FALSE;
		}
	}
	return TRUE;
}

void COnLineDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "COnLineDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

LONG COnLineDlg::OnHostMsg(WPARAM wParam, LPARAM lParam) {
	int nMsgLen = wParam;
	return wParam;
}

LRESULT COnLineDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("System\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void COnLineDlg::OnMsgEstcomm() {
	pGDev->pOLnMgr->EstablishConnection();
}

void COnLineDlg::ClearAll() {
	if (m_Mode) {
		m_Mode->DestroyWindow();
		m_Mode = NULL;
	}
	if (m_SVIDDlg) {
		m_SVIDDlg->DestroyWindow();
		m_SVIDDlg = NULL;
	}
	if (m_EventDlg) {
		m_EventDlg->DestroyWindow();
		m_EventDlg = NULL;
	}
	if (m_CJDlg) {
		m_CJDlg->DestroyWindow();
		m_CJDlg = NULL;
	}
	if (m_PJDlg) {
		m_PJDlg->DestroyWindow();
		m_PJDlg = NULL;
	}
	if (m_LogsDlg) {
		m_LogsDlg->DestroyWindow();
		m_LogsDlg = NULL;
	}
	if (m_AlarmDlg) {
		m_AlarmDlg->DestroyWindow();
		m_AlarmDlg = NULL;
	}
	if (m_ECIDDlg) {
		m_ECIDDlg->DestroyWindow();
		m_ECIDDlg = NULL;
	}
	if (m_SubstDlg) {
		m_SubstDlg->DestroyWindow();
		m_SubstDlg = NULL;
	}
}

void COnLineDlg::OnDestroy() {
	ClearAll();

	CResizableDialog::OnDestroy();
}

void COnLineDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}