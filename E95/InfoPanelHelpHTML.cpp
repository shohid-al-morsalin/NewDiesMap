// InfoPanelHelpHTML.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "InfoPanelHelpHTML.h"
#include "ST_SplitterWnd.h"
#include "..\SRC\DOSUtil.h"
#include "GlobalUIContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHelpHTML

IMPLEMENT_DYNCREATE(CInfoPanelHelpHTML, CResizableFormView)

CInfoPanelHelpHTML::CInfoPanelHelpHTML()
	: CResizableFormView(CInfoPanelHelpHTML::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelHelpHTML)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CInfoPanelHelpHTML::~CInfoPanelHelpHTML() {}

void CInfoPanelHelpHTML::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelHelpHTML)
	DDX_Control(pDX, IDC_BROWSER, m_cBrowser);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelHelpHTML, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelHelpHTML)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_BN_CLICKED(IDC_HOME, OnHome)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CInfoPanelHelpHTML, CResizableFormView)
	//{{AFX_EVENTSINK_MAP(CIEMozilla)
	ON_EVENT(CInfoPanelHelpHTML, 280, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHelpHTML diagnostics

#ifdef _DEBUG
void CInfoPanelHelpHTML::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelHelpHTML::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHelpHTML message handlers

LRESULT CInfoPanelHelpHTML::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	if (pUDev) pUDev->SetCurrentView("Help");
	return 0;
}

void CInfoPanelHelpHTML::OnInitialUpdate() {
	//	AddResizedControl(IDC_BROWSER,TOP_LEFT,BOTTOM_RIGHT);
	AddResizedControl(IDC_PREV, TOP_RIGHT);
	AddResizedControl(IDC_NEXT, TOP_RIGHT);
	AddResizedControl(IDC_HOME, TOP_RIGHT);
	AddResizedControl(IDC_ABOUT, TOP_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	bWnd = TRUE;

	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	//OnHome();	// Start on demand [6/30/2012 user]
}

int CInfoPanelHelpHTML::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CResizableFormView::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	// 	//Create the control, just make sure to use WS_CHILD and WS_VISIBLE.
	// 	if (!m_cBrowser.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), this, 9876))
	// 	{
	// 		//	AfxMessageBox("Failed to create HTML window");
	// 		return ;
	// 	}

	return 0;
}

void CInfoPanelHelpHTML::OnSize(UINT nType, int cx, int cy) {
	CResizableFormView::OnSize(nType, cx, cy);

	CWnd* pWnd = GetDlgItem(IDC_BROWSER);
	if (pWnd && m_cBrowser.m_hWnd) {
		CRect rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_cBrowser.MoveWindow(rc);
	}
}

void CInfoPanelHelpHTML::OnPrev() {
	try {
		m_cBrowser.GoBack();
	}
	catch (...) {}
}

void CInfoPanelHelpHTML::OnNext() {
	try {
		m_cBrowser.GoForward();
	}
	catch (...) {}
}

void CInfoPanelHelpHTML::OnHome() {
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "/index.htm";
	m_cBrowser.Navigate(dir, 0, 0, 0, 0);
}

void CInfoPanelHelpHTML::OnCommandStateChange(long Command, BOOL Enable) {}

void CInfoPanelHelpHTML::OnStnClickedFrame() {
	// TODO: Add your control notification handler code here
}