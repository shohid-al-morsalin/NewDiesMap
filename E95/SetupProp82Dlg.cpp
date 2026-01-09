// SetupProp82Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp82Dlg.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp82Dlg dialog

CSetupProp82Dlg::CSetupProp82Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp82Dlg::IDD, pParent), CSetupPropComm(pParent) {
	//{{AFX_DATA_INIT(CSetupProp82Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

// 	ActiveCamera = ECHOCAMERA;
}

void CSetupProp82Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp82Dlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp82Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp82Dlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp82Dlg message handlers

long CSetupProp82Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		bTabSelected = FALSE;
	}
	return 0;
}

long CSetupProp82Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		bTabSelected = TRUE;
	}

	return 0;
}

void CSetupProp82Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp82Dlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

BOOL CSetupProp82Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp82Dlg::LocalToRecipe() {}

void CSetupProp82Dlg::RecipeToLocal() {}