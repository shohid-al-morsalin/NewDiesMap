// EditBinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EditBinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditBinDlg dialog

CEditBinDlg::CEditBinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditBinDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CEditBinDlg)
	m_iHisto = 1;
	m_Limit = 0;
	//}}AFX_DATA_INIT

	dx = 5;
}

void CEditBinDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditBinDlg)
	DDX_Text(pDX, IDC_INCREMENT, m_iHisto);
	DDV_MinMaxInt(pDX, m_iHisto, 1, 8);
	DDX_Text(pDX, IDC_MAXIMUM, m_Limit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditBinDlg, CDialog)
	//{{AFX_MSG_MAP(CEditBinDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditBinDlg message handlers

void CEditBinDlg::OnRadio1() {
	dx = 1;
}

void CEditBinDlg::OnRadio2() {
	dx = 2;
}

void CEditBinDlg::OnRadio3() {
	dx = 3;
}

void CEditBinDlg::OnRadio4() {
	dx = 4;
}

void CEditBinDlg::OnRadio5() {
	dx = 5;
}

void CEditBinDlg::OnRadio6() {
	dx = 6;
}

BOOL CEditBinDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	RefreshDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditBinDlg::RefreshDialog() {
	if (dx == 1) {
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	}
	else if (dx == 2) {
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
	}
	else if (dx == 3) {
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
	}
	else if (dx == 4) {
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
	}
	else if (dx == 5) {
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(TRUE);
	}
	else if (dx == 6) {
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(TRUE);
	}
}