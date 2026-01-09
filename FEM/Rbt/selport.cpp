// selport.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "selport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelPort dialog

CSelPort::CSelPort(CWnd* pParent /*=NULL*/)
	: CDialog(CSelPort::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelPort)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Port = 0;
}

void CSelPort::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPort)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelPort, CDialog)
	//{{AFX_MSG_MAP(CSelPort)
	ON_BN_CLICKED(IDC_PORT1, OnPort1)
	ON_BN_CLICKED(IDC_PORT2, OnPort2)
	ON_BN_CLICKED(IDC_PORT3, OnPort3)
	ON_BN_CLICKED(IDC_PORT4, OnPort4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPort message handlers

void CSelPort::OnPort1() {
	Port = 1;
}

void CSelPort::OnPort2() {
	Port = 2;
}

void CSelPort::OnPort3() {
	Port = 3;
}

void CSelPort::OnPort4() {
	Port = 4;
}