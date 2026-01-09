#include "stdafx.h"

#include "e95.h"
#include "SetupProp81Dlg.h"

#include <MATH.H>
#include <fstream>
#include <cstdlib>
#include "XTabCtrl.h"

#include "GlobalUIContainer.h"
#include "413App.h"

#include "MPoint.h"
#include "SelectMeasurementDlg.h"

#include "..\SYS\CUMMsg.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"
#include "..\413\MotorS2.h"
#include "GlobalDeviceContainer.h"

#include "Pattern.h"
#include "CMPList.h"

#include "ViewData.h"
#include "DiceView.h"
#include "SetupProp14Dlg.h"
#include "CWL.h"
#include "InputCalibWaferThickness.h"
#include <chrono>

//[ [ARIF 3D View] 10152024
#include "Pegrpapi.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#pragma comment(lib,"PEGRP64G.lib")
//]
#include <algorithm>
#include "LSF3D.h"
#include <ProbeSwitching.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool isOk = false;

CSetupProp14Dlg::CSetupProp14Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp14Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupProp14Dlg)
	g = NULL;
	pRcp = NULL;
	bRun = FALSE;
	iRun = FALSE;
	m_ZPos = 16.0f;
	m_ZPos2 = 16.0f;
	mDieHeight = 30.0;
	mDieWidth = 30.0;
	m_cVideo.Name = "Setup14";
}

CSetupProp14Dlg::~CSetupProp14Dlg() {

	iRun = FALSE;

}

void CSetupProp14Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp14Dlg)
	DDX_Control(pDX, IDC_MESSAGE3, m_cList);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_PROFILE, m_cProfile);
	DDX_Text(pDX, IDC_CONFOCAL_Z_POS_EDIT, m_ZPos);
	DDX_Text(pDX, IDC_CONFOCAL_Z_POS_EDIT2, m_ZPos2);
	DDV_MinMaxFloat(pDX, m_ZPos, -30.0f, 30.0f); // Morsalin change to (-30,30) from (1, 30)
	DDV_MinMaxFloat(pDX, m_ZPos2, -30.0f, 30.0f); // Morsalin change to (-30,30) from (1, 30)
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);//Tushar 
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DIEHEIGHT, m_DieHeight);
	DDX_Control(pDX, IDC_DIEWIDTH, m_DieWidth);
	DDX_Control(pDX, IDC_3D_VIEWDLG14, m_cDiceWafer_V2);
	DDX_Control(pDX, IDC_CAMERAVIEWDLG14, m_cVideo);
	DDX_Control(pDX, IDC_MOUSEMESSAGEDLG14, m_cMouseMessage);
	DDX_Control(pDX, IDC_BUFFER, mBufferData);
	DDX_Control(pDX, IDC_OFFSET2, mOffsetData);
	DDX_Control(pDX, IDC_SCAN_FILTER, m_ScanFilter);
	DDX_Control(pDX, IDC_LTVMin, m_LTVMin);
	DDX_Control(pDX, IDC_LTVMax, m_LTVMax);
	DDX_Control(pDX, IDC_LTVAvg, m_LTVAvg);
	DDX_Control(pDX, IDC_LTV, m_LTV);
	DDX_Control(pDX, IDC_STIR, m_STIR);
	DDX_Control(pDX, IDC_INCLUDE_CHECK, m_Include);
	DDX_Text(pDX, IDC_ANCHOR_X, AnchorX);
	DDX_Text(pDX, IDC_ANCHOR_Y, AnchorY);
	DDX_Control(pDX, IDC_CONFOCAL_Z_POS_EDIT, m_zPos030);
	DDX_Control(pDX, IDC_CONFOCAL_Z_POS_EDIT2, m_zPos008);
	DDX_Control(pDX, IDC_THKSRH, thkNum);
	DDX_Control(pDX, IDC_WARPSRH, warpNum);
	DDX_Control(pDX, IDC_SRH_STEP, srhStepSize);
	DDX_Control(pDX, IDC_THICKNESS030, m_Thk030);
	DDX_Control(pDX, IDC_CALIB_AUTOFOCUS, m_calibAutoFocus);
	DDX_Control(pDX, IDC_THK_FIRST_SCAN, m_thkFirstScan); // 09232025_Shamim
	//DDX_Control(pDX, IDC_THICKNESS_SCAN_LENGTH, m_thk_scan_length); // 20250926_Mohir
}

BEGIN_MESSAGE_MAP(CSetupProp14Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp14Dlg)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_BN_CLICKED(IDC_MOTIONCONTROL1, OnMotioncontrol)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_MEASURE_CWL, &CSetupProp14Dlg::OnBnClickedMeasureCwl)
	ON_BN_CLICKED(IDC_CALIBRATE_CWL, &CSetupProp14Dlg::OnBnClickedCalibrateCwl)
	ON_BN_CLICKED(IDC_CH1DIS, &CSetupProp14Dlg::ChOneDisplamentModeBtn)
	ON_BN_CLICKED(IDC_CH2DIS, &CSetupProp14Dlg::ChTwoDisplacementMode)
	ON_BN_CLICKED(IDC_CH1INTENSITY, &CSetupProp14Dlg::ChOneIntensityBtn)
	ON_BN_CLICKED(IDC_CH2INTENSITY, &CSetupProp14Dlg::ChTwoIntensityBtn)
	ON_BN_CLICKED(IDC_CONFOCAL_Z_HEIGHT, &CSetupProp14Dlg::OnBnClickedConfocalZHeight)
	ON_EN_KILLFOCUS(IDC_CONFOCAL_Z_POS_EDIT, &CSetupProp14Dlg::OnKillfocusConfocalZPosEdit)
	ON_BN_CLICKED(IDC_CH1ZERO, &CSetupProp14Dlg::SetChannelOneZeroBtn)
	ON_BN_CLICKED(IDC_CH2ZERO, &CSetupProp14Dlg::SetChannelTwoZeroBtn)
	ON_BN_CLICKED(IDC_RESPONSE, &CSetupProp14Dlg::OnBnClickedResponse)
	ON_BN_CLICKED(IDC_PRESENCE, &CSetupProp14Dlg::OnBnClickedPresence)
	ON_BN_CLICKED(IDC_INSERT, &CSetupProp14Dlg::OnBnClickedInsert)
	ON_EN_CHANGE(IDC_THICKNESS_SCAN_LENGTH, &CSetupProp14Dlg::OnEnChangeThicknessScanLengthDlg14) // 20250926_Mohir
	ON_EN_CHANGE(IDC_APPROXZPOSCONF008DLG14, &CSetupProp14Dlg::OnEnChangeApproxZPosConf008Dlg14) // 10152024 Mahedi Kamal
	ON_EN_CHANGE(IDC_FOCUSDEVRANGE008DLG14, &CSetupProp14Dlg::OnEnChangeFocusDevRange008Dlg14) //  Mahedi Kamal 10152024
	ON_BN_CLICKED(IDC_SCANWAFERDLG14, &CSetupProp14Dlg::OnBnClickedScanwaferdlg14) //  Mahedi Kamal 10152024
	ON_BN_CLICKED(IDC_FOCUS008DLG14, &CSetupProp14Dlg::OnBnClickedFocus008dlg14) //  Mahedi Kamal 10152024
	ON_BN_CLICKED(IDC_STOPALLDLG14, &CSetupProp14Dlg::OnBnClickedStopalldlg14) //  Mahedi Kamal 10152024
	ON_BN_CLICKED(IDC_PLOT_DATA, &CSetupProp14Dlg::OnBnClickedPlotData)// [09042024 3D View] 10152024
	ON_BN_CLICKED(IDC_BtnMeasure2, &CSetupProp14Dlg::OnBnClickedBtnmeasure2)
	ON_BN_CLICKED(IDC_SAVE_SCAN_DATA, &CSetupProp14Dlg::OnBnClickedSaveScanData)
	ON_BN_CLICKED(IDC_RUN, &CSetupProp14Dlg::OnBnClickedRun)
	ON_STN_CLICKED(IDC_3D_VIEWDLG14, &CSetupProp14Dlg::OnStnClickedDiceMap)
	/*ON_BN_CLICKED(IDC_RADIO1, &CSetupProp14Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSetupProp14Dlg::OnBnClickedRadio2)*/
	ON_EN_CHANGE(IDC_ROUGHSTEP, &CSetupProp14Dlg::OnChangeRoughStep)
	ON_EN_CHANGE(IDC_ROUGHLINELEN, &CSetupProp14Dlg::OnChangeRoughLineLen)
	ON_BN_CLICKED(IDC_SHOWROUGHCWL, &CSetupProp14Dlg::OnBnClickedShowroughcwl)
	ON_BN_CLICKED(IDC_RMSROUGHCWL, &CSetupProp14Dlg::OnClickedRms)
	ON_BN_CLICKED(IDC_RAROUGHCWL, &CSetupProp14Dlg::OnClickedRa)
	ON_BN_CLICKED(IDC_UPDATE_INI, &CSetupProp14Dlg::OnBnClickedUpdateIni)
	ON_BN_CLICKED(IDC_STOP_SCAN, &CSetupProp14Dlg::OnBnClickedStopScan)
	ON_BN_CLICKED(IDC_CREATEPOINTS, &CSetupProp14Dlg::OnBnClickedCreatepoints)
	ON_BN_CLICKED(IDC_INCLUDE_CHECK, &CSetupProp14Dlg::OnBnClickedIncludeCheck)
	ON_BN_CLICKED(IDC_SCAN_FILTER, &CSetupProp14Dlg::OnBnClickedScanFilter)
	ON_BN_CLICKED(IDC_LTVMin, &CSetupProp14Dlg::OnBnClickedLTVMin)
	ON_BN_CLICKED(IDC_LTVMax, &CSetupProp14Dlg::OnBnClickedLTVMax)
	ON_BN_CLICKED(IDC_LTVAvg, &CSetupProp14Dlg::OnBnClickedLTVAvg)
	ON_BN_CLICKED(IDC_LTV, &CSetupProp14Dlg::OnBnClickedLTV)
	ON_BN_CLICKED(IDC_STIR, &CSetupProp14Dlg::OnBnClickedSTIR)
	ON_BN_CLICKED(IDC_FOCUS030DLG14, &CSetupProp14Dlg::OnBnClickedFocus030dlg14)
	ON_EN_CHANGE(IDC_APPROXZPOSCONF030DLG14, &CSetupProp14Dlg::OnEnChangeApproxzposconf030dlg14)
	ON_EN_CHANGE(IDC_FOCUSDEVRANGE030DLG14, &CSetupProp14Dlg::OnEnChangeFocusdevrange030dlg14)
	ON_BN_CLICKED(IDC_MONITOR008, &CSetupProp14Dlg::OnBnClickedMonitor008)
	ON_BN_CLICKED(IDC_MONITOR030, &CSetupProp14Dlg::OnBnClickedMonitor030)
	ON_BN_CLICKED(IDC_CAMERASETTINGDLG14, &CSetupProp14Dlg::OnBnClickedCamerasettingdlg14)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_BN_CLICKED(IDC_ADD, &CSetupProp14Dlg::OnBnClickedAdd)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_BN_CLICKED(IDC_LOCATIONS2DLG14, &CSetupProp14Dlg::OnBnClickedLocations2dlg14)
	ON_BN_CLICKED(IDC_TAKEPICTURE2DLG14, &CSetupProp14Dlg::OnBnClickedTakepicture2dlg14)
	//[[20241217
	ON_MESSAGE(IDC_ADDPOINT, OnAddpoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	//]]
	ON_BN_CLICKED(IDC_ROUGH_SCAN, &CSetupProp14Dlg::OnBnClickedRoughScan)
	ON_BN_CLICKED(IDC_CONFOCAL_TOP_MOTOR, &CSetupProp14Dlg::OnBnClickedConfocalTopMotor)
	
	ON_BN_CLICKED(IDC_MONITOR040, &CSetupProp14Dlg::OnBnClickedMonitor040) // 12262024 Mahedi Kamal
	
	ON_BN_CLICKED(IDC_ZPOSROUGH, &CSetupProp14Dlg::OnBnClickedZposrough)
	ON_COMMAND(IDC_MARK, OnMark)//01022025
	ON_BN_CLICKED(IDC_SET_CWL_MOVINGAVG008, &CSetupProp14Dlg::SetCWLMovingAvg008)
	ON_BN_CLICKED(IDC_SET_CWL_MOVINGAVG030, &CSetupProp14Dlg::SetCWLMovingAvg030)
	ON_BN_CLICKED(IDC_SET_CWL_MOVINGAVG040, &CSetupProp14Dlg::SetCWLMovingAvg040)
	ON_BN_CLICKED(IDC_RECIPE_AUTOFOCUS, &CSetupProp14Dlg::OnBnClickedRecipeAutofocus)
	ON_BN_CLICKED(IDC_ROUGHCWLNM, &CSetupProp14Dlg::OnBnClickedRoughcwlnm)
	ON_BN_CLICKED(IDC_ROUGHCWLA, &CSetupProp14Dlg::OnBnClickedRoughcwla)
	

	ON_EN_CHANGE(IDC_SRH_STEP, &CSetupProp14Dlg::OnEnChangeSearchStep)
	ON_EN_CHANGE(IDC_THKSRH, &CSetupProp14Dlg::OnEnChangeThkSearch)
	ON_EN_CHANGE(IDC_WARPSRH, &CSetupProp14Dlg::OnEnChangeWarpSearch)
	ON_COMMAND(ID_REFRESHUI, OnRefreshui)
	ON_BN_CLICKED(IDC_CALIB_AUTOFOCUS, &CSetupProp14Dlg::OnBnClickedCalibAutofocus)
	ON_BN_CLICKED(IDC_THK_FIRST_SCAN, &CSetupProp14Dlg::OnBnClickedThicknessFirstScan) // 09232025_Shamim
	ON_BN_CLICKED(IDC_THK_SCAN_TEST, &CSetupProp14Dlg::OnBnClickedThkScanTest)
	ON_BN_CLICKED(IDC_GET_CWL_MOVINGAVG008, &CSetupProp14Dlg::OnBnClickedGetCwlMovingavg008)
	ON_BN_CLICKED(IDC_GET_CWL_MOVINGAVG030, &CSetupProp14Dlg::OnBnClickedGetCwlMovingavg030)
	ON_BN_CLICKED(IDC_GET_CWL_MOVINGAVG040, &CSetupProp14Dlg::OnBnClickedGetCwlMovingavg040)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp14Dlg message handlers
long CSetupProp14Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		iRun = FALSE;
		isOk = false;
		if (!m_cVideo.IsReserved()) {
			p413App->StopCamera(&m_cVideo, NULL);
			m_cVideo.pWnd = NULL;
		}
		LocalToRecipe();
		bTabSelected = FALSE;
		if (displacementIntensityPlot.joinable())
			displacementIntensityPlot.detach();
		// Morsalin
		m_DieHeight.GetWindowText(DieHeight);
		m_DieWidth.GetWindowText(DieWidth);

		mDieHeight = _ttof(DieHeight);
		mDieWidth = _ttof(DieWidth);
		// ]
	}
	
	p413App->Global.setDiceDisplay = true;//10122024

	//OnBnClickedMonitor030();
	//p413App->monitor030 = FALSE;
	//p413App->monitor008 = FALSE;
	offAllMonitor(); // 12262024 Mahedi Kamal
	return 0;
}

