#include "stdafx.h"
#include "e95.h"

#include "413/MotorS2.h"
#include "413App.h"
#include "Data.h"
#include "EditCoordinateDlg.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "LOG/LogMsgList.h"
#include "MPoint.h"
#include "SYS/CUMMsg.h"
#include "XTabCtrl.h"
// #include "InfoPanelSetup.h"
// #include "MParam.h"
// #include "Recipe413.h"
// #include "SelectMeasurementDlg.h"
// #include "SetupProp3Dlg.h"
// #include "SiCamSettingDlg.h"
// #include "TFTestDlg.h"
// #include <float.h>
// #include <math.h>

#include "setupprob4dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_START	9999

/////////////////////////////////////////////////////////////////////////////
// CSetupProp4Dlg CResizableDialog

CSetupProp4Dlg::CSetupProp4Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp4Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp4Dlg)
	//}}AFX_DATA_INIT

	int i;

	nSel = 0;
	CurNo = 0;
	m_bStopFlag = TRUE;

	for (i = 0; i < MAXTHKVLU; i++) {
		m_FThkMax[i] = 10.0f;
		m_FThkMin[i] = 0.0f;
		m_RI[i] = 3.69f;
	}

	LoadParam();
	SaveParam();

	Initialize();
	m_cVideo.Name = "Setup4";
}

void CSetupProp4Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp4Dlg)
	DDX_Control(pDX, IDC_AUTOEXPOSE, m_cAutoExpose);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_SELECTEDPT, m_cSelectedPt);
	DDX_Control(pDX, IDC_MICROSCOPELIGHTOFF2, m_cMicroscopeLightOff);
	DDX_Control(pDX, IDC_MICROSCOPELIGHTON2, m_cMicroscopeLightOn);
	DDX_Control(pDX, IDC_CAMERASETTING2, m_cCameraSetting);
	DDX_Control(pDX, IDC_AUTOFOCUS1, m_cAutoFocus);
	DDX_Control(pDX, IDC_MATCH, m_cMatch);
	DDX_Control(pDX, IDC_LIST3, m_cList3);
	DDX_Control(pDX, IDC_RESULT3, m_cResult3);
	DDX_Control(pDX, IDC_RESULT2, m_cResult2);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_MESSAGE, m_cList);
	DDX_Control(pDX, IDC_MEASURE, m_cMeasure);
	DDX_Control(pDX, IDC_REFERENCE, m_cReference);
	DDX_Control(pDX, IDC_BACKGROUND, m_cBackground);
	DDX_Control(pDX, IDC_RESULT, m_cResult);
	DDX_Control(pDX, IDC_SPECTRUM, m_cSpec);
	DDX_Control(pDX, IDC_VIDEO, m_cVideo);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FTHKMAX_SP4, m_FThkMax[0]);
	DDX_Text(pDX, IDC_FTHKMAX_SP5, m_FThkMax[1]);
	DDX_Text(pDX, IDC_FTHKMAX_SP6, m_FThkMax[2]);
	DDX_Text(pDX, IDC_FTHKMAX_SP7, m_FThkMax[3]);
	DDX_Text(pDX, IDC_FTHKMAX_SP8, m_FThkMax[4]);
	DDX_Text(pDX, IDC_FTHKMAX_SP9, m_FThkMax[5]);
	DDX_Text(pDX, IDC_FTHKMIN_SP4, m_FThkMin[0]);
	DDX_Text(pDX, IDC_FTHKMIN_SP5, m_FThkMin[1]);
	DDX_Text(pDX, IDC_FTHKMIN_SP6, m_FThkMin[2]);
	DDX_Text(pDX, IDC_FTHKMIN_SP7, m_FThkMin[3]);
	DDX_Text(pDX, IDC_FTHKMIN_SP8, m_FThkMin[4]);
	DDX_Text(pDX, IDC_FTHKMIN_SP9, m_FThkMin[5]);
	DDX_Text(pDX, IDC_RI1, m_RI[0]);
	DDX_Text(pDX, IDC_RI2, m_RI[1]);
	DDX_Text(pDX, IDC_RI3, m_RI[2]);
	DDX_Text(pDX, IDC_RI4, m_RI[3]);
	DDX_Text(pDX, IDC_RI5, m_RI[4]);
	DDX_Text(pDX, IDC_RI6, m_RI[5]);
}

