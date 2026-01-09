#pragma once

#include <string>
#include "wafmap\OGL\GLViewStatic.h"

#define MAX_CHARS 256

class CGlut2DMapView : public CGLViewStatic {
public: // create from serialization only
	CGlut2DMapView();
	DECLARE_DYNCREATE(CGlut2DMapView)

public:
	int inc = 0;
	//	CWMapDoc* GetDocument();

public:
	int nDataIdx = 0;

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

public:
	void DrawCircle(std::vector <Point3D>& Point, float r, float dr, int idx);
	void Circle(std::vector <Point3D>& RawPoints);
	void Square(CRecipe413* pRcp, std::vector <Point3D>& RawPoints);
	void Vertex(float x, float y, float z);
	//void DrawWaferBoundary();
	void GenFakeData(std::vector <Point3D>& RawPoints);
	void GenRealData(CRecipe413* pRcp, std::vector <Point3D>& RawPoints);
	void SetGraphPoints(std::vector <Point3D>& Points);
	void DefaultGraphic();
	//void SetupTopView();
	void DrawScene();
	void SetDataColumn(int nColumn);
	//void DrawWaferMap2D();
public:
	void printstring(void* font, char* string);
	virtual void CreateFont(std::string fontName = _T("Times New Roman"), int height = 16);
	void MakeMashPoint(float x, float y, float z);
	void ShowLegend(CRecipe413* pRcp);
	void BuildAxisList();
	void PrintString(std::string str, double x, double y, double z);
	void MakeMashPoint(std::vector <Point3D>& RawPoints, float x, float y);
	void DrawColorBar();

	virtual ~CGlut2DMapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	GLYPHMETRICSFLOAT m_GlyphInfo[MAX_CHARS];

protected:
	CMenu* m_pPopUpMenu = NULL;
	double m_fZMin = 0, m_fZMax = 0;
	double m_Rad = 0;
	int nLevelAdjustment = 0;
	UINT m_hOldFont = 0;

protected:
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWaferviewToolExport();
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	void ExportTo(CString exportPath);

	//void CreateDisplayList();

	CPoint cpt;
	float m_fLog2Device;
	BOOL m_bFitScreen;
	int m_nWaferSize;

	// Ensure no rotation or elevation
	//float m_fRotateX = 0.0f;   // elevation
	//float m_fRotateY = 0.0f;   // azimuth
	//float m_fZoom = 1.0f;
	//// Translation (pan) — start centered
	//float m_xTrans = 0.0f;
	//float m_yTrans = 0.0f;
	//float m_zTrans = 0.0f;

	bool m_bDataChanged;  // Flag to indicate data changed
	bool m_bDisplayListValid;  // Flag to track if display list is up-to-date
	int m_nLastDataIdx;  // Track last data index
#define CELL_SIZE	(2)
};
