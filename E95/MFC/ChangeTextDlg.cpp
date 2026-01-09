// ChangeTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeTextDlg dialog


CChangeTextDlg::CChangeTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeTextDlg)
	m_from = _T("");
	m_to = _T("");
	//}}AFX_DATA_INIT
}


void CChangeTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeTextDlg)
	DDX_Text(pDX, IDC_CHGFROM, m_from);
	DDX_Text(pDX, IDC_CHGTO, m_to);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeTextDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeTextDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTextDlg message handlers
