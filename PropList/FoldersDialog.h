// FoldersDialog.h: interface for the CFoldersDialog class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <stdlib.h>
#include <tchar.h>
#include <Shlobj.h>

class CFoldersDialog {
public:
	CFoldersDialog();
	virtual ~CFoldersDialog();
	BOOL BrowseFolder(HWND, CString curPath, CString rootPath = "");
	CString GetFolder();
};
