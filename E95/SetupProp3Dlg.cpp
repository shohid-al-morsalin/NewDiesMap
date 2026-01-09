// SetupProp3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include <math.h>
#include "XTabCtrl.h"
#include "SetupProp3Dlg.h"
#include "InfoPanelMain.h"
#include "..\SYS\CUMMsg.h"

#include "MotionControl.h"

#include "Recipe413.h"
#include "..\413\EchoScope\EchoProbe.h"

#include "413App.h"

#include "InputCalibWaferThickness.h"

#include "..\413\EchoScope\ScanLine.h"
#include "..\413\EchoScope\ScanPoint.h"
#include "..\413\OceanOp\OceanBase.h"

#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "MPoint.h"

#include "ChuckMask.h"
#include "SiCamSettingDlg.h"

#include "Data.h"

#include "ProbeTest.h"

#include "EditBinDlg.h"
#include "PeakParam.h"

#include "RunRecipe.h"
#include "InfoPanelSetup.h"

#include "..\413\DMCMotorBase.h"
#include "..\413\MotorS2.h"

#include "ProbeDistParam.h"

#include "TitlePanel.h"
#include "..\SRC\LogInDataNew.h"

#include "OptimizeEchoDlg.h"

#include "ViewData.h"

#include "CWLBase.h"

// [Mahedi Kamal - 03202024
#include "RoughProbe.h"
#include "RoughProbeBase.h"
#include "RouPar.h"
// ]

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp3Dlg dialog

#define IDT_FOCUS		9999
#define IDT_START		9998
#define IDT_PROBETEST	9997

#define IDT_REFRESH		9996	// 12022022 ZHIMING

// Duplicated px & py, need to consolidate [8/15/2010 XPMUser]
extern float px[];
extern float py[];

CSetupProp3Dlg::CSetupProp3Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp3Dlg::IDD, pParent), CSetupPropComm(pParent) {
	//{{AFX_DATA_INIT(CSetupProp3Dlg)
	m_bPlot = TRUE;
	m_nScale = 0;
	m_ri = 3.5f;	// This is a fake number [10/12/2011 TelSys]
	m_SimuPk = 0;
	m_NAve = 8;
	//}}AFX_DATA_INIT

	for (int i = 0; i < MaXPA; i++) {
		Thk[i] = _T("");
		Name[i] = _T("");
		Ave[i] = 0;
		Stdev[i] = 0;
	}

	YMIN = -2048;
	YMAX = 2048;
	XMIN = 0;
	XMAX = 100;

	bDPCalib = FALSE;
	bDPCalibTest = FALSE;
	bSPCalib = FALSE;
	bSPCalibTest = FALSE;
	bSaveChart = FALSE;
	bDispHisto = FALSE;
	iHisto = 0;
	bReal = TRUE;
	m_cEchoPeaks.bReal = bReal;

	m_Parent = NULL;
	nDisplay = ECHO1;

	bAllcBMP = FALSE;
	WdBk = 0;
	HtBk = 0;

	m_bgColor = RGB(255, 255, 255);
	m_lnColor1 = RGB(0, 128, 255);
	m_lnColor2 = RGB(255, 128, 0);
	m_grColor = RGB(64, 64, 64);
	m_txtColor = RGB(16, 16, 16);

	pDump = NULL;
	pDryRunThread = NULL;

	SPCalibThick = 0;

	nThknBuf = 0;
	for (int j = 0; j < MaXPA; j++) {
		for (int i = 0; i < MAXAVEBUFF; i++) {
			fThknBuf[j][i] = 0;
		}
	}

	Show = 0;
	DumpCounter = DosUtil.GetDumpCounter();

	m_ThresholdFactor[0] = m_ThresholdFactor[1] = 10.0f;
	m_Level[0] = m_Level[1] = 60;
	m_PeakWidth[0] = m_PeakWidth[1] = 160;

	bSetFocus = bDoFocus = bEchoStarted = bClearFocus = FALSE;
	m_cVideo.Name = "Setup3";
}

CSetupProp3Dlg::~CSetupProp3Dlg() {
	if (pDump) {
		fprintf(pDump, "\n%s", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
		fclose(pDump);
		pDump = NULL;
	}
	if (pDmpPeakT) {
		fprintf(pDmpPeakT, "\n%s", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
		fclose(pDmpPeakT);
		pDmpPeakT = NULL;
	}
	if (pDmpPeakB) {
		fprintf(pDmpPeakB, "\n%s", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
		fclose(pDmpPeakB);
		pDmpPeakB = NULL;
	}
}

void CSetupProp3Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp3Dlg)
	DDX_Control(pDX, IDC_SAVEENCODERFACTOR, m_SaveCalib);
	DDX_Control(pDX, IDC_OVERLAYINFO, m_cOverlayInfo);
	DDX_Control(pDX, IDC_SLEEP, m_cSleep);
	DDX_Control(pDX, IDC_CLEARPC2, m_cClearPC2);
	DDX_Control(pDX, IDC_CLEARPC, m_cClearPC1);
	DDX_Control(pDX, IDC_STOPRUN, m_cStop);
	DDX_Control(pDX, IDC_CLEARAF, m_cClearSF);
	DDX_Control(pDX, IDC_GENMESSAGE, m_cGenMessage);
	DDX_Control(pDX, IDC_SAVEVIDEO2, m_cSaveVideo);
	DDX_Control(pDX, IDC_DIANOSTIC, m_cDianostic);
	DDX_Control(pDX, IDC_SIMUPK, m_cSimuPk);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_SETFOCUSPOINT, m_cSetFocusPoint);
	DDX_Control(pDX, IDC_DOFOCUS, m_cDoFocus);
	DDX_Control(pDX, IDC_FOCUSPOINT, m_cSetFocus);
	DDX_Control(pDX, IDC_DPCALIB, m_cDPCalib);
	DDX_Control(pDX, IDC_SPCALIB, m_cSPCalib);
	DDX_Control(pDX, IDC_TAKEPICTURE2, m_cTakePicture);
	DDX_Control(pDX, IDC_CAMERASETTING, m_cCameraSetting);
	DDX_Control(pDX, IDC_HISTOGRAM, m_cHisto);
	DDX_Control(pDX, IDC_TIME, m_cTime);
	DDX_Control(pDX, IDC_DRYRUN, m_cRun);
	DDX_Control(pDX, IDC_MESET, m_cMeSet);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_ECHOPEAKS, m_cEchoPeaks);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_COUNTER, m_cCounter);
	DDX_Control(pDX, IDC_STARTECHO, m_cStartEcho);
	DDX_Check(pDX, IDC_PLOT, m_bPlot);
	DDX_Text(pDX, IDC_EDSCALE, m_nScale);
	DDV_MinMaxInt(pDX, m_nScale, 0, 10);
	DDX_Text(pDX, IDC_RI, m_ri);
	DDX_Text(pDX, IDC_SIMUPK, m_SimuPk);
	DDX_Text(pDX, IDC_NAVE, m_NAve);
	//}}AFX_DATA_MAP
// 	DDX_Text(pDX, IDC_SRIB, p413App->SRIB);
// 	DDX_Text(pDX, IDC_SRIT, p413App->SRIT);

	DDX_Text(pDX, IDC_AIRBOT, p413App->cfBot);
	DDX_Text(pDX, IDC_AIRTOP, p413App->cfTop);

	DDX_Text(pDX, IDC_THKN1, Thk[0]);
	DDX_Text(pDX, IDC_THKN2, Thk[1]);
	DDX_Text(pDX, IDC_THKN3, Thk[2]);
	DDX_Text(pDX, IDC_THKN4, Thk[3]);
	DDX_Text(pDX, IDC_THKN5, Thk[4]);
	DDX_Text(pDX, IDC_THKN6, Thk[5]);
	DDX_Text(pDX, IDC_THKN7, Thk[6]);
	DDX_Text(pDX, IDC_THKN8, Thk[7]);
	DDX_Text(pDX, IDC_THKN9, Thk[8]);
	DDX_Text(pDX, IDC_THKN10, Thk[9]);
	DDX_Text(pDX, IDC_THKN11, Thk[10]);
	DDX_Text(pDX, IDC_THKN12, Thk[11]);
	DDX_Text(pDX, IDC_THKN13, Thk[12]);
	DDX_Text(pDX, IDC_THKN14, Thk[13]);
	DDX_Text(pDX, IDC_THKN15, Thk[14]);
	DDX_Text(pDX, IDC_THKN16, Thk[15]);

	DDX_Text(pDX, IDC_NAME1, Name[0]);
	DDX_Text(pDX, IDC_NAME2, Name[1]);
	DDX_Text(pDX, IDC_NAME3, Name[2]);
	DDX_Text(pDX, IDC_NAME4, Name[3]);
	DDX_Text(pDX, IDC_NAME5, Name[4]);
	DDX_Text(pDX, IDC_NAME6, Name[5]);
	DDX_Text(pDX, IDC_NAME7, Name[6]);
	DDX_Text(pDX, IDC_NAME8, Name[7]);
	DDX_Text(pDX, IDC_NAME9, Name[8]);
	DDX_Text(pDX, IDC_NAME10, Name[9]);
	DDX_Text(pDX, IDC_NAME11, Name[10]);
	DDX_Text(pDX, IDC_NAME12, Name[11]);
	DDX_Text(pDX, IDC_NAME13, Name[12]);
	DDX_Text(pDX, IDC_NAME14, Name[13]);
	DDX_Text(pDX, IDC_NAME15, Name[14]);
	DDX_Text(pDX, IDC_NAME16, Name[15]);

	DDX_Text(pDX, IDC_THKNAVE1, Ave[0]);
	DDX_Text(pDX, IDC_THKNAVE2, Ave[1]);
	DDX_Text(pDX, IDC_THKNAVE3, Ave[2]);
	DDX_Text(pDX, IDC_THKNAVE4, Ave[3]);
	DDX_Text(pDX, IDC_THKNAVE5, Ave[4]);
	DDX_Text(pDX, IDC_THKNAVE6, Ave[5]);
	DDX_Text(pDX, IDC_THKNAVE7, Ave[6]);
	DDX_Text(pDX, IDC_THKNAVE8, Ave[7]);
	DDX_Text(pDX, IDC_THKNAVE9, Ave[8]);
	DDX_Text(pDX, IDC_THKNAVE10, Ave[9]);
	DDX_Text(pDX, IDC_THKNAVE11, Ave[10]);
	DDX_Text(pDX, IDC_THKNAVE12, Ave[11]);
	DDX_Text(pDX, IDC_THKNAVE13, Ave[12]);
	DDX_Text(pDX, IDC_THKNAVE14, Ave[13]);
	DDX_Text(pDX, IDC_THKNAVE15, Ave[14]);
	DDX_Text(pDX, IDC_THKNAVE16, Ave[15]);

	DDX_Text(pDX, IDC_THKNSTD1, Stdev[0]);
	DDX_Text(pDX, IDC_THKNSTD2, Stdev[1]);
	DDX_Text(pDX, IDC_THKNSTD3, Stdev[2]);
	DDX_Text(pDX, IDC_THKNSTD4, Stdev[3]);
	DDX_Text(pDX, IDC_THKNSTD5, Stdev[4]);
	DDX_Text(pDX, IDC_THKNSTD6, Stdev[5]);
	DDX_Text(pDX, IDC_THKNSTD7, Stdev[6]);
	DDX_Text(pDX, IDC_THKNSTD8, Stdev[7]);
	DDX_Text(pDX, IDC_THKNSTD9, Stdev[8]);
	DDX_Text(pDX, IDC_THKNSTD10, Stdev[9]);
	DDX_Text(pDX, IDC_THKNSTD11, Stdev[10]);
	DDX_Text(pDX, IDC_THKNSTD12, Stdev[11]);
	DDX_Text(pDX, IDC_THKNSTD13, Stdev[12]);
	DDX_Text(pDX, IDC_THKNSTD14, Stdev[13]);
	DDX_Text(pDX, IDC_THKNSTD15, Stdev[14]);
	DDX_Text(pDX, IDC_THKNSTD16, Stdev[15]);

	DDX_Text(pDX, IDC_THRESHOLDFACTOR3, m_ThresholdFactor[0]);
	DDX_Text(pDX, IDC_THRESHOLDFACTOR2, m_ThresholdFactor[1]);
	DDX_Text(pDX, IDC_RCP_LEVEL1, m_Level[0]);
	DDX_Text(pDX, IDC_RCP_LEVEL2, m_Level[1]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH1, m_PeakWidth[0]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH2, m_PeakWidth[1]);
}

