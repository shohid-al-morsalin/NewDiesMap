// ViewContentDlg413.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ViewContentDlg413.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "InfoPanelView.h"
#include "..\SYS\CUMMsg.h"

#include "MPoint.h"
#include "Data.h"
#include "WaferViewOptions.h"
#include "MaxMinPa.h"
#include "413App.h"
#include "..\128\gem\OLnClt.h"

#include "ViewBaseline.h"

#include "..\MFC\KeyPad.h"
#include "..\PLT\HSL2RGB.h"
#include "WMap3DDlg.h"
#include "EnlargeDlg.h"
#include "WMap2DDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Thread function declaration
static UINT Create3DDialogThreadFunction(LPVOID pParam);

/////////////////////////////////////////////////////////////////////////////
// CViewContentDlg413 dialog

CViewContentDlg413::CViewContentDlg413(CWnd* pParent /*=NULL*/)
	: CViewContentDlgBase(CViewContentDlg413::IDD, pParent) {
	//{{AFX_DATA_INIT(CViewContentDlg413)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_cWaferMap.dmMode = CWaferMap::dmUSER;
	precision = 2;
	Show = 1;
	nCjName = 0;
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		dwCjName[i] = "";
	}

	// Initialize state variables
	m_currentViewState = VS_VALUE;
	m_bLastWas2DBefore3D = FALSE;
	is2Dclicked = false;
	lastSelectedCol = 0;
	isLTV = FALSE;
	isSTIR = FALSE;
	WaferSize = 0.00;
	pDlg = nullptr;
	pDlg2 = nullptr;
	ltvIdx = -1;
	stirIdx = -1;
	isRangeMeasurement = true;
	m_pGl2DView = nullptr;
}

CViewContentDlg413::~CViewContentDlg413() {
	if (pDlg) {
		delete pDlg;
		pDlg = nullptr;
	}
	if (m_pGl2DView) {
		m_pGl2DView = nullptr;
	}
}

void CViewContentDlg413::DoDataExchange(CDataExchange* pDX) {
	CViewContentDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewContentDlg413)
	DDX_Control(pDX, IDC_STATS, m_cStats);
	DDX_Control(pDX, IDC_REPORT, m_cReport);
	DDX_Control(pDX, IDC_VIEW, m_cWaferMap);
	DDX_Control(pDX, IDC_VIEW2, m_cDiceMap);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ROUGHNESS_UNIT_UM, cRoughnessUnit);
}

BEGIN_MESSAGE_MAP(CViewContentDlg413, CViewContentDlgBase)
	//{{AFX_MSG_MAP(CViewContentDlg413)
	ON_BN_CLICKED(IDC_CLOSETAB, OnClosetab)
	ON_BN_CLICKED(IDC_ENLARGE, OnEnlarge)
	ON_BN_CLICKED(IDC_WAFERVIEW, OnWaferview)
	ON_BN_CLICKED(IDC_2DVIEW, On2dview)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_SAVEBMP, OnSavebmp)
	ON_BN_CLICKED(IDC_PROFILEVIEW, OnProfileview)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_REPORT, OnColumnclickReport)
	ON_BN_CLICKED(IDC_BOW, OnBow)
	ON_BN_CLICKED(IDC_GENSECSREPORT, OnGensecsreport)
	ON_BN_CLICKED(IDC_GENSECSREPORT2, OnGensecsreport2)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_STATS, OnColumnclickStats)
	ON_BN_CLICKED(IDC_RELOAD, OnReload)
	ON_BN_CLICKED(IDC_BOW2, OnBow2)
	ON_BN_CLICKED(IDC_SWITCHVIEW, OnSwitchview)
	ON_BN_CLICKED(IDC_COORDINATEVIEW, OnCoordinateview)
	ON_BN_CLICKED(IDC_VIEWBASELINE, OnViewbaseline)
	ON_BN_CLICKED(IDC_BASELINEVIEW, OnBaselineview)
	ON_BN_CLICKED(IDC_EXPORT2, OnExport2)
	ON_BN_CLICKED(IDC_MAKEBLINE, OnMakebline)
	ON_BN_CLICKED(IDC_BLCORRECT, OnBlcorrect)
	ON_BN_CLICKED(IDC_BLRECOVER, OnBlrecover)
	ON_BN_CLICKED(IDC_XXX, OnXxx)
	ON_BN_CLICKED(IDC_XXX2, OnXxx2)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_3DVIEW, &CViewContentDlg413::OnBnClicked3dview)
	ON_BN_CLICKED(IDC_ROUGHNESS_UNIT_UM, &CViewContentDlg413::OnBnClickedRoughnessUnitUm)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CREATE_3DDIALOG_MSG, &CViewContentDlg413::OnCreate3DDialogMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewContentDlg413 message handlers

long CViewContentDlg413::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("View\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	p413App->Global.isCallFromViewContentDlg413 = true;

	is2Dclicked = false;
	return 0;
}

void CViewContentDlg413::PostNcDestroy() {
	CViewContentDlgBase::PostNcDestroy();
	delete this;
}

