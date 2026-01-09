#pragma once
// SetupProp52Dlg.h : header file
//

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp52Dlg dialog

class CSetupProp52Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void LocalToRecipe();
	void RecipeToLocal();
	CSetupProp52Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp52Dlg)
	enum { IDD = IDD_SETUPPROP52 };
	CButton	m_cClear2;
	CButton	m_cClear1;
	CButton	m_cSetting;
	short	m_DI;
	short	m_DT;
	float	m_MatchGOF;
	short	m_MResolution;
	short	m_Depth;
	float	m_Distance;
	float	ECHdX;
	float	ECHdY;
	float	TFdX;
	float	TFdY;
	float	m_StepSize;
	int		m_SearchN;
	BOOL	m_FastMatch;
	CButton	m_cInitialExposureAfterFocus2;
	CButton	m_cInitialFocus2;
	CButton	m_InitialExposureAfterFocus;
	CButton	m_cInitialFocus;
	CButton	m_cInitialExposeB4Focus;
	CButton	m_cMatchAtOffset;
	float	m_STIOffset;
	float	m_STIOffsetY;
	float	m_ODOffset;
	float	m_ODOffsetY;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp52Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp52Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusMatchdi();
	afx_msg void OnKillfocusMatchdt();
	afx_msg void OnKillfocusMatchgof();
	afx_msg void OnKillfocusMatchresolution();
	afx_msg void OnKillfocusDepth();
	afx_msg void OnKillfocusDistance();
	afx_msg void OnPatmethod1();
	afx_msg void OnPatmethod2();
	afx_msg void OnKillfocusOffsetx2();
	afx_msg void OnKillfocusOffsety2();
	afx_msg void OnKillfocusOffsetx();
	afx_msg void OnKillfocusOffsety();
	afx_msg void OnKillfocusRcpSearchStepsize();
	afx_msg void OnKillfocusRcpSearchAround();
	afx_msg void OnFastmatch();
	afx_msg void OnPatset1();
	afx_msg void OnPatset2();
	afx_msg void OnPatset3();
	afx_msg void OnPatset4();
	afx_msg void OnPatset5();
	afx_msg void OnPatset6();
	afx_msg void OnPatset7();
	afx_msg void OnPatset8();
	afx_msg void OnPatset9();
	afx_msg void OnSaverecipe();
	afx_msg void OnClearsetting();
	afx_msg void OnClearsetting2();
	afx_msg void OnPatmethod4();
	afx_msg void OnInitialexposurebeforefocus();
	afx_msg void OnInitialfocus();
	afx_msg void OnInitialexposureafterfocus();
	afx_msg void OnInitialfocus2();
	afx_msg void OnInitialexposureafterfocus2();
	afx_msg void OnMatchatoffset();
	afx_msg void OnKillfocusOdoffset();
	afx_msg void OnKillfocusOdoffsety();
	afx_msg void OnKillfocusStioffset();
	afx_msg void OnKillfocusStioffsety();
	afx_msg void OnPatmethod5();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
