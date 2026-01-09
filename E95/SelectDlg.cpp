// SelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog

CSelectDlg::CSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDlg)
	DDX_Control(pDX, IDC_LIST_ITEMS, m_Items);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDlg)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg message handlers

void CSelectDlg::OnButtonUp() {
	// TODO: Add your control notification handler code here
	int nIndex = m_Items.GetCount();
	if (nIndex) {
		int nPos = m_Items.GetCurSel();
		if (nPos != 0) {
			m_Items.SetCurSel(nPos - 1);
		}
	}
}

void CSelectDlg::OnButtonDown() {
	// TODO: Add your control notification handler code here
	int nIndex = m_Items.GetCount();
	if (nIndex) {
		int nPos = m_Items.GetCurSel();
		if (nPos < nIndex) {
			m_Items.SetCurSel(nPos + 1);
		}
	}
}

BOOL CSelectDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	int nNum = m_saItemList.GetSize();
	for (int nIdx = 0; nIdx < nNum; nIdx++) {
		m_Items.AddString(m_saItemList.GetAt(nIdx));
	}
	m_Items.SetCurSel(m_nSel);
	return TRUE;  // return TRUE unless you set the focus to a control
			  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectDlg::OnOK() {
	// TODO: Add extra validation here
	m_nSel = m_Items.GetCurSel();
	CDialog::OnOK();
}