BOOL CViewContentDlg413::OnInitDialog() {
	CViewContentDlgBase::OnInitDialog();

	// In OnInitDialog()
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_2DGLUT);
	pStatic->ModifyStyle(0, SS_NOTIFY); // Add SS_NOTIFY if not set in resource

	// Initially hide all view controls
	GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);

	// Set up anchors
	AddAnchor(IDC_RELOAD, TOP_RIGHT);
	AddAnchor(IDC_SAVE, TOP_RIGHT);
	AddAnchor(IDC_BOW, TOP_RIGHT);
	AddAnchor(IDC_BOW2, TOP_RIGHT);
	AddAnchor(IDC_BFPLANE, TOP_RIGHT);
	AddAnchor(IDC_GENSECSREPORT, TOP_RIGHT);
	AddAnchor(IDC_GENSECSREPORT2, TOP_RIGHT);
	AddAnchor(IDC_EXPORT, TOP_RIGHT);
	AddAnchor(IDC_EXPORT2, TOP_RIGHT);
	AddAnchor(IDC_CLOSETAB, BOTTOM_RIGHT);
	AddAnchor(IDC_STATS, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_REPORT, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_VIEWBASELINE, TOP_RIGHT);
	AddAnchor(IDC_MAKEBLINE, TOP_RIGHT);
	AddAnchor(IDC_BLCORRECT, TOP_RIGHT);
	AddAnchor(IDC_BLRECOVER, TOP_RIGHT);
	AddAnchor(IDC_ROUGHNESS_UNIT_UM, TOP_RIGHT);

	CString carrierID, recipe, filename;
	filename = ViewDir + ViewName;
	if (CasMgr.OpenCassette(filename)) {
		SetupReportHeader();
		short start = 0;
		m_cWaferMap.MapCol = 0;
		m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[0].GetDataCol(m_cWaferMap.MapCol, start);
		if (m_cWaferMap.DataCol == -1) {
			m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[1].GetDataCol(m_cWaferMap.MapCol, start) + MaXPA;
			if (m_cWaferMap.DataCol == -1) {
				m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[2].GetDataCol(m_cWaferMap.MapCol, start) + 2 * MaXPA;
				if (m_cWaferMap.DataCol == -1) {
					m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[3].GetDataCol(m_cWaferMap.MapCol, start) + 3 * MaXPA;
					if (m_cWaferMap.DataCol == -1) {
						m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[4].GetDataCol(m_cWaferMap.MapCol, start) + 4 * MaXPA;
						if (m_cWaferMap.DataCol == -1) {
							m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[5].GetDataCol(m_cWaferMap.MapCol, start) + 5 * MaXPA;
							if (m_cWaferMap.DataCol == -1) {
								m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[6].GetDataCol(m_cWaferMap.MapCol, start) + 6 * MaXPA;
								if (m_cWaferMap.DataCol == -1) {
									m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[7].GetDataCol(m_cWaferMap.MapCol, start) + 7 * MaXPA;
									if (m_cWaferMap.DataCol == -1) {
										m_cWaferMap.DataCol = GetCurrentRecipe()->MeParam[8].GetDataCol(m_cWaferMap.MapCol, start) + 8 * MaXPA;
									}
								}
							}
						}
					}
				}
			}
		}
		if (m_cWaferMap.DataCol != -1) {
			m_cWaferMap.Title = dwCjName[m_cWaferMap.MapCol + 3];
		}
		else {
			m_cWaferMap.Title = "";
		}

		WaferSize = 0.00;
		isLTV = FALSE;
		isSTIR = FALSE;
		isRangeMeasurement = true;
		IsDiceTypeData();
		RefreshDialog();
	}

	// Initialize the tooltip
	/*m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_ToolTip.AddTool(this, _T(""));
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 0);
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 2000);
	m_ToolTip.SetDelayTime(TTDT_RESHOW, 0);
	m_ToolTip.Activate(TRUE);*/

	return TRUE;
}

void CViewContentDlg413::RefreshDialog() {
	CNCas* pCas = CasMgr.GetCurCas();
	m_cWaferMap.pRcp = pCas->GetRecipe413();
	m_cWaferMap.bSiteView = FALSE;

	if (m_cWaferMap.pRcp->IsRoughnessMeasurement()) {
		cRoughnessUnit.ShowWindow(TRUE);
		cRoughnessUnit.SetCheck(p413App->Global.umUnitInRoughness);
	}

	if (m_cWaferMap.pRcp->IsConfocalThicknessMeasurement() || m_cWaferMap.pRcp->IsEchoThicknessMeasurement()) {
		isRangeMeasurement = false;
	}

	if (isLTV || isSTIR) {
		StoreData();
		p413App->Global.setDiceDisplay = true;
		GenerateMap();
		GetDlgItem(IDC_VIEW2)->ShowWindow(SW_SHOW);
		//[13012025
		// Initialize the tooltip
		m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
		m_ToolTip.AddTool(this, _T("")); // Associate tooltip with the dialog
		m_ToolTip.SetDelayTime(TTDT_INITIAL, 0);    // Show immediately
		m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 2000); // Display for 2 seconds
		m_ToolTip.SetDelayTime(TTDT_RESHOW, 0);     // No delay between reshow
		m_ToolTip.Activate(TRUE);
		//]
	}

	UpdateViewVisibility();
	UpdateReport(m_cWaferMap);
}

void CViewContentDlg413::UpdateViewVisibility() {
	// Hide all views first
	GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);

	// Show appropriate view based on state and data type
	switch (m_currentViewState) {
	case VS_VALUE:
		if (isLTV || isSTIR) {
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_SHOW);
			if (m_cDiceMap.GetSafeHwnd()) {
				GenerateMap();
			}
		}
		else {
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_SHOW);
		}
		break;

	case VS_2D:
		/*GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_SHOW);
		break;*/

		if (isLTV || isSTIR) {
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_SHOW);
			if (m_cDiceMap.GetSafeHwnd()) {
				GenerateMap();
			}
		}
		else {
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_SHOW);
		}
		break;

	case VS_3D:
		if (isLTV || isSTIR) {
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_SHOW);
			if (m_cDiceMap.GetSafeHwnd()) {
				GenerateMap();
			}
		}
		else {
			GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_2DGLUT)->ShowWindow(SW_SHOW);
			Show3DMapView();
		}
		break;

	case VS_PROFILE:
	case VS_BASELINE:
		GetDlgItem(IDC_VIEW)->ShowWindow(SW_SHOW);
		break;
	}
}

//void CViewContentDlg413::ShowValueView() {
//	m_currentViewState = VS_VALUE;
//	m_bLastWas2DBefore3D = FALSE;
//	is2Dclicked = false;
//	UpdateViewVisibility();
//}

void CViewContentDlg413::ShowValueView() {
	m_currentViewState = VS_VALUE;
	m_bLastWas2DBefore3D = FALSE;
	is2Dclicked = false;

	// CRITICAL FIX: Explicitly hide the OpenGL window
	if (m_pGl2DView && ::IsWindow(m_pGl2DView->GetSafeHwnd())) {
		m_pGl2DView->ShowWindow(SW_HIDE);

		// Also try invalidating and updating
		m_pGl2DView->Invalidate();
		m_pGl2DView->UpdateWindow();
	}

	// Hide the placeholder too
	CWnd* p2DGLUT = GetDlgItem(IDC_2DGLUT);
	if (p2DGLUT && ::IsWindow(p2DGLUT->GetSafeHwnd())) {
		p2DGLUT->ShowWindow(SW_HIDE);
	}

	UpdateViewVisibility();
}

void CViewContentDlg413::OnClosetab() {
	if (Parent) {
		ShowWindow(SW_HIDE);
		Parent->PostMessage(UM_CLOSETAB, 0, (LPARAM)this);
	}
}

void CViewContentDlg413::SetupReportHeader() {
	int i, j;
	int nSize[MAXMEPASET * MaXPA];
	int n = 0;
	dwCjName[n] = "No";    nSize[n++] = 42;
	dwCjName[n] = "X mm";  nSize[n++] = 56;
	dwCjName[n] = "Y mm";  nSize[n++] = 56;
	nCjName = 3;

	CRecipe413* pRcp = GetCurrentRecipe();
	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &pRcp->MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV_MIN ||
					Me->MPa[j].Prb == MParam::CONFOCAL_LTV_MAX ||
					Me->MPa[j].Prb == MParam::CONFOCAL_LTV_AVG) {
					continue;
				}
				dwCjName[n] = Me->MPa[j].Name;
				nSize[n++] = 56;
				nCjName++;
			}
		}
	}

	m_cReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_cStats.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	nListColumn.fmt = LVCFMT_LEFT;
	nListColumn.iSubItem = 0;
	nListColumn.cx = 68;
	nListColumn.pszText = "Name";
	m_cStats.InsertColumn(0, &nListColumn);

	for (i = 0; i < n; i++) {
		nListColumn.cx = nSize[i];
		nListColumn.pszText = dwCjName[i].GetBuffer(0);
		m_cReport.InsertColumn(i, &nListColumn);
		if (i > 2) {
			m_cStats.InsertColumn(i - 2, &nListColumn);
		}
	}
}

