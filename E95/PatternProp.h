#pragma once
// PatternProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatternProp dialog
class CRecipe413;

class CPatternProp : public CDialog {
public:
	CRecipe413* pRcp;

	// Construction
public:
	CPatternProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPatternProp)
	enum { IDD = IDD_PATTERNPROP };
	CStatic	m_cMask2;
	CStatic	m_cMask1;
	short	m_DI;
	short	m_DT;
	short	m_Size;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternProp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPatternProp)
	afx_msg void OnMaketemplate1();
	afx_msg void OnMaketemplate2();
	afx_msg void OnCleartemplate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
