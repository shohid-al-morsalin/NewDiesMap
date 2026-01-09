// QuickRecipeEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "QuickRecipeEditDlg.h"

#include "Recipe413.h"
#include "MPoint.h"
#include "413App.h"
#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickRecipeEditDlg dialog

static CQuickRecipeEditDlg* g_pPropList = NULL;

CQuickRecipeEditDlg::CQuickRecipeEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickRecipeEditDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CQuickRecipeEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pRcp = NULL;

	SelItem = "";
}

void CQuickRecipeEditDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickRecipeEditDlg)
	DDX_Control(pDX, IDC_LIST1, m_propList);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQuickRecipeEditDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickRecipeEditDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDOK3, OnOk3)
	ON_BN_CLICKED(IDC_REPLACEPOINTS, OnReplacepoints)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_BN_CLICKED(IDC_LOADDATAFILE, OnLoaddatafile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickRecipeEditDlg message handlers

void CQuickRecipeEditDlg::OnOK() {
	ParseEntry();
	CDialog::OnOK();
}

BOOL CQuickRecipeEditDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_tree.InsertItem("Echo Measurement");
	m_tree.InsertItem("MeSet(Echo)");
	m_tree.InsertItem("Echo Peak Control");
	m_tree.InsertItem("Thin Film Measurement");
	m_tree.InsertItem("MeSet(Thin Film)");
	m_tree.InsertItem("Pattern Matching");
	m_tree.InsertItem("Trim Width Measurement");
	m_tree.InsertItem("Trim Height Measurement");
	m_tree.InsertItem("Bow Warp Measurement");

	//if you use OnSelChanged, you must provid a PFN_OnSelChanged function point;
	g_pPropList = this;
	m_propList.SetPFNOnSelChanged(pfnOnSelChangedPropertyList);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void pfnOnSelChangedPropertyList(const CPropertyItem* pItem) {
	if (g_pPropList) {
		g_pPropList->SetDlgItemText(IDC_STATIC, pItem->m_propName + " = " + pItem->m_curValue);
	}
}

CString MatchMode[] = { "Position", "Second template","","Alternate template" };
CString FFTFilter[] = { "None", "Hann", "Hamming", "Gaussian", "Exponent", "BartleHann", "Blackman" };

void CQuickRecipeEditDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) {
	if (!pRcp) {
		return;
	}

	CString m, n;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM node = pNMTreeView->itemNew.hItem;
	CString s = m_tree.GetItemText(node);

	if (s.Find("Echo Measurement") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesEhoMeasurement();
	}
	if (s.Find("Echo Peak Control") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesEchoPeakControl();
	}
	else if (s.Find("MeSet(Echo)") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesMeSetEcho();
	}
	else if (s.Find("MeSet(Thin Film)") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesMeSetThinFilm();
	}
	else if (s.Find("Thin Film Measurement") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesThinFilmMeasurement();
	}
	else if (s.Find("Trim Width") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesTrimWidth();
	}
	else if (s.Find("Trim Height") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEnntriesTrimHeight();
	}
	else if (s.Find("Bow") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesBow();
	}
	else if (s.Find("Pattern") != -1) {
		ParseEntry();
		SelItem = s;
		m_propList.DeleteAll();
		AddEntriesPattern();
	}

	*pResult = 0;
}

void CQuickRecipeEditDlg::ParseEntry() {
	if (SelItem.Find("Echo Measurement") != -1) {
		ParseEchoMeasurement();
	}
	else if (SelItem.Find("Thin Film Measurement") != -1) {
		ParseThinFilmMeasurement();
	}
	else if (SelItem.Find("Peak Control") != -1) {
		ParsePeakControl();
	}
	else if (SelItem.Find("MeSet(Echo)") != -1) {
		ParseEchoMeSet();
	}
	else if (SelItem.Find("MeSet(Thin Film)") != -1) {
		ParseThinFilmMeSet();
	}
	else if (SelItem.Find("Pattern Matching") != -1) {
		ParsePatternMatching();
	}
	else if (SelItem.Find("Trim Width") != -1) {
		ParseTrimWidth();
	}
	else if (SelItem.Find("Trim Height") != -1) {
		ParseTrimHeight();
	}
	else if (SelItem.Find("Bow Warp") != -1) {
		ParseBowWarp();
	}
}