void CViewContentDlg413::UpdateReport(CWaferMap& m_cWaferMap) {
	m_cReport.DeleteAllItems();
	CRecipe413* pRcp = GetCurrentRecipe();

	CPtrList* pMPList = GetMPList();
	if (pMPList) {
		POSITION pos = pMPList->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
			if (p) {
				UpdateReportPoint(pRcp, p);
			}
		}

		m_cReport.EnsureVisible(m_cReport.GetItemCount(), FALSE);
	}

	m_cStats.DeleteAllItems();
	CPattern* pPatt = &pRcp->Patt;
	CString Name[7] = { "Max", "Min", "Ave", "StDev", "TTV", "", "" };
	if (p413App->Global.bEnableBow) { Name[5] = "Bow"; }
	if (p413App->Global.bEnableWarp) { Name[6] = "Warp"; }
	if (isRangeMeasurement == true) { Name[4] = "Range"; }

	if (pPatt) {
		m_cWaferMap.strList.RemoveAll();
		for (int k = 0; k < 7; k++) {
			int cnt = 1;
			int idx = m_cStats.InsertItem(k, Name[k]);
			for (int j = 0; j < MAXMEPASET; j++) {
				short jj = j * MaXPA;
				CMeParam* pMeP = &pRcp->MeParam[j];
				for (int i = 0; i < MaXPA; i++) {
					if (pMeP->MPa[i].Prb > 0) {
						CString str, str2;
						CString dec1;
						float val = 0.0f;
						switch (k) {
						case 0:
							val = pPatt->MP.Stats[jj + i].m_fMax;
							if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
								val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
							}
							dec1.Format("%%.%df", pMeP->MPa[i].D);
							str.Format(dec1, val);
							if (m_cWaferMap.DataCol == jj + i) {
								str2.Format("Max=%s", str);
								m_cWaferMap.strList.AddTail(str2);
							}
							break;
						case 1:
							val = pPatt->MP.Stats[jj + i].m_fMin;
							if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
								val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
							}
							dec1.Format("%%.%df", pMeP->MPa[i].D);
							str.Format(dec1, val);
							if (m_cWaferMap.DataCol == jj + i) {
								str2.Format("Min=%s", str);
								m_cWaferMap.strList.AddTail(str2);
							}
							break;
						case 2:
							val = pPatt->MP.Stats[jj + i].m_fAverage;
							if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
								val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
							}
							dec1.Format("%%.%df", pMeP->MPa[i].D);
							str.Format(dec1, val);
							if (m_cWaferMap.DataCol == jj + i) {
								str2.Format("Ave=%s", str);
								m_cWaferMap.strList.AddTail(str2);
							}
							break;
						case 3:
							val = pPatt->MP.Stats[jj + i].m_fSTD;
							if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
								val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
							}
							dec1.Format("%%.%df", pMeP->MPa[i].D);
							str.Format(dec1, val);
							if (m_cWaferMap.DataCol == jj + i) {
								str2.Format("StDev=%s", str);
								m_cWaferMap.strList.AddTail(str2);
							}
							break;
						case 4:
							if (pMeP->MPa[i].Fr == 1) {
								str = "";
								if (pMeP->MPa[i].IsTTV()) {
									val = pPatt->MP.Stats[jj + i].m_fTTV;
									if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
										val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
									}
									dec1.Format("%%.%df", pMeP->MPa[i].D);
									str.Format(dec1, val);
								}
							}
							else {
								val = pPatt->MP.Stats[jj + i].m_fTTV;
								if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
									val /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
								}
								dec1.Format("%%.%df", pMeP->MPa[i].D);
								str.Format(dec1, val);
							}
							if (m_cWaferMap.DataCol == jj + i) {
								str2.Format("TTV=%s", str);
								m_cWaferMap.strList.AddTail(str2);
							}
							break;
						case 5:
						case 6:
							switch (pMeP->MPa[i].Prb) {
							case MParam::BOWWARP1:
							case MParam::BOWWARP2:
							case MParam::CONFOCAL_WARP:
								switch (k) {
								case 5:
									val = pPatt->MP.Stats[jj + i].m_fBow;
									dec1.Format("%%.%df", pMeP->MPa[i].D);
									str.Format(dec1, val);
									if (m_cWaferMap.DataCol == jj + i) {
										str2.Format("Bow=%s", str);
										m_cWaferMap.strList.AddTail(str2);
									}
									break;
								case 6:
									val = pPatt->MP.Stats[jj + i].m_fWarp;
									dec1.Format("%%.%df", pMeP->MPa[i].D);
									str.Format(dec1, val);
									if (m_cWaferMap.DataCol == jj + i) {
										str2.Format("Warp=%s", str);
										m_cWaferMap.strList.AddTail(str2);
									}
									break;
								}
								break;
							case MParam::CONFOCAL_DUALWARP:
								switch (k) {
								case 5:
									val = pPatt->MP.Stats[jj + i].m_fBow;
									dec1.Format("%%.%df", pMeP->MPa[i].D);
									str.Format(dec1, val);
									if (m_cWaferMap.DataCol == jj + i) {
										str2.Format("Bow=%s", str);
										m_cWaferMap.strList.AddTail(str2);
									}
									break;
								case 6:
									val = pPatt->MP.Stats[jj + i].m_fWarp;
									dec1.Format("%%.%df", pMeP->MPa[i].D);
									str.Format(dec1, val);
									if (m_cWaferMap.DataCol == jj + i) {
										str2.Format("Warp=%s", str);
										m_cWaferMap.strList.AddTail(str2);
									}
									break;
								}
								break;
							default:
								str = "";
								break;
							}
							break;
						}
						m_cStats.SetItemText(idx, cnt++, str);
					}
				}
			}
		}
	}
	m_cWaferMap.Redraw();
}

