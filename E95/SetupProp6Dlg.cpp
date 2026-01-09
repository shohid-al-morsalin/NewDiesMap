// SetupProp6.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "SetupProp6Dlg.h"

#include "413App.h"
#include "..\413\MotorL2.h"

#include "AdvanceSetup.h"
#include "AdvanceSetup6.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "SiCamSettingDlg.h"
#include "Alignment.h"

#include "..\413\SIBase.h"
#include "..\SYS\CUMMsg.h"
#include "..\413\MotorS2.h"

#define IDT_REFRESH	9999
#define IDT_START	9998

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp6Dlg dialog
CSetupProp6Dlg* pSP6Dlg = NULL;

CSetupProp6Dlg::CSetupProp6Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp6Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp6Dlg)
	m_AccEchoTop = 0.0f;
	m_AccRough = 0.0f;
	m_AccX = 0.0f;
	m_AccP = 0.0f;
	m_CurEchoTop = 0.0f;
	m_CurRough = 0.0f;
	m_CurX = 0.0f;
	m_CurP = 0.0f;
	m_SpdEchoTop = 0.0f;
	m_SpdRough = 0.0f;
	m_SpdX = 0.0f;
	m_SpdP = 0.0f;
	m_ScaleEchoTop = 0.0;
	m_ScaleRough = 0.0;
	m_ScaleX = 0.0;
	m_ScaleP = 0.0;
	m_PolarityEchoTop = FALSE;
	m_PolarityRough = FALSE;
	m_PolarityX = FALSE;
	m_PolarityP = FALSE;
	m_HomeEchoTop = 0;
	m_HomeRough = 0;
	m_HomeX = 0;
	m_HomeP = 0;
	m_MaxEchoTop = 0;
	m_MaxRough = 0;
	m_MaxX = 0;
	m_MaxP = 0;
	m_MinEchoTop = 0;
	m_MinRough = 0;
	m_MinX = 0;
	m_MinP = 0;
	m_CurY = 0.0f;
	m_AccY = 0.0f;
	m_HomeY = 0;
	m_MaxY = 0;
	m_MinY = 0;
	m_PolarityY = FALSE;
	m_ScaleY = 0.0f;
	m_SpdY = 0.0f;
	m_OffsetEchoTop = 0;
	m_OffsetRough = 0;
	m_OffsetX = 0;
	m_OffsetY = 0;
	m_OffsetP = 0;
	m_XBacklash = 0.0f;
	m_YBacklash = 0.0f;
	m_ZBacklash = 0.0f;
	m_CXOffset = 0.0f;
	m_CYOffset = 0.0f;
	//}}AFX_DATA_INIT

	pSP6Dlg = this;

	bBusy = FALSE;

	radius = 150.0f;
	XFac = YFac = EchoTopZFac = RoughZFac = 1;
	m_cVideo.Name = "Setup6";

	// [05142024 ZHIMING
	m_AccEchoBottom = 0.0f;
	m_CurEchoBottom = 0.0f;
	m_SpdEchoBottom = 0.0f;
	m_HomeEchoBottom = 0.0f;
	m_ScaleEchoBottom = 0.0;
	m_PolarityEchoBottom = 0;
	m_MaxEchoBottom = 0;
	m_MinEchoBottom = 0;
	m_OffsetEchoBottom = 0;
	EchoBottomZFac = 1;
	// ]

	// [ 06112024/MORSALIN
	m_AccCWL = 0.0f;
	m_CurCWL = 0.0f;
	m_SpdCWL = 0.0f;
	m_HomeCWL = 0.0f;
	m_ScaleCWL = 0.0;
	m_PolarityCWL = 0;
	m_MaxCWL = 0;
	m_MinCWL = 0;
	m_OffsetCWL = 0;
	CWLZFac = 1;
	// ]
}