void CSetupProp14Dlg::LocalToRecipe() {
	UpdateData(TRUE);
	//pRcp->fThinFilmZ = m_ZPos;
	pRcp->fCWLTopZ = m_ZPos; // Morsalin
	pRcp->fCWLTopZ2 = m_ZPos2; // Morsalin
	m_DieHeight.GetWindowText(DieHeight);
	m_DieWidth.GetWindowText(DieWidth);
	// [ Morsalin
	mDieHeight = _ttof(DieHeight);
	mDieWidth = _ttof(DieWidth);
	pRcp->DieHeight = mDieHeight;
	pRcp->DieWidth = mDieWidth;
	pRcp->m_bIncludeOn = p413App->Global.m_bIncludeOn;
	pRcp->m_LSFOn = p413App->Global.m_LSFOn;
	pRcp->AnchorX = AnchorX;
	pRcp->AnchorY = AnchorY;
	// ]

	// [ 10312025_1 MORSALIN
	pRcp->r_LTVMin = p413App->Global.r_LTVMin;
	pRcp->r_LTVMax = p413App->Global.r_LTVMax;
	pRcp->r_LTVAvg = p413App->Global.r_LTVAvg;
	pRcp->r_LTV = p413App->Global.r_LTV;
	pRcp->r_STIR = p413App->Global.r_STIR;
	// ]
}

long CSetupProp14Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		g->ActiveCamera = CCamBase::CWL;

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

	CString strValue;
	strValue.Format(_T("%.2f"), p413App->RoughScanLineLenCWL);
	SetDlgItemText(IDC_ROUGHLINELEN, strValue);

	strValue.Format(_T("%.4f"), p413App->RoughStepSizeCWL);
	SetDlgItemText(IDC_ROUGHSTEP, strValue);

	if (p413App->IsRaCWL) {
		CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
		pRadioRms->SetCheck(BST_UNCHECKED);  // Set it as selected


		CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
		pRadioRa->SetCheck(BST_CHECKED);
	}
	else {
		CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
		pRadioRms->SetCheck(BST_CHECKED);  // Set it as selected


		CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
		pRadioRa->SetCheck(BST_UNCHECKED);
	}
	//20250306 =======
	if (p413App->IsNmCWL) {
		CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
		pRadioNm->SetCheck(BST_UNCHECKED);  // Set it as selected


		CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
		pRadioA->SetCheck(BST_CHECKED);
	}
	else {
		CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
		pRadioNm->SetCheck(BST_CHECKED);  // Set it as selected


		CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
		pRadioA->SetCheck(BST_UNCHECKED);
	}
	//=========
	
	GenerateMap();

	return 0;
}

