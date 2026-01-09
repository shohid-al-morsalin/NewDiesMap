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

// Exported Function: CStringList* getFileNames(TCHAR*, TCHAR*, HWND);
// Last HWND parameter is a default argument defaulting to NULL if
// unspecified by the caller.

// Sample Usage:
// CStringList* list = getFileNames("c:\\temp\\usr\\", "Pack Files\0*.pak\0\0");
// You must free the list after use i.e, delete list;
// Also note that you should append "\\" at the end of the path parameter, as in c:\\temp\\usr\\

//-------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "customfiledlg.h"
using namespace NSPCUSTOMDLG;

//-------------------------------------------------------------------------------------------

CStringList* getFileNames(TCHAR* szPath, TCHAR* szFilter, HWND hWndParent) {
	TCHAR filenameonly[MAX_CHAR];
	TCHAR* szSelections = NULL;
	CStringList* StringList = NULL;
	CCustomFileDlg CustomFileDialog;

	// Sanity Check

	if (GetFileAttributes(szPath) == 0xFFFFFFFF) {
		::MessageBox(hWndParent, "Invalid path specified.",
			"Path Error", MB_OK | MB_ICONEXCLAMATION);
		return (StringList = NULL);
	}

	// Allocate storage for szSelections
	szSelections = new TCHAR[_10K_];
	if (szSelections == NULL) // can't use assert in release builds
	{
		::MessageBox(hWndParent, "Could not allocate 10K block.",
			"Memory Allocation Error", MB_OK | MB_ICONEXCLAMATION);
		return (StringList = NULL);
	}
	memset(szSelections, NULL, _10K_);

	CustomFileDialog.m_ofn.Flags = FILE_DIALOG_STYLE; // defined in header file
	CustomFileDialog.m_ofn.lStructSize = sizeof(OPENFILENAMEA);
	CustomFileDialog.m_ofn.lpstrFilter = szFilter;
	CustomFileDialog.m_ofn.hwndOwner = hWndParent;
	CustomFileDialog.m_ofn.nFilterIndex = 0;
	CustomFileDialog.m_ofn.lpstrFileTitle = filenameonly;
	CustomFileDialog.m_ofn.nMaxFileTitle = sizeof(filenameonly) - 1;
	CustomFileDialog.m_ofn.lpstrInitialDir = szPath;
	CustomFileDialog.m_ofn.lpstrFile = szSelections;
	CustomFileDialog.m_ofn.nMaxFile = _10K_;
	CustomFileDialog.m_ofn.nFileOffset = WORD(strlen(szPath));

	if (CustomFileDialog.DoModal() == IDCANCEL) {
		if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL) {
			::MessageBox(hWndParent, "Could not open files - too many files were selected.",
				"File Selection Error", MB_OK | MB_ICONEXCLAMATION);
		}
		delete[] szSelections;
		return (StringList = NULL);
	}

	// All clear, so now allocate memory block
	StringList = new CStringList(BLOCK_GRANULARITY);

	// Build a list of CString objects from szSelections
	TCHAR szFileName[MAX_CHAR];
	int nIndex = 0;
	BOOL Over = FALSE;
	CString x;
	TCHAR szQualPath[MAX_CHAR];

	memset(szFileName, NULL, sizeof(szFileName));
	memset(szQualPath, NULL, sizeof(szQualPath));

	int start = (int)strlen(szPath);
	while (start && szSelections[0]) {
		switch (szSelections[start]) {
		case NULL:

			strcpy(szQualPath, szPath);
			strcat(szQualPath, szFileName);
			x = szQualPath;
			StringList->AddTail(x);
			nIndex = 0;
			memset(szFileName, NULL, sizeof(szFileName));
			if (szSelections[start + 1] == NULL) // Peek ahead
			{
				Over = TRUE;
				break;
			}
			break;

		default:

			szFileName[nIndex++] = szSelections[start];
			break;
		} // end switch
		if (Over == TRUE)
			break;
		start++;
	}
	delete[] szSelections;
	return StringList;
}

//-------------------------------------------------------------------------------------------