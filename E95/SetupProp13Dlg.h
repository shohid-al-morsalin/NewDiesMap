#pragma once
// SetupProp13Dlg.h : header file
//

#include "SICam.h"
#include "ImgWnd.h"
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp13Dlg dialog

class CSetupProp13Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	BOOL bInner;

	void LocalToRecipe();
	void RecipeToLocal();
	CSetupProp13Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp13Dlg)
	enum { IDD = IDD_SETUPPROP13 };
	CButton	m_cCameraa;
	CListCtrl	m_cList;
	CStatic	m_cMessage;
	CImgWnd	m_cImgWnd;
	CSICam	m_cVideo;
	CString	m_Result;
	int		m_THExposure;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp13Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp13Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCameraa();
	afx_msg void OnFind();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol3();
	afx_msg void OnClear();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnKillfocusTHExposure();
	//}}AFX_MSG
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
