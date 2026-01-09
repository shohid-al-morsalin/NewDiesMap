#include "stdafx.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "LTW/LTMgrE95.h"
#include "ONLINE/GEM/GemFactory.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/MFC/establishcomm.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "OnlineMgr.h"
#include "SYS/Var.h"
#include "TitlePanel.h"

#include "OnlineModeDlg.h"

#define IDT_TIMER2		9990
#define IDT_REFRESH		9999

extern BOOL    m_COMMODE;

// COnlineModeDlg dialog

//IMPLEMENT_DYNAMIC(COnlineModeDlg, CResizableDialog)
COnlineModeDlg::COnlineModeDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineModeDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineModeDlg)
	//}}AFX_DATA_INIT

	preState = -1;
	tout = 16000; // 16 seconds [12/23/2007 YUEN]
}

COnlineModeDlg::~COnlineModeDlg() {}

void COnlineModeDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineModeDlg)
	DDX_Control(pDX, IDC_SEPARATE, m_cSeparate);
	DDX_Control(pDX, IDC_SELECT, m_cSelect);
	DDX_Control(pDX, IDC_LOWLEVEL, m_cLowlevel);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_CONNECTION_STATUS, m_ConnInfo);
	DDX_Control(pDX, IDC_BUTTON_ONLINE, m_OnlineButt);
	DDX_Control(pDX, IDC_GOREMOTE, m_RemoteButt);
	DDX_Control(pDX, IDC_GOLOCAL, m_LocalButt);
	DDX_Control(pDX, IDC_GOOFFLINE, m_OffButt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineModeDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineModeDlg)
	ON_BN_CLICKED(IDC_GOREMOTE, OnBnClickedGoremote)
	ON_BN_CLICKED(IDC_GOLOCAL, OnBnClickedGolocal)
	ON_BN_CLICKED(IDC_GOOFFLINE, OnBnClickedGooffline)
	ON_BN_CLICKED(IDC_BUTTON_ONLINE, OnBnClickedButtonOnline)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_BN_CLICKED(IDC_SEPARATE, OnSeparate)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(UWM_USER_HOMEFINISHED , OnDoneRBHome)
END_MESSAGE_MAP()

// COnlineModeDlg message handlers

void COnlineModeDlg::OnBnClickedGoremote() {
	if (pGDev->pGEMObjs->GetLineMode() == EqpStatus::ATTEMPTONLINE) {
		return;
	}
	if (hmHSMSSS == CHsmsParaMeter::SELECTED) {
		if (hmCommState == CHsmsParaMeter::COMMUNICATING) {
			if (CHsmsParaMeter::m_ChgmodePossible == TRUE) {
				KillAllTimer();
				DisableAllButtons();
				m_COMMODE = TRUE;
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				switch (pGDev->pGEMObjs->GetLineMode()/* m_pDataBank->m_EStatus.LineMode*/) {
				case EqpStatus::OFFLINEEQP:
				case EqpStatus::OFFLINEHOST:
				case EqpStatus::LOCAL:
					pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE);
					break;
				default:
					break;
				}
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(pEvt);
				m_gEvtQue.CloseMemory();
				msg = "Go REMOTE initiated";
				Message(msg, 5);
				SetTimer(IDT_TIMER2 + 0, tout, NULL);
				return;
			}
		}
	}
	msg = "Cannot perform";
	Message(msg, 5);
	msg = "Not SELECTED or not COMMUNICATING";
	Message(msg, 5);
}

void COnlineModeDlg::OnBnClickedGolocal() {
	if (pGDev->pGEMObjs->GetLineMode() == EqpStatus::ATTEMPTONLINE) {
		return;
	}
	if (hmHSMSSS == CHsmsParaMeter::SELECTED) {
		if (hmCommState == CHsmsParaMeter::COMMUNICATING) {
			if (CHsmsParaMeter::m_ChgmodePossible == TRUE) {
				KillAllTimer();
				DisableAllButtons();
				m_COMMODE = TRUE;
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				switch (pGDev->pGEMObjs->GetLineMode()/* m_pDataBank->m_EStatus.LineMode*/) {
				case EqpStatus::OFFLINEEQP:
				case EqpStatus::OFFLINEHOST:
				case EqpStatus::REMOTE:
					pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_LOCAL);
					break;
				default:
					break;
				}
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(pEvt);
				m_gEvtQue.CloseMemory();
				msg = "Go LOCAL initiated";
				Message(msg, 5);
				SetTimer(IDT_TIMER2 + 1, tout, NULL);
				return;
			}
		}
	}
	msg = "Cannot perform";
	Message(msg, 5);
	msg = "Not SELECTED or not COMMUNICATING";
	Message(msg, 5);
}

