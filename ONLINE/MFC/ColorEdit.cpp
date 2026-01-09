#include "stdafx.h"

#include "ColorEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorEdit::CColorEdit() {
	m_fBold.CreateFont(18, 0, 0, 0,
		FW_BOLD,
		0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_QUALITY | FF_DONTCARE,
		"MS Sans Serif");
	EditColor = EDT_GREEN;
}

CColorEdit::~CColorEdit() {}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) {
	// TODO: Change any attributes of the DC here
	pDC->SetBkColor(RGB(0, 0, 0));
	static CBrush s_brush(RGB(0, 0, 0));
	switch (EditColor) {
	case EDT_GREEN:  pDC->SetTextColor(RGB(0, 255, 0)); break;
	case EDT_BLUE:   pDC->SetTextColor(RGB(0, 0, 255)); break;
	case EDT_RED:    pDC->SetTextColor(RGB(255, 0, 0)); break;
	case EDT_ORANGE: pDC->SetTextColor(RGB(255, 150, 50)); break;
	case EDT_WHITE:  pDC->SetTextColor(RGB(255, 255, 255)); break;
	case EDT_PINK:   pDC->SetTextColor(RGB(255, 125, 125)); break;
	}
	return s_brush;
}

HBRUSH CColorEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CColorEdit::SetCtrlFont() {
	SetFont(&m_fBold);
}

void CColorEdit::SetWhite() {
	EditColor = EDT_WHITE;
}

void CColorEdit::SetBlue() {
	EditColor = EDT_BLUE;
}

void CColorEdit::SetRed() {
	EditColor = EDT_RED;
}

void CColorEdit::SetOrange() {
	EditColor = EDT_ORANGE;
}

void CColorEdit::SetGreen() {
	EditColor = EDT_GREEN;
}

void CColorEdit::SetPink() {
	EditColor = EDT_PINK;
}