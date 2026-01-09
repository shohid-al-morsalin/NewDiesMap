#pragma once
// InputCalibWaferThickness.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputCalibWaferThickness dialog

class CInputCalibWaferThickness : public CDialog {
	// Construction
public:
	CInputCalibWaferThickness(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputCalibWaferThickness)
	enum { IDD = IDD_INPUTCALTHICK };
	float	m_CalibThickness;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputCalibWaferThickness)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputCalibWaferThickness)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
