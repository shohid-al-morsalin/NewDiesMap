// SetupProp52Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp52Dlg.h"

#include "XTabCtrl.h"
#include "413App.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "..\SYS\CUMMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp52Dlg dialog

CSetupProp52Dlg::CSetupProp52Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp52Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp52Dlg)
	m_DI = 0;
	m_DT = 0;
	m_MatchGOF = 0.0f;
	m_MResolution = 0;
	m_Depth = 0;
	m_Distance = 0.0f;
	ECHdX = 0.0f;
	ECHdY = 0.0f;
	TFdX = 0.0f;
	TFdY = 0.0f;
	m_StepSize = 0.0f;
	m_SearchN = 0;
	m_FastMatch = FALSE;
	m_STIOffset = 0.0f;
	m_STIOffsetY = 0.0f;
	m_ODOffset = 0.0f;
	m_ODOffsetY = 0.0f;
	//}}AFX_DATA_INIT
}

void CSetupProp52Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp52Dlg)
	DDX_Control(pDX, IDC_CLEARSETTING2, m_cClear2);
	DDX_Control(pDX, IDC_CLEARSETTING, m_cClear1);
	DDX_Control(pDX, IDC_SETTING, m_cSetting);
	DDX_Text(pDX, IDC_MATCHDI, m_DI);
	DDX_Text(pDX, IDC_MATCHDT, m_DT);
	DDX_Text(pDX, IDC_MATCHGOF, m_MatchGOF);
	DDX_Text(pDX, IDC_MATCHRESOLUTION, m_MResolution);
	DDX_Text(pDX, IDC_DEPTH, m_Depth);
	DDX_Text(pDX, IDC_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_OFFSETX2, ECHdX);
	DDX_Text(pDX, IDC_OFFSETY2, ECHdY);
	DDX_Text(pDX, IDC_OFFSETX, TFdX);
	DDX_Text(pDX, IDC_OFFSETY, TFdY);
	DDX_Text(pDX, IDC_RCP_SEARCH_STEPSIZE, m_StepSize);
	DDX_Text(pDX, IDC_RCP_SEARCH_AROUND, m_SearchN);
	DDX_Check(pDX, IDC_FASTMATCH, m_FastMatch);
	DDX_Control(pDX, IDC_INITIALEXPOSUREAFTERFOCUS2, m_cInitialExposureAfterFocus2);
	DDX_Control(pDX, IDC_INITIALFOCUS2, m_cInitialFocus2);
	DDX_Control(pDX, IDC_INITIALEXPOSUREAFTERFOCUS, m_InitialExposureAfterFocus);
	DDX_Control(pDX, IDC_INITIALFOCUS, m_cInitialFocus);
	DDX_Control(pDX, IDC_INITIALEXPOSUREBEFOREFOCUS, m_cInitialExposeB4Focus);
	DDX_Control(pDX, IDC_MATCHATOFFSET, m_cMatchAtOffset);
	DDX_Text(pDX, IDC_STIOFFSET, m_STIOffset);
	DDX_Text(pDX, IDC_STIOFFSETY, m_STIOffsetY);
	DDX_Text(pDX, IDC_ODOFFSET, m_ODOffset);
	DDX_Text(pDX, IDC_ODOFFSETY, m_ODOffsetY);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp52Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp52Dlg)
	ON_EN_KILLFOCUS(IDC_MATCHDI, OnKillfocusMatchdi)
	ON_EN_KILLFOCUS(IDC_MATCHDT, OnKillfocusMatchdt)
	ON_EN_KILLFOCUS(IDC_MATCHGOF, OnKillfocusMatchgof)
	ON_EN_KILLFOCUS(IDC_MATCHRESOLUTION, OnKillfocusMatchresolution)
	ON_EN_KILLFOCUS(IDC_DEPTH, OnKillfocusDepth)
	ON_EN_KILLFOCUS(IDC_DISTANCE, OnKillfocusDistance)
	ON_BN_CLICKED(IDC_PATMETHOD1, OnPatmethod1)
	ON_BN_CLICKED(IDC_PATMETHOD2, OnPatmethod2)
	ON_EN_KILLFOCUS(IDC_OFFSETX2, OnKillfocusOffsetx2)
	ON_EN_KILLFOCUS(IDC_OFFSETY2, OnKillfocusOffsety2)
	ON_EN_KILLFOCUS(IDC_OFFSETX, OnKillfocusOffsetx)
	ON_EN_KILLFOCUS(IDC_OFFSETY, OnKillfocusOffsety)
	ON_EN_KILLFOCUS(IDC_RCP_SEARCH_STEPSIZE, OnKillfocusRcpSearchStepsize)
	ON_EN_KILLFOCUS(IDC_RCP_SEARCH_AROUND, OnKillfocusRcpSearchAround)
	ON_BN_CLICKED(IDC_FASTMATCH, OnFastmatch)
	ON_BN_CLICKED(IDC_PATSET1, OnPatset1)
	ON_BN_CLICKED(IDC_PATSET2, OnPatset2)
	ON_BN_CLICKED(IDC_PATSET3, OnPatset3)
	ON_BN_CLICKED(IDC_PATSET4, OnPatset4)
	ON_BN_CLICKED(IDC_PATSET5, OnPatset5)
	ON_BN_CLICKED(IDC_PATSET6, OnPatset6)
	ON_BN_CLICKED(IDC_PATSET7, OnPatset7)
	ON_BN_CLICKED(IDC_PATSET8, OnPatset8)
	ON_BN_CLICKED(IDC_PATSET9, OnPatset9)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_CLEARSETTING2, OnClearsetting)
	ON_BN_CLICKED(IDC_CLEARSETTING2, OnClearsetting2)
	ON_BN_CLICKED(IDC_PATMETHOD4, OnPatmethod4)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREBEFOREFOCUS, OnInitialexposurebeforefocus)
	ON_BN_CLICKED(IDC_INITIALFOCUS, OnInitialfocus)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREAFTERFOCUS, OnInitialexposureafterfocus)
	ON_BN_CLICKED(IDC_INITIALFOCUS2, OnInitialfocus2)
	ON_BN_CLICKED(IDC_INITIALEXPOSUREAFTERFOCUS2, OnInitialexposureafterfocus2)
	ON_BN_CLICKED(IDC_MATCHATOFFSET, OnMatchatoffset)
	ON_EN_KILLFOCUS(IDC_ODOFFSET, OnKillfocusOdoffset)
	ON_EN_KILLFOCUS(IDC_ODOFFSETY, OnKillfocusOdoffsety)
	ON_EN_KILLFOCUS(IDC_STIOFFSET, OnKillfocusStioffset)
	ON_EN_KILLFOCUS(IDC_STIOFFSETY, OnKillfocusStioffsety)
	ON_BN_CLICKED(IDC_PATMETHOD5, OnPatmethod5)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp52Dlg message handlers
