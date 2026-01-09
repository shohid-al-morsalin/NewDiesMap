#pragma once
// EchoDraw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEchoDraw window

class CScanLine;

class CEchoDraw : public CStatic {
	// 	BOOL bSimu;
	CBitmap bmp;
	BOOL bAllcBMP;
	int WdBk, HtBk;

	short DrawMethod;
	BOOL bSaveChart;

	COLORREF m_bgColor;
	COLORREF m_lnColor1;
	COLORREF m_lnColor2;
	COLORREF m_grColor;
	COLORREF m_txtColor;

	// Construction
public:
	CEchoDraw();

	// Attributes
public:
	BOOL bReal;
	int* pY1, * pY2, * pX;
	CScanLine* pSLn1, * pSLn2;

	int pointsCnt;
	short ChartType, mode;
	double XMax, XMin;
	double YMax, YMin;
	short direc;  // 1 or -1 [7/19/2010 C2C]
	CString	m_ChartName;

	// Operations
public:
	void SaveBitmap(CString Name, CBitmap& bmp);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEchoDraw)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void Draw(CDC* pDC);
	// 	void Simulate();
	void SaveChart(CString name);
	void Refresh();
	void SetData1(CScanLine* pL1, CScanLine* pL2, double& YMin, double& YMax, short ChartType, short mode = 0xFF);
	void SetData0(int* mpX, int* mpY1, int* mpY2, int mpointsCnt, double mXMin, double mXMax, double mYMin, double mYMax,
		short mChartType, short mmode, short mDirc);
	void Draw0(CDC* pDC);
	void Draw1(CDC* pDC);
	virtual ~CEchoDraw();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEchoDraw)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
