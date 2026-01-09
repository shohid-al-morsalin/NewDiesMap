#pragma once
// SetupProp12Dlg.h : header file
//

#include "SICam.h"
#include "ImgWnd.h"
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp12Dlg dialog

class CInfoPanelMain;

class CSetupProp12Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void SetA();
	void SetB();
	void LocalToRecipe();
	void RecipeToLocal();
	CSetupProp12Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp12Dlg)
	enum { IDD = IDD_SETUPPROP12 };
	CStatic	m_cMessage;
	CImgWnd	m_cImgWnd;
	CListCtrl	m_cList;
	CButton	m_cMatch;
	CButton	m_cCameraA;
	CImgWnd	m_cMarkB;
	CImgWnd	m_cMarkA;
	CSICam	m_cVideo;
	short	m_Exposure;
	short	m_TemplateSize1;
	short	m_TemplateSize2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp12Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp12Dlg)
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();
	virtual BOOL OnInitDialog();
	afx_msg void OnClear2();
	afx_msg void OnClear1();
	afx_msg void OnGotolocation();
	afx_msg void OnCameraa();
	afx_msg void OnMatch();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol3();
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnRadio7();
	afx_msg void OnKillfocusTemplatesize();
	afx_msg void OnKillfocusTemplatesize2();
	afx_msg void OnClear();
	//}}AFX_MSG
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnBlueXHair(WPARAM wP, LPARAM lP);
	long OnGreenXHair(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};