void CSetupProp14Dlg::PostNcDestroy() {
	iRun = FALSE;
	isOk = false;

	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp14Dlg::DestroyWindow() {
	iRun = FALSE;
	if (displacementIntensityPlot.joinable()) {
		displacementIntensityPlot.detach();
	}

	p413App->runningWaferScanCWLThread = FALSE;
	p413App->monitor030 = FALSE;
	p413App->monitor008 = FALSE;
	Sleep(200);
	p413App->StopCamera(&m_cVideo, NULL);
	return CResizableDialog::DestroyWindow();
}

BOOL CSetupProp14Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_MEASURE_CWL, TOP_RIGHT);
	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	AddAnchor(IDC_SAVE_SCAN_DATA, TOP_RIGHT);
	AddAnchor(IDC_RUN, TOP_RIGHT);
	AddAnchor(IDC_STOP_SCAN, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTINGDLG14, TOP_RIGHT);
	AddAnchor(IDC_TAKEPICTURE2DLG14, TOP_RIGHT);
	AddAnchor(IDC_LOCATIONS2DLG14, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_MESSAGE3, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_MOTIONCONTROL1, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	// Tushar
	m_cWaferMap.pRcp = pRcp;
	m_cVideo.SetRecipe(pRcp);
	m_cWaferMap.bSiteView = FALSE;
	m_cWaferMap.pParent = this;
	//

	SetupColumnPos();
	SetupColumnMessage();

	// [01242024 ZHIMING
	GetDlgItem(IDC_CH2DIS)->EnableWindow(g->bDualConfocal);
	GetDlgItem(IDC_CH2INTENSITY)->EnableWindow(g->bDualConfocal);
	GetDlgItem(IDC_CALIBRATE_CWL)->EnableWindow(g->bDualConfocal);
	//[20240122 Tushar
	GetDlgItem(IDC_CH2ZERO)->EnableWindow(g->bDualConfocal);
	// ]
	//[ 03092024
	buffData = 200.00;
	offsetData = 1;
	//]
	
	//[[20250926_Mohir
	CString s; s.Format("%.3f", p413App->ThicknessScanLength);
	SetDlgItemText(IDC_THICKNESS_SCAN_LENGTH, s);
	//]]


	//[//  Mahedi Kamal 10152024
	s.Format("%.3f", p413App->ZFocusPos008);
	SetDlgItemText(IDC_APPROXZPOSCONF008DLG14, s);

	s.Format("%.3f", p413App->ZFocusDevRenge008);
	SetDlgItemText(IDC_FOCUSDEVRANGE008DLG14, s);

	s.Format("%.3f", p413App->ZFocusPos030);
	SetDlgItemText(IDC_APPROXZPOSCONF030DLG14, s);

	s.Format("%.3f", p413App->ZFocusDevRenge030);
	SetDlgItemText(IDC_FOCUSDEVRANGE030DLG14, s);
	//]
	UpdateData(FALSE);

	mDieHeight = 32.29;
	mDieWidth = 24.38;
	m_Radius = 100.00;
	//p413App->Global.ifWithVal = false;

	// [Morsalin
	/*m_ScanStep.SetWindowTextA("5.0");
	m_ScanOffset.SetWindowTextA("3.0");
	m_ScanSpeed.SetWindowTextA("2.5");
	m_ScanStep.GetWindowTextA(ScanStep);
	m_ScanOffset.GetWindowTextA(ScanOffset);
	m_ScanSpeed.GetWindowTextA(ScanSpeed);
	p413App->Global.scanStep = atof(ScanStep);
	p413App->Global.scanOffset = atof(ScanOffset);
	p413App->Global.scanSpeed = atof(ScanSpeed);*/
	p413App->Global.m_bIncludeOn = FALSE;
	p413App->Global.m_LSFOn = FALSE;
	// ]

	// [ 10312025_1 MORSALIN
	p413App->Global.r_LTVMin = FALSE;
	p413App->Global.r_LTVMax = FALSE;
	p413App->Global.r_LTVAvg = FALSE;
	p413App->Global.r_LTV = FALSE;
	p413App->Global.r_STIR = FALSE;
	// ]


	//[ hiding custor cwl roughness parameter
	CWnd* pControl = GetDlgItem(IDC_ROUGHSTEP);
	if (pControl != nullptr) {
		pControl->ShowWindow(SW_HIDE);
	}
	pControl = GetDlgItem(IDC_ROUGHLINELEN);
	if (pControl != nullptr) {
		pControl->ShowWindow(SW_HIDE);
	}
	//]

	if (p413App->Global.isSqDie)
	{
		m_DieWidth.ShowWindow(FALSE);
		GetDlgItem(IDC_DIEWIDTHST)->ShowWindow(FALSE);
		GetDlgItem(IDC_DIESIZE)->ShowWindow(TRUE);
		GetDlgItem(IDC_DIEHEIGHTST)->ShowWindow(FALSE);
	}
	else
	{
		m_DieWidth.ShowWindow(TRUE);
		GetDlgItem(IDC_DIEWIDTHST)->ShowWindow(TRUE);
		GetDlgItem(IDC_DIESIZE)->ShowWindow(FALSE);
		GetDlgItem(IDC_DIEHEIGHTST)->ShowWindow(TRUE);
	}

	//ARIF 
	if (!p413App->SaveScanTestValue) {
		GetDlgItem(IDC_ROUGH_SCAN)->ShowWindow(FALSE);
	}
	
	CString stn = "5";
	thkNum.SetWindowTextA(stn);
	warpNum.SetWindowTextA(stn);
	iThkNum = 5;
	iWarpNum = 5;
	searchStepSize = 0.5;
	CString stn2 = "0.5";
	srhStepSize.SetWindowTextA(stn2);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp14Dlg::SetupColumnPos() {
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
}

void CSetupProp14Dlg::SetupColumnMessage() {
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
	m_cList.SetColumnWidth(idx++, LVSCW_AUTOSIZE_USEHEADER);
}

void CSetupProp14Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

void CSetupProp14Dlg::OnSaverecipe() {
	LocalToRecipe();
	RecipeIO();
	// [ 01012025 MORSALIN open DieMap on load
	if (p413App->Global.isLoad) {
		GenerateMap();
		p413App->Global.isLoad = false;
	}
	if (p413App->Global.isNew) {
		GenerateMap();
		p413App->Global.isNew = false;
	}
	// ]
}

void CSetupProp14Dlg::RecipeToLocal() {
	pRcp->UpdateControl(m_cPoint);
	MParam* mp = &pRcp->MeParam[0].MPa[0];
	//m_ZPos = pRcp->fThinFilmZ;
	m_ZPos = pRcp->fCWLTopZ; // Morsalin
	m_ZPos2 = pRcp->fCWLTopZ2; // Morsalin
	mDieHeight = pRcp->DieHeight; //Morsalin
	mDieWidth = pRcp->DieWidth; //Morsalin
	AnchorX = pRcp->AnchorX; //Morsalin
	p413App->Global.AnchorX = AnchorX; //Morsalin
	AnchorY = pRcp->AnchorY; //Morsalin
	p413App->Global.AnchorY = AnchorY; //Morsalin
	CString txt;
	txt.Format("%.0f", mDieHeight);
	m_DieHeight.SetWindowTextA(txt);
	txt.Format("%.0f", mDieWidth);
	m_DieWidth.SetWindowTextA(txt);
	p413App->Global.m_bIncludeOn = pRcp->m_bIncludeOn; //Morsalin
	/*p413App->RoughScanLineLenCWL = pRcp->RoughScanLineLenCWL_rcp;
	p413App->RoughStepSizeCWL = pRcp->RoughStepSizeCWL_rcp;*/
	p413App->IsRaCWL = pRcp->bIsRaCWL_rcp;
	p413App->IsNmCWL = pRcp->bIsNmCWL; //20250306
	p413App->Global.m_LSFOn = pRcp->m_LSFOn; //Morsalin
	p413App->thkSrchNum = pRcp->thkSearchNum;//10032025_3
	p413App->warpSrchNum = pRcp->warpSearchNum;//10032025_3
	p413App->stepSrchNum = pRcp->stepSrhSize;//10032025_3


	// [ 10312025_1 MORSALIN
	p413App->Global.r_LTVMin = pRcp->r_LTVMin;
	p413App->Global.r_LTVMax = pRcp->r_LTVMax;
	p413App->Global.r_LTVAvg = pRcp->r_LTVAvg;
	p413App->Global.r_LTV = pRcp->r_LTV;
	p413App->Global.r_STIR = pRcp->r_STIR;
	// ]

	////[03192025
	p413App->isCaliBAutoFoucs = pRcp->isCalibrationAutoFocusNeeded;

	if (p413App->isCaliBAutoFoucs == TRUE) {
		m_calibAutoFocus.SetCheck(BST_CHECKED);
	}
	else {
		m_calibAutoFocus.SetCheck(BST_UNCHECKED);
	}

	//]

	// [ 09232025_Shamim
	
	if (pRcp->ThicknessWithScan == TRUE) {
		m_thkFirstScan.SetCheck(BST_CHECKED); 
	}
	else {
		m_thkFirstScan.SetCheck(BST_UNCHECKED);
	}
	//]

	if(pRcp->m_LSFOn)
		m_ScanFilter.SetCheck(BST_CHECKED);
	else 
		m_ScanFilter.SetCheck(BST_UNCHECKED);

	// [ 10312025_1 MORSALIN
	if (pRcp->r_LTVMin)m_LTVMin.SetCheck(BST_CHECKED);
	else m_LTVMin.SetCheck(BST_UNCHECKED);

	if (pRcp->r_LTVMax)m_LTVMax.SetCheck(BST_CHECKED);
	else m_LTVMax.SetCheck(BST_UNCHECKED);

	if (pRcp->r_LTVAvg)m_LTVAvg.SetCheck(BST_CHECKED);
	else m_LTVAvg.SetCheck(BST_UNCHECKED);

	if (pRcp->r_LTV)m_LTV.SetCheck(BST_CHECKED);
	else m_LTV.SetCheck(BST_UNCHECKED);

	if (pRcp->r_STIR)m_STIR.SetCheck(BST_CHECKED);
	else m_STIR.SetCheck(BST_UNCHECKED);
	// ]

	if (pRcp->m_bIncludeOn)
		m_Include.SetCheck(BST_CHECKED);
	else
		m_Include.SetCheck(BST_UNCHECKED);

	//20250306 ARIF =========
	if (pRcp->bIsNmCWL) {
		CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
		pRadioNm->SetCheck(BST_UNCHECKED);  // Set it as selected

		CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
		pRadioA->SetCheck(BST_CHECKED);  // Deselect the radio button
	}
	else {
		CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
		pRadioNm->SetCheck(BST_CHECKED);  // Set it as selected

		CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
		pRadioA->SetCheck(BST_UNCHECKED);  // Deselect the radio button
	}

	//=============

	//10032025
	CString stn;
	stn.Format("%d", p413App->thkSrchNum);
	thkNum.SetWindowTextA(stn);
	stn.Format("%d", p413App->warpSrchNum);
	warpNum.SetWindowTextA(stn);
	stn.Format("%0.4lf", p413App->stepSrchNum);
	srhStepSize.SetWindowTextA(stn);
	//

	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_RECIPE_AUTOFOCUS);
	pCheckbox->SetCheck(pRcp->bIsAutoFocus008);

	if (!p413App->IsRaCWL) {
		CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
		pRadioRms->SetCheck(BST_CHECKED);

		CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
		pRadioRa->SetCheck(BST_UNCHECKED);  // Deselect the radio button
	}
	else {
		CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
		pRadioRms->SetCheck(BST_UNCHECKED);  // Set it as selected


		CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
		pRadioRa->SetCheck(BST_CHECKED);  // Deselect the radio button
	}

	UpdateData(FALSE);
	m_cWaferMap.Redraw();
}

void CSetupProp14Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
}

void CSetupProp14Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		//DoSelectMeasurment(pMP, nSel, m_cList, NULL);
		DoSelectMeasurment(pMP, nSel, m_cList, &m_cWaferMap);
	}

	*pResult = 0;
}

void CSetupProp14Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	
	//10032025
	int rowCount = m_cPoint.GetItemCount();
	CMPoint* pMP = FindMPoint(rowCount - 1);
	pMP->MeSet = 1;
	DoMeasurmentSetOne(pMP, rowCount - 1, m_cPoint, &m_cWaferMap);
	//
	
}

void CSetupProp14Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp14Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp14Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
		m_cWaferMap.Redraw();//tushar
	}
}

void CSetupProp14Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		pRcp->UpdateControl(m_cPoint);
	}
}

short CSetupProp14Dlg::sign(long value)
{
	if (value < 0)
		return -1;
	return 1;
}

//[ 18012024 Tushar
void CSetupProp14Dlg::OnBnClickedMeasureCwl()
{
	// Tushar

	plotIntensity(&m_cProfile);
}

void CSetupProp14Dlg::plotIntensity(CChartWnd* pProfile) {
	if (iRun == TRUE) {	
		isOk = false;
		iRun = FALSE;
		displacementIntensityPlot.detach();//
		return;
	}
	else {
		iRun = TRUE;
		isOk = true;
	}
	auto func = [=]() {
		while (isOk==true)
		{
			if (!iRun)
			{
				iRun = FALSE;
				break;
			}
			//short* pBuf = new short[201];
			//for (int i = 0; i < 200; i++)
			//{
			//	pBuf[i] = 0;
			//}
			//pBuf[200] = 100;
			////Tushar
			//short* pBuf2 = new short[201];
			//for (int i = 0; i < 200; i++)
			//{
			//	pBuf2[i] = 0;
			//}
			//pBuf2[200] = 100;
			////
			//[03092024
			int tempSize = (int)buffData;
			short* pBuf = new short[tempSize + 1];
			for (int i = 0; i < tempSize; i++)
			{
				pBuf[i] = 0;
			}
			pBuf[tempSize] = 100;
			//Tushar
			short* pBuf2 = new short[tempSize + 1];
			for (int i = 0; i < tempSize; i++)
			{
				pBuf2[i] = 0;
			}
			pBuf2[tempSize] = 100;
			//

			double displacement[2];
			double intensity[2];

			GetConfocalTotalThickness();
			p413App->pCWL->GetMeasuredValueMFO(0, 1, displacement[0]);
			p413App->pCWL->GetMeasuredValueMFO(0, 2, displacement[1]);
			p413App->pCWL->GetMeasuredValueMFO(1, 1, intensity[0]);
			p413App->pCWL->GetMeasuredValueMFO(1, 2, intensity[1]);

			//Tushar
			if (isOk) {


				if (g->bDualConfocal) {
					if (displacement[0] < 1e-3) {
						// micro meter data
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];
					}
					else if (displacement[0] < 1e-6) {
						// nano data
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];
					}
					else {
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];//check
					}

					if (displacement[1] < 1e-3) {
						pBuf2[(int)(100 + ((-1) * displacement[1] * offsetData))] = intensity[1];
					}
					else if (displacement[1] < 1e-6) {
						pBuf2[(int)(100 + ((-1) * displacement[1] * offsetData))] = intensity[1];
					}
					else {
						pBuf2[(int)(100 + ((-1) * displacement[1] * offsetData))] = intensity[1];
					}

				}
				else {
					if (displacement[0] < 1e-3) {
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];
					}
					else if (displacement[0] < 1e-6) {
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];
					}
					else {
						pBuf[(int)(100 + ((-1) * displacement[0] * offsetData))] = intensity[0];
					}
				}
				//]

				//int len = 201;//03092024
				int len = tempSize + 1;//03092024
				short* pDspBuf = new short[len + 1];
				memcpy(pDspBuf, pBuf, len * sizeof(short));
				
				//Tushar
				short* pDspBuf2 = new short[len + 1];
				memcpy(pDspBuf2, pBuf2, len * sizeof(short));
				if (g->bDualConfocal) {
					pProfile->Set2(pDspBuf, pDspBuf2, len);
					pProfile->Invalidate();
				}
				else {
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				//
			}
			Sleep(150);

		}
	};

	displacementIntensityPlot = thread(func);
	if (iRun == FALSE) {
		displacementIntensityPlot.detach();
	}
}

void CSetupProp14Dlg::OnBnClickedCalibrateCwl()
{
	p413App->pPSwitch->MoveToCWL030prb();
	p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::CWL);
	p413App->pMtr->MoveThinFilmZ(m_ZPos);
	CInputCalibWaferThickness dlg;
	dlg.m_CalibThickness = g->fConfocalConstantK;
	if (dlg.DoModal() == IDOK) {
		g->fConfocalConstantK = dlg.m_CalibThickness;
		p413App->pCWL->SetCALKValue(g->fConfocalConstantK / 1000.0);
		DosUtil.SetConfocalK(g->fConfocalConstantK);
	}
}

