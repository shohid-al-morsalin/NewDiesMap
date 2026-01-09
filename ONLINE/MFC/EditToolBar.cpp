// EditToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "EditToolBar.h"
#include "..\inc\define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND m_hToolWnd;

int  m_ToolBarStartx;

/////////////////////////////////////////////////////////////////////////////
// CEditToolBar

CEditToolBar::CEditToolBar() {}

CEditToolBar::~CEditToolBar() {
	m_Font.DeleteObject();
	m_Brush.DeleteObject();
}

BEGIN_MESSAGE_MAP(CEditToolBar, CToolBar)
	//{{AFX_MSG_MAP(CEditToolBar)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_USER_TOOLEDITRPY, ReplyAlarm)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditToolBar message handlers
BOOL CEditToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID) {
	CRect rect(0, 0, 200, 200);
	return CToolBar::Create(pParentWnd, dwStyle, nID);
}
BOOL CEditToolBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID) {
	CRect rect(0, 0, 200, 200);
	//dwCtrlStyle = 0;//&= ~MFS_MOVEFRAME;
	return CToolBar::CreateEx(pParentWnd, dwCtrlStyle, dwStyle, rcBorders, nID);
}

BOOL CEditToolBar::DrawEdit() {
	return TRUE;
}

LRESULT CEditToolBar::ReplyAlarm(WPARAM wParam, LPARAM lParam) {
	char txt[200];
	if (wParam) {
		memset(txt, 0, sizeof txt);
		CString str("");
		str.Format("%s", (char*)lParam);
		SetDlgItemText(IDC_ALARMEDIT, "");
		SetDlgItemText(IDC_ALARMEDIT, str);
	}
	else {
		SetDlgItemText(IDC_ALARMEDIT, "");
	}
	return 0L;
}

int CEditToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	m_Font.CreateFont(25, 0, 0, 0, /*100*/FW_BOLD, FALSE, FALSE,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FF_SWISS, "Courier");

	CRect rect(90, 10, 1300, 30);
	m_Edit.Create(WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, rect, this, IDC_ALARMEDIT);
	m_Edit.ShowWindow(SW_SHOW);
	m_Edit.UpdateWindow();

	m_hToolWnd = this->GetSafeHwnd();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ALARMEDIT);
	pEdit->SetFont(&m_Font, FALSE);

	m_Brush.CreateSolidBrush(RGB(192, 192, 192));

	m_Edit.SetReadOnly();

	return 0;
}

HBRUSH CEditToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ALARMEDIT);

	HWND hWnd = pEdit->GetSafeHwnd();

	HBRUSH hbr = CToolBar::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC && hWnd == pWnd->GetSafeHwnd()) {
		//pDC->SetBkColor(RGB(255,0,0 ));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 20, 20));
		return hbr;
	}
	return  hbr;
}

void CEditToolBar::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	CToolBar::OnLButtonDblClk(nFlags, point);

	if (IsFloating()) {
		CWnd* pMiniFrame;
		CWnd* pDockBar;

		pDockBar = GetParent();
		pMiniFrame = pDockBar->GetParent();
		pMiniFrame->ModifyStyle(WS_SYSMENU, NULL);

		pMiniFrame->ShowWindow(SW_HIDE);
		pMiniFrame->ShowWindow(SW_SHOW);
	}
}