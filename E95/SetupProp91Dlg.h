#pragma once
// SetupProp91Dlg.h : header file
//

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

#include "WaferMap.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp91Dlg dialog

class CSetupProp91Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void EnabeSpecifiedPlane(BOOL bEn);
	void RecipeToLocal();
	void LocalToRecipe();
	CSetupProp91Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp91Dlg)
	enum { IDD = IDD_SETUPPROP91 };
	CWaferMap	m_cWaferMap;
	CEdit	m_cPoint3;
	CEdit	m_cPoint2;
	CEdit	m_cPoint1;
	short	m_RefPt1;
	short	m_RefPt2;
	short	m_RefPt3;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp91Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp91Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnKillfocusPoint1();
	afx_msg void OnKillfocusPoint2();
	afx_msg void OnKillfocusPoint3();
	afx_msg void OnSaverecipe();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