void CSetupProp14Dlg::GetConfocalTotalThickness() {
	double chOneVal = 0.0, chTwoVal = 0.0;
	p413App->pCWL->GetMeasuredValueMRO(1, chOneVal);
	p413App->pCWL->GetMeasuredValueMRO(2, chTwoVal);
	chOneVal *= 1000;
	chTwoVal *= 1000;
	float temp = chOneVal + chTwoVal + DosUtil.GetConfocalK();
	CString val, ch1, ch2;
	val.Format(_T("%f"), temp);
	ch1.Format(_T("%f"), chOneVal);
	ch2.Format(_T("%f"), chTwoVal);
	CString thicknessVal = "Thickness: " +val +",  " + "Channel 1: " + ch1 + ",  " + "Channel 2: " + ch2;
	m_cMessage.SetWindowText(thicknessVal);
}


void CSetupProp14Dlg::ChOneDisplamentModeBtn()
{
	p413App->pCWL->SetDisplacementMode(1);
}


void CSetupProp14Dlg::ChTwoDisplacementMode()
{
	p413App->pCWL->SetDisplacementMode(2);
}

// Auto Intensity
// 0 --> OFF Mode
// 1 --> ON Mode
// Setpoint Value for reference = 50 

void CSetupProp14Dlg::ChOneIntensityBtn()
{
	p413App->pCWL->SetAutoIntensity(1,1);
	p413App->pCWL->SetAutoTunningSetpoint(1, 50);
}

void CSetupProp14Dlg::ChTwoIntensityBtn()
{
	p413App->pCWL->SetAutoIntensity(2, 1);
	p413App->pCWL->SetAutoTunningSetpoint(2, 50);
}

void CSetupProp14Dlg::SetChannelOneZeroBtn()
{
	p413App->pCWL->SetZero(1);
}


void CSetupProp14Dlg::SetChannelTwoZeroBtn()
{
	p413App->pCWL->SetZero(2);
}
//]

void CSetupProp14Dlg::OnBnClickedConfocalZHeight()
{
	if (p413App->pMtr) p413App->pMtr->GetThinFilmZPosDev(m_ZPos);
	//pRcp->fThinFilmZ = m_ZPos;
	pRcp->fCWLTopZ = m_ZPos; // Morsalin
	pRcp->fCWLTopZ2 = m_ZPos2; // Morsalin
	pRcp->DieHeight = mDieHeight;
	pRcp->DieWidth = mDieWidth;
	UpdateData(FALSE);
}

void CSetupProp14Dlg::OnKillfocusConfocalZPosEdit()
{
	UpdateData(TRUE);
	//pRcp->fThinFilmZ = m_ZPos;
	pRcp->fCWLTopZ = m_ZPos; // Morsalin
	pRcp->fCWLTopZ2 = m_ZPos2; // Morsalin
	pRcp->DieHeight = mDieHeight;
	pRcp->DieWidth = mDieWidth;
}


void CSetupProp14Dlg::OnBnClickedResponse()
{
	pGDev->justTest();
}

void CSetupProp14Dlg::OnBnClickedPresence()
{
	
}


//[03092024

void CSetupProp14Dlg::OnBnClickedInsert()
{

	//p413App->Global.finalLTV = { 12.4, 34.2, 23.1, 23.4, 1.2, 34.2, 12.3 };
	m_DieHeight.GetWindowText(DieHeight);
	m_DieWidth.GetWindowText(DieWidth);

	mDieHeight = _ttof(DieHeight);
	mDieWidth = _ttof(DieWidth);
	if (p413App->Global.isSqDie)
	{
		if (mDieWidth != mDieHeight)
			m_DieWidth.SetWindowTextA(DieHeight), mDieWidth = mDieHeight;
	}
	//if ((mDieHeight < 10.0 || mDieHeight >40.0) || (mDieWidth < 10.0 || mDieWidth >40.0))
	if ((mDieHeight < 3.0 || mDieHeight >40.0) || (mDieWidth < 3.0 || mDieWidth >40.0))
	{
		m_DieHeight.SetWindowTextA("0");
		m_DieWidth.SetWindowTextA("0");
		mDieHeight = 0.0;
		mDieWidth = 0.0;
		AfxMessageBox("Die size must be between 3.0 to 40.0.", MB_OK);
		return;
	}
	//m_cDiceWafer_V2.Redraw();
	//pRcp->XYPair[2].y = pRcp->XYPair[0].x + bufSize;

	//pRcp->XYPair[1].y = pRcp->XYPair[0].y + offset;
	////pRcp->XYPair[0].x;

	//dX.x = pRcp->XYPair[2].x - pRcp->XYPair[0].x;//x
	//dX.y = pRcp->XYPair[2].y - pRcp->XYPair[0].y;

	//dY.x = pRcp->XYPair[1].x - pRcp->XYPair[0].x;//y
	//dY.y = pRcp->XYPair[1].y - pRcp->XYPair[0].y;
	CString str;


	GenerateMap();
	m_cWaferMap.Redraw();
}
//]


//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::updateConfocalValue() {
	auto f = [=]() {
		while (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor008) {
			CString s;
			double intensity;
			p413App->pCWL->GetMeasuredValueMFO(1, 2, intensity);
			s.Format("%lf", intensity);
			SetDlgItemText(IDC_CWLINTENSITY008DLG14, s);  // Update the intensity text

			double topCWLVal;
			p413App->pCWL->GetMeasuredValueMRO(2, topCWLVal);
			s.Format("%lf", topCWLVal);
			SetDlgItemText(IDC_RELATIVEDISTANCECWL008DLG14, s);  // Set the relative distance
			if (p413App->monitor008) {
				float zPos;
				p413App->pMtr->GetCWLTopZPosDev(zPos);
				s.Format("%lf", zPos /*- p413App->Global.COffZ2*/);
				SetDlgItemText(IDC_ZHEIGHTCWL008DLG14, s);  // Set the Z-height value
			}
			Sleep(200); 
		}
		CString s;
		float zPos;
		p413App->pMtr->GetCWLTopZPosDev(zPos);
		s.Format("%lf", zPos /*- p413App->Global.COffZ2*/);
		SetDlgItemText(IDC_ZHEIGHTCWL008DLG14, s);  // Set the Z-height value
	};
	updateCWLVal = thread(f);
	if (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor008) {
		updateCWLVal.detach();
	}
}
//]


//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::updateConfocalValue030() {
	auto f = [=]() {
		while (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor030) {
			CString s;
			double intensity;
			p413App->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
			s.Format("%lf", intensity);
			SetDlgItemText(IDC_CWLINTENSITY030DLG14, s);  // Update the intensity text

			//[03172025
			double topCWLVal;
			double topCWLVal2;
			p413App->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
			p413App->pCWL2->GetMeasuredValueMRO(2, topCWLVal2);
			double chOneVal, chTwoVal;
			chOneVal = topCWLVal;
			chTwoVal = topCWLVal2;
			chOneVal *= 1000;
			chTwoVal *= 1000;
			double calib = DosUtil.GetConfocalK();
			float temp = chOneVal + chTwoVal + calib;
			CString str;
			str.Format(_T("%.2f"), temp);
			SetDlgItemText(IDC_THICKNESS030, str);  // Set the relative distance
			//]
			s.Format("%lf", topCWLVal);
			SetDlgItemText(IDC_RELATIVEDISTANCECWL030DLG14, s);  // Set the relative distance
			if (p413App->monitor030) {
				float zPos;
				p413App->pMtr->GetCWLTopZPosDev(zPos);
				s.Format("%lf", zPos /*- p413App->Global.COffZ2*/);
				SetDlgItemText(IDC_ZHEIGHTCWL030DLG14, s);  // Set the Z-height value
			}
			Sleep(200);
		}
		CString s;
		float zPos;
		p413App->pMtr->GetCWLTopZPosDev(zPos);
		s.Format("%lf", zPos /*- p413App->Global.COffZ2*/);
		SetDlgItemText(IDC_ZHEIGHTCWL030DLG14, s);  // Set the Z-height value
	};
	updateCWLVal = thread(f);
	if (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor030) {
		updateCWLVal.detach();
	}
}
//]

//[ //  // 12262024 Mahedi Kamal
void CSetupProp14Dlg::updateConfocalValue040() {
	auto f = [=]() {
		while (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor040) {
			CString s;
			double intensity;
			p413App->pCWL2->GetMeasuredValueMFO(1, 2, intensity);
			s.Format("%lf", intensity);
			SetDlgItemText(IDC_CWLINTENSITY040DLG14, s);  // Update the intensity text

			double topCWLVal;
			p413App->pCWL2->GetMeasuredValueMRO(2, topCWLVal);
			s.Format("%lf", topCWLVal);
			SetDlgItemText(IDC_RELATIVEDISTANCECWL040DLG14, s);  // Set the relative distance
			Sleep(2000);
		}
	};
	updateCWLVal = thread(f);
	if (p413App->runningWaferScanCWLThread == TRUE || p413App->monitor040) {
		updateCWLVal.detach();
	}
}
//]

void CSetupProp14Dlg::offAllMonitor() {
	p413App->monitor040 = FALSE;
	p413App->monitor030 = FALSE;
	p413App->monitor008 = FALSE;
	Sleep(500);
}
//[[20250926_Mohir
void CSetupProp14Dlg::OnEnChangeThicknessScanLengthDlg14()
{
	CString str;
	GetDlgItemText(IDC_THICKNESS_SCAN_LENGTH, str);
	p413App->ThicknessScanLength = static_cast<float>(atof(CT2A(str)));
	
}

//]]



//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::OnEnChangeApproxZPosConf008Dlg14()
{
	CString str;
	GetDlgItemText(IDC_APPROXZPOSCONF008DLG14, str);
	p413App-> ZFocusPos008 = static_cast<float>(atof(CT2A(str)));
}
//]
//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::OnEnChangeFocusDevRange008Dlg14()
{
	CString str;
	GetDlgItemText(IDC_FOCUSDEVRANGE008DLG14, str);
	p413App->ZFocusDevRenge008 = static_cast<float>(atof(CT2A(str)));
}
//]
//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::OnBnClickedScanwaferdlg14()
{
	if (p413App->runningWaferScanCWLThread == FALSE) {
		p413App->runningWaferScanCWLThread = TRUE;
		updateConfocalValue();
		AfxBeginThread(C413AppCWL::MoveXYAndCollectCWLData, p413App);
	}
	else {
		AfxMessageBox("wait!! Focus Scan running");
	}
}
//]
//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::OnBnClickedFocus008dlg14()
{
	p413App->monitor008 = FALSE; Sleep(200);
	if (p413App->runningWaferScanCWLThread == FALSE) {
		p413App->runningWaferScanCWLThread = TRUE;
		updateConfocalValue();
		Sleep(100);
		//AfxBeginThread(C413AppCWL::FocusScanCWL, p413App);
		AfxBeginThread(C413AppCWL::QuickFocusScanCWL008, p413App);
		//AfxBeginThread(C413AppCWL::QuickFocusScanCWL008, p413App);
	}
	else {
		AfxMessageBox("wait!! Wafer Scan running");
	}
}
//]
//[ //  Mahedi Kamal 10152024
void CSetupProp14Dlg::OnBnClickedStopalldlg14()
{
	p413App->runningWaferScanCWLThread = FALSE;
}
//]