void CSetupProp6Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp6Dlg)
	DDX_Control(pDX, IDC_RIGHTP, m_cRightP);
	DDX_Control(pDX, IDC_LEFTP, m_cLeftP);
	DDX_Control(pDX, IDC_RECONNECT, m_cReconnect);
	DDX_Control(pDX, IDC_CENTEROFFSET, m_cCOffset);
	DDX_Control(pDX, IDC_POSZ2, m_cPosRoughZ);
	DDX_Control(pDX, IDC_POSZ1, m_cPosEchoTopZ);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_LOGMSG, m_cLogMsg);
	DDX_Control(pDX, IDC_OFFSET5, m_cOffsetP);
	DDX_Control(pDX, IDC_POLARITY4, m_cPolarityP);
	DDX_Control(pDX, IDC_SCALE4, m_cScaleP);
	DDX_Control(pDX, IDC_CUR4, m_cCurP);
	DDX_Control(pDX, IDC_SPD4, m_cSpdP);
	DDX_Control(pDX, IDC_ACC4, m_cAccP);
	DDX_Control(pDX, IDC_OFFSET2, m_cRoughOffset);
	DDX_Control(pDX, IDC_POLARITY2, m_cRoughPolarity);
	DDX_Control(pDX, IDC_SCALE2, m_cRoughScale);
	DDX_Control(pDX, IDC_CUR2, m_cRoughCur);
	DDX_Control(pDX, IDC_SPD2, m_cRoughSpd);
	DDX_Control(pDX, IDC_ACC2, m_cRoughAcc);
	DDX_Control(pDX, IDC_DOWNZ2, m_cDownRoughZ);
	DDX_Control(pDX, IDC_UPZ2, m_cUpRoughZ);
	DDX_Control(pDX, IDC_OFFSET1, m_cEchoTopOffset);
	DDX_Control(pDX, IDC_POLARITY1, m_cEchoTopPolarity);
	DDX_Control(pDX, IDC_SCALE1, m_cEchoTopScale);
	DDX_Control(pDX, IDC_CUR1, m_cEchoTopCur);
	DDX_Control(pDX, IDC_SPD1, m_cEchoTopSpd);
	DDX_Control(pDX, IDC_ACC1, m_cEchoTopAcc);
	DDX_Control(pDX, IDC_DOWNZ1, m_cDownEchoTopZ);
	DDX_Control(pDX, IDC_UPZ1, m_cUpEchoTopZ);
	DDX_Control(pDX, IDC_CAMERASETTING4, m_cCameraSetting);
	DDX_Control(pDX, IDC_RESET4, m_cResetP);
	DDX_Control(pDX, IDC_RESET5, m_cResetY);
	DDX_Control(pDX, IDC_RESET3, m_cResetX);
	DDX_Control(pDX, IDC_RESET2, m_cRoughReset);
	DDX_Control(pDX, IDC_RESET1, m_cEchoTopReset);
	DDX_Control(pDX, IDC_THINFILM, m_cThinFilmPos);
	DDX_Control(pDX, IDC_ECHO, m_cEchoPos);
	DDX_Control(pDX, IDC_RESETALL, m_cResetAll);
	DDX_Control(pDX, IDC_RESETXY, m_cResetXY);
	DDX_Control(pDX, IDC_RADIUS, m_cRadius);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_MESSAGE2, m_cMessage2);
	DDX_Control(pDX, IDC_XYSTEP2, m_YStep);
	DDX_Control(pDX, IDC_Z2STEP, m_RoughZStep);
	DDX_Control(pDX, IDC_Z1STEP, m_EchoTopZStep);
	DDX_Control(pDX, IDC_XYSTEP, m_XStep);
	DDX_Text(pDX, IDC_ACC1, m_AccEchoTop);
	DDX_Text(pDX, IDC_ACC2, m_AccRough);
	DDX_Text(pDX, IDC_ACC3, m_AccX);
	DDX_Text(pDX, IDC_ACC4, m_AccP);
	DDX_Text(pDX, IDC_CUR1, m_CurEchoTop);
	DDX_Text(pDX, IDC_CUR2, m_CurRough);
	DDX_Text(pDX, IDC_CUR3, m_CurX);
	DDX_Text(pDX, IDC_CUR4, m_CurP);
	DDX_Text(pDX, IDC_SPD1, m_SpdEchoTop);
	DDX_Text(pDX, IDC_SPD2, m_SpdRough);
	DDX_Text(pDX, IDC_SPD3, m_SpdX);
	DDX_Text(pDX, IDC_SPD4, m_SpdP);
	DDX_Text(pDX, IDC_SCALE1, m_ScaleEchoTop);
	DDX_Text(pDX, IDC_SCALE2, m_ScaleRough);
	DDX_Text(pDX, IDC_SCALE3, m_ScaleX);
	DDX_Text(pDX, IDC_SCALE4, m_ScaleP);
	DDX_Text(pDX, IDC_POLARITY1, m_PolarityEchoTop);
	DDX_Text(pDX, IDC_POLARITY2, m_PolarityRough);
	DDX_Text(pDX, IDC_POLARITY3, m_PolarityX);
	DDX_Text(pDX, IDC_POLARITY4, m_PolarityP);
	DDX_Text(pDX, IDC_HOME1, m_HomeEchoTop);
	DDX_Text(pDX, IDC_HOME2, m_HomeRough);
	DDX_Text(pDX, IDC_HOME3, m_HomeX);
	DDX_Text(pDX, IDC_HOME4, m_HomeP);
	DDX_Text(pDX, IDC_MAX1, m_MaxEchoTop);
	DDX_Text(pDX, IDC_MAX2, m_MaxRough);
	DDX_Text(pDX, IDC_MAX3, m_MaxX);
	DDX_Text(pDX, IDC_MAX4, m_MaxP);
	DDX_Text(pDX, IDC_MIN1, m_MinEchoTop);
	DDX_Text(pDX, IDC_MIN2, m_MinRough);
	DDX_Text(pDX, IDC_MIN3, m_MinX);
	DDX_Text(pDX, IDC_MIN4, m_MinP);
	DDX_Text(pDX, IDC_CUR5, m_CurY);
	DDX_Text(pDX, IDC_ACC5, m_AccY);
	DDX_Text(pDX, IDC_HOME5, m_HomeY);
	DDX_Text(pDX, IDC_MAX5, m_MaxY);
	DDX_Text(pDX, IDC_MIN5, m_MinY);
	DDX_Text(pDX, IDC_POLARITY5, m_PolarityY);
	DDX_Text(pDX, IDC_SCALE5, m_ScaleY);
	DDX_Text(pDX, IDC_SPD5, m_SpdY);
	DDX_Text(pDX, IDC_OFFSET1, m_OffsetEchoTop);
	DDX_Text(pDX, IDC_OFFSET2, m_OffsetRough);
	DDX_Text(pDX, IDC_OFFSET3, m_OffsetX);
	DDX_Text(pDX, IDC_OFFSET4, m_OffsetY);
	DDX_Text(pDX, IDC_OFFSET5, m_OffsetP);
	DDX_Text(pDX, IDC_XBACKLASH, m_XBacklash);
	DDX_Text(pDX, IDC_YBACKLASH, m_YBacklash);
	DDX_Text(pDX, IDC_ZBACKLASH, m_ZBacklash);
	DDX_Text(pDX, IDC_CXOFFSET, m_CXOffset);
	DDX_Text(pDX, IDC_CYOFFSET, m_CYOffset);
	//// [05132024 ZHIMING
	//DDX_Control(pDX, IDC_POSECHOBOTTOMZ, m_cPosEchoBottomZ);
	//DDX_Control(pDX, IDC_ECHOBOTTOMZSTEP, m_EchoBottomZStep);
	//DDX_Control(pDX, IDC_ECHOBOTTOMOFFSET, m_cEchoBottomOffset);
	//DDX_Control(pDX, IDC_ECHOBOTTOMPOLARITY, m_cEchoBottomPolarity);
	//DDX_Control(pDX, IDC_ECHOBOTTOMSCALE, m_cEchoBottomScale);
	//DDX_Control(pDX, IDC_ECHOBOTTOMCUR, m_cEchoBottomCur);
	//DDX_Control(pDX, IDC_ECHOBOTTOMSPD, m_cEchoBottomSpd);
	//DDX_Control(pDX, IDC_ECHOBOTTOMACC, m_cEchoBottomAcc);
	//DDX_Control(pDX, IDC_DOWNECHOBOTTOMZ, m_cDownEchoBottomZ);
	//DDX_Control(pDX, IDC_UPECHOBOTTOMZ, m_cUpEchoBottomZ);
	//DDX_Control(pDX, IDC_ECHOBOTTOMRESET, m_cEchoBottomReset);
	//DDX_Text(pDX, IDC_ECHOBOTTOMACC, m_AccEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMSPD, m_SpdEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMCUR, m_CurEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMSCALE, m_ScaleEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMPOLARITY, m_PolarityEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMHOME, m_HomeEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMMIN, m_MinEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMMAX, m_MaxEchoBottom);
	//DDX_Text(pDX, IDC_ECHOBOTTOMOFFSET, m_OffsetEchoBottom);
	//// ]

	// [ 06112024/MORSALIN
	DDX_Control(pDX, IDC_POS_CWLZ, m_cPosCWLZ);
	DDX_Control(pDX, IDC_CWLZ_STEP, m_CWLZStep);
	DDX_Control(pDX, IDC_CWL_OFFSET, m_cCWLOffset);
	DDX_Control(pDX, IDC_CWL_POLARITY, m_cCWLPolarity);
	DDX_Control(pDX, IDC_CWL_SCALE, m_cCWLScale);
	DDX_Control(pDX, IDC_CWL_CUR, m_cCWLCur);
	DDX_Control(pDX, IDC_CWL_SPD, m_cCWLSpd);
	DDX_Control(pDX, IDC_CWL_ACC, m_cCWLAcc);
	DDX_Control(pDX, IDC_DOWN_CWLZ, m_cDownCWLZ);
	DDX_Control(pDX, IDC_UP_CWLZ, m_cUpCWLZ);
	DDX_Control(pDX, IDC_CWL_RESET, m_cCWLReset);
	DDX_Text(pDX, IDC_CWL_ACC, m_AccCWL);
	DDX_Text(pDX, IDC_CWL_SPD, m_SpdCWL);
	DDX_Text(pDX, IDC_CWL_CUR, m_CurCWL);
	DDX_Text(pDX, IDC_CWL_SCALE, m_ScaleCWL);
	DDX_Text(pDX, IDC_CWL_POLARITY, m_PolarityCWL);
	DDX_Text(pDX, IDC_CWL_HOME, m_HomeCWL);
	DDX_Text(pDX, IDC_CWL_MIN, m_MinCWL);
	DDX_Text(pDX, IDC_CWL_MAX, m_MaxCWL);
	DDX_Text(pDX, IDC_CWL_OFFSET, m_OffsetCWL);
	// ]

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp6Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp6Dlg)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_RESET1, OnResetEchoTopZ)
	ON_BN_CLICKED(IDC_RESET2, OnResetRoughZ)
	ON_BN_CLICKED(IDC_RESET3, OnResetX)
	ON_BN_CLICKED(IDC_RESET4, OnResetP)
	ON_BN_CLICKED(IDC_UPZ1, OnUpzechotop)
	ON_BN_CLICKED(IDC_UPZ2, OnUpzrough)
	ON_BN_CLICKED(IDC_UP_CWLZ, OnUpzTopCWL)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWNZ1, OnDownzechotop)
	ON_BN_CLICKED(IDC_DOWNZ2, OnDownzrough)
	ON_BN_CLICKED(IDC_DOWN_CWLZ, OnDownzTopCWL)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_CBN_SELCHANGE(IDC_Z1STEP, OnSelchangeeEchotopZstep)
	ON_CBN_SELCHANGE(IDC_XYSTEP, OnSelchangeXystep)
	ON_CBN_SELCHANGE(IDC_Z2STEP, OnSelchangeRoughZstep)
	ON_BN_CLICKED(IDC_ECHO, OnEcho)
	ON_BN_CLICKED(IDC_THINFILM, OnThinfilm)
	ON_BN_CLICKED(IDC_ALLSTOP, OnAllstop)
	ON_BN_CLICKED(IDC_RESET5, OnResetY)
	ON_CBN_SELCHANGE(IDC_XYSTEP2, OnSelchangeXystep2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESETALL, OnResetall)
	ON_BN_CLICKED(IDC_BUT1, OnBut1)
	ON_BN_CLICKED(IDC_BUT2, OnBut2)
	ON_BN_CLICKED(IDC_BUT3, OnBut3)
	ON_BN_CLICKED(IDC_BUT4, OnBut4)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MOTIONCONTROL2, OnMotioncontrol2)
	ON_BN_CLICKED(IDC_CAMERASETTING4, OnCamerasetting)
	ON_BN_CLICKED(IDC_RESETXY, OnResetxy)
	ON_BN_CLICKED(IDC_ADVANCESETUP, OnAdvancesetup)
	ON_BN_CLICKED(IDC_BUT5, OnBut5)
	ON_BN_CLICKED(IDC_BUT6, OnBut6)
	ON_BN_CLICKED(IDC_BUT7, OnBut7)
	ON_BN_CLICKED(IDC_BUT8, OnBut8)
	ON_BN_CLICKED(IDC_BUT9, OnBut9)
	ON_CBN_SELCHANGE(IDC_RADIUS, OnSelchangeRadius)
	ON_BN_CLICKED(IDC_CHUCKVACON2, OnChuckvacon2)
	ON_BN_CLICKED(IDC_CHUCKVACOFF2, OnChuckvacoff2)
	ON_BN_CLICKED(IDC_POSX, OnPosx)
	ON_BN_CLICKED(IDC_POSY, OnPosy)
	ON_BN_CLICKED(IDC_POSZ1, OnPosz1)
	ON_BN_CLICKED(IDC_POSZ2, OnPosz2)
	ON_BN_CLICKED(IDC_POS_CWLZ, OnPosCWLTopZ)
	ON_BN_CLICKED(IDC_ALIGNMENT, OnAlignment)
	ON_BN_CLICKED(IDC_WAITFORXY, OnWaitforxy)
	ON_BN_CLICKED(IDC_WAITFORALL, OnWaitforall)
	ON_BN_CLICKED(IDC_GETPOSALL, OnGetposall)
	ON_EN_KILLFOCUS(IDC_CXOFFSET, OnKillfocusCxoffset)
	ON_EN_KILLFOCUS(IDC_CYOFFSET, OnKillfocusCyoffset)
	ON_BN_CLICKED(IDC_SAVECX, OnSavecx)
	ON_BN_CLICKED(IDC_RECONNECT, OnReconnect)
	ON_BN_CLICKED(IDC_LEFTP, OnLeftp)
	ON_BN_CLICKED(IDC_RIGHTP, OnRightp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_CBN_SELCHANGE(IDC_ECHOBOTTOMZSTEP, OnSelchangeEchobottomZstep)
	ON_BN_CLICKED(IDC_ECHOBOTTOMRESET, OnResetEchoBottomZ)
	ON_BN_CLICKED(IDC_CWL_RESET, OnResetCWLTopZ)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp6Dlg message handlers
long CSetupProp6Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			StopAllCamera(&m_cVideo);
			m_cVideo.pWnd = NULL;
		}
	}
	bTabSelected = FALSE;
	return 0;
}

