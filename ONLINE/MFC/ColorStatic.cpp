#include "stdafx.h"

#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"

#include "ColorStatic.h"

extern UINT	TimeFormat;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic() {
	m_fBold.CreateFont(15, 0, 0, 0,
		FW_BOLD,
		0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_QUALITY | FF_DONTCARE,
		"Verdana");
	//"Tahoma");
	//"MS Sans Serif" );
}

CColorStatic::~CColorStatic() {}

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_INITMENU()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor) {
	// TODO: Change any attributes of the DC here
	static CBrush s_brush(RGB(0, 0, 255));
	// 223 , 223 , 255 ) );
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 255, 0));
	// BK_COLOR);
	return s_brush;
}

void CColorStatic::OnTimer(UINT nIDEvent) {
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1) {
		/*		COleDateTime dwTime = COleDateTime::GetCurrentTime();
		CString dwVal("");
		dwVal.Format("%0.2d:%0.2d:%0.2d",
		dwTime.GetHour() ,
		dwTime.GetMinute(),
			dwTime.GetSecond()); */
		SYSTEMTIME CurrTime;
		CString dwVal("");
		::GetLocalTime(&CurrTime);
		if (TimeFormat == 1) {
			dwVal.Format("%0.4d/%0.2d/%0.2d %0.2d:%0.2d:%0.2d",//:%0.2d",
				CurrTime.wYear,
				CurrTime.wMonth,
				CurrTime.wDay,
				CurrTime.wHour,
				CurrTime.wMinute,
				CurrTime.wSecond
				// CurrTime.wMilliseconds/10
			);
		}
		else {
			dwVal.Format("%0.2d/%0.2d/%0.2d %0.2d:%0.2d:%0.2d",
				CurrTime.wYear % 100,
				CurrTime.wMonth,
				CurrTime.wDay,
				CurrTime.wHour,
				CurrTime.wMinute,
				CurrTime.wSecond
			);
		}
		SetWindowText(dwVal);
	}
	CStatic::OnTimer(nIDEvent);
}

void CColorStatic::OnInitMenu(CMenu* pMenu) {
	CStatic::OnInitMenu(pMenu);
	// TODO: Add your message handler code here
}

void CColorStatic::Setting_Font() {
	SetFont(&m_fBold);
}

void CColorStatic::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	// Generate event to get time from host
	CSharedMemQue	MemQue;
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	pEvt->Set(EVT_OTHER, EVT_REQUEST_TIME_FROM_HOST); // Equip Terminal Service Msg to Host
	MemQue.OpenMemory("EVTMSG");
	MemQue.WriteQue(pEvt);
	MemQue.CloseMemory();
	CStatic::OnLButtonDblClk(nFlags, point);
}