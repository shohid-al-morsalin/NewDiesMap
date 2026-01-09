// ManualAlgnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "413App.h"
#include "ManualAlgnDlg.h"
#include "SetupPropComm.h"
#include "..\SYS\CUMMsg.h"
#include "..\413\DMCMotorBase.h"
#include "..\413\MotorS2.h"
#include "..\413\EchoScope\EchoProbeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManualAlgnDlg dialog
#define IDT_STARTMOTOR	9999

// Only support Echo camera [6/24/2012 FSM413]

CManualAlgnDlg::CManualAlgnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManualAlgnDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CManualAlgnDlg)
	m_CarrierID = _T("");
	m_WaferID = _T("");
	m_LotID = _T("");
	m_Operator = _T("");
	//[[20251022_Mohir
	m_WaferSize = _T("");
	m_WaferType = _T("");
	m_Station = _T("");
	//]]
	//}}AFX_DATA_INIT

	pRcp = NULL;
	dx = 0.025f;

	m_cVideo.Name = "ManualAlgn";
}

void CManualAlgnDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManualAlgnDlg)
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Control(pDX, IDC_MOUSEMESSAGE, m_cMouseMessage);
	DDX_Control(pDX, IDC_AUTOFOCUS1, m_cAutoFocus);
	DDX_Control(pDX, IDC_IMGWND, m_cImgWnd);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Text(pDX, IDC_CARRIERID, m_CarrierID);
	DDX_Text(pDX, IDC_WAFERID, m_WaferID);
	DDX_Text(pDX, IDC_LOTID, m_LotID);
	DDX_Text(pDX, IDC_OPERATOR, m_Operator);
	//[[20251022_Mohir
	DDX_Text(pDX, IDC_WAFERTYPE, m_WaferType);
	DDX_Text(pDX, IDC_WAFERSIZE, m_WaferSize);
	DDX_Text(pDX, IDC_STATION, m_Station);
    //]]

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManualAlgnDlg, CDialog)
	//{{AFX_MSG_MAP(CManualAlgnDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_AUTOFOCUS1, OnAutofocus1)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_ZUP, OnZup)
	ON_BN_CLICKED(IDC_ZDOWN, OnZdown)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ZUP2, OnZup2)
	ON_BN_CLICKED(IDC_ZDOWN2, OnZdown2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_MESSAGE(UM_BUTTON_CLICK, OnButClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManualAlgnDlg message handlers

BOOL CManualAlgnDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	if (!pRcp) {
		ASSERT(0);
	}

	C413Global* g = &p413App->Global;
	g->ActiveCamera = CCamBase::ECHOCAM;
	StartCamera(CCamBase::ECHOCAM);
	m_cVideo.SetRecipe(pRcp);
	m_cVideo.SetExposure(pRcp->EchoNormalExposure + g->BaseExpoEcho);
	m_cVideo.pWnd = this;

	if (m_cVideo.IsSimu()) {
		CString dir, name;
		DosUtil.GetProgramDir(dir);
		dir += "\\IMAGES\\BSI\\CENTER2.BMP";
		name = dir;
		m_cVideo.LoadSimuDIB(name);
	}

	SetTimer(IDT_STARTMOTOR, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CManualAlgnDlg::DestroyWindow() {
	m_cVideo.pWnd = NULL;
	StopCamera(/*p413App->Global.ActiveCamera*/);
	return CDialog::DestroyWindow();
}

void CManualAlgnDlg::OnClose() {
	m_cVideo.pWnd = NULL;
	StopCamera(/*p413App->Global.ActiveCamera*/);
	CDialog::OnClose();
}

void CManualAlgnDlg::OnOK() {
	m_cVideo.pWnd = NULL;
	p413App->pDMC->Stop();
	StopCamera(/*p413App->Global.ActiveCamera*/);
	CDialog::OnOK();
}

void CManualAlgnDlg::OnCancel() {
	m_cVideo.pWnd = NULL;
	p413App->pDMC->Stop();
	StopCamera(/*p413App->Global.ActiveCamera*/);
	CDialog::OnCancel();
}

void CManualAlgnDlg::StartCamera(CCamBase::CAMERAID No) {
	if (p413App->StartCameraThread(No, &p413App->Global.RcpSetup, m_cVideo)) {
		if (p413App->pMtr) {
			switch (No) {
			default:
				p413App->pMtr->SetMicroscopeLight(TRUE);
				break;
			case CCamBase::THINFCAM:
				p413App->pMtr->SetSpectroIllumLight(TRUE);
				break;
			}
		}
		p413App->Global.ActiveCamera = No;
		m_cVideo.XX = No;
	}
}

void CManualAlgnDlg::StopCamera() {
	p413App->StopCameraThread(m_cVideo);
}

void CManualAlgnDlg::OnAutofocus1() {
	m_cAutoFocus.EnableWindow(FALSE);
	switch (p413App->Global.ActiveCamera) {
	default:
		p413App->AutoFocusEcho(pRcp, p413App->Global.ActiveCamera, &m_cVideo, pRcp->EchoFocus);
		break;
	case CCamBase::THINFCAM:
		p413App->AutoFocusVideo(p413App->Global.ActiveCamera, 0.5f, 0.5f, &m_cVideo);
		break;
	}
	m_cAutoFocus.EnableWindow(TRUE);
}

void CManualAlgnDlg::OnRadio1() {
	dx = 0.030f;
}

void CManualAlgnDlg::OnRadio2() {
	dx = 0.0025f;
}

void CManualAlgnDlg::OnZup() {
	p413App->pMtr->MoveREchoTopZ(-dx);
}

void CManualAlgnDlg::OnZdown() {
	p413App->pMtr->MoveREchoTopZ(dx);
}

void CManualAlgnDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_STARTMOTOR) {
		KillTimer(IDT_STARTMOTOR);
		// 		if (m_cVideo.IsSimu())
		// 		{
		// 			CString dir;
		// 			DosUtil.GetProgramDir(dir);
		// 			dir = dir + "P2.BMP";
		// 			m_cVideo.LoadSimuDIB(dir);
		// 		}

		if (!p413App->Global.bStageReset) {
			if (p413App->pMtr->ResetAll()) {
				p413App->Global.ResetLapTime = GetTickCount64();
			}
		}
		if (pRcp) {
			p413App->pMtr->MoveEchoTopZ(pRcp->AlgnZPos);
			p413App->pMtr->GotoXY(pRcp->AlgnXPos, pRcp->AlgnYPos);
			p413App->pMtr->WaitEchoTopZ();
		}
		p413App->pDMC->Run();
		p413App->pEcho->StartGetPeaks(NULL);
		p413App->pEcho->SetRecipe(pRcp);
		m_cOK.ShowWindow(SW_SHOW);
	}
}

long CManualAlgnDlg::OnMoMove(WPARAM wP, LPARAM lP) {
	CString str;
	CSICamPoints* pP = (CSICamPoints*)lP;
	CRect rc;
	C413Global* g = &p413App->Global;

	m_cVideo.GetClientRect(&rc);
	float sfx = g->ScrnCalibECH.ScrnX / float(rc.Width());
	float sfy = g->ScrnCalibECH.ScrnY / float(rc.Height());
	switch (g->ActiveCamera) {
	default:
		str.Format("(%d,%d)px (%.1f,%.1f)um",
			pP->CurPt.x, pP->CurPt.y,
			(pP->EndPt.x - pP->StartPt.x) * sfx / g->ScrnCalibECH.ppmX, (pP->EndPt.y - pP->StartPt.y) * sfy / g->ScrnCalibECH.ppmY);
		break;
	case CCamBase::THINFCAM:
		str.Format("(%d,%d)px (%.1f,%.1f)um",
			pP->CurPt.x, pP->CurPt.y,
			(pP->EndPt.x - pP->StartPt.x) * sfx / g->ScrnCalibTF.ppmX, (pP->EndPt.y - pP->StartPt.y) * sfy / g->ScrnCalibTF.ppmY);
		break;
	}
	m_cMouseMessage.SetWindowText(str);
	return 0;
}

long CManualAlgnDlg::OnButClick(WPARAM wP, LPARAM lP) {
	CRect rc;
	m_cVideo.GetClientRect(&rc);
	return p413App->DeltaMove((CPoint*)lP, p413App->Global.ActiveCamera, rc);
}

void CManualAlgnDlg::OnZup2() {
	p413App->pMtr->MoveREchoTopZ(-dx * 0.2f);
}

void CManualAlgnDlg::OnZdown2() {
	p413App->pMtr->MoveREchoTopZ(dx * 0.2f);
}