#pragma once
// SetupProp11Dlg.h : header file
//

#include "..\SRC\ResizableDialog.h"
#include "SetupPropComm.h"
#include "SICam.h"
#include "FPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp11Dlg dialog

class CSetupProp11Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp11Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp11Dlg)
	enum { IDD = IDD_SETUPPROP11 };
	CButton	m_cGoPos2;
	CButton	m_cGoPos1;
	CButton	m_cAlgn;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CSICam	m_cVideo;
	float	m_PosX1;
	float	m_PosX2;
	float	m_PosY1;
	float	m_PosY2;
	long	m_PostPos;
	long	m_OffsetPos;
	float	m_Angle;
	CString	m_Result;
	//}}AFX_DATA

	CFPoint Pos1, Pos2, Pos3;

	void LocalToRecipe() {};
	void RecipeToLocal() {};

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp11Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp11Dlg)
	afx_msg void OnCamerasetting5();
	afx_msg void OnMotioncontrol5();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnSetpos1();
	afx_msg void OnSetpos2();
	afx_msg void OnGopos1();
	afx_msg void OnGopos2();
	afx_msg void OnAlign();
	afx_msg void OnSetalign();
	virtual BOOL OnInitDialog();
	afx_msg void OnFindangle();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnFindcentr();
	//}}AFX_MSG
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};