// IncmplPrcsHdlr.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "IncmplPrcsHdlr.h"

#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIncmplPrcsHdlr dialog

CIncmplPrcsHdlr::CIncmplPrcsHdlr(CWnd* pParent /*=NULL*/)
	: CDialog(CIncmplPrcsHdlr::IDD, pParent) {
	//{{AFX_DATA_INIT(CIncmplPrcsHdlr)
	m_Prompt = _T("");
	//}}AFX_DATA_INIT
	unit = 0;
	Action = NOACTION;
}

void CIncmplPrcsHdlr::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIncmplPrcsHdlr)
	DDX_Control(pDX, IDC_ACTION2, m_Action2);
	DDX_Control(pDX, IDC_ACTION3, m_Action3);
	DDX_Text(pDX, IDC_STATIC1, m_Prompt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIncmplPrcsHdlr, CDialog)
	//{{AFX_MSG_MAP(CIncmplPrcsHdlr)
	ON_BN_CLICKED(IDC_ACTION1, OnAction1)
	ON_BN_CLICKED(IDC_ACTION2, OnAction2)
	ON_BN_CLICKED(IDC_ACTION3, OnAction3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIncmplPrcsHdlr message handlers

void CIncmplPrcsHdlr::OnAction1() {
	Action = FORGETANDCLOSEFOUP;
	CDialog::OnOK();
}

void CIncmplPrcsHdlr::OnAction2() {
	Action = DONOTCLOSEFOUP;
	CDialog::OnOK();
}

void CIncmplPrcsHdlr::OnAction3() {
	Action = STARTRECOVERY;
	CDialog::OnOK();
}

BOOL CIncmplPrcsHdlr::OnInitDialog() {
	CDialog::OnInitDialog();

	CString str = "413";
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
	case CGlobalDeviceContainer::FSM413MOT:
	case CGlobalDeviceContainer::FSM413SA:
		str = "413";
		break;
	}
	m_Prompt.Format(
		"Load port %d may have an incomplete process.\n"
		"Carrier %s may have wafers being processed in %s module\n"
		"Please select an action below.",
		unit, CarrierID, str);

	m_Action2.ShowWindow(SW_HIDE);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}