BEGIN_MESSAGE_MAP(CSetupProp4Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp4Dlg)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_BACKGROUND, OnBackground)
	ON_BN_CLICKED(IDC_REFERENCE, OnReference)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_CAMERASETTING2, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL3, OnMotioncontrol3)
	ON_BN_CLICKED(IDC_MEASURE, OnMeasure)
	ON_BN_CLICKED(IDC_TAKEPICTURE, OnTakepicture)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTON2, OnMicroscopelighton2)
	ON_BN_CLICKED(IDC_MICROSCOPELIGHTOFF2, OnMicroscopelightoff2)
	ON_BN_CLICKED(IDC_SPECTROMETERLIGHTON2, OnSpectrometerlighton2)
	ON_BN_CLICKED(IDC_SPECTROMETERLIGHTOFF2, OnSpectrometerlightoff2)
	ON_BN_CLICKED(IDC_SAVESPECTRUM, OnSavespectrum)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_LOCATIONS, OnLocations)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_AUTOFOCUS1, OnAutofocus1)
	ON_COMMAND(IDC_MATCHPATTERN, OnMatchpattern)
	ON_BN_CLICKED(IDC_COORDINATES, OnCoordinates)
	ON_WM_TIMER()
	ON_COMMAND(ID_REFRESHUI, OnRefresh)
	ON_BN_CLICKED(IDC_PATSET1, OnPatset1)
	ON_BN_CLICKED(IDC_PATSET2, OnPatset2)
	ON_BN_CLICKED(IDC_PATSET3, OnPatset3)
	ON_BN_CLICKED(IDC_PATSET4, OnPatset4)
	ON_BN_CLICKED(IDC_PATSET5, OnPatset5)
	ON_BN_CLICKED(IDC_PATSET6, OnPatset6)
	ON_BN_CLICKED(IDC_PATSET7, OnPatset7)
	ON_BN_CLICKED(IDC_PATSET8, OnPatset8)
	ON_BN_CLICKED(IDC_PATSET9, OnPatset9)
	ON_BN_CLICKED(IDC_MEASURE2, OnMeasure2)
	ON_BN_CLICKED(IDC_AUTOEXPOSE, OnAutoexpose)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList1)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP4, OnKillfocusFthkminSp4)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP4, OnKillfocusFthkmaxSp4)
	ON_EN_KILLFOCUS(IDC_RI1, OnKillfocusRi1)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP5, OnKillfocusFthkminSp5)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP5, OnKillfocusFthkmaxSp5)
	ON_EN_KILLFOCUS(IDC_RI2, OnKillfocusRi2)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP6, OnKillfocusFthkminSp6)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP6, OnKillfocusFthkmaxSp6)
	ON_EN_KILLFOCUS(IDC_RI3, OnKillfocusRi3)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP7, OnKillfocusFthkminSp7)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP7, OnKillfocusFthkmaxSp7)
	ON_EN_KILLFOCUS(IDC_RI4, OnKillfocusRi4)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP8, OnKillfocusFthkminSp8)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP8, OnKillfocusFthkmaxSp8)
	ON_EN_KILLFOCUS(IDC_RI5, OnKillfocusRi5)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP9, OnKillfocusFthkminSp9)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP9, OnKillfocusFthkmaxSp9)
	ON_EN_KILLFOCUS(IDC_RI6, OnKillfocusRi6)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DN, OnDn)
	ON_BN_CLICKED(IDC_HERE, OnHere)
	ON_BN_CLICKED(IDC_UPDATEPT, OnUpdatept)
	ON_BN_CLICKED(IDC_ADDZOFFSET, OnAddzoffset)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_BN_CLICKED(IDC_RUNRECIPE, OnRunrecipe)
	ON_BN_CLICKED(IDC_PATSSETA, OnPatsseta)
	ON_BN_CLICKED(IDC_PATSSETB, OnPatssetb)
	ON_BN_CLICKED(IDC_PATSSETC, OnPatssetc)
	ON_BN_CLICKED(IDC_PATSSETD, OnPatssetd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)
	ON_MESSAGE(ID_RELOCATETOTOP, OnRelocateToTop)
	ON_MESSAGE(ID_RELOCATETOBACK, OnRelocateToBack)
	ON_MESSAGE(IDC_EDITPOINT, OnEditpoint)
	ON_MESSAGE(IDC_ADDPOINT, OnAddpoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_COMMAND(UM_BUTTON_RDCLICK, OnCamerasetting)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp4Dlg message handlers
long CSetupProp4Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, &m_cList, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.pWnd = this;
			m_cVideo.RSpecClear();
		}
		m_cSpec.RSpec.Clear();
		RecipeToLocal();
		m_cWaferMap.pRcp = pRcp;
		pRcp->UpdateControl(m_cPoint);

		bTabSelected = TRUE;
	}
	return 0;
}

long CSetupProp4Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		m_cVideo.pWnd = NULL;
		m_cVideo.RSpecClear();
		StopOOI();
		if (!m_cVideo.IsReserved()) {
			p413App->StopCamera(&m_cVideo, NULL);
		}
		LocalToRecipe();
		bTabSelected = FALSE;
	}
	return 0;
}

