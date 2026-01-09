// SetupProp71.cpp : implementation file
//

#include "stdafx.h"
#include <MATH.H>
#include "e95.h"
#include "413App.h"
#include "SetupProp3Dlg.h"
#include "SetupProp71Dlg.h"
#include "GlobalUIContainer.h"
#include "InfoPanelSetup.h"

#include "MPoint.h"
#include "..\413\DMCMotorBase.h"
#include "SelectMeasurementDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp71Dlg dialog

CSetupProp71Dlg::CSetupProp71Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp71Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp71Dlg)
	m_NumLine = 0;
	m_PointPerLine = 0;
	m_ScanLength = 0;
	//}}AFX_DATA_INIT
}

void CSetupProp71Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp71Dlg)
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Text(pDX, IDC_NUMLINES, m_NumLine);
	DDX_Text(pDX, IDC_POINTSPERLINE, m_PointPerLine);
	DDX_Text(pDX, IDC_SCANLENGTH, m_ScanLength);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp71Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp71Dlg)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_KILLFOCUS(IDC_NUMLINES, OnKillfocusNumlines)
	ON_EN_KILLFOCUS(IDC_POINTSPERLINE, OnKillfocusPointsperline)
	ON_EN_KILLFOCUS(IDC_SCANLENGTH, OnKillfocusScanlength)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_RELOCATETOTOP, OnRelocateToTop)
	ON_MESSAGE(ID_RELOCATETOBACK, OnRelocateToBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp71Dlg message handlers
LRESULT CSetupProp71Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
	}
	bTabSelected = FALSE;

	return 0;
}

LRESULT CSetupProp71Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
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

void CSetupProp71Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp71Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_WAFERMAP, TOP_LEFT, BOTTOM_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	char* dwCjName[] =
	{
		"#", "Position", /*"Me",*/ "Pat"
	};
	int nSize[] = { 32, 160, /*32,*/ 32 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cPoint.InsertColumn(i, &nListColumn);
	}
	m_cPoint.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp71Dlg::OnSaverecipe() {
	LocalToRecipe();
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe(FALSE);
	// 	}
}

// void CSetupProp71Dlg::OnLoadrecipe()
// {
// 	if (pRcp->LoadRecipe())
// 	{
// 		if (pRcp->bUseMotorFile)
// 		{
// 			BOOL bRun = p413App->pDMC->bRun;
// 			pUDev->pInfoPanelSetup->m_Prop3Dlg->StoppingEcho();
// 			p413App->ResetEchoSamplingProperty(pRcp,NULL);
// 			if (bRun)
// 			{
// 				pUDev->pInfoPanelSetup->m_Prop3Dlg->StartingEcho();
// 			}
// 		}
// 		RecipeToLocal();
// 	}
// }

void CSetupProp71Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	pRcp->NumLines = m_NumLine;
	pRcp->PointsPerLine = m_PointPerLine;
	pRcp->ScanLength = m_ScanLength;
}

void CSetupProp71Dlg::RecipeToLocal() {
	m_NumLine = pRcp->NumLines;
	m_PointPerLine = pRcp->PointsPerLine;
	m_ScanLength = pRcp->ScanLength;

	UpdateData(FALSE);

	m_cWaferMap.Redraw();
	pRcp->UpdateControl(m_cPoint);
}

void CSetupProp71Dlg::OnSet() {
	LocalToRecipe();

	if ((m_PointPerLine % 2)) {
		m_PointPerLine++;
		UpdateData(FALSE);
	}

	if ((m_NumLine < 1) || (m_PointPerLine < 5) || (m_ScanLength < 1)) {
		return;
	}

	pRcp->Patt.Clear();

	short idx = 1;
	short nP = m_PointPerLine / 2;

	for (int i = 0; i < m_NumLine; i++) {
		float angle = i * 180 / float(m_NumLine);
		for (int j = -nP; j <= nP; j++) {
			float xx, yy;
			float radius = (m_ScanLength / 2.0f) * j / float(nP);
			g->Polar2Cartesian(radius, angle, xx, yy);
			pRcp->Patt.AddMPoint(idx, xx, yy, 0/*, 0 ,0*/);
			idx++;
		}
	}

	m_cWaferMap.Redraw();
	pRcp->UpdateControl(m_cPoint);
}

void CSetupProp71Dlg::OnClear() {
	pRcp->Patt.Clear();

	m_NumLine = 0;
	m_PointPerLine = 0;
	m_ScanLength = 0;

	LocalToRecipe();

	m_cWaferMap.Redraw();
	pRcp->UpdateControl(m_cPoint);
}

void CSetupProp71Dlg::OnKillfocusNumlines() {
	LocalToRecipe();
}

void CSetupProp71Dlg::OnKillfocusPointsperline() {
	LocalToRecipe();
}

void CSetupProp71Dlg::OnKillfocusScanlength() {
	LocalToRecipe();
}

void CSetupProp71Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENU413);
	pPopUp = menu.GetSubMenu(2);
	if (!pPopUp) return;
	ClientToScreen(&point);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
	switch (res) {
	case ID_RELOCATETOBACK:
		if (pMP) {
			RelocateToBack(&pMP->Co);
		}
		break;
	case ID_RELOCATETOTOP:
		if (pMP) {
			RelocateToTop(&pMP->Co);
		}
		break;
	case ID_UPDATEPOINT:
		if (pMP) {
			UpdatePoint(&pMP->Co);
			m_cPoint.EnsureVisible(nSel, FALSE);
			m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		}
		break;
	case ID_MATCHUPDATE:
		break;
	case ID_MODIFYPOINT:
		break;
	case IDC_GOTOPOINT:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co);
		}
		break;
	case IDC_GOTOXYNOZ:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co, FALSE);
		}
		break;
	case IDC_ALIGN:
		break;
	case ID_GOTOALGNUPDATE:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co);
		}
		break;
	case ID_ALGNUPDATEALL:
	{
		int n = 0;
		CMPoint* pMP1 = NULL;
		do {
			pMP1 = FindMPoint(n++);
			if (pMP1) {
				GotoPointB(NULL, &pMP1->Co);
			}
		} while (pMP1);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
	break;
	case ID_ADDZPOSITION:
		Addzposition(pMP);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		break;
	case ID_SETMARKER1:
		pRcp->Patt.MP.SetMarker(pMP, 1);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_SETMARKER2:
		pRcp->Patt.MP.SetMarker(pMP, 2);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_SETMARKER3:
		pRcp->Patt.MP.SetMarker(pMP, 3);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER1:
		pRcp->Patt.MP.ClearMarker(pMP, 1);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER2:
		pRcp->Patt.MP.ClearMarker(pMP, 2);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	case ID_CLEARMARKER3:
		pRcp->Patt.MP.ClearMarker(pMP, 3);
		pRcp->UpdateControl(m_cPoint);
		Renumber();
		break;
	}

	*pResult = 0;
}

void CSetupProp71Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
	}

	*pResult = 0;
}

long CSetupProp71Dlg::OnRelocateToBack(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToBack(p);
	return 0;
}

long CSetupProp71Dlg::OnRelocateToTop(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToTop(p);
	return 0;
}

void CSetupProp71Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp71Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp71Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp71Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		Renumber();
	}
}

// void CSetupProp71Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp71Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}