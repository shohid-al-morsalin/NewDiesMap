#include "stdafx.h"

#include "HistoDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHistoDraw::CHistoDraw() {
	m_bgColor = RGB(255, 255, 255);

	xMax = 360;
	xMin = 300;
	nCountLimit = 48;
	ClearBins();
}

CHistoDraw::~CHistoDraw() {}

BEGIN_MESSAGE_MAP(CHistoDraw, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CHistoDraw::ClearBins() {
	yMax = 0;
	for (int i = 0; i < MAXBINS; i++) {
		Bins[i] = 0;
	}
}

void CHistoDraw::SetRange(float min, float max) {
	xMin = min;
	xMax = max;
	float dx = (xMax - xMin) / float(MAXBINS);
	for (int i = 1; i <= MAXBINS; i++) {
		vBin[i - 1] = i * dx + xMin;
	}
	nCount = 0;
}

void CHistoDraw::Bin(float val) {
	if (nCount > nCountLimit) {
		ClearBins();
		nCount = 0;
	}
	nCount++;
	if (val < xMin) {
		Bins[0]++;
		if (Bins[0] > yMax) {
			yMax = Bins[0];
		}
		return;
	}
	if (val > xMax) {
		int n = MAXBINS - 1;
		Bins[n]++;
		if (Bins[n] > yMax) {
			yMax = Bins[n];
		}
		return;
	}
	for (int i = 0; i < MAXBINS; i++) {
		if (val <= vBin[i]) {
			Bins[i]++;
			if (Bins[i] > yMax) {
				yMax = Bins[i];
			}
			break;
		}
	}
}

void CHistoDraw::OnPaint() {
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
	ReleaseDC(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CHistoDraw::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);

	CDC* pDC = GetDC();
	Draw(pDC);
	ReleaseDC(pDC);
}

void CHistoDraw::Draw(CDC* pDC) {
	int n, on = -1, v = 0, rcwd, rcht;
	CRect rc;

	GetClientRect(&rc);
	rcwd = rc.Width();
	rcht = rc.Height();

	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	pDC->FillSolidRect(rc.left, rc.top, rcwd, rcht, m_bgColor);

	float sx = MAXBINS / float(rcwd);
	float sy = float(rcht) / yMax;
	for (int i = 0; i < rcwd; i++) {
		n = i * sx;
		if (on != n) {
			on = n;
			v = sy * Bins[n];
		}
		if (v > 0) {
			pDC->MoveTo(i, rc.bottom);
			pDC->LineTo(i, rc.bottom - v);
		}
	}
}

void CHistoDraw::GetRange(float& min, float& max) {
	max = xMax;
	min = xMin;
}