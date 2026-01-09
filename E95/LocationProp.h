#pragma once
// LocationProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocationProp dialog
#include "SetupProp14Dlg.h"
#include "WaferMap.h"
class CGridPatt;

class CRecipe413;

class CLocationProp : public CDialog {
	int SelPat;  // 0-no grid define, 1- grid1, 2-grid2 [6/24/2010 Yuen]

public:
	// 	CGridPatt *pPatt;
	HWND hWnd;
	
	CRecipe413* pRcp;
	CWaferMap	m_cWaferMap;

	// Construction
public:
	void RefreshDialog();
	BOOL SelectCombo(CComboBox* pBox);
	CLocationProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocationProp)
	enum { IDD = IDD_LOCATIONPROP };
	CEdit	m_cBaselineFile;
	CEdit	m_cMaskFilename;
	CComboBox	m_cWaferSize;
	CComboBox	m_cMapSize;
	float	m_ZPos;
	CString	m_BaselineFile;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocationProp)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLocationProp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonMapSize();
	afx_msg void OnButtonWaferSz();
	afx_msg void OnDefine();
	afx_msg void OnDefine2();
	afx_msg void OnDefine3();
	afx_msg void OnDefine4();
	afx_msg void OnKillfocusBaselinefile();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