//[ARIF 3D VIEW STARTS [10152024 3D View] 
void CSetupProp14Dlg::Create3D()
{
	if (m_hPE3)PEdestroy(m_hPE3);
	RECT wndRect, itemRect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_3D_VIEWDLG14)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;
	m_hPE3 = PEcreate(PECONTROL_3D, WS_VISIBLE, &itemRect, m_hWnd, 1001);

	// Enable smoother rotation and zooming //
	PEnset(m_hPE3, PEP_nSCROLLSMOOTHNESS, 6);
	PEnset(m_hPE3, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE3, PEP_nPINCHZOOMSMOOTHNESS, 3);


	// Zoom faster //
	float fZF = 10000.0F;
	PEvset(m_hPE3, PEP_fMOUSEWHEELZOOMFACTOR3D, &fZF, 1);
	PEvset(m_hPE3, PEP_fPINCHZOOMFACTOR3D, &fZF, 1);

	// Enable DegreePrompting, to view rotation, zoom, light location to aid
	// in determining different default values for such properties //
	PEnset(m_hPE3, PEP_bDEGREEPROMPTING, TRUE);
	PEnset(m_hPE3, PEP_nROTATIONSPEED, 68);
	PEnset(m_hPE3, PEP_nROTATIONINCREMENT, PERI_INCBY1);
	PEnset(m_hPE3, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);
	PEnset(m_hPE3, PEP_nROTATIONMENU, PEMC_GRAYED);
	PEnset(m_hPE3, PEP_nDEGREEOFROTATION, 180);

	//Set PlottingMethod //
	PEnset(m_hPE3, PEP_nPLOTTINGMETHOD, 4);  //4 Surface with contours
	//PEnset(m_hPE3, PEP_nCPLOTTINGMETHOD, 3);
	PEnset(m_hPE3, PEP_bALLOWSURFACECONTOUR, TRUE);

	//Axis scale
	PEnset(m_hPE3, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE3, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE3, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);//axis type

	// Axis label
	TCHAR szBufX[] = TEXT("X um");
	PEszset(m_hPE3, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[] = TEXT("Z um");
	PEszset(m_hPE3, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufY[] = TEXT("Y um");
	PEszset(m_hPE3, PEP_szZAXISLABEL, szBufY);

	// Set a light rotation location //
	Point3D p3d;
	p3d.X = 250.50F; p3d.Y = 50.5F; p3d.Z = 50.25F;
	PEvset(m_hPE3, PEP_struct3DXLIGHT0, &p3d, 1);

	// Set eye/camera distance, or Zoom amount //
	float fMyZoom = -25.0F;
	PEvset(m_hPE3, PEP_f3DXZOOM, &fMyZoom, 1);

	// Enable mouse dragging //
	PEnset(m_hPE3, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPE3, PEP_bMOUSEDRAGGINGY, TRUE);

	// Reduce the terrain height with respect to surface area
	float fX = 10.0F;  PEvset(m_hPE3, PEP_fGRIDASPECTX, &fX, 1);
	float fZ = 10.0F;  PEvset(m_hPE3, PEP_fGRIDASPECTZ, &fZ, 1);
	PEnset(m_hPE3, PEP_n3DXFITCONTROLSHAPE, FALSE);

	// Pass Data //
	long Row, Col, o = 0, e, nQ;
	long nTargetRows, nTargetCols;
	e = static_cast<long>(0.0F);

	nTargetRows = static_cast<long>(filterData.size() - 1);
	nTargetCols = static_cast<long>(filterData[0].size() - 1);

	/*nTargetRows = static_cast<long>(1000);
	nTargetCols = static_cast<long>(1000);*/
	nQ = (nTargetRows - 1) * (nTargetCols - 1);
	float* pMyXData = NULL;
	float* pMyYData = NULL;
	float* pMyZData = NULL;

	pMyXData = new float[nQ]; // v9.5
	pMyYData = new float[nQ];
	pMyZData = new float[nQ]; // v9.5

	long rowcounter, colcounter;
	//BOOL foundtargetindex = FALSE;
	rowcounter = 0;
	colcounter = 0;
	//int cnt = 0;

	for (Row = 0; Row < nTargetRows - 1; Row++) {
		for (Col = 0; Col < nTargetCols - 1; Col++) {
			pMyXData[o] = static_cast<float>(Col);
			pMyZData[o] = static_cast<float>(Row);
			pMyYData[o] = filterData[Row][Col];
			o++;
		}
	}

	PEnset(m_hPE3, PEP_nSUBSETS, nTargetRows - 1);
	PEnset(m_hPE3, PEP_nPOINTS, nTargetCols - 1);
	// Perform the actual transfer of data //
	PEvset(m_hPE3, PEP_faXDATA, pMyXData, nQ);
	PEvset(m_hPE3, PEP_faYDATA, pMyYData, nQ);
	PEvset(m_hPE3, PEP_faZDATA, pMyZData, nQ);
	PEnset(m_hPE3, PEP_nDUPLICATEXDATA, PEDD_POINTINCREMENT);  // v9.5
	PEnset(m_hPE3, PEP_nDUPLICATEZDATA, PEDD_SUBSETINCREMENT);  // v9.5

	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;

	//Color Bar [Legend]
	PEnset(m_hPE3, PEP_bSHOWLEGEND, FALSE);
	PEnset(m_hPE3, PEP_nLEGENDLOCATION, PELL_LEFT);

	//color bar and contour
	PEnset(m_hPE3, PEP_nCONTOURCOLORBLENDS, 4); //8 // this must be set before COLORSET, COLORSET ALWAYS LAST
	PEnset(m_hPE3, PEP_nCONTOURCOLORSET, PECCS_GREEN_YELLOW_RED);//PECCS_BLUE_GREEN_RED //PECCS_BLUE_GREEN_YELLOW_RED
	PEnset(m_hPE3, PEP_nSHOWCONTOUR, PESC_NONE);//PESC_BOTTOMLINES
	PEnset(m_hPE3, PEP_nCONTOURLEGENDPRECISION, 1);//set the precision of the legend

	PEnset(m_hPE3, PEP_nAUTOMINMAXPADDING, 0);
	PEnset(m_hPE3, PEP_bSURFACENULLDATAGAPS, TRUE);
	PEnset(m_hPE3, PEP_bCONTOURSTYLELEGEND, TRUE);//TRUE

	//Set various other properties //
	//font
	PEnset(m_hPE3, PEP_bFIXEDFONTS, TRUE);
	PEnset(m_hPE3, PEP_szLABELFONT, 10);
	PEnset(m_hPE3, PEP_nFONTSIZE, PEFS_MEDIUM);


	// Main title and sub title
	TCHAR mainTitle[] = TEXT("");
	TCHAR subTitle[] = TEXT("");
	TCHAR subSubTitle[] = TEXT("");
	PEszset(m_hPE3, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPE3, PEP_szSUBTITLE, subTitle);
	PEvsetcell(m_hPE3, PEP_szaMULTISUBTITLES, 0, subSubTitle);

	PEnset(m_hPE3, PEP_bBITMAPGRADIENTMODE, TRUE);//TRUE
	PEnset(m_hPE3, PEP_nQUICKSTYLE, PEQS_LIGHT_LINE);

	PEnset(m_hPE3, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE3, PEP_bCACHEBMP, TRUE);
	PEnset(m_hPE3, PEP_bFOCALRECT, FALSE);
	PEnset(m_hPE3, PEP_nSHOWBOUNDINGBOX, PESBB_NEVER);
	PEnset(m_hPE3, PEP_nSHADINGSTYLE, PESS_COLORSHADING);

	PEnset(m_hPE3, PEP_nIMAGEADJUSTLEFT, 0);
	PEnset(m_hPE3, PEP_nIMAGEADJUSTRIGHT, 0);
	PEnset(m_hPE3, PEP_nIMAGEADJUSTBOTTOM, 0);

	PEnset(m_hPE3, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE3, PEP_bLABELBOLD, TRUE);

	//! There are different plotting method values for each case of PolyMode  //
	PEnset(m_hPE3, PEP_bALLOWWIREFRAME, FALSE);
	PEnset(m_hPE3, PEP_nCONTOURMENU, PEMC_HIDE);

	// Set export defaults //
	PEnset(m_hPE3, PEP_nDPIX, 600);
	PEnset(m_hPE3, PEP_nDPIY, 600);
	PEnset(m_hPE3, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
	PEnset(m_hPE3, PEP_nEXPORTTYPEDEF, PEETD_PNG);
	PEnset(m_hPE3, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
	TCHAR exportX[] = TEXT("1280");
	TCHAR exportY[] = TEXT("768");
	PEszset(m_hPE3, PEP_szEXPORTUNITXDEF, exportX);
	PEszset(m_hPE3, PEP_szEXPORTUNITYDEF, exportY);
	PEnset(m_hPE3, PEP_nEXPORTIMAGEDPI, 300);
	PEnset(m_hPE3, PEP_bALLOWTEXTEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWEMFEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWWMFEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWSVGEXPORT, FALSE);

	PEnset(m_hPE3, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPE3, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPE3, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	PEnset(m_hPE3, PEP_nCURSORPROMPTSTYLE, 2);
	PEnset(m_hPE3, PEP_nDATAPRECISION, 2);
	PEnset(m_hPE3, PEP_dw3DHIGHLIGHTCOLOR, PERGB(255, 255, 0, 0));

	PEnset(m_hPE3, PEP_nDATASHADOWMENU, PEMC_SHOW);

	PEnset(m_hPE3, PEP_bALLOWANNOTATIONCONTROL, TRUE);
	PEnset(m_hPE3, PEP_nSHOWANNOTATIONTEXTMENU, PEMC_SHOW);
	PEnset(m_hPE3, PEP_bSHOWANNOTATIONTEXT, TRUE);
	PEnset(m_hPE3, PEP_nLINEANNOTATIONTEXTSIZE, 110);

	double d;  	int t;
	//int aCnt = 0;

	PEnset(m_hPE3, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE3, PEP_bSHOWGRAPHANNOTATIONS, TRUE);
	PEnset(m_hPE3, PEP_bLEFTJUSTIFICATIONOUTSIDE, TRUE);
	PEnset(m_hPE3, PEP_bGRAPHANNOTATIONSYMBOLOBSTACLES, TRUE);
	PEnset(m_hPE3, PEP_nGRAPHANNOTATIONTEXTSIZE, 110);
	PEnset(m_hPE3, PEP_bALLOWGRAPHANNOTHOTSPOTS, FALSE);

	// create a custom menu to toggle PEP_nVIEWINGMODE
	TCHAR customText[] = TEXT("");
	PEvsetcell(m_hPE3, PEP_szaCUSTOMMENUTEXT, 0, customText);
	t = PECMS_CHECKED; PEvsetcellEx(m_hPE3, PEP_naCUSTOMMENUSTATE, 0, 0, &t);
	t = PECML_ABOVE_SEPARATOR; PEvsetcell(m_hPE3, PEP_naCUSTOMMENULOCATION, 0, &t);
	PEvsetcell(m_hPE3, PEP_szaMULTISUBTITLES, 1, customText);

	Point3D p3;  // Set a default ViewingAt location to first graph annotation, index 0
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONX, 0, &d);  p3.X = (float)d;
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONY, 0, &d);  p3.Y = (float)d;
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONZ, 0, &d);  p3.Z = (float)d;
	PEvset(m_hPE3, PEP_structVIEWINGAT, &p3, 1);

	PEnset(m_hPE3, PEP_nRENDERENGINE, PERE_DIRECT3D);
	PEnset(m_hPE3, PEP_bFORCE3DXVERTICEREBUILD, TRUE);
	PEnset(m_hPE3, PEP_bFORCE3DXANNOTVERTICEREBUILD, TRUE);
}

void CSetupProp14Dlg::OnBnClickedPlotData()
{
	// TODO: Add your control notification handler code here
	data.clear();
	mnVal = 1000.0F;
	mxVal = -10000.0F;
	int row = 0, col = 0, mnX = -1, mnY = -1;
	CString FilePath;	//Height Data CSV File Path
	//2D/3D DRAWING BY ARIF IN DIFFERENT FUNCTION -> ARIF

	CFileDialog dlg(TRUE, _T(".csv"), NULL, OFN_FILEMUSTEXIST, _T("CSV Files (*.csv)|*.csv||"), this);
	//dlg.m_ofn.lpstrInitialDir = GetLocalPath() + "DATA\\HeightData";
	if (dlg.DoModal() == IDOK) {
		FilePath = dlg.GetPathName();
		std::ifstream file(FilePath);
		std::string line;

		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string value;
			std::vector<float>vec;
			col = 0;
			while (std::getline(lineStream, value, ',')) {
				//if (value == "") {
				//	if (col >= 120 && col <= 150)value = "-17.47";//ARIF ADD for Uwe's plot. 
				//	else value = "-54.00";
				//}
				//if (value == "")value = "0.00";// ARIF ADD FOR MAHEDI
				float val;
				val = static_cast<float>(stof(value));
				vec.push_back(val);
				mxVal = max(mxVal, val);
				if (val < mnVal) {
					mnVal = val;
					mnX = row + 1, mnY = col + 1;
				}

				col++;
			}
			//dataLength += vec.size();
			data.push_back(vec);
			vec.clear();
			row++;

		}
	}

	if (data.size() > 0) {
		filterData = data;
		Create3D();
	}
}
//]