BEGIN_MESSAGE_MAP(CSetupProp3Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp3Dlg)
	ON_BN_CLICKED(IDC_MOTIONCONTROL, OnMotioncontrol)
	ON_BN_CLICKED(IDC_NEWRECIPE, OnNewrecipe)
	ON_BN_CLICKED(IDC_STARTECHO, OnStartecho)
	ON_BN_CLICKED(IDC_TOPPROBE, OnTopprobe)
	ON_BN_CLICKED(IDC_BOTTOMPROBE, OnBottomprobe)
	ON_BN_CLICKED(IDC_EDPOSITION, OnEdposition)
	ON_BN_CLICKED(IDC_A, OnA)
	ON_BN_CLICKED(IDC_B, OnB)
	ON_COMMAND(IDC_DRAWCHART, OnDrawchart)
	ON_BN_CLICKED(IDC_DPCALIB, OnDpcalib)
	ON_BN_CLICKED(IDC_SPCALIB, OnSpcalib)
	ON_BN_CLICKED(IDC_PLOT, OnPlot)
	ON_BN_CLICKED(IDC_REALUNIT, OnRealunit)
	ON_BN_CLICKED(IDC_DRYRUN, OnDryrun)
	ON_BN_CLICKED(IDC_STOPRUN, OnStoprun)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_MARK, OnMark)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MAKEBASELINE, OnMakebaseline)
	ON_EN_KILLFOCUS(IDC_AIRBOT, OnKillfocusAirbot)
	ON_EN_KILLFOCUS(IDC_AIRTOP, OnKillfocusAirtop)
	ON_BN_CLICKED(IDC_SAVEENCODERFACTOR, OnSaveencoderfactor)
	ON_BN_CLICKED(IDC_TAKEPICTURE2, OnTakepicture2)
	ON_EN_KILLFOCUS(IDC_NAVE, OnKillfocusNave)
	ON_EN_KILLFOCUS(IDC_EDSCALE, OnKillfocusEdscale)
	ON_EN_KILLFOCUS(IDC_RI, OnKillfocusRi)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_SET10, OnSet10)
	ON_BN_CLICKED(IDC_STARTHISTO, OnStarthisto)
	ON_BN_CLICKED(IDC_STOPHISTO, OnStophisto)
	ON_BN_CLICKED(IDC_CONTHISTO, OnConthisto)
	ON_BN_CLICKED(IDC_LOADHISTO, OnLoadhisto)
	ON_EN_KILLFOCUS(IDC_THRESHOLDFACTOR3, OnKillfocusThresholdfactor3)
	ON_EN_KILLFOCUS(IDC_THRESHOLDFACTOR2, OnKillfocusThresholdfactor2)
	ON_EN_KILLFOCUS(IDC_RCP_LEVEL1, OnKillfocusRcpLevel1)
	ON_EN_KILLFOCUS(IDC_RCP_LEVEL2, OnKillfocusRcpLevel2)
	ON_EN_KILLFOCUS(IDC_RCP_PEAKWIDTH1, OnKillfocusRcpPeakwidth1)
	ON_EN_KILLFOCUS(IDC_RCP_PEAKWIDTH2, OnKillfocusRcpPeakwidth2)
	ON_BN_CLICKED(IDC_SETFOCUSPOINT, OnSetfocuspoint)
	ON_BN_CLICKED(IDC_DOFOCUS, OnDofocus)
	ON_WM_TIMER()
	ON_EN_KILLFOCUS(IDC_SIMUPK, OnKillfocusSimupk)
	ON_BN_CLICKED(IDC_LOCATIONS2, OnLocations2)
	ON_COMMAND(ID_REFRESHUI, OnRefreshui)
	ON_BN_CLICKED(IDC_DIANOSTIC, OnDianostic)
	ON_BN_CLICKED(IDC_SAVEVIDEO2, OnSavevideo2)
	ON_BN_CLICKED(IDC_CLEARBASELINE, OnClearbaseline)
	ON_BN_CLICKED(IDC_CLEARAF, OnClearaf)
	ON_BN_CLICKED(IDC_CLEARPC, OnClearpc)
	ON_BN_CLICKED(IDC_CLEARPC2, OnClearpc2)
	ON_BN_CLICKED(IDC_SLEEP, OnSleep)
	ON_BN_CLICKED(IDC_CLEARDATA, OnCleardata)
	ON_BN_CLICKED(IDC_VIEWDATA, OnViewdata)
	ON_BN_CLICKED(IDC_OVERLAYINFO, OnOverlayinfo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_RELOCATETOTOP, OnRelocateToTop)
	ON_MESSAGE(ID_RELOCATETOBACK, OnRelocateToBack)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(IDC_ADDPOINT, OnAddpoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_COMMAND(UM_BUTTON_RDCLICK, OnCamerasetting)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp3Dlg message handlers

long CSetupProp3Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		pRcp = &g->RcpSetup;
		p413App->pEcho->SetRecipe(pRcp);
		p413App->pEcho->bSignal = TRUE;
		m_cWaferMap.pRcp = pRcp;

		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.SetRecipe(pRcp);
			m_cVideo.SetAve(&Ave[0]);
			m_cVideo.pWnd = this;
			m_cVideo.RSpecClear();
			m_cVideo.bShowCrossHair = FALSE;
			m_cVideo.fTGtGrnX = pRcp->fTgtGRnX[g->CurPaSet - 1][0];
			m_cVideo.fTGtGrnY = pRcp->fTgtGRnY[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluX = pRcp->fTgtBLuX[g->CurPaSet - 1][0];
			m_cVideo.fTGtBluY = pRcp->fTgtBLuY[g->CurPaSet - 1][0];
		}
		RecipeToLocal();
		bTabSelected = TRUE;
	}
	return 0;
}

long CSetupProp3Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		UpdateData(TRUE);
		if (!m_cVideo.IsReserved()) {
			p413App->StopCamera(&m_cVideo, NULL);
			m_cVideo.pWnd = NULL;
		}
		p413App->pEcho->bSignal = FALSE;
		p413App->pEcho->pGenMessage = NULL;

		LocalToRecipe();
	}
	bTabSelected = FALSE;
	return 0;
}

void CSetupProp3Dlg::LocalToRecipe() {
	if (!g) return;

	UpdateData(TRUE);

	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
	p413App->pEcho->SimuPkNum = m_SimuPk;
	pRcp->MeParam[MeS].NAve = m_NAve;
}

