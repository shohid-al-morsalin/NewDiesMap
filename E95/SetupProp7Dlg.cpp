#include "stdafx.h"
#include <math.h>
#include "e95.h"

#include "413/MotorS2.h"
#include "413App.h"
#include "DuplicateDlg.h"
#include "EchoTest.h"
#include "EditCoordinateDlg.h"
#include "GlobalUIContainer.h"
#include "HelpProbes.h"
#include "MeSetMore.h"
#include "MPoint.h"
#include "XTabCtrl.h"

#include "SetupProp7Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp7Dlg dialog

CSetupProp7Dlg::CSetupProp7Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp7Dlg::IDD, pParent) {
	m_NAve = 8;
	m_Cln = 0;
	m_EnableAutoZ = FALSE;
	m_AutoZTarget = 0.0f;

	for (int i = 0; i < MaXPA; i++) {
		m_F[i] = m_T[i] = 1;
	}
}

void CSetupProp7Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COORTEXT2, m_cCoorText2);
	DDX_Control(pDX, IDC_COORTEXT, m_cCoorText);
	DDX_Control(pDX, IDC_RESETALL, m_cResetAll);
	DDX_Control(pDX, IDC_SELECTEDPT, m_cSelPoint);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_AUTOZTARGET, m_cAutoZTarget);
	DDX_Control(pDX, IDC_MESETBOX, m_cMeSet);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Text(pDX, IDC_NAVE, m_NAve);
	DDX_Check(pDX, IDC_ENABLEAUTOZ, m_EnableAutoZ);
	DDX_Text(pDX, IDC_AUTOZTARGET, m_AutoZTarget);

	DDX_Text(pDX, IDC_FROM1, m_F[0]);
	DDV_MinMaxInt(pDX, m_F[0], 1, 10);
	DDX_Text(pDX, IDC_FROM2, m_F[1]);
	DDV_MinMaxInt(pDX, m_F[1], 1, 10);
	DDX_Text(pDX, IDC_FROM3, m_F[2]);
	DDV_MinMaxInt(pDX, m_F[2], 1, 10);
	DDX_Text(pDX, IDC_FROM4, m_F[3]);
	DDV_MinMaxInt(pDX, m_F[3], 1, 10);
	DDX_Text(pDX, IDC_FROM5, m_F[4]);
	DDV_MinMaxInt(pDX, m_F[4], 1, 10);
	DDX_Text(pDX, IDC_FROM6, m_F[5]);
	DDV_MinMaxInt(pDX, m_F[5], 1, 10);
	DDX_Text(pDX, IDC_FROM7, m_F[6]);
	DDV_MinMaxInt(pDX, m_F[6], 1, 10);
	DDX_Text(pDX, IDC_FROM8, m_F[7]);
	DDV_MinMaxInt(pDX, m_F[7], 1, 10);
	DDX_Text(pDX, IDC_FROM9, m_F[8]);
	DDV_MinMaxInt(pDX, m_F[8], 1, 10);
	DDX_Text(pDX, IDC_FROM10, m_F[9]);
	DDV_MinMaxInt(pDX, m_F[9], 1, 10);
	DDX_Text(pDX, IDC_FROM11, m_F[10]);
	DDV_MinMaxInt(pDX, m_F[10], 1, 10);
	DDX_Text(pDX, IDC_FROM12, m_F[11]);
	DDV_MinMaxInt(pDX, m_F[11], 1, 10);
	DDX_Text(pDX, IDC_FROM13, m_F[12]);
	DDV_MinMaxInt(pDX, m_F[12], 1, 10);
	DDX_Text(pDX, IDC_FROM14, m_F[13]);
	DDV_MinMaxInt(pDX, m_F[13], 1, 10);
	DDX_Text(pDX, IDC_FROM15, m_F[14]);
	DDV_MinMaxInt(pDX, m_F[14], 1, 10);
	DDX_Text(pDX, IDC_FROM16, m_F[15]);
	DDV_MinMaxInt(pDX, m_F[15], 1, 10);

	DDX_Text(pDX, IDC_PROBE1, m_P[0]);
	DDV_MinMaxInt(pDX, m_P[0], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE2, m_P[1]);
	DDV_MinMaxInt(pDX, m_P[1], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE3, m_P[2]);
	DDV_MinMaxInt(pDX, m_P[2], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE4, m_P[3]);
	DDV_MinMaxInt(pDX, m_P[3], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE5, m_P[4]);
	DDV_MinMaxInt(pDX, m_P[4], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE6, m_P[5]);
	DDV_MinMaxInt(pDX, m_P[5], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE7, m_P[6]);
	DDV_MinMaxInt(pDX, m_P[6], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE8, m_P[7]);
	DDV_MinMaxInt(pDX, m_P[7], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE9, m_P[8]);
	DDV_MinMaxInt(pDX, m_P[8], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE10, m_P[9]);
	DDV_MinMaxInt(pDX, m_P[9], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE11, m_P[10]);
	DDV_MinMaxInt(pDX, m_P[10], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE12, m_P[11]);
	DDV_MinMaxInt(pDX, m_P[11], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE13, m_P[12]);
	DDV_MinMaxInt(pDX, m_P[12], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE14, m_P[13]);
	DDV_MinMaxInt(pDX, m_P[13], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE15, m_P[14]);
	DDV_MinMaxInt(pDX, m_P[14], 0, MParam::DISPPRB);
	DDX_Text(pDX, IDC_PROBE16, m_P[15]);
	DDV_MinMaxInt(pDX, m_P[15], 0, MParam::DISPPRB);

	DDX_Text(pDX, IDC_TO1, m_T[0]);
	DDV_MinMaxInt(pDX, m_T[0], 1, 10);
	DDX_Text(pDX, IDC_TO2, m_T[1]);
	DDV_MinMaxInt(pDX, m_T[1], 1, 10);
	DDX_Text(pDX, IDC_TO3, m_T[2]);
	DDV_MinMaxInt(pDX, m_T[2], 1, 10);
	DDX_Text(pDX, IDC_TO4, m_T[3]);
	DDV_MinMaxInt(pDX, m_T[3], 1, 10);
	DDX_Text(pDX, IDC_TO5, m_T[4]);
	DDV_MinMaxInt(pDX, m_T[4], 1, 10);
	DDX_Text(pDX, IDC_TO6, m_T[5]);
	DDV_MinMaxInt(pDX, m_T[5], 1, 10);
	DDX_Text(pDX, IDC_TO7, m_T[6]);
	DDV_MinMaxInt(pDX, m_T[6], 1, 10);
	DDX_Text(pDX, IDC_TO8, m_T[7]);
	DDV_MinMaxInt(pDX, m_T[7], 1, 10);
	DDX_Text(pDX, IDC_TO9, m_T[8]);
	DDV_MinMaxInt(pDX, m_T[8], 1, 10);
	DDX_Text(pDX, IDC_TO10, m_T[9]);
	DDV_MinMaxInt(pDX, m_T[9], 1, 10);
	DDX_Text(pDX, IDC_TO11, m_T[10]);
	DDV_MinMaxInt(pDX, m_T[10], 1, 10);
	DDX_Text(pDX, IDC_TO12, m_T[11]);
	DDV_MinMaxInt(pDX, m_T[11], 1, 10);
	DDX_Text(pDX, IDC_TO13, m_T[12]);
	DDV_MinMaxInt(pDX, m_T[12], 1, 10);
	DDX_Text(pDX, IDC_TO14, m_T[13]);
	DDV_MinMaxInt(pDX, m_T[13], 1, 10);
	DDX_Text(pDX, IDC_TO15, m_T[14]);
	DDV_MinMaxInt(pDX, m_T[14], 1, 10);
	DDX_Text(pDX, IDC_TO16, m_T[15]);
	DDV_MinMaxInt(pDX, m_T[15], 1, 10);

	DDX_Text(pDX, IDC_NAME1, m_N[0]);
	DDX_Text(pDX, IDC_NAME2, m_N[1]);
	DDX_Text(pDX, IDC_NAME3, m_N[2]);
	DDX_Text(pDX, IDC_NAME4, m_N[3]);
	DDX_Text(pDX, IDC_NAME5, m_N[4]);
	DDX_Text(pDX, IDC_NAME6, m_N[5]);
	DDX_Text(pDX, IDC_NAME7, m_N[6]);
	DDX_Text(pDX, IDC_NAME8, m_N[7]);
	DDX_Text(pDX, IDC_NAME9, m_N[8]);
	DDX_Text(pDX, IDC_NAME10, m_N[9]);
	DDX_Text(pDX, IDC_NAME11, m_N[10]);
	DDX_Text(pDX, IDC_NAME12, m_N[11]);
	DDX_Text(pDX, IDC_NAME13, m_N[12]);
	DDX_Text(pDX, IDC_NAME14, m_N[13]);
	DDX_Text(pDX, IDC_NAME15, m_N[14]);
	DDX_Text(pDX, IDC_NAME16, m_N[15]);

	DDX_Text(pDX, IDC_RI1, m_R[0]);
	DDX_Text(pDX, IDC_RI2, m_R[1]);
	DDX_Text(pDX, IDC_RI3, m_R[2]);
	DDX_Text(pDX, IDC_RI4, m_R[3]);
	DDX_Text(pDX, IDC_RI5, m_R[4]);
	DDX_Text(pDX, IDC_RI6, m_R[5]);
	DDX_Text(pDX, IDC_RI7, m_R[6]);
	DDX_Text(pDX, IDC_RI8, m_R[7]);
	DDX_Text(pDX, IDC_RI9, m_R[8]);
	DDX_Text(pDX, IDC_RI10, m_R[9]);
	DDX_Text(pDX, IDC_RI11, m_R[10]);
	DDX_Text(pDX, IDC_RI12, m_R[11]);
	DDX_Text(pDX, IDC_RI13, m_R[12]);
	DDX_Text(pDX, IDC_RI14, m_R[13]);
	DDX_Text(pDX, IDC_RI15, m_R[14]);
	DDX_Text(pDX, IDC_RI16, m_R[15]);
}