void CQuickRecipeEditDlg::ParseEchoMeasurement() {
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		CString m;
		for (int i = 1; i <= MAXMEPASET; i++) {
			m.Format("MeSet%d", i);
			if (pItem->IsCatalog(m)) {
				if (pItem->IsProp("Averaging")) {
					pRcp->MeParam[i - 1].NAve = atoi(pItem->m_curValue);
				}
			}
		}

		if (pItem->IsCatalog("Setting")) {
			if (pItem->IsProp("Auto Focus")) {
				pRcp->EchoFocus = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Echo Camera")) {
				pRcp->EchoNormalExposure = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Z1 Position")) {
				pRcp->fEchoTopZ = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Z1 Offset")) {
				pRcp->TFZOffset = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Baseline file")) {
				pRcp->BaseLineFile = pItem->m_curValue;
			}
		}
		else if (pItem->IsCatalog("Search")) {
			if (pItem->IsProp("Number")) {
				pRcp->nTrialEcho = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Distance")) {
				pRcp->fStepSizeEcho = atof(pItem->m_curValue);
			}
		}
		else if (pItem->IsCatalog("Options")) {
			if (pItem->IsProp("Save")) {
				pRcp->bSaveVideo = (pItem->m_curValue.Find('Y') != -1) ? TRUE : FALSE;
			}
			else if (pItem->IsProp("Show")) {
				pRcp->bEchoPeakMonitor = (pItem->m_curValue.Find('Y') != -1) ? TRUE : FALSE;
			}
			else if (pItem->IsProp("Alignment")) {
				pRcp->bUseWaferAlign = (pItem->m_curValue.Find('Y') != -1) ? TRUE : FALSE;
			}
		}
		else if (pItem->IsCatalog("Timeout")) {
			if (pItem->IsProp("Fail")) {
				pRcp->FailCountTimeOut = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Point")) {
				pRcp->PointTimeOut = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Wafer")) {
				pRcp->WaferTimeOut = atoi(pItem->m_curValue);
			}
		}
	}
}

void CQuickRecipeEditDlg::ParsePeakControl() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		short a = 0;
		for (int i = 1; i <= MAXMEPASET; i++) {
			CMeParam* pM = &pRcp->MeParam[i - 1];
			m.Format("MeSet%d", i);
			if (pItem->IsCatalog(m)) {
				if (pItem->IsCatalog("Top")) {
					a = 0;
				}
				else if (pItem->IsCatalog("Bottom")) {
					a = 1;
				}
				else {
					continue;
				}
				if (pItem->IsProp("Level")) {
					pM->PSG[a].Level = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("MinH%")) {
					pM->PSG[a].Minimum = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Nearest")) {
					pM->PSG[a].Near = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Smooth")) {
					pM->PSG[a].Smooth = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Threshold")) {
					pM->PSG[a].Thres = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Width")) {
					pM->PSG[a].Width = atoi(pItem->m_curValue);
				}
				break;
			}
		}
	}
}

void CQuickRecipeEditDlg::ParseEchoMeSet() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		for (int i = 1; i <= MAXMEPASET; i++) {
			m.Format("MeSet%d", i);
			if (pItem->IsCatalog(m)) {
				for (int j = 1; j <= MaXPA; j++) {
					MParam* pM = &pRcp->MeParam[i - 1].MPa[j - 1];
					n.Format("[%02d]", j);
					if (pItem->IsCatalog(n)) {
						if (pItem->IsProp("Peaks")) {
							pM->NPeak = atoi(pItem->m_curValue);
						}
						else if (pItem->IsProp("Decimal")) {
							pM->D = atoi(pItem->m_curValue);
						}
						else if (pItem->IsProp("Default")) {
							pM->DF = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("Formula")) {
							pM->Formula = pItem->m_curValue;
						}
						else if (pItem->IsProp("Form")) {
							pM->Fr = atoi(pItem->m_curValue);
						}
						else if (pItem->IsProp("High")) {
							pM->Hi = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("Low")) {
							pM->Lo = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("Name")) {
							pM->Name = pItem->m_curValue;
						}
						else if (pItem->IsProp("Value")) {
							pM->Off = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("Probe")) {
							pM->Prb = MParam::PRBNUM(atoi(pItem->m_curValue));
						}
						else if (pItem->IsProp("RI")) {
							pM->RI = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("Host")) {
							pM->bHost = (pItem->m_curValue.Find('Y') != -1) ? TRUE : FALSE;
						}
						else if (pItem->IsProp("StageX")) {
							pM->OffX = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("StageY")) {
							pM->OffY = atof(pItem->m_curValue);
						}
						else if (pItem->IsProp("To")) {
							pM->To = atoi(pItem->m_curValue);
						}
					}
				}
			}
			break;
		}
	}
}

void CQuickRecipeEditDlg::ParseThinFilmMeSet() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		for (int i = 1; i <= MAXMEPASET; i++) {
			CMeParam* pM = &pRcp->MeParam[i - 1];
			m.Format("MeSet%d", i);
			if (pItem->IsCatalog(m)) {
				if (pItem->IsProp("Boxcar")) {
					pM->Ta.Boxcar = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Filters")) {
					pRcp->FFTFilter[i - 1] = GetFilterValue(pItem->m_curValue);
				}
				else if (pItem->IsProp("Integration")) {
					pM->Ta.IntegTime = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Method")) {
					pM->Ta.Method = atoi(pItem->m_curValue);
				}
			}
		}
		if (pItem->IsCatalog("Options")) {
			if (pItem->IsProp("Cycle")) {
				pRcp->nCycle = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Expand")) {
				pRcp->nExpand = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Smooth")) {
				pRcp->NSmooth = atoi(pItem->m_curValue);
			}
		}
	}
}