BOOL CSetupProp14Dlg::LineScan(double fiX, double fiY, int ind, bool frwrd)
{
	double sideEdge = 1;
	double endX;


	int skipPoint = 0;
	int stageMoved = -1;
	ULONGLONG tick = GetTickCount64();

	std::thread t1([&]() {
		stageMoved = p413App->pMtr->GotoXY(fiX, fiY);
		}
	);

	std::thread t2([&]() {
		while (stageMoved == -1) {
			double topCWLVal;// = GetAverageValueWithStdDevCheck(p413App);
			p413App->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
			p413App->Global.scanData[ind].push_back(topCWLVal);
			//TRACE("---------------- %d\n", ind);
		}
		if (!frwrd) {
			reverse(p413App->Global.scanData[ind].begin(), p413App->Global.scanData[ind].end());
		}
	});

	t1.join();
	t2.join();

	return TRUE;
}

void saveScanDataToCSV(std::vector<double> scanData[70], const std::string& filename) {
	// std::ofstream outFile(filename); //987654321

	//if (!outFile) {
	//	//std::cerr << "Failed to open the file for writing.\n";
	//	return;
	//}

	//// Iterate over the array of vectors (scanData)
	//for (int i = 0; i < 70; ++i) {
	//	for (size_t j = 0; j < scanData[i].size(); ++j) {
	//		outFile << scanData[i][j];
	//		if (j < scanData[i].size() - 1) {
	//			outFile << ","; // separate values with commas
	//		}
	//	}
	//	outFile << "\n"; // new line for each vector
	//}

	//outFile.close();
	//std::cout << "Data saved to " << filename << "\n";
}

void loadScanDataFromCSV(std::vector<double> scanData[70], const std::string& filename) {
	std::ifstream inFile(filename);

	if (!inFile) {
		//std::cerr << "Failed to open the file for reading.\n";
		return;
	}

	std::string line;
	int i = 0;  // Index to keep track of the vector in scanData

	while (std::getline(inFile, line) && i < 70) {
		std::stringstream ss(line);
		std::string value;
		scanData[i].clear();  // Clear the vector for each new line of data

		while (std::getline(ss, value, ',')) {
			try {
				scanData[i].push_back(std::stod(value)); // convert string to double
			}
			catch (const std::invalid_argument& e) {
				//std::cerr << "Error parsing value: " << value << "\n";
			}
		}
		++i; // Move to the next vector in the array
	}

	inFile.close();
	//std::cout << "Data loaded from " << filename << "\n";
}
#include "..\413\MotorL2.h"
void CSetupProp14Dlg::ExecuteScan()
{
	//float Spd = p413App->pMtr->pMX->Param.Speed;
	//auto f = [=]() {

	//	bool frwrd = true;
	//	int ind = 0;
	//	for (int i = -pRcp->Wp.size; i <= pRcp->Wp.size; i += p413App->Global.scanStep) {
	//		if (p413App->Global.stopScan)
	//			break;
	//		double xSquared = pRcp->Wp.size * pRcp->Wp.size - i * i;
	//		// Calculate intersection points
	//		double x1 = sqrt(xSquared);  // Positive x
	//		double x2 = -sqrt(xSquared); // Negative x

	//		p413App->Global.LineCoordinate.push_back({ x1, i });
	//		if (!frwrd)
	//			swap(x1, x2);

	//		double stX = x1 + p413App->Global.scanOffset;
	//		double stY = i;
	//		double fiX = x2 - p413App->Global.scanOffset;
	//		double fiY = i;
	//		//CMotorParam* pParam;
	//		//pParam = &pMtr->pMEchoTopZ->Param;
	//		//pParam = &pMtr->pMX->Param;
	//		
	//		p413App->pMtr->pSiMc->SetSpeed(1, Spd);
	//		p413App->pMtr->GotoXY(stX, stY);

	//		p413App->pMtr->pSiMc->SetSpeed(1, p413App->Global.scanSpeed);

	//		LineScan(fiX, fiY, ind++, frwrd);
	//		if (frwrd) frwrd = false;
	//		else frwrd = true;
	//		p413App->Global.scanOffset *= (-1);
	//	}
	//	CollectDieInfo();
	//	p413App->pMtr->pSiMc->SetSpeed(1, Spd);
	//	p413App->runningWaferScanCWLThread = FALSE;
	//};

	//AreaScanThread = thread(f);
	//AreaScanThread.detach();
	
}

void CSetupProp14Dlg::OnBnClickedScan()
{
	double qq = p413App->Global.scanSpeed;
	double val;
	p413App->pCWL2->GetMeasuredValueMRO(1, val);
	CString st;
	st.Format("%lf", val);
	AfxMessageBox(st);

	BOOL ww = p413App->Global.m_LSFOn;
	BOOL www = pRcp->m_LSFOn;
	
	ww = p413App->Global.m_LSFOn;
	www = pRcp->m_LSFOn;



}
struct Point
{
	double x, y;
} ;

struct Die
{
	Point DL, UR;
};


Point P;
Die DieData[1000];

void CSetupProp14Dlg::CalLTV()
{
	/*p413App->Global.noOfDie = 1;
	DieData[0].DL.x = -2.0;
	DieData[0].DL.y = -2.0;
	DieData[0].UR.x = 8.0;
	DieData[0].UR.y = 8.0;*/


	//vector<double> LTV;
	p413App->Global.finalLTV.clear();
	p413App->Global.notMeasured.clear();
	for (int k = 0; k < p413App->Global.noOfDie; k++) {
		vector<int> selectedLines;
		double mx = -100.0;
		double mn = 100.0;
		int noOfLine = p413App->Global.LineCoordinate.size();
		for (int i = 1; i < noOfLine - 1; i++) {
			double LineY = p413App->Global.LineCoordinate[i].second;
			double DieLow = DieData[k].DL.y;
			double DieHigh = DieData[k].UR.y;
			if (LineY >= DieLow && LineY <= DieHigh) {
				selectedLines.push_back(i);
			}
		}

		noOfLine = selectedLines.size();
		for (int i = 0; i < noOfLine; i++) {
			int ind = selectedLines[i];
			double len = abs(p413App->Global.LineCoordinate[ind].first) * 2;
			double dataPerLine = len / (double)p413App->Global.scanData[ind].size();


			/*int stInd = round(DieData[k].DL.x + abs(p413App->Global.coordinate[ind].first)) / dataPerLine;
			int fiInd = round(DieData[k].UR.x + abs(p413App->Global.coordinate[ind].first)) / dataPerLine;*/

			double leftDist = abs(-p413App->Global.LineCoordinate[ind].first - DieData[k].DL.x);
			double rightDist = abs(-p413App->Global.LineCoordinate[ind].first - DieData[k].UR.x);
			
			int stInd = (p413App->Global.scanData[ind].size() * leftDist) / len;
			int fiInd = (p413App->Global.scanData[ind].size() * rightDist) / len;



			for (int j = stInd + 30; j < fiInd - 30; j++) {
				int sz = p413App->Global.scanData[ind].size();
				if (j < sz && j >= 0) {
					mn = min(mn, p413App->Global.scanData[ind][j]);
					mx = max(mx, p413App->Global.scanData[ind][j]);
				}
			}

		}
		if (mx == -100.0 || mn == 100.0)
			p413App->Global.finalLTV.push_back(-2.0);
		else 
			p413App->Global.finalLTV.push_back(mx - mn);
	}
}

void CSetupProp14Dlg::OnBnClickedBtnmeasure2()
{

}


void CSetupProp14Dlg::OnBnClickedSaveScanData()
{
	//SaveToCSV();
}



void CSetupProp14Dlg::ShowDieValue()
{
	//p413App->Global.ifWithVal = true;
	GenerateMap();
}

void removeOutliersByAverageLocal(int numberOfLine) {
	if (p413App->Global.bAborting)
		return;
	double sum;
	for (int i = 2; i < numberOfLine; i++) {
		sum = 0;
		for (int j = 0; j < p413App->Global.scanData[i].size(); j++) {
			sum += p413App->Global.scanData[i][j];
		}

		sum /= p413App->Global.scanData[i].size();

		std::vector<std::pair<int, double>>difference;
		for (int j = 0; j < p413App->Global.scanData[i].size(); j++) {
			difference.push_back({ j, sum - p413App->Global.scanData[i][j] });
		}
		// Sort by the difference in descending order
		std::sort(difference.begin(), difference.end(), [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
			return abs(a.second) > abs(b.second);
			});
		// Calculate the number of elements to remove
		float percentageToRemove = 0.2;
		size_t countToRemove = static_cast<size_t>(p413App->Global.scanData[i].size() * percentageToRemove);
		// Mark the indices to remove

		std::vector<bool> toRemove(p413App->Global.scanData[i].size(), false);
		for (size_t idx = 0; idx < countToRemove; ++idx) {
			toRemove[difference[idx].first] = true;
		}

		std::vector<double>trimData;
		for (size_t id = 0; id < p413App->Global.scanData[i].size(); ++id) {
			if (!toRemove[id]) {
				trimData.push_back(p413App->Global.scanData[i][id]);
			}
		}
		p413App->Global.scanData[i] = trimData;
	}
}



