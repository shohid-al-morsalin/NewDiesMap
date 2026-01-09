#include "stdafx.h"

#include "e95.h"

#include "AdvanceSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvanceSetup dialog

CAdvanceSetup::CAdvanceSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvanceSetup::IDD, pParent) {
	//{{AFX_DATA_INIT(CAdvanceSetup)
	m_XPos = 0.0f;
	m_YPos = 0.0f;
	m_ZPos = 0.0f;
	//}}AFX_DATA_INIT

	Type = LOADPOS;
}

void CAdvanceSetup::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvanceSetup)
	DDX_Control(pDX, IDC_NOTICE, m_cNotice);
	DDX_Text(pDX, IDC_LOADX, m_XPos);
	DDX_Text(pDX, IDC_LOADY, m_YPos);
	DDX_Text(pDX, IDC_LOADZ, m_ZPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAdvanceSetup, CDialog)
	//{{AFX_MSG_MAP(CAdvanceSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvanceSetup message handlers

BOOL CAdvanceSetup::OnInitDialog() {
	CDialog::OnInitDialog();

	switch (Type) {
	case LOADPOS:
		SetWindowText("Set Load Position");
		break;
	case CALIBECHO:
		SetWindowText("Set Calibrate (ECHO) Position");
		break;
	case CALIBTF:
		SetWindowText("Set Calibrate (TF) Position");
		break;
	case CALIBBG:
		SetWindowText("Set Calibrate Background Position");
		break;
	case CALIBROU:
		SetWindowText("Set Calibrate (ROU) Position");
		break;
	}

	if (Type != LOADPOS) {
		m_cNotice.SetWindowText("");
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}