short CQuickRecipeEditDlg::GetFilterValue(CString name) {
	for (int i = 0; i < 7; i++) {
		if (FFTFilter[i] == name) {
			return i;
		}
	}
	return 0;
}

void CQuickRecipeEditDlg::ParseThinFilmMeasurement() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		if (pItem->IsCatalog("Setting")) {
			if (pItem->IsProp("Z2 Position")) {
				pRcp->fThinFilmZ = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Auto")) {
				pRcp->AutoExposeTF = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("TF Exposure")) {
				pRcp->TFNormalExposure = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Exposure Spec")) {
				pRcp->TFTakeSpecExposure = atoi(pItem->m_curValue);
			}
		}
		for (int i = 1; i <= MAXMEPASET; i++) {
			m.Format("MeSet%d", i);
			if (pItem->IsCatalog(m)) {
				if (pItem->IsProp("GOF")) {
					pRcp->GoFScore[i - 1] = atof(pItem->m_curValue);
				}
				else if (pItem->IsProp("Resolution")) {
					pRcp->MatchRes[i - 1] = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Depth")) {
					pRcp->SearchDept[i - 1] = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Distance")) {
					pRcp->SearchDist[i - 1] = atof(pItem->m_curValue);
				}
				else if (pItem->IsProp("(I)")) {
					pRcp->SubSamI[i - 1] = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("(S)")) {
					pRcp->SubSamS[i - 1] = atoi(pItem->m_curValue);
				}
				else if (pItem->IsProp("Match Mode")) {
					pRcp->MatchAlgorithm[i - 1] = GetMatchMode(pItem->m_curValue);
				}
			}
			CMeParam* pM = &pRcp->MeParam[i - 1];
			for (int j = 1; j <= MAXTHKVLU; j++) {
				m.Format("MeSet%d, Point%d", i, j);
				if (pItem->IsCatalog(m)) {
					if (pItem->IsProp("Min")) {
						pM->Ta.ThkMin[j - 1] = atof(pItem->m_curValue);
					}
					else if (pItem->IsProp("Max")) {
						pM->Ta.ThkMax[j - 1] = atof(pItem->m_curValue);
					}
					else if (pItem->IsProp("RI")) {
						pM->Ta.RI[j - 1] = atof(pItem->m_curValue);
					}
				}
			}
			for (int j = 1; j <= 2; j++) {
				m.Format("PaSet%d P%d", j, i);
				if (pItem->IsCatalog(m)) {
					if (pItem->IsProp("X")) {
						pRcp->POffsetX[i - 1][j - 1] = atof(pItem->m_curValue);
					}
					else if (pItem->IsProp("Y")) {
						pRcp->POffsetY[i - 1][j - 1] = atof(pItem->m_curValue);
					}
				}
			}
		}
	}
}

short CQuickRecipeEditDlg::GetMatchMode(CString str) {
	if (MatchMode[0] == str) {
		return 0;
	}
	else if (MatchMode[1] == str) {
		return 1;
	}
	else if (MatchMode[2] == str) {
		return 2;
	}
	else if (MatchMode[3] == str) {
		return 3;
	}

	return 0;
}

void CQuickRecipeEditDlg::ParsePatternMatching() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		for (int i = 1; i <= MAXMEPASET; i++) {
			m.Format("MeSet%d", i);
			pRcp->PatMatOpt[i - 1] = 0;
			if (pItem->IsCatalog(m)) {
				if (pItem->IsProp("1.")) {
					if (pItem->m_curValue.Find('Y')) {
						pRcp->PatMatOpt[i - 1] |= CRecipe413::EXPB4FOCUS;
					}
				}
				else if (pItem->IsProp("2.")) {
					if (pItem->m_curValue.Find('Y')) {
						pRcp->PatMatOpt[i - 1] |= CRecipe413::FOCB4MATCH;
					}
				}
				else if (pItem->IsProp("3.")) {
					if (pItem->m_curValue.Find('Y')) {
						pRcp->PatMatOpt[i - 1] |= CRecipe413::EXPAFOCUS;
					}
				}
				else if (pItem->IsProp("4.")) {
					if (pItem->m_curValue.Find('Y')) {
						pRcp->PatMatOpt[i - 1] |= CRecipe413::FOCAMATCH;
					}
				}
				else if (pItem->IsProp("5.")) {
					if (pItem->m_curValue.Find('Y')) {
						pRcp->PatMatOpt[i - 1] |= CRecipe413::EXPAMATCH;
					}
				}
			}
		}
	}
}