long CSetupProp52Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		RecipeToLocal();

		bTabSelected = TRUE;
	}
	return 0;
}

long CSetupProp52Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

void CSetupProp52Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp52Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (Var.UIMode > 1) {
		m_cClear1.ShowWindow(SW_SHOW);
		m_cClear2.ShowWindow(SW_SHOW);
	}

	RecipeToLocal();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp52Dlg::OnKillfocusMatchdi() {
	UpdateData(TRUE);
	pRcp->SubSamI[g->CurPaSet - 1] = m_DI;
}

void CSetupProp52Dlg::OnKillfocusMatchdt() {
	UpdateData(TRUE);
	pRcp->SubSamS[g->CurPaSet - 1] = m_DT;
}

void CSetupProp52Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	pRcp->POffsetX[g->CurPaSet - 1][0] = m_ODOffset;
	pRcp->POffsetX[g->CurPaSet - 1][1] = m_STIOffset;
	pRcp->POffsetY[g->CurPaSet - 1][0] = m_ODOffsetY;
	pRcp->POffsetY[g->CurPaSet - 1][1] = m_STIOffsetY;

	pRcp->SubSamI[g->CurPaSet - 1] = m_DI;
	pRcp->SubSamS[g->CurPaSet - 1] = m_DT;

	pRcp->GoFScore[g->CurPaSet - 1] = m_MatchGOF;
	pRcp->MatchRes[g->CurPaSet - 1] = m_MResolution;
	pRcp->SearchDept[g->CurPaSet - 1] = m_Depth;
	pRcp->SearchDist[g->CurPaSet - 1] = m_Distance;

	g->ScrnCalibTF.dX = TFdX;
	g->ScrnCalibTF.dY = TFdY;

	g->ScrnCalibECH.dX = ECHdX;
	g->ScrnCalibECH.dY = ECHdY;

	pRcp->nTrialPat = m_SearchN;
	pRcp->fStepSizePat = m_StepSize;

	pRcp->bFastPMatch = m_FastMatch;
}

