// ViewContentDlgBase.cpp: implementation of the CViewContentDlgBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "ViewContentDlgBase.h"
#include "InfoPanelView.h"

#include "..\SYS\CUMMsg.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewContentDlgBase::CViewContentDlgBase() {
	ID = 0;
	Parent = NULL;

	ViewName = "";
	ViewDir = "";
}

CViewContentDlgBase::CViewContentDlgBase(UINT nIDTemplate, CWnd* pParentWnd)
	: CResizableDialog(nIDTemplate, pParentWnd) {
	ID = 0;
	Parent = NULL;

	ViewName = "";
	ViewDir = "";
}

CViewContentDlgBase::~CViewContentDlgBase() {}

void CViewContentDlgBase::OnClosetab() {
	if (Parent) {
		ShowWindow(SW_HIDE);
		Parent->PostMessage(UM_CLOSETAB, 0, (LPARAM)this);
	}
}

LRESULT CViewContentDlgBase::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("View\n%s", (char*)lP);
	if (pUDev) {
		pUDev->SetCurrentView(msg);
	}
	return 0;
}