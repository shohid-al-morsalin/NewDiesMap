#pragma once
// RecipeCreateDlg413.h : header file
//

#include "ImgWnd.h"
#include "WaferMap.h"
#include "Recipe413.h"
#include "RecipeCreateDlgBase.h"

/////////////////////////////////////////////////////////////////////////////
// CRecipeCreateDlg413 dialog

class CInfoPanelMain;

class CRecipeCreateDlg413 : public CRecipeCreateDlgBase {
	// Attribute
public:
	BOOL bInit;
	short MeSet, PatSet;
	short MapCol;
	CRecipe413 Rcp;

	// Construction
public:
	void UpdateUI();
	void EnableDlgItems();
	void DisableDlgItems();
	void EnaDisDlgItems(BOOL bEna);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	void ReloadDialog();
	void LockView();
	CRecipeCreateDlg413(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecipeCreateDlg413)
	enum { IDD = IDD_RECIPECREATEDLG413 };
	CButton	m_cMeSetEch;
	CListCtrl	m_cPoint;
	CButton	m_cMeSetPatt;
	CImgWnd	m_cMarkA2;
	CImgWnd	m_cMarkA1;
	CButton	m_cMeSetTF;
	CStatic	m_cRecipeName;
	CWaferMap	m_cWaferMap;
	short	m_Boxcar;
	float	m_RefractiveIndex;
	short	m_IntegrationTime;
	float	m_ZPosTF;
	short	m_Method;
	float	m_FThkMin;
	float	m_FThkMax;
	short	m_NAve;
	float	m_ZPosECH;
	//}}AFX_DATA
	float	m_ThresholdFactor[2];
	short	m_Level[2];
	short	m_PeakWidth[2];
public:
	short m_P[MaXPA];
	short m_F[MaXPA];
	short m_T[MaXPA];
	float m_R[MaXPA];
	float m_H[MaXPA];
	float m_L[MaXPA];
	short m_D[MaXPA];
	float m_DF[MaXPA];
	float m_Off[MaXPA];
	CString m_N[MaXPA];
	CString	m_Calc[MaXPA];

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CRecipeCreateDlg413)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecipeCreateDlg413)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadrecipe();
	afx_msg void OnSaverecipe();
	afx_msg void OnNewrecipe();
	afx_msg void OnHostUpload();
	afx_msg void OnEchoprop();
	afx_msg void OnLocations();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRefresh();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnEnlarge();
	afx_msg void OnSaveastext();
	afx_msg void OnLoadc2c();
	afx_msg void OnRefreshui();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
