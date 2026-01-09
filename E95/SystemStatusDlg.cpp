#include "stdafx.h"
#include "E95.h"

#include "128/GEM/OLnClt.h"
#include "413App.h"
#include "GEMEvt.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "LTW/LTWMgr.h"
#include "SRC/DOSUtil.h"
#include "TitlePanel.h"
#include "XTabCtrl.h"

#include "SystemStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_REFRESH		9999
#define IDT_SAVE		9998
#define IDT_SETLIGHT	9997

/////////////////////////////////////////////////////////////////////////////
// CSystemStatusDlg dialog

CSystemStatusDlg::CSystemStatusDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSystemStatusDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSystemStatusDlg)
	//}}AFX_DATA_INIT

// 	if (!pGDev)
// 	{
// 		ASSERT(0);
// 	}

	pGDev->RegisterSystemStatus(this);

	PrevState = SArams::NOSTATE;

	numLP = DosUtil.GetNumLP();
}

CSystemStatusDlg::~CSystemStatusDlg() {
	// 	if (pGDev)
	// 	{
	pGDev->RegisterSystemStatus(NULL);
	// 	}
}

void CSystemStatusDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemStatusDlg)
	DDX_Control(pDX, IDC_SUNSCHEDULEDDOWN, m_cUnscheduled);
	DDX_Control(pDX, IDC_SSTANDBY, m_cStandby);
	DDX_Control(pDX, IDC_SSCHEDULEDDOWN, m_cScheduleDown);
	DDX_Control(pDX, IDC_SPRODUCTIVE, m_cProductive);
	DDX_Control(pDX, IDC_SNONSCHEDULED, m_cNonSchedule);
	DDX_Control(pDX, IDC_SENGINEERING, m_cEngineering);
	DDX_Control(pDX, IDC_STATUS, m_cStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSystemStatusDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSystemStatusDlg)
	ON_BN_CLICKED(IDC_ARAMS_PRODUCTIVE, OnAramsProductive)
	ON_BN_CLICKED(IDC_ARAMS_ENGINEERING, OnAramsEngineering)
	ON_BN_CLICKED(IDC_ARAMS_NONSCHEDULED, OnAramsNonscheduled)
	ON_BN_CLICKED(IDC_ARAMS_SCHEDULEDDOWN, OnAramsScheduleddown)
	ON_BN_CLICKED(IDC_ARAMS_UNSCHEDULEDDOWN, OnAramsUnscheduleddown)
	ON_WM_TIMER()
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_BN_CLICKED(IDC_CLEARABORT, OnClearabort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemStatusDlg message handlers

void CSystemStatusDlg::OnAramsProductive() {
	SetProductive();
}

void CSystemStatusDlg::OnAramsEngineering() {
	SetEngineering();
}

void CSystemStatusDlg::OnAramsNonscheduled() {
	SetNonScheduled();
}

void CSystemStatusDlg::OnAramsScheduleddown() {
	SetScheduledDown();
}

void CSystemStatusDlg::OnAramsUnscheduleddown() {
	SetUnscheduledDown();
}

void CSystemStatusDlg::GetAramStatus() {
	CString str, s1;

	if (pGDev->AramsE95.Attr.CurrentState != PrevState) {
		s1 = pGDev->AramsE95.GetCurrentStateStr();
		s1.MakeUpper();
		str.Format("Equipment status: %s", s1);
		m_cStatus.SetWindowText(str);
		pUDev->pTitlePanel->SetEquipmentState(s1);

		((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetWindowText("Productive");
		((CButton*)GetDlgItem(IDC_ARAMS_ENGINEERING))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ARAMS_UNSCHEDULEDDOWN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ARAMS_SCHEDULEDDOWN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_ARAMS_NONSCHEDULED))->SetCheck(FALSE);
		pGDev->AramsE95.UpdateState();
		switch (pGDev->AramsE95.Attr.CurrentState) {
		case SArams::PRODUCTIVE:
			((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetCheck(TRUE);
			((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetWindowText("Productive\n(Productive)");
			break;
		case SArams::STANDBY:
			((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetCheck(TRUE);
			((CButton*)GetDlgItem(IDC_ARAMS_PRODUCTIVE))->SetWindowText("Productive\n(Standby)");
			break;
		case SArams::ENGINEERING:
			((CButton*)GetDlgItem(IDC_ARAMS_ENGINEERING))->SetCheck(TRUE);
			break;
		case SArams::UNSCHEDULEDDOWN:
			((CButton*)GetDlgItem(IDC_ARAMS_UNSCHEDULEDDOWN))->SetCheck(TRUE);
			break;
		case SArams::SCHEDULEDDOWN:
			((CButton*)GetDlgItem(IDC_ARAMS_SCHEDULEDDOWN))->SetCheck(TRUE);
			break;
		case SArams::NONSCHEDULED:
			((CButton*)GetDlgItem(IDC_ARAMS_NONSCHEDULED))->SetCheck(TRUE);
			break;
		}
		PrevState = pGDev->AramsE95.Attr.CurrentState;
	}
	pGDev->AramsE95.UpdateState();

	float dayconv = 1 / (60.0f * 60.0f * 24.0f);
	str.Format("Productive time: %.5f", pGDev->AramsE95.Attr.Productive * dayconv);
	m_cProductive.SetWindowText(str);
	str.Format("Standby time: %.5f", pGDev->AramsE95.Attr.Standby * dayconv);
	m_cStandby.SetWindowText(str);
	str.Format("Scheduled down time: %.5f", pGDev->AramsE95.Attr.ScheduledDown * dayconv);
	m_cScheduleDown.SetWindowText(str);
	str.Format("Unscheduled down time: %.5f", pGDev->AramsE95.Attr.UnscheduledDown * dayconv);
	m_cUnscheduled.SetWindowText(str);
	str.Format("Engineering time: %.5f", pGDev->AramsE95.Attr.Engineering * dayconv);
	m_cEngineering.SetWindowText(str);
	str.Format("Non scheduled time: %.5f", pGDev->AramsE95.Attr.NonScheduled * dayconv);
	m_cNonSchedule.SetWindowText(str);
}

void CSystemStatusDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		GetAramStatus();
		SetTimer(IDT_REFRESH, 1000, NULL);
	}
	if (nIDEvent == IDT_SAVE) {
		pGDev->AramsE95.Save("Aram413.bin");
	}
	if (nIDEvent == IDT_SETLIGHT) {
		KillTimer(IDT_SETLIGHT);
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
		else {
			SetTimer(IDT_SETLIGHT, 1000, NULL);
		}
	}
}

BOOL CSystemStatusDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_ARAMS_PRODUCTIVE, TOP_RIGHT);
	AddAnchor(IDC_ARAMS_ENGINEERING, TOP_RIGHT);
	AddAnchor(IDC_ARAMS_UNSCHEDULEDDOWN, TOP_RIGHT);
	AddAnchor(IDC_ARAMS_SCHEDULEDDOWN, TOP_RIGHT);
	AddAnchor(IDC_ARAMS_NONSCHEDULED, TOP_RIGHT);

	GetAramStatus();
	SetTimer(IDT_SETLIGHT, 2000, NULL);
	SetTimer(IDT_REFRESH, 1000, NULL);	// Refresh every 3 seconds
	SetTimer(IDT_SAVE, 10000, NULL);	// Save status every 10 seconds

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSystemStatusDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (pUDev) {
		CString msg;
		msg.Format("System\n%s", (char*)lP);
		pUDev->SetCurrentView(msg);
	}
	return 0;
}

void CSystemStatusDlg::SetProductive() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::PRODUCTIVE) {
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		if (p413App->IsOperating()) {
			pGDev->AramsE95.SetCurrentState(SArams::PRODUCTIVE);
			Ge.reason = CGEMEvt::EQPWORKING;  //  [2/7/2011 C2C]
		}
		else {
			pGDev->AramsE95.SetCurrentState(SArams::STANDBY);
			Ge.reason = CGEMEvt::EQPIDLE;
		}
		pGDev->pOLnClt->GEMEvent(Ge);

		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

void CSystemStatusDlg::SetEngineering() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::ENGINEERING) {
		pGDev->AramsE95.SetCurrentState(SArams::ENGINEERING);
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		Ge.reason = CGEMEvt::EQPMAINTANANCE;
		pGDev->pOLnClt->GEMEvent(Ge);
		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

void CSystemStatusDlg::SetNonScheduled() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::NONSCHEDULED) {
		pGDev->AramsE95.SetCurrentState(SArams::NONSCHEDULED);
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		Ge.reason = CGEMEvt::EQPIDLE;
		pGDev->pOLnClt->GEMEvent(Ge);
		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

void CSystemStatusDlg::SetScheduledDown() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::SCHEDULEDDOWN) {
		pGDev->AramsE95.SetCurrentState(SArams::SCHEDULEDDOWN);
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		Ge.reason = CGEMEvt::EQPMAINTANANCE;
		pGDev->pOLnClt->GEMEvent(Ge);
		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

void CSystemStatusDlg::SetUnscheduledDown() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::UNSCHEDULEDDOWN) {
		pGDev->AramsE95.SetCurrentState(SArams::UNSCHEDULEDDOWN);
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		Ge.reason = CGEMEvt::EQPMAINTANANCE;
		pGDev->pOLnClt->GEMEvent(Ge);
		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

void CSystemStatusDlg::SetStandby() {
	if (pGDev->AramsE95.GetCurrentState() != SArams::STANDBY) {
		pGDev->AramsE95.SetCurrentState(SArams::STANDBY);
		CGEMEvt Ge;
		Ge.act = CGEMEvt::EQUIPMENT;
		Ge.reason = CGEMEvt::EQPIDLE;
		pGDev->pOLnClt->GEMEvent(Ge);
		pGDev->AramsE95.Save("Aram413.bin");
		if (pGDev->pLTMgr) {
			pGDev->pLTMgr->SetLight();
		}
	}
}

SArams::ARAMSSTATE CSystemStatusDlg::GetCurrentState() {
	return pGDev->AramsE95.GetCurrentState();
}

void CSystemStatusDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSystemStatusDlg::OnClearabort() {
	p413App->Global.bAborting = FALSE;
	p413App->Global.bStop = FALSE;
}