void COnlineModeDlg::OnBnClickedGooffline() {
	if (CHsmsParaMeter::m_ChgmodePossible == TRUE) {
		KillAllTimer();
		DisableAllButtons();
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_OFFLINE);
		m_gEvtQue.OpenMemory("EVTMSG");
		m_gEvtQue.WriteQue(pEvt);
		m_gEvtQue.CloseMemory();
		msg = "Go OFFLINE initiated";
		Message(msg, 5);
		SetTimer(IDT_TIMER2 + 2, tout, NULL);
	}
}

void COnlineModeDlg::OnBnClickedButtonOnline() {
	pGDev->pOLnMgr->EstablishConnection();
}

void COnlineModeDlg::KillAllTimer() {
	KillTimer(IDT_TIMER2 + 0);
	KillTimer(IDT_TIMER2 + 1);
	KillTimer(IDT_TIMER2 + 2);
}

void COnlineModeDlg::DisableAllButtons() {
	m_OffButt.EnableWindow(FALSE);
	m_LocalButt.EnableWindow(FALSE);
	m_RemoteButt.EnableWindow(FALSE);
}

void COnlineModeDlg::UpdateButtons() {
	CString str;

	switch (hmHSMSSS) {
	case CHsmsParaMeter::CONNECTED:
		str = "CONNECTED";
		break;
	case CHsmsParaMeter::SELECTED:
		str = "SELECTED";
		break;
	case CHsmsParaMeter::NOTSELECTED:
		str = "NOT SELECTED";
		break;
	case CHsmsParaMeter::ATTEMPTCONNECT:
		str = "ATTEMPT CONNECT";
		break;
	default:
		str = "NOT CONNECTED";
		break;
	}
	str += "\n";
	switch (hmCommState) {
	case CHsmsParaMeter::COMMUNICATING:
		str += "COMMUNICATING";
		break;
	case CHsmsParaMeter::ENABLED:
		str += "ENABLED";
		break;
	case CHsmsParaMeter::WAITCRA:
		str += "WAITCRA";
		break;
	case CHsmsParaMeter::WAITDELAY:
		str += "WAITDELAY";
		break;
	default:
		str += "NOT COMMUNICATE";
		break;
	}
	str += "\n";
	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::ATTEMPTONLINE:
		str += "ATTEMPT ONLINE";
		break;
	case EqpStatus::OFFLINEEQP:
		str += "OFFLINE EQUIP";
		break;
	case EqpStatus::OFFLINEHOST:
		str += "OFFLINE HOST";
		break;
	case EqpStatus::LOCAL:
		str += "LOCAL";
		break;
	case EqpStatus::REMOTE:
		str += "REMOTE";
		break;
	}
	m_ConnInfo.SetWindowText(str);

	pUDev->pTitlePanel->SetCommState(str);

	if (hmHSMSSS == CHsmsParaMeter::SELECTED) {
		if (hmCommState == CHsmsParaMeter::COMMUNICATING) {
		}
		else {
		}
	}
	else {
	}

	// This implementation is for LineMode is independent of CommState
	//  [12/23/2007 YUEN]
	int nowState = pGDev->pGEMObjs->GetLineMode();
	if (nowState != preState) {
		preState = nowState;
		switch (nowState) {
		case EqpStatus::ATTEMPTONLINE:
			m_OffButt.EnableWindow(FALSE);
			m_LocalButt.EnableWindow(FALSE);
			m_RemoteButt.EnableWindow(FALSE);
			msg = "ATTEMPT ONLINE";
			Message(msg, 5);
			break;
		case EqpStatus::OFFLINEEQP:
		case EqpStatus::OFFLINEHOST:
			m_OffButt.EnableWindow(FALSE);
			m_LocalButt.EnableWindow(TRUE);
			m_RemoteButt.EnableWindow(TRUE);
			msg = "Go OFFLINE success";
			if (pGDev && pGDev->pLTMgr) {
				pGDev->pLTMgr->SetLight();
			}
			Message(msg, 5);
			break;
		case EqpStatus::LOCAL:
			m_OffButt.EnableWindow(TRUE);
			m_LocalButt.EnableWindow(FALSE);
			m_RemoteButt.EnableWindow(TRUE);
			msg = "Go LOCAL success";
			Message(msg, 5);
			if (pGDev && pGDev->pLTMgr) {
				pGDev->pLTMgr->SetLight();
			}
			break;
		case EqpStatus::REMOTE:
			m_OffButt.EnableWindow(TRUE);
			m_LocalButt.EnableWindow(TRUE);
			m_RemoteButt.EnableWindow(FALSE);
			msg = "Go REMOTE success";
			Message(msg, 5);
			if (pGDev && pGDev->pLTMgr) {
				pGDev->pLTMgr->SetLight();
			}
			break;
		}
	}
}

