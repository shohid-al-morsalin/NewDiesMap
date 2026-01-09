// SetupProp81Dlg.cpp : implementation file
//

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp81Dlg dialog

CSetupProp81Dlg::CSetupProp81Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp81Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp81Dlg)
	m_Angle1 = 0.0f;
	m_Angle2 = 0.0f;
	m_Angle3 = 0.0f;
	m_Angle4 = 0.0f;
	m_CXOffset = 0.0f;
	m_CYOffset = 0.0f;
	m_CR = 0.0f;
	m_CX = 0.0f;
	m_CY = 0.0f;
	m_Bevel = 0.0f;
	m_AutoExposeValue = 0;
	m_Radius = 0.0f;
	m_Probe1 = 0;
	m_Name = _T("");
	//}}AFX_DATA_INIT

	radius = 149.5f;
	g = NULL;
	pRcp = NULL;
	bRun = FALSE;
	m_cImgWnd1.bScaled = TRUE;
	m_cImgWnd2.bScaled = TRUE;
	m_cVideo2.Name = "Setup81-2";
}

void CSetupProp81Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp81Dlg)
	DDX_Control(pDX, IDC_DOAUTOEXPOSE, m_cAutoExpose);
	DDX_Control(pDX, IDC_CALCULATE, m_cCalc1);
	DDX_Control(pDX, IDC_CALCULATE6, m_cCalc6);
	DDX_Control(pDX, IDC_CALCULATE4, m_cCalc4);
	DDX_Control(pDX, IDC_FINDCXCY, m_cAlgn);
	DDX_Control(pDX, IDC_MESSAGE3, m_cList);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_SELECTEDPT, m_cSelectedPt);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_PROFILE2, m_cProfile2);
	DDX_Control(pDX, IDC_IMAGEVIEW, m_cImgWnd1);
	DDX_Control(pDX, IDC_IMAGEVIEW2, m_cImgWnd2);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_PROFILE, m_cProfile);
	DDX_Control(pDX, IDC_CAMERAB, m_cCameraB);
	DDX_Control(pDX, IDC_CAMERAA, m_cCameraA);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo1);
	DDX_Text(pDX, IDC_ANGLE1, m_Angle1);
	DDX_Text(pDX, IDC_ANGLE2, m_Angle2);
	DDX_Text(pDX, IDC_ANGLE3, m_Angle3);
	DDX_Text(pDX, IDC_ANGLE4, m_Angle4);
	DDX_Control(pDX, IDC_CAMERAVIEW2, m_cVideo2);
	DDX_Text(pDX, IDC_CXOFFSET, m_CXOffset);
	DDX_Text(pDX, IDC_CYOFFSET, m_CYOffset);
	DDX_Text(pDX, IDC_CR, m_CR);
	DDX_Text(pDX, IDC_CX, m_CX);
	DDX_Text(pDX, IDC_CY, m_CY);
	DDX_Text(pDX, IDC_BEVEL, m_Bevel);
	DDX_Text(pDX, IDC_AUTOEXPOSEVALUE, m_AutoExposeValue);
	DDX_Text(pDX, IDC_RADIUS, m_Radius);
	DDX_Text(pDX, IDC_PROBE1, m_Probe1);
	DDX_Text(pDX, IDC_NAME1, m_Name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProp81Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp81Dlg)
	ON_BN_CLICKED(IDC_CAMERAA, OnCameraa)
	ON_BN_CLICKED(IDC_CAMERAB, OnCamerab)
	ON_BN_CLICKED(IDC_CAMERASETTING, OnCamerasetting)
	ON_BN_CLICKED(IDC_MOTIONCONTROL1, OnMotioncontrol)
	ON_BN_CLICKED(IDC_BUT1, OnBut1)
	ON_BN_CLICKED(IDC_BUT2, OnBut2)
	ON_BN_CLICKED(IDC_BUT3, OnBut3)
	ON_BN_CLICKED(IDC_BUT4, OnBut4)
	ON_BN_CLICKED(IDC_BUT5, OnBut5)
	ON_BN_CLICKED(IDC_BUT6, OnBut6)
	ON_BN_CLICKED(IDC_BUT7, OnBut7)
	ON_BN_CLICKED(IDC_BUT8, OnBut8)
	ON_BN_CLICKED(IDC_BUT9, OnBut9)
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_BN_CLICKED(IDC_CALCULATE2, OnCalculate2)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_BN_CLICKED(IDC_GO1, OnGo1)
	ON_BN_CLICKED(IDC_GO2, OnGo2)
	ON_BN_CLICKED(IDC_GO3, OnGo3)
	ON_BN_CLICKED(IDC_GO4, OnGo4)
	ON_COMMAND(IDC_MATCHPATTERN, OnMatchpattern)
	// [20220615 ZHIMING
	ON_BN_CLICKED(IDC_CALCULATE3, OnCalculate3)  // Toggled 12182024
	//ON_BN_CLICKED(IDC_CALCULATE3, OnCalculate3_v1)
	// ]
	ON_BN_CLICKED(IDC_CALCULATE4, OnCalculate4)
	ON_BN_CLICKED(IDC_CALCULATE5, OnCalculate5)
	ON_BN_CLICKED(IDC_CALCULATE6, OnCalculate6)
	ON_BN_CLICKED(IDC_CALCULATE7, OnCalculate7)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_KILLFOCUS(IDC_CXOFFSET, OnKillfocusCxoffset)
	ON_EN_KILLFOCUS(IDC_CYOFFSET, OnKillfocusCyoffset)
	ON_BN_CLICKED(IDC_FINDCXCY, OnFindcxcy)
	ON_EN_KILLFOCUS(IDC_AUTOEXPOSEVALUE, OnKillfocusAutoexposevalue)
	ON_BN_CLICKED(IDC_DOAUTOEXPOSE, OnDoautoexpose)
	ON_EN_KILLFOCUS(IDC_RADIUS, OnKillfocusRadius)
	ON_EN_KILLFOCUS(IDC_PROBE1, OnKillfocusProbe1)
	ON_EN_KILLFOCUS(IDC_NAME1, OnKillfocusName1)
	ON_BN_CLICKED(IDC_VIEWDATA, OnViewdata)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
	ON_BN_CLICKED(IDC_CALCULATE8, &CSetupProp81Dlg::OnBnClickedCalculate8)
	ON_BN_CLICKED(IDC_CALCULATE9, &CSetupProp81Dlg::OnBnClickedCalculate9)
	ON_BN_CLICKED(IDC_BUTTON3, &CSetupProp81Dlg::OnCalculate8)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp81Dlg message handlers
long CSetupProp81Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo1.IsReserved()) {
			m_cVideo1.pWnd = NULL;
			p413App->StopCamera(&m_cVideo1, NULL);
			m_cVideo2.pWnd = NULL;
			StopCamera2(&m_cVideo2);
		}

		LocalToRecipe();
		bTabSelected = FALSE;
	}

	return 0;
}

void CSetupProp81Dlg::LocalToRecipe() {
	UpdateData(TRUE);
}

long CSetupProp81Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		if (!m_cVideo1.IsReserved()) {
			m_cVideo1.bDisableScaling = FALSE;
			m_cVideo2.bDisableScaling = FALSE;
			p413App->StartCamera(&m_cVideo1, CCamBase::ECHOCAM, &m_cList, pRcp);
			m_pVideo = &m_cVideo1;
			m_cVideo1.pWnd = this;
			m_cVideo1.bShowCrossHair = FALSE;
			if (!m_cVideo2.IsSimu()) {
				StartCamera2(CCamBase::ECHOCAM, &m_cVideo2);
				m_cVideo2.pWnd = this;
				m_cVideo2.bShowCrossHair = FALSE;
			}
		}

		RecipeToLocal();

		bTabSelected = TRUE;
	}

	return 0;
}

