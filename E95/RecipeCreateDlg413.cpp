// RecipeCreateDlg413.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "RecipeCreateDlg413.h"

#include "..\SYS\CUMMsg.h"
#include "..\128\CAS\CasMgr.h"
#include "..\Online\MFC\GetPW.h"
#include "..\SRC\Logindatanew.h"

#include "413App.h"
#include "InfoPanelMain.h"
#include "MotionControl.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecipeCreateDlg413 dialog

CRecipeCreateDlg413::CRecipeCreateDlg413(CWnd* pParent /*=NULL*/)
	: CRecipeCreateDlgBase(CRecipeCreateDlg413::IDD, pParent) {
	//{{AFX_DATA_INIT(CRecipeCreateDlg413)
	m_Boxcar = 0;
	m_RefractiveIndex = 0.0f;
	m_IntegrationTime = 0;
	m_ZPosTF = 0.0f;
	m_Method = 0;
	m_FThkMin = 0.0f;
	m_FThkMax = 0.0f;
	m_NAve = 1;
	m_ZPosECH = 0.0f;
	//}}AFX_DATA_INIT
	m_ThresholdFactor[0] = m_ThresholdFactor[1] = 10.0f;
	m_Level[0] = m_Level[1] = 20;
	m_PeakWidth[0] = m_PeakWidth[1] = 80;
	bInit = FALSE;
	MeSet = 1;
	PatSet = 1;
	MapCol = 0;
	m_cWaferMap.dmMode = CWaferMap::dmENGINNER;

	int i;
	for (i = 0; i < MaXPA; i++) {
		m_P[i] = 0;
		m_F[i] = 1;
		m_T[i] = 1;
		m_R[i] = 0;
		m_L[i] = 0;
		m_H[i] = 0;
		m_D[i] = 0;
		m_DF[i] = 0;
		m_Off[i] = 0;
		m_N[i] = "";
		m_Calc[i] = "";
	}
}

