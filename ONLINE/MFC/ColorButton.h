#pragma once
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton {
	// Construction
public:
	CColorButton();

	// Attributes
public:

	// Operations

public:
	// decrare variable
	CBrush    m_Brush;
	BOOL      m_bCapture;
	CDC* m_pMemDC;
	UINT      m_nBorder;
	COLORREF  m_nColor, m_sColor, m_hColor, m_dColor;
	HRGN      m_hRgn;
	CPoint    m_CenterPoint;
	BOOL      m_bNeedBitmaps;

	CBitmap* m_pNormal;
	CBitmap* m_pSelected;
	CBitmap* m_pHover;
	CBitmap* m_pDisabled;
	BOOL      m_bHover;
	CString   m_ButtonCaption;
	UINT      m_nState;
	BOOL      m_bMouseDown;
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CColorButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Re_DrawButton(CString nData);
	BOOL HitTest(CPoint point);
	void CheckHover(CPoint point);
	void Change_Caption(CString nCaption);
	void PrepareDisabledState(CDC* pDC, CDC* pMemDC, CRect* pRect);
	void PrepareHoverState(CDC* pDC, CDC* pMemDC, CRect* pRect);
	void PrepareSelectedState(CDC* pDC, CDC* pMemDC, CRect* pRect);
	void FrameRgn3D(HDC hDC, const HRGN hRgn, BOOL bSunken);
	void DrawButtonCaption(HDC hDC, CRect* pRect, BOOL bEnabled, BOOL bSunken);
	void PaintRgn(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, COLORREF color, CRect* pRect, BOOL bEnabled, BOOL bSunken);
	void PrepareNormalState(CDC* pDC, CDC* pMemDC, CRect* pRect);
	void DrawButton(CDC* pDC, CRect* pRect, UINT state);
	void PrepareStateBitmaps(CDC* pDC, CRect* pRect);
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID);
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID, UINT nBorder, COLORREF nColor, COLORREF sColor, COLORREF hColor, COLORREF dColor);
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