BEGIN_MESSAGE_MAP(CSetupProp7Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp7Dlg)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_BN_CLICKED(IDC_LOCATIONS, OnLocations)
	ON_COMMAND(IDC_MARK, OnMark)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_HELPME, OnHelpme)
	ON_COMMAND(IDC_MATCHPATTERN, OnMatchpattern)
	ON_BN_CLICKED(IDC_RENUMBER, OnRenumber)
	ON_BN_CLICKED(IDC_COORDINATES, OnCoordinates)
	ON_BN_CLICKED(IDC_ENABLEAUTOZ, OnEnableautoz)
	ON_EN_KILLFOCUS(IDC_AUTOZTARGET, OnKillfocusAutoztarget)
	ON_EN_KILLFOCUS(IDC_RCP_PEAKNEAR1, OnKillfocusRcpPeaknear1)
	ON_BN_CLICKED(IDC_NEWRECIPE, OnNewrecipe)
	ON_COMMAND(ID_REFRESHUI, OnRefreshui)
	ON_BN_CLICKED(IDC_MORE, OnMore)
	ON_BN_CLICKED(IDC_RESETALL, OnResetall)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList1)
	ON_BN_CLICKED(IDC_SORTDATA, OnSortdata)
	ON_BN_CLICKED(IDC_HELP, OnHelp)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_MESSAGE(ID_DUPLICATE, OnDuplicate)
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)
	ON_MESSAGE(ID_RELOCATETOTOP, OnRelocateToTop)
	ON_MESSAGE(ID_RELOCATETOBACK, OnRelocateToBack)
	ON_MESSAGE(IDC_EDITPOINT, OnEditpoint)
	ON_MESSAGE(IDC_ADDPOINT, OnAddpoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_CHECK1, &CSetupProp7Dlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_MOTIONCONTROL1, &CSetupProp7Dlg::OnBnClickedMotioncontrol1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp7Dlg message handlers
LRESULT CSetupProp7Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		LocalToRecipe();
	}
	bTabSelected = FALSE;
	return 0;
}
#include "ExportPDF.h"
LRESULT CSetupProp7Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	g = &p413App->Global;
	pRcp = &g->RcpSetup;
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		RecipeToLocal();
		bTabSelected = TRUE;
	}
	return 0;
}