void CRecipeCreateDlg413::DoDataExchange(CDataExchange* pDX) {
	CRecipeCreateDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecipeCreateDlg413)
	DDX_Control(pDX, IDC_MESETBOX, m_cMeSetEch);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_PATTSET, m_cMeSetPatt);
	DDX_Control(pDX, IDC_PATTERNB1, m_cMarkA2);
	DDX_Control(pDX, IDC_PATTERNA1, m_cMarkA1);
	DDX_Control(pDX, IDC_MESETBOX1, m_cMeSetTF);
	DDX_Control(pDX, IDC_RECIPENAME, m_cRecipeName);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Text(pDX, IDC_BOXCAR2, m_Boxcar);
	DDX_Text(pDX, IDC_REFRACTIVEINDEX2, m_RefractiveIndex);
	DDX_Text(pDX, IDC_FILMMEASTIME2, m_IntegrationTime);
	DDX_Text(pDX, IDC_METHOD_REC, m_Method);
	DDX_Text(pDX, IDC_FTHKMIN_REC, m_FThkMin);
	DDX_Text(pDX, IDC_FTHKMAX_REC, m_FThkMax);
	DDX_Text(pDX, IDC_NAVE, m_NAve);
	DDX_Text(pDX, IDC_ZPOSITION_REC, m_ZPosTF);
	DDX_Text(pDX, IDC_ZPOSITION2, m_ZPosECH);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_THRESHOLDFACTOR1, m_ThresholdFactor[0]);
	DDX_Text(pDX, IDC_THRESHOLDFACTOR2, m_ThresholdFactor[1]);
	DDX_Text(pDX, IDC_RCP_LEVEL1, m_Level[0]);
	DDX_Text(pDX, IDC_RCP_LEVEL2, m_Level[1]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH1, m_PeakWidth[0]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH2, m_PeakWidth[1]);

	DDX_Text(pDX, IDC_L1, m_L[0]);
	DDX_Text(pDX, IDC_L2, m_L[1]);
	DDX_Text(pDX, IDC_L3, m_L[2]);
	DDX_Text(pDX, IDC_L4, m_L[3]);
	DDX_Text(pDX, IDC_L5, m_L[4]);
	DDX_Text(pDX, IDC_L6, m_L[5]);
	DDX_Text(pDX, IDC_L7, m_L[6]);
	DDX_Text(pDX, IDC_L8, m_L[7]);

	DDX_Text(pDX, IDC_H1, m_H[0]);
	DDX_Text(pDX, IDC_H2, m_H[1]);
	DDX_Text(pDX, IDC_H3, m_H[2]);
	DDX_Text(pDX, IDC_H4, m_H[3]);
	DDX_Text(pDX, IDC_H5, m_H[4]);
	DDX_Text(pDX, IDC_H6, m_H[5]);
	DDX_Text(pDX, IDC_H7, m_H[6]);
	DDX_Text(pDX, IDC_H8, m_H[7]);

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

	DDX_Text(pDX, IDC_PROBE1, m_P[0]);
	DDV_MinMaxInt(pDX, m_P[0], 0, 9);
	DDX_Text(pDX, IDC_PROBE2, m_P[1]);
	DDV_MinMaxInt(pDX, m_P[1], 0, 9);
	DDX_Text(pDX, IDC_PROBE3, m_P[2]);
	DDV_MinMaxInt(pDX, m_P[2], 0, 9);
	DDX_Text(pDX, IDC_PROBE4, m_P[3]);
	DDV_MinMaxInt(pDX, m_P[3], 0, 9);
	DDX_Text(pDX, IDC_PROBE5, m_P[4]);
	DDV_MinMaxInt(pDX, m_P[4], 0, 9);
	DDX_Text(pDX, IDC_PROBE6, m_P[5]);
	DDV_MinMaxInt(pDX, m_P[5], 0, 9);
	DDX_Text(pDX, IDC_PROBE7, m_P[6]);
	DDV_MinMaxInt(pDX, m_P[6], 0, 9);
	DDX_Text(pDX, IDC_PROBE8, m_P[7]);
	DDV_MinMaxInt(pDX, m_P[7], 0, 9);

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

	DDX_Text(pDX, IDC_NAME1, m_N[0]);
	DDX_Text(pDX, IDC_NAME2, m_N[1]);
	DDX_Text(pDX, IDC_NAME3, m_N[2]);
	DDX_Text(pDX, IDC_NAME4, m_N[3]);
	DDX_Text(pDX, IDC_NAME5, m_N[4]);
	DDX_Text(pDX, IDC_NAME6, m_N[5]);
	DDX_Text(pDX, IDC_NAME7, m_N[6]);
	DDX_Text(pDX, IDC_NAME8, m_N[7]);

	DDX_Text(pDX, IDC_RI1, m_R[0]);
	DDX_Text(pDX, IDC_RI2, m_R[1]);
	DDX_Text(pDX, IDC_RI3, m_R[2]);
	DDX_Text(pDX, IDC_RI4, m_R[3]);
	DDX_Text(pDX, IDC_RI5, m_R[4]);
	DDX_Text(pDX, IDC_RI6, m_R[5]);
	DDX_Text(pDX, IDC_RI7, m_R[6]);
	DDX_Text(pDX, IDC_RI8, m_R[7]);

	DDX_Text(pDX, IDC_DECIMAL1, m_D[0]);
	DDV_MinMaxInt(pDX, m_D[0], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL2, m_D[1]);
	DDV_MinMaxInt(pDX, m_D[1], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL3, m_D[2]);
	DDV_MinMaxInt(pDX, m_D[2], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL4, m_D[3]);
	DDV_MinMaxInt(pDX, m_D[3], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL5, m_D[4]);
	DDV_MinMaxInt(pDX, m_D[4], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL6, m_D[5]);
	DDV_MinMaxInt(pDX, m_D[5], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL7, m_D[6]);
	DDV_MinMaxInt(pDX, m_D[6], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL8, m_D[7]);
	DDV_MinMaxInt(pDX, m_D[7], 0, 4);

	DDX_Text(pDX, IDC_DFLT1, m_DF[0]);
	DDX_Text(pDX, IDC_DFLT2, m_DF[1]);
	DDX_Text(pDX, IDC_DFLT3, m_DF[2]);
	DDX_Text(pDX, IDC_DFLT4, m_DF[3]);
	DDX_Text(pDX, IDC_DFLT5, m_DF[4]);
	DDX_Text(pDX, IDC_DFLT6, m_DF[5]);
	DDX_Text(pDX, IDC_DFLT7, m_DF[6]);
	DDX_Text(pDX, IDC_DFLT8, m_DF[7]);

	DDX_Text(pDX, IDC_OFF1, m_Off[0]);
	DDX_Text(pDX, IDC_OFF2, m_Off[1]);
	DDX_Text(pDX, IDC_OFF3, m_Off[2]);
	DDX_Text(pDX, IDC_OFF4, m_Off[3]);
	DDX_Text(pDX, IDC_OFF5, m_Off[4]);
	DDX_Text(pDX, IDC_OFF6, m_Off[5]);
	DDX_Text(pDX, IDC_OFF7, m_Off[6]);
	DDX_Text(pDX, IDC_OFF8, m_Off[7]);

	DDX_Text(pDX, IDC_FORMULA1, m_Calc[0]);
	DDX_Text(pDX, IDC_FORMULA2, m_Calc[1]);
	DDX_Text(pDX, IDC_FORMULA3, m_Calc[2]);
	DDX_Text(pDX, IDC_FORMULA4, m_Calc[3]);
	DDX_Text(pDX, IDC_FORMULA5, m_Calc[4]);
	DDX_Text(pDX, IDC_FORMULA6, m_Calc[5]);
	DDX_Text(pDX, IDC_FORMULA7, m_Calc[6]);
	DDX_Text(pDX, IDC_FORMULA8, m_Calc[7]);
}