void CSetupProp3Dlg::PostNcDestroy() {
	StopMotor();
	Deinitialize();
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp3Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_STARTECHO, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL, TOP_RIGHT);
	AddAnchor(IDC_MAKEBASELINE, TOP_RIGHT);
	AddAnchor(IDC_CLEARBASELINE, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_TEST, TOP_RIGHT);
	AddAnchor(IDC_NEWRECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	// 	AddAnchor(IDC_LOADRECIPE2, TOP_RIGHT);
	AddAnchor(IDC_DRYRUN, TOP_RIGHT);
	AddAnchor(IDC_STOPRUN, TOP_RIGHT);
	AddAnchor(IDC_WAFERMAP, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_TAKEPICTURE2, TOP_RIGHT);
	AddAnchor(IDC_LOCATIONS2, TOP_RIGHT);

	AddAnchor(IDC_ECHOPEAKS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_ECHOSIGNAL, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_REALUNIT, TOP_RIGHT);
	AddAnchor(IDC_SCALE, TOP_RIGHT);
	AddAnchor(IDC_EDSCALE, TOP_RIGHT);
	AddAnchor(IDC_PLOT, TOP_RIGHT);
	AddAnchor(IDC_SIMUPK, TOP_RIGHT);
	AddAnchor(IDC_CLEARDATA, TOP_RIGHT);
	AddAnchor(IDC_VIEWDATA, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cSleep.ShowWindow(FALSE);
	if (Var.UIMode > 1) {
		m_cClearPC1.ShowWindow(TRUE);
		m_cClearPC2.ShowWindow(TRUE);
		if (Var.UIMode > 2) {
			m_cSleep.ShowWindow(TRUE);
		}
	}
	if (g->Confg.bEchoMicroscopeMod) {
		m_cCameraSetting.ShowWindow(SW_SHOW);
		if (g->Confg.bEchoTopMotorMod) {
			m_cSetFocusPoint.EnableWindow(TRUE);
			m_cDoFocus.EnableWindow(TRUE);
			m_cClearSF.EnableWindow(TRUE);
		}
	}

	((CButton*)GetDlgItem(IDC_TOPPROBE))->SetCheck(TRUE);

	m_cVideo.SetRecipe(pRcp);
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;

	UpdatePosition();

	ArrangeLayout();

	//m_cVideo.ResizeWindow();

	SetTimer(IDT_START, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp3Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp3Dlg::OnNewrecipe() {
	RecipeIO();
}

void CSetupProp3Dlg::RecipeToLocal() {
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		m_ThresholdFactor[i] = pRcp->MeParam[MeS].PSG[i].Thres;
		m_Level[i] = pRcp->MeParam[MeS].PSG[i].Level;
		m_PeakWidth[i] = pRcp->MeParam[MeS].PSG[i].Width;
	}
	m_SimuPk = p413App->pEcho->SimuPkNum;
	m_NAve = pRcp->MeParam[MeS].NAve;

	m_cDianostic.SetCheck(pRcp->bEchoPeakMonitor);
	m_cSaveVideo.SetCheck(pRcp->bSaveVideo);
	m_cDianostic.SetCheck(pRcp->bEchoPeakMonitor);
	m_cOverlayInfo.SetCheck(m_cVideo.bDisplayOverlay);

	CString str;
	str.Format("%.2f um", pRcp->EchoFocus);
	m_cSetFocus.SetWindowText(str);

	RefreshMeasurement();

	m_cWaferMap.Redraw();
	UpdateData(FALSE);
}

void CSetupProp3Dlg::UpdateThkn(int i, float& fthkn) {
	if (i >= MaXPA) {
		ASSERT(0);
		return;
	}
	int IDC1 = IDC_THKN1;
	int IDC2 = IDC_THKNAVE1;
	int IDC3 = IDC_THKNSTD1;
	switch (i) {
	case 0:
		IDC1 = IDC_THKN1;
		IDC2 = IDC_THKNAVE1;
		IDC3 = IDC_THKNSTD1;
		break;
	case 1:
		IDC1 = IDC_THKN2;
		IDC2 = IDC_THKNAVE2;
		IDC3 = IDC_THKNSTD2;
		break;
	case 2:
		IDC1 = IDC_THKN3;
		IDC2 = IDC_THKNAVE3;
		IDC3 = IDC_THKNSTD3;
		break;
	case 3:
		IDC1 = IDC_THKN4;
		IDC2 = IDC_THKNAVE4;
		IDC3 = IDC_THKNSTD4;
		break;
	case 4:
		IDC1 = IDC_THKN5;
		IDC2 = IDC_THKNAVE5;
		IDC3 = IDC_THKNSTD5;
		break;
	case 5:
		IDC1 = IDC_THKN6;
		IDC2 = IDC_THKNAVE6;
		IDC3 = IDC_THKNSTD6;
		break;
	case 6:
		IDC1 = IDC_THKN7;
		IDC2 = IDC_THKNAVE7;
		IDC3 = IDC_THKNSTD7;
		break;
	case 7:
		IDC1 = IDC_THKN8;
		IDC2 = IDC_THKNAVE8;
		IDC3 = IDC_THKNSTD8;
		break;
	case 8:
		IDC1 = IDC_THKN9;
		IDC2 = IDC_THKNAVE9;
		IDC3 = IDC_THKNSTD9;
		break;
	case 9:
		IDC1 = IDC_THKN10;
		IDC2 = IDC_THKNAVE10;
		IDC3 = IDC_THKNSTD10;
		break;
	case 10:
		IDC1 = IDC_THKN11;
		IDC2 = IDC_THKNAVE11;
		IDC3 = IDC_THKNSTD11;
		break;
	case 11:
		IDC1 = IDC_THKN12;
		IDC2 = IDC_THKNAVE12;
		IDC3 = IDC_THKNSTD12;
		break;
	case 12:
		IDC1 = IDC_THKN13;
		IDC2 = IDC_THKNAVE13;
		IDC3 = IDC_THKNSTD13;
		break;
	case 13:
		IDC1 = IDC_THKN14;
		IDC2 = IDC_THKNAVE14;
		IDC3 = IDC_THKNSTD14;
		break;
	case 14:
		IDC1 = IDC_THKN15;
		IDC2 = IDC_THKNAVE15;
		IDC3 = IDC_THKNSTD15;
		break;
	case 15:
		IDC1 = IDC_THKN16;
		IDC2 = IDC_THKNAVE16;
		IDC3 = IDC_THKNSTD16;
		break;
	}
	CWnd* pText;
	CString strText;
	strText.Format("%.2f", fthkn);
	pText = GetDlgItem(IDC1);
	pText->SetWindowText(strText);
	strText.Format("%.2f", Ave[i]);
	pText = GetDlgItem(IDC2);
	pText->SetWindowText(strText);
	strText.Format("%.2f", Stdev[i]);
	pText = GetDlgItem(IDC3);
	pText->SetWindowText(strText);

	static int cnt = 0;
	if (bDispHisto && iHisto == i) {
		m_cHisto.Bin(fthkn);
		cnt++;
		if (cnt > 1) {
			cnt = 0;
			m_cHisto.Invalidate();
		}
	}
}

void CSetupProp3Dlg::RefreshMeasurement() {
	CString str;

	str.Format("Measuremnt Set %d", g->CurMeSet);
	m_cMeSet.SetWindowText(str);

	for (int i = 0; i < MaXPA; i++) {
		MParam* pa = &pRcp->MeParam[g->CurMeSet - 1].MPa[i];
		Name[i] = pa->Name;
		Thk[i].Format("%.2f", pa->fThkn);
		pa++;
	}
}

// void CSetupProp3Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp3Dlg::PostDrawChartMessage(void* pObject) {
	if (hWnd) {
		::PostMessage(hWnd, WM_COMMAND, IDC_DRAWCHART, NULL);
	}
}

static short SP3Cnt = 1;

void CSetupProp3Dlg::PostErrorMessage(CString MsgStr) {
	if (SP3Cnt > 32000) {
		SP3Cnt = 1;
	}
	CString str;
	str.Format("%d %s", SP3Cnt++, MsgStr);
	m_cGenMessage.SetWindowText(str);
}

void CSetupProp3Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CSetupProp3Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CSetupProp3Dlg::StartEcho() {
	StartMotor();
	if (!StartEchoThread()) {
		StopMotor();
		return FALSE;
	}
	bEchoStarted = TRUE;
	return TRUE;
}

BOOL CSetupProp3Dlg::StopEcho() {
	if (!StopEchoThread()) {
		return FALSE;
	}
	if (bEchoStarted) {
		StopMotor();
		bEchoStarted = FALSE;
		m_cStartEcho.SetWindowText("Start Echo Probe");
	}
	// 	m_cVideo.pWnd = NULL;
	m_cVideo.RSpecClear();
	p413App->pEcho->pGenMessage = NULL;
	return TRUE;
}

void CSetupProp3Dlg::OnTopprobe() {
	nDisplay = ECHO1;
}

void CSetupProp3Dlg::OnBottomprobe() {
	nDisplay = ECHO2;
}

void CSetupProp3Dlg::OnEdposition() {
	nDisplay = POSN;
}

void CSetupProp3Dlg::OnA() {
	nDisplay = CHNA;
}

void CSetupProp3Dlg::OnB() {
	nDisplay = CHNB;
}

void CSetupProp3Dlg::OnDpcalib() {
	BlockUI();
	m_cSPCalib.EnableWindow(FALSE);
	m_cDPCalib.EnableWindow(FALSE);
	m_SaveCalib.EnableWindow(FALSE);	
	if (!bDPCalib) {
		if (p413App->pMtr) {
			p413App->pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
			//p413App->pMtr->MoveEchoBottomZ(pRcp->fEchoBottomZ);
		}
		Dpposition();
		CInputCalibWaferThickness dlg;
		dlg.m_CalibThickness = g->DPGageThickness;
		if (dlg.DoModal() == IDOK) {
			g->DPGageThickness = dlg.m_CalibThickness;
			g->SaveMeasurementOption();
			bDPCalib = TRUE;
			SimuPkBak = p413App->pEcho->SimuPkNum;
			p413App->pEcho->SimuPkNum = 0;
			CalCount = 0;
		}
		else {
			m_cDPCalib.EnableWindow(TRUE);
			m_cSPCalib.EnableWindow(TRUE);
			m_SaveCalib.EnableWindow(TRUE);
			ReleaseUI();
		}
	}
}

void CSetupProp3Dlg::OnSpcalib() {
	// 10/3/24 temporary bypassed auth
	// 
	//if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
	//	pUDev->pTitlePanel->AskLogin();
	//	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
	//		return;
	//	}
	//}

	//if (AfxMessageBox("Are you sure?", MB_YESNO) != IDYES) {
	//	return;
	//}

	BlockUI();

	m_cSPCalib.EnableWindow(FALSE);
	m_cDPCalib.EnableWindow(FALSE);
	m_SaveCalib.EnableWindow(FALSE);
	CInputCalibWaferThickness dlg;

	dlg.m_CalibThickness = g->SPGageThickness; //changed AP 01192011
	if (dlg.DoModal() == IDOK) {
		SPCalibThick = dlg.m_CalibThickness;
		g->SPGageThickness = dlg.m_CalibThickness; //added AP 01192011
		g->SaveMeasurementOption();
		if (!bSPCalib) {
			bSPCalib = TRUE;
			CalCount = 0;
		}
	}
	else {
		m_cSPCalib.EnableWindow(TRUE);
		m_cDPCalib.EnableWindow(TRUE);
		m_SaveCalib.EnableWindow(TRUE);

		ReleaseUI();
	}
}

void CSetupProp3Dlg::OnPlot() {
	if (m_bPlot) m_bPlot = FALSE; else m_bPlot = TRUE;
}

void CSetupProp3Dlg::ResizeRect(CRect& rc) {
	if (bSaveChart) {
		rc.bottom *= 1.0;
		rc.right *= 1.8;
	}
	else {
		rc.bottom = rc.top + 215;
	}
}

void CSetupProp3Dlg::PlotPeaks(CDC* pDC, CRect& rec, int nNumberOfData, int DispChn, int Scale, BOOL bBothChn) {
	CEchoProbeBase* pEcho = p413App->pEcho;

	int i;
	int Wd = rec.Width();
	int Ht = rec.Height();

	BOOL bFP2 = TRUE;
	short nRFactor = 24;
	int nReducedData = nNumberOfData / nRFactor;

	static int nRD = 0;
	BOOL bChanged = FALSE;
	if (nRD != nReducedData) {
		bChanged = TRUE;
		nRD = nReducedData;
	}

	if (pRcp->MeParam[g->CurMeSet - 1].PSG[0].Near > 0) {
		bFP2 = TRUE;
	}
	else {
		bFP2 = FALSE;
	}

	switch (DispChn) {
	case ECHO1:
	{
		int nData1;
		int* pX = pfXData;

		switch (Scale) {
		default: YMIN = -1900; YMAX = 1900; break;
		case 1:	YMIN = -1024; YMAX = 1024; break;
		case 2:	YMIN = -512; YMAX = 512; break;
		case 3:	YMIN = -256; YMAX = 256; break;
		case 4:	YMIN = -128; YMAX = 128; break;
		case 5:	YMIN = -64; YMAX = 64; break;
		case 6:	YMIN = -32; YMAX = 32; break;
		case 7:	YMIN = -16; YMAX = 16; break;
		case 8:	YMIN = -8; YMAX = 8; break;
		case 9:	YMIN = -4; YMAX = 4; break;
		case 10: YMIN = -2; YMAX = 2; break;
		}

		int Xmax, Xmin;
		nData1 = pEcho->GetEcho1Data(pfEcho1Data, pfXData, Xmax, Xmin, nRFactor, FALSE);  // for echo1 [3/10/2010 Valued Customer]
		if (!bFP2) {
			XMIN = Xmin; XMAX = Xmax;
			YMIN += 3072;
			YMAX += 3072;
		}
		else {
			YMIN += 2048;
			YMAX += 2048;
		}
		m_pointsCnt = nData1;
	}
	break;
	case ECHO2:
	{
		int nData1;
		int* pX = pfXData;

		switch (Scale) {
		default: YMIN = -1900; YMAX = 1900; break;
		case 1:	YMIN = -1024; YMAX = 1024; break;
		case 2:	YMIN = -512; YMAX = 512; break;
		case 3:	YMIN = -256; YMAX = 256; break;
		case 4:	YMIN = -128; YMAX = 128; break;
		case 5:	YMIN = -64; YMAX = 64; break;
		case 6:	YMIN = -32; YMAX = 32; break;
		case 7:	YMIN = -16; YMAX = 16; break;
		case 8:	YMIN = -8; YMAX = 8; break;
		case 9:	YMIN = -4; YMAX = 4; break;
		case 10: YMIN = -2; YMAX = 2; break;
		}
		int Xmax, Xmin;
		nData1 = pEcho->GetEcho2Data(pfEcho2Data, pfXData, Xmax, Xmin, nRFactor, FALSE);  // for echo2 [3/10/2010 Valued Customer]
		if (!bFP2) {
			XMIN = Xmin; XMAX = Xmax;
			YMIN += 3072;
			YMAX += 3072;
		}
		else {
			YMIN += 2048;
			YMAX += 2048;
		}

		m_pointsCnt = nData1;
	}
	break;

	case CHNA:
	case CHNB:
	{
		short rdc = rec.Width() / 2;
		if (DispChn == CHNA) {
			pEcho->GetAChannelData(pfEcho1Data, rdc, TRUE);
		}
		else {
			pEcho->GetBChannelData(pfEcho1Data, rdc, TRUE);
		}
		int* fX = pfXData;
		for (i = 0; i < rdc; i++) {
			*(fX++) = i;
		}
		XMIN = 0; XMAX = rdc - 1;

		int* fY = pfEcho1Data;
		YMIN = YMAX = *(fY++);
		for (i = 1; i < rdc; i++) {
			if (*fY < YMIN) {
				YMIN = *fY;
			}
			if (*fY > YMAX) {
				YMAX = *fY;
			}
			fY++;
		}

		m_pointsCnt = rdc;
	}
	break;
	case POSN:
	{
		nReducedData = pEcho->GetPositionData(pfEcho1Data, nRFactor, FALSE);
		int* pX = pfXData;
		for (i = 0; i < nReducedData; i++) {
			*(pX++) = i;
		}

		XMIN = 0; XMAX = nReducedData - 1;

		int* f = pfEcho1Data;
		YMIN = YMAX = *f;
		for (i = 0; i < nReducedData; i++) {
			if (*f < YMIN) {
				YMIN = *f;
			}
			if (*f > YMAX) {
				YMAX = *f;
			}
			f++;
		}
		m_pointsCnt = nReducedData;
	}
	break;
	}

	if (pDC) {
		if (!bBothChn) {
			switch (DispChn) {
			case ECHO1:
				if (bFP2) {
					m_cEchoPeaks.SetData1(p413App->pEcho->pSLineT, NULL, YMIN, YMAX, 0, 0);
				}
				else {
					m_cEchoPeaks.SetData0(pfXData, pfEcho1Data, NULL, m_pointsCnt, XMIN, XMAX, YMIN, YMAX, 0, 0, p413App->pEcho->m_nDirection);
				}
				break;
			case ECHO2:
				if (bFP2) {
					m_cEchoPeaks.SetData1(p413App->pEcho->pSLineB, NULL, YMIN, YMAX, 0, 0);
				}
				else {
					m_cEchoPeaks.SetData0(pfXData, pfEcho2Data, NULL, m_pointsCnt, XMIN, XMAX, YMIN, YMAX, 0, 0, p413App->pEcho->m_nDirection);
				}
				break;
			case POSN:
			case CHNA:
			case CHNB:
				m_cEchoPeaks.SetData0(pfXData, pfEcho1Data, NULL, m_pointsCnt - 1, XMIN, XMAX, YMIN, YMAX, 1, 0, p413App->pEcho->m_nDirection);
				break;
			default:
				m_cEchoPeaks.SetData0(pfXData, pfEcho1Data, NULL, m_pointsCnt - 1, XMIN, XMAX, YMIN, YMAX, 0, 0, p413App->pEcho->m_nDirection);
				break;
			}
		}
		else {
			m_cEchoPeaks.SetData0(pfXData, pfEcho1Data, pfEcho2Data, m_pointsCnt, XMIN, XMAX, YMIN, YMAX, 0, 0x08, p413App->pEcho->m_nDirection);
		}
	}
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

void CSetupProp3Dlg::SaveBitmap(CString Name, CBitmap& bmp) {
	BITMAP bitmapInfo;
	bmp.GetBitmap(&bitmapInfo);
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER pbih;
	UCHAR* buf2;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr);

	long BytesPerRow = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;

	pbih.biSize = sizeof(BITMAPINFOHEADER);
	pbih.biWidth = bitmapInfo.bmWidth;
	pbih.biHeight = bitmapInfo.bmHeight;
	pbih.biPlanes = bitmapInfo.bmPlanes;
	pbih.biBitCount = bitmapInfo.bmBitsPixel;
	pbih.biCompression = 0;
	pbih.biSizeImage = BytesPerRow * abs(pbih.biHeight);
	pbih.biXPelsPerMeter = 4000;
	pbih.biYPelsPerMeter = 4000;
	pbih.biClrUsed = 0;
	pbih.biClrImportant = 0;

	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + pbih.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	buf2 = new UCHAR[pbih.biSizeImage];
	int iTransfer;
	int iByteToTransfer;
	iByteToTransfer = BytesPerRow * abs(bitmapInfo.bmHeight);
	UCHAR* buf3 = new UCHAR[iByteToTransfer];
	iTransfer = bmp.GetBitmapBits(iByteToTransfer, buf3);
	UCHAR* pSrc, * pDest;
	int i, j;
	for (i = 0, j = bitmapInfo.bmHeight - 1; i < bitmapInfo.bmHeight; i++, j--) {
		pSrc = buf3 + i * bitmapInfo.bmWidthBytes;
		pDest = buf2 + j * bitmapInfo.bmWidthBytes;
		memcpy(pDest, pSrc, bitmapInfo.bmWidthBytes);
	}
	delete[] buf3;

	CString fname;
	fname = "C:\\TEMP\\";
	fname += Name + ".BMP";

	CFile file;
	if (file.Open(fname, CFile::modeWrite | CFile::modeCreate)) {
		file.Write(&hdr, sizeof(BITMAPFILEHEADER));
		file.Write(&pbih, sizeof(BITMAPINFOHEADER));
		file.Write(buf2, pbih.biSizeImage);
		file.Close();
	}
	if (buf2) delete[](buf2);
}

void CSetupProp3Dlg::OnRealunit() {
	if (bReal) {
		bReal = FALSE;
		m_cEchoPeaks.bReal = bReal;
	}
	else {
		bReal = TRUE;
		m_cEchoPeaks.bReal = bReal;
	}
}

// void CSetupProp3Dlg::OnSaverecipe()
// {
// 	if (p413App->ValidCheckRcp(g->RcpSetup))
// 	{
// 		pRcp->SaveRecipe();
// 		RecipeToLocal();
// 	}
// }

BOOL CSetupProp3Dlg::DestroyWindow() {
	Log("Stop Echo");
	StopEcho();
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

void CSetupProp3Dlg::OnDryrun() {
	m_cRun.EnableWindow(FALSE);
	BlockUI();
	StopEcho();
	p413App->Global.bStop = FALSE;
	pDryRunThread = AfxBeginThread(CSetupProp3Dlg::ExecuteRecipe, this);
	m_cStop.EnableWindow(TRUE);
}

void CSetupProp3Dlg::OnStoprun() {
	p413App->Global.bStop = TRUE;
	while (p413App->Global.bStop) {
		p413App->Yld(200);
	}
	m_cRun.EnableWindow(TRUE);
	m_cStop.EnableWindow(FALSE);
	ReleaseUI();
}

// Call from other as a thread [9/23/2012 Administrator]
UINT CSetupProp3Dlg::ExecuteRecipe(LPVOID pObj) {
	CSetupProp3Dlg* pDlg = (CSetupProp3Dlg*)pObj;

	if (pDlg) {
		pDlg->DoMeasurement();
		pDlg->m_cRun.EnableWindow(TRUE);
		pDlg->m_cStop.EnableWindow(FALSE);
		pDlg->ReleaseUI();
		p413App->Global.bStop = FALSE;
		AfxEndThread(0);
		return TRUE;
	}
	return FALSE;
}

// Call from separate thread [7/7/2010 128 NT]
BOOL CSetupProp3Dlg::DoMeasurement() {
	BOOL bIsEcho = pRcp->IsEchoMeasurement();
	if (bIsEcho) {
		p413App->GotoPositionechoNonBlocking();  // Need to be non-blocking [7/13/2010 C2C]
		p413App->pDMC->Run();
	}
	pRcp->ClearData();
	m_cWaferMap.dmMode = CWaferMap::dmUSER;
	m_cWaferMap.Redraw();
	bDoMeasurement = TRUE;
	pRcp->CarrierID = "MANUALRUN";
	CString str;
	str.Format("%s", CTime::GetCurrentTime().Format("%H%M"));
	pRcp->SlotID = atoi(str);
	MeasureOneWafer(pRcp);
	bDoMeasurement = FALSE;
	m_cWaferMap.dmMode = CWaferMap::dmENGINNER;

	if (bIsEcho) {
		p413App->pDMC->Stop();
	}
	return TRUE;
}

void CSetupProp3Dlg::UpdatePosition() {
	CString msg;

	msg.Format("X:%.2f Y:%.2f EchoTopZ:%.2f RoughZ:%.2f", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocRoughZ);
	m_cMessage.SetWindowText(msg);
	m_cWaferMap.Redraw();
}

void CSetupProp3Dlg::OnClose() {
	StopEcho();
	p413App->StopCamera(&m_cVideo, NULL);
	CResizableDialog::OnClose();
}

void CSetupProp3Dlg::OnMark() {
	if (MarkThisPoint()) {
		RecipeToLocal();
	}
}

void CSetupProp3Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp3Dlg::OnStartecho() {
	if (m_cStartEcho.GetCheck()) {
		StartingEcho();
		m_cSPCalib.EnableWindow(TRUE);
		m_cDPCalib.EnableWindow(TRUE);
		m_SaveCalib.EnableWindow(TRUE);
	}
	else {
		m_cSPCalib.EnableWindow(FALSE);
		m_cDPCalib.EnableWindow(FALSE);
		m_SaveCalib.EnableWindow(FALSE);
		StoppingEcho();
	}
}

float* pfSumT = NULL;
float* pfSumB = NULL;
float* pfST, * pfSB;

void CSetupProp3Dlg::OnDrawchart() {
	int i;
	float fTT = 0.0;

	static short counter = 0;
	static ULONGLONG tcount = GetTickCount64();

	CEchoProbeBase* pEcho = p413App->pEcho;

	if (bDPCalibTest) {
		bDPCalibTest = FALSE;

		float ThkT = pEcho->fGetThickness(0, 0, 1);
		float ThkB = pEcho->fGetThickness(1, 0, 1);
		float Thkn = (pEcho->fCalibProbeDistance[0] - ThkT) + (pEcho->fCalibProbeDistance[1] - ThkB);
		if (fabs(Thkn) < 0.3f) {
			CString msg;
			msg.Format("RefT=%.3f   RefB=%.3f   ThkS=%.3f   DP=%.3f", ThkT, ThkB, Thkn, g->DPDistance);
			AfxMessageBox(msg);
			m_cDPCalib.EnableWindow(TRUE);
			m_cSPCalib.EnableWindow(TRUE);
			m_SaveCalib.EnableWindow(TRUE);
			pEcho->SimuPkNum = SimuPkBak;
			ReleaseUI();
		}
		else {
			bDPCalib = TRUE;
		}
	}
	if (bDPCalib) {
		if (CalCount < 32) {
			if (CalCount == 0) {
				if (pfSumT) {
					delete[] pfSumT; pfSumT = NULL;
				}
				if (pfSumB) {
					delete[] pfSumB; pfSumB = NULL;
				}
				pfSumT = new float[MAXAVEBUFF]; pfST = pfSumT;
				pfSumB = new float[MAXAVEBUFF]; pfSB = pfSumB;
			}
			*(pfST++) = pEcho->fGetThickness(TOPPRB, 0, 1);
			*(pfSB++) = pEcho->fGetThickness(BOTPRB, 0, 1);
			CalCount++;
		}
		else {
			float ThkT = p413App->fRemoveFlyer(CalCount, pfSumT, 0, 1);
			float ThkB = p413App->fRemoveFlyer(CalCount, pfSumB, 0, 1);
			if (pfSumT) {
				delete[] pfSumT;
				pfSumT = NULL;
			}
			if (pfSumB) {
				delete[] pfSumB;
				pfSumB = NULL;
			}
			CalCount = 0;
			pEcho->fCalibProbeDistance[0] = ThkT;
			pEcho->fCalibProbeDistance[1] = ThkB;
			float pos = 0;
			p413App->pMtr->GetEchoTopZPosDev(pos);
			g->DPZPos = pos * 1000;	// Convert to micron [3/28/2013 Yuen]
			g->DPZOff = 0;
			float ThkS = g->DPGageThickness;
			g->DPDistance = (ThkT + ThkB) + ThkS;
			g->SaveCalFactor();
			bDPCalib = FALSE;
			bDPCalibTest = TRUE;
		}
	}

	if (bSPCalibTest) {
		bSPCalibTest = FALSE;

		float ThkT = fabs(pEcho->fGetThickness(0, 1, 2) * p413App->cfTop / (p413App->SRIT) - SPCalibThick);
		float ThkB = fabs(pEcho->fGetThickness(1, 1, 2) * p413App->cfBot / (p413App->SRIB) - SPCalibThick); //changed AP 01192011

		if ((ThkT < 0.12f) && (ThkB < 0.12f)) {
			CString str;
			str.Format("Gauge,%.4f,,Cal Top,%.8f,,Cal Bottom,%.8f", g->SPGageThickness, p413App->cfTop, p413App->cfBot);
			p413App->RecordSPCalibEvent(str);

			str.Format("factorTop = %.6f, factorBot = %.6f", p413App->cfTop, p413App->cfBot);
			m_ri = 3.5f;
			UpdateData(FALSE);
			AfxMessageBox(str);
			m_cSPCalib.EnableWindow(TRUE);
			m_cDPCalib.EnableWindow(TRUE);
			m_SaveCalib.EnableWindow(TRUE);
			ReleaseUI();
		}
		else {
			bSPCalib = TRUE;
		}
	}
	if (bSPCalib) {
		if (CalCount < 32) {
			if (CalCount == 0) {
				if (pfSumT) {
					delete[] pfSumT; pfSumT = NULL;
				}
				if (pfSumB) {
					delete[] pfSumB; pfSumB = NULL;
				}
				pfSumT = new float[129]; pfST = pfSumT;
				pfSumB = new float[129]; pfSB = pfSumB;
				*(pfST++) = pEcho->fGetThickness(0, 1, 2);
				*(pfSB++) = pEcho->fGetThickness(1, 1, 2);
			}
			else {
				*(pfST++) = pEcho->fGetThickness(0, 1, 2);
				*(pfSB++) = pEcho->fGetThickness(1, 1, 2);
			}
			CalCount++;
		}
		else {
			SumT = p413App->fRemoveFlyer(CalCount, pfSumT, 0, 1);
			SumB = p413App->fRemoveFlyer(CalCount, pfSumB, 0, 1);
			if (pfSumT) {
				delete[] pfSumT; pfSumT = NULL;
			}
			if (pfSumB) {
				delete[] pfSumB; pfSumB = NULL;
			}

			CalCount = 0;
			bSPCalib = FALSE;
			bSPCalibTest = TRUE;
			//20131124 SCOTT FOR TO MEET SPEC p413App->cfTop = SPCalibThick * p413App->SRIT / SumT;
			p413App->cfTop = (SPCalibThick * p413App->SRIT / SumT) + 0.0;
			p413App->cfBot = SPCalibThick * p413App->SRIB / SumB;
		}
	}
	if (bSetFocus) {
		bSetFocus = FALSE;
		pRcp->EchoFocus = pEcho->fGetThickness(0, 0, 1);  // Always top probe [8/24/2011 FSMT]
		CString str;
		str.Format("%.2f um", pRcp->EchoFocus);
		m_cSetFocus.SetWindowText(str);
		m_cSetFocusPoint.EnableWindow(TRUE);
	}
	if (bClearFocus) {
		bClearFocus = FALSE;
		pRcp->EchoFocus = 0;
		CString str;
		str.Format("%.2f um", pRcp->EchoFocus);
		m_cSetFocus.SetWindowText(str);
		m_cClearSF.EnableWindow(TRUE);
	}
	if (bDoFocus) {
		bDoFocus = FALSE;
		float fFinal = 5.0f;
		float z = pEcho->fGetThickness(0, 0, 1);
		float dz = z - pRcp->EchoFocus;
		if (fabs(dz) < fFinal) {
			// Reach limit [8/25/2011 FSMT]
			m_cDoFocus.EnableWindow(TRUE);
			m_cSetFocusPoint.EnableWindow(TRUE);
		}
		else if (fabs(dz) < 5 * fFinal) {
			p413App->pMtr->MoveREchoTopZ(dz * 0.75f * 1e-3);
			SetTimer(IDT_FOCUS, 100, NULL);
		}
		else {
			p413App->pMtr->MoveREchoTopZ(dz * 1e-3);
			SetTimer(IDT_FOCUS, 100, NULL);
		}
	}

	if (pRcp->bEchoPeakMonitor) {
		CSingleLock Lock(&m_cVideo.CS1);
		Lock.Lock();
		ASSERT(m_cVideo.AllocSLine());
		m_cVideo.pSLineT->Dup(pEcho->pSLineT);
		m_cVideo.pSLineB->Dup(pEcho->pSLineB);
		m_cVideo.pSLineT->CalcMaxMin();
		m_cVideo.pSLineB->CalcMaxMin();
		m_cVideo.pPeakBuf = &pEcho->PeaksBuf[0];
		Lock.Unlock();
		m_cVideo.FileDirc = "";
		m_cVideo.FileBase = "";
		m_cVideo.ScanLineReady = 1;
	}

	if (pDump) {
		fprintf(pDump, "%s,", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
	}
	for (i = 0; i < MaXPA; i++) {
		MParam* mp = &pRcp->MeParam[g->CurMeSet - 1].MPa[i];
		if (mp->Prb > 0) {
			if (mp->IsThicknessMeasurement()) {
				// Single probe measurement [3/10/2010 Valued Customer]
				if ((mp->Fr) && (mp->To)) {
					if (mp->Prb == MParam::ECHO1PRB) {
						float fTh = pEcho->fGetThickness(0, mp->Fr - 1, mp->To - 1);
						if (mp->Fr == 1)	// air to air measurement [5/6/2013 user]
						{
							mp->fThkn = fTh + mp->Off;
						}
						else {
							if (mp->RI != 1)	// Thickness measurement [5/6/2013 user]
							{
								mp->fThkn = fTh * p413App->cfTop / (mp->RI * p413App->TRiCorr) + mp->Off;
							}
							else	// air to air measurement [5/6/2013 user]
							{
								mp->fThkn = fTh * p413App->cfTop + mp->Off;
							}
						}
					}
					else {
						float fTh = pEcho->fGetThickness(1, mp->Fr - 1, mp->To - 1);
						if (mp->Fr == 1)	// air to air measurement [5/6/2013 user]
						{
							mp->fThkn = fTh + mp->Off;
						}
						else {
							if (mp->RI != 1)	// Thickness measurement [5/6/2013 user]
							{
								mp->fThkn = fTh * p413App->cfBot / (mp->RI * p413App->BRiCorr) + mp->Off;
							}
							else	// air to air measurement [5/6/2013 user]
							{
								mp->fThkn = fTh * p413App->cfBot + mp->Off;
							}
						}
					}
					fThknBuf[i][nThknBuf] = mp->fThkn;
					CalAveStdev(i);
					UpdateThkn(i, mp->fThkn);
					if (pDump) {
						fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
					}
				}
				else {
					UpdateThkn(i, fTT);
				}
			}
			// 			else if (mp->IsBowWarpMeasurement())
			// 			{
			// 				// Single probe measurement [3/10/2010 Valued Customer]
			// 				if (mp->IsTopProbe())
			// 				{
			// 					mp->fThkn = pEcho->fGetThickness(0, 0, 1) + mp->Off;
			// 				}
			// 				else
			// 				{
			// 					mp->fThkn = pEcho->fGetThickness(1, 0, 1) + mp->Off;
			// 				}
			// 				fThknBuf[i][nThknBuf] = mp->fThkn;
			// 				CalAveStdev(i);
			// 				UpdateThkn(i,mp->fThkn);
			// 				if (pDump)
			// 				{
			// 					fprintf(pDump,"%.3f,%.3f,%.3f,",mp->fThkn,Ave[i],Stdev[i]);
			// 				}
			// 			}
			else if (mp->IsTotalThicknessMeasurement()) {
				// Do not support auto focus feature here [4/17/2013 Administrator]
				// Dual Probe measurement [3/10/2010 Valued Customer]
				float fThkT = pEcho->fGetThickness(TOPPRB, 0, 1);
				float fThkB = pEcho->fGetThickness(BOTPRB, 0, 1);
				mp->fThkn = (pEcho->fCalibProbeDistance[TOPPRB] - fThkT) +
					(pEcho->fCalibProbeDistance[BOTPRB] - fThkB) +
					g->DPGageThickness + mp->Off + g->DPZOff;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			// 			else if ((mp->Prb == MParam::TFILMPRB) || (mp->Prb == MParam::TFILMSPMV) || (mp->Prb == MParam::TFILMFASTPRB))
			// 			{
			// 				if (p413App->pOceanOp)
			// 				{
			// 					CMeParam *p = &pRcp->MeParam[g->CurMeSet-1];
			// 					p413App->pOceanOp->SetParam(p->Ta.Method,1,p->Ta.IntegTime,p->Ta.Boxcar,g->TFScalingFactor);
			// 					mp->fThkn = p413App->pOceanOp->GetPolyThickness(p,p->Ta.ThkMin[0],p->Ta.ThkMax[0],p->Ta.RI[0],
			// 						(CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet-1],pRcp->nCycle,pRcp->nMappedPoints,pRcp->NSmooth,
			// 						pRcp->nExpand) + mp->Off;
			// 					UpdateThkn(i,mp->fThkn);
			// 					if (pDump)
			// 					{
			// 						fprintf(pDump,"%.3f,",mp->fThkn);
			// 					}
			// 				}
			// 			}
			// 			else if (mp->Prb == MParam::STRESSPRB)
			// 			{
			// 				// Single probe measurement [3/10/2010 Valued Customer]
			// 				mp->fThkn = pEcho->fGetThickness(TOPPRB, 0, 1) + mp->Off;
			// 				fThknBuf[i][nThknBuf] = mp->fThkn;
			// 				CalAveStdev(i);
			// 				UpdateThkn(i,mp->fThkn);
			// 				if (pDump)
			// 				{
			// 					fprintf(pDump,"%.3f,%.3f,%.3f,",mp->fThkn,Ave[i],Stdev[i]);
			// 				}
			// 			}
			
			//[ Mahedi Kamal - 03202024
			//else if (mp->Prb == MParam::ROUGHPRB) {
			else if (mp->IsRoughnessMeasurement()) {
				mp->fRough = p413App->pRough->Read();
				fThknBuf[i][nThknBuf] = mp->fRough;
				CalAveStdev(i);
				UpdateThkn(i, mp->fRough);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fRough, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::INTENSI1PRB) {
				// Peak Intensity probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[TOPPRB].Peaks[mp->Fr - 1].YMaxima;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::INTENSI2PRB) {
				// Peak Intensity probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[BOTPRB].Peaks[mp->Fr - 1].YMaxima;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::PPOS1PRB) {
				// Peak Intensity probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[TOPPRB].Peaks[mp->Fr - 1].fCMs * 1e-4;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::PPOS2PRB) {
				// Peak Intensity probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[BOTPRB].Peaks[mp->Fr - 1].fCMs * 1e-4;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::MATHPRB) {
				CMeParam* Me = &pRcp->MeParam[g->CurMeSet - 1];
				short ix, n = 0;
				float value = 0;
				CString* pstr = &mp->Formula;
				for (int k = 0; k < ((pstr->GetLength() + 1) / 2); k++) {
					if (n == 0) {
						ix = GetPAIdx(pstr->GetAt(n));
						if ((ix >= 0) && (ix < MaXPA)) {
							value = Me->MPa[ix].fThkn;
						}
						n++;
					}
					else {
						char ch = pstr->GetAt(n);
						n++;
						ix = GetPAIdx(pstr->GetAt(n));
						if ((ix >= 0) && (ix < MaXPA)) {
							switch (ch) {
							case '+':
								value += Me->MPa[ix].fThkn;
								break;
							case '-':
								value -= Me->MPa[ix].fThkn;
								break;
							case '*':
								value *= Me->MPa[ix].fThkn;
								break;
							case '/':
								if (Me->MPa[ix].fThkn) {
									value /= Me->MPa[ix].fThkn;
								}
								break;
							}
						}
						n++;
					}
				}
				mp->fThkn = value + mp->Off;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::BESTVALUEPRB) {
				CMeParam* Me = &pRcp->MeParam[g->CurMeSet - 1];
				short ix;
				float value = BADDATANEG;
				mp->fThkn = BADDATANEG;
				CString* pstr = &mp->Formula;
				for (int k = 0; k < pstr->GetLength(); k++) {
					ix = GetPAIdx(pstr->GetAt(k));
					if ((ix >= 0) && (ix < MaXPA)) {
						value = Me->MPa[ix].fThkn;
						if (value >= 0) {
							mp->fThkn = value + mp->Off;
							break;
						}
					}
				}
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::SHAPEFAC1PRB) {
				// Shape factor probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[TOPPRB].Peaks[mp->Fr - 1].GetShapeFactor();
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::SHAPEFAC2PRB) {
				// Shape factor probe [12/17/2010 C2C]
				mp->fThkn = pEcho->PeaksBuf[1].Peaks[mp->Fr - 1].GetShapeFactor();
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			// [01182024 // 11302022 ZHIMING
			else if (mp->Prb == MParam::CONFOCAL_TOP) {
				double val = 0.0;
				p413App->pCWL->GetMeasuredValueMRO(1, val);
				mp->fThkn = val * 1000;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			else if (mp->Prb == MParam::CONFOCAL_BOT) {
				double val = 0.0;
				p413App->pCWL->GetMeasuredValueMRO(2, val);
				mp->fThkn = val * 1000;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			// ]
			//[Tushar //20231227
			else if (mp->Prb == MParam::CONFOCAL_TOTAL) {
				/*double channelOneVal = 0.0, channelTwoVal=0.0;
				char txt[256];
				CString inifile;
				DosUtil.GetLocalCfgFile(inifile);
				CString sec = "CWL";
				double CalibraionConstant;

				p413App->pCWL->GetMeasuredValueMRO(1, channelOneVal);
				p413App->pCWL->GetMeasuredValueMRO(1, channelTwoVal);

				GetPrivateProfileString(sec, "CalibrationSampleThickness", "1", txt, 255, inifile);
				CalibraionConstant = atof(txt);
				double val = channelOneVal + channelTwoVal + CalibraionConstant;
				mp->fThkn = val * 1000;
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}*/


				CString inifile;
				double result = -1.0, channelOneVal = 0.0, chennelTwoVal = 0.0;
				p413App->pCWL->GetMeasuredValueMRO(1, channelOneVal);
				p413App->pCWL->GetMeasuredValueMRO(2, chennelTwoVal);
				result = channelOneVal + chennelTwoVal + DosUtil.GetConfocalK();				
				mp->fThkn = result * 1000.0;	// convert to um
				fThknBuf[i][nThknBuf] = mp->fThkn;
				CalAveStdev(i);
				UpdateThkn(i, mp->fThkn);
				if (pDump) {
					fprintf(pDump, "%.3f,%.3f,%.3f,", mp->fThkn, Ave[i], Stdev[i]);
				}
			}
			//]
		}
		else {
			Ave[i] = 0;
			Stdev[i] = 0;
			UpdateThkn(i, fTT);
		}
	}

	if (nThknBuf < (m_NAve - 1)) {
		nThknBuf++;
	}
	else {
		nThknBuf = 0;
	}
	if (pDump) {
		fprintf(pDump, "\n");
	}
	SetEvent(m_hCalculatingEvent);

	if (m_bPlot) {
		CRect rec;
		m_cEchoPeaks.GetClientRect(&rec);
		m_cEchoPeaks.ClientToScreen(&rec);
		ScreenToClient(&rec);
		InflateRect(rec, -6, -6);
		CDC* pDC = GetDC();
		if (pDC) {
			PlotPeaks(pDC, rec, pEcho->GetNumberOfData(), nDisplay, m_nScale, FALSE);
			ReleaseDC(pDC);
		}
	}

	CString str;
	counter++;
	ULONGLONG tick;
	if (counter > m_NAve) {
		tick = GetTickCount64();
		str.Format("%.1f", m_NAve / ((tick - tcount) / 1000.0f));
		m_cTime.SetWindowText(str);
		counter = 0;
		tcount = tick;
	}
	str.Format("%d", counter);
	m_cCounter.SetWindowText(str);
}

BOOL CSetupProp3Dlg::MeasureOneWafer(CRecipe413* pRcp) {
	if (!g || !pRcp) {
		ASSERT(0);
		return FALSE;
	}
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return FALSE;
	}
	CPtrList* pMPList = &pRcp->Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}

	// Getting ready to measure [7/12/2010 128 NT]
	// Set all points to NOTMEASURE [6/14/2011 XPMUser]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			p->ProgressSM = CMPoint::NOTMEASURE;
		}
	}

	p413App->Global.DLoc.ClearAll();

	// Measurement commences: Echo Probe First [7/12/2010 128 NT]

	//////////////////////////////////////////////////////////////////////////
	// Measure Thickness [6/14/2011 XPMUser]
	//////////////////////////////////////////////////////////////////////////

	int n;
	BOOL bFirst;
	// Measure Thickness
	if (pRcp->IsTotalThickness()) {
		// perform dual probe calibration [8/10/2010 XPMUser]
		p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::ECHOCAM);

		float ZPos = 0;
		CPeakParam PParam1;
		p413App->pMtr->GetEchoTopZPosDev(ZPos);
		p413App->SetPeakParamDualCalib(PParam1);
		p413App->DoDualProbeCalibration(g->DPGageThickness, ZPos, &g->RcpSetup, g->CurMeSet, NULL, &m_cVideo, &PParam1, FALSE, 0);
	}

	n = 1;
	bFirst = TRUE;
	pos = pMPList->GetHeadPosition();
	while (pos && (!g->bStop)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (!pRcp->IsEchoMeasurement(p)) {
				continue;
			}
			if (bFirst) {
				pMtr->SetMicroscopeLight(TRUE);
				bFirst = FALSE;
				// goto echo position [7/13/2010 C2C]
				p413App->GotoPositionecho();
				pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
			}
			if (p413App->pEcho->bSimu) {
				if (pRcp->IsProfileMeasurement(p)) {
					p413App->DefinePeakPosition1(p, 0);
				}
				else {
					p413App->DefinePeakPosition0(14);
				}
			}
			p413App->MeasureOnePointEcho(pRcp, p, code, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, NULL, NULL, FALSE);
			UpdatePosition();
		}
	}

	// Measure Edge
	n = 1;
	bFirst = TRUE;
	pos = pMPList->GetHeadPosition();
	while (pos && (!g->bStop)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (!pRcp->IsEdgeMeasurement(p)) {
				continue;
			}
			if (bFirst) {
				bFirst = FALSE;
				pMtr->SetMicroscopeLight(TRUE);
				p413App->GotoPositionecho();
				pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
			}
			p->ProgressSM = CMPoint::NOTMEASURE;
			p413App->MeasureTrim(pRcp, p, g->ActiveCamera, &m_cVideo, NULL, NULL, NULL);
			UpdatePosition();
		}
	}

	// Measure Thin Film
	n = 1;
	bFirst = TRUE;
	pos = pMPList->GetHeadPosition();
	while (pos && (!g->bStop)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (!pRcp->IsThinFilmMeasurement(p)) {
				continue;
			}
			if (bFirst) {
				bFirst = FALSE;
				m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%m%d%H%M%S"));  // Select a base name [7/28/2011 XPMUser]
				// Probe goto thin film position [7/13/2010 C2C]
				pMtr->SetSpectroIllumLight(FALSE);
				pMtr->SetSpectroSourceLight(TRUE);
				p413App->GotoPositionthinfilm();
				pMtr->MoveThinFilmZ(pRcp->fThinFilmZ);
				p413App->TakeBackground(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF);
				p413App->TakeReference(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF);
			}
			p->ProgressSM = CMPoint::NOTMEASURE;
			p413App->MeasureThinFilm(pRcp, p, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, NULL, NULL);
			UpdatePosition();
		}
	}
	pMtr->SetSpectroIllumLight(TRUE);
	pMtr->SetSpectroSourceLight(FALSE);

	return TRUE;
}

