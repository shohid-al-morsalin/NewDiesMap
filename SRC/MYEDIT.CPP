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
#include "myedit.h"

//-------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)

	ON_WM_CHAR()

END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// Clear selection, if any
	DWORD dwSel = GetSel();
	LOWORD(dwSel) == HIWORD(dwSel) ? NULL : SetWindowText("");

	CString strWindowText;
	GetWindowText(strWindowText);
	if (nChar == '\\' || nChar == ':' || (nChar == '.' && strWindowText.GetLength() < 1))
		return; // Don't pass on to base for processing
	CEdit::OnChar(nChar, nRepCnt, nFlags); // Pass on to base for processing
}

//-------------------------------------------------------------------------------------------