// ViewContentDlgBase.h: interface for the CViewContentDlgBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\Sys\Var.h"
#include "..\SRC\ResizableDialog.h"

class CInfoPanelView;

class CViewContentDlgBase : public CResizableDialog {
protected:
	void OnClosetab();
	long OnTabSelected(WPARAM wP, LPARAM lP);

public:
	short ID;
	CString ViewDir;
	CString ViewName;
	CInfoPanelView* Parent;

	CViewContentDlgBase();
	CViewContentDlgBase(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CViewContentDlgBase();
};
