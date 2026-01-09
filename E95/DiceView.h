#pragma once
#include<atlimage.h>


class CRecipe413;


class CSiteItem {
public:
	
	BOOL bSel;
	short n;
	float X, Y, W, H, m_Radius;
	std::vector<CString> Values;
	CSiteItem() {
		n = 0;
		bSel = FALSE;
	}
	void Sel() {
		bSel = TRUE;
	}
	void Desel() {
		bSel = FALSE;
	}
	void Set(short nn, float fX, float fY, float fW, float fH) {
		n = nn;
		X = fX; Y = fY; W = fW; H = fH;
	}
	void Set2(short id, float x, float y, float w, float h, float Radius) {
		n = id;
		X = x;
		Y = y;
		W = w;
		H = h;
		m_Radius = Radius;
		bSel = false;
	}

};

class CDiceView : public CStatic {
	DECLARE_DYNAMIC(CDiceView)

public:
	//CToolTipCtrl m_ToolTip;
	CDiceView();
	virtual ~CDiceView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	COLORREF m_bgColor;
	CPtrList Sites;
	float iX, iY;
	
public:
	void SetSelectAll();
	void ClearCells();

	void RedrawWithData(void);

	BOOL IsPointInRectangle(float xCoords, float yCoords, float topLeftX, float topLeftY, float topRightX, float topRightY, float bottomRightX, float bottomRightY, float bottomLeftX, float bottomLeftY);

	void DrawWithData(CDC* pDC);

	int CalculateDynamicFontSize(CDC* pDC, float rectWidth, float rectHeight);

	CPoint CalculateTextPosition(CSiteItem* p, CSize textSize, float sfX2, float sfY2, int cx, int cy, bool isFirstLine, float rectHeight);


	void AddCellWithData(float cx, float cy, float wd, float ht, short n, float m_Radius, int ltvIdx, int stirIdx, CString rcpName, int precision);

	void LocateAndSelect(float fX, float fY);

	void Show3DMapView();

	BOOL SaveBitmap(CString& path);

	//BOOL SaveBitmap(CString path);


	CString SetDiceMapFolderPath();

	void GetCoor2(POINT& coord, float& fX, float& fY);

	BOOL CaptureToPDF();

	BOOL CaptureCurrentView(CImage& image);



	void ClearSelectAll();
	afx_msg void OnPaint();
	void Redraw(void);
	void Draw(CDC* pDC);
	void AddCell(float cx, float cy, float wd, float ht, short n);
	void Clear(void);
	void Locate(float fX, float fY);
	void GetCoor(POINT& coord, float& fX, float& fY);
	float rad;
	int ltvIdx;
	int stirIdx;
	CString rcpName;
	int precision;// precision of text from recipe
};
