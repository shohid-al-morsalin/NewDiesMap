// ViewBaseline.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ViewBaseline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewBaseline dialog

CViewBaseline::CViewBaseline(CWnd* pParent /*=NULL*/)
	: CDialog(CViewBaseline::IDD, pParent) {
	//{{AFX_DATA_INIT(CViewBaseline)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CViewBaseline::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewBaseline)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewBaseline, CDialog)
	//{{AFX_MSG_MAP(CViewBaseline)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewBaseline message handlers