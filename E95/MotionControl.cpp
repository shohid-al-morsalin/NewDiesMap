// MotionControl.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "413App.h"
#include "SICam.h"
#include "MotionControl.h"
#include "AdvanceOption.h"
#include "WaferMapDlg.h"
#include "GlobalDeviceContainer.h"
#include "Illumination.h"
#include "RecipeIO.h"
#include "LocDefineDice.h"
#include "..\413\MotorS2.h"
#include "GlobalUIContainer.h"
#include "..\SRC\LogInDataNew.h"
#include "TitlePanel.h"
#include "ProbeSwitchingBase.h" //02082024/MORSALIN

#include "DrLockMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_START	9999

/////////////////////////////////////////////////////////////////////////////
// CMotionControl dialog

CWaferMapDlg* pMap = NULL;

CMotionControl::CMotionControl(CWnd* pParent /*=NULL*/)
	: CDialog(CMotionControl::IDD, pParent)
	, m_RadioBtn(0)  //02082024/MORSALIN
	, movingStep(0)  //02082024/MORSALIN
{
	//{{AFX_DATA_INIT(CMotionControl)
	m_XPos = 0.0f;
	m_YPos = 0.0f;
	m_EchoTopZPos = 0.0f;
	m_RoughZPos = 0.0f;
	m_CWLTopZPos = 0.0f;
	m_Angle = 0.0f;
	m_Radius = 0.0f;
	m_Dx = 0.0f;
	m_DxY = 0.0f;
	m_Dy = 0.0f;
	m_DyX = 0.0f;
	//}}AFX_DATA_INIT

	pWnd = NULL;
	pVideo = NULL;

	bBusy = FALSE;
	//dirX = dirY = 1;
	Fac = EchoTopZFac = RoughZFac = 1;
	
	// [04292024 tushar
	//EchoBottomZFac = 1;
	// [05142024 ZHIMING
	m_EchoBottomZPos = 0.0f;
	// ]
}

void CMotionControl::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMotionControl)
	DDX_Control(pDX, IDC_BUTTON5, m_cdXRight);
	DDX_Control(pDX, IDC_BUTTON7, m_cdYDown);
	DDX_Control(pDX, IDC_BUTTON3, m_cdXLeft);
	DDX_Control(pDX, IDC_BUTTON6, m_cdYUp);
	DDX_Control(pDX, IDC_GOTORA, m_cGotoRA);
	DDX_Control(pDX, IDC_GOTOXY, m_cGotoXY);
	DDX_Control(pDX, IDC_TOPLEFT, m_cXYTopLeft);
	DDX_Control(pDX, IDC_LEFT, m_cXYLeft);
	DDX_Control(pDX, IDC_BOTTOMLEFT, m_cBotLeft);
	DDX_Control(pDX, IDC_DOWN, m_cXYDown);
	DDX_Control(pDX, IDC_BOTTOMRIGHT, m_cXYBotRight);
	DDX_Control(pDX, IDC_RIGHT, m_cXYRight);
	DDX_Control(pDX, IDC_TOPRIGHT, m_cXYTopRight);
	DDX_Control(pDX, IDC_UP, m_cXYUp);

	/*DDX_Control(pDX, IDC_GOTOCWL, m_cGotoRoughZ);
	DDX_Control(pDX, IDC_ZPOSCWL, m_cRoughZPos);
	DDX_Control(pDX, IDC_DOWNCWLZ, m_cDownRoughZ);
	DDX_Control(pDX, IDC_UPCWLZ, m_cUpRoughZ);*/
	
	DDX_Control(pDX, IDC_GOTOZ2, m_cGotoRoughZ);
	DDX_Control(pDX, IDC_ZPOS2, m_cRoughZPos);
	DDX_Control(pDX, IDC_DOWNZ2, m_cDownRoughZ);
	DDX_Control(pDX, IDC_UPZ2, m_cUpRoughZ);

	DDX_Control(pDX, IDC_GOTOCWL, m_cGotoCWLTopZ);
	DDX_Control(pDX, IDC_ZPOSCWL, m_cCWLTopZPos);
	DDX_Control(pDX, IDC_DOWNCWLZ, m_cDownCWLTopZ);
	DDX_Control(pDX, IDC_UPCWLZ, m_cUpCWLTopZ);

	DDX_Control(pDX, IDC_ZPOS, m_cEchoTopZPos);
	DDX_Control(pDX, IDC_GOTOZ, m_cGotoEchoTopZ);
	DDX_Control(pDX, IDC_DOWNZ, m_cDownEchoTopZ);
	DDX_Control(pDX, IDC_UPZ, m_cUpEchoTopZ);
	DDX_Control(pDX, IDC_CALIBPOS2, m_cGotoDPPos2);
	//DDX_Control(pDX, IDC_BGPOSITION, m_cGotoBGPos);
	DDX_Control(pDX, IDC_CALIBPOS, m_cGotoDPPos);
	DDX_Control(pDX, IDC_ORIGIN, m_cGotoZeroXY);
	DDX_Control(pDX, IDC_LOADPOS, m_cLoadPos);
	DDX_Control(pDX, IDC_THINFILMPOS, m_cThinFilmPos);
	DDX_Control(pDX, IDC_ECHOPOS, m_cEchoPos);
	DDX_Text(pDX, IDC_XPOS, m_XPos);
	DDX_Text(pDX, IDC_YPOS, m_YPos);
	DDX_Text(pDX, IDC_ZPOS, m_EchoTopZPos);
	DDX_Text(pDX, IDC_ZPOS2, m_RoughZPos);
	DDX_Text(pDX, IDC_ZPOSCWL, m_CWLTopZPos);
	DDX_Text(pDX, IDC_ANGLE, m_Angle);
	DDX_Text(pDX, IDC_RADUS, m_Radius);
	DDX_Text(pDX, IDC_EDIT1, m_Dx);
	DDX_Text(pDX, IDC_EDIT2, m_DxY);
	DDX_Text(pDX, IDC_EDIT3, m_Dy);
	DDX_Text(pDX, IDC_EDIT4, m_DyX);
	//}}AFX_DATA_MAP
	/*DDX_Control(pDX, IDC_DOWNECHOBOTTOMZ, m_cDownEchoBottomZ);
	DDX_Control(pDX, IDC_UPECHOBOTTOMZ, m_cUpEchoBottomZ);
	DDX_Control(pDX, IDC_ECHOBOTTOMZPOS, m_cEchoBottomZPos);
	DDX_Control(pDX, IDC_GOTOECHOBOTTOMZ, m_cGotoEchoBottomZ);*/
	DDX_Text(pDX, IDC_ECHOBOTTOMZPOS, m_EchoBottomZPos);
	DDX_Control(pDX, IDC_GO_ECHO_PROBE, m_EchoPrb);
	DDX_Control(pDX, IDC_GO_CWL008_PROBE, m_CWL008prb); // 10172024 Morsalin
	DDX_Control(pDX, IDC_GO_CWL030_PROBE, m_CWL030prb); // 10172024 Morsalin
	DDX_Control(pDX, IDC_GO_ROUGHNESS_PROBE, m_RoughnessPrb);
	DDX_Control(pDX, IDC_ECHO_POS_VAL, m_MicroscopePrbPos); // 10172024 Morsalin
	DDX_Control(pDX, IDC_CWL008_POS_VAL, m_CWL008PrbPos); // 10172024 Morsalin
	DDX_Control(pDX, IDC_CWL030_POS_VAL, m_CWL030PrbPos); // 10172024 Morsalin
	DDX_Control(pDX, IDC_ROUGHNESS_POS_VAL, m_RoughnessPrbPos);
	DDX_Text(pDX, IDC_AMOVE_VAL, movingStep);
	DDX_Control(pDX, IDC_GO_ABSOLUTE, m_MoveAbtn);
	DDX_Control(pDX, IDC_GO_RELATIVE_LEFT, m_MoveRLbtn);
	DDX_Control(pDX, IDC_GO_RELATIVE_RIGHT, m_MoveRRbtn);
	DDX_Radio(pDX, IDC_RADIO_1, m_RadioBtn); //02082024/MORSALIN
}