void CSetupProp52Dlg::RecipeToLocal() {
	CString str;

	str.Format("PaSet%d setting", g->CurPaSet);
	m_cSetting.SetWindowText(str);

	m_DI = pRcp->SubSamI[g->CurPaSet - 1];
	m_DT = pRcp->SubSamS[g->CurPaSet - 1];

	((CButton*)GetDlgItem(IDC_PATMETHOD1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD2))->SetCheck(FALSE);
	// 	((CButton *)GetDlgItem(IDC_PATMETHOD3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATMETHOD5))->SetCheck(FALSE);
	switch (pRcp->MatchAlgorithm[g->CurPaSet - 1]) {
	case 0:
		((CButton*)GetDlgItem(IDC_PATMETHOD1))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_PATMETHOD2))->SetCheck(TRUE);
		break;
		// 	case 2:
		// 		((CButton *)GetDlgItem(IDC_PATMETHOD3))->SetCheck(TRUE);
		// 		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATMETHOD4))->SetCheck(TRUE);
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_PATMETHOD5))->SetCheck(TRUE);
		break;
	}

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPB4FOCUS) {
		m_cInitialExposeB4Focus.SetCheck(TRUE);
	}
	else {
		m_cInitialExposeB4Focus.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAFOCUS) {
		m_InitialExposureAfterFocus.SetCheck(TRUE);
	}
	else {
		m_InitialExposureAfterFocus.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCB4MATCH) {
		m_cInitialFocus.SetCheck(TRUE);
	}
	else {
		m_cInitialFocus.SetCheck(FALSE);
	}

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAMATCH) {
		m_cInitialExposureAfterFocus2.SetCheck(TRUE);
	}
	else {
		m_cInitialExposureAfterFocus2.SetCheck(FALSE);
	}
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCAMATCH) {
		m_cInitialFocus2.SetCheck(TRUE);
	}
	else {
		m_cInitialFocus2.SetCheck(FALSE);
	}

	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		m_cMatchAtOffset.SetCheck(TRUE);
	}
	else {
		m_cMatchAtOffset.SetCheck(FALSE);
	}

	m_MatchGOF = pRcp->GoFScore[g->CurPaSet - 1];
	m_MResolution = pRcp->MatchRes[g->CurPaSet - 1];
	m_Depth = pRcp->SearchDept[g->CurPaSet - 1];
	m_Distance = pRcp->SearchDist[g->CurPaSet - 1];

	TFdX = g->ScrnCalibTF.dX;
	TFdY = g->ScrnCalibTF.dY;

	ECHdX = g->ScrnCalibECH.dX;
	ECHdY = g->ScrnCalibECH.dY;

	m_SearchN = pRcp->nTrialPat;
	m_StepSize = pRcp->fStepSizePat;

	m_FastMatch = pRcp->bFastPMatch;

	m_ODOffset = pRcp->POffsetX[g->CurPaSet - 1][0];
	m_STIOffset = pRcp->POffsetX[g->CurPaSet - 1][1];
	m_ODOffsetY = pRcp->POffsetY[g->CurPaSet - 1][0];
	m_STIOffsetY = pRcp->POffsetY[g->CurPaSet - 1][1];

	UpdateData(FALSE);
}

void CSetupProp52Dlg::OnKillfocusMatchgof() {
	UpdateData(TRUE);
	pRcp->GoFScore[g->CurPaSet - 1] = m_MatchGOF;
}

void CSetupProp52Dlg::OnKillfocusMatchresolution() {
	UpdateData(TRUE);
	pRcp->MatchRes[g->CurPaSet - 1] = m_MResolution;
}

void CSetupProp52Dlg::OnKillfocusDepth() {
	UpdateData(TRUE);
	pRcp->SearchDept[g->CurPaSet - 1] = m_Depth;
}

void CSetupProp52Dlg::OnKillfocusDistance() {
	UpdateData(TRUE);
	pRcp->SearchDist[g->CurPaSet - 1] = m_Distance;
}

