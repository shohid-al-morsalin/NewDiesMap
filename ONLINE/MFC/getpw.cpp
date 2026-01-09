// getpw.cpp : implementation file
//

#include "stdafx.h"
#include "getpw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetPW dialog

CGetPW::CGetPW(CWnd* pParent /*=NULL*/)
	: CDialog(CGetPW::IDD, pParent) {
	//{{AFX_DATA_INIT(CGetPW)
	m_password = _T("");
	m_username = _T("");
	//}}AFX_DATA_INIT
}

void CGetPW::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPW)
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGetPW, CDialog)
	//{{AFX_MSG_MAP(CGetPW)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPW message handlers

void CGetPW::OnOK() {
	UpdateData(TRUE);
	CDialog::OnOK();
}