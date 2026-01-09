#pragma once
// LocDefineDice.h : header file
//

#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CLocDefineDice dialog

class CRecipe413;

struct XYPair {
	float x, y;

	XYPair() {
		x = y = 0;
	}
	void Get(CString& str) {
		str.Format("%.3f,%.3f", x, y);
	}
	void Set(float xx, float yy) {
		x = xx; y = yy;
	}
};

class CLocDefineDice : public CDialog {
public:
	CStatic	m_cPos[3];

	static XYPair O, P[3];

public:
	void UpdatePointPos(short n);
	void RefreshDialog();
	CLocDefineDice(CWnd* pParent = NULL);   // standard constructor

// Construction

// Dialog Data
	//{{AFX_DATA(CLocDefineDice)
	enum { IDD = IDD_LOCATIONDICE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocDefineDice)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLocDefineDice)
	afx_msg void OnGoto1();
	afx_msg void OnGoto2();
	afx_msg void OnGoto3();
	afx_msg void OnSetp1();
	afx_msg void OnSetp2();
	afx_msg void OnSetp3();
	afx_msg void OnAddP1();
	afx_msg void OnAddP2();
	afx_msg void OnAddP3();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
