#include "stdafx.h"
#include "resource.h"

#include "413App.h"
#include "413/MotorS2.h"

#include "AdvanceOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvanceOption dialog

CAdvanceOption::CAdvanceOption(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvanceOption::IDD, pParent) {
	//{{AFX_DATA_INIT(CAdvanceOption)
	//}}AFX_DATA_INIT
	pVideo = NULL;
}

void CAdvanceOption::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvanceOption)
	DDX_Control(pDX, ID_HOME, m_cHome);
	DDX_Control(pDX, ID_RESETXY, m_cResetXY);
	DDX_Control(pDX, IDC_RESETZTHINFILM, m_cResetRough);
	DDX_Control(pDX, IDC_RESETCWLZ, m_cResetTopCWLZ);
	DDX_Control(pDX, IDC_RESETZ, m_cResetEchoTopZ);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAdvanceOption, CDialog)
	//{{AFX_MSG_MAP(CAdvanceOption)
	ON_BN_CLICKED(IDC_RESETZ, OnResetz)
	ON_BN_CLICKED(IDC_RESETROUGHZ, OnResetroughz)
	ON_BN_CLICKED(IDC_RESETCWLZ, OnResetTopCWLz)
	ON_BN_CLICKED(ID_RESETXY, OnResetxy)
	ON_BN_CLICKED(ID_HOME, OnHome)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvanceOption message handlers

BOOL CAdvanceOption::OnInitDialog() {
	CDialog::OnInitDialog();

	if (p413App->Global.Confg.bEchoTopMotorMod) {
		m_cResetEchoTopZ.EnableWindow(TRUE);
	}
	if (p413App->Global.Confg.bRoughMotorMod) {
		m_cResetRough.EnableWindow(TRUE);
	}
	// [ 06112024/MORSALIN
	if (p413App->Global.Confg.bConfocalTopMotorMod) {
		m_cResetTopCWLZ.EnableWindow(TRUE);
	}
	// ]
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvanceOption::OnHome() {
	m_cHome.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->GotoXY(0, 0);
	m_cHome.EnableWindow(TRUE);
	CDialog::OnOK();
}

void CAdvanceOption::OnResetz() {
	m_cResetEchoTopZ.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->ResetEchoTopZ();
	m_cResetEchoTopZ.EnableWindow(TRUE);
	CDialog::OnOK();
}

void CAdvanceOption::OnResetroughz() {
	m_cResetRough.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->ResetRoughZ();
	m_cResetRough.EnableWindow(TRUE);
	CDialog::OnOK();
}

// [ 06112024/MORSALIN
void CAdvanceOption::OnResetTopCWLz() {
	m_cResetTopCWLZ.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->ResetCWLTopZ();
	m_cResetTopCWLZ.EnableWindow(TRUE);
	CDialog::OnOK();
}
// ]

void CAdvanceOption::OnResetxy() {
	m_cResetXY.EnableWindow(FALSE);
	if (p413App->pMtr) {
		if (p413App->pMtr->ResetXY()) {
			p413App->Global.ResetLapTime = GetTickCount64();
		}
	}
	m_cResetXY.EnableWindow(TRUE);
	CDialog::OnOK();
}