void CViewContentDlg413::UpdateReportPoint(CRecipe413* pRcp, CMPoint* p) {
	if (!pRcp->IsValidMeSet(p)) {
		if (p->magic > 9) {
			return;
		}
	}

	if (p->magic == 9) {
		p->MeSet = 1;
	}
	int cnt = m_cReport.GetItemCount();
	CString str;
	str.Format("%d", p->Co.n);
	int idx = m_cReport.InsertItem(cnt, str);
	m_cReport.SetItemText(idx, 1, p->GetXCoorStr());
	m_cReport.SetItemText(idx, 2, p->GetYCoorStr());
	CData* d = p->GetData(0, FALSE);
	if (!d) {
		return;
	}
	short MeSetX = MaXPA * (p->MeSet - 1);
	CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
	for (int j = 0; j < MaXPA; j++) {
		if (Me->MPa[j].Prb > 0) {
			for (int i = 3; i < nCjName; i++) {
				if (Me->MPa[j].Name.GetLength() > 0) {
					if (dwCjName[i] == Me->MPa[j].Name) {
						float fdata = d->Get(MeSetX + j);
						if (Me->MPa[j].Prb == MParam::ROUGHPRB) {
							fdata /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
						}
						if (fdata != BADDATANEG) {
							CString dec1;
							dec1.Format("%%.%df", Me->MPa[j].D);
							precision = Me->MPa[j].D;
							str.Format(dec1, fdata);
						}
						else {
							str = "N/A";
						}
						m_cReport.SetItemText(idx, i, str);
						break;
					}
				}
			}
		}
	}
}

CRecipe413* CViewContentDlg413::GetCurrentRecipe() {
	CNCas* pCas = CasMgr.GetCurCas();
	return pCas->GetRecipe413();
}

void CViewContentDlg413::OnEnlarge() {
	p413App->Enlarge(GetCurrentRecipe(), &m_cWaferMap, &m_cDiceMap);
}

void CViewContentDlg413::OnOptions() {
	CWaferViewOptions dlg;

	dlg.pWaferMap = &m_cWaferMap;
	dlg.DoModal();
}

void CViewContentDlg413::OnExport() {
	CFile fp;
	CString filename;
	filename = ViewDir + ViewName;
	int bExt = filename.Find("\\C2C");
	CasMgr.SaveAsTextFile(filename, (bExt == -1), FALSE);
}

CString CViewContentDlg413::GetReportPointStr(int cnt, CRecipe413* pRcp, CMPoint* p) {
	CString str;
	CString total;
	str.Format("%d,", cnt + 1);
	total += str;
	total += p->GetXCoorStr() + ",";
	total += p->GetYCoorStr();
	CData* d = p->GetData(0, FALSE);

	short MeSetX = MaXPA * (p->MeSet - 1);
	CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
	for (int j = 0; j < MaXPA; j++) {
		if (Me->MPa[j].Prb > 0) {
			float fdata = d->Get(MeSetX + j);
			if (Me->MPa[j].Prb == MParam::ROUGHPRB) {
				fdata /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
			}
			if (fdata != BADDATANEG) {
				CString dec1;
				dec1.Format(",%%.%df", Me->MPa[j].D);
				str.Format(dec1, fdata);
			}
			else {
				str = "N/A";
			}
			total += str;
		}
	}
	total += "\n";

	return total;
}

void CViewContentDlg413::OnSavebmp() {
	CFile fp;
	CFileDialog fd(FALSE, "BMP", NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		"Bitmap file|*.BMP|");
	if (fd.DoModal() == IDOK) {
		CString path = fd.GetPathName();

		if (isLTV || isSTIR) {
			m_cDiceMap.SaveBitmap(path);
		}
		else {
			if (fp.Open(path, CFile::modeCreate | CFile::modeWrite)) {
				CRect rc;
				rc.SetRect(0, 0, 1280, 960);
				m_cWaferMap.SaveBitmap(fp, rc);
			}
			fp.Close();
		}
	}
}

void CViewContentDlg413::OnProfileview() {
	m_currentViewState = VS_PROFILE;
	m_bLastWas2DBefore3D = FALSE;
	UpdateViewVisibility();

	m_cWaferMap.DataCol = 0;
	m_cWaferMap.m_nImageType = 2;
	m_cWaferMap.Redraw();
}

void CViewContentDlg413::OnWaferview() {
	ShowValueView();

	if (isLTV || isSTIR) {
		GenerateMap();
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
	}
	else {
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
		m_cWaferMap.Redraw();
	}
}

void CViewContentDlg413::On2dview() {
	if (!p413App->Global.is3DClosed) {
		if (pDlg) {
			delete pDlg;
			pDlg = nullptr;
		}
	}

	Show2DMapView();

	/*m_currentViewState = VS_2D;
	is2Dclicked = true;
	p413App->Global.isCallFromViewContentDlg413 = true;
	p413App->Global.callFromBtn = true;

	GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);

	CWnd* pPlaceholder = GetDlgItem(IDC_2DGLUT);
	if (!pPlaceholder) {
		AfxMessageBox(_T("IDC_2DGLUT not found!"));
		return;
	}

	CRect rc;
	pPlaceholder->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_pGl2DView = static_cast<CGlut2DMapView*>(RUNTIME_CLASS(CGlut2DMapView)->CreateObject());
	if (!m_pGl2DView) {
		AfxMessageBox(_T("Failed to create CWMapView!"));
		return;
	}

	if (!m_pGl2DView->Create(
		NULL,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		rc,
		this,
		IDC_2DGLUT
	)) {
		AfxMessageBox(_T("Failed to create OpenGL view window!"));
		delete m_pGl2DView;
		m_pGl2DView = nullptr;
		return;
	}

	CRecipe413* pRcp = GetCurrentRecipe();
	m_pGl2DView->pRcp = pRcp;
	m_pGl2DView->nDataIdx = lastSelectedCol;
	m_pGl2DView->OnInitialUpdate();*/
	CRecipe413* pRcp = GetCurrentRecipe();
	p413App->Global.isCallFromViewContentDlg413 = true;
	p413App->Global.callFromBtn = true;

	// MODIFIED: Use thread for 3D view creation
	if (!p413App->Global.is3DClosed) {
		// Create thread data
		Thread3DData* pData = new Thread3DData();
		pData->pRecipe = pRcp;
		pData->dataColumn = lastSelectedCol;
		pData->pDialog = this;
		pData->bIsLTV = isLTV;
		pData->bIsSTIR = isSTIR;

		// Start the thread
		AfxBeginThread(Create3DDialogThreadFunction, pData, THREAD_PRIORITY_NORMAL);
	}

	GetDlgItem(IDC_VIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);

	//m_pGl2DView->Invalidate();
	m_currentViewState = VS_2D;
	UpdateViewVisibility();
	//Invalidate();
}

void CViewContentDlg413::OnBnClicked3dview() {
	p413App->Global.is3DClosed = false;
	m_bLastWas2DBefore3D = (m_currentViewState == VS_2D);
	m_currentViewState = VS_3D;

	p413App->Global.isCallFromViewContentDlg413 = true;
	p413App->Global.callFromBtn = true;

	if (isLTV || isSTIR) {
		GenerateMap();
		Show3DMapView();
		UpdateViewVisibility();
	}
	else {
		CRecipe413* pRcp = GetCurrentRecipe();
		pRcp->Desc.MapCol = 0;
		m_cWaferMap.m_nImageType = 3;
		p413App->setSelectedTitle(&m_cWaferMap);
		Show3DMapView();
	}
}

