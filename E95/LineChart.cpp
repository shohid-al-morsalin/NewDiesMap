// LineChart.cpp : implementation file
//

#include "stdafx.h"
//#include "e95.h"
#include "LineChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineChart

CLineChart::CLineChart() {
	size = 0;
	cpos = 0;
	zoom = 1.0f;
	pX = NULL;
	pY = NULL;
	Ymin = Ymax = 0;

	WdBk = HtBk = 0;
	bAllcBMP = FALSE;

	m_bgColor = RGB(255, 255, 255);
	m_lnColor1 = RGB(0, 0, 255);
	m_lnColor2 = RGB(255, 0, 0);
	m_grColor = RGB(0, 256, 0);
	//m_txtColor = RGB(16,16,16);

	method = 0;

	ClearPos();
}

CLineChart::~CLineChart() {}

BEGIN_MESSAGE_MAP(CLineChart, CStatic)
	//{{AFX_MSG_MAP(CLineChart)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineChart message handlers

void CLineChart::OnPaint() {
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
	ReleaseDC(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CLineChart::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);
	Invalidate();
}

void CLineChart::Draw(CDC* pDC) {
	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	HBITMAP oldBmp = NULL;
	HPEN oldPen = NULL;

	CDC dcMem;
	CPen pen1;
	CPen pen2;
	CPen pen3;
	int i, Wd, Ht;

	Wd = rc.right;
	Ht = rc.bottom;

	if ((Wd != WdBk) || (Ht != HtBk)) {
		if (bAllcBMP) {
			DrawBitMap.DeleteObject();
		}
		DrawBitMap.CreateCompatibleBitmap(pDC, Wd, Ht);
		bAllcBMP = TRUE;
		WdBk = Wd; HtBk = Ht;
	}
	if (!dcMem.CreateCompatibleDC(pDC)) return;
	oldBmp = (HBITMAP)dcMem.SelectObject(DrawBitMap);

	dcMem.FillSolidRect(rc.left, rc.top, Wd, Ht, m_bgColor);

	int rangeX = rc.right - rc.left;
	int rangeY = rc.bottom - rc.top;
	int rX = size * zoom;
	int rY = 4100;
	int rYmin = 0;
	if (method == 1) {
		rYmin = 1850;
		rY = 4250;
	}

	short Mtd = method;
	unsigned short* pYY = pY;
	if (pX) {
		if (*(pX + 16) > *(pX + 17)) {
			// reverse data set [11/17/2011 Administrator]
			Mtd += 16;
			pYY += size;
		}
	}

	int x = cpos - rX / 2;
	if (x < 0) {
		x = 0;
	}
	if (x >= size) {
		x = size - rX / 2 - 1;
	}
	float sf = rX / (float(rangeX));
	float sy = rangeY / float(rY - rYmin);

	// Draw Data 1 top probe signal [3/27/2010 Valued Customer]
	if (pYY && size) {
		pen1.CreatePen(PS_SOLID, 1, m_lnColor1);
		oldPen = (HPEN)dcMem.SelectObject(pen1);

		switch (Mtd) {
		case 0:
			dcMem.MoveTo(0, rangeY - *(pYY + x) * sy);
			for (i = 1; i < rangeX; i++) {
				int v = (*(pYY + x + int(sf * i))) * sy;
				dcMem.LineTo(i, rangeY - v);
			}
			break;
		case 16:
			dcMem.MoveTo(0, rangeY - *(pYY - x) * sy);
			for (i = 1; i < rangeX; i++) {
				int v = (*(pYY - (x + int(sf * i))) * sy);
				dcMem.LineTo(i, rangeY - v);
			}
			break;
		case 1:
			dcMem.MoveTo(0, rangeY - *(pYY + x) * sy);
			for (i = 1; i < rangeX; i++) {
				int v = *(pYY + x + int(sf * i));
				v -= rYmin;
				v *= sy;
				dcMem.LineTo(i, rangeY - v);
			}
			break;
		case 17:
			dcMem.MoveTo(0, rangeY - *(pYY - x) * sy);
			for (i = 1; i < rangeX; i++) {
				int v = *(pYY - (x + int(sf * i)));
				v -= rYmin;
				v *= sy;
				dcMem.LineTo(i, rangeY - v);
			}
			break;
		}

		dcMem.SelectObject(oldPen);
		pen1.DeleteObject();
	}

	// idx = x + sf * xx
	pen1.CreatePen(PS_SOLID, 1, m_lnColor1);
	oldPen = (HPEN)dcMem.SelectObject(pen1);
	for (i = 0; i < MAXLINEPOS; i++) {
		int xx = (Pos1[i] - x) / sf;
		if ((xx > 0) && (xx < rangeX)) {
			dcMem.MoveTo(xx, 0);
			dcMem.LineTo(xx, rangeY);
		}
	}
	pen1.DeleteObject();
	pen2.CreatePen(PS_SOLID, 1, m_grColor);
	dcMem.SelectObject(pen2);
	for (i = 0; i < MAXLINEPOS; i++) {
		int xx = (Pos2[i] - x) / sf;
		if ((xx > 0) && (xx < rangeX)) {
			dcMem.MoveTo(xx, 0);
			dcMem.LineTo(xx, rangeY);
		}
	}
	pen2.DeleteObject();
	pen3.CreatePen(PS_SOLID, 1, m_lnColor2);
	dcMem.SelectObject(pen3);
	for (i = 0; i < MAXLINEPOS; i++) {
		int xx = (Pos3[i] - x) / sf;
		if ((xx > 0) && (xx < rangeX)) {
			dcMem.MoveTo(xx, 0);
			dcMem.LineTo(xx, rangeY);
		}
	}
	pen3.DeleteObject();
	dcMem.SelectObject(oldPen);

	pDC->BitBlt(0, 0, Wd, Ht, &dcMem, 0, 0, SRCCOPY);

	if (oldBmp) dcMem.SelectObject(oldBmp);
	dcMem.DeleteDC();
	oldBmp = NULL;
}

void CLineChart::ClearPos() {
	for (int i = 0; i < MAXLINEPOS; i++) {
		Pos1[i] = Pos2[i] = Pos3[i] = 0;
	}
}