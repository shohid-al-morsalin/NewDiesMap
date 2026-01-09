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

#ifndef _EDIT_H_
#define _EDIT_H_

//-------------------------------------------------------------------------------------------

#include <afxwin.h>

//-------------------------------------------------------------------------------------------

class CMyEdit : public CEdit {
protected:

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

//-------------------------------------------------------------------------------------------

#endif // _EDIT_H_

//-------------------------------------------------------------------------------------------