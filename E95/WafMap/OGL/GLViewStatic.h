#pragma once

#include <vector>
#include <string>
#include <gl\gl.h>
#include <gl\glu.h>

#include "BaseData.h"
// using namespace std;

class CRecipe413;
class CDIBSurface;

class CGLViewStatic : public CView {
protected:
	CGLViewStatic();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGLViewStatic)

	//======== New data
public:
	long	m_BkClr;	// Background color
	HGLRC	m_hRC;		// Rendering context OpenGL
	HDC		m_hdc;		// Windows device context
	GLfloat	m_AngleX;	// Rotation angle (around X-axis)
	GLfloat m_AngleY;	// Rotation angle (around Y-axis)
	GLfloat	m_AngleView;	// Perspective angle
	//	GLfloat	m_fRangeX;	// Graphics dimension (along X-axis)
	//	GLfloat m_fRangeY;	// Graphics dimension (along Y-axis)
	//	GLfloat m_fRangeZ;	// Graphics dimension (along Z-axis)
	GLfloat	m_dx;		// Displacement quantum (along X-axis)
	GLfloat m_dy;		// Displacement quantum (along Y-axis)
	GLfloat	m_xTrans;	// Displacement (along X-axis)
	GLfloat m_yTrans;	// Displacement (along Y-axis)
	GLfloat m_zTrans;	// Displacement (along Z-axis)
	GLenum	m_FillMode;	// Polygon filling mode
	BOOL	m_bCaptured;	// Mouse capture flag
	BOOL	m_bRightButton;	// Right mouse button flag
	BOOL	m_bQuad;	// Flag of using GL_QUAD (instead of GL_QUAD_STRIP)
	CPoint	m_pt;		// Current mouse position
	GLfloat m_zScale;	// Z - height scaling factor
	//	UINT	m_xSize;	// Current client window sixe (along X-axis)
	//	UINT	m_zSize;	// Current client window sixe (along -axis)
	int	m_LightParam[11];	// Graphics dimension (along X-axis)

	short m_GridSize;
	std::vector <Point3D> MashPoints;

	GLfloat cr[26], cg[26], cb[26];

	// Attributes
public:
	BOOL bInit;
	BOOL m_bSmooth;
	int m_nFontBase;

	CRecipe413* pRcp = NULL;

	CSize sizeBitmap;
	CDIBSurface* pDIBSurf;

public:
	virtual void DrawScene();
	virtual UINT GetPropDlgID();
	virtual void DefaultGraphic();

public:
	virtual void ShowLegend(CRecipe413* pRcp);
	virtual void CreateFont(std::string fontName = _T("Times New Roman"), int height = 16);
	void DeleteFont();
	BOOL m_bShowLegend;
	void Polar2Cart(float& x, float& z);
	void Cart2Polar(float& r, float& a);
	BOOL SetDCPixelFormat(HDC hDC, DWORD dwFlags);
	void GetLightParams(int* pPos);
	void SetLightParam(short lp, int nPos);
	void LimitAngles();
	void SetLight();
	void SetBkColor();

public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

protected:
	virtual ~CGLViewStatic();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOptionFill();
	afx_msg void OnUpdateOptionFill(CCmdUI* pCmdUI);
	afx_msg void OnOptionQuad();
	afx_msg void OnUpdateOptionQuad(CCmdUI* pCmdUI);
	afx_msg void OnOptionBackground();
	afx_msg void OnOptionProperties();
	afx_msg void OnOptionSmooth();
	afx_msg void OnUpdateOptionSmooth(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

#define BEGIN_GL	bool bNoRC=false; if(!::wglGetCurrentContext()) { bNoRC=true; ::wglMakeCurrent(m_hDC, m_hRC); }
#define END_GL	if(bNoRC) ::wglMakeCurrent(m_hDC, NULL);
