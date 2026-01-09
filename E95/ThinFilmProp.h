#pragma once
// ThinFilmProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThinFilmProp dialog
class CRecipe413;

class CThinFilmProp : public CDialog {
public:
	CRecipe413* pRcp;

	// Construction
public:
	CThinFilmProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThinFilmProp)
	enum { IDD = IDD_THINFILMPROP };
	short	m_Boxcar;
	float	m_RefractiveIndex;
	short	m_IntegrationTime;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThinFilmProp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CThinFilmProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};