void CSetupProp81Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp81Dlg::DestroyWindow() {
	m_cVideo1.pWnd = NULL;
	p413App->StopCamera(&m_cVideo1, NULL);
	m_cVideo2.pWnd = NULL;
	StopCamera2(&m_cVideo2);
	return CResizableDialog::DestroyWindow();
}

BOOL CSetupProp81Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_CAMERAA, TOP_RIGHT);
	AddAnchor(IDC_CAMERAB, TOP_RIGHT);
	AddAnchor(IDC_MOTIONCONTROL1, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING, TOP_RIGHT);
	AddAnchor(IDC_STEPHEIGHT, TOP_RIGHT);
	AddAnchor(IDC_FINDSTEPHEIGHT, TOP_RIGHT);

	AddAnchor(IDC_SAVERECIPE, TOP_RIGHT);
	//	AddAnchor(IDC_LOADRECIPE2, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_MESSAGE3, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_VIEWDATA, TOP_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	SetupColumnPos();
	SetupColumnMessage();

	m_Radius = radius = 150.0f;

	UpdateData(FALSE);

	// [02052024 ZHIMING
	if (g->Confg.bThinFilmMicroscopeMod) {
		m_cCameraA.ShowWindow(SW_SHOW);
	}
	// ]

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp81Dlg::SetupColumnPos() {
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

void CSetupProp81Dlg::SetupColumnMessage() {
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

void CSetupProp81Dlg::OnCameraa() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo1);
	p413App->GotoPositionecho();
	p413App->StartCamera(&m_cVideo1, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo1;
	m_cVideo1.pWnd = this;
	m_cVideo1.bShowCrossHair = FALSE;
	//UpdateTargets(&m_cVideo1);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
	RecipeToLocal();
	activeCamera = ECHO_CAMERA;
}

void CSetupProp81Dlg::OnCamerab() {
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo1);
	p413App->GotoPositionthinfilm();
	p413App->StartCamera(&m_cVideo1, CCamBase::THINFCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo1;
	m_cVideo1.pWnd = this;
	m_cVideo1.bShowCrossHair = FALSE;
	//UpdateTargets(&m_cVideo1);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
	RecipeToLocal();
	activeCamera = THIN_FILM_CAMERA;
}

void CSetupProp81Dlg::OnCamerasetting() {
	p413App->CameraSetting(m_cVideo1);
}

void CSetupProp81Dlg::OnMotioncontrol() {
	ShowMotionControl(this);
}

// void CSetupProp81Dlg::OnFindFirstTrim()
// {
// 	m_cVideo1.SnapOneFrame();
// 	CPackedDIB *pDIB = &g->StoredDIB;
// 	if (!pDIB->pDIB)
// 	{
// 		// No image data [8/25/2011 FSMT]
// 		return ;
// 	}
// 	m_cImgWnd1.DupStore1(*pDIB);
// 	int siz = 3*pDIB->pDIB->BmpInfo.biWidth*(pDIB->pDIB->BmpInfo.biBitCount/8);
// 	short *pBuf = new short[siz];
//
// 	BOOL bX;
// 	short len;
// 	short n = pRcp->TrimWidthLines;
// 	for(int i=-n; i<=n; i++)
// 	{
// 		int idx1;
// 		len = m_cImgWnd1.ExtractLine(idx1,pBuf,siz,m_Ang,i*pRcp->TrimWidthSpacing,bX/*,0,255,0*/);
// 		if (i==0)
// 		{
// 			m_cProfile.Set(pBuf,len);  // pBuf is deleted inside m_cProfile [9/14/2011 Administrator]
// 			m_cProfile.Invalidate();
// 		}
// 	}
// 	// do not delete pBuf [9/14/2011 Administrator]
// }

// void CSetupProp81Dlg::OnKillfocusLine()
// {
// 	UpdateData(TRUE);
// }

// void CSetupProp81Dlg::OnNext()
// {
// 	UpdateData(FALSE);
// 	float x,y;
// 	g->Polar2Cartesian(radius,m_Ang,x,y);
// 	if (p413App->pMtr) p413App->pMtr->GotoXY(x,y);
// 	OnFindFirstTrim();
// 	m_Ang += 5;
// 	if (m_Ang > 360)
// 	{
// 		m_Ang -= 360;
// 	}
// }

void CSetupProp81Dlg::OnBut1() {
	float x, y;
	g->Polar2Cartesian(radius, 90.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 90;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut2() {
	float x, y;
	g->Polar2Cartesian(radius, 180.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 180;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut3() {
	float x, y;
	g->Polar2Cartesian(radius, 0.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 0;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut4() {
	float x, y;
	g->Polar2Cartesian(radius, 270.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 270;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut5() {
	float x, y;
	g->Polar2Cartesian(radius, 315.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 315;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut6() {
	float x, y;
	g->Polar2Cartesian(radius, 45.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 45;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut7() {
	float x, y;
	g->Polar2Cartesian(radius, 135.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 135;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut8() {
	float x, y;
	g->Polar2Cartesian(radius, 225.0f, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = 225;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnBut9() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(0, 0);
}

// void CSetupProp81Dlg::OnAuto()
// {
// 	float x,y;
// 	for (int i=0; i< 360; i+=5)
// 	{
// 		g->Polar2Cartesian(radius,i,x,y);
// 		if (p413App->pMtr) p413App->pMtr->GotoXY(x,y);
// 		OnFindFirstTrim();
// 		m_Ang = i;
// 		UpdateData(FALSE);
// 	}
// }

// void CSetupProp81Dlg::Dump(short *pBuf, short len)
// {
// 	CString name;
// 	name.Format("C:\\TEMP\\Profile%.2f_%.1f.CSV",m_Ang,radius);
// 	p413App->Dump2(name,pBuf, len);
// }

// short * CSetupProp81Dlg::Read(short &len)
// {
// 	len=0;
// 	short *pBuf = new short[2*512];
// 	short val;
// 	CString name;
// 	name.Format("C:\\TEMP\\Profile%.2f_%.1f.CSV",m_Ang,radius);
// 	FILE *fp;
// 	fp = fopen(name,"rb");
// 	if (fp)
// 	{
// 		while (fscanf(fp,"%d",&val) != EOF)
// 		{
// 			*(pBuf+len) = val;
// 			len++;
// 			if (len >= 1023)
// 			{
// 				break;
// 			}
// 		}
// 		fclose(fp);
// 		return pBuf;
// 	}
// 	if (pBuf) delete [] pBuf;
// 	return NULL;
// }

// void CSetupProp81Dlg::OnPlus()
// {
// 	m_Ang += 5;
// 	if (m_Ang >= 360)
// 	{
// 		m_Ang -= 360;
// 	}
// 	UpdateData(FALSE);
// 	OnCalculate();
// }

// void CSetupProp81Dlg::OnMinus()
// {
// 	m_Ang -= 5;
// 	if (m_Ang < 0)
// 	{
// 		m_Ang += 360;
// 	}
// 	UpdateData(FALSE);
// 	OnCalculate();
// }

void CSetupProp81Dlg::OnCalculate2() {
	CMPoint mP;
	float ang, rad;

	mP.MeSet = 1;
	MParam* pM = &pRcp->MeParam[mP.MeSet - 1].MPa[0];

	p413App->pMtr->GetXYPos(&mP.Co.x, &mP.Co.y);
	p413App->Global.Cartesian2Polar(mP.Co.x, mP.Co.y, rad, ang);

	switch (pM->Prb) {
	case MParam::WTRIMPRB1:
	{
		CString str;
		float r1, r2;
		if (!m_cVideo2.IsSimu()) {
			r1 = p413App->fMeasureSecondTrimWidthInner(rad, ang, 1, pRcp, &m_cImgWnd2, &m_cProfile, &m_cVideo2, &m_cList);
		}
		else {
			r1 = p413App->fMeasureSecondTrimWidthInner(rad, ang, 1, pRcp, &m_cImgWnd2, &m_cProfile, &m_cVideo1, &m_cList);
		}
		r2 = p413App->fMeasureSecondTrimWidthOuter(ang, 1, pRcp, &m_cImgWnd1, &m_cProfile2, &m_cVideo1, &m_cList);
		str.Format("TW1: %.4fmm  (%.4fmm , %.4fmm)", r2 - r1, r2, r1);
		p413App->Message(&m_cList, str);
	}
	break;
	case MParam::WTRIMPRB2:
	case MParam::WTRIMPRB3:
	{
		short Ret;
		STrimWH St;
		mP.MeSet = 1;
		if (m_cVideo1.IsSimu()) {
			Ret = p413App->MeasureWideTrimWidth(St, &mP, 0, pRcp, &m_cImgWnd1, &m_cVideo1, &m_cImgWnd2, &m_cVideo2, &m_cProfile, &m_cList);
		}
		else {
			Ret = p413App->MeasureWideTrimWidth(St, &mP, 0, pRcp, &m_cImgWnd1, &m_cVideo1, &m_cImgWnd2, &m_cVideo1, &m_cProfile, &m_cList);
		}
		if (Ret >= 0) {
			CString tmsg;
			tmsg.Format("TW2: (%.4fmm , %.4fmm)", St.TW1, St.TW2);
			p413App->Message(&m_cList, tmsg);
		}
	}
	break;
	}
}

// void CSetupProp81Dlg::OnMinus2()
// {
// 	m_Ang -= 5;
// 	if (m_Ang < 0)
// 	{
// 		m_Ang += 360;
// 	}
// 	UpdateData(FALSE);
// 	OnCalculate2();
// }
//
// void CSetupProp81Dlg::OnPlus2()
// {
// 	m_Ang += 5;
// 	if (m_Ang >= 360)
// 	{
// 		m_Ang -= 360;
// 	}
// 	UpdateData(FALSE);
// 	OnCalculate2();
// }

// void CSetupProp81Dlg::OnAuto2()
// {
// 	for (int i=0; i< 360; i+=5)
// 	{
// 		m_Ang = i;
// 		UpdateData(FALSE);
// 		OnCalculate2();
// 	}
// }

void CSetupProp81Dlg::OnSaverecipe() {
	LocalToRecipe();
	RecipeIO();
	// 	if (p413App->ValidCheckRcp(p413App->Global.RcpSetup))
	// 	{
	// 		pRcp->SaveRecipe();
	// 	}
}

// void CSetupProp81Dlg::OnLoadrecipe()
// {
// 	if (LoadRcpSetup())
// 	{
// 		RecipeToLocal();
// 	}
// }

void CSetupProp81Dlg::RecipeToLocal() {
	pRcp->UpdateControl(m_cPoint);
	// [05082024 ZHIMING
	p413App->pMtr->GetActiveCameraOffset(m_CXOffset, m_CYOffset);
	// ]
	MParam* mp = &pRcp->MeParam[0].MPa[0];
	m_Probe1 = mp->Prb;
	m_Name = mp->Name;

	UpdateData(FALSE);
}

void CSetupProp81Dlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
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
	case ID_MODIFYPOINT:
		break;
	case IDC_GOTOPOINT:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co);
			CString str;
			str.Format("%d", pMP->Co.n);
			m_cSelectedPt.SetWindowText(str);
		}
		break;
	case IDC_GOTOXYNOZ:
		if (pMP) {
			GotoPointB(NULL, &pMP->Co, FALSE);
			CString str;
			str.Format("%d", pMP->Co.n);
			m_cSelectedPt.SetWindowText(str);
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

void CSetupProp81Dlg::OnMatchpattern() {
	SDoPattrnMatching Param;

	p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
	Param.Set(&m_cVideo1, &m_cImgWnd1, &m_cList);
	p413App->SetupParam(pRcp, Param, nPatSSet);
	Param.bApplyOffset = FALSE;
	if (pRcp->MatchAlgorithm[g->CurPaSet - 1] == 4) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}

	p413App->bLoadDIB = FALSE;
	p413App->DoPattrnMatching(Param);
	p413App->bLoadDIB = TRUE;
}

void CSetupProp81Dlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cPoint.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMPoint* pMP = FindMPoint(nSel);

	if (pMP) {
		DoSelectMeasurment(pMP, nSel, m_cList, NULL);
	}

	*pResult = 0;
}

void CSetupProp81Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
}

void CSetupProp81Dlg::RelocateToBack(CCoor* p) {
	if (RelocateToBackB(p)) {
		Renumber();
	}
}

void CSetupProp81Dlg::RelocateToTop(CCoor* p) {
	if (RelocateToTopB(p)) {
		Renumber();
	}
}

void CSetupProp81Dlg::UpdatePoint(CCoor* p) {
	if (UpdatePointB(p)) {
		pRcp->UpdateControl(m_cPoint);
	}
}

void CSetupProp81Dlg::Addzposition(CMPoint* pMP) {
	if (AddZPositionB(pMP)) {
		pRcp->UpdateControl(m_cPoint);
	}
}

// void CSetupProp81Dlg::LoadPoint()
// {
// 	pRcp->UpdateControl(m_cPoint);
// }

// void CSetupProp81Dlg::OnStepheight()
// {
// 	CMPoint MP;
// 	MP.Co.n = 1;
// 	if (p413App->pMtr && (!p413App->pMtr->IsSimu()))
// 	{
// 		p413App->pMtr->GetXYPos(&MP.Co.x,&MP.Co.y);
// 	}
// 	else
// 	{
// 		MP.Co.x = 140.0f;
// 		MP.Co.y = 0;
// 	}
// }

void CSetupProp81Dlg::OnGo1() {
	UpdateData(TRUE);
	float x, y;
	g->Polar2Cartesian(radius, m_Angle1, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = m_Angle1;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnGo2() {
	UpdateData(TRUE);
	float x, y;
	g->Polar2Cartesian(radius, m_Angle2, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = m_Angle2;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnGo3() {
	UpdateData(TRUE);
	float x, y;
	g->Polar2Cartesian(radius, m_Angle3, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = m_Angle3;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

void CSetupProp81Dlg::OnGo4() {
	UpdateData(TRUE);
	float x, y;
	g->Polar2Cartesian(radius, m_Angle4, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
	//m_Ang = m_Angle4;
	UpdateData(FALSE);
	//OnFindFirstTrim();
}

long CSetupProp81Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

long CSetupProp81Dlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo1.GetClientRect(&rc);
	return ButClick(wP, lP, rc);
}

// void CSetupProp81Dlg::GrabOneFrame(CSICam &m_Video, short Exposure)
// {
// 	int exposure = m_Video.GetExposure();
// 	m_Video.SetExposure(Exposure);
// 	m_Video.SnapOneFrame(Exposure);
// 	m_Video.SetExposure(exposure);
// }

// void CSetupProp81Dlg::OnExp()
// {
// 	int i;
// 	BOOL bX;
// 	float x,y;
// 	g->Polar2Cartesian(radius,m_Ang,x,y);
// 	if (p413App->pMtr) p413App->pMtr->GotoXY(x,y);
//
// 	GrabOneFrame(m_cVideo1,pRcp->TWExposureInner);
//
// 	CPackedDIB *pDIB = &g->StoredDIB;
// 	m_cImgWnd1.DupStore1(*pDIB);
//
// 	short left,right;
// 	short n = pRcp->TrimWidthLines;
// 	short Offset = pRcp->TrimWidthSpacing;
// 	float *pfTrimWidth = new float[2*n+1];
// 	short cnt = 0;
// 	int siz = 3*pDIB->pDIB->BmpInfo.biWidth*(pDIB->pDIB->BmpInfo.biBitCount/8);
// 	CString tm;
// 	tm = CTime::GetCurrentTime().Format("%H%M%S");
// 	for (i=-n; i<= n; i++)
// 	{
// 		int idx1;
// 		short *pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
// 		short len = m_cImgWnd1.ExtractLine(idx1,pBuf,siz,m_Ang,i*Offset,bX/*,0,255,0*/);
// 		if (len >= siz)
// 		{
// 			ASSERT(0);
// 		}
// 		if (len > 0)
// 		{
// 			p413App->SaveExtractedLine(i,pBuf,len,tm+"-1");
// 			p413App->PreprocessTrimData(&pBuf,len,pRcp->TrimSmooth,TRUE,"");
// 			float TW = p413App->GetTrimWidth(pBuf,len,m_Ang,bX,left,right);
// 			*(pfTrimWidth+(cnt++)) = TW;
// 			if (i==0)
// 			{
// 				short *pDspBuf = new short[len+1];
// 				memcpy(pDspBuf,pBuf,len*sizeof(short));
// 				m_cProfile.Set(pDspBuf,len);
// 				m_cProfile.Invalidate();
// 			}
// 			short *p = pBuf;
// 			int j;
// 			for (j=0;j<left;j++)
// 			{
// 				*(p++) = 0;
// 			}
// 			for (j=left;j<=right;j++)
// 			{
// 				*(p++) = 255;
// 			}
// 			for (j=right+1;j<len;j++)
// 			{
// 				*(p++) = 0;
// 			}
// 			m_cImgWnd1.ReplaceLine(pBuf,m_Ang,i*Offset,bX);
// 		}
// 		if (pBuf)
// 		{
// 			delete [] pBuf;
// 		}
// 	}
// 	m_cImgWnd1.Refresh();
// 	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
// 	float Ave = p413App->fRemoveFlyer(cnt,pfTrimWidth,0,1) * sfx / g->ScrnCalibECH.ppmX;
// 	delete [] pfTrimWidth;
//
// 	CString str;
// 	str.Format("Trim Width = %.2f mm (%d)", Ave, cnt);
// 	m_cMessage.SetWindowText(str);
// }

void CSetupProp81Dlg::OnCalculate5() {
	// Set kI to 999 indicate simulation [5/20/2014 FSMT]
	CString str;
	CMPoint mP;
	float ang, rad, radbak;

	mP.MeSet = 1;
	p413App->pMtr->GetXYPos(&mP.Co.x, &mP.Co.y);
	p413App->Global.Cartesian2Polar(mP.Co.x, mP.Co.y, rad, ang);
	radbak = pRcp->TrimWidthOuterRadius;
	pRcp->TrimWidthOuterRadius = rad;

	MParam* pM = &pRcp->MeParam[mP.MeSet - 1].MPa[0];
	switch (pM->Prb) {
	case MParam::NTRIMPROBE1:
	{
		STrimPARAM Pa;
		Pa.pRcp = pRcp;
		Pa.pMP = &mP;
		Pa.kI = 0;
		Pa.pImgWnd = &m_cImgWnd1;
		Pa.pProfile = &m_cProfile;
		Pa.pVideo = &m_cVideo1;
		Pa.pList = &m_cList;
		str = "Measure first trim width";
		p413App->Message(&m_cList, str);
		p413App->MeasureFirstTrimWidth_v2(Pa);
	}
	break;
	case  MParam::NTRIMPROBE2:
	case  MParam::NTRIMPROBE3:
	{
		str = "Measure narrow trim width";
		p413App->Message(&m_cList, str);
		STrimWH St;
		p413App->MeasureNarrowTrimWidth(St, &mP, 0, pRcp, &m_cImgWnd1, &m_cImgWnd2, &m_cVideo1, &m_cProfile, &m_cList);
	}
	break;
	default:
		str = "No probe specified";
		p413App->Message(&m_cList, str);
		return;
		break;
	}
	pRcp->TrimWidthOuterRadius = radbak;
}

void CSetupProp81Dlg::OnCalculate3_v1() {
	/*==========================================
	PreviousBevelAlgo() - provides the previous bevel width algorithm
						- performance is quite bad for thin bevel
	So, this new algorithms comes. And it works good for both thin bevel and wide bevel
	============================================*/
	float x, y, ang, rad;
	BOOL bX;
	p413App->pMtr->GetXYPos(&x, &y);
	g->Cartesian2Polar(x, y, rad, ang);
	g->Polar2Cartesian(radius, ang, x, y);	// Usig angle information, move to desire radius [7/3/2014 Yuen]
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);

	m_cVideo1.SnapOneFrame(pRcp->TWExposureBevel + g->BaseExpoEcho);
	m_cImgWnd1.DupStore1(g->StoredDIB);
	UpdateData(TRUE);
	CPackedDIB* pDIB = &g->StoredDIB;


	//CPackedDIB* pDIB = m_cImgWnd1.pPackedDIB1;	

	short minLen = short(-1 * pDIB->pDIB->BmpInfo.biHeight * 0.25f);	//biHeight negative
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);
	short* pBuf = new short[siz];
	//short* pCrossLine = new short[siz];
	float* pScore = new float[siz];

	short n = pRcp->TrimWidthLines;
	short offset = pRcp->TrimWidthSpacing;
	short cnt = 0;
	float* pfTrimWidth = new float[2 * n + 1];
	if (pBuf)
	{
		short nL = sqrt(pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biHeight + pDIB->pDIB->BmpInfo.biWidth * pDIB->pDIB->BmpInfo.biWidth) * 0.75;
		//short* lImg = new short[-1 * pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biWidth];

		//SLnX* pL = new SLnX[nL * 2 + 1];
		float* pS1, * pS = pScore;
		float Offset = 1.0f;	//pixel offsets
		ang += 90.0f;
		if (ang >= 360.0f)
		{
			ang -= 360.0f;
		}
		short i, m = 0;
		//SLnX* pLL;
		int idx1;
		float Smin = 0, Smax = 0;
		pS1 = pScore;
		map<int, int>offsetIndex;
		for (i = -nL; i <= nL; i++)
		{
			//pLL = pL + m;
			//pLL->angle = ang;
			//pLL->offset = i * Offset;
			short len = m_cImgWnd1.ExtractLine(idx1, pBuf, siz, ang, i * Offset, bX);
			if (len > minLen)
			{
				offsetIndex[m] = i * Offset;
				*pS = /*pLL->score =*/ m_cImgWnd1.ScoreLine1(pBuf, len);
				if (m)
				{
					if (*pS < Smin) Smin = *pS;
					if (*pS > Smax) Smax = *pS;
				}
				else
				{
					Smin = *pS;	Smax = *pS;
				}
				m++;
				pS++;
			}
		}
		CString msg = "";
		msg.AppendFormat("%.0f, %.0f, ", Smin, Smax);
		Smin = pRcp->FindBevelThres * Smin;
		pS = pScore;
		msg.AppendFormat("%.0f, ", Smin);
		for (int i = 0; i < m; i++)
		{
			TRACE("%f\n", *pS);
			if (*pS > Smin)
			{
				*pS++ = 255;
			}
			else
			{
				*pS++ = 0;
			}
		}

		pS = pScore;
		short firstOffset = -5000, secondOffset = -5000;	//position of first edge and second edge
		short skip = 1;
		for (i = 1; i < (m - 1); i++)
		{
			if (pS[i] == 255.0f && pS[i + 1] == 0.0f)
			{
				firstOffset = offsetIndex[i] - 2;
			}
			else if (pS[i] == 0.0f && pS[i + 1] == 255.0f)
			{
				secondOffset = offsetIndex[i];
				break;
			}
		}

		if (firstOffset != -5000 && secondOffset != -5000)
		{
			m_cImgWnd1.SetLineColor(ang, firstOffset, 0, 255, 0);	//green color the bevel edge
			m_cImgWnd1.SetLineColor(ang, secondOffset, 0, 255, 0); //rgb

			ang -= 90;
			if (ang < 0.0f)
			{
				ang += 360.0f;
			}
			int deviation[5] = { 0, 1, -1, 2, -2 };
			for (i = -n; i <= n; i++)
			{
				short left, right;
				for (int j = 0; j < 5; j++)
				{
					if (m_cImgWnd1.FindColor(idx1, siz, ang, i * offset + deviation[j], bX, left, right))
					{
						short len = m_cImgWnd1.DetectBevel(idx1, pBuf, siz, ang, i * offset + deviation[j], bX, left, right);
						m_cImgWnd1.ReplaceLine(pBuf, ang, i * offset + deviation[j], bX);
						short d = right - left;
						short v = d;
						if (bX) {
							d /= cos(ang * 0.01745329);
						}
						else {
							d /= sin(ang * 0.01745329);
						}
						float TW = abs(d) * sign(long(v));
						*(pfTrimWidth + (cnt++)) = TW;
						break;
					}
				}
			}
		}
		msg.AppendFormat("%d", cnt);
		p413App->Message(&m_cList, msg);
	}
	m_cImgWnd1.Refresh();
	//Back to the previous tpe of camera after measurement bevel width

	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibECH.ppmX;
	delete[] pfTrimWidth;
	delete[] pBuf;
	delete[] pScore;

	CString str;
	str.Format("Trim Width = %.2f um (%d)", Ave, cnt);
	m_cMessage.SetWindowText(str);
}

void CSetupProp81Dlg::OnCalculate3() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);
	int BevelAlgo = GetPrivateProfileInt("Bevel", "UpdateAlgo", 1, inifile);

	if (BevelAlgo == 1) {
		OnCalculate3_v1();
		return;
	}

	int i;
	BOOL bX;
	float x, y, ang, rad;

	p413App->pMtr->GetXYPos(&x, &y);
	g->Cartesian2Polar(x, y, rad, ang);
	g->Polar2Cartesian(radius, ang, x, y);	// Usig angle information, move to desire radius [7/3/2014 Yuen]
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);

	m_cVideo1.SnapOneFrame(pRcp->TWExposureBevel);
	m_cImgWnd1.DupStore1(g->StoredDIB);

	short cnt = 0;
	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;
	float* pfTrimWidth = new float[2 * n + 1];
	CPackedDIB* pDIB = &g->StoredDIB;
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);
	short c = 1;
	for (i = -n; i <= n; i++) {
		int idx1;
		short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
		short len = m_cImgWnd1.ExtractLine(idx1, pBuf, siz, ang, i * Offset, bX/*,0,255,0*/);
		if (len >= siz) {
			ASSERT(0);
		}
		if (len > 0) {
			CString EV;
			EV.Format("%d", c++);
			p413App->PreprocessBevelData(pBuf, len, 6, TRUE, EV);
			short left = 0, right = 0;
			float TW = p413App->GetBevelWidth(pBuf, len, ang, bX, left, right);
			if ((right > 0) && (right < len) && (left > 0) && (left < len) && right > left) {
				*(pfTrimWidth + (cnt++)) = TW;
				if (i == 0) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					m_cProfile.Set(pDspBuf, len);
					m_cProfile.Invalidate();
				}
				short* p = pBuf;
				int j;
				for (j = 0; j < left; j++) {
					*(p++) = 255;
				}
				for (j = left; j <= right; j++) {
					*(p++) = 0;
				}
				for (j = right + 1; j < len; j++) {
					*(p++) = 255;
				}
				m_cImgWnd1.ReplaceLine(pBuf, ang, i * Offset, bX);
			}
		}

		if (pBuf) {
			delete[] pBuf;
		}
	}
	m_cImgWnd1.Refresh();

	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibECH.ppmX;
	delete[] pfTrimWidth;

	CString str;
	str.Format("Trim Width = %.2f um (%d)", Ave, cnt);
	m_cMessage.SetWindowText(str);
}

UINT CSetupProp81Dlg::RunRecipe(LPVOID pObj) {
	CSetupProp81Dlg* pDlg = (CSetupProp81Dlg*)pObj;

	pDlg->BlockUI();
	pDlg->bRun = TRUE;
	pDlg->m_cCalc4.SetWindowText("Stop");
	p413App->Global.bAborting = FALSE;

	// Retrieving points database [9/20/2012 Administrator]
	STrimWH St;
	CPtrList* pMPLst = &pDlg->pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		if (p413App->Global.bAborting) {
			//p413App->Global.bStop = FALSE;
			break;
		}
		CMPoint* pMP = (CMPoint*)pMPLst->GetNext(pos);
		if (pMP) {
			for (int k = 0; k < MaXPA; k++) {
				if (p413App->Global.bAborting) {
					break;
				}
				short Ret = -1;
				if (!pDlg->m_cVideo1.IsSimu()) {
					Ret = p413App->MeasureTrimWidthAndHeight(St, pMP, k, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cImgWnd2, &pDlg->m_cVideo1, &pDlg->m_cVideo1, &pDlg->m_cList);
				}
				else {
					Ret = p413App->MeasureTrimWidthAndHeight(St, pMP, k, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cImgWnd2, &pDlg->m_cVideo1, &pDlg->m_cVideo2, &pDlg->m_cList);
				}
				if (Ret >= 0) {
					CString tmsg;
					tmsg.Format("%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
						St.Rout - St.Rin, St.TTin - St.TTout, St.HTout - St.HTin, St.Oout, St.Oin, St.Rout, St.Rin, St.TTin, St.TTout, St.HTin, St.HTout);
					p413App->Message(&pDlg->m_cList, tmsg);
				}
			}
		}
	}
	pDlg->bRun = FALSE;
	pDlg->m_cCalc4.SetWindowText("Run");
	pDlg->ReleaseUI();
	p413App->Global.bStop = FALSE;
	p413App->Global.bAborting = FALSE;
	AfxEndThread(0);
	return 0;
}

UINT CSetupProp81Dlg::RunRecipeNarrowTrim(LPVOID pObj) {
	CSetupProp81Dlg* pDlg = (CSetupProp81Dlg*)pObj;

	pDlg->BlockUI();
	pDlg->bRun = TRUE;
	pDlg->m_cCalc1.SetWindowText("Stop");
	p413App->Global.bAborting = FALSE;

	// Retrieving points database [9/20/2012 Administrator]
	STrimWH St;
	CPtrList* pMPLst = &pDlg->pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		if (p413App->Global.bAborting) {
			//p413App->Global.bStop = FALSE;
			break;
		}
		CMPoint* pMP = (CMPoint*)pMPLst->GetNext(pos);
		if (pMP) {
			for (int k = 0; k < MaXPA; k++) {
				if (p413App->Global.bAborting) {
					break;
				}
				MParam* pM = &pDlg->pRcp->MeParam[pMP->MeSet - 1].MPa[k];
				switch (pM->Prb) {
				case MParam::NTRIMPROBE1:
				{
					STrimPARAM Pa;
					Pa.pRcp = pDlg->pRcp;
					Pa.pMP = pMP;
					Pa.kI = k;
					Pa.pImgWnd = &pDlg->m_cImgWnd1;
					Pa.pProfile = &pDlg->m_cProfile;
					Pa.pVideo = &pDlg->m_cVideo1;
					Pa.pList = &pDlg->m_cList;
					p413App->MeasureFirstTrimWidth_v2(Pa);
				}
				break;
				case  MParam::NTRIMPROBE2:
				case  MParam::NTRIMPROBE3:
				{
					STrimWH St;
					p413App->MeasureNarrowTrimWidth(St, pMP, k, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cImgWnd2, &pDlg->m_cVideo1, &pDlg->m_cProfile, &pDlg->m_cList);
					CString tmsg;
					tmsg.Format("TW2: %.4fmm , %.4fmm", St.TW1, St.TW2);
					p413App->Message(&pDlg->m_cList, tmsg);
				}
				break;
				default:
					break;
				}
			}
		}
	}
	pDlg->bRun = FALSE;
	pDlg->m_cCalc1.SetWindowText("Run");
	pDlg->ReleaseUI();
	p413App->Global.bStop = FALSE;
	p413App->Global.bAborting = FALSE;
	AfxEndThread(0);
	return 0;
}

UINT CSetupProp81Dlg::RunRecipeWideTrim(LPVOID pObj) {
	CSetupProp81Dlg* pDlg = (CSetupProp81Dlg*)pObj;

	pDlg->BlockUI();
	pDlg->bRun = TRUE;
	pDlg->m_cCalc6.SetWindowText("Stop");
	p413App->Global.bAborting = FALSE;

	// Retrieving points database [9/20/2012 Administrator]
	STrimWH St;
	CPtrList* pMPLst = &pDlg->pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		if (p413App->Global.bAborting) {
			//p413App->Global.bStop = FALSE;
			break;
		}
		CMPoint* pMP = (CMPoint*)pMPLst->GetNext(pos);
		if (pMP) {
			for (int k = 0; k < MaXPA; k++) {
				if (p413App->Global.bAborting) {
					break;
				}
				MParam* pM = &pDlg->pRcp->MeParam[pMP->MeSet - 1].MPa[k];
				switch (pM->Prb) {
				case MParam::WTRIMPRB1:
				{
					CString str;
					float r1, r2, ang, rad;
					p413App->Global.Cartesian2Polar(pMP->Co.x, pMP->Co.y, rad, ang);	// Retrieve angle information [4/14/2013 Administrator]
					r2 = p413App->fMeasureSecondTrimWidthOuter(ang, 1, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cProfile2, &pDlg->m_cVideo1, &pDlg->m_cList);
					if (pDlg->m_cVideo1.IsSimu()) {
						r1 = p413App->fMeasureSecondTrimWidthInner(rad, ang, 1, pDlg->pRcp, &pDlg->m_cImgWnd2, &pDlg->m_cProfile, &pDlg->m_cVideo2, &pDlg->m_cList);
					}
					else {
						r1 = p413App->fMeasureSecondTrimWidthInner(rad, ang, 1, pDlg->pRcp, &pDlg->m_cImgWnd2, &pDlg->m_cProfile, &pDlg->m_cVideo1, &pDlg->m_cList);
					}
					str.Format("TW1: %.4fmm  (%.4fmm , %.4fmm)", r2 - r1, r2, r1);
					p413App->Message(&pDlg->m_cList, str);
				}
				break;
				case MParam::WTRIMPRB2:
				case MParam::WTRIMPRB3:
				{
					short Ret = -1;
					if (pDlg->m_cVideo1.IsSimu()) {
						Ret = p413App->MeasureWideTrimWidth(St, pMP, k, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cVideo1, &pDlg->m_cImgWnd2, &pDlg->m_cVideo2, &pDlg->m_cProfile, &pDlg->m_cList);
					}
					else {
						Ret = p413App->MeasureWideTrimWidth(St, pMP, k, pDlg->pRcp, &pDlg->m_cImgWnd1, &pDlg->m_cVideo1, &pDlg->m_cImgWnd2, &pDlg->m_cVideo1, &pDlg->m_cProfile, &pDlg->m_cList);
					}
					if (Ret >= 0) {
						CString tmsg;
						tmsg.Format("TW2: (%.4fmm , %.4fmm)", St.TW1, St.TW2);
						p413App->Message(&pDlg->m_cList, tmsg);
					}
				}
				break;
				}
			}
		}
	}
	pDlg->bRun = FALSE;
	pDlg->m_cCalc6.SetWindowText("Run");
	pDlg->ReleaseUI();
	p413App->Global.bStop = FALSE;
	p413App->Global.bAborting = FALSE;
	AfxEndThread(0);
	return 0;
}

void CSetupProp81Dlg::OnCalculate4() {
	static CWinThread* pThread = NULL;
	if (!bRun) {
		pThread = AfxBeginThread(CSetupProp81Dlg::RunRecipe, this);
	}
	else {
		p413App->Global.bAborting = TRUE;
		// 		while (p413App->Global.bAborting)
		// 		{
		// 			Sleep(3000);
		// 		}
		// 		m_cCalc4.SetWindowText("Run");
		// 		ReleaseUI();
	}
}

void CSetupProp81Dlg::OnCalculate() {
	static CWinThread* pThread = NULL;
	if (!bRun) {
		pThread = AfxBeginThread(CSetupProp81Dlg::RunRecipeNarrowTrim, this);
	}
	else {
		p413App->Global.bAborting = TRUE;
		// 		while (p413App->Global.bAborting)
		// 		{
		// 			Sleep(3000);
		// 		}
		// 		m_cCalc1.SetWindowText("Run");
		// 		ReleaseUI();
	}
}

// Wide trim calculation [4/15/2013 Administrator]
void CSetupProp81Dlg::OnCalculate6() {
	static CWinThread* pThread = NULL;
	if (!bRun) {
		pThread = AfxBeginThread(CSetupProp81Dlg::RunRecipeWideTrim, this);
	}
	else {
		p413App->Global.bAborting = TRUE;
		// 		while (p413App->Global.bAborting)
		// 		{
		// 			Sleep(3000);
		// 		}
		// 		m_cCalc6.SetWindowText("Run");
		// 		ReleaseUI();
	}
}

void CSetupProp81Dlg::OnCalculate7() {
	STrimWH St;
	CMPoint MP;
	CString tmsg;
	float ang, rad;

	MP.MeSet = 1;

	p413App->pMtr->GetXYPos(&MP.Co.x, &MP.Co.y);
	p413App->Global.Cartesian2Polar(MP.Co.x, MP.Co.y, rad, ang);	// Retrieve angle information [4/14/2013 Administrator]
	tmsg.Format("x=%.3f, y=%.3f, r=%.3f, a=%.3f", MP.Co.x, MP.Co.y, rad, ang);
	p413App->Message(&m_cList, tmsg);

	short Ret = -1;
	if (!m_cVideo1.IsSimu()) {
		Ret = p413App->MeasureTrimWidthAndHeight(St, &MP, 0, pRcp, &m_cImgWnd1, &m_cImgWnd2, &m_cVideo1, &m_cVideo1, &m_cList);
	}
	else {
		Ret = p413App->MeasureTrimWidthAndHeight(St, &MP, 0, pRcp, &m_cImgWnd1, &m_cImgWnd2, &m_cVideo1, &m_cVideo2, &m_cList);
	}
	if (Ret >= 0) {
		tmsg.Format("%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
			St.Rout - St.Rin, St.TTin - St.TTout, St.HTout - St.HTin, St.Oout, St.Oin, St.Rout, St.Rin, St.TTin, St.TTout, St.HTin, St.HTout);
		p413App->Message(&m_cList, tmsg);
	}
}

void CSetupProp81Dlg::OnClear() {
	m_cList.DeleteAllItems();
}

void CSetupProp81Dlg::OnKillfocusCxoffset() {
	UpdateData(TRUE);
	// [05082024 ZHIMING
	p413App->pMtr->GetActiveCameraOffset(m_CXOffset, m_CYOffset);
	// ]
	if (AfxMessageBox("Save to INI", MB_YESNO) == IDYES) {
		g->SaveMotorOption();
	}
}

void CSetupProp81Dlg::OnKillfocusCyoffset() {
	UpdateData(TRUE);
	switch (g->ActiveCamera) {
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
	// [05082024 ZHIMING
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
	if (AfxMessageBox("Save to INI", MB_YESNO) == IDYES) {
		g->SaveMotorOption();
	}
}

// void CSetupProp81Dlg::OnLoadimg()
// {
// 	m_cVideo1.LoadImage();
// 	m_cVideo1.GetAngle(m_Ang);
// 	UpdateData(FALSE);
// 	OnCalculate7();
// }

void CSetupProp81Dlg::OnFindcxcy() {
	m_cAlgn.EnableWindow(FALSE);
	m_cImgWnd1.ClearTextInfo();
	BOOL bFE = FALSE;
	SFINDCENTER S;
	S.pCXOffset = &m_CX;
	S.pCYOffset = &m_CY;
	S.pCRadius = &m_CR;
	S.pRcp = pRcp;
	S.pVideo = &m_cVideo1;
	S.pImgWnd = &m_cImgWnd1;
	S.pList = NULL;
	S.Method = pRcp->FindEdgeType;
	switch (pRcp->FindEdgeMethod) {
	case 2:
		bFE = p413App->FindWaferCenterUsingEdgeDetectionFast(S);
		break;
	default:
		bFE = p413App->FindWaferCenterUsingEdgeDetection(S);
		break;
	}
	if (!bFE) {
		m_cAlgn.EnableWindow(TRUE);
		return;
	}
	// 	p413App->FindWaferCenterUsingEdgeDetection(m_CX,m_CY,m_CR,pRcp,&m_cVideo1,&m_cImgWnd1,NULL);
	m_Bevel = g->BevelWidth;
	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Get(m_CXOffset, m_CYOffset);
		//m_CXOffset = p413App->Global.CXOffsetEcho;
		//m_CYOffset = p413App->Global.CYOffsetEcho;
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Get(m_CXOffset, m_CYOffset);
		//m_CXOffset = p413App->Global.CXOffsetTF;
		//m_CYOffset = p413App->Global.CYOffsetTF;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		g->COffCWL.Get(m_CXOffset, m_CYOffset);
		break;
	case CCamBase::ROUGH:
		g->COffRough.Get(m_CXOffset, m_CYOffset);
		break;
	// ]
	}
	m_cAlgn.EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CSetupProp81Dlg::OnKillfocusAutoexposevalue() {
	// TODO: Add your control notification handler code here
}

void CSetupProp81Dlg::OnDoautoexpose() {
	BlockUI();
	m_cAutoExpose.EnableWindow(FALSE);
	UpdateData(TRUE);
	m_cVideo1.Level = m_AutoExposeValue;
	m_cVideo1.AutoAdjust();
	m_cAutoExpose.EnableWindow(TRUE);
	ReleaseUI();
}

void CSetupProp81Dlg::OnKillfocusRadius() {
	UpdateData(TRUE);
	if ((m_Radius < 0) || (m_Radius > 155.0f)) {
		AfxMessageBox("Radius is between 0 and 155 mm");
		return;
	}
	radius = m_Radius;
}

void CSetupProp81Dlg::OnKillfocusProbe1() {
	UpdateData(TRUE);
	MParam* mp = &pRcp->MeParam[0].MPa[0];
	mp->Prb = (MParam::PRBNUM)m_Probe1;
}

void CSetupProp81Dlg::OnKillfocusName1() {
	UpdateData(TRUE);
	MParam* mp = &pRcp->MeParam[0].MPa[0];
	mp->Name = m_Name;
}

void CSetupProp81Dlg::OnViewdata() {
	CViewData dlg;
	dlg.DoModal();
}

void CSetupProp81Dlg::OnBnClickedCalculate8() {
	CPackedDIB DiB;
	std::vector<SPont> Ln;
	float angle[] = { 0, 30.f, 45.f, 60.f, 90.f, 120.f, 135.f, 150.f };
	//float angle[] = { 180, 210.f, 225.f, 240.f, 270.f, 300.f, 315.f, 330.f };

	Ln.reserve(sqrt(powf(320, 2) + powf(240, 2)));

#if 1
	for (int a = 0; a <= 360; a += 15) {
		DiB.Load("C:\\TEMP\\211111\\1.BMP");
		DiB.GetLine(Ln, a, 320, 240);
		int sz = int(Ln.size());
		float sf = 255 / float(sz);
		for (int i = 0; i < sz; i++) {
			Ln[i].v.R = sf * i;
			Ln[i].v.G = 0;
			Ln[i].v.B = 0;
		}
		DiB.SetLine(Ln);

		float x, y;
		for (int n = -3; n <= 3; n++) {
			Ln.clear();
			DiB.Polar2Cartesian(float(n * 10), float(a), x, y);
			DiB.GetLine(Ln, a + 90, x + 320, y + 240);
			sz = int(Ln.size());
			sf = 255 / float(sz);
			for (int i = 0; i < sz; i++) {
				Ln[i].v.R = 0;
				Ln[i].v.G = sf * i;
				Ln[i].v.B = 0;
			}
			DiB.SetLine(Ln);
		}
		CString str; str.Format("C:\\TEMP\\A%d.BMP", a); DiB.Save(str);
	}
#endif

#if 0
	for (int a = 0; a <= 360; a += 15) {
		DiB.Load("C:\\TEMP\\211111\\1.BMP");
		DiB.GetLine(Ln, a, 320, 240);
		int sz = int(Ln.size());
		float sf = 255 / float(sz);
		for (int i = 0; i < sz; i++) {
			Ln[i].v.R = sf * i;
			Ln[i].v.G = 0;
			Ln[i].v.B = 0;
		}
		DiB.SetLine(Ln);

		float x, y;
		for (int j = -3; j <= 3; j++) {
			Ln.clear();
			DiB.Polar2Cartesian(float(j * 10), float(a), x, y);
			DiB.GetLine(Ln, a + 90, 320 + x, 240 + y);
			sz = int(Ln.size());
			sf = 255 / float(sz);
			for (int i = 0; i < sz; i++) {
				Ln[i].v.R = sf * i;
				Ln[i].v.G = sf * i;
				Ln[i].v.B = 0;
			}
			DiB.SetLine(Ln);
		}
		CString str; str.Format("C:\\TEMP\\B%d.BMP", a); DiB.Save(str);
	}
#endif

	// for (int a = 0; a < 8; a++) {
	// 	DiB.GetLine(Ln, angle[a], 480, 360);
	// 	int sz = int(Ln.size());
	// 	float sf = 255 / float(sz);
	// 	for (int i = 0; i < sz; i++) {
	// 		Ln[i].v.R = sf * i;
	// 		Ln[i].v.G = 0;
	// 		Ln[i].v.B = sf * (sz - i);
	// 	}
	// 	DiB.SetLine(Ln);
	// }
	// for (int a = 0; a < 8; a++) {
	// 	DiB.GetLine(Ln, angle[a]+180, 160, 120);
	// 	int sz = int(Ln.size());
	// 	float sf = 255 / float(sz);
	// 	for (int i = 0; i < sz; i++) {
	// 		Ln[i].v.R = sf * i;
	// 		Ln[i].v.G = 0;
	// 		Ln[i].v.B = sf * (sz - i);
	// 	}
	// 	DiB.SetLine(Ln);
	// }
	//for (int a = 0; a < 8; a++) {
	//	DiB.GetLine(Ln, angle[a] + 180, 160, 360);
	//	int sz = int(Ln.size());
	//	float sf = 255 / float(sz);
	//	for (int i = 0; i < sz; i++) {
	//		Ln[i].v.R = sf * i;
	//		Ln[i].v.G = 0;
	//		Ln[i].v.B = sf * (sz - i);
	//	}
	//	DiB.SetLine(Ln);
	//}
	//for (int a = 0; a < 8; a++) {
	//	DiB.GetLine(Ln, angle[a], 480, 120);
	//	int sz = int(Ln.size());
	//	float sf = 255 / float(sz);
	//	for (int i = 0; i < sz; i++) {
	//		Ln[i].v.R = sf * i;
	//		Ln[i].v.G = 0;
	//		Ln[i].v.B = sf * (sz - i);
	//	}
	//	DiB.SetLine(Ln);
	//}
	//DiB.Save("C:\\TEMP\\A.BMP");
}

void CSetupProp81Dlg::OnBnClickedCalculate9() {
	CPackedDIB DiB;
	SLine L;
	int x = 240, y = 240;
	float angle = 0.0f;

	std::vector<SPont> L2;
	std::vector<SPont>& Ln = L.Ln;
	Ln.reserve(1000);

	CString str;
	for (int i = 1; i <= 4; i++) {
		str.Format("C:\\TEMP\\211111\\%d.BMP", i);
		DiB.Load(str);
		DiB.GetLine(Ln, angle, x, y);
		int sz = int(Ln.size());
		DiB.PlotLine(Ln, 460, 0.2f);
		for (int i = 0; i < sz; i++) {
			SPont& Lx = Ln[i];
			DiB.GetLine(L2, angle + 90, Lx.x, Lx.y);
			L.Score(i, L2);
		}
		DiB.PlotScore(Ln, 460, 0.2f);
		DiB.Filter(Ln);
		str.Format("C:\\TEMP\\%d.CSV", i);
		DiB.Dump(str, Ln);
		for (int i = 0; i < sz; i++) {
			Ln[i].v.R = 255; Ln[i].v.G = 0; Ln[i].v.B = 0;
		}
		int right = 0;
		for (int i = 0; i < sz; i++) {
			if (Ln[i].t == 0) {
				right = i;
				break;
			}
		}
		int left = 0;
		for (int i = sz - 1; i >= 0; i--) {
			if (Ln[i].b == 255) {
				left = i;
				break;
			}
		}
		for (int i = left; i < right; i++) {
			Ln[i].v.R = 0; Ln[i].v.G = 255; Ln[i].v.B = 0;
		}
		DiB.SetLine(Ln);

		//DiB.GetLine(Ln, angle + 90, x, y);
		//for (int i = 0; i < sz; i++) {
		//	Ln[i].v.R = 255; Ln[i].v.G = 255; Ln[i].v.B = 0;
		//}
		//DiB.SetLine(Ln);

		str.Format("C:\\TEMP\\%d.BMP", i);
		DiB.Save(str);
	}
}

short CSetupProp81Dlg::sign(long value)
{
	if (value < 0)
		return -1;
	return 1;
}

void CSetupProp81Dlg::SwitchToEchoCamera()
{
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo1);
	p413App->GotoPositionecho();
	p413App->StartCamera(&m_cVideo1, CCamBase::ECHOCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo1;
	m_cVideo1.pWnd = this;
	m_cVideo1.bShowCrossHair = FALSE;
	//UpdateTargets(&m_cVideo1);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
	RecipeToLocal();
	activeCamera = ECHO_CAMERA;
}

void CSetupProp81Dlg::SwitchToThinFilmCamera()
{
	m_cCameraA.EnableWindow(FALSE);
	m_cCameraB.EnableWindow(FALSE);
	StopAllCamera(&m_cVideo1);
	p413App->GotoPositionthinfilm();
	p413App->StartCamera(&m_cVideo1, CCamBase::THINFCAM, &m_cList, pRcp);
	m_pVideo = &m_cVideo1;
	m_cVideo1.pWnd = this;
	m_cVideo1.bShowCrossHair = FALSE;
	//UpdateTargets(&m_cVideo1);
	m_cCameraA.EnableWindow(TRUE);
	m_cCameraB.EnableWindow(TRUE);
	RecipeToLocal();
}

/*=======================================
06/03/2022 - HAQUE
Perform thin bevel detection using thin film camera
=========================================*/
void CSetupProp81Dlg::OnCalculate8()
{
	// TODO: Add your control notification handler code here
	//active the thin film camera in the dialog
	if (activeCamera != THIN_FILM_CAMERA)
	{
		SwitchToThinFilmCamera();
	}
	float x, y, ang, rad;
	BOOL bX;
	p413App->pMtr->GetXYPos(&x, &y);
	g->Cartesian2Polar(x, y, rad, ang);
	g->Polar2Cartesian(radius, ang, x, y);	// Usig angle information, move to desire radius [7/3/2014 Yuen]
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);

	m_cVideo1.SnapOneFrame(pRcp->TWExposureBevel);
	m_cImgWnd1.DupStore1(g->StoredDIB);

	CPackedDIB* pDIB = &g->StoredDIB;
	short minLen = short(-1 * pDIB->pDIB->BmpInfo.biHeight * 0.25f);	//biHeight negative
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);
	short* pBuf = new short[siz];
	short* pCrossLine = new short[siz];
	float* pScore = new float[siz];

	short n = pRcp->TrimWidthLines;
	short offset = pRcp->TrimWidthSpacing;
	short cnt = 0;
	float* pfTrimWidth = new float[2 * n + 1];
	if (pBuf)
	{
		short nL = sqrt(pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biHeight + pDIB->pDIB->BmpInfo.biWidth * pDIB->pDIB->BmpInfo.biWidth) * 0.75;
		short* lImg = new short[-1 * pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biWidth];

		SLnX* pL = new SLnX[nL * 2 + 1];
		float* pS1, * pS = pScore;
		float Offset = 1.0f;	//pixel offsets
		ang += 90.0f;
		if (ang >= 360.0f)
		{
			ang -= 360.0f;
		}
		short i, m = 0;
		SLnX* pLL;
		int idx1;
		float Smin = 0;
		pS1 = pScore;
		map<int, int>offsetIndex;
		for (i = -nL; i <= nL; i++)
		{
			pLL = pL + m;
			pLL->angle = ang;
			pLL->offset = i * Offset;
			short len = m_cImgWnd1.ExtractLine(idx1, pBuf, siz, ang, i * Offset, bX);
			if (len > minLen)
			{
				offsetIndex[m] = i * Offset;
				*pS = pLL->score = m_cImgWnd1.ScoreLine1(pBuf, len);
				if (m)
				{
					if (*pS < Smin)
					{
						Smin = *pS;
					}
				}
				else
				{
					Smin = *pS;
				}
				m++;
				pS++;
			}
		}
		Smin = 3.5 * Smin;
		pS = pScore;
		for (int i = 0; i < m; i++)
		{
			TRACE("%f\n", *pS);
			if (*pS > Smin)
			{
				*pS++ = 255;
			}
			else
			{
				*pS++ = 0;
			}
		}

		pS = pScore;
		short firstOffset, secondOffset;
		short skip = 1;
		for (i = 1; i < (m - 1); i++)
		{
			if (pS[i] == 255.0f && pS[i + 1] == 0.0f)
			{
				firstOffset = offsetIndex[i] - 2;
			}
			else if (pS[i] == 0.0f && pS[i + 1] == 255.0f)
			{
				secondOffset = offsetIndex[i];
				break;
			}
		}

		m_cImgWnd1.SetLineColor(ang, firstOffset, 0, 255, 0);
		m_cImgWnd1.SetLineColor(ang, secondOffset, 0, 255, 0); //rgb

		ang -= 90;
		if (ang < 0.0f)
		{
			ang += 360.0f;
		}
		int deviation[5] = { 0, 1, -1, 2, -2 };
		for (i = -n; i <= n; i++)
		{
			short left, right;
			for (int j = 0; j < 5; j++)
			{
				if (m_cImgWnd1.FindColor(idx1, siz, ang, i * offset + deviation[j], bX, left, right))
				{
					short len = m_cImgWnd1.DetectBevel(idx1, pBuf, siz, ang, i * offset + deviation[j], bX, left, right);
					m_cImgWnd1.ReplaceLine(pBuf, ang, i * offset + deviation[j], bX);
					short d = right - left;
					short v = d;
					if (bX) {
						d /= cos(ang * 0.01745329);
					}
					else {
						d /= sin(ang * 0.01745329);
					}
					float TW = abs(d) * sign(long(v));
					*(pfTrimWidth + (cnt++)) = TW;
					break;
				}
			}
		}
	}
	m_cImgWnd1.Refresh();
	//Back to the previous tpe of camera after measurement bevel width

	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibECH.ppmX;
	delete[] pfTrimWidth;

	CString str;
	str.Format("Trim Width = %.2f um (%d)", Ave, cnt);
	m_cMessage.SetWindowText(str);
	if (activeCamera != ECHO_CAMERA)
	{
		SwitchToEchoCamera();
	}
}