void CSetupProp3Dlg::OnMakebaseline() {
	if (pRcp) pRcp->SaveBaseline();
}

void CSetupProp3Dlg::StaticRepeatabilityTest() {
	Dpposition();

	int i;
	CMPoint mp;
	CString str;
	CString v;
	v = CTime::GetCurrentTime().Format("%H%M%S");
	str.Format("SR%s", v);
	bSaveSpec1 = TRUE;
	bSaveSpec2 = TRUE;
	bSaveSpec3 = TRUE;
	g->ImageTitle = str;
	g->ImageCount = 1;
	mp.MeSet = 1;
	short code[MaXPA];
	for (i = 0; i < MaXPA; i++) {
		code[i] = 0;  // all measurement OK [3/14/2010 Valued Customer]
	}
	if (!p413App->GetSubstrateThicknesses(&g->RcpSetup, &mp, TRUE, code, NULL, NULL)) {
		m_cGenMessage.SetWindowText(p413App->MsgStr);
	}
	CData* data = mp.GetData(0, FALSE);

	str.Format("C:\\TEMP\\SR-%s.CSV", v);
	FILE* fp = fopen(str, "wb");
	if (fp) {
		for (i = 0; i < MaXPA; i++) {
			float fData = data->Get(i);
			if (fData != BADDATANEG) {
				fprintf(fp, "%.4f\n", fData);
			}
			else {
				fprintf(fp, "N/A\n");
			}
		}
		fclose(fp);
	}
	AfxMessageBox("Done");
}

