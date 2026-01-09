#pragma once
// SetupProp83Dlg.h : header file
//

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp83Dlg dialog

class CInfoPanelMain;

class CSetupProp83Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void LocalToRecipe();
	void RecipeToLocal();

	// 	CInfoPanelMain * m_Parent;

	CSetupProp83Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp83Dlg)
	enum { IDD = IDD_SETUPPROP83 };
	CButton	m_cSaveVideo;
	short	m_Spacing;
	short	m_NLines;
	short	m_Exposure1;
	short	m_Exposure2;
	short	m_Exposure3;
	float	m_TrimWidthOuterRadius;
	float	m_ZPos;
	float	m_TrimHeightDistanceInner;
	float	m_TrimHeightDistanceInnerB;
	float	m_TrimHeightDistanceOuter1;
	float	m_TrimHeightDistanceOuter2;
	float	m_TrimHeightZOffset;
	short	m_NMeasureOut;
	short	m_Smooth;
	float	m_TrimWidthInnerRadius;
	int		m_NTrial;
	float	m_SearchSpacing;
	short	m_AutoExposeIn;
	short	m_AutoExposeOut;
	short	m_NMeasureIn;
	float	m_FindTrimThres;
	short	m_SamplePerPoint;
	int		m_THExposure;
	float	m_TrimWidthOuterRadius2;
	float	m_EstNTrimWidth;

	float	m_ContrastLevel;	//20210329
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp83Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp83Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusSpacing();
	afx_msg void OnKillfocusNumberoflines();
	afx_msg void OnKillfocusExposure();
	afx_msg void OnKillfocusExposure3();
	afx_msg void OnKillfocusExposure5();
	afx_msg void OnKillfocusTrimwidthradius();
	afx_msg void OnKillfocusZposition();
	afx_msg void OnReczpos();
	afx_msg void OnKillfocusTrimheightdistance();
	afx_msg void OnKillfocusTrimheightdistance2();
	afx_msg void OnKillfocusTrimheightdistance3();
	afx_msg void OnKillfocusTrimheightzoffset();
	afx_msg void OnKillfocusNmeasurement();
	afx_msg void OnKillfocusSmooth();
	afx_msg void OnKillfocusTrimwidthinnerradius();
	afx_msg void OnKillfocusNtrial();
	afx_msg void OnKillfocusSearchSpacing();
	afx_msg void OnSaverecipe();
	afx_msg void OnHelps();
	afx_msg void OnMeasureouter();
	afx_msg void OnKillfocusAutoexposurein();
	afx_msg void OnKillfocusAutoexposureout();
	afx_msg void OnSavevideo2();
	afx_msg void OnKillfocusNmeasurement2();
	afx_msg void OnKillfocusThreshold2();
	afx_msg void OnKillfocusSampleperpoint();
	afx_msg void OnMethod1();
	afx_msg void OnMethod2();
	afx_msg void OnLocateinneredge();
	afx_msg void OnKillfocusTHExposure();
	afx_msg void OnKillfocusTrimheightdistanceB();
	afx_msg void OnKillfocusTrimwidthradius2();
	afx_msg void OnKillfocusEstwtrimwidth();
	afx_msg void OnLocateinneredge2();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillfocusContrastLevel();
	float m_FindBevelThres;
	afx_msg void OnEnKillfocusThreshold3();
};
