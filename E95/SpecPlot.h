#pragma once
// SpecPlot.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecPlot window

#include "..\413\OceanOp\Spec.h"
#include "..\413\OceanOp\OceanBase.h"

#define SPMAX			7681

class CSpecPlot : public CStatic {
	// Construction
public:
	CSpecPlot();

	// Attributes
public:
	COLORREF m_bgColor2;	// background
	COLORREF m_lnColor1;	// theoretical curve
	COLORREF m_lnColor2;	// measured curve
	COLORREF m_lnColor3;	// grid lines
	COLORREF m_lnColor4;	// baseline [6/17/2011 C2C]
	COLORREF m_lnColor5;	// reference
	COLORREF m_lnColor6;	// max bar
	COLORREF m_lnColorTxt;	// text

	BOOL bFileOpen;
	BOOL bFilter;
	BOOL bSaveBitmap;

	CSpec RSpec;

	short BmpNo;
	CString SpecName;

	int method;
	POINT	Pnts[SPMAX];		// Used in Display only, allocate extra memory [7/21/2009 Valued Customer]

	BOOL bDrawBackgn, bDrawRef;

	CString msgStr;

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSpecPlot)
		//}}AFX_VIRTUAL

	// Implementation
public:
	float GetZoom(short N);
	void SaveBitmap2(CString Name, CBitmap& Bmp);
	HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal);
	void SaveBitmapA(CString SpecName, short n);
	void SaveBitmap(CString Name, CBitmap& Bmp);
	void DrawGrid(CDC* dcMem, CRect rc);
	void DrawHisto(CDC* dcMem, CRect rc, CSpec* pRSpec);
	void Refresh();
	void DrawSpectrum(CDC* dcMem, CRect rc, CSpec* pSpec, BOOL bDrawLabel, COLORREF& co);
	void Draw(CDC* pDC);
	virtual ~CSpecPlot();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpecPlot)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
