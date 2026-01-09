#pragma once
// SetupProp1Dlg.h : header file
//

#include "SICam.h"
#include "ImgWnd.h"
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp1Dlg dialog

class CSetupProp1Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp1Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp1Dlg)
	enum { IDD = IDD_SETUPPROP1 };
	CSICam	m_cVideo;
	CStatic	m_cMessage;
	//}}AFX_DATA

	void LocalToRecipe();
	void RecipeToLocal();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp1Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp1Dlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
};