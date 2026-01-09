#include "stdafx.h"
#include "e95.h"
#include "ChartWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartWnd::CChartWnd() {
	nData = 0;
	nData2 = 0;
	pData = NULL;
	pData2 = NULL; //Tushar
	Ymax = Ymin = 0;
	Ymax2 = Ymin2 = 0;//Tushar
	isDoublePlot = FALSE;//Tushar
}

CChartWnd::~CChartWnd() {
	if (pData) {
		delete[] pData;
	}

	//Tushar
	if (pData2) {
		delete[] pData2;
	}
}

BEGIN_MESSAGE_MAP(CChartWnd, CStatic)
	//{{AFX_MSG_MAP(CChartWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChartWnd message handlers

void CChartWnd::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectClipRgn(&Rg);

	CBrush br(RGB(220, 225, 230));
	dc.FillRect(rc, &br);

	if (!pData) {
		return;
	}

	int ht = rc.Height();
	float sx = nData / float(rc.Width());
	float sy = ht / float(Ymax - Ymin);

	CPen pen;
	HPEN oldPen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	oldPen = (HPEN)dc.SelectObject(pen);

	dc.MoveTo(0, ht - (*pData - Ymin) * sy);
	for (int x = 0; x < rc.Width(); x++) {
		int n = x * sx;
		dc.LineTo(x, ht - (*(pData + n) - Ymin) * sy);
	}
	dc.SelectObject(oldPen);
	pen.DeleteObject();

	

	if (isDoublePlot == TRUE) {
		//TUSHAR
		CPen pen2;
		pen2.CreatePen(PS_SOLID, 1, RGB(0, 0, 255)); //blue color
		HPEN oldPen2 = (HPEN)dc.SelectObject(pen2);
		float sy2 = ht / (Ymax2 - Ymin2);
		dc.MoveTo(0, ht - (*(pData2)-Ymin2) * sy2);
		for (int x = 0; x < rc.Width(); x++) {
			int n = x * sx;
			dc.LineTo(x, ht - (*(pData2 + n) - Ymin2) * sy2);
		}

		dc.SelectObject(oldPen2);
		pen2.DeleteObject();
	}

	// Do not call CStatic::OnPaint() for painting messages
}

//Tushar

void CChartWnd::Set2(short* pBuf,short* pBuf2, short n) {
	//Use this for ploting 2 lines at same time
	if (!pBuf2) {
		return;
	}
	if (pData2) {
		delete[] pData2;
	}

	pData2 = pBuf2;
	nData2 = n;
	Ymax2 = Ymin2 = *(pBuf2++);

	for (int i = 1; i < n; i++) {
		if (*pBuf2 < Ymin2) {
			Ymin2 = *pBuf2;
		}
		if (*pBuf2 > Ymax2) {
			Ymax2 = *pBuf2;
		}
		pBuf2++;
	}

	if (!pBuf) {
		return;
	}
	if (pData) {
		delete[] pData;
	}

	pData = pBuf;
	nData = n;
	Ymax = Ymin = *(pBuf++);

	for (int i = 1; i < n; i++) {
		if (*pBuf < Ymin) {
			Ymin = *pBuf;
		}
		if (*pBuf > Ymax) {
			Ymax = *pBuf;
		}
		pBuf++;
	}

	isDoublePlot = TRUE;
}
//

void CChartWnd::Set(short* pBuf, short n) {
	if (!pBuf) {
		return;
	}
	if (pData) {
		delete[] pData;
	}

	pData = pBuf;
	nData = n;
	Ymax = Ymin = *(pBuf++);

	for (int i = 1; i < n; i++) {
		if (*pBuf < Ymin) {
			Ymin = *pBuf;
		}
		if (*pBuf > Ymax) {
			Ymax = *pBuf;
		}
		pBuf++;
	}
}