long CSetupProp6Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.SetRecipe(pRcp);
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

void CSetupProp6Dlg::PostNcDestroy() {
	pSP6Dlg = NULL;

	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp6Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

BOOL CSetupProp6Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_ADVANCESETUP, TOP_RIGHT);
	AddAnchor(IDC_SET, TOP_RIGHT);
	AddAnchor(IDC_ALLSTOP, TOP_RIGHT);
	AddAnchor(IDC_RESETALL, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL2, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING4, TOP_RIGHT);
	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);
	AddAnchor(IDC_RESETXY, TOP_RIGHT);
	AddAnchor(IDC_CHUCKVACON2, TOP_RIGHT);
	AddAnchor(IDC_CHUCKVACOFF2, TOP_RIGHT);
	AddAnchor(IDC_ALIGNMENT, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (g->Confg.bEchoMicroscopeMod || g->Confg.bThinFilmMicroscopeMod) {
		m_cVideo.ShowWindow(SW_SHOW);
		m_cCameraSetting.ShowWindow(SW_SHOW);
		m_cCameraA.ShowWindow(SW_SHOW);
		m_cCameraB.ShowWindow(SW_SHOW);
	}

	// [02052024 ZHIMING
	if (g->Confg.bThinFilmMicroscopeMod == FALSE) {
		m_cCameraB.ShowWindow(SW_HIDE);
	}
	// ]

	if (g->Confg.bEchoTopMotorMod) {
		m_cUpEchoTopZ.EnableWindow(TRUE);
		m_cDownEchoTopZ.EnableWindow(TRUE);
		m_EchoTopZStep.EnableWindow(TRUE);
		m_cEchoTopAcc.EnableWindow(TRUE);
		m_cEchoTopSpd.EnableWindow(TRUE);
		m_cEchoTopCur.EnableWindow(TRUE);
		m_cEchoTopScale.EnableWindow(TRUE);
		m_cEchoTopPolarity.EnableWindow(TRUE);
		m_cEchoTopOffset.EnableWindow(TRUE);
		m_cEchoTopReset.EnableWindow(TRUE);
		m_cPosEchoTopZ.EnableWindow(TRUE);
	}
	// [ 06112024/MORSALIN
	if (g->Confg.bConfocalTopMotorMod) {
		m_cUpCWLZ.EnableWindow(TRUE);
		m_cDownCWLZ.EnableWindow(TRUE);
		m_CWLZStep.EnableWindow(TRUE);
		m_cEchoTopAcc.EnableWindow(TRUE);
		m_cCWLSpd.EnableWindow(TRUE);
		m_cCWLCur.EnableWindow(TRUE);
		m_cCWLScale.EnableWindow(TRUE);
		m_cCWLPolarity.EnableWindow(TRUE);
		m_cCWLOffset.EnableWindow(TRUE);
		m_cCWLReset.EnableWindow(TRUE);
		m_cPosCWLZ.EnableWindow(TRUE);
	}
	// ]
	if (g->Confg.bRoughMotorMod) {
		m_cUpRoughZ.EnableWindow(TRUE);
		m_cDownRoughZ.EnableWindow(TRUE);
		m_RoughZStep.EnableWindow(TRUE);
		m_cRoughAcc.EnableWindow(TRUE);
		m_cRoughSpd.EnableWindow(TRUE);
		m_cRoughCur.EnableWindow(TRUE);
		m_cRoughScale.EnableWindow(TRUE);
		m_cRoughPolarity.EnableWindow(TRUE);
		m_cRoughOffset.EnableWindow(TRUE);
		m_cRoughReset.EnableWindow(TRUE);
		m_cPosRoughZ.EnableWindow(TRUE);
	}
	if (g->Confg.bPMotorMod) {
		m_cEchoPos.EnableWindow(TRUE);
		m_cThinFilmPos.EnableWindow(TRUE);
		m_cLeftP.EnableWindow(TRUE);
		m_cRightP.EnableWindow(TRUE);
		m_cAccP.EnableWindow(TRUE);
		m_cSpdP.EnableWindow(TRUE);
		m_cCurP.EnableWindow(TRUE);
		m_cScaleP.EnableWindow(TRUE);
		m_cPolarityP.EnableWindow(TRUE);
		m_cOffsetP.EnableWindow(TRUE);
		m_cResetP.EnableWindow(TRUE);
	}
	//if (g->Confg.bEchoBottomMotorMod) {
	//	m_cUpEchoBottomZ.EnableWindow(TRUE);
	//	m_cDownEchoBottomZ.EnableWindow(TRUE);
	//	m_EchoBottomZStep.EnableWindow(TRUE);
	//	m_cEchoBottomAcc.EnableWindow(TRUE);
	//	m_cEchoBottomSpd.EnableWindow(TRUE);
	//	m_cEchoBottomCur.EnableWindow(TRUE);
	//	m_cEchoBottomScale.EnableWindow(TRUE);
	//	m_cEchoBottomPolarity.EnableWindow(TRUE);
	//	m_cEchoBottomOffset.EnableWindow(TRUE);
	//	m_cEchoBottomReset.EnableWindow(TRUE);
	//	m_cPosEchoBottomZ.EnableWindow(TRUE);
	//}

	RecipeToLocal();

	m_EchoTopZStep.SetCurSel(3);
	m_RoughZStep.SetCurSel(3);
	m_XStep.SetCurSel(3);
	m_YStep.SetCurSel(3);
	//m_EchoBottomZStep.SetCurSel(3);
	m_cRadius.SetCurSel(0); radius = 150;
	m_CWLZStep.SetCurSel(3); // 06112024/MORSALIN

	UpdatePosition();

	m_cVideo.SetRecipe(&g->RcpSetup);

	SetTimer(IDT_START, 500, NULL);
	SetTimer(IDT_REFRESH, 950, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp6Dlg::LocalToRecipe() {}

void CSetupProp6Dlg::RecipeToLocal() {
	CMotorParam* pParam;

	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr && pMtr->pMEchoTopZ) {
		pParam = &pMtr->pMEchoTopZ->Param;
		m_CurEchoTop = pParam->MotorCurrent;
		m_SpdEchoTop = pParam->Speed;
		m_AccEchoTop = pParam->Acceleration;
		m_ScaleEchoTop = pParam->Scale;
		m_PolarityEchoTop = pParam->bPolarity;
		m_HomeEchoTop = pParam->HomePos1;
		m_MaxEchoTop = pParam->MaxPos1;
		m_MinEchoTop = pParam->MinPos1;
		m_OffsetEchoTop = pParam->Offset;
	}
	// [ 06112024/MORSALIN
	if (pMtr && pMtr->pMCWLTopZ) {
		pParam = &pMtr->pMCWLTopZ->Param;
		m_CurCWL = pParam->MotorCurrent;
		m_SpdCWL = pParam->Speed;
		m_AccCWL = pParam->Acceleration;
		m_ScaleCWL = pParam->Scale;
		m_PolarityCWL = pParam->bPolarity;
		m_HomeCWL = pParam->HomePos1;
		m_MaxCWL = pParam->MaxPos1;
		m_MinCWL = pParam->MinPos1;
		m_OffsetCWL = pParam->Offset;
	}
	// ]

	if (pMtr && pMtr->pMRoughZ) {
		pParam = &pMtr->pMRoughZ->Param;
		m_CurRough = pParam->MotorCurrent;
		m_SpdRough = pParam->Speed;
		m_AccRough = pParam->Acceleration;
		m_ScaleRough = pParam->Scale;
		m_PolarityRough = pParam->bPolarity;
		m_HomeRough = pParam->HomePos1;
		m_MaxRough = pParam->MaxPos1;
		m_MinRough = pParam->MinPos1;
		m_OffsetRough = pParam->Offset;
	}

	if (pMtr && pMtr->pMX) {
		pParam = &pMtr->pMX->Param;
		m_CurX = pParam->MotorCurrent;
		m_SpdX = pParam->Speed;
		m_AccX = pParam->Acceleration;
		m_ScaleX = pParam->Scale;
		m_PolarityX = pParam->bPolarity;
		m_HomeX = pParam->HomePos1;
		m_MaxX = pParam->MaxPos1;
		m_MinX = pParam->MinPos1;
		m_OffsetX = pParam->Offset;
	}

	if (pMtr && pMtr->pMP) {
		pParam = &pMtr->pMP->Param;
		m_CurP = pParam->MotorCurrent;
		m_SpdP = pParam->Speed;
		m_AccP = pParam->Acceleration;
		m_ScaleP = pParam->Scale;
		m_PolarityP = pParam->bPolarity;
		m_HomeP = pParam->HomePos1;
		m_MaxP = pParam->MaxPos1;
		m_MinP = pParam->MinPos1;
		m_OffsetY = pParam->Offset;
	}

	if (pMtr && pMtr->pMY) {
		pParam = &pMtr->pMY->Param;
		m_CurY = pParam->MotorCurrent;
		m_SpdY = pParam->Speed;
		m_AccY = pParam->Acceleration;
		m_ScaleY = pParam->Scale;
		m_PolarityY = pParam->bPolarity;
		m_HomeY = pParam->HomePos1;
		m_MaxY = pParam->MaxPos1;
		m_MinY = pParam->MinPos1;
		m_OffsetP = pParam->Offset;
	}

	//// [05142024 ZHIMING
	//if (pMtr && pMtr->pMEchoBottomZ) {
	//	pParam = &pMtr->pMEchoBottomZ->Param;
	//	m_AccEchoBottom = pParam->Acceleration;
	//	m_CurEchoBottom = pParam->MotorCurrent;
	//	m_SpdEchoBottom = pParam->Speed;
	//	m_ScaleEchoBottom = pParam->Scale;
	//	m_PolarityEchoBottom = pParam->bPolarity;
	//	m_HomeEchoBottom = pParam->HomePos1;
	//	m_MaxEchoBottom = pParam->MaxPos1;
	//	m_MinEchoBottom = pParam->MinPos1;
	//	m_OffsetEchoBottom = pParam->Offset;
	//}
	//// ]

	m_XBacklash = p413App->backLashX;
	m_YBacklash = p413App->backLashY;
	m_ZBacklash = p413App->backLashZ;

	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Get(m_CXOffset, m_CYOffset);
		//m_CXOffset = g->CXOffsetEcho;
		//m_CYOffset = g->CYOffsetEcho;
		m_cCOffset.SetWindowText("Echo Center Offset (mm)");
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Get(m_CXOffset, m_CYOffset);						//20220221 / yukchiu
		//m_CXOffset = g->CXOffsetTF;
		//m_CYOffset = g->CYOffsetTF;
		m_cCOffset.SetWindowText("TF Center Offset (mm)");
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->COffCWL.Get(m_CXOffset, m_CYOffset);
		m_cCOffset.SetWindowText("CWL Center Offset (mm)");
		break;
	case CCamBase::ROUGH:
		g->COffRough.Get(m_CXOffset, m_CYOffset);
		m_cCOffset.SetWindowText("Rough Center Offset (mm)");
		break;
	// ]
	}

	UpdateData(FALSE);
}

