//-------------------------------------------------------------------------------------------

// Customized version of the File Open common dialog
// Copyright (C) 1999 by Shanker.C
// All rights reserved
// Author's consent required if this program is to be used for commercial purposes
// No warranty of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the user.
// For a full description of the code, see associated documentation.
// Please send comments/suggestions/criticisms to: Shanker@xlprint.com
// November 3, 1999

//-------------------------------------------------------------------------------------------

#ifndef _CUSTOMFILEDLG_H_
#define _CUSTOMFILEDLG_H_

//-------------------------------------------------------------------------------------------

#include <afxdlgs.h>
#include <cderr.h>

//-------------------------------------------------------------------------------------------

// Start namespace
namespace NSPCUSTOMDLG {
	//-------------------------------------------------------------------------------------------

	const int _10K_ = 10000;
	const int MAX_CHAR = 250;
	const int TB_BTN_UPONELEVEL = 40961;
	const int TB_BTN_NEWFOLDER = 40962;
	const int LOOK_IN_COMBO = 1137;
	const int LAST_LISTVIEW_MSG = 4146;
	const int BLOCK_GRANULARITY = 30;
#define FILE_DIALOG_STYLE OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST

	//-------------------------------------------------------------------------------------------

	class CCustomFileDlg : public CFileDialog {
	public:

		CCustomFileDlg(CWnd* pWnd = NULL) :
			CFileDialog(TRUE, NULL, NULL, FILE_DIALOG_STYLE, NULL, pWnd) {}

		~CCustomFileDlg();
		static int OnceOnly;

	protected:

		virtual BOOL OnInitDialog();
	};

	//-------------------------------------------------------------------------------------------
} // End namespace

//-------------------------------------------------------------------------------------------

LRESULT CALLBACK Hooker(int, WPARAM, LPARAM);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
CStringList* getFileNames(TCHAR*, TCHAR*, HWND hWndParent = NULL);
void HideToolbarBtns(HWND);
extern HHOOK HookHandle;

//-------------------------------------------------------------------------------------------

#endif //_CUSTOMFILEDLG_H_

//-------------------------------------------------------------------------------------------