void CViewContentDlg413::OnColumnclickReport(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	short start = 0;
	CRecipe413* pRcp = GetCurrentRecipe();
	m_cWaferMap.MapCol = pNMListView->iSubItem - 3;
	m_cWaferMap.DataCol = pRcp->MeParam[0].GetDataCol(m_cWaferMap.MapCol, start);
	if (m_cWaferMap.DataCol == -1) {
		m_cWaferMap.DataCol = pRcp->MeParam[1].GetDataCol(m_cWaferMap.MapCol, start) + MaXPA;
		if (m_cWaferMap.DataCol == -1) {
			m_cWaferMap.DataCol = pRcp->MeParam[2].GetDataCol(m_cWaferMap.MapCol, start) + 2 * MaXPA;
			if (m_cWaferMap.DataCol == -1) {
				m_cWaferMap.DataCol = pRcp->MeParam[3].GetDataCol(m_cWaferMap.MapCol, start) + 3 * MaXPA;
				if (m_cWaferMap.DataCol == -1) {
					m_cWaferMap.DataCol = pRcp->MeParam[4].GetDataCol(m_cWaferMap.MapCol, start) + 4 * MaXPA;
					if (m_cWaferMap.DataCol == -1) {
						m_cWaferMap.DataCol = pRcp->MeParam[5].GetDataCol(m_cWaferMap.MapCol, start) + 5 * MaXPA;
						if (m_cWaferMap.DataCol == -1) {
							m_cWaferMap.DataCol = pRcp->MeParam[6].GetDataCol(m_cWaferMap.MapCol, start) + 6 * MaXPA;
							if (m_cWaferMap.DataCol == -1) {
								m_cWaferMap.DataCol = pRcp->MeParam[7].GetDataCol(m_cWaferMap.MapCol, start) + 7 * MaXPA;
								if (m_cWaferMap.DataCol == -1) {
									m_cWaferMap.DataCol = pRcp->MeParam[8].GetDataCol(m_cWaferMap.MapCol, start) + 8 * MaXPA;
								}
							}
						}
					}
				}
			}
		}
	}
	lastSelectedCol = m_cWaferMap.DataCol;
	if (m_cWaferMap.DataCol != -1) {
		m_cWaferMap.Title = dwCjName[m_cWaferMap.MapCol + 3];
	}
	else {
		m_cWaferMap.Title = "";
	}

	if (isLTV || isSTIR) {
		if (is2Dclicked) {
			m_currentViewState = VS_2D;
		}
		else {
			m_currentViewState = VS_VALUE;
		}
		UpdateViewVisibility();
	}

	UpdateReport(m_cWaferMap);
	if (is2Dclicked) {
		m_cWaferMap.PreComputeInterpolation();
		On2dview();
	}

	*pResult = 0;
	p413App->Global.is3Ddone = false;
}

void CViewContentDlg413::OnColumnclickStats(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CRecipe413* pRcp = GetCurrentRecipe();

	short start = 0;
	m_cWaferMap.MapCol = pNMListView->iSubItem - 1;
	m_cWaferMap.DataCol = pRcp->MeParam[0].GetDataCol(m_cWaferMap.MapCol, start);
	if (m_cWaferMap.DataCol == -1) {
		m_cWaferMap.DataCol = pRcp->MeParam[1].GetDataCol(m_cWaferMap.MapCol, start) + MaXPA;
		if (m_cWaferMap.DataCol == -1) {
			m_cWaferMap.DataCol = pRcp->MeParam[2].GetDataCol(m_cWaferMap.MapCol, start) + 2 * MaXPA;
			if (m_cWaferMap.DataCol == -1) {
				m_cWaferMap.DataCol = pRcp->MeParam[3].GetDataCol(m_cWaferMap.MapCol, start) + 3 * MaXPA;
				if (m_cWaferMap.DataCol == -1) {
					m_cWaferMap.DataCol = pRcp->MeParam[4].GetDataCol(m_cWaferMap.MapCol, start) + 4 * MaXPA;
					if (m_cWaferMap.DataCol == -1) {
						m_cWaferMap.DataCol = pRcp->MeParam[5].GetDataCol(m_cWaferMap.MapCol, start) + 5 * MaXPA;
						if (m_cWaferMap.DataCol == -1) {
							m_cWaferMap.DataCol = pRcp->MeParam[6].GetDataCol(m_cWaferMap.MapCol, start) + 6 * MaXPA;
							if (m_cWaferMap.DataCol == -1) {
								m_cWaferMap.DataCol = pRcp->MeParam[7].GetDataCol(m_cWaferMap.MapCol, start) + 7 * MaXPA;
								if (m_cWaferMap.DataCol == -1) {
									m_cWaferMap.DataCol = pRcp->MeParam[8].GetDataCol(m_cWaferMap.MapCol, start) + 8 * MaXPA;
								}
							}
						}
					}
				}
			}
		}
	}
	if (m_cWaferMap.DataCol != -1) {
		m_cWaferMap.Title = dwCjName[m_cWaferMap.MapCol + 3];
	}
	else {
		m_cWaferMap.Title = "";
	}
	UpdateReport(m_cWaferMap);

	*pResult = 0;
}

CPtrList* CViewContentDlg413::GetMPList() {
	CRecipe413* pRcp = GetCurrentRecipe();
	if (pRcp) {
		CPattern* pPatt = &pRcp->Patt;
		if (pPatt) {
			return &pPatt->MP.MPLst;
		}
	}
	return NULL;
}

void CViewContentDlg413::OnBow() {
	GenerateBowA();
	UpdateReport(m_cWaferMap);
}

void CViewContentDlg413::GenerateBowA() {
	CRecipe413* pRcp = GetCurrentRecipe();
	CPattern* pPatt = &pRcp->Patt;
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short idx = j * MaXPA + i;
				if (pMeP->MPa[i].IsEchoBowWarpMeasurement() || pMeP->MPa[i].IsConfocalWarpMeasurement() || pMeP->MPa[i].IsConfocalDualWarpMeasurement()) {
					short n = 0;
					do {
						pPatt->MP.NormalizeWaferHeight(idx);
						pPatt->MP.CalcStat(j, i, pMeP->MPa[i], TRUE);
						n++;
						if (n > 16) {
							break;
						}
					} while (pPatt->MP.Stats[idx].m_fMin != 0);
				}
				else {
					pPatt->MP.CalcStat(j, i, pMeP->MPa[i], TRUE);
				}
			}
		}
	}
}