void CSetupProp4Dlg::PostNcDestroy() {
	Deinitialize();
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CSetupProp4Dlg::OnBackground() {
	BlockUI();

	m_cBackground.EnableWindow(FALSE);
	StopOOI();

	float pos;
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		if (pMtr) pMtr->GetThinFilmZPosDev(pos);
		if (pMtr) pMtr->MoveThinFilmZ(g->CalZPosTF);
		if (pMtr) pMtr->WaitThinFilmZ();
	}
	CString str;
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
	Message(str);
	COceanBase* pOcean = p413App->pOceanOp;
	if (pOcean) {
		//p413App->pOceanOp->SetThkRange2(p->Ta.fThkMin[0],p->Ta.fThkMax[0],p->Ta.Method,1,p->Ta.fRI[0],p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
		pOcean->SetParam(p->Ta.Method, 1, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
		pOcean->ClearPolyThk(&pOcean->BackGdSpectrum);
		pOcean->SetActivatePoly(&pOcean->BackGdSpectrum, 0, p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.RI[0]);
		if (p413App->TakeBackground(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
			DisplaySpectrum(&pOcean->BackGdSpectrum);
		}
	}
	if (pMtr) pMtr->MoveThinFilmZ(pos);
	m_cBackground.EnableWindow(TRUE);

	ReleaseUI();
}

void CSetupProp4Dlg::OnReference() {
	BlockUI();

	m_cReference.EnableWindow(FALSE);
	StopOOI();

	float pos;
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) pMtr->GetThinFilmZPosDev(pos);
	if (pMtr) pMtr->MoveThinFilmZ(g->CalZPosTF);
	if (pMtr) pMtr->WaitThinFilmZ();
	CString str;
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
	Message(str);
	COceanBase* pOcean = p413App->pOceanOp;
	if (pOcean) {
		//p413App->pOceanOp->SetThkRange2(p->Ta.fThkMin[0],p->Ta.fThkMax[0],p->Ta.Method,1,p->Ta.fRI[0],p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
		pOcean->SetParam(p->Ta.Method, 1, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
		pOcean->ClearPolyThk(&pOcean->CalibrSpectrum);
		pOcean->SetActivatePoly(&pOcean->CalibrSpectrum, 0, p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.RI[0]);
		if (p413App->TakeReference(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
			DisplaySpectrum(&pOcean->CalibrSpectrum);
		}
	}
	if (pMtr) pMtr->MoveThinFilmZ(pos);
	m_cReference.EnableWindow(TRUE);

	ReleaseUI();
}

void CSetupProp4Dlg::OnStart() {
	if (m_bStopFlag) {
		BlockUI();
		StartOOI();
		m_cVideo.SetExposure(pRcp->TFTakeSpecExposure + g->BaseExpoTF);
	}
	else {
		m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
		StopOOI();
		ReleaseUI();
	}
}

void CSetupProp4Dlg::OnReset() {
	if (p413App->pOceanOp) {
		p413App->pOceanOp->CalibrationReset(NumDATAPT);
	}
}

BOOL CSetupProp4Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_BACKGROUND, TOP_RIGHT);
	AddAnchor(IDC_REFERENCE, TOP_RIGHT);
	AddAnchor(IDC_START, TOP_RIGHT);
	AddAnchor(IDC_RESET, TOP_RIGHT);
	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_SAVESPECTRUM, TOP_RIGHT);

	AddAnchor(IDC_MOTIONCONTROL3, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING2, TOP_RIGHT);
	AddAnchor(IDC_MEASURE, TOP_RIGHT);
	AddAnchor(IDC_MEASURE2, TOP_RIGHT);
	AddAnchor(IDC_LIST3, TOP_LEFT, BOTTOM_LEFT);

	AddAnchor(IDC_MICROSCOPELIGHTON2, TOP_RIGHT);
	AddAnchor(IDC_MICROSCOPELIGHTOFF2, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERLIGHTON2, TOP_RIGHT);
	AddAnchor(IDC_SPECTROMETERLIGHTOFF2, TOP_RIGHT);
	AddAnchor(IDC_TAKEPICTURE, TOP_RIGHT);
	AddAnchor(IDC_TEST, BOTTOM_RIGHT);

	AddAnchor(IDC_WAFERMAP, TOP_LEFT, BOTTOM_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (g->Confg.bThinFilmMicroscopeMod) {
		m_cCameraSetting.ShowWindow(SW_SHOW);
		m_cMicroscopeLightOff.ShowWindow(SW_SHOW);
		m_cMicroscopeLightOn.ShowWindow(SW_SHOW);
		if (g->Confg.bThinFilmMotorMod) {
			m_cAutoFocus.ShowWindow(SW_SHOW);
		}
	}

	SetupColumn();

	RecipeToLocal();

	m_cVideo.SetRecipe(pRcp);
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

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

	SetTimer(IDT_START, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp4Dlg::OnClose() {
	StopOOI();
	p413App->StopCamera(&m_cVideo, NULL);
	CResizableDialog::OnClose();
}

BOOL CSetupProp4Dlg::DestroyWindow() {
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp4Dlg::OnSaverecipe() {
	RecipeIO();
	pRcp->UpdateControl(m_cPoint);
	// 	LocalToRecipe();
	// 	if (p413App->ValidCheckRcp(g->RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

void CSetupProp4Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp4Dlg::LoadParam() {}

void CSetupProp4Dlg::SaveParam() {}

BOOL CSetupProp4Dlg::Initialize() {
	m_hThread = NULL;

	Rmeasmin = FLT_MAX;
	Rmeasmax = FLT_MIN;

	m_nLmax = 1650.0f;
	m_nLmin = 950.0f;

	return TRUE;
}

void CSetupProp4Dlg::Deinitialize() {}

void CSetupProp4Dlg::OnMotioncontrol3() {
	ShowMotionControl(this);
}

void CSetupProp4Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp4Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp4Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp4Dlg::OnSet4() {
	SwapMeSet(4);
}

void CSetupProp4Dlg::OnSet5() {
	SwapMeSet(5);
}

void CSetupProp4Dlg::OnSet6() {
	SwapMeSet(6);
}

void CSetupProp4Dlg::OnSet7() {
	SwapMeSet(7);
}

void CSetupProp4Dlg::OnSet8() {
	SwapMeSet(8);
}

void CSetupProp4Dlg::OnSet9() {
	SwapMeSet(9);
}

void CSetupProp4Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}
void CSetupProp4Dlg::OnMeasure() {
	BlockUI();

	UpdateData(TRUE);
	COceanBase* pOcean = p413App->pOceanOp;
	if (!pOcean) {
		return;
	}
	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	CString str;
	str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
	Message(str);

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\SEM06\\%03d-0_1.CSV";

	pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
	m_cMeasure.EnableWindow(FALSE);
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroIllumLight(FALSE);
		if (!pOcean->bSimu) {
			Sleep(700);
		}
	}
	m_cVideo.SetExposure(pRcp->TFTakeSpecExposure + g->BaseExpoTF);
	int temp = pOcean->StartTF(p, dir, 49 * rand() / float(RAND_MAX) + 1);
	BOOL bTaken = FALSE;
	pOcean->ClearPolyThk(&pOcean->CurrSpectrum);
	for (int i = 0; i < MAXTHKVLU; i++) {
		if (p->Ta.RI[i] > 0) {
			pOcean->SetActivatePoly(&pOcean->CurrSpectrum, i, p->Ta.ThkMin[i], p->Ta.ThkMax[i], p->Ta.RI[i]);
		}
	}
	BOOL bret = pOcean->Result(&pOcean->CurrSpectrum, &pOcean->ResultSpectrum, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);

	if (bret) {
		DisplaySpectrum(&pOcean->ResultSpectrum);
		CSingleLock Lock(&m_cVideo.RsltSpec.CS);
		Lock.Lock();
		m_cVideo.RsltSpec.Dup(pOcean->ResultSpectrum);
		Lock.Unlock();
		ShowResultUI();
	}
	m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(TRUE);

	if (bret && g->bTakePictureOnMeasure) {
		Sleep(800);
		OnTakepicture();
	}
	m_cMeasure.EnableWindow(TRUE);

	ReleaseUI();
}

void CSetupProp4Dlg::StartOOI() {
	CWnd* pButton = GetDlgItem(IDC_START);
	pButton->SetWindowText("");

	DWORD dwID;
	if (!m_bStopFlag) {
		m_bStopFlag = TRUE;
		if (m_hThread) {
			WaitForSingleObject(m_hThread, 5000);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
	pButton->SetWindowText("Stop");
	m_bStopFlag = FALSE;
	// Get ready measurement parameters [7/26/2010 Yuen]
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MeasureProcTF, this, 0, &dwID);
}

void CSetupProp4Dlg::StopOOI() {
	CWnd* pButton = GetDlgItem(IDC_START);
	pButton->SetWindowText("");

	m_bStopFlag = TRUE;
	if (m_hThread) {
		WaitForSingleObject(m_hThread, 1000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	m_bStopFlag = TRUE;
	pButton->SetWindowText("Run");
}

// Static function [1/10/2013 FSM413]
// Processing thread [5/26/2014 Yuen]
// Independent thread: Measurement thread [7/26/2010 Yuen]
DWORD CSetupProp4Dlg::MeasureProcTF(LPVOID pParam) {
	CSetupProp4Dlg* pDlg = (CSetupProp4Dlg*)pParam;
	C413Global* g = &p413App->Global;
	CRecipe413* pRcp = &g->RcpSetup;
	COceanBase* pOcean = p413App->pOceanOp;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\SEM06\\%03d-0_1.CSV";

	while (!pDlg->m_bStopFlag) {
		CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
		CString str;
		str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
		pDlg->Message(str);
		//pOcean->SetThkRange2(p->Ta.fThkMin[0],p->Ta.fThkMax[0],p->Ta.Method,1,p->Ta.fRI[0],p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
		pOcean->SetParam(p->Ta.Method, 1, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
		int temp = pOcean->StartTF(p, dir, 49 * rand() / float(RAND_MAX) + 1);
		if ((temp == 512) || (temp == 2048)) {
			pOcean->ClearPolyThk(&pOcean->CurrSpectrum);
			pOcean->SetActivatePoly(&pOcean->CurrSpectrum, 0, p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.RI[0]);
			switch (pOcean->Result(&pOcean->CurrSpectrum, &pOcean->ResultSpectrum, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand)) {
			case -1:
				pDlg->Message("No good points");
				break;
			}
			//pDlg->m_Poly = pOcean->ResultSpectrum.Poly[0].fThk;
			pDlg->DisplaySpectrum(&pOcean->ResultSpectrum);
			pDlg->ShowResultUI();
			pOcean->Wait();
		}
	}
	CWnd* pButton = pDlg->GetDlgItem(IDC_START);
	pDlg->m_bStopFlag = TRUE;
	pButton->SetWindowText("Run");
	return 1;
}

// [7/1/2020]
// Similar to MeasureProcTF, but with recipe point measurement and data exporting
UINT CSetupProp4Dlg::RunRecipeProcTF(LPVOID pParam) {
	CSetupProp4Dlg* pDlg = (CSetupProp4Dlg*)pParam;
	C413Global* g = &p413App->Global;
	CRecipe413* pRcp = &g->RcpSetup;
	CPattern* pPatt = &pRcp->Patt;
	COceanBase* pOcean = p413App->pOceanOp;

	// Ensure the first probe number in MeSet is 8 otherwise it will not measure
	if (pRcp->MeParam[0].MPa[0].Prb != 8) {
		pDlg->Message("Probe 1 on MeSet 1 is not 8");
		return 0;
	}

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\SEM06\\%03d-0_1.CSV";

	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();

	while (pos) {
		CMPoint* pMP = (CMPoint*)pMPLst->GetNext(pos);
		if (!pMP)
			return 0;

		CData* pData = pMP->GetData(0, FALSE);
		if (!pData)
			return 0;

		if (pMP) {
			// measurement here
			CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
			if (p413App->pMtr)
				p413App->pMtr->GotoXY(pMP->Co.x, pMP->Co.y);
			CString str;
			str.Format("X: %.3f, Y: %.3f", pMP->Co.x, pMP->Co.y);
			pDlg->Message(str);
			str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
			pDlg->Message(str);
			//pOcean->SetThkRange2(p->Ta.fThkMin[0],p->Ta.fThkMax[0],p->Ta.Method,1,p->Ta.fRI[0],p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
			pOcean->SetParam(p->Ta.Method, 1, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
			int temp = pOcean->StartTF(p, dir, 49 * rand() / float(RAND_MAX) + 1);
			if ((temp == 512) || (temp == 2048)) {
				pOcean->ClearPolyThk(&pOcean->CurrSpectrum);
				pOcean->SetActivatePoly(&pOcean->CurrSpectrum, 0, p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.RI[0]);
				switch (pOcean->Result(&pOcean->CurrSpectrum, &pOcean->ResultSpectrum, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand)) {
				case -1:
					pDlg->Message("No good points");
					break;
				}
				//pDlg->m_Poly = pOcean->ResultSpectrum.Poly[0].fThk;
				pDlg->DisplaySpectrum(&pOcean->ResultSpectrum);
				pDlg->ShowResultUI();

				// hard coded setting first data thin film thickness
				pData->Set(0, p413App->pOceanOp->ResultSpectrum.Poly[0].fThk);

				pOcean->Wait();
			}
		}
	}

	// calculate statistic
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}

	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroSourceLight(FALSE);
		p413App->pMtr->SetSpectroIllumLight(FALSE);
	}

	pDlg->SaveData(pRcp);
	return 1;
}

// [7/1/2020]
void CSetupProp4Dlg::SaveData(CRecipe413* pRcp) {
	CFileDialog dlg(FALSE,
		"CSV",
		pRcp->RcpeName,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"CSV file|*.CSV|");

	CString path = "C:\\DATA\\";
	dlg.m_ofn.lpstrInitialDir = path;

	if (dlg.DoModal() == IDOK) {
		path = dlg.GetPathName();
		CFile file;
		if (file.Open(path, CFile::modeCreate | CFile::modeWrite)) {
			pRcp->ExportData(file, FALSE);
			file.Close();
		}
	}
}

void CSetupProp4Dlg::DisplaySpectrum(CSpec* pRSpec) {
	COceanBase* pOcean = p413App->pOceanOp;
	if (pOcean) {
		m_cSpec.RSpec.Dup(*pRSpec);
		CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
		m_cSpec.method = p->Ta.Method;
		m_cSpec.Refresh();
	}
}

void CSetupProp4Dlg::ShowResultUI() {
	CString result;
	COceanBase* pOcean = p413App->pOceanOp;
	if (!p413App->pOceanOp) {
		return;
	}
	result.Format("Thickness: %.4f um", /*m_Poly*/pOcean->ResultSpectrum.Poly[0].fThk);
	m_cResult.SetWindowText(result);

	// @@@ [11/20/2012 Yuen]
// 	result.Format("GOF: %.2f", pOcean->GOF);
// 	m_cResult2.SetWindowText(result);
//
// 	result.Format("%.4f  %.4f  %.4f  I:%.4f  P:%d",
// 		pOcean->PolyL, pOcean->PolyM, pOcean->PolyH, pOcean->PolyR/g->TFScalingFactor,pOcean->jPmax);
// 	m_cResult3.SetWindowText(result);

	pOcean->ShowFreqSpec(m_cList3);
}

void CSetupProp4Dlg::OnTakepicture() {
	m_cVideo.FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d"));
	m_cVideo.FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S"), int(rand() / float(RAND_MAX) * 100));
	m_cVideo.bStoreComposite = TRUE;
	while (m_cVideo.bStoreComposite) {
		Sleep(80);
	}
	DosUtil.RemoveExtension(m_cVideo.FileName);
	m_cSpec.SaveBitmapA(m_cVideo.FileName, 0);

	COceanBase* pOcean = p413App->pOceanOp;

	if (pOcean && pOcean->ResultSpectrum.pPt) {
		int i;
		DosUtil.RemoveExtension(m_cVideo.FileName);
		FILE* f = fopen(m_cVideo.FileName + ".TXT", "w");
		if (f) {
			CSpecPoint* pSp = pOcean->ResultSpectrum.pPt;
			for (i = 0; i < pOcean->ResultSpectrum.N; i++) {
				fprintf(f, "%.3f\t%.3f\n", 1 / pSp->X1, pSp->Y1);
				pSp++;
			}
			fclose(f);
		}
		f = fopen(m_cVideo.FileName + ".CSV", "w");
		if (f) {
			CFFTPoint* pSf = pOcean->ResultSpectrum.pV;
			for (i = 0; i < pOcean->ResultSpectrum.S; i++) {
				fprintf(f, "%d,%.f\n", i + 1, pSf->V);
				pSf++;
			}
			fclose(f);
		}
	}
}

void CSetupProp4Dlg::OnMicroscopelighton2() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(TRUE);
}

void CSetupProp4Dlg::OnMicroscopelightoff2() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(FALSE);
}

void CSetupProp4Dlg::OnSpectrometerlighton2() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroSourceLight(TRUE);
}

void CSetupProp4Dlg::OnSpectrometerlightoff2() {
	if (p413App->pMtr) p413App->pMtr->SetSpectroSourceLight(FALSE);
}

void CSetupProp4Dlg::OnSavespectrum() {
	CString str;
	str.Format("C:\\TEMP\\SP%s", CTime::GetCurrentTime().Format("%m%d%H%M%S"));
	p413App->SaveSpectrum(str);
}

void CSetupProp4Dlg::Message(CString msg) {
	Log(msg);

	short idx = m_cList.InsertItem(0, CTime::GetCurrentTime().Format("%y:%m:%d %H:%M:%S"));
	int cnt = 1;
	m_cList.SetItemText(idx, 1, msg);

	short len = m_cList.GetItemCount();
	if (len > MAX_LOGMSGITEM) {
		for (int i = 0; i < MAX_LOGMSGITEM / 5; i++) {
			m_cList.DeleteItem(len - i);
		}
	}
}

void CSetupProp4Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CSetupProp4Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CSetupProp4Dlg::SetupColumn() {
	int idx = 0;

	// Insert a column. This override is the most convenient.
	m_cList.InsertColumn(idx, _T("Time       "), LVCFMT_LEFT);
	m_cList.SetColumnWidth(idx++, 96);

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("Message");
	m_cList.InsertColumn(idx, &col);
	m_cList.SetColumnWidth(idx++, 360);
}

// void CSetupProp4Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 		pRcp->UpdateControl(m_cPoint);
// 	}
// }

void CSetupProp4Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

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
		if (pMP) {
			MatchUpdatePoint(nSel);
			m_cPoint.EnsureVisible(nSel, FALSE);
			m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		}
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
			GotoPointB(&m_cVideo, &pMP->Co);
		}
		break;
	case IDC_GOTOXYNOZ:
		if (pMP) {
			GotoPointB(&m_cVideo, &pMP->Co, FALSE);
		}
		break;
	case IDC_ALIGN:
		OnMatchpattern();
		break;
	case ID_GOTOALGNUPDATE:
		if (pMP) {
			GotoPointB(&m_cVideo, &pMP->Co);
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
				GotoPointB(&m_cVideo, &pMP1->Co);
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

void CSetupProp4Dlg::UpdatePoint(CCoor* p, BOOL bRefresh) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp4Dlg::MatchUpdatePoint(int nSel) {
	for (int i = nSel; i < m_cPoint.GetItemCount(); i++) {
		CMPoint* pMP = FindMPoint(i);
		if (!pMP) {
			continue;
		}

		CString str;
		CCoor* p = &pMP->Co;

		GotoPointB(&m_cVideo, p);

		p413App->AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo);
		m_cMatch.EnableWindow(FALSE);

		SDoPattrnMatching Param;

		p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
		Param.Set(&m_cVideo, NULL, &m_cList);
		p413App->SetupParam(pRcp, Param, nPatSSet);
		Param.bApplyOffset = FALSE;
		if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
			Param.bMoveCenter = TRUE;
		}
		else {
			Param.bMoveCenter = FALSE;
		}
		Param.Set(&m_cVideo, NULL, &m_cList);

		p413App->bLoadDIB = FALSE;
		short PatMatchRsl = p413App->DoPattrnMatching(Param);
		p413App->bLoadDIB = TRUE;
		switch (PatMatchRsl) {
		case  C413AppPatt::pmOK:
			break;
		default:	// All other cases consider failure [2/25/2013 Administrator]
			m_cMatch.EnableWindow(TRUE);
			str.Format("Point %d not found, hand locate", i + 1);
			if (AfxMessageBox(str, MB_YESNO) == IDYES) {
				str.Format("Update point %d?", i + 1);
				if (AfxMessageBox(str, MB_YESNO) == IDYES) {
					UpdatePoint(p, FALSE);
				}
			}
			else {
				if (AfxMessageBox("Quit", MB_YESNO) == IDYES) {
					return;
				}
			}
			continue;
			break;
		}
		str.Format("Update point %d?", i + 1);
		if (AfxMessageBox(str, MB_YESNO) == IDYES) {
			UpdatePoint(p, FALSE);
		}
		else {
			if (AfxMessageBox("Quit", MB_YESNO) == IDYES) {
				return;
			}
		}
		m_cMatch.EnableWindow(TRUE);
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp4Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();
	}
}

void CSetupProp4Dlg::OnLocations() {
	p413App->LocationProp(g->RcpSetup, m_hWnd);
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

void CSetupProp4Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		DoSelectMeasurment(pMP, nSel, m_cPoint, &m_cWaferMap);
	}

	*pResult = 0;
}

