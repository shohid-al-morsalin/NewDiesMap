#include "stdafx.h"
#include "DelimiterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelimiterDlg dialog

CDelimiterDlg::CDelimiterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelimiterDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CDelimiterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	term = '\t';
}

void CDelimiterDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelimiterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDelimiterDlg, CDialog)
	//{{AFX_MSG_MAP(CDelimiterDlg)
	ON_BN_CLICKED(IDC_TAB, OnTab)
	ON_BN_CLICKED(IDC_SPACES, OnSpaces)
	ON_BN_CLICKED(IDC_COMMA, OnComma)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelimiterDlg message handlers

void CDelimiterDlg::OnTab() {
	term = '\t';
}

void CDelimiterDlg::OnSpaces() {
	term = ' ';
}

void CDelimiterDlg::OnComma() {
	term = ',';
}

BOOL CDelimiterDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_TAB))->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}