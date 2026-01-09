#pragma once
// LineChart.h : header file
//

#define MAXLINEPOS	64

/////////////////////////////////////////////////////////////////////////////
// CLineChart window

class CLineChart : public CStatic {
	int WdBk, HtBk;
	CBitmap DrawBitMap;
	BOOL bAllcBMP;

	COLORREF m_bgColor;
	COLORREF m_lnColor1;
	COLORREF m_lnColor2;
	COLORREF m_grColor;
	//	COLORREF m_txtColor;

	// Construction
public:
	CLineChart();

	// Attributes
public:
	short method;
	int* pX, size, cpos;
	float zoom;  // express in percentage [9/24/2011 Administrator]
	unsigned short* pY;
	unsigned short Ymin, Ymax;
	int Pos1[MAXLINEPOS]; // Position markers [9/25/2011 Administrator]
	int Pos2[MAXLINEPOS]; // Position markers [9/25/2011 Administrator]
	int Pos3[MAXLINEPOS]; // Position markers [9/25/2011 Administrator]
// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CLineChart)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void ClearPos();
	void Draw(CDC* pDC);
	virtual ~CLineChart();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineChart)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
