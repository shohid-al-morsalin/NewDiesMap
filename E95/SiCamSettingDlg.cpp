// SiCamSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SiCamSettingDlg.h"

#include "siusb.h"

#include "413App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSiCamSettingDlg dialog

CSiCamSettingDlg::CSiCamSettingDlg(/*CSI3300CamDlg *pCam,*/ CWnd* pParent /*=NULL*/)
	: CDialog(CSiCamSettingDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSiCamSettingDlg)
	m_TargetExposure = 0;
	m_TargetExposure2 = 0;
	//}}AFX_DATA_INIT
	nTime = 0;
	bAuto = FALSE;
	bFocus = FALSE;
}

void CSiCamSettingDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiCamSettingDlg)
	DDX_Text(pDX, IDC_EXPOSURE_TIME, nTime);
	DDX_Text(pDX, IDC_TARGETEXPOSURE, m_TargetExposure);
	DDX_Text(pDX, IDC_TARGETEXPOSURE2, m_TargetExposure2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSiCamSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSiCamSettingDlg)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_AUTOEXPOSURE, OnAutoexposure)
	ON_BN_CLICKED(IDC_AUTOFOCUS, OnAutofocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiCamSettingDlg message handlers

BOOL CSiCamSettingDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	//04112023/yukchiu
	C413Global* g = &p413App->Global;

	if (g->ActiveCamera == CCamBase::ECHOCAM) {
		nTime = g->Exposure[0];
	}
	else if (g->ActiveCamera == CCamBase::THINFCAM) {
		nTime = g->Exposure[1];
	}
	else {
		nTime = g->Exposure[0];
	}
	//================
	if (nTime == 0) {
		
		// [02062024 ZHIMING MAKE EXPOSURE TIME DEFAULT
		// Need to load exposure time from CSI300Cam [6/24/2010 Yuen]
		// ASSERT(0);
		p413App->Log(_T("Expo became 0, set to default 30"));
		nTime = 30;
		// ]
		
	}
	m_TargetExposure = p413App->Global.RcpSetup.AutoExposeEcho;
	m_TargetExposure2 = p413App->Global.RcpSetup.AutoExposeTF;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSiCamSettingDlg::EnableButtons(BOOL bEnable) {
	GetDlgItem(IDC_APPLY)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMSET_AUTOEXPOSURE)->EnableWindow(bEnable);
}

void CSiCamSettingDlg::OnOk() {
	UpdateData(TRUE);

	p413App->Global.RcpSetup.AutoExposeEcho = m_TargetExposure;
	p413App->Global.RcpSetup.AutoExposeTF = m_TargetExposure2;

	CDialog::OnOK();
}

void CSiCamSettingDlg::OnAutoexposure() {
	bAuto = TRUE;
	CDialog::OnOK();
}

void CSiCamSettingDlg::OnAutofocus() {
	bFocus = TRUE;
	CDialog::OnOK();
}