BEGIN_MESSAGE_MAP(CRecipeCreateDlg413, CRecipeCreateDlgBase)
	//{{AFX_MSG_MAP(CRecipeCreateDlg413)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_NEWRECIPE, OnNewrecipe)
	ON_BN_CLICKED(IDC_HOST_UPLOAD, OnHostUpload)
	ON_BN_CLICKED(IDC_ECHOPROP, OnEchoprop)
	ON_BN_CLICKED(IDC_LOCATIONS, OnLocations)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_ENLARGE1, OnEnlarge)
	ON_BN_CLICKED(IDC_SAVEASTEXT, OnSaveastext)
	ON_BN_CLICKED(IDC_LOADC2C, OnLoadc2c)
	ON_COMMAND(ID_REFRESHUI, OnRefreshui)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecipeCreateDlg413 message handlers

void CRecipeCreateDlg413::LockView() {}

void CRecipeCreateDlg413::PostNcDestroy() {
	CRecipeCreateDlgBase::PostNcDestroy();
	delete this;
}

// void CRecipeCreateDlg413::OnLogin()
// {
// 	if (bLock)
// 	{
// 		if (pGDev->AuthenticateUser())
// 		{
// 			EnableDlgItems();
// 			bLock= FALSE;
// 			ReloadDialog();
// 		}
// 	}
// }

void CRecipeCreateDlg413::EnableDlgItems() {
	EnaDisDlgItems(TRUE);
}

void CRecipeCreateDlg413::DisableDlgItems() {
	EnaDisDlgItems(FALSE);
}

void CRecipeCreateDlg413::EnaDisDlgItems(BOOL bEna) {
	EnaDlg(IDC_SAVERECIPE, bEna);
	EnaDlg(IDC_NEWRECIPE, bEna);
	EnaDlg(IDC_LOCATIONS, bEna);
	EnaDlg(IDC_ECHOPROP, bEna);
	EnaDlg(IDC_THINFILMPROP, bEna);
	EnaDlg(IDC_PATTERNPROP, bEna);
}

void CRecipeCreateDlg413::ReloadDialog() {
	if (!bInit) {
		return;
	}
	m_cRecipeName.SetWindowText(Rcp.GetRecipeName());
	m_cWaferMap.Redraw();
	UpdateUI();
}

