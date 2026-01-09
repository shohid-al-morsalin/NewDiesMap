#pragma once
// SetupProp43Dlg.h : header file
//

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp43Dlg dialog

class CInfoPanelMain;

class CSetupProp43Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	float	m_ThkMax[MAXTHKVLU];
	float	m_ThkMin[MAXTHKVLU];
	float	m_RI[MAXTHKVLU];

	void SwapMeSet(short next);
	void LocalToRecipe();
	void RecipeToLocal();

	// 	CInfoPanelMain * m_Parent;

	CSetupProp43Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp43Dlg)
	enum { IDD = IDD_SETUPPROP43 };
	CButton	m_cClearRg2;
	CButton	m_cClearRg1;
	CButton	m_cClearTF2;
	CButton	m_cClearTF1;
	CButton	m_cDianostic;
	CButton	m_cThkRange;
	CButton	m_cTakePic;
	short	m_IntegrationTime;
	short	m_Boxcar;
	short	m_Method;
	short	m_nCycle;
	short	m_Expand;
	short	m_Smooth;
	float	m_ZPos;
	int		m_NumberOfMappedPoints;
	short	m_AutoExpose;
	short	m_Exposure;
	short	m_Exposure2;
	float	m_ODOffset;
	float	m_ODOffsetY;
	float	m_STIOffset;
	float	m_STIOffsetY;
	short	m_ZOffset;
	CButton	m_cMeSet;
	int		m_TimeLimit;
	int		m_PointTimeout;
	int		m_FailPoint;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp43Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp43Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaverecipe();
	afx_msg void OnKillfocusFilmmeastime();
	afx_msg void OnKillfocusBoxcar();
	afx_msg void OnKillfocusMethodSp4();
	afx_msg void OnKillfocusNcycle();
	afx_msg void OnKillfocusExpand();
	afx_msg void OnKillfocusSmooth();
	afx_msg void OnRadio1();
	afx_msg void OnRadio10();
	afx_msg void OnRadio12();
	afx_msg void OnRadio3();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnRadio9();
	afx_msg void OnReczpos();
	afx_msg void OnKillfocusZposition();
	afx_msg void OnKillfocusNumberofmappedpoints();
	afx_msg void OnKillfocusExposure3();
	afx_msg void OnKillfocusExposure();
	afx_msg void OnKillfocusExposure2();
	afx_msg void OnTakepictureonmeasure();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnPatset1();
	afx_msg void OnPatset2();
	afx_msg void OnPatset3();
	afx_msg void OnPatset4();
	afx_msg void OnPatset5();
	afx_msg void OnPatset6();
	afx_msg void OnPatset7();
	afx_msg void OnPatset8();
	afx_msg void OnPatset9();
	afx_msg void OnKillfocusOdoffset();
	afx_msg void OnKillfocusOdoffsety();
	afx_msg void OnKillfocusStioffset2();
	afx_msg void OnKillfocusStioffsety();
	afx_msg void OnKillfocusZoffset();
	afx_msg void OnKillfocusFthkminSp4();
	afx_msg void OnKillfocusFthkminSp5();
	afx_msg void OnKillfocusFthkminSp6();
	afx_msg void OnKillfocusFthkminSp7();
	afx_msg void OnKillfocusFthkminSp8();
	afx_msg void OnKillfocusFthkminSp9();
	afx_msg void OnKillfocusFthkmaxSp4();
	afx_msg void OnKillfocusFthkmaxSp5();
	afx_msg void OnKillfocusFthkmaxSp6();
	afx_msg void OnKillfocusFthkmaxSp7();
	afx_msg void OnKillfocusFthkmaxSp8();
	afx_msg void OnKillfocusFthkmaxSp9();
	afx_msg void OnKillfocusRi1();
	afx_msg void OnKillfocusRi2();
	afx_msg void OnKillfocusRi3();
	afx_msg void OnKillfocusRi4();
	afx_msg void OnKillfocusRi5();
	afx_msg void OnKillfocusRi6();
	afx_msg void OnSetexposure();
	afx_msg void OnDianostic();
	afx_msg void OnClearthkrange();
	afx_msg void OnCleartf();
	afx_msg void OnClearthkrange2();
	afx_msg void OnCleartf2();
	afx_msg void OnKillfocusTimelimit();
	afx_msg void OnKillfocusPointtimeout();
	afx_msg void OnKillfocusFailpoint();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