BEGIN_MESSAGE_MAP(CMotionControl, CDialog)
	//{{AFX_MSG_MAP(CMotionControl)
	ON_BN_CLICKED(IDC_Z1X, OnZ1x)
	ON_BN_CLICKED(IDC_Z2X, OnZ2x)
	ON_BN_CLICKED(IDC_Z5X, OnZ5x)
	ON_BN_CLICKED(IDC_Z10X, OnZ10x)
	ON_BN_CLICKED(IDC_Z20X, OnZ20x)
	ON_BN_CLICKED(IDC_Z1X2, OnZ1x2)
	ON_BN_CLICKED(IDC_Z2X2, OnZ2x2)
	ON_BN_CLICKED(IDC_Z5X2, OnZ5x2)
	ON_BN_CLICKED(IDC_Z10X2, OnZ10x2)
	ON_BN_CLICKED(IDC_Z20X2, OnZ20x2)
	ON_BN_CLICKED(IDC_X1, OnX1)
	ON_BN_CLICKED(IDC_X2, OnX2)
	ON_BN_CLICKED(IDC_X5, OnX5)
	ON_BN_CLICKED(IDC_X10, OnX10)
	ON_BN_CLICKED(IDC_X50, OnX50)
	ON_BN_CLICKED(IDC_X100, OnX100)
	ON_BN_CLICKED(IDC_X250, OnX250)
	ON_BN_CLICKED(IDC_X500, OnX500)
	ON_BN_CLICKED(IDC_X1000, OnX1000)
	ON_BN_CLICKED(IDC_UPZ, OnUpz)
	ON_BN_CLICKED(IDC_DOWNZ, OnDownz)
	ON_BN_CLICKED(IDC_UPZ2, OnUpzrough)
	ON_BN_CLICKED(IDC_DOWNZ2, OnDownzrough)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_BN_CLICKED(IDC_GOTOXY, OnGotoxy)
	ON_BN_CLICKED(IDC_GOTOZ, OnGotoechotopz)
	ON_BN_CLICKED(IDC_ADVANCE, OnAdvance)
	ON_BN_CLICKED(IDC_GOTOZ2, OnGotoroughz)
	ON_BN_CLICKED(IDC_ECHOPOS, OnEchopos)
	ON_BN_CLICKED(IDC_THINFILMPOS, OnThinfilmpos)
	ON_BN_CLICKED(IDC_MARK, OnMark)
	ON_BN_CLICKED(IDC_ALIGN, OnAlign)
	ON_BN_CLICKED(IDC_LOADPOS, OnLoadpos)
	ON_BN_CLICKED(IDC_ORIGIN, OnOrigin)
	ON_BN_CLICKED(IDC_CALIBPOS, OnCalibpos)
	ON_BN_CLICKED(IDC_BGPOSITION, OnBgposition)
	ON_BN_CLICKED(IDC_Z20X3, OnZ20x3)
	ON_BN_CLICKED(IDC_CALIBPOS2, OnCalibpos2)
	ON_BN_CLICKED(IDC_TOPLEFT, OnTopleft)
	ON_BN_CLICKED(IDC_TOPRIGHT, OnTopright)
	ON_BN_CLICKED(IDC_BOTTOMRIGHT, OnBottomright)
	ON_BN_CLICKED(IDC_BOTTOMLEFT, OnBottomleft)
	ON_BN_CLICKED(IDC_WAFERMAP, OnWafermap)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GOTORA, OnGotora)
	ON_BN_CLICKED(IDC_X3, OnX3)
	ON_BN_CLICKED(IDC_X101, OnX101)
	ON_BN_CLICKED(IDC_X1001, OnX1001)
	ON_BN_CLICKED(IDC_X4, OnX4)
	ON_BN_CLICKED(IDC_LIGHTS, OnLights)
	ON_BN_CLICKED(IDC_RECIPE, OnRecipe)
	ON_BN_CLICKED(IDC_ECHOCAMERA, OnEchocamera)
	ON_BN_CLICKED(IDC_THINFILMCAMERA, OnThinfilmcamera)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, OnKillfocusEdit4)
	ON_BN_CLICKED(IDC_BUTTON3, OnAddX)
	ON_BN_CLICKED(IDC_BUTTON5, OnMinusX)
	ON_BN_CLICKED(IDC_BUTTON6, OnAddY)
	ON_BN_CLICKED(IDC_BUTTON7, OnMinusY)
	ON_BN_CLICKED(IDC_FFU, OnFfu)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	/*ON_BN_CLICKED(IDC_DOWNECHOBOTTOMZ, &CMotionControl::OnDownEchoBottomZ)
	ON_BN_CLICKED(IDC_UPECHOBOTTOMZ, &CMotionControl::OnUpEchoBottomZ)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ1, &CMotionControl::OnEchoBottomZx1)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ2, &CMotionControl::OnEchoBottomZx2)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ3, &CMotionControl::OnEchoBottomZx3)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ4, &CMotionControl::OnEchoBottomZx4)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ5, &CMotionControl::OnEchoBottomZx5)
	ON_BN_CLICKED(IDC_ECHOBOTTOMZ6, &CMotionControl::OnEchoBottomZx6)*/
	//ON_BN_CLICKED(IDC_GOTOECHOBOTTOMZ, &CMotionControl::OnBnClickedGotoechobottomz)
	ON_BN_CLICKED(IDC_Z20X4, &CMotionControl::OnBnClickedZ20x4)

	ON_BN_CLICKED(IDC_GO_ECHO_PROBE, &CMotionControl::OnBnClickedGoEchoProbe)
	ON_BN_CLICKED(IDC_GO_CWL008_PROBE, &CMotionControl::OnBnClickedGoCwl008Probe) // 10172024 Morsalin
	ON_BN_CLICKED(IDC_GO_CWL030_PROBE, &CMotionControl::OnBnClickedGoCwl030Probe) // 10172024 Morsalin
	ON_BN_CLICKED(IDC_GO_ROUGHNESS_PROBE, &CMotionControl::OnBnClickedGoRoughnessProbe)
	ON_BN_CLICKED(IDC_SET_ECHO_POS, &CMotionControl::OnBnClickedSetEchoPos)
	ON_BN_CLICKED(IDC_SET_CWL008_POS, &CMotionControl::OnBnClickedSetCwl008Pos) // 10172024 Morsalin
	ON_BN_CLICKED(IDC_SET_CWL030_POS, &CMotionControl::OnBnClickedSetCwl030Pos) // 10172024 Morsalin
	ON_BN_CLICKED(IDC_SET_ROUGHNESS_POS, &CMotionControl::OnBnClickedSetRoughnessPos)
	ON_BN_CLICKED(IDC_GO_ABSOLUTE, &CMotionControl::OnBnClickedGoAbsolute)
	ON_BN_CLICKED(IDC_GO_RELATIVE_LEFT, &CMotionControl::OnBnClickedGoRelativeLeft)
	ON_BN_CLICKED(IDC_GO_RELATIVE_RIGHT, &CMotionControl::OnBnClickedGoRelativeRight)
	ON_BN_CLICKED(IDC_RADIO_1, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_5, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_10, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_50, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_100, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_500, &CMotionControl::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_GOTOCWL, &CMotionControl::OnBnClickedGotocwl)
	ON_BN_CLICKED(IDC_Z3X1, &CMotionControl::OnBnClickedZ3x1)
	ON_BN_CLICKED(IDC_Z3X5, &CMotionControl::OnBnClickedZ3x5)
	ON_BN_CLICKED(IDC_Z3X10, &CMotionControl::OnBnClickedZ3x10)
	ON_BN_CLICKED(IDC_Z3X25, &CMotionControl::OnBnClickedZ3x25)
	ON_BN_CLICKED(IDC_Z3X50, &CMotionControl::OnBnClickedZ3x50)
	ON_BN_CLICKED(IDC_Z3X100, &CMotionControl::OnBnClickedZ3x100)
	ON_BN_CLICKED(IDC_Z3X500, &CMotionControl::OnBnClickedZ3x500)
	ON_BN_CLICKED(IDC_UPCWLZ, &CMotionControl::OnBnClickedUpcwlz)
	ON_BN_CLICKED(IDC_DOWNCWLZ, &CMotionControl::OnBnClickedDowncwlz)
		ON_BN_CLICKED(IDC_ModifyProbePos, &CMotionControl::OnBnClickedModifyprobepos)
		END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotionControl message handlers

