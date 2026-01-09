// InfoPanelLogs.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "..\SRC\DOSUtil.h"
#include "ST_SplitterWnd.h"
#include "InfoPanelLogs.h"
#include "InfoPanelLogsMainDlg.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

// CInfoPanelLogs

IMPLEMENT_DYNCREATE(CInfoPanelLogs, CResizableFormView)

CInfoPanelLogs::CInfoPanelLogs()
	: CResizableFormView(CInfoPanelLogs::IDD)
	, m_InfoPanelLogsMainDlg(NULL) {
	//{{AFX_DATA_INIT(CInfoPanelLogs)
	//}}AFX_DATA_INIT
}

CInfoPanelLogs::~CInfoPanelLogs() {}

void CInfoPanelLogs::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelLogs)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelLogs, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelLogs)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelLogs diagnostics

#ifdef _DEBUG
void CInfoPanelLogs::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelLogs::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelLogs message handlers

void CInfoPanelLogs::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();

	short nTab = 0;
	m_InfoPanelLogsMainDlg = new CInfoPanelLogsMainDlg;
	if (m_InfoPanelLogsMainDlg) {
		m_InfoPanelLogsMainDlg->level = DosUtil.GetLogLevel();
		m_InfoPanelLogsMainDlg->Behavior = CInfoPanelLogsMainDlg::MAIN;
		m_InfoPanelLogsMainDlg->Create(CInfoPanelLogsMainDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_InfoPanelLogsMainDlg, "Main", nTab++);
	}
}

LRESULT CInfoPanelLogs::OnSTSelected(WPARAM wP, LPARAM lP) {
	if (pUDev) pUDev->SetCurrentView("Log");
	if (m_InfoPanelLogsMainDlg) {
	}
	m_tabctrl.ActivateCurrentTab();

	return 0;
}

void CInfoPanelLogs::OnDestroy() {
	if (m_InfoPanelLogsMainDlg) {
		m_InfoPanelLogsMainDlg->DestroyWindow();
		m_InfoPanelLogsMainDlg = NULL;
	}
	CResizableFormView::OnDestroy();
}