// receipeCreateDlgBase.cpp: implementation of the CRecipeCreateDlgBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "RecipeCreateDlgBase.h"
#include "InfoPanelMain.h"
#include "..\MFC\Keypad.h"
#include "..\Online\MFC\GetPW.h"
#include "..\SRC\Logindatanew.h"
//#include "..\RCP\Phases.h"
//#include "RecipeCreateDlg.h"
#include "..\SYS\CUMMsg.h"

#include "GlobalUIContainer.h"
// #include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecipeCreateDlgBase::CRecipeCreateDlgBase() {
	ID = 0;
	m_Parent = NULL;
	bLock = TRUE;
	bInspect = FALSE;
	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
}

CRecipeCreateDlgBase::CRecipeCreateDlgBase(UINT nIDTemplate, CWnd* pParentWnd)
	: CResizableDialog(nIDTemplate, pParentWnd) {
	ID = 0;
	m_Parent = NULL;
	bLock = TRUE;
	bInspect = FALSE;
	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
}

CRecipeCreateDlgBase::~CRecipeCreateDlgBase() {}

void CRecipeCreateDlgBase::OnClosetab() {
	if (m_Parent) {
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_CLOSETAB, 0, (LPARAM)this);
	}
}

LRESULT CRecipeCreateDlgBase::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Recipe\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CRecipeCreateDlgBase::EnaDlg(int ID, BOOL bEn) {
	(HWND*)GetDlgItem(ID)->EnableWindow(bEn);
}