// InfoPanelLogsMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "XTabCtrl.h"
#include "InfoPanelLogsMainDlg.h"
#include "..\LOG\CLogMsgItem.h"
#include "..\LOG\LogMsgList.h"
#include "..\SRC\DosUtil.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#define IDT_REGISTER	9999

CInfoPanelLogsMainDlg::CInfoPanelLogsMainDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CInfoPanelLogsMainDlg::IDD, pParent)
	, level(10) {
	//{{AFX_DATA_INIT(CInfoPanelLogsMainDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Behavior = MAIN;

	if (!pUDev) {
		ASSERT(0);
	}
}

CInfoPanelLogsMainDlg::~CInfoPanelLogsMainDlg() {}

void CInfoPanelLogsMainDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelLogsMainDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelLogsMainDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CInfoPanelLogsMainDlg)
	ON_WM_TIMER()
	ON_MESSAGE(ID_LOG_REFRESH, OnLogRefresh)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelLogsMainDlg message handlers

BOOL CInfoPanelLogsMainDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_REFRESH, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);

	level = DosUtil.GetLogLevel();
	DosUtil.WriteLogLevel(level);

	SetupColumn();

	SetTimer(IDT_REGISTER, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInfoPanelLogsMainDlg::SetupColumn(void) {
	int idx = 0;

	// Insert a column. This override is the most convenient.
	m_List.InsertColumn(idx, _T("Time       "), LVCFMT_LEFT);
	m_List.SetColumnWidth(idx++, 96/*LVSCW_AUTOSIZE_USEHEADER*/);

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("Message");
	m_List.InsertColumn(idx, &col);
	m_List.SetColumnWidth(idx++, LVSCW_AUTOSIZE_USEHEADER);
	// 	col.pszText = _T("Source     ");
	// 	m_List.InsertColumn(idx, &col);
	// 	m_List.SetColumnWidth(idx++, 196/*LVSCW_AUTOSIZE_USEHEADER*/);
	// 	col.pszText = _T("Destination");
	// 	m_List.InsertColumn(idx, &col);
	// 	m_List.SetColumnWidth(idx++, 8/*LVSCW_AUTOSIZE_USEHEADER*/);
}

void CInfoPanelLogsMainDlg::ClearList() {
	if (m_List.GetItemCount() > 0) {
		m_List.DeleteAllItems();
	}
}

void CInfoPanelLogsMainDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REGISTER) {
		KillTimer(IDT_REGISTER);
		if (Behavior == MAIN) {
			if (!pGDev->RegisterWithLogSvr(m_hWnd)) {
				SetTimer(IDT_REGISTER, 100, NULL);
			}
		}
	}
}

LRESULT CInfoPanelLogsMainDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Log\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

LRESULT CInfoPanelLogsMainDlg::OnLogRefresh(WPARAM wP, LPARAM lP) {
	CLogMsgItem* p = (CLogMsgItem*)lP;
	if (!p) {
		return -1;
	}

	if (p->Level >= 0 && p->Level <= level) {
		short idx = m_List.InsertItem(0, p->DateTime);
		//int cnt = 1;
		m_List.SetItemText(idx, 1, p->MessageText);
		// 		m_List.SetItemText(idx, 2, p->Source);
		// 		m_List.SetItemText(idx, 3, p->Destination);
		short len = m_List.GetItemCount();
		if (len > MAX_LOGMSGITEM) {
			for (int i = 0; i < MAX_LOGMSGITEM / 3; i++) {
				m_List.DeleteItem(len - i);
			}
		}
	}
	return 0;
}

void CInfoPanelLogsMainDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CInfoPanelLogsMainDlg::OnDestroy() {
	ClearList();
	CResizableDialog::OnDestroy();
}