void CSetupProp7Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp7Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	//	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_WAFERMAP, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_LOCATIONS, TOP_RIGHT);
	AddAnchor(IDC_TEST, BOTTOM_RIGHT);
	AddAnchor(IDC_COORDINATES, TOP_RIGHT);
	AddAnchor(IDC_NEWRECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_QUICKEDIT, TOP_RIGHT);
	AddAnchor(IDC_RESETALL, TOP_RIGHT);
	AddAnchor(IDC_SORTDATA, TOP_RIGHT);
	AddAnchor(IDC_RENUMBER, TOP_RIGHT);
	AddAnchor(IDC_HELPME, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL1, TOP_RIGHT);//01022025
	

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	CButton* pButton;
	switch (g->CurMeSet) {
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_SET1);
		pButton->SetCheck(TRUE);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_SET2);
		pButton->SetCheck(TRUE);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_SET3);
		pButton->SetCheck(TRUE);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_SET4);
		pButton->SetCheck(TRUE);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_SET5);
		pButton->SetCheck(TRUE);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_SET6);
		pButton->SetCheck(TRUE);
		break;
	case 7:
		pButton = (CButton*)GetDlgItem(IDC_SET7);
		pButton->SetCheck(TRUE);
		break;
	case 8:
		pButton = (CButton*)GetDlgItem(IDC_SET8);
		pButton->SetCheck(TRUE);
		break;
	case 9:
		pButton = (CButton*)GetDlgItem(IDC_SET9);
		pButton->SetCheck(TRUE);
		break;
	}

	RecipeToLocal();

	char* dwCjName[] =
	{
		"#", "Position", "Me", "Pat"
	};
	int nSize[] = { 32, 160, 32, 32 };
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

