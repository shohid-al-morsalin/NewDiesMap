// StopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "StopDlg.h"

#include "413App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStopDlg dialog

CStopDlg::CStopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStopDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CStopDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CStopDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStopDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStopDlg, CDialog)
	//{{AFX_MSG_MAP(CStopDlg)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStopDlg message handlers

void CStopDlg::OnStop() {
	p413App->Global.bStop = TRUE;
	OnOK();
}