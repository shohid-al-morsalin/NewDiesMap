#pragma once

#include <string>
#include "wafmap\OGL\GLView.h"
#include<atlimage.h> //20251210_Mohir
#define MAX_CHARS 256

class CWMapView : public CGLView {
protected: // create from serialization only
	CWMapView();
	DECLARE_DYNCREATE(CWMapView)

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

public:
	void DrawCircle(std::vector <Point3D>& Point, float r, float dr, int idx);
	void Circle(std::vector <Point3D>& RawPoints);
	void Square(CRecipe413* pRcp, std::vector <Point3D>& RawPoints);
	void Vertex(float x, float y, float z);
	void GenFakeData(std::vector <Point3D>& RawPoints);
	void GenRealData(CRecipe413* pRcp, std::vector <Point3D>& RawPoints);
	void SetGraphPoints(std::vector <Point3D>& Points);
	void DefaultGraphic();
	void DrawScene();
public:
	void printstring(void* font, char* string);
	virtual void CreateFont(std::string fontName = _T("Times New Roman"), int height = 16);
	void MakeMashPoint(float x, float y, float z);
	void ShowLegend(CRecipe413* pRcp);
	void BuildAxisList();
	void PrintString(std::string str, double x, double y, double z);
	void MakeMashPoint(std::vector <Point3D>& RawPoints, float x, float y);
	void DrawColorBar();

	virtual ~CWMapView();
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
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWaferviewToolExport();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
//	void ExportTo(CString exportPath);
	void ExportToCImage(CString exportPath); //20251210_Mohir
	
	
	void ExportTo(CString exportPath);
	void OnDraw(CDC* pDC);
	CPoint cpt;
	float m_fLog2Device;
	BOOL m_bFitScreen;
	int m_nWaferSize;
#define CELL_SIZE	(2)
};