void CSetupProp6Dlg::OnSet() {
	UpdateData(TRUE);
	SetMotorParam();
}

void CSetupProp6Dlg::SetMotorParam() {
	CMotorParam* pParam;

	UpdateData(TRUE);

	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr && pMtr->pMEchoTopZ) {
		pParam = &pMtr->pMEchoTopZ->Param;
		pParam->MotorCurrent = m_CurEchoTop;
		pParam->Speed = m_SpdEchoTop;
		pParam->Acceleration = m_AccEchoTop;
		pParam->Scale = m_ScaleEchoTop;
		pParam->bPolarity = m_PolarityEchoTop;
		pParam->HomePos1 = m_HomeEchoTop;
		pParam->MaxPos1 = m_MaxEchoTop;
		pParam->MinPos1 = m_MinEchoTop;
		pParam->Offset = m_OffsetEchoTop;
		pMtr->pMEchoTopZ->vSetParameter("EchoTopMotor");
	}

	if (pMtr && pMtr->pMRoughZ) {
		pParam = &pMtr->pMRoughZ->Param;
		pParam->MotorCurrent = m_CurRough;
		pParam->Speed = m_SpdRough;
		pParam->Acceleration = m_AccRough;
		pParam->Scale = m_ScaleRough;
		pParam->bPolarity = m_PolarityRough;
		pParam->HomePos1 = m_HomeRough;
		pParam->MaxPos1 = m_MaxRough;
		pParam->MinPos1 = m_MinRough;
		pParam->Offset = m_OffsetRough;
		pMtr->pMRoughZ->vSetParameter("RoughMotor");
	}

	// [ 06112024/MORSALIN
	if (pMtr && pMtr->pMCWLTopZ) {
		pParam = &pMtr->pMCWLTopZ->Param;
		pParam->MotorCurrent = m_CurCWL;
		pParam->Speed = m_SpdCWL;
		pParam->Acceleration = m_AccCWL;
		pParam->Scale = m_ScaleCWL;
		pParam->bPolarity = m_PolarityCWL;
		pParam->HomePos1 = m_HomeCWL;
		pParam->MaxPos1 = m_MaxCWL;
		pParam->MinPos1 = m_MinCWL;
		pParam->Offset = m_OffsetCWL;
		pMtr->pMCWLTopZ->vSetParameter("CWLMotor");
	}
	// ]

	if (pMtr && pMtr->pMX) {
		pParam = &pMtr->pMX->Param;
		pParam->MotorCurrent = m_CurX;
		pParam->Speed = m_SpdX;
		pParam->Acceleration = m_AccX;
		pParam->Scale = m_ScaleX;
		pParam->bPolarity = m_PolarityX;
		pParam->HomePos1 = m_HomeX;
		pParam->MaxPos1 = m_MaxX;
		pParam->MinPos1 = m_MinX;
		pParam->Offset = m_OffsetX;
		pMtr->pMX->vSetParameter("XMotor");
	}

	if (pMtr && pMtr->pMY) {
		pParam = &pMtr->pMY->Param;
		pParam->MotorCurrent = m_CurY;
		pParam->Speed = m_SpdY;
		pParam->Acceleration = m_AccY;
		pParam->Scale = m_ScaleY;
		pParam->bPolarity = m_PolarityY;
		pParam->HomePos1 = m_HomeY;
		pParam->MaxPos1 = m_MaxY;
		pParam->MinPos1 = m_MinY;
		pParam->Offset = m_OffsetY;
		pMtr->pMY->vSetParameter("YMotor");
	}

	if (pMtr && pMtr->pMP) {
		pParam = &pMtr->pMP->Param;
		pParam->MotorCurrent = m_CurP;
		pParam->Speed = m_SpdP;
		pParam->Acceleration = m_AccP;
		pParam->Scale = m_ScaleP;
		pParam->bPolarity = m_PolarityP;
		pParam->HomePos1 = m_HomeP;
		pParam->MaxPos1 = m_MaxP;
		pParam->MinPos1 = m_MinP;
		pParam->Offset = m_OffsetP;
		pMtr->pMP->vSetParameter("PMotor");
	}

	//// [05142024 ZHIMING
	//if (pMtr && pMtr->pMEchoBottomZ) {
	//	pParam = &pMtr->pMEchoBottomZ->Param;
	//	pParam->MotorCurrent = m_CurEchoBottom;
	//	pParam->Speed = m_SpdEchoBottom;
	//	pParam->Acceleration = m_AccEchoBottom;
	//	pParam->Scale = m_ScaleEchoBottom;
	//	pParam->bPolarity = m_PolarityEchoBottom;
	//	pParam->HomePos1 = m_HomeEchoBottom;
	//	pParam->MaxPos1 = m_MaxEchoBottom;
	//	pParam->MinPos1 = m_MinEchoBottom;
	//	pParam->Offset = m_OffsetEchoBottom;
	//	pMtr->pMEchoBottomZ->vSetParameter("EchoBottomMotor");
	//}
	//// ]

	p413App->backLashX = m_XBacklash;
	p413App->backLashY = m_YBacklash;
	p413App->backLashZ = m_ZBacklash;

	p413App->SaveMotorOption();
}

