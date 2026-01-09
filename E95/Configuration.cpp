#include "stdafx.h"
#include "e95.h"

#include "413App.h"

#include "Configuration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfiguration dialog

CConfiguration::CConfiguration(CWnd* pParent /*=NULL*/)
	: CDialog(CConfiguration::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfiguration)
	m_EchoMicroscope = FALSE;
	m_PatternRec = FALSE;
	m_PMotor = FALSE;
	m_SecsGEM = FALSE;
	m_ThinFilmMicroscope = FALSE;
	m_ThinFilmModule = FALSE;
	m_TrimModule = FALSE;
	m_EchoProbeModule = FALSE;
	m_ThinFilmMotor = FALSE;
	m_EchoTopMotor = FALSE;
	m_StressModule = FALSE;
	// [20240306 ZHIMING
	m_AkusenseProbe = FALSE;
	// [20240307
	m_BowWarp = FALSE;
	// [04162024
	m_Roughness = FALSE;
	// [05032024
	m_EchoBottomMotorMod = FALSE;
	// [05092024
	m_ConfocalTopMotorMod = FALSE;
	m_RoughMotorMod = FALSE;
	// ]
	m_Taiko = FALSE; // 08092024 Mahedi Kamal
	//}}AFX_DATA_INIT
}

void CConfiguration::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfiguration)
	DDX_Check(pDX, IDC_ECHOMICROSCOPE, m_EchoMicroscope);
	DDX_Check(pDX, IDC_PATTERNREC, m_PatternRec);
	DDX_Check(pDX, IDC_PMOTOR, m_PMotor);
	DDX_Check(pDX, IDC_SECSGEM, m_SecsGEM);
	DDX_Check(pDX, IDC_THINFILMMICROSCOPE, m_ThinFilmMicroscope);
	DDX_Check(pDX, IDC_THINFILMMODULE, m_ThinFilmModule);
	DDX_Check(pDX, IDC_TRIMMODULE, m_TrimModule);
	DDX_Check(pDX, IDC_ECHOPROBE, m_EchoProbeModule);
	DDX_Check(pDX, IDC_THINFILMMOTOR, m_ThinFilmMotor);
	DDX_Check(pDX, IDC_ECHOMOTOR, m_EchoTopMotor);
	DDX_Check(pDX, IDC_STRESSMODULE, m_StressModule);
	DDX_Check(pDX, IDC_AKUSENSE_RPOBE, m_AkusenseProbe);
	DDX_Check(pDX, IDC_CONFIGURATION_BOW_WARP, m_BowWarp);
	DDX_Check(pDX, IDC_CONFIGURATION_ROUGHNESS, m_Roughness);
	DDX_Check(pDX, IDC_CONFIGURATION_TAIKO, m_Taiko); // 08092024 Mahedi Kamal
	DDX_Check(pDX, IDC_ECHOBOTTOMMOTOR, m_EchoBottomMotorMod);
	DDX_Check(pDX, IDC_CONFOCAL_TOP_MOTOR, m_ConfocalTopMotorMod);
	DDX_Check(pDX, IDC_ROUGHNESS_MOTOR, m_RoughMotorMod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfiguration, CDialog)
	//{{AFX_MSG_MAP(CConfiguration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfiguration message handlers

BOOL CConfiguration::OnInitDialog() {
	CDialog::OnInitDialog();

	C413Confg* pCfg = &p413App->Global.Confg;

	m_EchoMicroscope = pCfg->bEchoMicroscopeMod;
	m_PatternRec = pCfg->bPatMatchMod;
	m_PMotor = pCfg->bPMotorMod;
	m_SecsGEM = pCfg->bSecsMod;
	m_ThinFilmMicroscope = pCfg->bThinFilmMicroscopeMod;
	m_ThinFilmModule = pCfg->bThinFilmMod;
	m_TrimModule = pCfg->bTrimMod;
	m_EchoProbeModule = pCfg->bEchoMod;
	m_EchoTopMotor = pCfg->bEchoTopMotorMod;
	m_ThinFilmMotor = pCfg->bThinFilmMotorMod;
	m_StressModule = pCfg->bStressMod;
	// [11302022 ZHIMING
	m_AkusenseProbe = pCfg->bConfocalMod;
	// [20240307
	m_BowWarp = pCfg->bBowWarp;
	// [04162024
	m_Roughness = pCfg->bRoughness;
	// [05032024
	m_EchoBottomMotorMod = pCfg->bEchoBottomMotorMod;
	// [05092024
	m_ConfocalTopMotorMod = pCfg->bConfocalTopMotorMod;
	m_RoughMotorMod = pCfg->bRoughMotorMod;
	// ]
	m_Taiko = pCfg->bTaiko; // 08092024 Mahedi Kamal

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfiguration::OnOK() {
	UpdateData(TRUE);

	C413Confg* pCfg = &p413App->Global.Confg;

	pCfg->bEchoMicroscopeMod = m_EchoMicroscope;
	pCfg->bPatMatchMod = m_PatternRec;
	pCfg->bPMotorMod = m_PMotor;
	pCfg->bSecsMod = m_SecsGEM;
	pCfg->bThinFilmMicroscopeMod = m_ThinFilmMicroscope;
	pCfg->bThinFilmMod = m_ThinFilmModule;
	pCfg->bTrimMod = m_TrimModule;
	pCfg->bEchoMod = m_EchoProbeModule;
	pCfg->bEchoTopMotorMod = m_EchoTopMotor;
	pCfg->bThinFilmMotorMod = m_ThinFilmMotor;
	pCfg->bStressMod = m_StressModule;
	// [11302022 ZHIMING
	pCfg->bConfocalMod = m_AkusenseProbe;
	// [20240307
	pCfg->bBowWarp = m_BowWarp;
	// [04162024
	pCfg->bRoughness = m_Roughness;
	// [05032024
	pCfg->bEchoBottomMotorMod = m_EchoBottomMotorMod;
	// [05092024
	pCfg->bConfocalTopMotorMod = m_ConfocalTopMotorMod;
	pCfg->bRoughMotorMod = m_RoughMotorMod;
	// ]
	pCfg->bTaiko = m_Taiko; // 08092024 Mahedi Kamal

	pCfg->Save();

	CDialog::OnOK();
}