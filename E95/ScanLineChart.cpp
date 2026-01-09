// ScanLineChart.cpp : implementation file
//

#include "stdafx.h"
//#include "E95.h"
#include "ScanLineChart.h"
#include "..\413\EchoScope\ScanPoint.h"

// CScanLineChart

IMPLEMENT_DYNAMIC(CScanLineChart, CStatic)

CScanLineChart::CScanLineChart() {
	WdBk = HtBk = 0;
	bAllcBMP = FALSE;

	m_bgColor = RGB(255, 255, 255);
	m_lnColor1 = RGB(0, 0, 255);
	m_lnColor2 = RGB(255, 0, 0);
	m_grColor = RGB(0, 256, 0);
}

CScanLineChart::~CScanLineChart() {}

BEGIN_MESSAGE_MAP(CScanLineChart, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CScanLineChart message handlers

void CScanLineChart::OnPaint() {
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
	ReleaseDC(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CScanLineChart::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);
	Invalidate();
}

void CScanLineChart::Draw(CDC* pDC) {
	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	HBITMAP oldBmp = NULL;
	HPEN oldPen = NULL;
	CDC dcMem;
	int Wd, Ht;

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

	DrawContent(dcMem);

	pDC->BitBlt(0, 0, Wd, Ht, &dcMem, 0, 0, SRCCOPY);

	if (oldBmp) dcMem.SelectObject(oldBmp);
	dcMem.DeleteDC();
}

void CScanLineChart::DrawContent(CDC& dc) {
	DrawContent(dc, L1);
	DrawContent(dc, L2);
}

void CScanLineChart::Add2L1(CScanLine* pScanLine) {
	if (!pScanLine) {
		return;
	}
	L1.Dup(pScanLine);
	L1.CalcMaxMin();
}

void CScanLineChart::Add2L2(CScanLine* pScanLine) {
	if (!pScanLine) {
		return;
	}
	L2.Dup(pScanLine);
	L2.CalcMaxMin();
}

void CScanLineChart::DrawContent(CDC& dc, CScanLine& SL) {
	int i;
	CRect rc;
	CPen pen1;
	HPEN oldPen = NULL;

	GetClientRect(&rc);

	float sfx = rc.Width() / float(SL.MaxMinX.iMax - SL.MaxMinX.iMin);
	float sfy = rc.Height() / float(SL.MaxMinY1.iMax - SL.MaxMinX.iMax);

	pen1.CreatePen(PS_SOLID, 1, m_lnColor1);
	oldPen = (HPEN)dc.SelectObject(pen1);

	CScanPoint* p = SL.pData;
	if (p) {
		dc.MoveTo(sfx * (p->x - SL.MaxMinX.iMin), sfy * (p->y1 - SL.MaxMinY1.iMin));
		p++;
		for (i = 1; i < SL.Idx; i++) {
			dc.LineTo(sfx * (p->x - SL.MaxMinX.iMin), sfy * (p->y1 - SL.MaxMinY1.iMin));
			p++;
		}
	}
	dc.SelectObject(oldPen);
	pen1.DeleteObject();
}