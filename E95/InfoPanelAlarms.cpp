// InfoPanelAlarms.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "AlarmAllDlg.h"
#include "InfoPanelAlarms.h"

#include "XTabCtrl.h"
#include "ST_SplitterWnd.h"

#include "..\ONLINE\MFC\AlarmListDlg.h"

// CInfoPanelAlarms

IMPLEMENT_DYNCREATE(CInfoPanelAlarms, CResizableFormView)

CInfoPanelAlarms::CInfoPanelAlarms()
	: CResizableFormView(CInfoPanelAlarms::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelAlarms)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_AlarmAll = NULL;
	m_AlarmList = NULL;
}

CInfoPanelAlarms::~CInfoPanelAlarms() {
	if (m_AlarmAll) {
		delete m_AlarmAll;
	}
	if (m_AlarmList) {
		delete m_AlarmList;
	}
}

void CInfoPanelAlarms::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelAlarms)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelAlarms, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelAlarms)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
END_MESSAGE_MAP()

// CInfoPanelAlarms diagnostics

#ifdef _DEBUG
void CInfoPanelAlarms::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelAlarms::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelAlarms message handlers
void CInfoPanelAlarms::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();

	short nTab = 0;
	m_AlarmList = new CAlarmListDlg;
	if (m_AlarmList) {
		m_AlarmList->Create(CAlarmListDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_AlarmList, "Active", nTab++);
	}
	//m_AlarmAll = new CAlarmAllDlg;
	if (m_AlarmAll) {
		m_AlarmAll->Create(CAlarmAllDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_AlarmAll, "All Alarms", nTab++);
	}
}

LRESULT CInfoPanelAlarms::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	return 0;
}

void CInfoPanelAlarms::OnDestroy() {
	if (m_AlarmAll) {
		m_AlarmAll->DestroyWindow();
		m_AlarmAll = NULL;
	}
	if (m_AlarmList) {
		m_AlarmList->DestroyWindow();
		m_AlarmList = NULL;
	}
	CResizableFormView::OnDestroy();
}