void CSetupProp4Dlg::OnMatch() {
	BlockUI();
	m_cMatch.EnableWindow(FALSE);

	SDoPattrnMatching Param;

	p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
	Param.Set(&m_cVideo, NULL, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}
	Param.Set(&m_cVideo, NULL, &m_cList);

	p413App->bLoadDIB = FALSE;
	short PatMatchRsl = p413App->DoPattrnMatching(Param);
	p413App->bLoadDIB = TRUE;
	switch (PatMatchRsl) {
	case C413AppPatt::pmOK:
		break;
	default:	// All other cases consider failure [2/25/2013 Administrator]
		AfxMessageBox("Not Found");
		m_cMatch.EnableWindow(TRUE);
		ReleaseUI();
		return;
		break;
	}
	m_cMatch.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp4Dlg::OnAutofocus1() {
	BlockUI();
	m_cAutoFocus.EnableWindow(FALSE);
	p413App->AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo);
	m_cAutoFocus.EnableWindow(TRUE);
	ReleaseUI();
}

long CSetupProp4Dlg::OnDeletepoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp4Dlg::OnEditpoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

long CSetupProp4Dlg::OnAddpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp4Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp4Dlg::OnRelocateToBack(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToBack(p);
	return 0;
}

long CSetupProp4Dlg::OnRelocateToTop(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToTop(p);
	return 0;
}

