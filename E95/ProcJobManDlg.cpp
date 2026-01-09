// ProcJobManDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ProcJobManDlg.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_DATAREQ_TIMEOUT		9993

/////////////////////////////////////////////////////////////////////////////
// CProcJobManDlg dialog

CProcJobManDlg::CProcJobManDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobManDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobManDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProcJobManDlg::~CProcJobManDlg() {}

void CProcJobManDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobManDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobManDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobManDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobManDlg message handlers

BOOL CProcJobManDlg::DestroyWindow() {
	// TODO: Add your specialized code here and/or call the base class

	return CResizableDialog::DestroyWindow();
}

void CProcJobManDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CProcJobManDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobManDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobManDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

int CProcJobManDlg::GetHWnd() {
	return (int)m_hWnd;
}

void CProcJobManDlg::SetDataReqTimeOut(long dur) {
	SetTimer(IDT_DATAREQ_TIMEOUT, dur, NULL);
}