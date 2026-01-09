#pragma once
// Tips.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTips window

class CTips : public CWnd {
	// Construction
public:
	CTips();

	// Attributes
private:
	void Move();

public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTips)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void Show();
	void Hide();
	void Move(CRect& rc);
	CString msg;
	int dy;
	int dx;
	CRect iniRc;
	CPoint pt;
	BOOL bButDown;
	virtual ~CTips();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTips)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