void CSetupProp3Dlg::DumpValues() {
	if (pDump) {
		fprintf(pDump, "\nEnd,%.3f", GetTickCount64() / 1000.0f);
		fclose(pDump);
		pDump = NULL;
	}
	else {
		// [20220720 ZHIMING WILL PREVENT ERROR FOR NO TEMP FOLDER
		CString str;
		str.Format("C:\\TEMP\\");
		if (GetFileAttributes(str) == INVALID_FILE_ATTRIBUTES) {
			CreateDirectory(str, NULL);
		}
		// ]
		str.Format("C:\\TEMP\\DUMP%d.CSV", DumpCounter);
		DumpCounter++;
		DosUtil.SetDumpCounter(DumpCounter);
		pDump = fopen(str, "wb");
		fprintf(pDump, "Number of averaging = %d\n", m_NAve);
		fprintf(pDump, "Start,%.3f\n", GetTickCount64() / 1000.0f);

		fprintf(pDump, "%s,", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
		for (int i = 0; i < MaXPA; i++) {
			MParam* pa = &pRcp->MeParam[g->CurMeSet - 1].MPa[i];
			if (pa->Name.GetLength() > 0) {
				fprintf(pDump, "%s,", pa->Name.GetBuffer(0));
				fprintf(pDump, "%s,", "Ave");
				fprintf(pDump, "%s,", "Stdev");
			}
		}
		fprintf(pDump, "\n");
	}
}

long CSetupProp3Dlg::OnRelocateToBack(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToBack(p);
	return 0;
}

long CSetupProp3Dlg::OnRelocateToTop(WPARAM wP, LPARAM lP) {
	CCoor* p = (CCoor*)lP;
	RelocateToTop(p);
	return 0;
}
void CSetupProp3Dlg::OnTest() {
	CProbeTest dlg;

	dlg.bDump = (pDump ? TRUE : FALSE);
	dlg.bDumpPeaks = ((pDmpPeakT || pDmpPeakB) ? TRUE : FALSE);
	dlg.DoModal();

	switch (dlg.Cmd) {
	case CProbeTest::PTRECORD:	// Record [3/21/2013 Yuen]
		DumpValues();
		break;
	case 2:
		//	p413App->pEcho->SimuRemoveFlyer();
		break;
	case 3:
		//	StaticRepeatabilityTest();
		break;
	case CProbeTest::PTDUMPDATA:	// Dump Data [3/21/2013 Yuen]
	{
		CString v, str;
		v = CTime::GetCurrentTime().Format("%H%M%S");
		str.Format("SR%s", v);
		g->ImageTitle = str;
		g->ImageCount = 1;

		bSaveSpec1 = TRUE;
		bSaveSpec2 = TRUE;
		bSaveSpec3 = TRUE;
		bSaveSpec31 = TRUE;
		bSaveSpec4 = TRUE;
		bSaveSpec5 = TRUE;
	}
	break;
	case CProbeTest::PTRECORDPOS:	// Record Position [3/21/2013 Yuen]
		if (pDmpPeakT || pDmpPeakB) {
			p413App->CloseDmpPeak();
		}
		else {
			p413App->OpenDmpPeak();
		}
		break;
	case CProbeTest::PTEXPORTWITHBASELINE:	// Export with baseline [3/21/2013 Yuen]
		ExportWithBaseline();
		break;
	case CProbeTest::PROBEDISTANCE:			// Probe distance test [3/21/2013 Yuen]
	{
		CProbeDistParam dlg;

		dlg.m_Distance = 10.0f;
		dlg.m_NSteps = 5;

		if (dlg.DoModal() == IDOK) {
			ProbDistParam.n = 0;
			ProbDistParam.dist = dlg.m_Distance;
			ProbDistParam.nstep = dlg.m_NSteps;
			SetTimer(IDT_PROBETEST, 50, NULL);
		}
	}
	break;
	case CProbeTest::PROBEOPTIMIZE:
		DoOptimizeEcho();
		break;
	}
}

void CSetupProp3Dlg::CalAveStdev(short n) {
	Ave[n] = 0;
	//	Ave2[n] = 0;

	int i;
	double Av = 0;	// mean [7/19/2013 Yuen]
	double Av2 = 0;	// deviation square [7/19/2013 Yuen]

	// Calculate mean [7/19/2013 Yuen]
	for (i = 0; i < m_NAve; i++) {
		double v = (double)(fThknBuf[n][i]);
		Av += v;
	}
	Av /= double(m_NAve);
	// calculate variance [7/19/2013 Yuen]
	for (i = 0; i < m_NAve; i++) {
		double v = (double)(fThknBuf[n][i] - Av);
		Av2 += (v * v);
	}
	Av2 /= double(m_NAve - 1);	// this is variance, -1 for sample variance, no -1 for population variance [7/19/2013 Yuen]

	Stdev[n] = sqrt(Av2);  // square root of variance [7/19/2013 Yuen]
	Ave[n] = Av;
}

void CSetupProp3Dlg::OnKillfocusAirbot() {
	UpdateData(TRUE);
}

void CSetupProp3Dlg::OnKillfocusAirtop() {
	UpdateData(TRUE);
}

void CSetupProp3Dlg::OnSaveencoderfactor() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		pUDev->pTitlePanel->AskLogin();
		if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}

	DosUtil.SetEncoderFactor(1, p413App->cfTop);
	DosUtil.SetEncoderFactor(2, p413App->cfBot);
}

