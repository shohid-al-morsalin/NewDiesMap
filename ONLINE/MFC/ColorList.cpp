#include "stdafx.h"
#include "ColorList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorList::CColorList() {
	m_fBold.CreateFont(14, 0, 0, 0,
		FW_BOLD,
		0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_QUALITY | FF_DONTCARE,
		"MS Sans Serif");
}

CColorList::~CColorList() {}

BEGIN_MESSAGE_MAP(CColorList, CListBox)
	//{{AFX_MSG_MAP(CColorList)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorList message handlers

HBRUSH CColorList::CtlColor(CDC* pDC, UINT nCtlColor) {
	static CBrush s_brush(RGB(0, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 255, 0));//BK_COLOR);
	return s_brush;
}

BOOL CColorList::OnEraseBkgnd(CDC* pDC) {
	// TODO: Add your message handler code here and/or call default
	return CListBox::OnEraseBkgnd(pDC);
}

void CColorList::SetCtrlFont() {
	SetFont(&m_fBold);
}

void CColorList::Add_String(CString data) {
	AddString(data);
}