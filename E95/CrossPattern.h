#pragma once

#include "resource.h"
#include "Pattern.h"

class CCrossPattern/* : public CPattern*/
{
public:
	CCrossPattern();
	virtual ~CCrossPattern();

	CPattern* pPatt;
	// 	CPattern dmy;

	//protected:
	float m_fXCenter;
	float m_fYCenter;
	float m_fXPitch;
	float m_fYPitch;
	float m_fXMax;
	float m_fYMax;

public:
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	CCrossPattern& operator = (CCrossPattern& CrossPattern);

	virtual void Serialize(CArchive& ar);
	virtual BOOL SetUp(CWnd* pWnd);
	virtual BOOL Generate(float);
	virtual BOOL Generate2(int, int, float, float, float, float, float);
	virtual void DrawPattern(CDC* pDC, const CRect& rect, float fRatio);
};

/////////////////////////////////////////////////////////////////////////////
// CCrossPatternDlg dialog

class CCrossPatternDlg : public CDialog {
	// Construction
public:
	CCrossPatternDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCrossPatternDlg)
	enum { IDD = IDD_DEF_CROSS_PAT };
	float	m_XCenter;
	float	m_XPitch;
	float	m_YPitch;
	float	m_YCenter;
	float	m_fXMax;
	float	m_fYMax;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrossPatternDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCrossPatternDlg)
	afx_msg void OnUseHolePitch();
	afx_msg void OnCenteryBut();
	afx_msg void OnMaxyBut();
	afx_msg void OnXcenterBut();
	afx_msg void OnXmaxBut();
	afx_msg void OnXpitchBut();
	afx_msg void OnYpitchBut();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