void CSetupProp3Dlg::OnTakepicture2() {
	m_cVideo.FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d"));
	m_cVideo.FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S"), int(rand() / float(RAND_MAX) * 100));
	m_cVideo.bStoreComposite = TRUE;
	while (m_cVideo.bStoreComposite) {
		Sleep(80);
	}
}

void CSetupProp3Dlg::StopMotor() {
	p413App->pDMC->Stop();
}

void CSetupProp3Dlg::StartMotor() {
	p413App->pDMC->Run();
}

void CSetupProp3Dlg::OnKillfocusNave() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	pRcp->MeParam[MeS].NAve = m_NAve;
}

void CSetupProp3Dlg::Dpposition() {
	p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::ECHOCAM);
}

void CSetupProp3Dlg::OnKillfocusEdscale() {
	UpdateData(TRUE);
}

void CSetupProp3Dlg::OnKillfocusRi() {
	UpdateData(TRUE);
}

void CSetupProp3Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp3Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp3Dlg::OnSet4() {
	SwapMeSet(3);
}

void CSetupProp3Dlg::OnSet5() {
	SwapMeSet(4);
}

void CSetupProp3Dlg::OnSet6() {
	SwapMeSet(5);
}

void CSetupProp3Dlg::OnSet7() {
	SwapMeSet(6);
}