void CSetupProp6Dlg::SaveMotorParam() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr && pMtr->pMEchoTopZ) {
		pMtr->pMEchoTopZ->vSaveParameter("EchoTopMotor");
		pMtr->pMEchoTopZ->vGetParameter("EchoTopMotor");
	}
	if (pMtr && pMtr->pMRoughZ) {
		pMtr->pMRoughZ->vSaveParameter("RoughMotor");
		pMtr->pMRoughZ->vGetParameter("RoughMotor");
	}
	// [ 06112024/MORSALIN
	if (pMtr && pMtr->pMCWLTopZ) {
		pMtr->pMCWLTopZ->vSaveParameter("CWLMotor");
		pMtr->pMCWLTopZ->vGetParameter("CWLMotor");
	}
	// ]
	if (pMtr && pMtr->pMX) {
		pMtr->pMX->vSaveParameter("XMotor");
		pMtr->pMX->vGetParameter("XMotor");
	}
	if (pMtr && pMtr->pMY) {
		pMtr->pMY->vSaveParameter("YMotor");
		pMtr->pMY->vGetParameter("YMotor");
	}
	if (pMtr && pMtr->pMP) {
		pMtr->pMP->vSaveParameter("PMotor");
		pMtr->pMP->vGetParameter("PMotor");
	}
	//// [05142024 ZHIMING
	//if (pMtr && pMtr->pMEchoBottomZ) {
	//	pMtr->pMEchoBottomZ->vSaveParameter("EchoBottomMotor");
	//	pMtr->pMEchoBottomZ->vGetParameter("EchoBottomMotor");
	//}
	//// ]
}

void CSetupProp6Dlg::OnResetEchoTopZ() {
	if (bBusy) { return; }
	BlockUI();
	m_cEchoTopReset.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetEchoTopZ();
		bBusy = FALSE;
	}
	m_cEchoTopReset.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp6Dlg::OnResetRoughZ() {
	if (bBusy) { return; }
	BlockUI();
	m_cRoughReset.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetRoughZ();
		bBusy = FALSE;
	}
	m_cRoughReset.EnableWindow(TRUE);
	ReleaseUI();
}

// [ 06112024/MORSALIN
void CSetupProp6Dlg::OnResetCWLTopZ() {
	if (bBusy) { return; }
	BlockUI();
	m_cCWLReset.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetCWLTopZ();
		bBusy = FALSE;
	}
	m_cCWLReset.EnableWindow(TRUE);
	ReleaseUI();
}
// ]