void CQuickRecipeEditDlg::ParseTrimWidth() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		if (pItem->IsCatalog("Auto")) {
			if (pItem->IsProp("Inner")) {
				pRcp->AutoExposeInner = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Outer")) {
				pRcp->AutoExposeOuter = atoi(pItem->m_curValue);
			}
		}
		else if (pItem->IsCatalog("Monitor")) {
			if (pItem->IsProp("Spacing")) {
				pRcp->TrimWidthSpacing = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Lines")) {
				pRcp->TrimWidthLines = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Smooth")) {
				pRcp->TrimSmooth = atoi(pItem->m_curValue);
			}
		}
		else if (pItem->IsCatalog("Setting")) {
			if (pItem->IsProp("Outer")) {
				pRcp->TrimWidthOuterRadius = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Inner")) {
				pRcp->TrimWidthInnerRadius = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Position")) {
				pRcp->fEchoTopZ = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Threshold")) {
				pRcp->FindTrimThres = atof(pItem->m_curValue);
			}
		}
	}
}

void CQuickRecipeEditDlg::ParseTrimHeight() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		if (pItem->IsCatalog("Search")) {
			if (pItem->IsProp("Spacing")) {
				pRcp->fStepSizeTrim = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("Number")) {
				pRcp->nTrialTrim = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("Z Offset")) {
				pRcp->TrimHeightZOffset = atof(pItem->m_curValue);
			}
		}
		else if (pItem->IsCatalog("Setting")) {
			if (pItem->IsProp("Inner1")) {
				pRcp->TrimHeightDistanceInner1 = atof(pItem->m_curValue);
			}
			if (pItem->IsProp("Inner2")) {
				pRcp->TrimHeightDistanceInner2 = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("3.")) {
				pRcp->TrimHeightDistanceOuter1 = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("4.")) {
				pRcp->TrimHeightDistanceOuter2 = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("5. Number")) {
				pRcp->NMeasureOut = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("6. Number")) {
				pRcp->NMeasureIn = atof(pItem->m_curValue);
			}
			else if (pItem->IsProp("7. Sample")) {
				pRcp->SamplePerPoint = atof(pItem->m_curValue);
			}
		}
	}
}

void CQuickRecipeEditDlg::ParseBowWarp() {
	CString m, n;
	for (POSITION pos = m_propList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_propList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		if (pItem->IsCatalog("Reference")) {
			pRcp->BFPlane = CRecipe413::bfBESTFIT;
			if (pItem->IsProp("Best")) {
				if (pItem->m_curValue.Find('Y')) {
					pRcp->BFPlane = CRecipe413::bfBESTFIT;
				}
			}
			else if (pItem->IsProp("Zero")) {
				if (pItem->m_curValue.Find('Y')) {
					pRcp->BFPlane = CRecipe413::bfZEROMIN;
				}
			}
			else if (pItem->IsProp("Specified")) {
				if (pItem->m_curValue.Find('Y')) {
					pRcp->BFPlane = CRecipe413::bf3POINTS;
				}
			}
		}
		else if (pItem->IsCatalog("Control")) {
			if (pItem->IsProp("1.")) {
				pRcp->RefPt1 = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("2.")) {
				pRcp->RefPt2 = atoi(pItem->m_curValue);
			}
			else if (pItem->IsProp("3.")) {
				pRcp->RefPt3 = atoi(pItem->m_curValue);
			}
		}
	}
}

void CQuickRecipeEditDlg::OnOk3() {
	CString mdir, fname;

	DosUtil.GetProgramDir(mdir);
	fname.Format("%s\\Recipe\\%s.RCP.CSV", mdir, pRcp->RcpeName);
	m_propList.fp = fopen(fname, "wb");

	if (!m_propList.fp) {
		return;
	}
	CString str;
	str = pRcp->RcpeName;
	str.MakeUpper();
	if (str.GetLength() > 0) {
		fprintf(m_propList.fp, "RECIPE,%s.RCP\n", str.GetBuffer());
	}
	else {
		fprintf(m_propList.fp, "RECIPE,\n");
	}

	m_propList.DeleteAll();
	AddEntriesEhoMeasurement();

	m_propList.DeleteAll();
	AddEntriesEchoPeakControl();

	m_propList.DeleteAll();
	AddEntriesMeSetEcho();

	m_propList.DeleteAll();
	AddEntriesMeSetThinFilm();

	m_propList.DeleteAll();
	AddEntriesThinFilmMeasurement();

	m_propList.DeleteAll();
	AddEntriesTrimWidth();

	m_propList.DeleteAll();
	AddEnntriesTrimHeight();

	m_propList.DeleteAll();
	AddEntriesBow();

	m_propList.DeleteAll();
	AddEntriesPattern();

	AddMeasurementPoints(m_propList.fp);

	fclose(m_propList.fp);
	m_propList.fp = NULL;
}