void CSetupProp3Dlg::OnSet8() {
	SwapMeSet(7);
}

void CSetupProp3Dlg::OnSet9() {
	SwapMeSet(8);
}

void CSetupProp3Dlg::OnSet10() {
	SwapMeSet(9);
}

void CSetupProp3Dlg::SwapMeSet(short next) {
	if (g) {
		g->CurMeSet = next;
		RecipeToLocal();
	}
}

void CSetupProp3Dlg::OnStarthisto() {
	CEditBinDlg dlg;

	static int dx = 5.0f;
	dlg.m_iHisto = iHisto + 1;
	dlg.m_Limit = m_cHisto.nCountLimit;
	dlg.dx = dx;
	if (dlg.DoModal() == IDOK) {
		m_cHisto.ClearBins();
		iHisto = dlg.m_iHisto - 1;
		m_cHisto.nCountLimit = dlg.m_Limit;
		m_cHisto.SetRange(Ave[iHisto] - dlg.dx, Ave[iHisto] + dlg.dx);
		dx = dlg.dx;
		bDispHisto = TRUE;
	}
}

void CSetupProp3Dlg::OnStophisto() {
	bDispHisto = FALSE;
}

void CSetupProp3Dlg::OnConthisto() {
	bDispHisto = TRUE;
}

void CSetupProp3Dlg::OnLoadhisto() {
	CFileDialog dlg(TRUE,
		"TXT",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Text files|*.TXT|");
	if (dlg.DoModal() == IDOK) {
		FILE* fp = fopen(dlg.GetPathName(), "rb");
		if (fp) {
			float fMax, fMin;
			fscanf(fp, "%f\n", &fMax);
			fscanf(fp, "%f\n", &fMin);
			float r = (fMax - fMin) / 16.0;
			m_cHisto.ClearBins();
			m_cHisto.SetRange(fMin - r, fMax + r);
			do {
				fscanf(fp, "%f\n", &fMax);
				m_cHisto.Bin(fMax);
			} while (!feof(fp));
			fclose(fp);
			m_cHisto.Invalidate();
		}
	}
}

void CSetupProp3Dlg::OnKillfocusThresholdfactor3() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::OnKillfocusThresholdfactor2() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::OnKillfocusRcpLevel1() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::OnKillfocusRcpLevel2() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::OnKillfocusRcpPeakwidth1() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::OnKillfocusRcpPeakwidth2() {
	UpdateData(TRUE);
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}
}

void CSetupProp3Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		m_cWaferMap.Redraw();
	}
}

void CSetupProp3Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp3Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp3Dlg::OnSetfocuspoint() {
	m_cSetFocusPoint.EnableWindow(FALSE);
	if (!m_cStartEcho.GetCheck()) {
		StartingEcho();
		m_cStartEcho.SetCheck(FALSE);
	}
	bSetFocus = TRUE;
}

void CSetupProp3Dlg::OnDofocus() {
	m_cDoFocus.EnableWindow(FALSE);
	m_cSetFocusPoint.EnableWindow(FALSE);
	bDoFocus = TRUE;
}

void CSetupProp3Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_FOCUS) {
		KillTimer(IDT_FOCUS);
		bDoFocus = TRUE;
	}
	if (nIDEvent == IDT_START) {
		if (p413App->pEcho) {
			KillTimer(IDT_START);
			if (p413App->pEcho->bSimu) {
				m_cSimuPk.ShowWindow(TRUE);
			}
			CEchoFinder::Initialize();
			p413App->pEcho->SetRecipe(pRcp);
		}
		// Retry if pEcho is NULL until succeed [6/3/2012 Administrator]
	}
	if (nIDEvent == IDT_PROBETEST) {
		KillTimer(IDT_PROBETEST);
		ExecuteProbeDistanceTest();
	}
	// [12022022 ZHIMING
	if (nIDEvent == IDT_REFRESH) {
		PostDrawChartMessage(nullptr);
	}
	// ]
}

long CSetupProp3Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

long CSetupProp3Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

// void CSetupProp3Dlg::OnKillfocusSrit()
// {
// 	UpdateData(TRUE);
// 	p413App->TRiCorr = p413App->SRIT / 3.5f;
// 	p413App->BRiCorr = p413App->SRIB / 3.5f;
// }

// void CSetupProp3Dlg::OnKillfocusSrib()
// {
// 	UpdateData(TRUE);
// 	p413App->TRiCorr = p413App->SRIT / 3.5f;
// 	p413App->BRiCorr = p413App->SRIB / 3.5f;
// }

void CSetupProp3Dlg::StoppingEcho() {
	Log("Stop Echo");
	m_cStartEcho.EnableWindow(FALSE);
	if (StopEcho()) {
		m_cStartEcho.SetWindowText("Start Echo\nProbe");
	}
	m_cStartEcho.EnableWindow(TRUE);
}

void CSetupProp3Dlg::StartingEcho() {
	Log("Start Echo Probe");
	m_cStartEcho.EnableWindow(FALSE);
	hWnd = this->m_hWnd;
	if (p413App->pEcho->bSimu) {
		p413App->DefinePeakPosition0(0);
	}
	p413App->pEcho->pGenMessage = &m_cGenMessage;
	if (StartEcho()) {
		m_cStartEcho.SetWindowText("Stop Echo\nProbe");
	}
	m_cStartEcho.EnableWindow(TRUE);
}

void CSetupProp3Dlg::OnKillfocusSimupk() {
	UpdateData(TRUE);
	p413App->pEcho->SimuPkNum = m_SimuPk;
}

// void CSetupProp3Dlg::OnLoadrecipe2()
// {
// 	if (p413App->LoadRecipeFromDataFile(g->RcpSetup))
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp3Dlg::OnLocations2() {
	p413App->LocationProp(g->RcpSetup, m_hWnd);
	RecipeToLocal();
}

void CSetupProp3Dlg::OnRefreshui() {
	UpdatePosition();
	RecipeToLocal();
	m_cWaferMap.Redraw();
}

void CSetupProp3Dlg::OnDianostic() {
	if (pRcp->bEchoPeakMonitor) {
		pRcp->bEchoPeakMonitor = FALSE;
		m_cDianostic.SetCheck(FALSE);
	}
	else {
		pRcp->bEchoPeakMonitor = TRUE;
		m_cDianostic.SetCheck(TRUE);
	}
}

void CSetupProp3Dlg::OnSavevideo2() {
	if (pRcp->bSaveVideo) {
		pRcp->bSaveVideo = FALSE;
		m_cSaveVideo.SetCheck(FALSE);
	}
	else {
		pRcp->bSaveVideo = TRUE;
		m_cSaveVideo.SetCheck(TRUE);
	}
}

void CSetupProp3Dlg::OnClearbaseline() {
	if (pRcp) pRcp->ClearBaseLine();
}

void CSetupProp3Dlg::ExportWithBaseline() {
	CFileDialog dlg(FALSE,
		"CSV",
		NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"CSV file|*.CSV|");

	CString filename = "C:\\DATA\\";

	dlg.m_ofn.lpstrInitialDir = filename;
	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		CFile fp;
		if (fp.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
			pRcp->ExportData(fp, TRUE);
			fp.Close();
		}
	}
}

long CSetupProp3Dlg::OnAddpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp3Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

void CSetupProp3Dlg::Renumber() {
	if (pRcp) pRcp->Renumber();
	m_cWaferMap.Redraw();
}

short CSetupProp3Dlg::GetPAIdx(TCHAR ch) {
	switch (ch) {
	case '2':
		return 1;
	case '3':
		return 2;
	case '4':
		return 3;
	case '5':
		return 4;
	case '6':
		return 5;
	case '7':
		return 6;
	case '8':
		return 7;
	case '9':
		return 8;
	case 'A':
	case 'a':
		return 9;
	case 'B':
	case 'b':
		return 10;
	case 'C':
	case 'c':
		return 11;
	case 'D':
	case 'd':
		return 12;
	case 'E':
	case 'e':
		return 13;
	case 'F':
	case 'f':
		return 14;
	case 'G':
	case 'g':
		return 15;
	default:
		return 0;
	}
}

void CSetupProp3Dlg::ExecuteProbeDistanceTest() {
	switch (ProbDistParam.n) {
	case 0:
	{
		// Dump initial data [3/21/2013 Yuen]
		CString v, str;
		v = CTime::GetCurrentTime().Format("%H%M%S");
		str.Format("SR%s", v);
		g->ImageTitle = str;
		g->ImageCount = 1;

		bSaveSpec1 = TRUE;
		bSaveSpec5 = TRUE;
		ProbDistParam.n = 1;
		SetTimer(IDT_PROBETEST, 2000, NULL);
	}
	break;
	case 1:
		// Move probe away by step distance [3/21/2013 Yuen]
		if (ProbDistParam.nstep > 0) {
			if (p413App->pMtr->MoveREchoTopZ(-ProbDistParam.dist)) {
				ProbDistParam.n = 0;
				ProbDistParam.nstep--;
				SetTimer(IDT_PROBETEST, 500, NULL);
			}
		}
		else {
			AfxMessageBox("Done Probe Distance Test");
		}
		break;
	}
}

