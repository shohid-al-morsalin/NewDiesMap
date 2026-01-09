// InputFloat.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "InputFloat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputFloat dialog

CInputFloat::CInputFloat(CWnd* pParent /*=NULL*/)
	: CDialog(CInputFloat::IDD, pParent) {
	//{{AFX_DATA_INIT(CInputFloat)
	m_Value = 0.0f;
	m_cPrompt = _T("");
	//}}AFX_DATA_INIT
}

void CInputFloat::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputFloat)
	DDX_Text(pDX, IDC_VALUE, m_Value);
	DDX_Text(pDX, IDC_PROMPT, m_cPrompt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputFloat, CDialog)
	//{{AFX_MSG_MAP(CInputFloat)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputFloat message handlers