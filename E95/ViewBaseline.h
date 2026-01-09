#pragma once
// ViewBaseline.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewBaseline dialog

class CViewBaseline : public CDialog {
	// Construction
public:
	CViewBaseline(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewBaseline)
	enum { IDD = IDD_VIEWBASELINE };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewBaseline)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewBaseline)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
