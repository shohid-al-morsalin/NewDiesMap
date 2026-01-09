#pragma once
// SetupProp82Dlg.h : header file
//

#include "SICam.h"
#include "SetupPropComm.h"

#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp82Dlg dialog

class CInfoPanelMain;

class CSetupProp82Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp82Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp82Dlg)
	enum { IDD = IDD_SETUPPROP82 };
	//}}AFX_DATA

public:
	void RecipeToLocal();
	void LocalToRecipe();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp82Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp82Dlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
