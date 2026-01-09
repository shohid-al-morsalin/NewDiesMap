// ParaEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ParaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParaEditDlg dialog

CParaEditDlg::CParaEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParaEditDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CParaEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CParaEditDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParaEditDlg)
	DDX_Control(pDX, IDC_EDIT_MAX, m_CDesc);
	DDX_Control(pDX, IDC_EDIT_DESC, m_CRangeMax);
	DDX_Control(pDX, IDC_PARAMETER, m_CParameter);
	DDX_Control(pDX, IDC_RANGE, m_CRange);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParaEditDlg, CDialog)
	//{{AFX_MSG_MAP(CParaEditDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK2, OnBnClickedOk2)
	ON_BN_CLICKED(IDCANCEL2, OnBnClickedCancel2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParaEditDlg message handlers

BOOL CParaEditDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	CRect nRc;
	CString dd("");

	GetWindowRect(&nRc);
	MoveWindow(300, 300, nRc.right - nRc.left, nRc.bottom - nRc.top);
	SetDlgItemText(IDC_PARAMETER, m_ParaName);
	SetDlgItemText(IDC_VALUE, m_Value);

	SetDlgItemText(IDC_RANGE, m_ParaRangeMin);
	SetDlgItemText(IDC_EDIT_MAX, m_ParaRangeMax);
	SetDlgItemText(IDC_EDIT_DESC, m_ParaDesc);
	m_CRange.SetReadOnly(TRUE);    // EnableWindow(FALSE);
	GetDlgItemText(IDC_RANGE, dd);
	m_CParameter.SetReadOnly(TRUE);   // SetReadOnly( FALSE );
	m_CRangeMax.SetReadOnly(TRUE);
	m_CDesc.SetReadOnly(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CParaEditDlg::OnOk() {
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_VALUE, m_Value);
	CDialog::OnOK();
}

void CParaEditDlg::OnBnClickedOk2() {
	// TODO: Add your control notification handler code here
	OnOk();
}

void CParaEditDlg::OnBnClickedCancel2() {
	CDialog::OnCancel();
}