BOOL CRecipeCreateDlg413::OnInitDialog() {
	CRecipeCreateDlgBase::OnInitDialog();

	// 	AddAnchor(IDC_LOGIN, TOP_RIGHT);
	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_LOADC2C, TOP_RIGHT);
	AddAnchor(IDC_SAVEASTEXT, TOP_RIGHT);
	// 	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_NEWRECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_HOST_UPLOAD, TOP_RIGHT);

	// 	AddAnchor(IDC_LOCATIONS, TOP_RIGHT);
	// 	AddAnchor(IDC_ECHOPROP, TOP_RIGHT);
	// 	AddAnchor(IDC_THINFILMPROP, TOP_RIGHT);
	// 	AddAnchor(IDC_PATTERNPROP, TOP_RIGHT);

	// 	AddAnchor(IDC_CAMERA1, TOP_RIGHT);
	// 	AddAnchor(IDC_CAMERA2, TOP_RIGHT);
	// 	AddAnchor(IDC_CAMERACONTROL, TOP_RIGHT);
	// 	AddAnchor(IDC_MOTIONCONTROL, BOTTOM_RIGHT);
	// 	AddAnchor(IDC_ECHOCONTROL, BOTTOM_RIGHT);
	// 	AddAnchor(IDC_THINFILMCONTROL, BOTTOM_RIGHT);

	// 	AddAnchor(IDC_CLOSETAB, BOTTOM_RIGHT);

	// 	AddAnchor(IDC_REFRESH, BOTTOM_LEFT);

	UpdateData(FALSE);

	bInit = TRUE;

	m_cWaferMap.pRcp = &Rcp;
	m_cWaferMap.pParent = this;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]

	char* dwCjName[] =
	{
		"#", "Position", "Me", "Pat"
	};
	int nSize[] = { 32, 92, 32, 32 };
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

	ReloadDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecipeCreateDlg413::OnLoadrecipe() {
	Rcp.LoadRecipe();
	Rcp.UpdateControl(m_cPoint);
	ReloadDialog();
}

void CRecipeCreateDlg413::OnSaverecipe() {
	if (p413App->ValidCheckRcp(Rcp)) {
		Rcp.SaveRecipe();
	}
}

void CRecipeCreateDlg413::OnNewrecipe() {
	if (!bLock) {
		Rcp.Clear();
		ReloadDialog();
	}
}

void CRecipeCreateDlg413::OnHostUpload() {}

LRESULT CRecipeCreateDlg413::OnTabSelected(WPARAM wP, LPARAM lP) {
	return CRecipeCreateDlgBase::OnTabSelected(wP, lP);
}

void CRecipeCreateDlg413::OnEchoprop() {
	p413App->EchoProp(Rcp, MeSet);
}

// void CRecipeCreateDlg413::OnThinfilmprop()
// {
// 	p413App->ThinFilmProp(Rcp);
// }

// void CRecipeCreateDlg413::OnPatternprop()
// {
// 	p413App->PatternProp(Rcp);
// }

void CRecipeCreateDlg413::OnLocations() {
	p413App->LocationProp(Rcp, m_hWnd);
	OnRefresh();
}

// void CRecipeCreateDlg413::OnMotioncontrol()
// {
// 	if (p413App->pMCtrl)
// 	{
// 		p413App->pMCtrl->pWnd = this;
// 		::SetWindowPos(p413App->pMCtrl->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
// 		p413App->pMCtrl->ShowWindow(SW_SHOW);
// 	}
// }

void CRecipeCreateDlg413::OnSize(UINT nType, int cx, int cy) {
	CRecipeCreateDlgBase::OnSize(nType, cx, cy);

	ReloadDialog();
}

void CRecipeCreateDlg413::OnRefresh() {
	m_cWaferMap.Redraw();
}