void CSetupProp7Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp7Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp7Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp7Dlg::OnSet4() {
	SwapMeSet(4);
}

void CSetupProp7Dlg::OnSet5() {
	SwapMeSet(5);
}

void CSetupProp7Dlg::OnSet6() {
	SwapMeSet(6);
}

void CSetupProp7Dlg::OnSet7() {
	SwapMeSet(7);
}

void CSetupProp7Dlg::OnSet8() {
	SwapMeSet(8);
}

void CSetupProp7Dlg::OnSet9() {
	SwapMeSet(9);
}

void CSetupProp7Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}

void CSetupProp7Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
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
	case ID_DUPLICATE:
		OnDuplicate(0, (LPARAM)pMP);
		break;
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
		// 		if (pMP)
		// 		{
		// 			MatchUpdatePoint(nSel);
		// 			m_cPoint.EnsureVisible(nSel,FALSE);
		// 			m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		// 		}
		break;
	case ID_MODIFYPOINT:
		break;
	case ID_ENABLEPT:
		if (pMP) {
			DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
		}
		break;
	case ID_DISABLEPT:
		if (pMP) {
			pMP->MeSet = 0;
			pMP->PatSet = 0;
			pRcp->UpdateControl(m_cPoint);
		}
		break;
	case IDC_GOTOPOINT:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co);
			CString str;
			str.Format("%d", pMP->Co.n);
			m_cSelPoint.SetWindowText(str);
		}
		break;
	case IDC_GOTOXYNOZ:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co, FALSE);
			CString str;
			str.Format("%d", pMP->Co.n);
			m_cSelPoint.SetWindowText(str);
		}
		break;
	case IDC_ALIGN:
		OnMatchpattern();
		break;
	case ID_GOTOALGNUPDATE:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co);
			OnMatchpattern();
			UpdatePoint(&pMP->Co);
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
				OnMatchpattern();
				UpdatePoint(&pMP1->Co);
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
		Renumber();
		break;
	case ID_SETMARKER2:
		pRcp->Patt.MP.SetMarker(pMP, 2);
		Renumber();
		break;
	case ID_SETMARKER3:
		pRcp->Patt.MP.SetMarker(pMP, 3);
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