void CSetupProp4Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp4Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp4Dlg::OnMatchpattern() {
	SDoPattrnMatching Param;

	p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
	Param.Set(&m_cVideo, NULL, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}
	Param.Set(&m_cVideo, NULL, &m_cList);

	p413App->bLoadDIB = FALSE;
	short PatMatchRsl = p413App->DoPattrnMatching(Param);
	p413App->bLoadDIB = TRUE;
	switch (PatMatchRsl) {
	case C413AppPatt::pmOK:
		//		p413App->AdjustProbePosition(X,Y,pRcp,g->CurPaSet,NULL,NULL);
		break;
	default:
		AfxMessageBox("Not Found");
		return;
		break;
	}
}

void CSetupProp4Dlg::OnCoordinates() {
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
				p413App->pMtr->GetThinFilmZPosDev(cz);
				OffsetZ(cz);
				pRcp->UpdateControl(m_cPoint);
				m_cWaferMap.Redraw();
			}
		}
		break;
		case CEditCoordinateDlg::APPLYXYOFFSET:
			if (AfxMessageBox("Are you sure", MB_YESNO) == IDYES) {
				if (p413App->pMtr) {
					float x, y;
					p413App->pMtr->GetXYPos(&x, &y);
					OffsetXY(x, y);
				}
				pRcp->UpdateControl(m_cPoint);
				m_cWaferMap.Redraw();
			}
			break;
		}
	}
}

