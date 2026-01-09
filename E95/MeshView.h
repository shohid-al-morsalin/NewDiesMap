#if !defined(AFX_MESHVIEW_H__36AF9B34_CD4F_4F38_A634_E690EBAD7CA6__INCLUDED_)
#define AFX_MESHVIEW_H__36AF9B34_CD4F_4F38_A634_E690EBAD7CA6__INCLUDED_

#include "MeshDoc.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeshView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshView view

class CMeshView : public CView
{
public:
	CMeshView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMeshView)

// Attributes
public:
	CMeshDoc *pDoc;
	short nSPL;
	CString m_Title;

	// OpenGL specific
	BOOL SetWindowPixelFormat(HDC hDC,DWORD dwFlags);
	BOOL CreateViewGLContext(HDC hDC);
	HGLRC m_hGLContext;
	int m_GLPixelIndex;
	void SetClearColor(void) {	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f); }
	unsigned char *SnapClient(CSize *pSize);
	
	// Mouse 
	BOOL m_LeftButtonDown;
	BOOL m_RightButtonDown;
	CPoint m_LeftDownPos;
	CPoint m_RightDownPos;
	HCURSOR m_CursorRotation;
	
	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	BOOL m_xyRotation;
	
	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;
	
	float m_xScaling;
	float m_yScaling;
	float m_zScaling;
	
	float m_SpeedTranslation;
	float m_SpeedRotation;
	
	// Colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;
	
	// Animation
	float m_StepRotationX;
	float m_StepRotationY;
	float m_StepRotationZ;
	
	CSize szDIB;
	CSize m_szPage;	
	BITMAPINFO	m_bmi;
	HBITMAP		m_hDib;
	HGDIOBJ		m_hOldDib;
	LPVOID		m_pBitmapBits;
	HDC			m_hOldDC;
	HDC			m_hMemDC;
	HGLRC		m_hOldRC;
	HGLRC		m_hMemRC;

// Operations
public:
	void OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView);
	void OnPrint2(CDC* pDC);
//	BOOL SetDCPixelFormat(HDC hDC, DWORD dwFlags);
	
	// Position, rotation ,scaling
	void InitGeometry(void);
	void GenerateScene();
	void LoadGeometry();
	std::vector<CPoint3D> *  GetDataPtr();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
	virtual void PostNcDestroy();

// Implementation
public:
	BOOL SetOpenGLState(HDC hDC);
	virtual ~CMeshView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMeshView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnProperty();
	afx_msg void OnMap3dAddwireframe();
	afx_msg void OnUpdateMap3dAddwireframe(CCmdUI* pCmdUI);
	afx_msg void OnMap3dColorfromheight();
	afx_msg void OnMap3dFace();
	afx_msg void OnUpdateMap3dFace(CCmdUI* pCmdUI);
	afx_msg void OnMap3dLight();
	afx_msg void OnUpdateMap3dLight(CCmdUI* pCmdUI);
	afx_msg void OnMap3dLine();
	afx_msg void OnUpdateMap3dLine(CCmdUI* pCmdUI);
	afx_msg void OnMap3dSmooth();
	afx_msg void OnUpdateMap3dSmooth(CCmdUI* pCmdUI);
	afx_msg void OnMap3dSubdivision();
	afx_msg void OnMap3dVertex();
	afx_msg void OnUpdateMap3dVertex(CCmdUI* pCmdUI);
	afx_msg void OnMap3dBackgroundcolor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHVIEW_H__36AF9B34_CD4F_4F38_A634_E690EBAD7CA6__INCLUDED_)
