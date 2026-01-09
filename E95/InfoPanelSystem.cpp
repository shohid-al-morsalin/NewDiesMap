// InfoPanelSystem.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "InfoPanelSystem.h"
#include "OnLineDlg.h"
#include "LoadPortDlg.h"
#include "SystemStatusDlg.h"
#include "SystemRobotDlg.h"
#include "SystemLTowerDlg.h"
#include "System413Dlg.h"
#include "..\SYS\CUMMsg.h"
#include "ST_SplitterWnd.h"
#include "RecoveryDlg.h"

#include "..\SRC\DOSUtil.h"

#include "413App.h"
#include "GlobalDeviceContainer.h"

// CInfoPanelSystem

IMPLEMENT_DYNCREATE(CInfoPanelSystem, CResizableFormView)

CInfoPanelSystem::CInfoPanelSystem()
	: CResizableFormView(CInfoPanelSystem::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelSystem)
	//}}AFX_DATA_INIT
	nLoadPort = 0;

	m_OnLineDlg = NULL;
	m_RecoveryDlg = NULL;
	m_System413Dlg = NULL;
	m_SystemRobotDlg = NULL;
	m_SystemLTowerDlg = NULL;
	m_SystemStatusDlg = NULL;

	for (int i = 0; i < 4; i++) {
		m_LoadPortDlg[i] = NULL;
	}
	nLoadPort = DosUtil.GetNumLP();
}

CInfoPanelSystem::~CInfoPanelSystem() {}

void CInfoPanelSystem::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelSystem)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelSystem, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelSystem)
	ON_MESSAGE(UM_WAFERRECOVERY, OnRobotRecovery)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_MESSAGE(UM_CLOSETAB, OnCloseView)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelSystem diagnostics

#ifdef _DEBUG
void CInfoPanelSystem::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelSystem::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelSystem message handlers

void CInfoPanelSystem::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();

	int nTab = 0;

	if (p413App->Global.Confg.bSecsMod) {
		m_OnLineDlg = new COnLineDlg;
		m_OnLineDlg->Create(COnLineDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_OnLineDlg, "Online Status", nTab++);
	}

	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		for (int i = 0; i < nLoadPort; i++) {
			m_LoadPortDlg[i] = new CLoadPortDlg;
			if (m_LoadPortDlg[i]) {
				m_LoadPortDlg[i]->ID = i;	// assign load port unit to it
				m_LoadPortDlg[i]->Create(CLoadPortDlg::IDD, &m_tabctrl);
				CString str;
				str.Format("Load Port #%d Status", i + 1);
				m_tabctrl.AddTab(m_LoadPortDlg[i], str.GetBuffer(0), nTab++);
			}
		}
	}

	m_SystemStatusDlg = new CSystemStatusDlg;
	if (m_SystemStatusDlg) {
		m_SystemStatusDlg->Create(CSystemStatusDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_SystemStatusDlg, "Equipment Status", nTab++);
	}

	if (p413App->Global.Confg.bSecsMod) {
		if ((pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) ||
			(pGDev->SystemModel == CGlobalDeviceContainer::FSM413MOT) ||
			(pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA)) {
			//m_System413Dlg = new CSystem413Dlg;
			if (m_System413Dlg) {
				m_System413Dlg->Create(CSystem413Dlg::IDD, &m_tabctrl);
				m_tabctrl.AddTab(m_System413Dlg, "413 Module Status", nTab++);
			}
		}
	}

	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		//m_SystemRobotDlg = new CSystemRobotDlg;
		if (m_SystemRobotDlg) {
			m_SystemRobotDlg->m_Parent = this;
			m_SystemRobotDlg->Create(CSystemRobotDlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_SystemRobotDlg, "Robot Status", nTab++);
		}
	}
	//m_SystemLTowerDlg = new CSystemLTowerDlg;
	if (m_SystemLTowerDlg) {
		m_SystemLTowerDlg->Create(CSystemLTowerDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_SystemLTowerDlg, "Light Tower Status", nTab++);
	}
}

LRESULT CInfoPanelSystem::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	return 0;
}

LRESULT CInfoPanelSystem::OnRobotRecovery(WPARAM wP, LPARAM lP) {
	int idx;
	static CString ViewName = "";
	if (m_RecoveryDlg) {
		idx = m_tabctrl.GetTabIndexByName(ViewName.GetBuffer(0));
		if (idx > -1) {
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)m_RecoveryDlg->GetSafeHwnd() != 0xfeeefeee)) {
				m_RecoveryDlg->DestroyWindow();	// if could not find tab means m_RecoveryDlg has been destroyed
			}
			m_RecoveryDlg = NULL;
		}
	}
	//[20211108
	if (pGDev->SystemModel != CGlobalDeviceContainer::FSM413C2C) {
		PostMessage(UM_TO_PROCJOBMANUALDLG, 0, 0);
		return 0;
	}
	//]
	m_RecoveryDlg = new CRecoveryDlg;
	if (m_RecoveryDlg) {
		m_RecoveryDlg->ID = (short)lP;
		m_RecoveryDlg->type = wP;	// 1 is 128, 2 is 900
		m_RecoveryDlg->m_ParentMain = NULL;
		m_RecoveryDlg->m_ParentSystem = this;
		m_RecoveryDlg->Create(CRecoveryDlg::IDD, &m_tabctrl);
		switch (wP) {
		case 3:
			ViewName = "413 Recovery";
			break;
		default:
			ASSERT(0);
			break;
		}
		m_tabctrl.AddTab(m_RecoveryDlg, ViewName.GetBuffer(0), 1);
		idx = m_tabctrl.GetTabIndexByName(ViewName.GetBuffer(0));
		m_tabctrl.SelectTab(idx);
	}
	return 0;
}

LRESULT CInfoPanelSystem::OnCloseView(WPARAM wP, LPARAM lP) {
	if (!lP) {
		ASSERT(0);
		return -1;
	}
	CRecoveryDlg* pV = (CRecoveryDlg*)lP;
	if (pV) {
		int idx = m_tabctrl.FindTabIndex(pV);
		if (idx > -1) {
			if (idx > 0) m_tabctrl.SelectTab(idx - 1);
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)pV->GetSafeHwnd() != 0xfeeefeee)) {
				delete pV;
			}
			m_RecoveryDlg = NULL;
		}
	}
	return 0;
}

void CInfoPanelSystem::OnDestroy() {
	ClearAll();
	CResizableFormView::OnDestroy();
}

void CInfoPanelSystem::ClearAll() {
	for (int i = 0; i < 4; i++) {
		if (m_LoadPortDlg[i]) {
			m_LoadPortDlg[i]->DestroyWindow();
			m_LoadPortDlg[i] = NULL;
		}
	}
	if (m_SystemRobotDlg) {
		m_SystemRobotDlg->DestroyWindow();
		m_SystemRobotDlg = NULL;
	}
	if (m_OnLineDlg) {
		m_OnLineDlg->DestroyWindow();
		m_OnLineDlg = NULL;
	}
	if (m_SystemStatusDlg) {
		m_SystemStatusDlg->DestroyWindow();
		m_SystemStatusDlg = NULL;
	}
	if (m_System413Dlg) {
		m_System413Dlg->Stop();
		m_System413Dlg->DestroyWindow();
		m_System413Dlg = NULL;
	}
	if (m_SystemLTowerDlg) {
		m_SystemLTowerDlg->DestroyWindow();
		m_SystemLTowerDlg = NULL;
	}
	if (m_RecoveryDlg) {
		m_RecoveryDlg->DestroyWindow();
		m_RecoveryDlg = NULL;
	}
}