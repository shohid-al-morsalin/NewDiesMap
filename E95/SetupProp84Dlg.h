#pragma once
// SetupProp84Dlg.h : header file
//
#include "SetupPropComm.h"
#include "SICam.h"
#include "ImgWnd.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp84Dlg dialog

class CSetupProp84Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp84Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp84Dlg)
	enum { IDD = IDD_SETUPPROP84 };
	CButton	m_cUseWaferAlign;
	CButton	m_cAlgn;
	CImgWnd m_cImgWnd;
	CSICam	m_cVideo;
	float	m_Bevel;
	float	m_CR;
	float	m_CX;
	float	m_CY;
	int		m_Exposure;
	float	m_ZPos;
	short	m_Method;
	float	m_Thres;
	float	m_WaferSize;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp84Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	void RecipeToLocal();
	void LocalToRecipe() {};
	void StartCamera(CCamBase::CAMERAID No);

	// Generated message map functions
	//{{AFX_MSG(CSetupProp84Dlg)
	afx_msg void OnFindcxcy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClearoffset();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol1();
	afx_msg void OnKillfocusWalignexposure();
	afx_msg void OnKillfocusZpos1();
	afx_msg void OnZpos();
	afx_msg void OnExposure();
	afx_msg void OnKillfocusWafersize();
	afx_msg void OnAdjloadpos();
	afx_msg void OnKillfocusMethod();
	afx_msg void OnKillfocusThreshold();
	afx_msg void OnSaverecipe();
	afx_msg void OnUsecentering();
	afx_msg void OnDarkf();
	afx_msg void OnDarkb();
	afx_msg void OnBrightf();
	afx_msg void OnBrightb();
	afx_msg void OnTestrsl();
	//}}AFX_MSG
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
