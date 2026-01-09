#pragma once

class CDefineRadial : public CDialog {
	// Construction
public:
	CDefineRadial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineRadial)
	enum { IDD = IDD_RADIAL };
	short	m_NoLines;
	short	m_PointPerDiameter;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineRadial)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineRadial)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