void CSetupProp52Dlg::OnPatmethod1() {
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 0;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatmethod2() {
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 1;
	RecipeToLocal();
}

// void CSetupProp52Dlg::OnPatmethod3()
// {
// 	pRcp->MatchAlgorithm[g->CurPaSet-1] &= 0x4000;
// 	RecipeToLocal();
// }

void CSetupProp52Dlg::OnPatmethod4() {
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 3;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnKillfocusOffsetx2() {
	UpdateData(TRUE);
	g->ScrnCalibECH.dX = ECHdX;
	g->SaveINI();
}

void CSetupProp52Dlg::OnKillfocusOffsety2() {
	UpdateData(TRUE);
	g->ScrnCalibECH.dY = ECHdY;
	g->SaveINI();
}

void CSetupProp52Dlg::OnKillfocusOffsetx() {
	UpdateData(TRUE);
	g->ScrnCalibTF.dX = TFdX;
	g->SaveINI();
}

void CSetupProp52Dlg::OnKillfocusOffsety() {
	UpdateData(TRUE);
	g->ScrnCalibTF.dY = TFdY;
	g->SaveINI();
}

void CSetupProp52Dlg::OnKillfocusRcpSearchStepsize() {
	UpdateData(TRUE);
	pRcp->fStepSizePat = m_StepSize;
}

void CSetupProp52Dlg::OnKillfocusRcpSearchAround() {
	UpdateData(TRUE);
	pRcp->nTrialPat = m_SearchN;
}

void CSetupProp52Dlg::OnFastmatch() {
	UpdateData(TRUE);
	pRcp->bFastPMatch = m_FastMatch;
}

void CSetupProp52Dlg::OnPatset1() {
	g->CurPaSet = 1;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset2() {
	g->CurPaSet = 2;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset3() {
	g->CurPaSet = 3;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset4() {
	g->CurPaSet = 4;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset5() {
	g->CurPaSet = 5;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset6() {
	g->CurPaSet = 6;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset7() {
	g->CurPaSet = 7;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset8() {
	g->CurPaSet = 8;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnPatset9() {
	g->CurPaSet = 9;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnSaverecipe() {
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe(FALSE);
	// 	}
}

// void CSetupProp52Dlg::OnLoadrecipe()
// {
// 	pRcp->LoadRecipe();
// 	RecipeToLocal();
// }

// void CSetupProp52Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp52Dlg::OnClearsetting() {
	pRcp->GoFScore[g->CurPaSet - 1] = 0;
	pRcp->MatchRes[g->CurPaSet - 1] = 3;
	pRcp->SearchDept[g->CurPaSet - 1] = 6;
	pRcp->SearchDist[g->CurPaSet - 1] = 0;
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 0;
	pRcp->SubSamI[g->CurPaSet - 1] = 2;
	pRcp->SubSamS[g->CurPaSet - 1] = 5;
	RecipeToLocal();
}

void CSetupProp52Dlg::OnClearsetting2() {
	for (int i = 0; i < MAXMEPASET; i++) {
		pRcp->GoFScore[i] = 0;
		pRcp->MatchRes[i] = 3;
		pRcp->SearchDept[i] = 6;
		pRcp->SearchDist[i] = 0;
		pRcp->MatchAlgorithm[i] = 0;
		pRcp->SubSamI[i] = 2;
		pRcp->SubSamS[i] = 5;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnInitialexposurebeforefocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPB4FOCUS) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPB4FOCUS;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPB4FOCUS;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnInitialfocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCB4MATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::FOCB4MATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::FOCB4MATCH;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnInitialexposureafterfocus() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAFOCUS) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPAFOCUS;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPAFOCUS;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnInitialfocus2() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::FOCAMATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::FOCAMATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::FOCAMATCH;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnInitialexposureafterfocus2() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::EXPAMATCH) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::EXPAMATCH;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::EXPAMATCH;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnMatchatoffset() {
	if (pRcp->PatMatOpt[g->CurPaSet - 1] & CRecipe413::MATCHATOFFSET) {
		pRcp->PatMatOpt[g->CurPaSet - 1] &= ~CRecipe413::MATCHATOFFSET;
	}
	else {
		pRcp->PatMatOpt[g->CurPaSet - 1] |= CRecipe413::MATCHATOFFSET;
	}
	RecipeToLocal();
}

void CSetupProp52Dlg::OnKillfocusOdoffset() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][0] = m_ODOffset;
}

void CSetupProp52Dlg::OnKillfocusOdoffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][0] = m_ODOffsetY;
}

void CSetupProp52Dlg::OnKillfocusStioffset() {
	UpdateData(TRUE);
	pRcp->POffsetX[g->CurPaSet - 1][1] = m_STIOffset;
}

void CSetupProp52Dlg::OnKillfocusStioffsety() {
	UpdateData(TRUE);
	pRcp->POffsetY[g->CurPaSet - 1][1] = m_STIOffsetY;
}

void CSetupProp52Dlg::OnPatmethod5() {
	pRcp->MatchAlgorithm[g->CurPaSet - 1] = 4;
	RecipeToLocal();
}