void CSetupProp6Dlg::OnResetX() {
	if (bBusy) { return; }
	BlockUI();
	m_cResetX.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetX();
		bBusy = FALSE;
	}
	m_cResetX.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp6Dlg::OnResetP() {
	m_cResetP.EnableWindow(FALSE);
	p413App->ResetPMotor();
	m_cResetP.EnableWindow(TRUE);
}

void CSetupProp6Dlg::OnResetY() {
	if (bBusy) { return; }
	BlockUI();
	m_cResetY.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetY();
		bBusy = FALSE;
	}
	m_cResetY.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp6Dlg::OnResetxy() {
	if (bBusy) { return; }
	BlockUI();
	m_cResetXY.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetXY();
		g->ResetLapTime = GetTickCount64();
		//g->bConfirmStage = TRUE;
		bBusy = FALSE;
	}
	m_cResetXY.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp6Dlg::OnUpzechotop() {
	if (bBusy) { return; }
	CString str;
	m_EchoTopZStep.GetLBText(m_EchoTopZStep.GetCurSel(), str);
	CMotorS2* pMtr = p413App->pMtr;
	float pos = atof(str);
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveREchoTopZ(-pos);
		bBusy = FALSE;;
	}
}

// [ 06112024/MORSALIN
void CSetupProp6Dlg::OnUpzTopCWL() {
	if (bBusy) { return; }
	CString str;
	m_EchoTopZStep.GetLBText(m_EchoTopZStep.GetCurSel(), str);
	CMotorS2* pMtr = p413App->pMtr;
	float pos = atof(str);
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRCWLTopZ(-pos);
		bBusy = FALSE;;
	}
}

void CSetupProp6Dlg::OnDownzTopCWL() {
	if (bBusy) { return; }
	CString str;
	m_EchoTopZStep.GetLBText(m_EchoTopZStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRCWLTopZ(pos);
		bBusy = FALSE;
	}
}
// ]