void CSetupProp3Dlg::OnClearaf() {
	m_cClearSF.EnableWindow(FALSE);
	bClearFocus = TRUE;
}

void CSetupProp3Dlg::DoOptimizeEcho() {
	p413App->Global.bStop = FALSE;

	// Optimize echo motor here [6/17/2013 Yuen]
	// Read and parse Motor.txt [6/17/2013 Yuen]
	char* pBuf = NULL;
	CString* pStr = NULL;
	int FSize = 0;
	int NLines = 0;
	int FilePointer = 0;
	CFile* fp = NULL;
	CFileException ex;
	fp = new CFile;
	if (fp) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "motor.txt";
		if (fp->Open(dir, CFile::modeRead | CFile::typeBinary, &ex)) {
			FSize = fp->GetLength();
			pBuf = new char[FSize + 16];
			if (pBuf) {
				TRY
				{
					fp->Read(pBuf,FSize);
				}
					CATCH(CMemoryException, e) {
					if (pBuf) {
						delete[] pBuf; pBuf = NULL;
						fp->Close();
						delete fp; fp = NULL;
					}
					return;
				}
				END_CATCH
					delete[] pBuf;
			}
			fp->Close();
			delete fp; fp = NULL;

			NLines = GetNbLine(pBuf, FSize);
			if (NLines > 0) {
				pStr = new CString[NLines + 1];
				if (pStr) {
					for (int i = 0; i < NLines; i++) {
						*(pStr + i) = GetOneLine(pBuf, FilePointer, FSize);
					}
				}
			}
		}
		else {
			delete fp;
			return;
		}
	}
	COptimizeEchoDlg* pdlg = new COptimizeEchoDlg;
	if (pdlg) {
		pdlg->Create(COptimizeEchoDlg::IDD);
		pdlg->ShowWindow(SW_SHOW);

		if (!m_cStartEcho.GetCheck()) {
			StoppingEcho();
			m_cSPCalib.EnableWindow(TRUE);
			m_cDPCalib.EnableWindow(TRUE);
			m_SaveCalib.EnableWindow(TRUE);
		}

		// Start motor here [6/17/2013 Yuen]
		p413App->pDMC->Run();
		OptimizeEcho(pdlg, NLines, pStr);
		// Stop motor here [6/17/2013 Yuen]
		p413App->pDMC->Stop();

		pdlg->DestroyWindow();
		delete pdlg;
	}
	delete[] pStr; pStr = 0;
}

#define MAXLINE 1024

CString CSetupProp3Dlg::GetOneLine(char* pBuf, int& FilePointer, int FSize) {
	CString str;
	char* pBufLine = new char[MAXLINE + 16];
	int i = 0;
	do {
		pBufLine[i] = *(pBuf + (FilePointer++));
		if (pBufLine[i] != '\r') {
			i++;
		}
	} while ((*(pBuf + FilePointer - 1) != '\n') &&
		(i < MAXLINE) &&
		(FilePointer < FSize));
	pBufLine[i - 1] = 0;
	if (FilePointer >= FSize) {
		str = "EOF";
		delete[] pBufLine;
		return str;
	}
	str = pBufLine;
	delete[] pBufLine;
	return str;
}

void CSetupProp3Dlg::OptimizeEcho(COptimizeEchoDlg* pDlg, int NLines, CString* pStr) {
	p413App->GotoXYS(p413App->Global.DPPos.X, p413App->Global.DPPos.Y, CCamBase::ECHOCAM);

	short cnt;
	int i, j;
	CPeaks Pk;
	CString value;
	float Ave, Ave2;
	float TTop[36];
	float Average[16], Stdev[16];
	float K1 = GetValue("K1", pStr, NLines);
	if (K1 != -1) {
		// Optimize K1 [6/17/2013 Yuen]
		for (i = -6; i <= 6; i++) {
			if (p413App->Global.bStop) {
				return;
			}
			float v = K1 + i * 0.25f;
			if (v >= 0) {
				value.Format("K1%.1f\r", v);
				pDlg->m_cList.InsertItem(0, value);
				p413App->pDMC->Send(value.GetBuffer(0));
				Sleep(1000);
				cnt = 0;
				// Take data [6/17/2013 Yuen]
				for (j = 0; j < 32; j++) {
					if (p413App->Global.bStop) {
						return;
					}
					//CEchoProbeBase::GETPEAKSERR ret = p413App->pEcho->GetPeaks(pRcp,Pk,1,NULL,NULL,NULL,1,"");
					if (p413App->pEcho->GetPeaks(pRcp, Pk, 1, NULL, NULL, NULL, 1, "") == CEchoProbeBase::GPALLHOK) {
						TTop[cnt] = p413App->pEcho->fGetThickness(0, 2, 3);
						cnt++;
					}
				}
				// Calculate statistics [6/17/2013 Yuen]
				Ave = 0;
				Ave2 = 0;
				for (j = 0; j < cnt; j++) {
					float v = TTop[j];
					Ave += v;
					Ave2 += (v * v);
				}
				Stdev[i + 6] = sqrt(fabs(cnt * Ave2 - Ave * Ave) / (cnt * (cnt - 1)));
				Ave /= cnt;
				Average[i + 6] = Ave;
			}
		}
		// Pick the best K1 [6/17/2013 Yuen]
		Ave = -1;
		Ave2 = -1;
		for (i = 0; i <= 13; i++) {
			if (Stdev[i] > Ave) {
				Ave = Stdev[i];
				Ave2 = i - 6;
			}
		}
		// Found best K1 [6/17/2013 Yuen]
		K1 += Ave2 * 0.25f;
		value.Format("K1%.1f\r", K1);
		pDlg->m_cList.InsertItem(0, value);
		p413App->pDMC->Send(value.GetBuffer(0));
	}

	float K2 = GetValue("K2", pStr, NLines);
	if (K2 != -1) {
		// Optimize K2 [6/17/2013 Yuen]
		for (i = -6; i <= 6; i++) {
			if (p413App->Global.bStop) {
				return;
			}
			float v = K2 + i * 0.25f;
			if (v >= 0) {
				value.Format("K2%.1f\r", v);
				pDlg->m_cList.InsertItem(0, value);
				p413App->pDMC->Send(value.GetBuffer(0));
				Sleep(1000);
				cnt = 0;
				// Take data [6/17/2013 Yuen]
				for (j = 0; j < 32; j++) {
					if (p413App->Global.bStop) {
						return;
					}
					//CEchoProbeBase::GETPEAKSERR ret = p413App->pEcho->GetPeaks(pRcp,Pk,1,NULL,NULL,NULL,1,"");
					if (p413App->pEcho->GetPeaks(pRcp, Pk, 1, NULL, NULL, NULL, 1, "") == CEchoProbeBase::GPALLHOK) {
						TTop[cnt] = p413App->pEcho->fGetThickness(0, 2, 3);
						cnt++;
					}
				}
				// Calculate statistics [6/17/2013 Yuen]
				Ave = 0;
				Ave2 = 0;
				for (j = 0; j < cnt; j++) {
					float v = TTop[j];
					Ave += v;
					Ave2 += (v * v);
				}
				Stdev[i + 6] = sqrt(fabs(cnt * Ave2 - Ave * Ave) / (cnt * (cnt - 1)));
				Ave /= cnt;
				Average[i + 6] = Ave;
			}
		}
		// Pick the best K2 [6/17/2013 Yuen]
		Ave = -1;
		Ave2 = -1;
		for (i = 0; i <= 13; i++) {
			if (Stdev[i] > Ave) {
				Ave = Stdev[i];
				Ave2 = i - 6;
			}
		}
		// Found best K2 [6/17/2013 Yuen]
		K2 += Ave2 * 0.25f;
		value.Format("K2%.1f\r", K2);
		pDlg->m_cList.InsertItem(0, value);
		p413App->pDMC->Send(value.GetBuffer(0));
	}

	float K3 = GetValue("K3", pStr, NLines);
	if (K3 != -1) {
		// Optimize K2 [6/17/2013 Yuen]
		for (i = -6; i <= 6; i++) {
			if (p413App->Global.bStop) {
				return;
			}
			float v = K3 + i * 0.25f;
			if (v >= 0) {
				value.Format("K3%.1f\r", v);
				pDlg->m_cList.InsertItem(0, value);
				p413App->pDMC->Send(value.GetBuffer(0));
				Sleep(1000);
				cnt = 0;
				// Take data [6/17/2013 Yuen]
				for (j = 0; j < 32; j++) {
					if (p413App->Global.bStop) {
						return;
					}
					//CEchoProbeBase::GETPEAKSERR ret = p413App->pEcho->GetPeaks(pRcp,Pk,1,NULL,NULL,NULL,1,"");
					if (p413App->pEcho->GetPeaks(pRcp, Pk, 1, NULL, NULL, NULL, 1, "") == CEchoProbeBase::GPALLHOK) {
						TTop[cnt] = p413App->pEcho->fGetThickness(0, 2, 3);
						cnt++;
					}
				}
				// Calculate statistics [6/17/2013 Yuen]
				Ave = 0;
				Ave2 = 0;
				for (j = 0; j < cnt; j++) {
					float v = TTop[j];
					Ave += v;
					Ave2 += (v * v);
				}
				Stdev[i + 6] = sqrt(fabs(cnt * Ave2 - Ave * Ave) / (cnt * (cnt - 1)));
				Ave /= cnt;
				Average[i + 6] = Ave;
			}
		}
		// Pick the best K3 [6/17/2013 Yuen]
		Ave = -1;
		Ave2 = -1;
		for (i = 0; i <= 13; i++) {
			if (Stdev[i] > Ave) {
				Ave = Stdev[i];
				Ave2 = i - 6;
			}
		}
		// Found best K3 [6/17/2013 Yuen]
		K3 += Ave2 * 0.25f;
		value.Format("K3%.1f\r", K3);
		pDlg->m_cList.InsertItem(0, value);
		p413App->pDMC->Send(value.GetBuffer(0));
	}
}

int CSetupProp3Dlg::GetNbLine(char* pBuf, int FSize) {
	CString str = "";
	int nLine = 0;
	int FilePointer = 0;

	while (1) {
		str = GetOneLine(pBuf, FilePointer, FSize);
		if (str != "EOF") {
			nLine++;
		}
		else {
			break;
		}
	}
	return nLine;
}

float CSetupProp3Dlg::GetValue(char* key, CString* pStr, int NLines) {
	int n;
	CString* p;
	for (int i = 0; i < NLines; i++) {
		p = (pStr + i);
		n = p->Find(key);
		if (n != -1) {
			return atof(p->Mid(n + 1));
		}
	}
	return -1;
}

void CSetupProp3Dlg::OnClearpc() {
	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = 0;
		pRcp->MeParam[MeS].PSG[i].Level = 70;
		pRcp->MeParam[MeS].PSG[i].Width = 180;
	}
	RecipeToLocal();
}

void CSetupProp3Dlg::OnClearpc2() {
	for (int j = 0; j < MAXMEPASET; j++) {
		for (int i = 0; i < 2; i++) {
			pRcp->MeParam[j].PSG[i].Thres = 0;
			pRcp->MeParam[j].PSG[i].Level = 70;
			pRcp->MeParam[j].PSG[i].Width = 180;
		}
	}
	RecipeToLocal();
}

void CSetupProp3Dlg::OnSleep() {
	m_cSleep.EnableWindow(FALSE);
	Sleep(10000);
	m_cSleep.EnableWindow(TRUE);
}

void CSetupProp3Dlg::OnCleardata() {
	pRcp->ClearData();
	RecipeToLocal();
}

void CSetupProp3Dlg::OnViewdata() {
	CViewData dlg;
	dlg.DoModal();
}

void CSetupProp3Dlg::OnOverlayinfo() {
	if (m_cVideo.bDisplayOverlay) {
		m_cVideo.bDisplayOverlay = FALSE;
	}
	else {
		m_cVideo.bDisplayOverlay = TRUE;
	}
	RecipeToLocal();
}
