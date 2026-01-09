#pragma once
// EditToolBar.h : header file
//

#define IDC_ALARMEDIT   7000

/////////////////////////////////////////////////////////////////////////////
// CEditToolBar window

class CEditToolBar : public CToolBar {
	// Construction
public:
	CEditToolBar();

public:
	CEdit  m_Edit;
	CFont  m_Font;
	HWND   m_lhWnd;
	CBrush m_Brush;
	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEditToolBar)
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP, UINT nID = AFX_IDW_TOOLBAR);
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, CRect rcBorders = CRect(0, 0, 0, 0), UINT nID = AFX_IDW_TOOLBAR);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL DrawEdit();
	virtual ~CEditToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT ReplyAlarm(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