void CViewContentDlg413::GenerateBowB() {
	CPtrList* pMPList = GetMPList();
	if (!pMPList) {
		return;
	}

	int i, j;
	CMaxMinPa MM;
	CRecipe413* pRcp = GetCurrentRecipe();

	POSITION opos;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		opos = pos;
		BOOL bDel = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			CData* d = p->GetData(0, FALSE);
			for (i = 0; i < MAXMEPASET; i++) {
				int ii = i * MaXPA;
				CMeParam* Me = &pRcp->MeParam[i];
				for (j = 0; j < MaXPA; j++) {
					if (Me->MPa[j].IsEchoBowWarpMeasurement() || Me->MPa[j].IsConfocalWarpMeasurement() || Me->MPa[j].IsConfocalDualWarpMeasurement()) {
						if (d->Get(i * MaXPA + j) < 0) {
							bDel = TRUE;
						}
						else {
							MM.MM[ii + j].SetMM(d->Get(i * MaXPA + j));
						}
					}
				}
			}
			if (bDel) {
				pMPList->RemoveAt(opos);
				delete p;
			}
		}
	}

	pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			CData* d = p->GetData(0, FALSE);
			for (i = 0; i < MAXMEPASET; i++) {
				int ii = i * MaXPA;
				CMeParam* Me = &pRcp->MeParam[i];
				for (j = 0; j < MaXPA; j++) {
					if (Me->MPa[j].IsEchoBowWarpMeasurement() || Me->MPa[j].IsConfocalWarpMeasurement() || Me->MPa[j].IsConfocalDualWarpMeasurement()) {
						float fData = d->Get(i * MaXPA + j);
						if (fData != BADDATANEG) {
							d->Set(i * MaXPA + j, fData - MM.MM[ii + j].fMin);
						}
						else {
							d->Set(i * MaXPA + j, BADDATANEG);
						}
					}
				}
			}
		}
	}
	CPattern* pPatt = &pRcp->Patt;
	for (j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], TRUE);
			}
		}
	}
}

void CViewContentDlg413::OnGensecsreport() {
	GenSECSReport(1);
}

void CViewContentDlg413::OnGensecsreport2() {
	GenSECSReport(1);
}

void CViewContentDlg413::GenSECSReport(short which) {
	int cnt = 0;
	CPtrList* pMPList = GetMPList();
	if (pMPList) {
		CRecipe413* pRcp = GetCurrentRecipe();
		POSITION pos = pMPList->GetHeadPosition();
		while (pos) {
			CMPoint* mp = (CMPoint*)pMPList->GetNext(pos);
			if (mp) {
				CMeParam* Me = &pRcp->MeParam[mp->MeSet - 1];
				pGDev->pOLnClt->GEMEventData(which, mp, Me);
				cnt++;
			}
			if (cnt > 3) {
				break;
			}
		}
	}
}

void CViewContentDlg413::OnReload() {}

void CViewContentDlg413::OnBow2() {
	GenerateBowB();
	UpdateReport(m_cWaferMap);
}

void CViewContentDlg413::RecalculateStats() {
	GenerateBowA();
}

void CViewContentDlg413::OnSwitchview() {
	switch (Show) {
	case 0:
		Show = 1;
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
		break;
	case 1:
		Show = 2;
		m_cWaferMap.m_nImageType = 1;
		m_cWaferMap.MapCol = 0;
		m_cWaferMap.DataCol = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
		break;
	default:
		Show = 0;
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.DataCol = 0;
		m_cWaferMap.dmMode = CWaferMap::dmENGINNER;
		break;
	}
	m_cWaferMap.Redraw();
}

void CViewContentDlg413::OnCoordinateview() {
	ShowValueView();
	m_cWaferMap.m_nImageType = 0;
	m_cWaferMap.dmMode = CWaferMap::dmENGINNER;
	m_cWaferMap.Redraw();
}

void CViewContentDlg413::OnViewbaseline() {
	ShowValueView();
	CViewBaseline dlg;
	dlg.DoModal();
}

void CViewContentDlg413::OnBaselineview() {
	m_currentViewState = VS_BASELINE;
	m_bLastWas2DBefore3D = FALSE;
	UpdateViewVisibility();

	CRecipe413* pRcp = GetCurrentRecipe();
	pRcp->Desc.MapCol = 0;
	m_cWaferMap.DataCol = 999;
	m_cWaferMap.m_nImageType = 0;
	m_cWaferMap.Redraw();
}

void CViewContentDlg413::OnExport2() {
	CFile fp;
	CString filename;
	filename = ViewDir + ViewName;
	int bExt = filename.Find("\\C2C");
	CasMgr.SaveAsTextFile(filename, (bExt == -1), TRUE);
}

void CViewContentDlg413::OnMakebline() {
	CRecipe413* pRcp = GetCurrentRecipe();
	if (pRcp) {
		pRcp->SaveBaseline();
		m_cWaferMap.Redraw();
		UpdateReport(m_cWaferMap);
	}
}

void CViewContentDlg413::OnBlcorrect() {
	CRecipe413* pRcp = GetCurrentRecipe();
	if (pRcp) {
		pRcp->BaselineCorrectSelf();
		m_cWaferMap.Redraw();
		UpdateReport(m_cWaferMap);
	}
}

void CViewContentDlg413::OnBlrecover() {
	CRecipe413* pRcp = GetCurrentRecipe();
	if (pRcp) {
		pRcp->RemoveBaseLineSelf();
		m_cWaferMap.Redraw();
		UpdateReport(m_cWaferMap);
	}
}

float v = 0;
short off = 0;

void CViewContentDlg413::OnXxx() {
	if (CKeyPad::Edit(v) == TRUE) {
		CHslRgb H;
		for (int i = 0; i < FILL_LEVEL; i++) {
			H.RgB1(i / float(FILL_LEVEL + FILL_LEVEL / 6 + off) + v, 1.0f, 0.5f);
			m_cWaferMap.m_crFill[FILL_LEVEL - i - 1] = RGB(H.R, H.G, H.B);
		}
		m_cWaferMap.Redraw();
	}
}

void CViewContentDlg413::OnXxx2() {
	if (CKeyPad::Edit(off) == TRUE) {
		CHslRgb H;
		for (int i = 0; i < FILL_LEVEL; i++) {
			H.RgB1(i / float(FILL_LEVEL + FILL_LEVEL / 6 + off) + v, 1.0f, 0.5f);
			m_cWaferMap.m_crFill[FILL_LEVEL - i - 1] = RGB(H.R, H.G, H.B);
		}
		m_cWaferMap.Redraw();
	}
}

void CViewContentDlg413::OnBnClickedRoughnessUnitUm() {
	p413App->Global.umUnitInRoughness = !p413App->Global.umUnitInRoughness;
	UpdateReport(m_cWaferMap);
}