void CSetupProp7Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
	}
	*pResult = 0;
}

// void CSetupProp7Dlg::OnSaverecipe()
// {
// 	LocalToRecipe();
// 	m_cWaferMap.Redraw();
// 	SwapMeSet(g->CurMeSet);
// 	if (p413App->ValidCheckRcp(g->RcpSetup))
// 	{
// 		pRcp->SaveRecipe();
// 	}
// }

void CSetupProp7Dlg::OnLocations() {
	p413App->LocationProp(g->RcpSetup, m_hWnd);
	Renumber();
}

long CSetupProp7Dlg::OnDeletepoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp7Dlg::OnEditpoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp7Dlg::OnAddpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

void CSetupProp7Dlg::OnMark() {
	if (MarkThisPoint()) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

long CSetupProp7Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

void CSetupProp7Dlg::OnTest() {
	CEchoTest dlg;

	if (dlg.DoModal() == IDOK) {
	}
}

void CSetupProp7Dlg::OnHelpme() {
	CHelpProbes dlg;
	dlg.DoModal();
}

long CSetupProp7Dlg::OnRelocateToBack(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToBack(p);
	return 0;
}

long CSetupProp7Dlg::OnRelocateToTop(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToTop(p);
	return 0;
}

void CSetupProp7Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp7Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp7Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp7Dlg::OnMatchpattern() {
	return;	// Not possible to perform here [6/9/2014 Yuen]
// 	SDoPattrnMatching Param;
//
// 	p413App->pMtr->GetXYPos(&Param.initX,&Param.initY);
// 	p413App->SetupParam(pRcp,Param,nPatSSet);
// 	Param.bApplyOffset = FALSE;
// 	if (pRcp->MatchAlgorithm[g->CurPaSet-1] == 4)
// 	{
// 		Param.bMoveCenter = TRUE;
// 	}
// 	else
// 	{
// 		Param.bMoveCenter = FALSE;
// 	}
// 	Param.Set(&m_cVideo, &m_cImgWnd, &m_cList);
//
// 	p413App->bLoadDIB = FALSE;
// 	short PatMatchRsl = p413App->DoPattrnMatching(Param);
// 	p413App->bLoadDIB = TRUE;
// 	switch (PatMatchRsl)
// 	{
// 	case C413AppPatt::pmOK:
// 		break;
// 	default:
// 		AfxMessageBox("Not Found");
// 		return;
// 		break;
// 	}
}

void CSetupProp7Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp7Dlg::OnRenumber() {
	Renumber();
}

void CSetupProp7Dlg::PositionEcho() {
	p413App->GotoPositionecho();
}

void CSetupProp7Dlg::PositionThinFilm() {
	p413App->GotoPositionthinfilm();
}

// void CSetupProp7Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp7Dlg::OnCoordinates() {
	CEditCoordinateDlg dlg;

	if (dlg.DoModal() == IDOK) {
		switch (dlg.cmd) {
		case CEditCoordinateDlg::REVERSEZ:
			ReverseZ();
			pRcp->UpdateControl(m_cPoint);
			m_cWaferMap.Redraw();
			break;
		case CEditCoordinateDlg::ZEROZ:
			ZeroZ(pRcp);
			pRcp->UpdateControl(m_cPoint);
			m_cWaferMap.Redraw();
			break;
		case CEditCoordinateDlg::OFFSETZ:
		{
			float cz;
			if (p413App->pMtr) {
				p413App->pMtr->GetEchoTopZPosDev(cz);
				OffsetZ(cz);
				pRcp->UpdateControl(m_cPoint);
				m_cWaferMap.Redraw();
			}
		}
		break;
		case CEditCoordinateDlg::APPLYXYOFFSET:
			if (AfxMessageBox("Are you sure", MB_YESNO) == IDYES) {
				float x, y;
				if (p413App->pMtr) {
					p413App->pMtr->GetXYPos(&x, &y);
					OffsetXY(x, y);
				}
				pRcp->UpdateControl(m_cPoint);
				m_cWaferMap.Redraw();
			}
			break;
		}
		RecipeToLocal();
	}
}