void CSetupProp6Dlg::OnUpzrough() {
	if (bBusy) { return; }
	CString str;
	m_RoughZStep.GetLBText(m_RoughZStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRRoughZ(-pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnDownzechotop() {
	if (bBusy) { return; }
	CString str;
	m_EchoTopZStep.GetLBText(m_EchoTopZStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveREchoTopZ(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnDownzrough() {
	if (bBusy) { return; }
	CString str;
	m_RoughZStep.GetLBText(m_RoughZStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRRoughZ(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnUp() {
	if (bBusy) { return; }
	CString str;
	m_YStep.GetLBText(m_YStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRY(-pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnDown() {
	if (bBusy) { return; }
	CString str;
	m_YStep.GetLBText(m_YStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRY(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnLeft() {
	if (bBusy) { return; }
	CString str;
	m_XStep.GetLBText(m_XStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRX(-pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnRight() {
	if (bBusy) { return; }
	CString str;
	m_XStep.GetLBText(m_XStep.GetCurSel(), str);
	float pos = atof(str);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRX(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnSelchangeeEchotopZstep() {
	CString msg;
	m_EchoTopZStep.GetLBText(m_EchoTopZStep.GetCurSel(), msg);
	EchoTopZFac = atoi(msg);
}

void CSetupProp6Dlg::OnSelchangeXystep() {
	CString msg;
	m_XStep.GetLBText(m_XStep.GetCurSel(), msg);
	XFac = atoi(msg);
}

void CSetupProp6Dlg::OnSelchangeRoughZstep() {
	CString msg;
	m_RoughZStep.GetLBText(m_RoughZStep.GetCurSel(), msg);
	RoughZFac = atoi(msg);
}
// [ 06112024/MORSALIN
void CSetupProp6Dlg::OnSelchangeCWLTopZstep() {
	CString msg;
	m_CWLZStep.GetLBText(m_CWLZStep.GetCurSel(), msg);
	CWLZFac = atoi(msg);
}// ]

void CSetupProp6Dlg::OnSelchangeXystep2() {
	CString msg;
	m_YStep.GetLBText(m_YStep.GetCurSel(), msg);
	YFac = atoi(msg);
}

void CSetupProp6Dlg::OnSelchangeEchobottomZstep() {
	CString msg;
	m_EchoBottomZStep.GetLBText(m_EchoBottomZStep.GetCurSel(), msg);
	EchoBottomZFac = atoi(msg);
}

void CSetupProp6Dlg::OnEcho() {
	m_cEchoPos.EnableWindow(FALSE);
	m_cThinFilmPos.EnableWindow(FALSE);
	p413App->GotoPositionecho();
	m_cEchoPos.EnableWindow(TRUE);
	m_cThinFilmPos.EnableWindow(TRUE);
}

void CSetupProp6Dlg::OnThinfilm() {
	m_cEchoPos.EnableWindow(FALSE);
	m_cThinFilmPos.EnableWindow(FALSE);
	p413App->GotoPositionthinfilm();
	m_cEchoPos.EnableWindow(TRUE);
	m_cThinFilmPos.EnableWindow(TRUE);
}

void CSetupProp6Dlg::OnAllstop() {
	if (p413App->pMtr) {
		p413App->pMtr->AllStop();
	}
}

void CSetupProp6Dlg::UpdatePosition() {
	CString msg;

	C413Global* g = &p413App->Global;
	/*msg.Format("X:%.2fmm  Y:%.2fmm  EchoTopZ:%.2fmm  RoughZ:%.2fmm  CWLZ:%.2fmm  P:%.2fau  LoadPos:(%.2f,%.2f)",
		g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocRoughZ, g->LocCWLTopZ, g->LocP, g->LoadPos.PX, g->LoadPos.PY);*/
	msg.Format("X:%.2fmm  Y:%.2fmm  MicroscopeZ:%.2fmm  RoughZ:%.2fmm  CWLZ:%.2fmm  P:%.2fau  LoadPos:(%.2f,%.2f)",
		g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocRoughZ, g->LocCWLTopZ, g->LocP, g->LoadPos.PX, g->LoadPos.PY);
	m_cMessage2.SetWindowText(msg);
}

void CSetupProp6Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		UpdatePosition();
	}
	if (nIDEvent == IDT_START) {
		CMotorS2* pMtr = p413App->pMtr;
		if (pMtr && pMtr->pSiMc) {
			KillTimer(IDT_START);
			pMtr->pSiMc->SetCallBack(OnMessageProc);
			m_cMessage.SetWindowText(pMtr->pSiMc->IDStr);
		}
	}
}

void CSetupProp6Dlg::OnResetall() {
	BlockUI();
	m_cResetAll.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		if (pMtr->ResetAll()) {
			pMtr->GotoXY(0, 0);
			//pMtr->SetHome();
			g->ResetLapTime = GetTickCount64();
			//g->bConfirmStage = TRUE;
		}
	}
	m_cResetAll.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp6Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	OnEcho();
	p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

void CSetupProp6Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo);
	OnThinfilm();
	p413App->StartCamera(&m_cVideo, CCamBase::THINFCAM, NULL, pRcp);
	m_pVideo = &m_cVideo;
	m_cVideo.pWnd = this;
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);

	RecipeToLocal();
}

void CSetupProp6Dlg::OnClose() {
	StopAllCamera(&m_cVideo);
	CResizableDialog::OnClose();
}

void CSetupProp6Dlg::OnMotioncontrol2() {
	ShowMotionControl(this);
}

void CSetupProp6Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp6Dlg::OnAdvancesetup() {
	CAdvanceSetup6 dlg;

	if (dlg.DoModal() == IDOK) {
		CMotorS2* pMtr = p413App->pMtr;
		switch (dlg.cmd) {
		case CAdvanceSetup6::CLEARORIGIN:
		{
			m_HomeX = 0;
			m_HomeY = 0;
			if (pMtr) {
				if (pMtr->pMX) {
					pMtr->pMX->Param.HomePos1 = 0;
				}
				if (pMtr->pMY) {
					pMtr->pMY->Param.HomePos1 = 0;
				}
			}
			UpdateData(FALSE);
			OnResetX();
			OnResetY();
		}
		break;
		case CAdvanceSetup6::SAVEPARAM:
		{
			if (AfxMessageBox("Date will be written to INI file.\nAre U sure?", MB_YESNO) == IDYES) {
				SetMotorParam();
				SaveMotorParam();
				RecipeToLocal();
			}
		}
		break;
		case CAdvanceSetup6::LOADPARAM:
		{
			RecipeToLocal();
		}
		break;
		case CAdvanceSetup6::SETLOADPOS:
		{
			CAdvanceSetup Addlg;

			CCamBase::CAMERAID ac = g->ActiveCamera;
			g->ActiveCamera = CCamBase::ECHOCAM;
			if (pMtr) {
				pMtr->GetXYPos(&Addlg.m_XPos, &Addlg.m_YPos);
				Addlg.m_ZPos = 0;
			}

			Addlg.Type = CAdvanceSetup::LOADPOS;
			if (Addlg.DoModal() == IDOK) {
				g->LoadPos.PX = Addlg.m_XPos;
				g->LoadPos.PY = Addlg.m_YPos;
				g->SaveMotorOption();
			}
			g->ActiveCamera = ac;
		}
		break;
		case CAdvanceSetup6::SETBGPOS:
		{
			CAdvanceSetup Addlg;
			CCamBase::CAMERAID ac = g->ActiveCamera;
			g->ActiveCamera = CCamBase::THINFCAM;
			if (pMtr) {
				pMtr->GetXYPos(&Addlg.m_XPos, &Addlg.m_YPos);
				Addlg.m_ZPos = 0;
			}

			Addlg.Type = CAdvanceSetup::CALIBBG;
			if (Addlg.DoModal() == IDOK) {
				g->BGPos.Set(Addlg.m_XPos, Addlg.m_YPos);
				//g->BGPosY = Addlg.m_YPos;
				g->SaveMotorOption();
			}
			g->ActiveCamera = ac;
		}
		break;
		case CAdvanceSetup6::SETCALIBPOS:
		{
			CAdvanceSetup Addlg;
			CCamBase::CAMERAID ac = g->ActiveCamera;
			g->ActiveCamera = CCamBase::ECHOCAM;
			if (pMtr) {
				pMtr->GetXYPos(&Addlg.m_XPos, &Addlg.m_YPos);
				pMtr->GetEchoTopZPosDev(Addlg.m_ZPos);
			}

			Addlg.Type = CAdvanceSetup::CALIBECHO;
			if (Addlg.DoModal() == IDOK) {
				g->DPPos.X = Addlg.m_XPos;
				g->DPPos.Y = Addlg.m_YPos;
				g->CalZPosEcho = Addlg.m_ZPos;
				g->SaveMotorOption();
				g->SaveEchoOption();
			}
			g->ActiveCamera = ac;
		}
		break;
		case CAdvanceSetup6::SETCALIBPOSTF:
		{
			CAdvanceSetup Addlg;
			CCamBase::CAMERAID ac = g->ActiveCamera;
			g->ActiveCamera = CCamBase::THINFCAM;
			if (pMtr) {
				pMtr->GetXYPos(&Addlg.m_XPos, &Addlg.m_YPos);
				pMtr->GetThinFilmZPosDev(Addlg.m_ZPos);
			}

			Addlg.Type = CAdvanceSetup::CALIBTF;
			if (Addlg.DoModal() == IDOK) {
				g->TFPos.Get(Addlg.m_XPos, Addlg.m_YPos);
				//g->TFPosY = Addlg.m_YPos;
				g->CalZPosTF = Addlg.m_ZPos;
				g->SaveMotorOption();
				g->SaveThinFilmOption();
			}
			g->ActiveCamera = ac;
		}
		break;
		// [05142024 ZHIMING
		case CAdvanceSetup6::SETCALIBPOSROU:
		{
			CAdvanceSetup Addlg;
			CCamBase::CAMERAID ac = g->ActiveCamera;
			g->ActiveCamera = CCamBase::ROUGH;
			if (pMtr) {
				pMtr->GetXYPos(&Addlg.m_XPos, &Addlg.m_YPos);
				pMtr->GetRoughZPosDev(Addlg.m_ZPos);
			}

			Addlg.Type = CAdvanceSetup::CALIBROU;
			if (Addlg.DoModal() == IDOK) {
				g->RoughCalibPos.Get(Addlg.m_XPos, Addlg.m_YPos);
				g->CalZPosRough = Addlg.m_ZPos;
				g->SaveMotorOption();
				g->SaveRoughOption();
			}
			g->ActiveCamera = ac;
		}
		break;
		// ]
		case CAdvanceSetup6::SETORIGINECHO:
			if (pMtr && (pMtr->pMX) && (pMtr->pMY)) {
				float posX, posY;
				pMtr->pMX->fReadPos(posX);
				pMtr->pMY->fReadPos(posY);
				m_HomeX = pMtr->pMX->Param.HomePos1 -= pMtr->pMX->Param.bPolarity ? posX : -posX;
				m_HomeY = pMtr->pMY->Param.HomePos1 -= pMtr->pMY->Param.bPolarity ? posY : -posY;
				pMtr->pMX->vSaveParameter("XMotor");
				pMtr->pMY->vSaveParameter("YMotor");

				UpdateData(FALSE);
				if (pMtr) pMtr->ResetXY();
			}
			break;
		case CAdvanceSetup6::SETORIGINTF:
			if (pMtr && (pMtr->pMX) && (pMtr->pMY)) {
				float posX, posY;
				pMtr->pMX->fReadPos(posX);
				pMtr->pMY->fReadPos(posY);
				m_HomeX = pMtr->pMX->Param.HomePos2 -= pMtr->pMX->Param.bPolarity ? posX : -posX;
				m_HomeY = pMtr->pMY->Param.HomePos2 -= pMtr->pMY->Param.bPolarity ? posY : -posY;
				pMtr->pMX->vSaveParameter("XMotor");
				pMtr->pMY->vSaveParameter("YMotor");

				UpdateData(FALSE);
				if (pMtr) pMtr->ResetXY();
			}
			break;
		// [05142024 ZHIMING
		case CAdvanceSetup6::SETORIGINROU:
			if (pMtr && (pMtr->pMX) && (pMtr->pMY)) {
				float posX, posY;
				pMtr->pMX->fReadPos(posX);
				pMtr->pMY->fReadPos(posY);
				m_HomeX = pMtr->pMX->Param.HomePos2 -= pMtr->pMX->Param.bPolarity ? posX : -posX;
				m_HomeY = pMtr->pMY->Param.HomePos2 -= pMtr->pMY->Param.bPolarity ? posY : -posY;
				pMtr->pMX->vSaveParameter("XMotor");
				pMtr->pMY->vSaveParameter("YMotor");

				UpdateData(FALSE);
				if (pMtr) pMtr->ResetXY();
			}
			break;
		// ]
		case CAdvanceSetup6::SETPECHO:
			if (pMtr && pMtr->pMP) {
				float posP;
				pMtr->pMP->fReadPos(posP);
				m_HomeP = pMtr->pMP->Param.HomePos1 = posP;
				pMtr->pMP->vSaveParameter("PMotor");
				UpdateData(FALSE);
			}
			break;
		case CAdvanceSetup6::SETPTF:
			if (pMtr && pMtr->pMP) {
				float posP;
				pMtr->pMP->fReadPos(posP);
				pMtr->pMP->Param.HomePos2 = posP;
				pMtr->pMP->vSaveParameter("PMotor");
				UpdateData(FALSE);
			}
			break;
		}
	}
}

// 0 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut3() {
	float x, y;
	g->Polar2Cartesian(radius, 0.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 90 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut1() {
	float x, y;
	g->Polar2Cartesian(radius, 90.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 180 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut2() {
	float x, y;
	g->Polar2Cartesian(radius, 180.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 270 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut4() {
	float x, y;
	g->Polar2Cartesian(radius, 270.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 315 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut5() {
	float x, y;
	g->Polar2Cartesian(radius, 315.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 45 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut6() {
	float x, y;
	g->Polar2Cartesian(radius, 45.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 135 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut7() {
	float x, y;
	g->Polar2Cartesian(radius, 135.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// 225 degree [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut8() {
	float x, y;
	g->Polar2Cartesian(radius, 225.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}

// Center point [8/7/2010 XPMUser]
void CSetupProp6Dlg::OnBut9() {
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(0, 0);
	}
}

void CSetupProp6Dlg::OnSelchangeRadius() {
	CString str;
	m_cRadius.GetLBText(m_cRadius.GetCurSel(), str);
	radius = atof(str);
}

void CSetupProp6Dlg::OnChuckvacon2() {
	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(TRUE);
	}
}

void CSetupProp6Dlg::OnChuckvacoff2() {
	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(FALSE);
	}
}

void CSetupProp6Dlg::Gotozeroxy() {
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(0, 0);
		p413App->pMtr->SetHome();
	}
}

void CSetupProp6Dlg::PositionEcho() {
	p413App->GotoPositionecho();
}

void CSetupProp6Dlg::OnMessageProc(int wParam, LPVOID lParam) {
	if (pSP6Dlg && lParam) {
		pSP6Dlg->m_cLogMsg.InsertString(0, (char*)lParam);
		short cnt = pSP6Dlg->m_cLogMsg.GetCount();
		if (cnt > 1024) {
			pSP6Dlg->m_cLogMsg.DeleteString(cnt);
		}
	}
}

void CSetupProp6Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "GLMotor");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CSetupProp6Dlg::OnPosx() {
	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&g->LocXY.X, &g->LocXY.Y);
	}
}

void CSetupProp6Dlg::OnPosy() {
	if (p413App->pMtr) {
		p413App->pMtr->GetXYPos(&g->LocXY.X, &g->LocXY.Y);
	}
}

void CSetupProp6Dlg::OnPosz1() {
	if (p413App->pMtr) {
		float pos;
		if (p413App->pMtr->pMEchoTopZ->bGetPos(&pos)) {
			g->LocEchoTopZ = pos;
		}
	}
}

void CSetupProp6Dlg::OnPosz2() {
	if (p413App->pMtr) {
		float pos;
		if (p413App->pMtr->pMRoughZ->bGetPos(&pos)) {
			g->LocRoughZ = pos;
		}
	}
}
// [ 06112024/MORSALIN
void CSetupProp6Dlg::OnPosCWLTopZ() {
	if (p413App->pMtr) {
		float pos;
		if (p413App->pMtr->pMCWLTopZ->bGetPos(&pos)) {
			g->LocCWLTopZ = pos;
		}
	}
}
// ]

CAlignment* pdlg = NULL;

void CSetupProp6Dlg::OnAlignment() {
	if (pdlg) {
		delete pdlg;
		pdlg = NULL;
	}
	pdlg = new CAlignment;
	if (pdlg) {
		pdlg->Create(CAlignment::IDD);
		pdlg->ShowWindow(SW_SHOW);
	}
}

long CSetupProp6Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

void CSetupProp6Dlg::OnWaitforxy() {
	BOOL bPos;
	p413App->pMtr->pSiMc->_WaitForXY(120000, bPos);
}

void CSetupProp6Dlg::OnWaitforall() {
	BOOL bPos = FALSE;
	p413App->pMtr->pSiMc->_WaitAllStop(120000, bPos);
}

void CSetupProp6Dlg::OnGetposall() {
	float fX, fY;
	p413App->pMtr->pSiMc->devGetXYPos(&fX, &fY, 10000);
}

void CSetupProp6Dlg::OnKillfocusCxoffset() {
	UpdateData(TRUE);
	switch (g->ActiveCamera) {
	// [05082024 ZHIMING
	default:
		g->COffEcho.X = m_CXOffset;
		g->COffTF.X = m_CXOffset;
		g->COffCWL.X = m_CXOffset;
		g->COffRough.X = m_CXOffset;
		break;
	case CCamBase::THINFCAM:
		g->COffEcho.X = m_CXOffset;
		g->COffTF.X = m_CXOffset;
		g->COffCWL.X = m_CXOffset;
		g->COffRough.X = m_CXOffset;
		break;
	case CCamBase::CWL:
		g->COffEcho.X = m_CXOffset;
		g->COffTF.X = m_CXOffset;
		g->COffCWL.X = m_CXOffset;
		g->COffRough.X = m_CXOffset;
		break;
	case CCamBase::ROUGH:
		g->COffEcho.X = m_CXOffset;
		g->COffTF.X = m_CXOffset;
		g->COffCWL.X = m_CXOffset;
		g->COffRough.X = m_CXOffset;
		break;
	// ]
	}
}

void CSetupProp6Dlg::OnKillfocusCyoffset() {
	UpdateData(TRUE);
	switch (g->ActiveCamera) {
	// [05082024 ZHIMING
	default:
		g->COffEcho.Y = m_CYOffset;
		g->COffTF.Y = m_CYOffset;
		g->COffCWL.Y = m_CYOffset;
		g->COffRough.Y = m_CYOffset;
		break;
	case CCamBase::THINFCAM:
		g->COffEcho.Y = m_CYOffset;
		g->COffTF.Y = m_CYOffset;
		g->COffCWL.Y = m_CYOffset;
		g->COffRough.Y = m_CYOffset;
		break;
	case CCamBase::CWL:
		g->COffEcho.Y = m_CYOffset;
		g->COffTF.Y = m_CYOffset;
		g->COffCWL.Y = m_CYOffset;
		g->COffRough.Y = m_CYOffset;
		break;
	case CCamBase::ROUGH:
		g->COffEcho.Y = m_CYOffset;
		g->COffTF.Y = m_CYOffset;
		g->COffCWL.Y = m_CYOffset;
		g->COffRough.Y = m_CYOffset;
		break;
	// ]
	}
}

void CSetupProp6Dlg::OnSavecx() {
	g->SaveMotorOption();
}

void CSetupProp6Dlg::OnReconnect() {
	m_cReconnect.EnableWindow(FALSE);
	p413App->pMtr->ReconnectTCP();
	m_cReconnect.EnableWindow(TRUE);
}

void CSetupProp6Dlg::OnLeftp() {
	if (bBusy) { return; }
	float pos = -1.00;
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRP(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnRightp() {
	if (bBusy) { return; }
	float pos = 1.0;
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->MoveRP(pos);
		bBusy = FALSE;
	}
}

void CSetupProp6Dlg::OnResetEchoBottomZ() {
	if (bBusy) { return; }
	BlockUI();
	m_cEchoBottomReset.EnableWindow(FALSE);
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		bBusy = TRUE;
		pMtr->ResetEchoBottomZ();
		bBusy = FALSE;
	}
	m_cEchoBottomReset.EnableWindow(TRUE);
	ReleaseUI();
}