#pragma once
// SetupProp72Dlg.h : header file
//

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp72Dlg dialog

class CSetupProp72Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp72Dlg(CWnd* pParent = NULL);   // standard constructor

	void SwapMeSet(short next);
	void LocalToRecipe();
	void RecipeToLocal();

	// Dialog Data
		//{{AFX_DATA(CSetupProp72Dlg)
	enum { IDD = IDD_SETUPPROP72 };
	CButton	m_cDianostic;
	CButton	m_cDefinePeaks;
	CButton	m_cSaveVideo;
	CButton	m_cUseWaferAlign;
	CButton	m_cUse;
	CButton	m_cUseMotorFile;
	int		m_SamplingFreq;
	int		m_InterestedDataSet;
	CString	m_MotorFile;
	float	m_StepSize;
	short	m_SearchN;
	int		m_TimeLimt;
	int		m_PointTimeout;
	int		m_FailPoint;
	float	m_BumpOffsetX;
	float	m_BumpOffsetY;
	float	m_EchoTopZPos;
	float	m_ZPosOffset;
	short	m_Exposure;
	//}}AFX_DATA

	float	m_Threshold[2];
	short	m_Level[2];
	short	m_PeakWidth[2];
	short	m_PeakNear[2];
	short	m_PeakSmooth[2];
	short	m_PeakMinimum[2];

	//01312024/yukchiu
	float	m_settlingTime;
	// [05152024 ZHIMING
	float	m_EchoBottomZPos;
	// ]

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp72Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp72Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusMotorfile();
	afx_msg void OnUsemotorfile();
	afx_msg void OnUse();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusRcpSearchStepsize();
	afx_msg void OnKillfocusRcpSearchAround();
	afx_msg void OnKillfocusTimelimit();
	afx_msg void OnKillfocusPointtimeout();
	afx_msg void OnKillfocusFailpoint();
	afx_msg void OnKillfocusBumpoffsetx();
	afx_msg void OnKillfocusBumpoffsety();
	afx_msg void OnUsewaferalign();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnReczpos();
	afx_msg void OnKillfocusZposition();
	afx_msg void OnReczposoff();
	afx_msg void OnKillfocusZpositionoff();
	afx_msg void OnKillfocusExposure();
	afx_msg void OnSavevideo2();
	afx_msg void OnSaverecipe();
	afx_msg void OnPeakcontrol();
	afx_msg void OnDefinepeak();
	afx_msg void OnDianostic();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillfocusSettlingtime();				//01312024/yukchiu
	afx_msg void OnBnClickedRecechobottomzpos();
	afx_msg void OnKillfocusEchobottomzposition();
};