void CSetupProp7Dlg::OnEnableautoz() {
	UpdateData(TRUE);
	m_cAutoZTarget.EnableWindow(m_EnableAutoZ);
	// Save selection in recipe [9/14/2011 Administrator]
	pRcp->bEnableAutoZ = m_EnableAutoZ;
}

void CSetupProp7Dlg::OnKillfocusAutoztarget() {
	UpdateData(TRUE);
	// Save selection in recipe [9/14/2011 Administrator]
	pRcp->fAutoZValue = m_AutoZTarget;
}

void CSetupProp7Dlg::OnKillfocusRcpPeaknear1() {
	UpdateData(TRUE);
}

void CSetupProp7Dlg::OnNewrecipe() {
	LocalToRecipe();
	m_cWaferMap.Redraw();
	SwapMeSet(g->CurMeSet);
	RecipeIO();
	// 	pRcp->Clear();
	// 	RecipeToLocal();
}

void CSetupProp7Dlg::OnRefreshui() {
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

void CSetupProp7Dlg::OnMore() {
	CMeSetMore dlg;

	dlg.MeS = g->CurMeSet;
	dlg.DoModal();
	m_cWaferMap.Redraw();
}

long CSetupProp7Dlg::OnDuplicate(WPARAM wP, LPARAM lP) {
	CDuplicateDlg dlg;

	dlg.m_Value = 1;
	if (dlg.DoModal() != IDOK) {
		return -1;
	}
	if ((dlg.m_Value < 1) || (dlg.m_Value > 2048)) {
		return -2;
	}
	// Duplicate points [10/18/2012 Yuen]
	CPattern* pPat = &pRcp->Patt;
	CMPoint* p = (CMPoint*)lP;
	if (p) {
		for (int i = 0; i < dlg.m_Value; i++) {
			CMPoint* q = new CMPoint;
			if (q) {
				*q = *p;
				pPat->MP.AddTailPoint(q);
			}
		}
	}
	Renumber();
	return 0;
}

// void CSetupProp7Dlg::OnQuickedit()
// {
// 	p413App->QuickEdit(pRcp);
// 	RecipeToLocal();
// }

void CSetupProp7Dlg::OnResetall() {
	m_cResetAll.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->ResetAll();
	m_cResetAll.EnableWindow(TRUE);
}

void CSetupProp7Dlg::OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult) {
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey) {
	case VK_DELETE:
	{
		int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		if (nSel > -1) {
			if (GetAsyncKeyState(VK_LCONTROL)) {
				DeletePoint(nSel);
				m_cPoint.DeleteItem(nSel);
				m_cWaferMap.Redraw();
			}
		}
	}
	break;
	case 66:  // B button [7/17/2013 Yuen]
	{
		int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		if (nSel > -1) {
			if (GetAsyncKeyState(VK_LCONTROL)) {
				CMPoint* pMP = FindMPoint(nSel);
				if (pMP) {
					RelocateToBack(&pMP->Co);
				}
			}
		}
	}
	break;
	}
	*pResult = 0;
}