void RemoveZeroLocal(int numOfLine)
{
	if (p413App->Global.bAborting)
		return;
	int dx[] = { -1, -1, -1,  0,  1,  1,  1,  0 };
	int dy[] = { -1,  0,  1,  1,  1,  0, -1, -1 };
	for (int i = 2; i < numOfLine; i++)
	{
		int sz = p413App->Global.scanData[i].size();
		for (int j = 0; j < sz; j++)
		{
			if (p413App->Global.scanData[i][j] == 0)
			{
				double sum = 0.0;
				int cnt = 0;
				for (int k = 0; k < 8; k++)
				{
					int curX = i + dx[k];
					int curY = j + dy[k];
					TRACE("%d %d %d %d\n", i, j, curX, curY);
					if (curY >= 0 && curX >= 0 && curX < numOfLine && curY < p413App->Global.scanData[curX].size())
					{
						if (p413App->Global.scanData[curX][curY] != 0) {
							sum += p413App->Global.scanData[curX][curY];
							cnt++;
						}
					}
				}
				if (cnt != 0) {
					p413App->Global.scanData[i][j] = sum / cnt;
				}
			}
		}
		TRACE("%d\n", i);
	}
}





void CSetupProp14Dlg::OnBnClickedRun()
{	
	//FullWaferScan(pRcp);
	double val;
	p413App->pCWL2->GetMeasuredValueMRO(2, val);
	
}

//[10122024
void CSetupProp14Dlg::GenerateMap() {

	// [ 01012025 MORSALIN open DieMap on load 
	if (p413App->Global.isNew && mDieHeight == 0) {
		mDieHeight = 500;
		mDieWidth = 500;
	}
	// ]
	m_cDiceWafer_V2.ClearCells();
	int idx = 1;
	p413App->Global.m_bIncludeOn ;
	float x, y, bx, by, Ax, Ay;
	m_Radius = pRcp->Wp.size / 2;

	// Anchor point [6/19/2013 Yuen]
	Ax = p413App->Global.AnchorX;
	Ay = p413App->Global.AnchorY;
	

	//dX.x = (pRcp->XYPair[2].x - pRcp->XYPair[0].x);
	dX.x = mDieWidth;
	dY.y = mDieHeight;
	dX.y = 0.00;
	//dY.y = (pRcp->XYPair[1].y - pRcp->XYPair[0].y);
	dY.x = 0.00;

	if (dX.x <= 2) { // 11102025 MORSALIN
		return;
	}
	if (dY.y <= 2) { // 11102025 MORSALIN
		return;
	}

	short szX = 50;
	short szY = 50;
	for (int j = -szY; j <= szY; j++)  // y direction [3/10/2011 FSMT]
	{
		for (int i = -szX; i <= szX; i++) // x direction [3/10/2011 FSMT]
		{
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
			BOOL bFirst = FALSE;
			if (dist1 <= m_Radius) {
				//bFirst = TRUE;
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
				if (!p413App->Global.m_bIncludeOn && (cnt != 4)) {
					continue;
				}

				float x1 = 00.00, y1 = 0.00;// bottom left corner
				float x2 = 0.00, y2 = 0.00;// top right corner
				x1 = bx;
				//y1 = by - dY.y;
				y1 = by;
				x2 = bx + dX.x;
				y2 = by + dY.y;

				CString rcpName = "";
				// send n = -999 , ltvIdx = -1 ,stirIdx = -1, precision -1 to see deafult dice map, 
 				m_cDiceWafer_V2.AddCellWithData(bx, by, dX.x, dY.y, -999, m_Radius,-1,-1, rcpName,-1); 

				idx++;
			}
		}
	}
	m_cDiceWafer_V2.RedrawWithData();
	
}
//]


void CSetupProp14Dlg::OnStnClickedDiceMap()
{
	float fX, fY;
	GetCursorPos(&coord);
	m_cDiceWafer_V2.GetCoor(coord, fX, fY);
	m_cDiceWafer_V2.Locate(fX, fY);
	// 	m_cDiceWafer.Redraw();

	//CString str;
	//str.Format("(%d,%d)  (%.3f,%.3f)", coord.x, coord.y, fX, fY);
	//m_cCoor.SetWindowText(str);
}

//
//void CSetupProp14Dlg::OnBnClickedRadio1()
//{
//	m_Radius = 75.00;
//	p413App->Global.radius = m_Radius;
//}
//
//
//void CSetupProp14Dlg::OnBnClickedRadio2()
//{
//	m_Radius = 100.00;
//	p413App->Global.radius = m_Radius;
//}


void CSetupProp14Dlg::OnBnClickedUpdateIni()
{
	p413App->Global.LoadLTV_STIR_Options();
	// [Morsalin
	/*m_ScanStep.GetWindowTextA(ScanStep);
	m_ScanOffset.GetWindowTextA(ScanOffset);
	m_ScanSpeed.GetWindowTextA(ScanSpeed);
	p413App->Global.scanStep = atof(ScanStep);
	p413App->Global.scanOffset = atof(ScanOffset);
	p413App->Global.scanSpeed = atof(ScanSpeed);*/
	// ]
}


void CSetupProp14Dlg::OnBnClickedStopScan()
{
	p413App->Global.stopScan = true;
}


void CSetupProp14Dlg::OnBnClickedCreatepoints()
{
	if (pRcp) {
		float dieSz = pRcp->Wp.size;
		pRcp->Clear();
		pRcp->Wp.size = dieSz;
	}
	m_DieHeight.GetWindowText(DieHeight);
	m_DieWidth.GetWindowText(DieWidth);
	p413App->Global.DieHeight = _ttof(DieHeight);
	p413App->Global.DieWidth = _ttof(DieWidth);
	if (p413App->Global.isSqDie)
	{
		if (p413App->Global.DieWidth != p413App->Global.DieHeight)
			m_DieWidth.SetWindowTextA(DieHeight), p413App->Global.DieWidth = p413App->Global.DieHeight;
	}
	/*if ((p413App->Global.DieHeight < 10.0 || p413App->Global.DieHeight >40.0) || (p413App->Global.DieWidth < 10.0 || p413App->Global.DieWidth > 40.0))
	{
		AfxMessageBox("Die size must be between 10.0 to 40.0.", MB_OK);
		return;
	}*/

	if ((p413App->Global.DieHeight < 3.0 || p413App->Global.DieHeight >40.0) || (p413App->Global.DieWidth < 3.0 || p413App->Global.DieWidth > 40.0))
	{
		AfxMessageBox("Die size must be between 3.0 to 40.0.", MB_OK);
		return;
	}


	if (pRcp->gridpatt.bClearData) {
		if (pRcp->gridpatt.pPatt) {
			pRcp->gridpatt.pPatt->ClearData();
		}
	}
	float fRadius = 100.0;
	//fRadius *= 150.0 / 100.f;
	fRadius *= pRcp->Wp.size / 100.f;
	p413App->Global.forWaferScan = true;
	pRcp->gridpatt.Generate3(fRadius, pRcp->Wp.size / 2.0, p413App->Global.m_bIncludeOn);
	p413App->Global.forWaferScan = true;
	m_cWaferMap.Redraw();
	pRcp->UpdateControl(m_cPoint);

	//[03142025_3

	int rowCount = m_cPoint.GetItemCount();
	CMPoint* pMP = FindMPoint(rowCount - 1);
	pMP->MeSet = 1;
	DoMeasurmentSetOneAllOnce(pMP, rowCount - 1, m_cPoint, &m_cWaferMap);
	//
}

// [[[ Mahedi Kamal 12120224
void CSetupProp14Dlg::OnChangeRoughStep()
{
	CString strValue;
	GetDlgItemText(IDC_ROUGHSTEP, strValue);  // Get value as string

	// Convert the string to an integer
	float value = atof(strValue);  // _ttoi is a safe way to convert CString to int

	float minRange = 0.005;
	float maxRange = 0.1;

	/*if (value < minRange || value > maxRange)
	{
		AfxMessageBox(_T("The value must be between 0.005 and 0.1"), MB_ICONWARNING);
	}
	else
	{*/
		p413App->RoughStepSizeCWL = value;
		pRcp->RoughStepSizeCWL_rcp = value;
	//}
}


void CSetupProp14Dlg::OnChangeRoughLineLen()
{
	CString strValue;
	GetDlgItemText(IDC_ROUGHLINELEN, strValue);  // Get value as string

	// Convert the string to an integer
	float value = atof(strValue);  // _ttoi is a safe way to convert CString to int

	float minRange = 0.5;
	float maxRange = 3;

	/*if (value < minRange || value > maxRange)
	{
		AfxMessageBox(_T("The value must be between 0.5 and 3.0"), MB_ICONWARNING);
	}
	else
	{*/
		p413App->RoughScanLineLenCWL = value;
		pRcp->RoughScanLineLenCWL_rcp = value;
	//}
}


void CSetupProp14Dlg::OnBnClickedShowroughcwl()
{
	GetDlgItem(IDC_ROUGHVALCWL)->SetWindowText("");
	GetDlgItem(IDC_ROUGHVALCWLA)->SetWindowText("");
	p413App->Yld(500);
	p413App->monitor008 = FALSE; Sleep(200);

	float roughRa, roughRms;
	float X, Y;
	p413App->pMtr->GetXYPos(&X, &Y);
	
	p413App->ReadCWLData(X, Y, roughRa, roughRms);
	p413App->pMtr->MoveAXY(X, Y);
	CString strValue;
	//20250306 ========================================
	if (p413App->IsNmCWL) {
		if (p413App->IsRaCWL) {
			strValue.Format(_T("%.2f"), roughRa);
			SetDlgItemText(IDC_ROUGHVALCWL, strValue);

			
			strValue.Format(_T("%.2f"), roughRa * 10.0);
			SetDlgItemText(IDC_ROUGHVALCWLA, strValue);
		}
		else {
			strValue.Format(_T("%.2f"), roughRms);
			SetDlgItemText(IDC_ROUGHVALCWL, strValue);

			strValue.Format(_T("%.2f"), roughRms * 10.0);
			SetDlgItemText(IDC_ROUGHVALCWLA, strValue);
		}
	}
	else {
		if (p413App->IsRaCWL) {
			strValue.Format(_T("%.2f"), roughRa/10.0);
			SetDlgItemText(IDC_ROUGHVALCWL, strValue);

			strValue.Format(_T("%.2f"), roughRa);
			SetDlgItemText(IDC_ROUGHVALCWLA, strValue);
		}
		else {
			strValue.Format(_T("%.2f"), roughRms/10.0);
			SetDlgItemText(IDC_ROUGHVALCWL, strValue);

			strValue.Format(_T("%.2f"), roughRms);
			SetDlgItemText(IDC_ROUGHVALCWLA, strValue);
		}
	}
	//====================================================
	

}


void CSetupProp14Dlg::OnClickedRms()
{
	p413App->IsRaCWL = FALSE;
	pRcp->bIsRaCWL_rcp = FALSE;
	CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
	pRadioRms->SetCheck(BST_CHECKED);  // Set it as selected

	
	CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
	pRadioRa->SetCheck(BST_UNCHECKED);  // Deselect the radio button


}


void CSetupProp14Dlg::OnClickedRa()
{
	p413App->IsRaCWL = TRUE;
	pRcp->bIsRaCWL_rcp = TRUE;
	CButton* pRadioRms = (CButton*)GetDlgItem(IDC_RMSROUGHCWL);
	pRadioRms->SetCheck(BST_UNCHECKED);  // Set it as selected


	CButton* pRadioRa = (CButton*)GetDlgItem(IDC_RAROUGHCWL);
	pRadioRa->SetCheck(BST_CHECKED);  // Deselect the radio button
}
//]]]

void CSetupProp14Dlg::OnBnClickedIncludeCheck()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_INCLUDE_CHECK);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.m_bIncludeOn = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.m_bIncludeOn = FALSE;

		}
	}
}


