// SetupProp91Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp91Dlg.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "413App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp91Dlg dialog

CSetupProp91Dlg::CSetupProp91Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp91Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp91Dlg)
	m_RefPt1 = 0;
	m_RefPt2 = 0;
	m_RefPt3 = 0;
	//}}AFX_DATA_INIT
}

void CSetupProp91Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp91Dlg)
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_POINT3, m_cPoint3);
	DDX_Control(pDX, IDC_POINT2, m_cPoint2);
	DDX_Control(pDX, IDC_POINT1, m_cPoint1);
	DDX_Text(pDX, IDC_POINT1, m_RefPt1);
	DDX_Text(pDX, IDC_POINT2, m_RefPt2);
	DDX_Text(pDX, IDC_POINT3, m_RefPt3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp91Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp91Dlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_EN_KILLFOCUS(IDC_POINT1, OnKillfocusPoint1)
	ON_EN_KILLFOCUS(IDC_POINT2, OnKillfocusPoint2)
	ON_EN_KILLFOCUS(IDC_POINT3, OnKillfocusPoint3)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp91Dlg message handlers

void CSetupProp91Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

long CSetupProp91Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
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

long CSetupProp91Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
		bTabSelected = FALSE;
	}
	return 0;
}

void CSetupProp91Dlg::LocalToRecipe() {}

void CSetupProp91Dlg::RecipeToLocal() {
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.Redraw();

	m_RefPt1 = pRcp->RefPt1;
	m_RefPt2 = pRcp->RefPt2;
	m_RefPt3 = pRcp->RefPt3;

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(FALSE);
	switch (pRcp->BFPlane) {
	case CRecipe413::bfBESTFIT:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		EnabeSpecifiedPlane(FALSE);
		break;
	case CRecipe413::bfZEROMIN:
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
		EnabeSpecifiedPlane(FALSE);
		break;
	case CRecipe413::bf3POINTS:
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
		EnabeSpecifiedPlane(TRUE);
		break;
	}
	switch (pRcp->BFPlaneCalc) {
	case CRecipe413::bfcSURFACE:
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
		break;
	case CRecipe413::bfcMEDIAN:
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(TRUE);
		break;
	}
	UpdateData(FALSE);
}

BOOL CSetupProp91Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	RecipeToLocal();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp91Dlg::OnRadio1() {
	pRcp->BFPlane = CRecipe413::bfBESTFIT;
	EnabeSpecifiedPlane(FALSE);
}

void CSetupProp91Dlg::OnRadio2() {
	pRcp->BFPlane = CRecipe413::bfZEROMIN;
	EnabeSpecifiedPlane(FALSE);
}

void CSetupProp91Dlg::OnRadio3() {
	pRcp->BFPlane = CRecipe413::bf3POINTS;
	EnabeSpecifiedPlane(TRUE);
}

void CSetupProp91Dlg::OnKillfocusPoint1() {
	UpdateData(TRUE);
	pRcp->RefPt1 = m_RefPt1;
}

void CSetupProp91Dlg::OnKillfocusPoint2() {
	UpdateData(TRUE);
	pRcp->RefPt2 = m_RefPt2;
}

void CSetupProp91Dlg::OnKillfocusPoint3() {
	UpdateData(TRUE);
	pRcp->RefPt3 = m_RefPt3;
}

void CSetupProp91Dlg::EnabeSpecifiedPlane(BOOL bEn) {
	m_cPoint1.EnableWindow(bEn);
	m_cPoint2.EnableWindow(bEn);
	m_cPoint3.EnableWindow(bEn);
}

void CSetupProp91Dlg::OnSaverecipe() {
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(*pRcp))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp91Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp91Dlg::OnRadio4() {
	pRcp->BFPlaneCalc = CRecipe413::bfcSURFACE;
}

void CSetupProp91Dlg::OnRadio5() {
	pRcp->BFPlaneCalc = CRecipe413::bfcMEDIAN;
}

BOOL CSetupProp91Dlg::DestroyWindow() {
	// TODO: Add your specialized code here and/or call the base class

	return CResizableDialog::DestroyWindow();
}