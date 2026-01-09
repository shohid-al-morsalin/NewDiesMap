#pragma once

class CAdvanceSetup : public CDialog {
	// Construction
public:
	enum ASTYPE {
		LOADPOS, CALIBECHO, CALIBTF, CALIBBG,
		CALIBROU
	};
	ASTYPE Type;
	CAdvanceSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvanceSetup)
	enum { IDD = IDD_ADVANCESETUP };
	CStatic	m_cNotice;
	float	m_XPos;
	float	m_YPos;
	float	m_ZPos;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvanceSetup)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvanceSetup)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
