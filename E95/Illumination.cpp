// Illumination.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "Illumination.h"
#include "413App.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIllumination dialog

CIllumination::CIllumination(CWnd* pParent /*=NULL*/)
	: CDialog(CIllumination::IDD, pParent) {
	//{{AFX_DATA_INIT(CIllumination)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CIllumination::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIllumination)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIllumination, CDialog)
	//{{AFX_MSG_MAP(CIllumination)
	ON_BN_CLICKED(IDC_ECHOON, OnEchoon)
	ON_BN_CLICKED(IDC_ECHOOFF, OnEchooff)
	ON_BN_CLICKED(IDC_TFILMON, OnTfilmon)
	ON_BN_CLICKED(IDC_TFILMOFF, OnTfilmoff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIllumination message handlers

void CIllumination::OnEchoon() {
	if (p413App->pMtr) p413App->pMtr->SetMicroscopeLight(TRUE);
}

void CIllumination::OnEchooff() {
	if (p413App->pMtr) p413App->pMtr->SetMicroscopeLight(FALSE);
}

void CIllumination::OnTfilmon() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(TRUE);
}

void CIllumination::OnTfilmoff() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(FALSE);
}