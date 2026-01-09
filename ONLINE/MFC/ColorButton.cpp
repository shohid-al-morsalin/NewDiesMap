// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

/***************************************************************************
 * This code came from CODEGURU
 * Also, little bit changed from Alex , Kang
 *
 ***************************************************************************/

CColorButton::CColorButton() {
	m_nColor = GetSysColor(COLOR_BTNFACE);
	m_sColor = m_nColor;
	m_hColor = m_nColor;
	m_dColor = m_nColor;
	m_nBorder = 1;
	//m_lfEscapement = 0;
	m_pNormal = NULL;
	m_pSelected = NULL;
	m_pHover = NULL;
	m_pDisabled = NULL;
	//m_hRgn = 0;
	m_bHover = FALSE;
	m_bCapture = FALSE;
	m_bMouseDown = FALSE;
	m_bNeedBitmaps = TRUE;
	m_ButtonCaption.Empty();
	m_pMemDC = NULL;
	m_nState = 0;
}

CColorButton::~CColorButton() {
	m_Brush.DeleteObject();
	delete m_pDisabled;
	m_pDisabled = NULL;
	delete m_pHover;
	m_pHover = NULL;
	delete m_pSelected;
	m_pSelected = NULL;
	delete m_pNormal;
	m_pNormal = NULL;
}

BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers
BOOL CColorButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point,
	const HRGN hRgn, CWnd* pParentWnd, UINT nID) {
	m_ButtonCaption = lpszCaption;
	DeleteObject(m_hRgn);
	m_hRgn = CreateRectRgn(0, 0, 31, 31);
	CRect box(0, 0, 0, 0);
	if (m_hRgn != 0)
		CombineRgn(m_hRgn, hRgn, 0, RGN_COPY);
	GetRgnBox(m_hRgn, &box);
	OffsetRgn(m_hRgn, -box.left, -box.top);
	GetRgnBox(m_hRgn, &box);
	m_CenterPoint = CPoint(box.left + box.Width() / 2, box.top + box.Height() / 2);
	box.OffsetRect(point);

	return CButton::Create(lpszCaption, dwStyle, box, pParentWnd, nID);
}

BOOL CColorButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point,
	const HRGN hRgn, CWnd* pParentWnd, UINT nID, UINT nBorder,
	COLORREF nColor, COLORREF sColor, COLORREF hColor, COLORREF dColor) {
	m_nBorder = nBorder;
	m_nColor = nColor;
	m_sColor = sColor;
	m_hColor = hColor;
	m_dColor = dColor;
	m_Brush.CreateSolidBrush(m_dColor);

	return Create(lpszCaption, dwStyle, point, hRgn, pParentWnd, nID);
}

void CColorButton::PreSubclassWindow() {
	// TODO: Add your specialized code here and/or call the base class
	ModifyStyle(0, BS_OWNERDRAW | BS_PUSHBUTTON);
	CButton::PreSubclassWindow();
}

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	//m_pMemDC = new CDC;

	m_pMemDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rect;
	GetClientRect(rect);
	if (m_bNeedBitmaps)
		PrepareStateBitmaps(pDC, &rect);
	DrawButton(pDC, &rect, lpDrawItemStruct->itemState);
	m_nState = lpDrawItemStruct->itemState;
}

void CColorButton::PrepareStateBitmaps(CDC* pDC, CRect* pRect) {
	CDC* pMemDC;
	pMemDC = new CDC;
	pMemDC->CreateCompatibleDC(pDC);

	PrepareNormalState(pDC, pMemDC, pRect);
	PrepareSelectedState(pDC, pMemDC, pRect);
	PrepareHoverState(pDC, pMemDC, pRect);
	PrepareDisabledState(pDC, pMemDC, pRect);

	// clean up
	delete pMemDC;
	m_bNeedBitmaps = FALSE;
}

void CColorButton::DrawButton(CDC* pDC, CRect* pRect, UINT state) {
	CDC* pMemDC = new CDC;
	pMemDC->CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap;

	// get region
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hRgn);

	// select bitmap to paint depending upon button state
	if (state & ODS_DISABLED)
		pOldBitmap = pMemDC->SelectObject(m_pDisabled);
	else {
		if (state & ODS_SELECTED)
			pOldBitmap = pMemDC->SelectObject(m_pSelected);
		else {
			if (m_bHover)
				pOldBitmap = pMemDC->SelectObject(m_pHover);
			else
				pOldBitmap = pMemDC->SelectObject(m_pNormal);
		}
	}

	// paint using region for clipping
	::SelectClipRgn(pDC->GetSafeHdc(), hRgn);
	pDC->BitBlt(0, 0, pRect->Width(), pRect->Height(), pMemDC, 0, 0, SRCCOPY);
	::SelectClipRgn(pDC->GetSafeHdc(), NULL);

	// clean up
	DeleteObject(hRgn);
	pMemDC->SelectObject(pOldBitmap);
	delete pMemDC;
}

