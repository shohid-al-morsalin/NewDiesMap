#pragma once

class CAdvanceSetup6 : public CDialog {
	// Construction
public:
	CAdvanceSetup6(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvanceSetup6)
	enum { IDD = IDD_ADVANCESETUP6 };
	float	m_StageOriginXEcho;
	float	m_StageOriginYEcho;
	float	m_StageOriginXTF;
	float	m_StageOriginYTF;
	float	m_StagePEcho;
	float	m_StagePTF;
	//}}AFX_DATA

	enum AS6COMMANDS {
		NOCMD, CLEARORIGIN, SAVEPARAM, LOADPARAM, SETLOADPOS,
		SETBGPOS, SETCALIBPOS, SETORIGINECHO, SETCALIBPOSTF, SETORIGINTF, SETPECHO, SETPTF,
		SETCALIBPOSROU, SETORIGINROU
	};
	AS6COMMANDS cmd;
	
	// [05142024 ZHIMING
	float	m_StageOriginXRough;
	float	m_StageOriginYRough;
	float	m_StagePRough;
	// ]

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvanceSetup6)
	afx_msg void OnClearhomexy();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnAdvancexy();
	afx_msg void OnSetbgpos();
	afx_msg void OnSetdppos();
	afx_msg void OnSethomexy();
	afx_msg void OnSetdppos2();
	afx_msg void OnSethomexyrough();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetpthinf();
	afx_msg void OnSetpecho();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
