#pragma once
// SelectResol.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectResol dialog

class CSelectResol : public CDialog {
	// Construction
public:
	CSelectResol(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectResol)
	enum { IDD = IDD_SELECTRESOL };
	int	m_Height;
	int	m_Width;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectResol)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectResol)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};