void CColorButton::PrepareNormalState(CDC* pDC, CDC* pMemDC, CRect* pRect) {
	delete m_pNormal;
	m_pNormal = new CBitmap;
	PaintRgn(pDC, pMemDC, m_pNormal, m_nColor, pRect, TRUE, FALSE);
}

void CColorButton::PaintRgn(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, COLORREF color,
	CRect* pRect, BOOL bEnabled, BOOL bSunken) {
	pBitmap->CreateCompatibleBitmap(pDC, pRect->Width(), pRect->Height());
	CBitmap* pOldBitmap = pMemDC->SelectObject(pBitmap);

	// prepare region
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hRgn);

	// fill rect a with transparent color and fill rgn
	HBRUSH hBrush = CreateSolidBrush(color);
	pMemDC->FillSolidRect(pRect, RGB(0, 0, 0));
	FillRgn(pMemDC->GetSafeHdc(), hRgn, hBrush);
	DeleteObject(hBrush);

	// draw 3D border and text
	DrawButtonCaption(pMemDC->GetSafeHdc(), pRect, bEnabled, bSunken);
	FrameRgn3D(pMemDC->GetSafeHdc(), hRgn, bSunken);

	// clean up
	DeleteObject(hRgn);
	pMemDC->SelectObject(pOldBitmap);
}

void CColorButton::DrawButtonCaption(HDC hDC, CRect* pRect, BOOL bEnabled, BOOL bSunken) {
	int nOldMode = SetBkMode(hDC, TRANSPARENT);
	CString text;

	//GetWindowText(text);
	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	HFONT hFont = CreateFontIndirect(&lf);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	// determine point where to output text
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	CPoint p = CPoint(m_CenterPoint.x, m_CenterPoint.y + tm.tmHeight / 2);
	if (bSunken)
		p.Offset(m_nBorder, m_nBorder);

	// draw button caption depending upon button state
	if (bEnabled) {
		SetTextColor(hDC, RGB(0, 0, 0));//GetSysColor(COLOR_BTNTEXT));
		SetTextAlign(hDC, TA_CENTER | TA_BOTTOM);
		TextOut(hDC, p.x, p.y, m_ButtonCaption, m_ButtonCaption.GetLength());
	}
	else {
		SetTextColor(hDC, GetSysColor(COLOR_3DHILIGHT));
		TextOut(hDC, p.x + 1, p.y + 1, m_ButtonCaption, m_ButtonCaption.GetLength());
		SetTextColor(hDC, GetSysColor(COLOR_3DSHADOW));
		TextOut(hDC, p.x, p.y, m_ButtonCaption, m_ButtonCaption.GetLength());
	}

	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	SetBkMode(hDC, nOldMode);
}

void CColorButton::FrameRgn3D(HDC hDC, const HRGN hRgn, BOOL bSunken) {
	HBRUSH hBrush;
	HRGN hBaseRgn = CreateRectRgn(0, 0, 0, 0);

	COLORREF ltOuter, ltInner, rbOuter, rbInner;	// colors of inner and outer shadow for top-left and right-bottom corners

	// decide on color scheme
	if (!bSunken) {
		ltOuter = GetSysColor(COLOR_3DLIGHT);
		ltInner = GetSysColor(COLOR_3DHILIGHT);
		rbOuter = GetSysColor(COLOR_3DDKSHADOW);
		rbInner = GetSysColor(COLOR_3DSHADOW);
	}
	else {
		rbInner = GetSysColor(COLOR_3DLIGHT);
		rbOuter = GetSysColor(COLOR_3DHILIGHT);
		ltInner = GetSysColor(COLOR_3DDKSHADOW);
		ltOuter = GetSysColor(COLOR_3DSHADOW);
	}

	// offset highlight and shadow regions
	// substract them from the base region
	switch (m_nBorder) {
	case 2:

		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, 2, 2);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(ltInner);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);

		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, -2, -2);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(rbInner);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);

		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, 1, 1);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(ltOuter);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);

		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, -1, -1);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(rbOuter);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);
		break;

	default:
		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, 1, 1);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(ltInner);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);

		CombineRgn(hBaseRgn, hRgn, 0, RGN_COPY);
		OffsetRgn(hBaseRgn, -1, -1);
		CombineRgn(hBaseRgn, hRgn, hBaseRgn, RGN_DIFF);
		hBrush = CreateSolidBrush(rbOuter);
		FillRgn(hDC, hBaseRgn, hBrush);
		DeleteObject(hBrush);
		break;
	}

	// clean up regions
	DeleteObject(hBaseRgn);
}