void CMotionControl::OnZ1x() {
	EchoTopZFac = 0.005f;
}

void CMotionControl::OnZ2x() {
	EchoTopZFac = 0.010f;
}

void CMotionControl::OnZ5x() {
	EchoTopZFac = 0.025f;
}

void CMotionControl::OnZ10x() {
	EchoTopZFac = 0.050f;
}

void CMotionControl::OnZ20x() {
	EchoTopZFac = 0.100f;
}

void CMotionControl::OnZ1x2() {
	RoughZFac = 0.005f;
}

void CMotionControl::OnZ2x2() {
	RoughZFac = 0.010f;
}

void CMotionControl::OnZ5x2() {
	RoughZFac = 0.025f;
}

void CMotionControl::OnZ10x2() {
	RoughZFac = 0.050f;
}

void CMotionControl::OnZ20x2() {
	RoughZFac = 0.100f;
}

// 1 micron [6/23/2012 FSM413]
void CMotionControl::OnX3() {
	Fac = 0.001f;
}

// 2.5 microns [6/23/2012 FSM413]
void CMotionControl::OnX4() {
	Fac = 0.0025f;
}

// 5 microns [6/23/2012 FSM413]
void CMotionControl::OnX1() {
	Fac = 0.005f;
}

// 10 microns [6/23/2012 FSM413]
void CMotionControl::OnX2() {
	Fac = 0.010f;
}

// 25 microns [6/23/2012 FSM413]
void CMotionControl::OnX5() {
	Fac = 0.025f;
}

// 50 microns [6/23/2012 FSM413]
void CMotionControl::OnX10() {
	Fac = 0.050f;
}

// 100 microns [6/23/2012 FSM413]
void CMotionControl::OnX50() {
	Fac = 0.100f;
}

// 500 microns [6/23/2012 FSM413]
void CMotionControl::OnX100() {
	Fac = 0.500f;
}

// 1 mm [6/23/2012 FSM413]
void CMotionControl::OnX101() {
	Fac = 1.000f;
}

// 1.25 mm [6/23/2012 FSM413]
void CMotionControl::OnX250() {
	Fac = 1.250f;
}