void CSetupProp7Dlg::RecipeToLocal() {
	int i;
	CString str;
	short MeS = g->CurMeSet - 1;

	m_NAve = pRcp->MeParam[MeS].NAve;
	m_EnableAutoZ = pRcp->bEnableAutoZ;
	m_AutoZTarget = pRcp->fAutoZValue;
	if (pRcp->bEnableAutoZ) {
		m_cAutoZTarget.EnableWindow(TRUE);
	}
	else {
		m_cAutoZTarget.EnableWindow(FALSE);
	}

	str.Format("Content of Measurement Set (MeSet) %d", g->CurMeSet);
	m_cMeSet.SetWindowText(str);

	for (i = 0; i < MaXPA; i++) {
		MParam* mp = &pRcp->MeParam[MeS].MPa[i];
		m_P[i] = mp->Prb;
		m_F[i] = mp->Fr;
		m_T[i] = mp->To;
		m_R[i] = mp->RI;
		m_N[i] = mp->Name;
	}

	pRcp->UpdateControl(m_cPoint);

	m_cMessage.SetWindowText(pRcp->BaseLineFile);
	m_cWaferMap.Redraw();

	UpdateData(FALSE);
}

void CSetupProp7Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	int i;
	short MeS = g->CurMeSet - 1;
	for (i = 0; i < MaXPA; i++) {
		MParam* mp = &pRcp->MeParam[MeS].MPa[i];
		mp->Prb = (MParam::PRBNUM)m_P[i];
		mp->Fr = m_F[i];
		mp->To = m_T[i];
		mp->RI = m_R[i];
		mp->Name = m_N[i];
	}
	pRcp->Desc.Clear();

	pRcp->MeParam[MeS].NAve = m_NAve;
	pRcp->bEnableAutoZ = m_EnableAutoZ;
	pRcp->fAutoZValue = m_AutoZTarget;
}

void CSetupProp7Dlg::OnSortdata() {
	pRcp->Patt.MP.Sort();
	pRcp->Patt.MP.EnsureData();
	pRcp->Renumber();
	RecipeToLocal();
}

void CSetupProp7Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
	//10032025
	int rowCount = m_cPoint.GetItemCount();
	CMPoint* pMP = FindMPoint(rowCount - 1);
	if (pMP) {
		pMP->MeSet = 1;
		DoMeasurmentSetOne(pMP, rowCount - 1, m_cPoint, &m_cWaferMap);
	}
	
	
	//
}

void CSetupProp7Dlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	CDiceLoc Loc;

	CMPoint* p = pRcp->Patt.GetMarker(1);
	if (!p) { return; }// No marker define [7/23/2013 Yuen]
	Loc.Pt[1].Set(p->Co.x, p->Co.y);

	p = pRcp->Patt.GetMarker(2);
	if (!p) { return; }
	Loc.Pt[2].Set(p->Co.x, p->Co.y);

	p = pRcp->Patt.GetMarker(3);
	if (!p) { return; }
	Loc.Pt[3].Set(p->Co.x, p->Co.y);

	Loc.Calc();

	short nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	p = FindMPoint(nSel);
	if (p) {
		float X, Y;
		X = p->Co.x;
		Y = p->Co.y;
		if (Loc.GetCoor(X, Y)) {
			CString str;
			str.Format("%.3f %.3f    %.3f %.3f", X, Y, X - p->Co.x, Y - p->Co.y);
			m_cCoorText.SetWindowText(str);
			str.Format("%.3f %.3f %.3f %.3f", Loc.dX, Loc.dXy, Loc.dY, Loc.dYx);
			m_cCoorText2.SetWindowText(str);
		}
	}

	*pResult = 0;
}

//[[20241213/Sihab
void CSetupProp7Dlg::OnBnClickedCheck1()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_CHECK1);
	if (pCheckbox)
	{
		
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->bMask = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->bMask = FALSE;

		}
	}

}
//]]

//[01022025
void CSetupProp7Dlg::OnBnClickedMotioncontrol1()
{
	ShowMotionControl(this);
}
//]