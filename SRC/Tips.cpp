// Tips.cpp : implementation file
//

#include "stdafx.h"
#include "Tips.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTips

CTips::CTips() {
	msg = "";
	dx = dy = 0;
	bButDown = FALSE;
}

CTips::~CTips() {}

BEGIN_MESSAGE_MAP(CTips, CWnd)
	//{{AFX_MSG_MAP(CTips)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTips message handlers

void CTips::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CFont font, * pfont;
	font.CreatePointFont(110, "Arial Narrow");
	pfont = (CFont*)dc.SelectObject(&font);

	CRect rc;
	GetClientRect(&rc);

	CBrush* pbr;
	CBrush br(RGB(250, 250, 228));
	pbr = (CBrush*)dc.SelectObject(&br);
	dc.SetBkMode(TRANSPARENT);
	dc.Rectangle(&rc);
	dc.TextOut((rc.Width() - dc.GetTextExtent(msg).cx) / 2, 1, msg);
	dc.SelectObject(pbr);

	dc.SelectObject(pfont);

	// Do not call CWnd::OnPaint() for painting messages
}

void CTips::OnMouseMove(UINT nFlags, CPoint point) {
	if (bButDown) {
		dx = point.x - pt.x;
		dy = point.y - pt.y;

		iniRc.left += dx; iniRc.right += dx;
		iniRc.top += dy; iniRc.bottom += dy;
		MoveWindow(iniRc.left, iniRc.top, iniRc.Width(), iniRc.Height(), TRUE);
	}
}

void CTips::OnLButtonDown(UINT nFlags, CPoint point) {
	bButDown = TRUE;
	SetCapture();
	pt = point;
}

void CTips::OnLButtonUp(UINT nFlags, CPoint point) {
	ReleaseCapture();
	bButDown = FALSE;
}

void CTips::Move() {
	MoveWindow(iniRc.left, iniRc.top, iniRc.Width(), iniRc.Height(), TRUE);
}

void CTips::Move(CRect& rc) {
	iniRc = rc;
	Move();
	Invalidate();
}

void CTips::Hide() {
	ShowWindow(FALSE);
}

void CTips::Show() {
	ShowWindow(TRUE);
}