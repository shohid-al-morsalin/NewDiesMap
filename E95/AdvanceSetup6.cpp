#include "stdafx.h"

#include "e95.h"

#include "413App.h"
#include "413/MotorL2.h"
#include "413/MotorS2.h"

#include "AdvanceSetup6.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvanceSetup6 dialog

CAdvanceSetup6::CAdvanceSetup6(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvanceSetup6::IDD, pParent) {
	//{{AFX_DATA_INIT(CAdvanceSetup6)
	m_StageOriginXEcho = 0.0f;
	m_StageOriginYEcho = 0.0f;
	m_StageOriginXTF = 0.0f;
	m_StageOriginYTF = 0.0f;
	m_StagePEcho = 0.0f;
	m_StagePTF = 0.0f;
	//}}AFX_DATA_INIT
	cmd = NOCMD;
}

void CAdvanceSetup6::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvanceSetup6)
	DDX_Text(pDX, IDC_STAGEORIGINX, m_StageOriginXEcho);
	DDX_Text(pDX, IDC_STAGEORIGINY, m_StageOriginYEcho);
	DDX_Text(pDX, IDC_STAGEORIGINX2, m_StageOriginXTF);
	DDX_Text(pDX, IDC_STAGEORIGINY2, m_StageOriginYTF);
	DDX_Text(pDX, IDC_STAGEPECHO, m_StagePEcho);
	DDX_Text(pDX, IDC_STAGEPTF, m_StagePTF);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAdvanceSetup6, CDialog)
	//{{AFX_MSG_MAP(CAdvanceSetup6)
	ON_BN_CLICKED(IDC_CLEARHOMEXY, OnClearhomexy)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_ADVANCEXY, OnAdvancexy)
	ON_BN_CLICKED(IDC_SETBGPOS, OnSetbgpos)
	ON_BN_CLICKED(IDC_SETDPPOS, OnSetdppos)
	ON_BN_CLICKED(IDC_SETHOMEXY, OnSethomexy)
	ON_BN_CLICKED(IDC_SETDPPOS2, OnSetdppos2)
	ON_BN_CLICKED(IDC_SETHOMEXYROUGH, OnSethomexyrough)
	ON_BN_CLICKED(IDC_SETPTHINF, OnSetpthinf)
	ON_BN_CLICKED(IDC_SETPECHO, OnSetpecho)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvanceSetup6 message handlers

void CAdvanceSetup6::OnClearhomexy() {
	cmd = CLEARORIGIN;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSave() {
	cmd = SAVEPARAM;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnLoad() {
	cmd = LOADPARAM;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnAdvancexy() {
	cmd = SETLOADPOS;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSetbgpos() {
	cmd = SETBGPOS;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSetdppos() {
	cmd = SETCALIBPOS;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSethomexy() {
	cmd = SETORIGINECHO;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSetdppos2() {
	cmd = SETCALIBPOSROU;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSethomexyrough() {
	cmd = SETORIGINROU;
	CDialog::OnOK();
}

BOOL CAdvanceSetup6::OnInitDialog() {
	CDialog::OnInitDialog();

	CMotorS2* pMtr = p413App->pMtr;

	m_StageOriginXEcho = pMtr->pMX->Param.HomePos1;
	m_StageOriginYEcho = pMtr->pMY->Param.HomePos1;
	m_StageOriginXRough = pMtr->pMX->Param.HomePos2;
	m_StageOriginYRough = pMtr->pMY->Param.HomePos2;
	if (pMtr->pMP) {
		m_StagePEcho = pMtr->pMP->Param.HomePos1;
		m_StagePRough = pMtr->pMP->Param.HomePos2;
	}

	UpdateData(FALSE);

	if (p413App->Global.Confg.bPMotorMod) {
		((CButton*)GetDlgItem(IDC_STAGEPECHO))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_PECHO))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_PSTAGEECHO))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_SETPECHO))->ShowWindow(SW_SHOW);

		((CButton*)GetDlgItem(IDC_STAGEPTF))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_PTF))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_PSTAGETF))->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_SETPTHINF))->ShowWindow(SW_SHOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvanceSetup6::OnSetpthinf() {
	cmd = SETPTF;
	CDialog::OnOK();
}

void CAdvanceSetup6::OnSetpecho() {
	cmd = SETPECHO;
	CDialog::OnOK();
}
