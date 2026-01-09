// OnlineSubstrateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
// #include "..\IPC\SMHdr.h"
#include "..\IPC\SMOL.h"
#include "OnlineSubstrateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnlineSubstrateDlg dialog

COnlineSubstrateDlg::COnlineSubstrateDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineSubstrateDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineSubstrateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void COnlineSubstrateDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineSubstrateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineSubstrateDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineSubstrateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineSubstrateDlg message handlers

BOOL COnlineSubstrateDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	CString str = "";
	CString tmp;
	CSubstObj pS;
	for (int i = CSubstObj::NEEDS_PROCESSING; i <= CSubstObj::REJECTED; i++) {
		tmp.Format("%s = %d\n", pS.GetSubstProcStateStr((CSubstObj::SUBSTPROCSTATE)i), i);
		str += tmp;
	}
	((CStatic*)GetDlgItem(IDC_SUBSTPROCSTATE))->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineSubstrateDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}