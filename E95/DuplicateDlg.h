#pragma once
// DuplicateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDuplicateDlg dialog

class CDuplicateDlg : public CDialog {
	// Construction
public:
	CDuplicateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDuplicateDlg)
	enum { IDD = IDD_DUPLICATE };
	short	m_Value;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDuplicateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDuplicateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
