#pragma once

#include "..\413\EchoScope\ScanLine.h"

// CScanLineChart

class CScanLineChart : public CStatic {
	DECLARE_DYNAMIC(CScanLineChart)

	int WdBk, HtBk;
	CBitmap DrawBitMap;
	BOOL bAllcBMP;

	COLORREF m_bgColor;
	COLORREF m_lnColor1;
	COLORREF m_lnColor2;
	COLORREF m_grColor;

public:
	CScanLine L1, L2;

	CScanLineChart();
	virtual ~CScanLineChart();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Draw(CDC* pDC);
	void DrawContent(CDC& dc);
	void Add2L1(CScanLine* pScanLine);
	void Add2L2(CScanLine* pScanLine);
	void DrawContent(CDC& dc, CScanLine& ScanLine);
};