void CQuickRecipeEditDlg::AddEntriesPattern() {
	CString Cat, n;
	for (int i = 1; i <= MAXMEPASET; i++) {
		int a = pRcp->PatMatOpt[i - 1];
		Cat.Format("MeSet%d", i);
		n.Format("%C", (a & CRecipe413::EXPB4FOCUS) ? 'Y' : 'N');
		m_propList.AddPropItem(Cat, "1. Adjust Exposure Before Focusing", n, PIT_CHECK, "");
		n.Format("%C", (a & CRecipe413::FOCB4MATCH) ? 'Y' : 'N');
		m_propList.AddPropItem(Cat, "2. Focusing Before Matching", n, PIT_CHECK, "");
		n.Format("%C", (a & CRecipe413::EXPAFOCUS) ? 'Y' : 'N');
		m_propList.AddPropItem(Cat, "3. Adjust Exposure Afer Focusing 1", n, PIT_CHECK, "");
		n.Format("%C", (a & CRecipe413::FOCAMATCH) ? 'Y' : 'N');
		m_propList.AddPropItem(Cat, "4. Focusing After Matching", n, PIT_CHECK, "");
		n.Format("%C", (a & CRecipe413::EXPAMATCH) ? 'Y' : 'N');
		m_propList.AddPropItem(Cat, "5. Adjust Exposure Afer Focusing 2", n, PIT_CHECK, "");
	}
}

void CQuickRecipeEditDlg::AddEntriesBow() {
	CString Cat, n;
	Cat = "1. Reference Plane";
	n.Format("%C", (pRcp->BFPlane == CRecipe413::bfBESTFIT) ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "1. Best Fit Plane", n, PIT_CHECK, "");
	n.Format("%C", (pRcp->BFPlane == CRecipe413::bfZEROMIN) ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "2. Zero minimum", n, PIT_CHECK, "");
	n.Format("%C", (pRcp->BFPlane == CRecipe413::bf3POINTS) ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "3. Specified Plane", n, PIT_CHECK, "");

	Cat = "2. Control Points";
	n.Format("%d", pRcp->RefPt1);
	m_propList.AddPropItem(Cat, "1. Point1", n, PIT_EDIT, "");
	n.Format("%d", pRcp->RefPt2);
	m_propList.AddPropItem(Cat, "2. Point2", n, PIT_EDIT, "");
	n.Format("%d", pRcp->RefPt3);
	m_propList.AddPropItem(Cat, "3. Point3", n, PIT_EDIT, "");
}

