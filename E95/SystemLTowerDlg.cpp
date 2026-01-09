// SystemLTowerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "SystemLTowerDlg.h"

#include "GlobalUIContainer.h"
// #include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemLTowerDlg dialog

CSystemLTowerDlg::CSystemLTowerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemLTowerDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSystemLTowerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
// 	if (!pGDev)
// 	{
// 		ASSERT(0);
// 	}
}

void CSystemLTowerDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemLTowerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSystemLTowerDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemLTowerDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemLTowerDlg message handlers

LRESULT CSystemLTowerDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("System\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CSystemLTowerDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}