long CSetupProp4Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

long CSetupProp4Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp4Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		if (p413App->pOceanOp) {
			KillTimer(IDT_START);
		}
	}
}

void CSetupProp4Dlg::OnRefresh() {
	m_cWaferMap.pRcp = pRcp;
	int m_NumberOfMappedPoints = pRcp->nMappedPoints;
	if (p413App->pOceanOp) p413App->pOceanOp->nFFTSP = m_NumberOfMappedPoints / p413App->pOceanOp->nFFTFac;
	pRcp->UpdateControl(m_cPoint);
}

void CSetupProp4Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

void CSetupProp4Dlg::OnPatset1() {
	g->CurPaSet = 1;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset2() {
	g->CurPaSet = 2;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset3() {
	g->CurPaSet = 3;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset4() {
	g->CurPaSet = 4;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset5() {
	g->CurPaSet = 5;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset6() {
	g->CurPaSet = 6;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset7() {
	g->CurPaSet = 7;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset8() {
	g->CurPaSet = 8;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatset9() {
	g->CurPaSet = 9;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnMeasure2() {
	BlockUI();

	UpdateData(TRUE);
	COceanBase* pOcean = p413App->pOceanOp;
	if (!pOcean) {
		return;
	}

	CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
	CString str;
	str.Format("M:%d, RI:%.4f, Tk(%.2f,%.2f), Time:%d,  B:%d", p->Ta.Method, p->Ta.RI[0], p->Ta.ThkMin[0], p->Ta.ThkMax[0], p->Ta.IntegTime, p->Ta.Boxcar);
	Message(str);

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\SEM06\\%03d-0_1.CSV";
	pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
	m_cMeasure.EnableWindow(FALSE);
	if (p413App->pMtr) {
		p413App->pMtr->SetSpectroIllumLight(FALSE);
		if (!pOcean->bSimu) {
			Sleep(1000);
		}
	}
	m_cVideo.SetExposure(pRcp->TFTakeSpecExposure + g->BaseExpoTF);
	int temp = pOcean->StartTF(p, dir, (rand() / float(RAND_MAX)) * 49 + 1);
	BOOL bTaken = FALSE;
	pOcean->ClearPolyThk(&pOcean->CurrSpectrum);
	for (int i = 0; i < MAXTHKVLU; i++) {
		if (p->Ta.RI[i] > 0) {
			pOcean->SetActivatePoly(&pOcean->CurrSpectrum, i, p->Ta.ThkMin[i], p->Ta.ThkMax[i], p->Ta.RI[i]);
		}
	}
	pOcean->Result(&pOcean->CurrSpectrum, &pOcean->ResultSpectrum, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
	DisplaySpectrum(&pOcean->ResultSpectrum);
	ShowResultUI();
	m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
	if (p413App->pMtr) p413App->pMtr->SetSpectroIllumLight(TRUE);
	m_cMeasure.EnableWindow(TRUE);

	if (g->bTakePictureOnMeasure) {
		Sleep(800);
		OnTakepicture();
	}

	ReleaseUI();
}

void CSetupProp4Dlg::OnAutoexpose() {
	m_cAutoExpose.EnableWindow(FALSE);
	short Exposure;
	if (g->ActiveCamera == CCamBase::THINFCAM) {
		Exposure = pRcp->AutoExposeTF;
	}
	else {
		Exposure = pRcp->AutoExposeEcho;
	}
	m_cVideo.Level = Exposure;
	m_cVideo.AutoAdjust();
	m_cAutoExpose.EnableWindow(TRUE);
}

void CSetupProp4Dlg::OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult) {
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey) {
	case VK_DELETE:
	{
		nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		if (nSel > -1) {
			if (GetAsyncKeyState(VK_LCONTROL)) {
				//m_cPoint.SetItemState(nSel, ~LVIS_SELECTED, LVIS_SELECTED);
				DeletePoint(nSel);
				m_cPoint.DeleteItem(nSel);
				m_cWaferMap.Redraw();
				//Renumber();
				//m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED|LVIS_FOCUSED);
				//m_cPoint.SetSelectionMark(nSel);
			}
		}
	}
	break;
	}
	*pResult = 0;
}

void CSetupProp4Dlg::OnKillfocusFthkminSp4() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[CurNo] = m_FThkMin[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp4() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[CurNo] = m_FThkMax[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusRi1() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkminSp5() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[CurNo] = m_FThkMin[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp5() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[CurNo] = m_FThkMax[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusRi2() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkminSp6() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[2] = m_FThkMin[2];
	RecalculateSpectrum(2);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp6() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[2] = m_FThkMax[2];
	RecalculateSpectrum(2);
}

void CSetupProp4Dlg::OnKillfocusRi3() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkminSp7() {
	UpdateData(TRUE);
	CurNo = 3;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[CurNo] = m_FThkMin[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp7() {
	UpdateData(TRUE);
	CurNo = 3;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[CurNo] = m_FThkMax[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusRi4() {
	UpdateData(TRUE);
	CurNo = 3;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkminSp8() {
	UpdateData(TRUE);
	CurNo = 4;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[CurNo] = m_FThkMin[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp8() {
	UpdateData(TRUE);
	CurNo = 4;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[CurNo] = m_FThkMax[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusRi5() {
	UpdateData(TRUE);
	CurNo = 4;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkminSp9() {
	UpdateData(TRUE);
	CurNo = 5;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[CurNo] = m_FThkMin[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusFthkmaxSp9() {
	UpdateData(TRUE);
	CurNo = 5;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[CurNo] = m_FThkMax[CurNo];
	RecalculateSpectrum(CurNo);
}

void CSetupProp4Dlg::OnKillfocusRi6() {
	UpdateData(TRUE);
	CurNo = 5;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[CurNo] = m_RI[CurNo];
	RecalculateSpectrum(CurNo);
}

float CSetupProp4Dlg::RecalculateSpectrum(short which) {
	return 0;

	// 	UpdateData(TRUE);
	//
	// 	COceanBase *pOcean = p413App->pOceanOp;
	// 	if (!pOcean)
	// 	{
	// 		return 0;
	// 	}
	// 	CSpec *pIn;
	// 	CSpec *pOut;
	// 	CMeParam *p;
	// 	p = &pRcp->MeParam[g->CurMeSet-1];
	// 	pIn = &m_cSpec;
	// 	pOut = &m_cSpec;
	// 	p->Ta.RI[which] = m_RI[which];
	// 	p->Ta.ThkMax[which] = m_FThkMax[which];
	// 	p->Ta.ThkMin[which] = m_FThkMin[which];
	//
	// 	CurNo = which;
	//
	// 	pOcean->SetParam(p->Ta.Method,1.0f,p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
	// 	pOcean->ClearPolyThk(pIn);
	// 	pOcean->SetActivatePoly(pIn,0,p->Ta.ThkMin[which],p->Ta.ThkMax[which],p->Ta.RI[which]);
	// 	pOcean->Result(pIn,pOut,(CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet-1],pRcp->nCycle,pRcp->nMappedPoints,pRcp->NSmooth,pRcp->nExpand);
	// 	DisplaySpectrum(which,SpecName,pOut);
	// 	return pOut->Poly[0].fThk;
}

void CSetupProp4Dlg::OnUp() {
	nSel--;
	if (nSel < 0) {
		nSel = 0;
		return;
	}
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp4Dlg::OnDn() {
	if (nSel >= m_cPoint.GetItemCount()) {
		return;
	}
	nSel++;
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp4Dlg::OnHere() {
	if ((nSel < 1) || (nSel >= m_cPoint.GetItemCount())) {
		return;
	}
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		GotoPointB(&m_cVideo, &pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
		UpdateSelectionItem(pMP);
	}
}

void CSetupProp4Dlg::OnUpdatept() {
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		UpdatePoint(&pMP->Co);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CSetupProp4Dlg::OnAddzoffset() {
	CMPoint* pMP = FindMPoint(nSel);
	if (pMP) {
		Addzposition(pMP);
		m_cPoint.EnsureVisible(nSel, FALSE);
		m_cPoint.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CSetupProp4Dlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		UpdateSelectionItem(pMP);
	}

	*pResult = 0;
}

void CSetupProp4Dlg::UpdateSelectionItem(CMPoint* pMP) {
	CString str;
	str.Format("%d", pMP->Co.n);
	m_cSelectedPt.SetWindowText(str);
}

void CSetupProp4Dlg::RecipeToLocal() {
	short MeS = g->CurMeSet - 1;

	for (int i = 0; i < MAXTHKVLU; i++) {
		m_FThkMax[i] = pRcp->MeParam[MeS].Ta.ThkMax[i];
		m_FThkMin[i] = pRcp->MeParam[MeS].Ta.ThkMin[i];
		m_RI[i] = pRcp->MeParam[MeS].Ta.RI[i];
	}
	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_PATSET1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET2))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET5))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET8))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSET9))->SetCheck(FALSE);
	switch (g->CurPaSet) {
	case 1:
		((CButton*)GetDlgItem(IDC_PATSET1))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_PATSET2))->SetCheck(TRUE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATSET3))->SetCheck(TRUE);
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_PATSET4))->SetCheck(TRUE);
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_PATSET5))->SetCheck(TRUE);
		break;
	case 6:
		((CButton*)GetDlgItem(IDC_PATSET6))->SetCheck(TRUE);
		break;
	case 7:
		((CButton*)GetDlgItem(IDC_PATSET7))->SetCheck(TRUE);
		break;
	case 8:
		((CButton*)GetDlgItem(IDC_PATSET8))->SetCheck(TRUE);
		break;
	case 9:
		((CButton*)GetDlgItem(IDC_PATSET9))->SetCheck(TRUE);
		break;
	}

	((CButton*)GetDlgItem(IDC_PATSSETA))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETB))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_PATSSETD))->SetCheck(FALSE);
	switch (nPatSSet) {
	case 0:
		((CButton*)GetDlgItem(IDC_PATSSETA))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_PATSSETB))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_PATSSETC))->SetCheck(TRUE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_PATSSETD))->SetCheck(TRUE);
		break;
	}

	((CButton*)GetDlgItem(IDC_SET1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET2))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET4))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET5))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET8))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SET9))->SetCheck(FALSE);
	switch (g->CurMeSet) {
	case 1:
		((CButton*)GetDlgItem(IDC_SET1))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_SET2))->SetCheck(TRUE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_SET3))->SetCheck(TRUE);
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_SET4))->SetCheck(TRUE);
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_SET5))->SetCheck(TRUE);
		break;
	case 6:
		((CButton*)GetDlgItem(IDC_SET6))->SetCheck(TRUE);
		break;
	case 7:
		((CButton*)GetDlgItem(IDC_SET7))->SetCheck(TRUE);
		break;
	case 8:
		((CButton*)GetDlgItem(IDC_SET8))->SetCheck(TRUE);
		break;
	case 9:
		((CButton*)GetDlgItem(IDC_SET9))->SetCheck(TRUE);
		break;
	}

	m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][nPatSSet];
	m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][nPatSSet];

	m_cWaferMap.Redraw();
}

void CSetupProp4Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	short MeS = g->CurMeSet - 1;

	for (int i = 0; i < MAXTHKVLU; i++) {
		pRcp->MeParam[MeS].Ta.ThkMax[i] = m_FThkMax[i];
		pRcp->MeParam[MeS].Ta.ThkMin[i] = m_FThkMin[i];
		pRcp->MeParam[MeS].Ta.RI[i] = m_RI[i];
	}
}

void CSetupProp4Dlg::OnRunrecipe() {
	static CWinThread* pThread = NULL;
	pThread = AfxBeginThread(CSetupProp4Dlg::RunRecipeProcTF, this);
}

void CSetupProp4Dlg::OnPatsseta() {
	nPatSSet = 0;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatssetb() {
	nPatSSet = 1;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatssetc() {
	nPatSSet = 2;
	RecipeToLocal();
}

void CSetupProp4Dlg::OnPatssetd() {
	nPatSSet = 3;
	RecipeToLocal();
}