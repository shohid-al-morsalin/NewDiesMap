// DuplicateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "DuplicateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDuplicateDlg dialog

CDuplicateDlg::CDuplicateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDuplicateDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CDuplicateDlg)
	m_Value = 0;
	//}}AFX_DATA_INIT
}

void CDuplicateDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDuplicateDlg)
	DDX_Text(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDuplicateDlg, CDialog)
	//{{AFX_MSG_MAP(CDuplicateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDuplicateDlg message handlers