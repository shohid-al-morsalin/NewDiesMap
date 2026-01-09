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

#include "stdafx.h"
#include <direct.h>
#include "customfiledlg.h"
#include <dlgs.h>
#include "myedit.h"
using namespace NSPCUSTOMDLG;

//-------------------------------------------------------------------------------------------

HHOOK HookHandle; //extern
int CCustomFileDlg::OnceOnly = 0;
CMyEdit m_edit;

//-------------------------------------------------------------------------------------------

BOOL CCustomFileDlg::OnInitDialog() {
	// Hide unwanted controls
	EnumChildWindows((HWND)::GetParent(this->m_hWnd), EnumChildProc, (LPARAM)NULL);
	// Set hook
	HookHandle = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)Hooker,
		(HINSTANCE)NULL, (DWORD)GetCurrentThreadId());
	//TRACE("HOOK SET");

	return CFileDialog::OnInitDialog();
}

//-------------------------------------------------------------------------------------------

CCustomFileDlg :: ~CCustomFileDlg() {
	UnhookWindowsHookEx(HookHandle);
	OnceOnly = 0;
}

//-------------------------------------------------------------------------------------------

LRESULT CALLBACK Hooker(int nCode, WPARAM wParam, LPARAM lParam) {
	TCHAR szClassName[MAX_CHAR];
	TCHAR szItemName[MAX_CHAR];
	LVITEM item;
	static HWND hWndToolbar = NULL; // Persist

	CWPSTRUCT* x = (CWPSTRUCT*)lParam;
	GetClassName(x->hwnd, szClassName, MAX_CHAR);

	// Save toolbar's handle only once
	if (strcmp(_strlwr(szClassName), "toolbarwindow32") == 0) {
		if (!CCustomFileDlg::OnceOnly) {
			hWndToolbar = x->hwnd;
			++CCustomFileDlg::OnceOnly;
		}
	}

	// Get rid of items in the list view control
	if (strcmp(_strlwr(szClassName), "syslistview32") == 0) {
		switch (x->message) {
		case WM_NCPAINT: // Needed if there are no items to display
		case LAST_LISTVIEW_MSG: // Magic message sent after all items are inserted

			int count = ListView_GetItemCount(x->hwnd);
			for (int i = 0; i < count; i++) {
				item.mask = LVIF_TEXT | LVIF_PARAM;
				item.iItem = i;
				item.iSubItem = 0;
				item.pszText = szItemName;
				item.cchTextMax = MAX_CHAR;
				ListView_GetItem(x->hwnd, &item);
				if (GetFileAttributes(szItemName) & FILE_ATTRIBUTE_DIRECTORY)
					ListView_DeleteItem(x->hwnd, i);
			}

			break;
		} // End switch
		HideToolbarBtns(hWndToolbar);
	} // End if

	// Subclass the edit control (only once)
	if (strcmp(_strlwr(szClassName), "edit") == 0) {
		static int once = 0;
		if (!once) {
			m_edit.SubclassDlgItem(edt1, (CFileDialog*)CWnd::FromHandle(GetParent(x->hwnd)));
			++once;
		}
	}	// End if

	return CallNextHookEx(HookHandle, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
	int id = ::GetDlgCtrlID(hWnd);

	switch (id) {
	case LOOK_IN_COMBO: // Disable combo box at top of dialog

		::EnableWindow(hWnd, FALSE);
		break;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------

void HideToolbarBtns(HWND hWndToolbar) {
	TBBUTTONINFO tbinfo;
	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_STATE;
	tbinfo.fsState = TBSTATE_HIDDEN | TBSTATE_INDETERMINATE;
	::SendMessage(hWndToolbar, TB_SETBUTTONINFO, (WPARAM)TB_BTN_UPONELEVEL, (LPARAM)&tbinfo);
	::SendMessage(hWndToolbar, TB_SETBUTTONINFO, (WPARAM)TB_BTN_NEWFOLDER, (LPARAM)&tbinfo);
}

//-------------------------------------------------------------------------------------------