void CSetupProp14Dlg::OnBnClickedScanFilter()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_SCAN_FILTER);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.m_LSFOn = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.m_LSFOn = FALSE;

		}
	}
}
// [ 10312025_1 MORSALIN
void CSetupProp14Dlg::OnBnClickedLTVMin()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_LTVMin);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.r_LTVMin = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.r_LTVMin = FALSE;

		}
	}
}

void CSetupProp14Dlg::OnBnClickedLTVMax()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_LTVMax);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.r_LTVMax = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.r_LTVMax = FALSE;

		}
	}
}

void CSetupProp14Dlg::OnBnClickedLTVAvg()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_LTVAvg);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.r_LTVAvg = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.r_LTVAvg = FALSE;

		}
	}
}

void CSetupProp14Dlg::OnBnClickedLTV()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_LTV);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.r_LTV = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.r_LTV = FALSE;

		}
	}
}

void CSetupProp14Dlg::OnBnClickedSTIR()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_STIR);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			p413App->Global.r_STIR = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			p413App->Global.r_STIR = FALSE;

		}
	}
}
// ]


void CSetupProp14Dlg::OnBnClickedFocus030dlg14()
{
	p413App->monitor030 = FALSE; Sleep(200);
	if (p413App->runningWaferScanCWLThread == FALSE) {
		p413App->runningWaferScanCWLThread = TRUE;
		updateConfocalValue030();
		Sleep(100);
		//AfxBeginThread(C413AppCWL::FocusScanCWL030, p413App);
		AfxBeginThread(C413AppCWL::QuickFocusScanCWL030, p413App);
	}
	else {
		AfxMessageBox("wait!! Wafer Scan running");
	}
}


void CSetupProp14Dlg::OnEnChangeApproxzposconf030dlg14()
{
	CString str;
	GetDlgItemText(IDC_APPROXZPOSCONF030DLG14, str);
	p413App->ZFocusPos030 = static_cast<float>(atof(CT2A(str)));
}


void CSetupProp14Dlg::OnEnChangeFocusdevrange030dlg14()
{
	CString str;
	GetDlgItemText(IDC_FOCUSDEVRANGE030DLG14, str);
	p413App->ZFocusDevRenge030 = static_cast<float>(atof(CT2A(str)));
}


void CSetupProp14Dlg::OnBnClickedMonitor008()
{
	if (p413App->monitor008 == FALSE && p413App->runningWaferScanCWLThread == FALSE) {
		p413App->monitor008 = TRUE;
		updateConfocalValue();
	}
	else {
		p413App->monitor008 = FALSE;
	}
}


void CSetupProp14Dlg::OnBnClickedMonitor030()
{
	if (p413App->monitor030 == FALSE && p413App->runningWaferScanCWLThread == FALSE) {
		p413App->monitor030 = TRUE;
		updateConfocalValue030();
	}
	else {
		p413App->monitor030 = FALSE;
	}
}

void CSetupProp14Dlg::OnBnClickedMonitor040()
{
	if (p413App->monitor040 == FALSE && p413App->runningWaferScanCWLThread == FALSE) {
		p413App->monitor040 = TRUE;
		updateConfocalValue040();
	}
	else {
		p413App->monitor040 = FALSE;
	}
}

void CSetupProp14Dlg::OnBnClickedCamerasettingdlg14()
{
	p413App->CameraSetting(m_cVideo);
}

long CSetupProp14Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

void CSetupProp14Dlg::OnBnClickedAdd()
{
	mBufferData.GetWindowText(sBuffData);
	mOffsetData.GetWindowText(sOffsetData);

	buffData = _ttof(sBuffData);
	offsetData = _ttof(sOffsetData);
}


long CSetupProp14Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp14Dlg::OnBnClickedLocations2dlg14()
{
	p413App->LocationProp(g->RcpSetup, m_hWnd);
	
	RecipeToLocal();
	m_cWaferMap.Redraw();
}
//[03142025_1
void CSetupProp14Dlg::OnRefreshui() {
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}
//]


void CSetupProp14Dlg::OnBnClickedTakepicture2dlg14()
{
	m_cVideo.FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d"));
	m_cVideo.FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S"), int(rand() / float(RAND_MAX) * 100));
	m_cVideo.bStoreComposite = TRUE;
	while (m_cVideo.bStoreComposite) {
		Sleep(80);
	}
}


void CSetupProp14Dlg::OnBnClickedRoughScan()
{
	if (p413App->pMtr)
	{
		p413App->pMtr->GotoXY(0, 0);
	}
	std::ofstream outFile("D:\\RoughScan\\Data.csv");  //987654321
	double CWLval;
	
	if (p413App->pCWL)
	{
		for (int i = 0;i < 2000;i++)
		{
			p413App->pCWL->GetMeasuredValueMRO(1, CWLval);
			outFile << CWLval << ",";  //987654321
		}
	}
	outFile.close(); //987654321
}

//[[20241217
long CSetupProp14Dlg::OnAddpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

long CSetupProp14Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}
//]]

// [ MORSALIN//01022024
void CSetupProp14Dlg::OnBnClickedConfocalTopMotor()
{
	// TODO: Add your control notification handler code here
	float val;
	if (p413App->pMtr)
		p413App->pMtr->GetCWLTopZPosDev(val);
	CString st;
	st.Format("%f", val);
	m_zPos030.SetWindowTextA(st);
}


void CSetupProp14Dlg::OnBnClickedZposrough()
{
		// TODO: Add your control notification handler code here
	float val;
	if (p413App->pMtr)
		p413App->pMtr->GetCWLTopZPosDev(val);
	CString st;
	st.Format("%f", val);
	m_zPos008.SetWindowTextA(st);
}
// ]

//[01022025
void CSetupProp14Dlg::OnMark() {
	if (MarkThisPoint()) {
		RecipeToLocal();
	}
}
//]


int CSetupProp14Dlg::calculatePowerOf2(int value) {
	int pow = 0;
	while (value >= 2) {
		value /= 2;
		pow++;
	}
	return pow;
}
int CSetupProp14Dlg::calculateValueofPow2(int pow) {
	int value = 1;
	for (int i = 1; i <= pow; i++) {
		value *= 2;
	}
	return value;
}
void CSetupProp14Dlg::SetCWLMovingAvg008()
{
	CString str;
	GetDlgItemText(IDC_CWL008VALUESET, str);
	int pow = max(static_cast<float>(atoi(CT2A(str))), 0);
	//pow = min(pow, 9);
	pow = calculatePowerOf2(pow);

	p413App->pCWL->SetMovingAverage(1, pow);
}


void CSetupProp14Dlg::SetCWLMovingAvg030()
{
	CString str;
	GetDlgItemText(IDC_CWL030VALUESET, str);
	int pow = max(static_cast<float>(atoi(CT2A(str))), 0);
	//pow = min(pow, 9);
	pow = calculatePowerOf2(pow);

	p413App->pCWL2->SetMovingAverage(1, pow);
}


void CSetupProp14Dlg::SetCWLMovingAvg040()
{
	CString str;
	GetDlgItemText(IDC_CWL040VALUESET, str);
	int pow = max(static_cast<float>(atoi(CT2A(str))), 0);
	//pow = min(pow, 9);
	pow = calculatePowerOf2(pow);

	p413App->pCWL2->SetMovingAverage(2, pow);
}


void CSetupProp14Dlg::OnBnClickedRecipeAutofocus()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_RECIPE_AUTOFOCUS);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			pRcp->bIsAutoFocus008 = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			pRcp->bIsAutoFocus008 = FALSE;
		}
	}
}



//20250306 ARIF ========
void CSetupProp14Dlg::OnBnClickedRoughcwlnm()
{
	p413App->IsNmCWL = TRUE;
	pRcp->bIsNmCWL = TRUE;
	CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
	pRadioNm->SetCheck(BST_UNCHECKED);  // Set it as selected


	CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
	pRadioA->SetCheck(BST_CHECKED);  // Deselect the radio button
}

void CSetupProp14Dlg::OnBnClickedRoughcwla()
{
	p413App->IsNmCWL = FALSE;
	pRcp->bIsNmCWL = FALSE;
	CButton* pRadioNm = (CButton*)GetDlgItem(IDC_ROUGHCWLA);
	pRadioNm->SetCheck(BST_CHECKED);  // Set it as selected


	CButton* pRadioA = (CButton*)GetDlgItem(IDC_ROUGHCWLNM);
	pRadioA->SetCheck(BST_UNCHECKED);  // Deselect the radio button
}

// ============






//[10032025_3

void CSetupProp14Dlg::OnEnChangeSearchStep()
{
	CString str;
	GetDlgItemText(IDC_SRH_STEP, str);
	searchStepSize = _ttof(str);
	pRcp->stepSrhSize = searchStepSize;
}


void CSetupProp14Dlg::OnEnChangeThkSearch()
{
	CString str;
	GetDlgItemText(IDC_THKSRH, str);
	iThkNum = _ttoi(str);
	pRcp->thkSearchNum = iThkNum;
}


void CSetupProp14Dlg::OnEnChangeWarpSearch()
{
	
	CString str;
	GetDlgItemText(IDC_WARPSRH, str);
	iWarpNum = _ttoi(str);
	pRcp->warpSearchNum = iWarpNum;
}
//]
// [ 09232025_Shamim
void CSetupProp14Dlg::OnBnClickedThicknessFirstScan() {
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_THK_FIRST_SCAN);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			pRcp->ThicknessWithScan = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			pRcp->ThicknessWithScan = FALSE;
		}
	}
}
//]
////03192025
void CSetupProp14Dlg::OnBnClickedCalibAutofocus()
{
	CButton* pCheckbox = (CButton*)GetDlgItem(IDC_CALIB_AUTOFOCUS);
	if (pCheckbox) {
		int state = pCheckbox->GetCheck();
		if (state == BST_CHECKED)
		{
			
			pRcp->isCalibrationAutoFocusNeeded = TRUE;
		}
		else if (state == BST_UNCHECKED)
		{
			pRcp->isCalibrationAutoFocusNeeded = FALSE;

		}
	}
}
//


//[ 09242025
void CSetupProp14Dlg::OnBnClickedThkScanTest()
{
	double Val030, Val040;
	float X = 0, Y = 0;
	p413App->pMtr->GetXYPos(&X, &Y);
	p413App->Global.saveDataWhileThkLineScan = TRUE;
	for (int i = 0; i < 10;i++) {
		p413App->GetCWL_030_040_AvgVal(X, Y, Val030, Val040);
	}
	p413App->Global.saveDataWhileThkLineScan = FALSE;
}
//]

void CSetupProp14Dlg::OnBnClickedGetCwlMovingavg008()
{
	int mAvg008 = p413App->pCWL->GetMovingAverage(1);

	mAvg008 = calculateValueofPow2(mAvg008);

	CString str;
	str.Format(_T("%d"), mAvg008);
	GetDlgItem(IDC_MOVINGANG_008)->SetWindowText(str);
	
}


void CSetupProp14Dlg::OnBnClickedGetCwlMovingavg030()
{
	int mAvg030 = p413App->pCWL2->GetMovingAverage(1);
	mAvg030 = calculateValueofPow2(mAvg030);
	CString str;
	str.Format(_T("%d"), mAvg030);
	GetDlgItem(IDC_MOVINGANG_030)->SetWindowText(str);
}


void CSetupProp14Dlg::OnBnClickedGetCwlMovingavg040()
{
	int mAvg040 = p413App->pCWL2->GetMovingAverage(2);
	mAvg040 = calculateValueofPow2(mAvg040);
	CString str;
	str.Format(_T("%d"), mAvg040);
	GetDlgItem(IDC_MOVINGANG_040)->SetWindowText(str);
}