void CQuickRecipeEditDlg::AddEnntriesTrimHeight() {
	CString Cat, n;

	Cat = "Trim Height Setting";
	n.Format("%.3f", pRcp->TrimHeightDistanceInner1);
	m_propList.AddPropItem(Cat, "1. Inner1 Distance (0.00-20.00) mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TrimHeightDistanceInner2);
	m_propList.AddPropItem(Cat, "2. Inner2 Distance (0.00-20.00) mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TrimHeightDistanceOuter1);
	m_propList.AddPropItem(Cat, "3. Outer Distance A (0.00-20.00)mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TrimHeightDistanceOuter2);
	m_propList.AddPropItem(Cat, "4. Outer Distance B (0.00-20.00)mm", n, PIT_EDIT, "");
	n.Format("%d", pRcp->NMeasureOut);
	m_propList.AddPropItem(Cat, "5. Number of points Outer (1-32)mm", n, PIT_EDIT, "");
	n.Format("%d", pRcp->NMeasureIn);
	m_propList.AddPropItem(Cat, "6. Number of points Inner (1-32)mm", n, PIT_EDIT, "");
	n.Format("%d", pRcp->SamplePerPoint);
	m_propList.AddPropItem(Cat, "7. Sample per point (1-8)", n, PIT_EDIT, "");

	Cat = "Search Around";
	n.Format("%.3f", pRcp->fStepSizeTrim);
	m_propList.AddPropItem(Cat, "1. Spacing (0-1.000) mm", n, PIT_EDIT, "");
	n.Format("%d", pRcp->nTrialTrim);
	m_propList.AddPropItem(Cat, "2. Number of Search (0-48)", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TrimHeightZOffset);
	m_propList.AddPropItem(Cat, "3. Z Offset (0-1.000) mm", n, PIT_EDIT, "");
}

void CQuickRecipeEditDlg::AddEntriesTrimWidth() {
	CString Cat, n;

	Cat = "Trim Width Setting";
	n.Format("%.3f", pRcp->TrimWidthOuterRadius);
	m_propList.AddPropItem(Cat, "1. Outer (100.00-155.00) mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TrimWidthInnerRadius);
	m_propList.AddPropItem(Cat, "2. Inner (50.00-150.00)mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->fEchoTopZ);
	m_propList.AddPropItem(Cat, "3. Z Position (0-16.000)mm", n, PIT_EDIT, "");
	n.Format("%.2f", pRcp->FindTrimThres);
	m_propList.AddPropItem(Cat, "4. Find Trim Width Threshold (0.00-1.00)%", n, PIT_EDIT, "");

	Cat = "Monitor Wafer";
	n.Format("%d", pRcp->TrimWidthSpacing);
	m_propList.AddPropItem(Cat, "1. Spacing (1-32)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->TrimWidthLines);
	m_propList.AddPropItem(Cat, "2. Number of Lines (1-32)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->TrimSmooth);
	m_propList.AddPropItem(Cat, "3. Smooth (0-8)", n, PIT_EDIT, "");

	Cat = "Auto Exposures";
	n.Format("%d", pRcp->AutoExposeInner);
	m_propList.AddPropItem(Cat, "1. NTW / Inner TW (1-255)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->AutoExposeOuter);
	m_propList.AddPropItem(Cat, "2. Outer TW (1-255)", n, PIT_EDIT, "");
}

void CQuickRecipeEditDlg::AddEntriesThinFilmMeasurement() {
	int i, j;
	CString Cat, n;
	Cat = "Setting";
	n.Format("%.3f", pRcp->fThinFilmZ);
	m_propList.AddPropItem(Cat, "1. Z2 Position (0-6.000) mm", n, PIT_EDIT, "");
	n.Format("%d", pRcp->AutoExposeTF);
	m_propList.AddPropItem(Cat, "2. Auto Exposure (1-255)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->TFNormalExposure);
	m_propList.AddPropItem(Cat, "3. TF Exposure (1-2040)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->TFTakeSpecExposure);
	m_propList.AddPropItem(Cat, "4. Exposure Spec (1-2040)", n, PIT_EDIT, "");

	for (i = 1; i <= MAXMEPASET; i++) {
		Cat.Format("Setting: MeSet%d", i);
		n.Format("%.3f", pRcp->GoFScore[i - 1]);
		m_propList.AddPropItem(Cat, "1. GOF (0-1.00)", n, PIT_EDIT, "");
		n.Format("%d", pRcp->MatchRes[i - 1]);
		m_propList.AddPropItem(Cat, "2. Resolution (1-8) pixels", n, PIT_EDIT, "");
		n.Format("%d", pRcp->SearchDept[i - 1]);
		m_propList.AddPropItem(Cat, "3. Depth (0-16)", n, PIT_EDIT, "");
		n.Format("%.3f", pRcp->SearchDist[i - 1]);
		m_propList.AddPropItem(Cat, "4. Distance (0-1.00) mm", n, PIT_EDIT, "");
		n.Format("%d", pRcp->SubSamI[i - 1]);
		m_propList.AddPropItem(Cat, "5. Sampling (I)", n, PIT_EDIT, "");
		n.Format("%d", pRcp->SubSamS[i - 1]);
		m_propList.AddPropItem(Cat, "6. Sampling (S)", n, PIT_EDIT, "");
		n.Format("%s", MatchMode[pRcp->MatchAlgorithm[i - 1]]);
		m_propList.AddPropItem(Cat, "7. Match Mode", n, PIT_COMBO, "Position|Second template|""|Alternate template");
	}

	for (i = 1; i <= MAXMEPASET; i++) {
		CMeParam* pM = &pRcp->MeParam[i - 1];
		for (int j = 1; j <= MAXTHKVLU; j++) {
			Cat.Format("Thickness Range: MeSet%d, Point%d", i, j);
			n.Format("%.3f", pM->Ta.ThkMin[j - 1]);
			m_propList.AddPropItem(Cat, "1. Min", n, PIT_EDIT, "");
			n.Format("%.3f", pM->Ta.ThkMax[j - 1]);
			m_propList.AddPropItem(Cat, "2. Max", n, PIT_EDIT, "");
			n.Format("%.3f", pM->Ta.RI[j - 1]);
			m_propList.AddPropItem(Cat, "3. RI", n, PIT_EDIT, "");
		}
	}

	for (j = 1; j <= MAXMEPASET; j++) {
		for (i = 1; i <= 2; i++) {
			Cat.Format("Offsets PaSet%d P%d", j, i);
			n.Format("%.3f", pRcp->POffsetX[j - 1][i - 1]);
			m_propList.AddPropItem(Cat, "1. X", n, PIT_EDIT, "");
			n.Format("%.3f", pRcp->POffsetY[j - 1][i - 1]);
			m_propList.AddPropItem(Cat, "2. Y", n, PIT_EDIT, "");
		}
	}
}

void CQuickRecipeEditDlg::AddEntriesMeSetThinFilm() {
	CString Cat, n;
	for (int i = 1; i <= MAXMEPASET; i++) {
		Cat.Format("MeSet%d", i);
		CMeParam* pM = &pRcp->MeParam[i - 1];
		n.Format("%d", pM->Ta.IntegTime);
		m_propList.AddPropItem(Cat, "1. Integration time (0-1000) msec", n, PIT_EDIT, "");
		n.Format("%d", pM->Ta.Boxcar);
		m_propList.AddPropItem(Cat, "2. Boxcar Averager (1-10", n, PIT_EDIT, "");
		n.Format("%d", pM->Ta.Method);
		m_propList.AddPropItem(Cat, "3. Method (0-2)", n, PIT_EDIT, "");
		n.Format("%s", FFTFilter[pRcp->FFTFilter[i - 1]]);
		m_propList.AddPropItem(Cat, "4. Filters", n, PIT_COMBO, "None|Hann|Hamming|Gaussian|Exponent|BartlettHann|Blackman");
	}
	Cat = "1. Options";
	n.Format("%d", pRcp->nCycle);
	m_propList.AddPropItem(Cat, "1. Cycle (1-8)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->nExpand);
	m_propList.AddPropItem(Cat, "2. Expand (1-8)", n, PIT_EDIT, "");
	n.Format("%d", pRcp->NSmooth);
	m_propList.AddPropItem(Cat, "3. Smooth (0-16)", n, PIT_EDIT, "");
}

void CQuickRecipeEditDlg::AddEntriesMeSetEcho() {
	CString Cat, n;
	for (int i = 1; i <= MAXMEPASET; i++) {
		for (int j = 1; j <= MaXPA; j++) {
			MParam* pM = &pRcp->MeParam[i - 1].MPa[j - 1];
			Cat.Format("MeSet%d [%02d]", i, j);
			n.Format("%d", pM->Prb);
			m_propList.AddPropItem(Cat, "1. Probe (1-99)", n, PIT_EDIT, "");
			n.Format("%s", pM->Name);
			m_propList.AddPropItem(Cat, "2. Name", n, PIT_EDIT, "");
			n.Format("%d", pM->Fr);
			m_propList.AddPropItem(Cat, "3. From (1-10)", n, PIT_EDIT, "");
			n.Format("%d", pM->To);
			m_propList.AddPropItem(Cat, "4. To (1-10)", n, PIT_EDIT, "");
			n.Format("%.6f", pM->RI);
			m_propList.AddPropItem(Cat, "5. RI (Reflective Index)", n, PIT_EDIT, "");
			n.Format("%d", pM->D);
			m_propList.AddPropItem(Cat, "6. Decimal (0-5)", n, PIT_EDIT, "");
			n.Format("%.3f", pM->DF);
			m_propList.AddPropItem(Cat, "7. Default (0-1.000) mm", n, PIT_EDIT, "");
			n.Format("%.3f", pM->Off);
			m_propList.AddPropItem(Cat, "8. Value Offset (0-1.000) mm", n, PIT_EDIT, "");
			n.Format("%d", pM->NPeak);
			m_propList.AddPropItem(Cat, "9. # Peaks (0-10)", n, PIT_EDIT, "");
			n.Format("%.3f", pM->Lo);
			m_propList.AddPropItem(Cat, "A. Low Limit (0-1.000) mm", n, PIT_EDIT, "");
			n.Format("%.3f", pM->Hi);
			m_propList.AddPropItem(Cat, "B. High Limit (0-1.000) mm", n, PIT_EDIT, "");
			n.Format("%s", pM->Formula);
			m_propList.AddPropItem(Cat, "C. Formula", n, PIT_EDIT, "");
			n.Format("%C", pM->bHost ? 'Y' : 'N');
			m_propList.AddPropItem(Cat, "D. Send to Host", n, PIT_CHECK, "");
			n.Format("%.3f", pM->OffX);
			m_propList.AddPropItem(Cat, "E. StageX (0-150.000)mm", n, PIT_EDIT, "");
			n.Format("%.3f", pM->OffY);
			m_propList.AddPropItem(Cat, "E. StageY (0-150.000)mm", n, PIT_EDIT, "");
		}
	}
}

void CQuickRecipeEditDlg::AddEntriesEchoPeakControl() {
	CString Cat, n;
	for (int i = 1; i <= MAXMEPASET; i++) {
		CMeParam* pM = &pRcp->MeParam[i - 1];
		Cat.Format("MeSet%d. Top Probe Peak Control", i);
		n.Format("%d", pM->PSG[0].Thres);
		m_propList.AddPropItem(Cat, "1. Threshold (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[0].Level);
		m_propList.AddPropItem(Cat, "2. Level (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[0].Width);
		m_propList.AddPropItem(Cat, "3. Width (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[0].Near);
		m_propList.AddPropItem(Cat, "4. Nearest (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[0].Smooth);
		m_propList.AddPropItem(Cat, "5. Smooth (0-16) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[0].Minimum);
		m_propList.AddPropItem(Cat, "6. MinH% (0-90.00) %", n, PIT_EDIT, "");

		Cat.Format("MeSet%d. Bottom Probe Peak Control", i);
		n.Format("%d", pM->PSG[1].Thres);
		m_propList.AddPropItem(Cat, "1. Threshold (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[1].Level);
		m_propList.AddPropItem(Cat, "2. Level (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[1].Width);
		m_propList.AddPropItem(Cat, "3. Width (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[1].Near);
		m_propList.AddPropItem(Cat, "4. Nearest (0-2040) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[1].Smooth);
		m_propList.AddPropItem(Cat, "5. Smooth (0-16) au", n, PIT_EDIT, "");
		n.Format("%d", pM->PSG[1].Minimum);
		m_propList.AddPropItem(Cat, "6. MinH% (0-90.00) %", n, PIT_EDIT, "");
	}
}

void CQuickRecipeEditDlg::AddEntriesEhoMeasurement() {
	CString Cat, m, n;

	m = "1. Sample Averaging (1-128)";
	for (int i = 1; i <= MAXMEPASET; i++) {
		Cat.Format("1. MeSet%d", i, i);
		n.Format("%d", pRcp->MeParam[i - 1].NAve);
		m_propList.AddPropItem(Cat, m, n, PIT_EDIT, "");
	}

	Cat = "2. Setting";
	n.Format("%d", pRcp->EchoNormalExposure);
	m_propList.AddPropItem(Cat, "1. Echo Camera Exposure (1-6000)", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->fEchoTopZ);
	m_propList.AddPropItem(Cat, "2. Z1 Position (0-15.000) mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->TFZOffset);
	m_propList.AddPropItem(Cat, "3. Z1 Offset(0-12.000) mm", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->EchoFocus);
	m_propList.AddPropItem(Cat, "4. Auto Focus Preset (0-8.000) mm", n, PIT_EDIT, "");
	m_propList.AddPropItem(Cat, "5. Baseline file", pRcp->BaseLineFile, PIT_EDIT, "");

	Cat = "3. Search Around";
	n.Format("%d", pRcp->nTrialEcho);
	m_propList.AddPropItem(Cat, "1. Number of Search (0-49)", n, PIT_EDIT, "");
	n.Format("%.3f", pRcp->fStepSizeEcho);
	m_propList.AddPropItem(Cat, "2. Search Distance (0.001-1.000) mm", n, PIT_EDIT, "");

	Cat = "4. Options";
	n.Format("%C", pRcp->bEchoPeakMonitor ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "1. Show Peaks", n, PIT_CHECK, "");
	n.Format("%C", pRcp->bSaveVideo ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "2. Save Video", n, PIT_CHECK, "");
	n.Format("%C", pRcp->bUseWaferAlign ? 'Y' : 'N');
	m_propList.AddPropItem(Cat, "3. Use Secondary Wafer Alignment", n, PIT_CHECK, "");

	Cat = "5. Measurement Timeout";
	n.Format("%d", pRcp->WaferTimeOut);
	m_propList.AddPropItem(Cat, "1. Wafer (0-360000) sec", n, PIT_EDIT, "");
	n.Format("%d", pRcp->PointTimeOut);
	m_propList.AddPropItem(Cat, "2. Point (0-60000) sec", n, PIT_EDIT, "");
	n.Format("%d", pRcp->FailCountTimeOut);
	m_propList.AddPropItem(Cat, "3. Fail Count (0-48)", n, PIT_EDIT, "");
}

void CQuickRecipeEditDlg::AddMeasurementPoints(FILE* fp) {
	fprintf(fp, "Measurement Points\n");
	fprintf(fp, ",X,Y,Z,MeSet,PaSet\n");
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
		if (p) {
			p->Dump(fp);
		}
	}
}

void CQuickRecipeEditDlg::OnReplacepoints() {
	CRecipe413 RcpN;

	if (RcpN.LoadRecipe()) {
		pRcp->Patt.MP.Clear();
		CPtrList* pMPLst = &RcpN.Patt.MP.MPLst;
		POSITION pos = pMPLst->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
			if (p) {
				pRcp->Patt.AddMPoint(p->Co);
			}
		}
	}
}

void CQuickRecipeEditDlg::OnLoadrecipe() {
	pRcp->LoadRecipe();
}

void CQuickRecipeEditDlg::OnLoaddatafile() {
	if (p413App->LoadRecipeFromDataFile(*pRcp)) {
	}
}