// TFTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "TFTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTFTestDlg dialog

CTFTestDlg::CTFTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTFTestDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CTFTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Selection = 0;
}

void CTFTestDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTFTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTFTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTFTestDlg)
	ON_BN_CLICKED(IDC_SINEDATA, OnSinedata)
	ON_BN_CLICKED(IDC_REFERENCESPEC, OnReferencespec)
	ON_BN_CLICKED(IDC_DARKSPEC, OnDarkspec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTFTestDlg message handlers

void CTFTestDlg::OnSinedata() {
	Selection = 1;
	OnOK();
}

void CTFTestDlg::OnReferencespec() {
	Selection = 2;
	OnOK();
}

void CTFTestDlg::OnDarkspec() {
	Selection = 3;
	OnOK();
}