void CViewContentDlg413::GenerateMap() {
	CRecipe413* pRcp = GetCurrentRecipe();
	CString rcpName = ViewName;
	m_cDiceMap.ClearCells();
	float m_Radius = WaferSize / 2.00;
	int idx = 1;
	float x, y, bx, by, Ax, Ay;

	Ax = pRcp->AnchorX;
	Ay = pRcp->AnchorY;

	dX.x = pRcp->DieWidth;
	dX.y = 0;
	dY.y = pRcp->DieHeight;
	dY.x = 0;

	if (dX.x <= 2) {
		return;
	}
	if (dY.y <= 2) {
		return;
	}

	short szX = 50;
	short szY = 50;
	for (int j = -szY; j <= szY; j++) {
		for (int i = -szX; i <= szX; i++) {
			x = Ax + i * dX.x + j * dY.x;
			y = Ay + j * dY.y + i * dX.y;
			bx = x; by = y;
			float dist1 = (float)sqrt(x * x + y * y);
			x += dX.x; y += dX.y;
			float dist2 = (float)sqrt(x * x + y * y);
			y += dY.y; x += dY.x;
			float dist3 = (float)sqrt(x * x + y * y);
			x -= dX.x; y -= dX.y;
			float dist4 = (float)sqrt(x * x + y * y);

			short cnt = 0;
			if (dist1 <= m_Radius) {
				cnt++;
			}
			if (dist2 <= m_Radius) {
				cnt++;
			}
			if (dist3 <= m_Radius) {
				cnt++;
			}
			if (dist4 <= m_Radius) {
				cnt++;
			}

			if (cnt > 0) {
				if (!pRcp->m_bIncludeOn && (cnt != 4)) {
					continue;
				}
				float x1 = 00.00, y1 = 0.00;
				float x2 = 0.00, y2 = 0.00;
				x1 = bx;
				y1 = by;
				x2 = bx + dX.x;
				y2 = by + dY.y;

				m_cDiceMap.AddCellWithData(bx, by, dX.x, dY.y, idx, m_Radius, ltvIdx, stirIdx, rcpName, precision);
				idx++;
			}
		}
	}
}

void CViewContentDlg413::IsDiceTypeData() {
	CRecipe413* pRcp = GetCurrentRecipe();
	CPattern* pPatt = &pRcp->Patt;
	for (int j = 0; j < MAXMEPASET; j++) {
		short jj = j * MaXPA;
		CMeParam* pMeP = &pRcp->MeParam[j];
		float wsize = pRcp->Wp.size;
		WaferSize = wsize;
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV) {
					p413App->Global.ltvHighLimit = pMeP->MPa[i].Hi;
					p413App->Global.ltvLowLimit = pMeP->MPa[i].Lo;
					isLTV = TRUE;
					return;
				}

				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_STIR) {
					p413App->Global.stirHighLimit = pMeP->MPa[i].Hi;
					p413App->Global.stirLowLimit = pMeP->MPa[i].Lo;
					isSTIR = TRUE;
					return;
				}
			}
		}
	}
	p413App->Global.setDiceDisplay = false;
}

void CViewContentDlg413::Show3DMapView() {
	if (pDlg) {
		delete pDlg;
		pDlg = nullptr;
	}
	CRecipe413* pRcp = GetCurrentRecipe();
	pDlg = new CWMap3DDlg;
	m_cWaferMap.DataCol = lastSelectedCol;
	pDlg->SetParam(pRcp, m_cWaferMap.DataCol);
	pDlg->Create(CWMap3DDlg::IDD);
	pDlg->ShowWindow(SW_SHOW);
}

void CViewContentDlg413::Show2DMapView() {
	if (pDlg2) {
		delete pDlg2;
		pDlg2 = nullptr;
	}
	CRecipe413* pRcp = GetCurrentRecipe();
	pDlg2 = new CWMap2DDlg;
	m_cWaferMap.DataCol = lastSelectedCol;
	pDlg2->SetParam(pRcp, m_cWaferMap.DataCol);
	pDlg2->Create(CWMap2DDlg::IDD);
	pDlg2->ShowWindow(SW_SHOW);
}

void CViewContentDlg413::StoreData() {
	p413App->Global.stt.clear();

	CRecipe413* pRcp = GetCurrentRecipe();
	CPtrList* pMPList = GetMPList();
	if (!pMPList) {
		return;
	}
	ltvIdx = -1, stirIdx = -1;

	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		float xPos = p->Co.x;
		float yPos = p->Co.y;
		vector<string> temp;
		temp.emplace_back(std::to_string(xPos));
		temp.emplace_back(std::to_string(yPos));

		if (!pRcp->IsValidMeSet(p)) {
			if (p->magic > 9) {
				return;
			}
		}

		if (p->magic == 9) {
			p->MeSet = 1;
		}

		CData* d = p->GetData(0, FALSE);
		if (!d) {
			return;
		}

		short MeSetX = MaXPA * (p->MeSet - 1);
		CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];

		for (int j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				for (int i = 3; i < nCjName; i++) {
					if (Me->MPa[j].Name.GetLength() > 0) {
						if (dwCjName[i] == Me->MPa[j].Name) {
							if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {
								if (ltvIdx == -1) {
									ltvIdx = j;
								}
							}
							if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {
								if (stirIdx == -1) {
									stirIdx = j;
								}
							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_LTV) && (Me->MPa[j + 1].Prb == MParam::CONFOCAL_STIR)) {
								if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {
									string datapart = std::to_string(d->Get(MeSetX + j));
									string datapart2 = std::to_string(d->Get(MeSetX + j + 2));
									string datapart3 = std::to_string(d->Get(MeSetX + j + 3));
									string datapart4 = std::to_string(d->Get(MeSetX + j + 4));
									temp.emplace_back(datapart);
								}
								else {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}

								if (Me->MPa[j + 1].Prb == MParam::CONFOCAL_STIR) {
									string datapart = std::to_string(d->Get(MeSetX + j + 1));
									temp.emplace_back(datapart);
								}
								else {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}
							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_STIR) && (Me->MPa[j + 1].Prb == MParam::CONFOCAL_LTV)) {
								if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {
									string datapart = std::to_string(d->Get(MeSetX + j));
									temp.emplace_back(datapart);
								}
								else {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}

								if (Me->MPa[j + 1].Prb == MParam::CONFOCAL_LTV) {
									string datapart = std::to_string(d->Get(MeSetX + j + 1));
									temp.emplace_back(datapart);
								}
								else {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}
							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_STIR) && (Me->MPa[j + 1].Prb != MParam::CONFOCAL_LTV)) {
								if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {
									string datapart = std::to_string(d->Get(MeSetX + j));
									temp.emplace_back(datapart);
								}

								if (Me->MPa[j + 1].Prb != MParam::CONFOCAL_LTV) {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}
							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_LTV) && (Me->MPa[j + 1].Prb != MParam::CONFOCAL_STIR)) {
								if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {
									string datapart = std::to_string(d->Get(MeSetX + j));
									temp.emplace_back(datapart);
								}

								if (Me->MPa[j + 1].Prb != MParam::CONFOCAL_STIR) {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}
							}

							p413App->Global.stt.emplace_back(temp);
						}
					}
					temp.clear();
				}
			}
		}
		temp.clear();
	}
}

