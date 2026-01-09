#pragma once
// ProbeDistParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProbeDistParam dialog

class CProbeDistParam : public CDialog {
	// Construction
public:
	CProbeDistParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProbeDistParam)
	enum { IDD = IDD_PROBEDISTTEST };
	float	m_Distance;
	short	m_NSteps;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProbeDistParam)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProbeDistParam)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};