void CRecipeCreateDlg413::OnSet1() {
	MeSet = 1;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet2() {
	MeSet = 2;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet3() {
	MeSet = 3;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet4() {
	MeSet = 4;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet5() {
	MeSet = 5;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet6() {
	MeSet = 6;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet7() {
	MeSet = 7;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet8() {
	MeSet = 8;
	UpdateUI();
}

void CRecipeCreateDlg413::OnSet9() {
	MeSet = 9;
	UpdateUI();
}

void CRecipeCreateDlg413::UpdateUI() {
	CString str;

	// Thin Film Section [8/15/2010 XPMUser]

	CMeParam* p = &Rcp.MeParam[MeSet - 1];
	m_IntegrationTime = p->Ta.IntegTime;
	m_Boxcar = p->Ta.Boxcar;
	m_RefractiveIndex = p->Ta.RI[0];
	m_Method = p->Ta.Method;
	m_FThkMax = p->Ta.ThkMax[0];
	m_FThkMin = p->Ta.ThkMin[0];

	m_ZPosTF = Rcp.fThinFilmZ;

	str.Format("Thin Film Parameter set %d", PatSet);
	m_cMeSetTF.SetWindowText(str);

	// Pattern Matching Section [8/15/2010 XPMUser]
	m_cMarkA1.DupStore1(Rcp.MatchDIBA1[PatSet - 1]);
	//m_cMarkA1.Refresh();

	m_cMarkA2.DupStore1(Rcp.MatchDIBB1[PatSet - 1]);
	//m_cMarkA2.Refresh();

	str.Format("Pattern set %d", PatSet);
	m_cMeSetPatt.SetWindowText(str);

	// Echo Probe Section [8/15/2010 XPMUser]
	int i;
	for (i = 0; i < MaXPA; i++) {
		m_P[i] = Rcp.MeParam[MeSet - 1].MPa[i].Prb;
		m_F[i] = Rcp.MeParam[MeSet - 1].MPa[i].Fr;
		m_T[i] = Rcp.MeParam[MeSet - 1].MPa[i].To;
		m_R[i] = Rcp.MeParam[MeSet - 1].MPa[i].RI;
		m_L[i] = Rcp.MeParam[MeSet - 1].MPa[i].Lo;
		m_H[i] = Rcp.MeParam[MeSet - 1].MPa[i].Hi;
		m_D[i] = Rcp.MeParam[MeSet - 1].MPa[i].D;
		m_DF[i] = Rcp.MeParam[MeSet - 1].MPa[i].DF;
		m_Off[i] = Rcp.MeParam[MeSet - 1].MPa[i].Off;
		m_N[i] = Rcp.MeParam[MeSet - 1].MPa[i].Name;
		m_Calc[i] = Rcp.MeParam[MeSet - 1].MPa[i].Formula;
	}
	MapCol = Rcp.Desc.MapCol;

	for (i = 0; i < 2; i++) {
		m_ThresholdFactor[i] = Rcp.MeParam[MeSet - 1].PSG[i].Thres;
		m_Level[i] = Rcp.MeParam[MeSet - 1].PSG[i].Level;
		m_PeakWidth[i] = Rcp.MeParam[MeSet - 1].PSG[i].Width;
	}

	m_NAve = Rcp.MeParam[MeSet - 1].NAve;
	m_ZPosECH = Rcp.fEchoTopZ;

	str.Format("Substrate Thickness Parameter set %d", MeSet);
	m_cMeSetEch.SetWindowText(str);

	UpdateData(FALSE);
}

void CRecipeCreateDlg413::OnEnlarge() {
	p413App->Enlarge(&Rcp, /*m_cWaferMap.strList,*/ &m_cWaferMap);
}

void CRecipeCreateDlg413::OnSaveastext() {
	CString str;
	str.Format("C:\\TEMP\\%s.RCP.TXT", Rcp.RcpeName);
	Rcp.Verbose(str);
}

void CRecipeCreateDlg413::OnLoadc2c() {
	CString month;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y%m");
	month += "\\C2C";

	CFileDialog dlg(TRUE,
		"C2C",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Data file|*.C2C|");

	CString filename = "";
	CString root = DosUtil.DirSys.GetRootDir() + "\\" + month;
	dlg.m_ofn.lpstrInitialDir = root;

	if (dlg.DoModal() != IDOK) {
		return;
	}
	filename = dlg.GetPathName();

	CCasMgr CasMgr;
	if (CasMgr.OpenCassette(filename)) {
		if (CasMgr.pCas) {
			Rcp = *CasMgr.GetCurCas()->GetRecipe413();
			Rcp.UpdateControl(m_cPoint);
			ReloadDialog();
		}
	}
}

void CRecipeCreateDlg413::OnRefreshui() {
	ReloadDialog();
}