//void CViewContentDlg413::OnLButtonDown(UINT nFlags, CPoint point) {
//	if (isLTV || isSTIR) {
//		float fX, fY;
//		CPoint coord = point;
//		ClientToScreen(&coord);
//		m_cDiceMap.GetCoor2(coord, fX, fY);
//
//		bool diceFound = false;
//
//		for (auto it = p413App->Global.rectangleMap.begin(); it != p413App->Global.rectangleMap.end(); ++it) {
//			int rectId = it->first;
//			std::vector<float>& coords = it->second;
//
//			if (fX >= coords[0] && fX <= coords[2] &&
//				fY >= coords[1] && fY <= coords[3]) {
//
//				CString strDiceID;
//				strDiceID.Format(_T("Dice: (%d)"), (rectId / 2) + 1);
//
//				m_ToolTip.UpdateTipText(strDiceID, this);
//				m_ToolTip.Activate(TRUE);
//				SetTimer(1, 3000, NULL);
//
//				diceFound = true;
//				break;
//			}
//		}
//
//		if (!diceFound) {
//			m_ToolTip.Activate(FALSE);
//		}
//	}
//	else {
//		return;
//	}
//	CDialog::OnLButtonDown(nFlags, point);
//}

void CViewContentDlg413::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (isLTV || isSTIR) {
		float fX, fY;
		CPoint coord = point;
		ClientToScreen(&coord);
		m_cDiceMap.GetCoor2(coord, fX, fY);

		bool diceFound = false;

		for (auto it = p413App->Global.rectangleMap.begin(); it != p413App->Global.rectangleMap.end(); ++it) {
			int rectId = it->first;
			std::vector<float>& coords = it->second;

			if (fX >= coords[0] && fX <= coords[2] &&
				fY >= coords[1] && fY <= coords[3]) {

				CString strDiceID;
				strDiceID.Format(_T("Dice: (%d)"), (rectId /*/ 2*/) /*+ 1*/);

				// Update tooltip text
				m_ToolTip.UpdateTipText(strDiceID, this);

				// Activate tooltip
				m_ToolTip.Activate(TRUE);

				// Set a timer to hide the tooltip after 3 seconds
				SetTimer(1, 3000, NULL); // Timer ID 1, 3000ms = 3 seconds

				diceFound = true;
				break;
			}
		}

		// If no dice found, hide tooltip immediately
		if (!diceFound) {
			m_ToolTip.Activate(FALSE);
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CViewContentDlg413::OnMouseMove(UINT nFlags, CPoint point) {
	CDialog::OnMouseMove(nFlags, point);
}

void CViewContentDlg413::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == 1) {
		m_ToolTip.Activate(FALSE);
		KillTimer(1);
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CViewContentDlg413::PreTranslateMessage(MSG* pMsg) {
	if (m_ToolTip.m_hWnd != NULL)
		m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CViewContentDlg413::SimulateClickOnStatic(UINT nID)
{
	CWnd* pStatic = GetDlgItem(nID);
	if (!pStatic || !::IsWindow(pStatic->m_hWnd))
		return;

	CRect rect;
	pStatic->GetClientRect(&rect);
	CPoint ptCenter(rect.Width() / 2, rect.Height() / 2);
	pStatic->ClientToScreen(&ptCenter);

	::SendMessage(pStatic->m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON,
		MAKELPARAM(ptCenter.x, ptCenter.y));

	::SendMessage(pStatic->m_hWnd, WM_LBUTTONUP, MK_LBUTTON,
		MAKELPARAM(ptCenter.x, ptCenter.y));
}

/////////////////////////////////////////////////////////////////////////////
// Thread-related implementations
/////////////////////////////////////////////////////////////////////////////

// Thread function to execute in background
static UINT Create3DDialogThreadFunction(LPVOID pParam)
{
	CViewContentDlg413::Thread3DData* pData =
		reinterpret_cast<CViewContentDlg413::Thread3DData*>(pParam);

	if (!pData || !pData->pDialog) {
		delete pData;
		return 1;
	}

	// Sleep in background thread (100ms delay)
	Sleep(100);

	// Post message to main UI thread to create the dialog
	::PostMessage(pData->pDialog->GetSafeHwnd(),
		CViewContentDlg413::WM_CREATE_3DDIALOG_MSG,
		reinterpret_cast<WPARAM>(pData),
		0);

	return 0;
}

// Message handler for creating 3D dialog
LRESULT CViewContentDlg413::OnCreate3DDialogMsg(WPARAM wParam, LPARAM lParam)
{
	Thread3DData* pData = reinterpret_cast<Thread3DData*>(wParam);

	if (!pData) {
		return 0;
	}

	// Restore the state captured from the thread
	CRecipe413* pRcp = pData->pRecipe;
	int dataCol = pData->dataColumn;

	// Now safely execute on UI thread
	p413App->Global.is3DClosed = false;
	m_bLastWas2DBefore3D = (m_currentViewState == VS_2D);
	m_currentViewState = VS_3D;
	p413App->Global.isCallFromViewContentDlg413 = true;
	p413App->Global.callFromBtn = true;

	// Check if we need to generate map based on thread-captured state
	if (pData->bIsLTV || pData->bIsSTIR) {
		// Temporarily set the state
		BOOL bOldLTV = isLTV;
		BOOL bOldSTIR = isSTIR;
		isLTV = pData->bIsLTV;
		isSTIR = pData->bIsSTIR;

		GenerateMap();
		Show3DMapView();
		UpdateViewVisibility();

		// Restore original state
		isLTV = bOldLTV;
		isSTIR = bOldSTIR;
	}
	else {
		// Regular 3D view
		if (pDlg) {
			delete pDlg;
			pDlg = nullptr;
		}

		pDlg = new CWMap3DDlg;
		m_cWaferMap.DataCol = dataCol;
		pDlg->SetParam(pRcp, m_cWaferMap.DataCol);
		pDlg->Create(CWMap3DDlg::IDD);
		pDlg->ShowWindow(SW_SHOW);
	}

	// Clean up
	delete pData;

	return 0;
}

// Simple wrapper function (alternative to PostDelayed3DDialogCreation)
void CViewContentDlg413::Create3DViewAsync()
{
	// Create thread data
	Thread3DData* pData = new Thread3DData();
	pData->pRecipe = GetCurrentRecipe();
	pData->dataColumn = lastSelectedCol;
	pData->pDialog = this;
	pData->bIsLTV = isLTV;
	pData->bIsSTIR = isSTIR;

	// Start thread using MFC's thread mechanism
	AfxBeginThread(Create3DDialogThreadFunction, pData, THREAD_PRIORITY_NORMAL);
}