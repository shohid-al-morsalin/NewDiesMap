#pragma once

class CConfiguration : public CDialog {
	// Construction
public:
	CConfiguration(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CConfiguration)
	enum { IDD = IDD_CONFIGURATION };
	BOOL	m_EchoMicroscope;
	BOOL	m_PatternRec;
	BOOL	m_PMotor;
	BOOL	m_SecsGEM;
	BOOL	m_ThinFilmMicroscope;
	BOOL	m_ThinFilmModule;
	BOOL	m_TrimModule;
	BOOL	m_EchoProbeModule;
	BOOL	m_ThinFilmMotor;
	BOOL	m_EchoTopMotor;
	BOOL	m_StressModule;
	// [11302022 ZHIMING
	BOOL	m_AkusenseProbe;
	// [20240307
	BOOL	m_BowWarp;
	// [04162024
	BOOL	m_Roughness;
	// [05032024
	BOOL	m_EchoBottomMotorMod;
	// [05092024
	BOOL	m_ConfocalTopMotorMod;
	BOOL	m_RoughMotorMod;
	// ]
	BOOL	m_Taiko; // 08092024 Mahedi Kamal

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfiguration)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfiguration)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
