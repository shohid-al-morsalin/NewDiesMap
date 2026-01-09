// RecipeIO.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "RecipeIO.h"
#include "413App.h"
#include "InfoPanelSetup.h"
#include "GlobalUIContainer.h"
#include "SetupProp3Dlg.h"
#include "..\413\DMCMotorBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecipeIO dialog

void CRecipeIO::LoadParam() {
	DosUtil.GetLocalCfgFile(inifile);
}

void CRecipeIO::SaveParam() {
}

CRecipeIO::CRecipeIO(CWnd* pParent /*=NULL*/)
	: CDialog(CRecipeIO::IDD, pParent) {
	LoadParam(); SaveParam();
}

void CRecipeIO::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRecipeIO, CDialog)
	//{{AFX_MSG_MAP(CRecipeIO)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOADF, OnLoadf)
	ON_BN_CLICKED(IDC_LOADQ, OnLoadq)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecipeIO message handlers

void CRecipeIO::OnNew() {
	p413App->Global.isNew = true; // 01012025 MORSALIN open DieMap on load
	if (pRcp) {
		pRcp->Clear();
		CDialog::OnOK();
	}
	CDialog::OnCancel();
}

void CRecipeIO::OnLoad() {
	p413App->Global.isLoad = true; // 01012025 MORSALIN open DieMap on load
	if (pRcp) {
		pRcp->LoadRecipe();
		if (pRcp->bUseMotorFile) {
			BOOL bRun = p413App->pDMC->bRun;
			pUDev->pInfoPanelSetup->m_Prop3Dlg->StoppingEcho();
			p413App->ResetEchoSamplingProperty(pRcp, NULL);
			if (bRun) {
				pUDev->pInfoPanelSetup->m_Prop3Dlg->StartingEcho();
			}
		}
		CDialog::OnOK();
	}
	CDialog::OnCancel();
}

void CRecipeIO::OnSave() {
	if (pRcp) {
		if (p413App->ValidCheckRcp(*pRcp)) {
			pRcp->SaveRecipe();
		}
		CDialog::OnOK();
	}
	CDialog::OnCancel();
}

void CRecipeIO::OnLoadf() {
	if (pRcp) {
		if (p413App->LoadRecipeFromDataFile(*pRcp)) {
			CDialog::OnOK();
		}
	}
	CDialog::OnCancel();
}

void CRecipeIO::OnLoadq() {
#ifdef _DEV
	if (pRcp) {
		p413App->QuickEdit(pRcp);
		CDialog::OnOK();
	}
	CDialog::OnCancel();
#endif
}

BOOL CRecipeIO::OnInitDialog() {
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}