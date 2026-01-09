// InputCarrierID.cpp : implementation file
//

#include "stdafx.h"
#include "InputCarrierID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputCarrierID dialog

CInputCarrierID::CInputCarrierID(CWnd* pParent /*=NULL*/)
	: CDialog(CInputCarrierID::IDD, pParent) {
	//{{AFX_DATA_INIT(CInputCarrierID)
	m_CarrierID = _T("");
	m_Prompt = _T("");
	//}}AFX_DATA_INIT
}

void CInputCarrierID::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputCarrierID)
	DDX_Text(pDX, IDC_CARRIERID, m_CarrierID);
	DDX_Text(pDX, IDC_STATIC1, m_Prompt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputCarrierID, CDialog)
	//{{AFX_MSG_MAP(CInputCarrierID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputCarrierID message handlers

BOOL CInputCarrierID::OnInitDialog() {
	CDialog::OnInitDialog();

	SetForegroundWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputCarrierID::OnOK() {
	UpdateData(TRUE);
	if (m_CarrierID.FindOneOf("\\/:?*<>\"|") != -1) {
		AfxMessageBox("Illegal charecter \\/:?*<>\"|");
	}
	else {
		CDialog::OnOK();
	}
}