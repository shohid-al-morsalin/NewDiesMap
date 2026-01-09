// WelcomeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "WelcomeDlg.h"

// CWelcomeDlg dialog

IMPLEMENT_DYNAMIC(CWelcomeDlg, CDialog)
CWelcomeDlg::CWelcomeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWelcomeDlg::IDD, pParent) {}

CWelcomeDlg::~CWelcomeDlg() {}

void CWelcomeDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWelcomeDlg, CDialog)
END_MESSAGE_MAP()

// CWelcomeDlg message handlers