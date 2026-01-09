#pragma once
// InputFloat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputFloat dialog

class CInputFloat : public CDialog {
	// Construction
public:
	CInputFloat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputFloat)
	enum { IDD = IDD_INPUTFLOAT };
	float	m_Value;
	CString	m_cPrompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputFloat)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputFloat)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