// 2.5 mm [6/23/2012 FSM413]
void CMotionControl::OnX500() {
	Fac = 2.500f;
}

// 5 mm [6/23/2012 FSM413]
void CMotionControl::OnX1000() {
	Fac = 5.000f;
}

// 10 mm [6/23/2012 FSM413]
void CMotionControl::OnX1001() {
	Fac = 10.000f;
}

void CMotionControl::OnUpz() {
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cUpEchoTopZ.EnableWindow(FALSE);
	EchoTopZMove(-EchoTopZFac);
	UpdateLocation();
	m_cUpEchoTopZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnDownz() {
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cDownEchoTopZ.EnableWindow(FALSE);
	EchoTopZMove(EchoTopZFac);
	UpdateLocation();
	m_cDownEchoTopZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnUpzrough() {
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cUpRoughZ.EnableWindow(FALSE);
	RoughZMove(-RoughZFac);
	UpdateLocation();
	m_cUpRoughZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnDownzrough() {
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cDownRoughZ.EnableWindow(FALSE);
	RoughZMove(RoughZFac);
	UpdateLocation();
	m_cDownRoughZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnUp() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYUp.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(0, Fac/**dirY*/);
	UpdateLocation();
	m_cXYUp.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnDown() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYDown.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(0, -Fac/**dirY*/);
	UpdateLocation();
	m_cXYDown.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnLeft() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYLeft.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(-Fac/**dirX*/, 0);
	UpdateLocation();
	m_cXYLeft.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnRight() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYRight.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(Fac/**dirX*/, 0);
	UpdateLocation();
	m_cXYRight.EnableWindow(TRUE);
	bBusy = FALSE;
}

// Relative move [6/25/2012 FSM413]
void CMotionControl::Move(float dx, float dy) {
	if (p413App->pMtr) {
		p413App->MouseWait(GetSafeHwnd());
		p413App->pMtr->MoveRXY(dx, dy);
		p413App->MouseNormal(GetSafeHwnd());
	}
}

// Relative move [6/25/2012 FSM413]
void CMotionControl::EchoTopZMove(float dx) {
	if (p413App->pMtr) {
		p413App->MouseWait(GetSafeHwnd());
		p413App->pMtr->MoveREchoTopZ(dx);
		p413App->MouseNormal(GetSafeHwnd());
	}
}

// Relative move [6/25/2012 FSM413]
void CMotionControl::RoughZMove(float dx) {
	if (p413App->pMtr) {
		p413App->MouseWait(GetSafeHwnd());
		p413App->pMtr->MoveRRoughZ(dx);
		p413App->MouseNormal(GetSafeHwnd());
	}
}
// [ //06112024/MORSALIN
void CMotionControl::CWLTopZMove(float dx) {
	if (p413App->pMtr) {
		p413App->MouseWait(GetSafeHwnd());
		p413App->pMtr->MoveRCWLTopZ(dx);
		p413App->MouseNormal(GetSafeHwnd());
	}
}
// ]

void CMotionControl::UpdateLocation(BOOL bUp) {
	static int cnt = 0;
	C413Global* g = &p413App->Global;

	if (bUp) {
		m_XPos = g->LocXY.X;
		m_YPos = g->LocXY.Y;
		m_EchoTopZPos = g->LocEchoTopZ;
		m_RoughZPos = g->LocRoughZ;
		m_CWLTopZPos = g->LocCWLTopZ;
		m_EchoBottomZPos = g->LocEchoBottomZ;
		UpdateData(FALSE);
	}
	UpdateButton(g->ProbeSwitchCurPos); //02082024/MORSALIN
	CString str;
	//02082024/MORSALIN -> Added PrbSwitchMtr and remove RoughZ
	//str.Format("Motion Control: Cur Pos (X=%.3f, Y=%.3f, MicroscopeZ=%.3f, RoughZ=%.3f, CWLZ=%.3f, PrbSwitchMtr=%d) %d", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocRoughZ, g->LocCWLTopZ, g->ProbeSwitchCurPos, cnt++);
	str.Format("Motion Control: Cur Pos (X=%.3f, Y=%.3f, MicroscopeZ=%.3f, CWLZ=%.4f, PrbSwitchMtr=%d) %d", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocCWLTopZ, g->ProbeSwitchCurPos, cnt++);
	SetWindowText(str);
}

BOOL CMotionControl::OnInitDialog() {
	CDialog::OnInitDialog();

	if (p413App->Global.Confg.bEchoTopMotorMod) {
		GetDlgItem(IDC_Z1X)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z2X)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z5X)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z10X)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z20X)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z20X3)->EnableWindow(TRUE);
		m_cUpEchoTopZ.EnableWindow(TRUE);
		m_cDownEchoTopZ.EnableWindow(TRUE);
		m_cEchoTopZPos.EnableWindow(TRUE);
		m_cGotoEchoTopZ.EnableWindow(TRUE);
	}
	//if (p413App->Global.Confg.bEchoBottomMotorMod) {
	//	GetDlgItem(IDC_ECHOBOTTOMZ1)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_ECHOBOTTOMZ2)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_ECHOBOTTOMZ3)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_ECHOBOTTOMZ4)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_ECHOBOTTOMZ5)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_ECHOBOTTOMZ6)->EnableWindow(TRUE);
	//	/*m_cUpEchoBottomZ.EnableWindow(TRUE);
	//	m_cDownEchoBottomZ.EnableWindow(TRUE);
	//	m_cEchoBottomZPos.EnableWindow(TRUE);
	//	m_cGotoEchoBottomZ.EnableWindow(TRUE);*/
	//}
	if (p413App->Global.Confg.bRoughMotorMod) {
		GetDlgItem(IDC_Z1X2)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z2X2)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z5X2)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z10X2)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z20X2)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z20X4)->EnableWindow(TRUE);
		
		m_cUpRoughZ.EnableWindow(TRUE);
		m_cDownRoughZ.EnableWindow(TRUE);
		m_cRoughZPos.EnableWindow(TRUE);
		m_cGotoRoughZ.EnableWindow(TRUE);
		

		//GetDlgItem(IDC_GO_ROUGHNESS_PROBE)->EnableWindow(TRUE);
		//GetDlgItem(IDC_SET_ROUGHNESS_POS)->EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(TRUE);
	}
	// [ 06112024/MORSALIN
	if (p413App->Global.Confg.bConfocalTopMotorMod) {
		GetDlgItem(IDC_Z3X1)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X5)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X10)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X25)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X50)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X100)->EnableWindow(TRUE);
		GetDlgItem(IDC_Z3X500)->EnableWindow(TRUE);
		m_cUpCWLTopZ.EnableWindow(TRUE);
		m_cDownCWLTopZ.EnableWindow(TRUE);
		m_cCWLTopZPos.EnableWindow(TRUE);
		m_cGotoCWLTopZ.EnableWindow(TRUE);
	}
	// ]

	// [ 11042025 MORSALIN
	GetDlgItem(IDC_SET_ECHO_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SET_CWL030_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SET_CWL008_POS)->EnableWindow(FALSE);
	// ]

	// [ 02082024/MORSALIN // 10172024 Morsalin
	m_MicroscopePrbPos.EnableWindow(FALSE);
	m_CWL008PrbPos.EnableWindow(FALSE);
	m_CWL030PrbPos.EnableWindow(FALSE);
	m_RoughnessPrbPos.EnableWindow(FALSE);

	m_MicroscopePrbPos.SetWindowTextA((to_string(p413App->Global.MicroscopePrbPos)).c_str());
	m_CWL008PrbPos.SetWindowTextA((to_string(p413App->Global.CWL008prbPos)).c_str());
	m_CWL030PrbPos.SetWindowTextA((to_string(p413App->Global.CWL030prbPos)).c_str());
	m_RoughnessPrbPos.SetWindowTextA((to_string(p413App->Global.RoughnessPrbPos)).c_str());

	if (!p413App->Global.isPSwitchingSimu) {
		p413App->pPSwitch->CurrentPosition(); // To update dialog header position
		UpdateButton(p413App->Global.ProbeSwitchCurPos); // To update probe button
	}
	else UpdateButton(0);
	// ]

	if (p413App->Global.Confg.bPMotorMod) {
		m_cEchoPos.EnableWindow(TRUE);
		m_cThinFilmPos.EnableWindow(TRUE);
	}

	SetTimer(IDT_START, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMotionControl::OnGotoxy() {
	UpdateData(TRUE);

	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	m_cGotoXY.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->GotoXY(m_XPos, m_YPos);
	m_cGotoXY.EnableWindow(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	UpdateLocation();
	bBusy = FALSE;
}

// Absolute move to position [6/25/2012 FSM413]
void CMotionControl::OnGotoechotopz() {
	UpdateData(TRUE);

	if (bBusy) { return; }
	bBusy = TRUE;
	m_cGotoEchoTopZ.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->MoveEchoTopZ(m_EchoTopZPos);
	UpdateLocation();
	m_cGotoEchoTopZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnGotoroughz() {
	UpdateData(TRUE);

	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	m_cGotoRoughZ.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->MoveRoughZ(m_RoughZPos);
	m_cGotoRoughZ.EnableWindow(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	UpdateLocation();
	bBusy = FALSE;
}

void CMotionControl::OnBnClickedGotocwl()
{
	UpdateData(TRUE);

	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	m_cGotoCWLTopZ.EnableWindow(FALSE);
	if (p413App->pMtr) p413App->pMtr->MoveCWLTopZ(m_CWLTopZPos);
	m_cGotoCWLTopZ.EnableWindow(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	UpdateLocation();
	bBusy = FALSE;
}

void CMotionControl::OnAdvance() {
	CAdvanceOption dlg;

	dlg.pVideo = pVideo;
	dlg.DoModal();
	UpdateLocation();
}

void CMotionControl::OnEchopos() {
	p413App->MouseWait(GetSafeHwnd());
	SetPosControl(FALSE);
	p413App->GotoPositionecho();
	SetPosControl(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	UpdateLocation();
}

void CMotionControl::OnThinfilmpos() {
	p413App->MouseWait(GetSafeHwnd());
	SetPosControl(FALSE);
	p413App->GotoPositionthinfilm();
	SetPosControl(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	UpdateLocation();
}

void CMotionControl::SetPosControl(BOOL bSet) {
	if (p413App->Global.Confg.bPMotorMod) {
		m_cEchoPos.EnableWindow(bSet);
		m_cThinFilmPos.EnableWindow(bSet);
	}
}

void CMotionControl::OnMark() {
	if (pWnd) {
		pWnd->PostMessage(WM_COMMAND, IDC_MARK, NULL);
	}
}

void CMotionControl::OnAlign() {
	CLocDefineDice dlg;
	dlg.DoModal();

	m_Dx = dlg.P[2].x - dlg.P[0].x;
	m_DxY = dlg.P[2].y - dlg.P[0].y;

	m_Dy = dlg.P[1].y - dlg.P[0].y;
	m_DyX = dlg.P[1].x - dlg.P[0].x;

	UpdateData(FALSE);
}

void CMotionControl::OnLoadpos() {
	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	DisableButtons();
	if (!p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) {
		p413App->StageMoveError();
	}
	EnableButtons();
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
	UpdateLocation();
}

void CMotionControl::OnOrigin() {
	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	DisableButtons();
	if (p413App->pMtr) p413App->pMtr->GotoXY(0, 0);
	EnableButtons();
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
	UpdateLocation();
}

void CMotionControl::OnCalibpos() {
	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	DisableButtons();
	// [ 11042025 MORSALIN
	if(p413App->pPSwitch)
		p413App->pPSwitch->MoveToCWL030prb();
	// ]
	if (!p413App->GotoXYS(p413App->Global.DPPos.X, p413App->Global.DPPos.Y, CCamBase::ECHOCAM)) {
		p413App->StageMoveError();
	}
	EnableButtons();
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
	UpdateLocation();
}

void CMotionControl::OnBgposition() {
	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	DisableButtons();
	if (!p413App->GotoXYS(p413App->Global.BGPos.X, p413App->Global.BGPos.Y, CCamBase::THINFCAM)) {
		p413App->StageMoveError();
	}
	EnableButtons();
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
	UpdateLocation();
}

void CMotionControl::DisableButtons() {
	m_cLoadPos.EnableWindow(FALSE);
	m_cGotoZeroXY.EnableWindow(FALSE);
	m_cGotoDPPos.EnableWindow(FALSE);
	m_cGotoDPPos2.EnableWindow(FALSE);
	//m_cGotoBGPos.EnableWindow(FALSE);
}

void CMotionControl::EnableButtons() {
	m_cLoadPos.EnableWindow(TRUE);
	m_cGotoZeroXY.EnableWindow(TRUE);
	m_cGotoDPPos.EnableWindow(TRUE);
	m_cGotoDPPos2.EnableWindow(TRUE);
	//m_cGotoBGPos.EnableWindow(TRUE);
}

void CMotionControl::OnZ20x3() {
	EchoTopZFac = 0.500f;
}

void CMotionControl::OnCalibpos2() {
	if (bBusy) { return; }
	bBusy = TRUE;
	p413App->MouseWait(GetSafeHwnd());
	DisableButtons();
	// [ 11042025 MORSALIN
	if (p413App->pPSwitch)
		p413App->pPSwitch->MoveToCWL008prb();
	// ]
	if (!p413App->GotoXYS(p413App->Global.RoughCalibPos.X, p413App->Global.RoughCalibPos.Y, CCamBase::ECHOCAM)) {
		p413App->StageMoveError();
	}
	EnableButtons();
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
	UpdateLocation();
}

void CMotionControl::OnTopleft() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYTopLeft.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(-Fac/**dirX*/, Fac/**dirY*/);
	UpdateLocation();
	m_cXYTopLeft.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnTopright() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYTopRight.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(Fac/**dirX*/, Fac/**dirY*/);
	UpdateLocation();
	m_cXYTopRight.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnBottomright() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cXYBotRight.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(Fac/**dirX*/, -Fac/**dirY*/);
	UpdateLocation();
	m_cXYBotRight.EnableWindow(TRUE);
	bBusy = FALSE;
}

void CMotionControl::OnBottomleft() {
	if (bBusy) { return; }
	bBusy = TRUE;
	m_cBotLeft.EnableWindow(FALSE);
	//GetMotorDirection();
	Move(-Fac/**dirX*/, -Fac/**dirY*/);
	UpdateLocation();
	m_cBotLeft.EnableWindow(TRUE);
	bBusy = FALSE;
}

// void CMotionControl::GetMotorDirection()
// {
// 	return ; // bypass [6/11/2014 FSMT]
//
// 	if (p413App->Global.bRevX)
// 	{
// 		dirX = -1;
// 	}
// 	else
// 	{
// 		dirX = 1;
// 	}
// 	if (p413App->Global.bRevY)
// 	{
// 		dirY = -1;
// 	}
// 	else
// 	{
// 		dirY = 1;
// 	}
// }

void CMotionControl::OnWafermap() {
	if (!pMap) {
		pMap = new CWaferMapDlg;
		pMap->Create(CWaferMapDlg::IDD);
	}
	pMap->m_cWaferMap.pRcp = &p413App->Global.RcpSetup;
	pMap->m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	pMap->m_cWaferMap.pParent = this;
	pMap->ShowWindow(SW_SHOW);
}

BOOL CMotionControl::DestroyWindow() {
	if (pMap) {
		pMap->ShowWindow(SW_HIDE);
		delete pMap;
		pMap = NULL;
	}
	pGDev->ContinueDoorMonitor();
	return CDialog::DestroyWindow();
}

void CMotionControl::HideWaferMap() {
	if (pMap) {
		pMap->ShowWindow(SW_HIDE);
	}
}

void CMotionControl::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		//KillTimer(IDT_START);
		UpdateLocation();
	}
}

void CMotionControl::OnGotora() {
	UpdateData(TRUE);

	if (bBusy) { return; }
	p413App->MouseWait(GetSafeHwnd());
	m_cGotoRA.EnableWindow(FALSE);
	bBusy = TRUE;
	if (p413App->pMtr) {
		float xPos, yPos;
		p413App->Global.Polar2Cartesian(m_Radius, m_Angle, xPos, yPos);
		switch (p413App->Global.ActiveCamera) {
		default:
			p413App->pMtr->GotoXY(xPos, yPos);
			break;
		case CCamBase::THINFCAM:
			p413App->pMtr->GotoXY(xPos, yPos);
			break;
		}
		UpdateLocation();
	}
	m_cGotoRA.EnableWindow(TRUE);
	p413App->MouseNormal(GetSafeHwnd());
	bBusy = FALSE;
}

void CMotionControl::OnLights() {
	CIllumination dlg;
	dlg.DoModal();
}

void CMotionControl::OnRecipe() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel < CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	p413App->QuickEdit(&p413App->Global.RcpSetup);

	// 	CRecipeIO dlg;
	// 	dlg.DoModal();
}

void CMotionControl::OnEchocamera() {}

void CMotionControl::OnThinfilmcamera() {}

void CMotionControl::OnUpdate() {
	UpdateLocation(TRUE);
}

void CMotionControl::OnKillfocusEdit1() {
	UpdateData(TRUE);
}

void CMotionControl::OnKillfocusEdit2() {
	UpdateData(TRUE);
}

void CMotionControl::OnKillfocusEdit3() {
	UpdateData(TRUE);
}

void CMotionControl::OnKillfocusEdit4() {
	UpdateData(TRUE);
}

void CMotionControl::OnAddX() {
	UpdateLocation(TRUE);
	m_cdXLeft.EnableWindow(FALSE);
	m_XPos += m_Dx;
	m_YPos += m_DxY;
	UpdateData(FALSE);
	OnGotoxy();
	m_cdXLeft.EnableWindow(TRUE);
}

void CMotionControl::OnMinusX() {
	UpdateLocation(TRUE);
	m_cdXRight.EnableWindow(FALSE);
	m_XPos -= m_Dx;
	m_YPos -= m_DxY;
	UpdateData(FALSE);
	OnGotoxy();
	m_cdXRight.EnableWindow(TRUE);
}

void CMotionControl::OnAddY() {
	UpdateLocation(TRUE);
	m_cdYUp.EnableWindow(FALSE);
	m_YPos += m_Dy;
	m_XPos += m_DyX;
	UpdateData(FALSE);
	OnGotoxy();
	m_cdYUp.EnableWindow(TRUE);
}

void CMotionControl::OnMinusY() {
	UpdateLocation(TRUE);
	m_cdYDown.EnableWindow(FALSE);
	m_YPos -= m_Dy;
	m_XPos -= m_DyX;
	UpdateData(FALSE);
	OnGotoxy();
	m_cdYDown.EnableWindow(TRUE);
}

void CMotionControl::OnFfu() {
	OnAddX();
	p413App->pMtr->GetFFUPressure();
	OnMinusX();
}

void CMotionControl::OnClose() {
	pGDev->ContinueDoorMonitor();
	CDialog::OnClose();
}

//[20240426
//void CMotionControl::OnUpEchoBottomZ()
//{
//	if (bBusy) { return; }
//
//	bBusy = TRUE;
//	m_cUpEchoBottomZ.EnableWindow(FALSE);
//	EchoBottomZMove(-EchoBottomZFac);
//	UpdateLocation();
//	m_cUpEchoBottomZ.EnableWindow(TRUE);
//	bBusy = FALSE;
//}
//
//void CMotionControl::OnDownEchoBottomZ()
//{
//	if (bBusy) { return; }
//
//	bBusy = TRUE;
//	m_cDownEchoBottomZ.EnableWindow(FALSE);
//	EchoBottomZMove(EchoBottomZFac);
//	UpdateLocation();
//	m_cDownEchoBottomZ.EnableWindow(TRUE);
//	bBusy = FALSE;
//}
//
//void CMotionControl::EchoBottomZMove(float dx) {
//	if (p413App->pMtr) {
//		p413App->MouseWait(GetSafeHwnd());
//		p413App->pMtr->MoveREchoBottomZ(dx);
//		p413App->MouseNormal(GetSafeHwnd());
//	}
//}
//
//void CMotionControl::OnEchoBottomZx1()
//{
//	EchoBottomZFac = 0.005f;
//}
//void CMotionControl::OnEchoBottomZx2()
//{
//	EchoBottomZFac = 0.010f;
//}
//
//void CMotionControl::OnEchoBottomZx3()
//{
//	EchoBottomZFac = 0.025f;
//}
//
//void CMotionControl::OnEchoBottomZx4()
//{
//	EchoBottomZFac = 0.050f;
//}
//
//void CMotionControl::OnEchoBottomZx5()
//{
//	EchoBottomZFac = 0.100f;
//}
//
//void CMotionControl::OnEchoBottomZx6()
//{
//	EchoBottomZFac = 0.500f;
//}
////]
//
//void CMotionControl::OnBnClickedGotoechobottomz()
//{
//	UpdateData(TRUE);
//
//	if (bBusy) { return; }
//	bBusy = TRUE;
//	p413App->MouseWait(GetSafeHwnd());
//	m_cGotoEchoBottomZ.EnableWindow(FALSE);
//	if (p413App->pMtr) p413App->pMtr->MoveEchoBottomZ(m_EchoBottomZPos);
//	m_cGotoEchoBottomZ.EnableWindow(TRUE);
//	p413App->MouseNormal(GetSafeHwnd());
//	UpdateLocation();
//	bBusy = FALSE;
//}

void CMotionControl::OnBnClickedZ20x4()
{
	RoughZFac = 0.500f;
}

// [ 02082024/MORSALIN // 10172024 Morsalin
void CMotionControl::UpdateButton(int pos)
{
	int EPos = p413App->Global.MicroscopePrbPos;
	int C008Pos = p413App->Global.CWL008prbPos;
	int C030Pos = p413App->Global.CWL030prbPos;
	int RPos = p413App->Global.RoughnessPrbPos;
	if (pos <= EPos + 1 && pos >= EPos - 1) {
		m_EchoPrb.EnableWindow(FALSE);
		m_CWL008prb.EnableWindow(TRUE);
		m_CWL030prb.EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(TRUE);
	}
	else if (pos <= C008Pos + 1 && pos >= C008Pos - 1) {
		m_EchoPrb.EnableWindow(TRUE);
		m_CWL008prb.EnableWindow(FALSE);
		m_CWL030prb.EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(TRUE);
	}
	else if (pos <= C030Pos + 1 && pos >= C030Pos - 1) {
		m_EchoPrb.EnableWindow(TRUE);
		m_CWL030prb.EnableWindow(FALSE);
		m_CWL008prb.EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(TRUE);
	}
	else if (pos <= RPos + 1 && pos >= RPos - 1) {
		m_EchoPrb.EnableWindow(TRUE);
		m_CWL008prb.EnableWindow(TRUE);
		m_CWL030prb.EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(FALSE);
	}
	else {
		m_EchoPrb.EnableWindow(TRUE);
		m_CWL008prb.EnableWindow(TRUE);
		m_CWL030prb.EnableWindow(TRUE);
		m_RoughnessPrb.EnableWindow(TRUE);
	}
	m_RoughnessPrb.EnableWindow(FALSE);
}

void CMotionControl::OnBnClickedGoEchoProbe()
{
	m_EchoPrb.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos = p413App->Global.MicroscopePrbPos;
	}
	else {
		p413App->pPSwitch->MoveToECHOprb();
	}
	m_EchoPrb.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedGoCwl008Probe()
{
	m_CWL008prb.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos = p413App->Global.CWL008prbPos;
	}
	else {
		p413App->pPSwitch->MoveToCWL008prb();
	}
	m_CWL008prb.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedGoCwl030Probe()
{
	m_CWL030prb.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos = p413App->Global.CWL030prbPos;
	}
	else {
		p413App->pPSwitch->MoveToCWL030prb();
	}
	m_CWL030prb.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedGoRoughnessProbe()
{
	m_RoughnessPrb.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos = p413App->Global.RoughnessPrbPos;
	}
	else {
		p413App->pPSwitch->MoveToROUGHNESSprb();
	}

	m_RoughnessPrb.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedSetEchoPos()
{
	if (!p413App->Global.isPSwitchingSimu) {
		int pos;
		if (p413App->pPSwitch->CurrentPosition()) {
			pos = p413App->Global.ProbeSwitchCurPos;
			m_MicroscopePrbPos.SetWindowTextA((to_string(pos)).c_str());
			p413App->Global.MicroscopePrbPos = pos;

			// Updating the ini file
			CString Val, inifile;
			DosUtil.GetLocalCfgFile(inifile);
			Val.Format("%d", pos);
			WritePrivateProfileString("ProbeSwitching", "MicroscopePrbPos", Val, inifile);
		}
	}
}

void CMotionControl::OnBnClickedSetCwl008Pos() // 10172024 Morsalin
{
	if (!p413App->Global.isPSwitchingSimu) {
		int pos;
		if (p413App->pPSwitch->CurrentPosition()) {
			pos = p413App->Global.ProbeSwitchCurPos;
			m_CWL008PrbPos.SetWindowTextA((to_string(pos)).c_str());
			p413App->Global.CWL008prbPos = pos;

			// Updating the ini file
			CString Val, inifile;
			DosUtil.GetLocalCfgFile(inifile);
			Val.Format("%d", pos);
			WritePrivateProfileString("ProbeSwitching", "CWL008prbPos", Val, inifile);
		}
	}
}

void CMotionControl::OnBnClickedSetCwl030Pos() // 10172024 Morsalin
{
	if (!p413App->Global.isPSwitchingSimu) {
		int pos;
		if (p413App->pPSwitch->CurrentPosition()) {
			pos = p413App->Global.ProbeSwitchCurPos;
			m_CWL030PrbPos.SetWindowTextA((to_string(pos)).c_str());
			p413App->Global.CWL030prbPos = pos;

			// Updating the ini file
			CString Val, inifile;
			DosUtil.GetLocalCfgFile(inifile);
			Val.Format("%d", pos);
			WritePrivateProfileString("ProbeSwitching", "CWL030prbPos", Val, inifile);
		}
	}
}

void CMotionControl::OnBnClickedSetRoughnessPos()
{
	if (!p413App->Global.isPSwitchingSimu) {
		int pos;
		if (p413App->pPSwitch->CurrentPosition()) {
			pos = p413App->Global.ProbeSwitchCurPos;
			m_RoughnessPrbPos.SetWindowTextA((to_string(pos)).c_str());
			p413App->Global.RoughnessPrbPos = pos;

			// Updating the ini file
			CString Val, inifile;
			DosUtil.GetLocalCfgFile(inifile);
			Val.Format("%d", pos);
			WritePrivateProfileString("ProbeSwitching", "RoughnessPrbPos", Val, inifile);
		}
	}
}

void CMotionControl::OnBnClickedGoAbsolute()
{
	/*p413App->pPSwitch->ResetAlarm();
	return;*/

	m_MoveAbtn.EnableWindow(FALSE);
	UpdateData(TRUE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos = movingStep;
	}
	else {
		p413App->pPSwitch->MoveA(movingStep);
	}

	m_MoveAbtn.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedGoRelativeLeft()
{
	m_MoveRLbtn.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos -= p413App->Global.RMovingStep;
	}
	else {
		p413App->pPSwitch->MoveR(-p413App->Global.RMovingStep);
	}

	m_MoveRLbtn.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedGoRelativeRight()
{
	m_MoveRRbtn.EnableWindow(FALSE);
	if (p413App->Global.isPSwitchingSimu) {
		p413App->Global.ProbeSwitchCurPos += p413App->Global.RMovingStep;
	}
	else {
		p413App->pPSwitch->MoveR(p413App->Global.RMovingStep);
	}
	m_MoveRRbtn.EnableWindow(TRUE);
	UpdateLocation();
}

void CMotionControl::OnBnClickedRadio()
{
	UpdateData(TRUE);
	C413Global* g = &p413App->Global;
	switch (m_RadioBtn)
	{
	case 0:
		g->RMovingStep = 1;
		break;
	case 1:
		g->RMovingStep = 5;
		break;
	case 2:
		g->RMovingStep = 10;
		break;
	case 3:
		g->RMovingStep = 50;
		break;
	case 4:
		g->RMovingStep = 100;
		break;
	case 5:
		g->RMovingStep = 500;
		break;
	}
}
// ]

// [ 06112024/MORSALIN
void CMotionControl::OnBnClickedZ3x1()
{
	CWLTopZFac = 0.001f;
}

void CMotionControl::OnBnClickedZ3x5()
{
	CWLTopZFac = 0.005f;
}


void CMotionControl::OnBnClickedZ3x10()
{
	CWLTopZFac = 0.01f;
}


void CMotionControl::OnBnClickedZ3x25()
{
	CWLTopZFac = 0.025f;
}


void CMotionControl::OnBnClickedZ3x50()
{
	CWLTopZFac = 0.05f;
}

void CMotionControl::OnBnClickedZ3x100()
{
	CWLTopZFac = 0.1f;
}

void CMotionControl::OnBnClickedZ3x500()
{
	CWLTopZFac = 0.5f;
}

void CMotionControl::OnBnClickedUpcwlz()
{
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cUpCWLTopZ.EnableWindow(FALSE);
	CWLTopZMove(-CWLTopZFac);
	UpdateLocation();
	m_cUpCWLTopZ.EnableWindow(TRUE);
	bBusy = FALSE;
}
// ]

void CMotionControl::OnBnClickedDowncwlz()
{
	if (bBusy) { return; }

	bBusy = TRUE;
	m_cDownCWLTopZ.EnableWindow(FALSE);
	CWLTopZMove(CWLTopZFac);
	UpdateLocation();
	m_cDownCWLTopZ.EnableWindow(TRUE);
	bBusy = FALSE;
}

// [ 11042025 MORSALIN
void CMotionControl::OnBnClickedModifyprobepos()
{
	// TODO: Add your control notification handler code here
	BOOL isEnabled = GetDlgItem(IDC_SET_ECHO_POS)->IsWindowEnabled();
	if(isEnabled)
		GetDlgItem(IDC_SET_ECHO_POS)->EnableWindow(FALSE);
	else 
		GetDlgItem(IDC_SET_ECHO_POS)->EnableWindow(TRUE);

	isEnabled = GetDlgItem(IDC_SET_CWL030_POS)->IsWindowEnabled();
	if (isEnabled)
		GetDlgItem(IDC_SET_CWL030_POS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_SET_CWL030_POS)->EnableWindow(TRUE);

	isEnabled = GetDlgItem(IDC_SET_CWL008_POS)->IsWindowEnabled();
	if (isEnabled)
		GetDlgItem(IDC_SET_CWL008_POS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_SET_CWL008_POS)->EnableWindow(TRUE);
}
// ]