void CColorButton::PrepareSelectedState(CDC* pDC, CDC* pMemDC, CRect* pRect) {
	delete m_pSelected;
	m_pSelected = new CBitmap;
	PaintRgn(pDC, pMemDC, m_pSelected, m_sColor, pRect, TRUE, TRUE);
}

int CColorButton::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	m_bNeedBitmaps = TRUE;
	SetWindowRgn(m_hRgn, TRUE);
	return 0;
}

void CColorButton::PrepareHoverState(CDC* pDC, CDC* pMemDC, CRect* pRect) {
	delete m_pHover;
	m_pHover = new CBitmap;
	PaintRgn(pDC, pMemDC, m_pHover, m_hColor, pRect, TRUE, FALSE);
}

void CColorButton::PrepareDisabledState(CDC* pDC, CDC* pMemDC, CRect* pRect) {
	delete m_pDisabled;
	m_pDisabled = new CBitmap;
	PaintRgn(pDC, pMemDC, m_pDisabled, m_dColor, pRect, FALSE, FALSE);
}

BOOL CColorButton::HitTest(CPoint point) {
	BOOL result = FALSE;

	// Obtain handle to window region.
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hRgn);
	CRect rgnRect;
	GetRgnBox(hRgn, &rgnRect);

	// First check if point is in region bounding rect.
	// Then check if point is in the region in adition to being in the bouding rect.
	result = PtInRect(&rgnRect, point) && PtInRegion(hRgn, point.x, point.y);

	// Clean up and exit.
	DeleteObject(hRgn);
	return result;
}
void CColorButton::Change_Caption(CString nCaption) {
	m_ButtonCaption = nCaption;

	CDC* pDC = GetDC();

	CRect rect;
	GetClientRect(rect);
	if (m_bNeedBitmaps)
		PrepareStateBitmaps(pDC, &rect);
	Re_DrawButton(m_ButtonCaption);
}

void CColorButton::Re_DrawButton(CString nData) {
	//
	CRect rc;
	GetClientRect(&rc);
	//SetWindowText( nData );
	Invalidate();
}

void CColorButton::OnPaint() {
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(rect);

	PrepareStateBitmaps(pDC, &rect);
	DrawButton(pDC, &rect, 0);

	// Do not call CButton::OnPaint() for painting messages
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	if (!m_bMouseDown)
		CheckHover(point);
	CButton::OnMouseMove(nFlags, point);
}
void CColorButton::CheckHover(CPoint point) {
	if (HitTest(point)) {
		if (!m_bCapture) {
			SetCapture();
			m_bCapture = TRUE;
		}
		if (!m_bHover) {
			m_bHover = TRUE;
			RedrawWindow();
		}
	}
	else {
		if (m_bCapture) {
			ReleaseCapture();
			m_bCapture = FALSE;
		}
		m_bHover = FALSE;
		RedrawWindow();
	}
}

void CColorButton::OnLButtonDown(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	m_bMouseDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);

	if (!m_bCapture) {
		SetCapture();
		m_bCapture = TRUE;
	}
}

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonUp(nFlags, point);
	m_bMouseDown = FALSE;
	if (m_bCapture) {
		ReleaseCapture();
		m_bCapture = FALSE;
	}
	CheckHover(point);
}

LRESULT CColorButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	// TODO: Add your specialized code here and/or call the base class
	//if( message == WM_LBUTTONDBLCLK )
	//	message = WM_LBUTTONDOWN;
	return CButton::DefWindowProc(message, wParam, lParam);
}

HBRUSH CColorButton::CtlColor(CDC* pDC, UINT nCtlColor) {
	return m_Brush;	//return NULL;
}

HBRUSH CColorButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CColorButton::OnEraseBkgnd(CDC* pDC) {
	// TODO: Add your message handler code here and/or call default
	return TRUE;//CButton::OnEraseBkgnd(pDC);
}