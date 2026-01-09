#pragma once
// WaferViewOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaferViewOptions dialog

class CWaferMap;

class CWaferViewOptions : public CDialog {
	// Construction
public:
	CWaferMap* pWaferMap;
	CWaferViewOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaferViewOptions)
	enum { IDD = IDD_WAFERMAPOPTION };
	BOOL	m_cDrawPattern;
	BOOL	m_bDrawPoint;
	BOOL	m_bDrawValues;
	BOOL	m_bDrawText;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaferViewOptions)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaferViewOptions)
	afx_msg void OnDrawpattern();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawpoint();
	afx_msg void OnDrawvalues();
	afx_msg void OnDrawtext();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