BOOL COnlineModeDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_GOREMOTE, TOP_RIGHT);
	AddAnchor(IDC_GOLOCAL, TOP_RIGHT);
	AddAnchor(IDC_GOOFFLINE, TOP_RIGHT);
	AddAnchor(IDC_SEPARATE, TOP_RIGHT);
	AddAnchor(IDC_SELECT, TOP_RIGHT);
	AddAnchor(IDC_LOWLEVEL, TOP_RIGHT);
	AddAnchor(IDC_BUTTON_ONLINE, BOTTOM_RIGHT);

	if (Var.UIMode > 1) {
		m_cSelect.ShowWindow(TRUE);
		m_cSeparate.ShowWindow(TRUE);
		m_cLowlevel.ShowWindow(TRUE);
	}

	UpdateButtons();

	msg.Format("EstablishCom timeout value: %d ms", tout);
	Message(msg, 5);

	SetTimer(IDT_REFRESH, 1750, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineModeDlg::OnRemoteChangeTimeout() {
	if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) {
		msg = "Change REMOTE timeout";
		Message(msg, 5);
		preState = -1;  // enable buttons to change [12/23/2007 YUEN]
		UpdateButtons();
	}
}
void COnlineModeDlg::OnOfflineChangeTimeout() {
	if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
		msg = "Change OFFLINE timeout";
		Message(msg, 5);
		preState = -1;  // enable buttons to change [12/23/2007 YUEN]
		UpdateButtons();
	}
}
void COnlineModeDlg::OnLocalChangeTimeout() {
	if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::LOCAL) {
		msg = "Change LOCAL timeout";
		Message(msg, 5);
		preState = -1;  // enable buttons to change [12/23/2007 YUEN]
		UpdateButtons();
	}
}
void COnlineModeDlg::OnTimer(UINT nIDEvent) {
	switch (nIDEvent) {
	case IDT_TIMER2 + 0:
		KillTimer(IDT_TIMER2);
		OnRemoteChangeTimeout();
		break;
	case IDT_TIMER2 + 1:
		KillTimer(IDT_TIMER2 + 1);
		OnLocalChangeTimeout();
		break;
	case IDT_TIMER2 + 2:
		KillTimer(IDT_TIMER2 + 2);
		OnOfflineChangeTimeout();
		break;
	case IDT_REFRESH:
		KillTimer(IDT_REFRESH);
		UpdateButtons();
		SetTimer(IDT_REFRESH, 1000, NULL);
		break;
	}
	CResizableDialog::OnTimer(nIDEvent);
}

void COnlineModeDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "OLMode");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineModeDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void COnlineModeDlg::OnDestroy() {
	CResizableDialog::OnDestroy();
}

void COnlineModeDlg::Message(CString& msg, short lvl) {
	Log(msg, lvl);
	m_cList.InsertString(0, msg);
	if (m_cList.GetCount() > 64) {
		int cnt = m_cList.GetCount();
		for (int i = 0; i < 24; i++) {
			m_cList.DeleteString(cnt--);
		}
	}
}

void COnlineModeDlg::OnSelect() {
	if (pGDev->pOLnMgr) {
		pGDev->pOLnMgr->SendSelectReq();
	}
}

void COnlineModeDlg::OnSeparate() {
	if (pGDev->pOLnMgr) {
		pGDev->pOLnMgr->SendSeparateReq();
	}
}