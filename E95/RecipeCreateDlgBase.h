// receipeCreateDlgBase.h: interface for the CRecipeCreateDlgBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\SRC\ResizableDialog.h"

class CInfoPanelMain;

class CRecipeCreateDlgBase : public CResizableDialog {
protected:

	void OnClosetab();
	long OnTabSelected(WPARAM wP, LPARAM lP);
public:

	void EnaDlg(int ID, BOOL bEn = TRUE);

protected:

public:
	short ID;
	CInfoPanelMain* m_Parent;
	BOOL bLock;
	BOOL bInspect;

	CRecipeCreateDlgBase();
	CRecipeCreateDlgBase(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CRecipeCreateDlgBase();
};