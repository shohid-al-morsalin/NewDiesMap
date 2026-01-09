#include "stdafx.h"
#include "E95.h"

#include "413/MotorL2.h"
#include "413/MotorS2.h"
#include "413App.h"
//#include <math.h>

#include "Alignment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlignment dialog

CAlignment::CAlignment(CWnd* pParent /*=NULL*/)
	: CDialog(CAlignment::IDD, pParent) {
	//{{AFX_DATA_INIT(CAlignment)
	m_LeftXE = 0.0f;
	m_LeftXT = 0.0f;
	m_LeftYE = 0.0f;
	m_LeftYT = 0.0f;
	m_RightXE = 0.0f;
	m_RightXT = 0.0f;
	m_RightYE = 0.0f;
	m_RightYT = 0.0f;
	m_OffsetAngle = 22.5f;
	//}}AFX_DATA_INIT
}

void CAlignment::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlignment)
	DDX_Control(pDX, IDC_RESULT, m_cResult);
	DDX_Text(pDX, IDC_LEFTXE, m_LeftXE);
	DDX_Text(pDX, IDC_LEFTXT, m_LeftXT);
	DDX_Text(pDX, IDC_LEFTYE, m_LeftYE);
	DDX_Text(pDX, IDC_LEFTYT, m_LeftYT);
	DDX_Text(pDX, IDC_RIGHTXE, m_RightXE);
	DDX_Text(pDX, IDC_RIGHTXT, m_RightXT);
	DDX_Text(pDX, IDC_RIGHTYE, m_RightYE);
	DDX_Text(pDX, IDC_RIGHTYT, m_RightYT);
	DDX_Text(pDX, IDC_OFFSETANGLE, m_OffsetAngle);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlignment, CDialog)
	//{{AFX_MSG_MAP(CAlignment)
	ON_BN_CLICKED(IDC_LEFTREADE, OnLeftreade)
	ON_BN_CLICKED(IDC_LEFTREADT, OnLeftreadt)
	ON_BN_CLICKED(IDC_RIGHTREADE, OnRightreade)
	ON_BN_CLICKED(IDC_RIGHTREADT, OnRightreadt)
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_BN_CLICKED(IDC_CALCULATE4, OnCalculate4)
	ON_BN_CLICKED(IDC_RESETANGLE, OnResetangle)
	ON_EN_KILLFOCUS(IDC_OFFSETANGLE, OnKillfocusOffsetangle)
	ON_BN_CLICKED(IDC_CALCULATEX, OnCalculatex)
	ON_BN_CLICKED(IDC_CALCULATEY, OnCalculatey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignment message handlers

// Read left marker of Echo probe [10/28/2012 Yuen]
void CAlignment::OnLeftreade() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		// Use Echo camera coordinate only [6/28/2012 FSM413]
		float pos;
		if (pMtr->pMX->bGetPos(&pos)) {
			m_LeftXE = pos;
		}
		if (pMtr->pMY->bGetPos(&pos)) {
			m_LeftYE = pos;
		}
		UpdateData(FALSE);
	}
}

// Read left marker of TF probe [10/28/2012 Yuen]
void CAlignment::OnLeftreadt() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		// Use Echo camera coordinate only [6/28/2012 FSM413]
		float pos;
		if (pMtr->pMX->bGetPos(&pos)) {
			m_LeftXT = pos;
		}
		if (pMtr->pMY->bGetPos(&pos)) {
			m_LeftYT = pos;
		}
		UpdateData(FALSE);
	}
}

// Read right marker of Echo probe [10/28/2012 Yuen]
void CAlignment::OnRightreade() {
	CMotorS2* pMtr = p413App->pMtr;

	if (pMtr) {
		// Use Echo camera coordinate only [6/28/2012 FSM413]
		float pos;
		if (pMtr->pMX->bGetPos(&pos)) {
			m_RightXE = pos;
		}
		if (pMtr->pMY->bGetPos(&pos)) {
			m_RightYE = pos;
		}
		UpdateData(FALSE);
	}
}

// Read right marker of TF probe [10/28/2012 Yuen]
void CAlignment::OnRightreadt() {
	CMotorS2* pMtr = p413App->pMtr;

	if (pMtr) {
		// Use Echo camera coordinate only [6/28/2012 FSM413]
		float pos;
		if (pMtr->pMX->bGetPos(&pos)) {
			m_RightXT = pos;
		}
		if (pMtr->pMY->bGetPos(&pos)) {
			m_RightYT = pos;
		}
		UpdateData(FALSE);
	}
}

void CAlignment::OnCalculate() {
	UpdateData(TRUE);

	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return;
	}
	if (AfxMessageBox("System coordinate will be change, Proceed?", MB_YESNO) == IDYES) {
		float posX = (m_RightXE + m_LeftXE) / 2.0f;
		float posY = (m_RightYE + m_LeftYE) / 2.0f;
		CMotorParam* pParaX = &pMtr->pMX->Param;
		CMotorParam* pParaY = &pMtr->pMY->Param;
		//if (pParaX->bPolarity)
		//{
		pParaX->HomePos1 -= posX;
		UpdateFixPosX(-posX);
		//}
		//else
		//{
		//	pParaX->HomePos1 += posX;
		//	UpdateFixPosX(posX);
		//}
		pParaX->MaxPos1 = pParaX->HomePos1 + 150.0f;
		pParaX->MinPos1 = pParaX->HomePos1 - 150.0f;
		//if (pParaY->bPolarity)
		//{
		pParaY->HomePos1 -= posY;
		UpdateFixPosY(-posY);
		//}
		//else
		//{
		//	pParaY->HomePos1 += posY;
		//	UpdateFixPosY(posY);
		//}
		pParaY->MaxPos1 = pParaY->HomePos1 + 150.0f;
		pParaY->MinPos1 = pParaY->HomePos1 - 150.0f;

		//pParaX->Angle = atan((m_RightYE-m_LeftYE) / (m_RightXE-m_LeftXE)) - atan(m_OffsetAngle * 3.141592654f / 180.0f);
		//pParaX->Tx = 0;
		//pParaX->Ty = 0;
		pParaX->TxR.Set(0, 0, 0);
		pParaY->TxR.Set(0, 0, 0);

		pMtr->pMX->vSaveParameter("XMotor");
		pMtr->pMY->vSaveParameter("YMotor");

		UpdateDialog();

		pMtr->ResetXY();
		p413App->GotoXYS(0, 0, CCamBase::ECHOCAM);
	}
}

BOOL CAlignment::OnInitDialog() {
	CDialog::OnInitDialog();

	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		CMotorParam* pParaX = &pMtr->pMX->Param;
		CMotorParam* pParaY = &pMtr->pMY->Param;
		oAngle = pParaX->TxR.Tr * 180.0f / 3.141592654f;
		Tx1 = pParaX->TxR.Tx;
		Tx2 = pParaY->TxR.Tx;
		Ty1 = pParaX->TxR.Ty;
		Ty2 = pParaY->TxR.Ty;
	}

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

// UMC [6/24/2012 FSM413]
// TF Left(-252.949,0.232114), Right(47.1614,0.272417)
void CAlignment::UpdateDialog() {
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) { return; }
	CMotorParam* pParaX = &pMtr->pMX->Param;
	CMotorParam* pParaY = &pMtr->pMY->Param;

	CString str;
	m_cResult.ResetContent();
	str.Format("Echo: HomePosX=%.4fmm  HomePosY=%.4fmm", pParaX->HomePos1, pParaY->HomePos1);
	m_cResult.AddString(str);
	str.Format("Echo: Tx=%.4fmm  Ty=%.4fmm, Rotation=%.4fmR", pParaX->TxR.Tx, pParaX->TxR.Ty, pParaX->TxR.Tr);
	m_cResult.AddString(str);
	str.Format("Thin Film: HomePosX=%.4fmm  HomePosY=%.4fmm", pParaX->HomePos2, pParaY->HomePos2);
	m_cResult.AddString(str);
	str.Format("Thin Film: Tx=%.4fmm  Ty=%.4fmm, Rotation=%.4fmR", pParaY->TxR.Tx, pParaY->TxR.Ty, pParaY->TxR.Tr);
	m_cResult.AddString(str);
}

void CAlignment::OnCalculate4() {
	if (AfxMessageBox("System coordinate will be change, Proceed?", MB_YESNO) == IDYES) {
		CMotorS2* pMtr = p413App->pMtr;
		if (!pMtr) { return; }
		float posX2 = (m_RightXT + m_LeftXT) / 2.0f;
		float posY2 = (m_RightYT + m_LeftYT) / 2.0f;

		CMotorParam* pParaX = &pMtr->pMX->Param;
		CMotorParam* pParaY = &pMtr->pMY->Param;

		pParaX->HomePos2 = pParaX->HomePos1 + posX2;
		pParaX->MaxPos2 = pParaX->HomePos2 + 150.0f;
		pParaX->MinPos2 = pParaX->HomePos2 - 150.0f;
		pParaY->HomePos2 = pParaY->HomePos1 + posY2;
		pParaY->MaxPos2 = pParaY->HomePos2 + 150.0f;
		pParaY->MinPos2 = pParaY->HomePos2 - 150.0f;

		pParaY->TxR.Tx = posX2;
		pParaY->TxR.Ty = posY2;

		pMtr->pMX->vSaveParameter("XMotor");
		pMtr->pMY->vSaveParameter("YMotor");

		UpdateDialog();
	}
}

void CAlignment::OnCancel() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		CMotorParam* pParaX = &pMtr->pMX->Param;
		CMotorParam* pParaY = &pMtr->pMY->Param;
		pParaX->TxR.Tr = oAngle * 3.141592654f / 180.0f;
		pParaX->TxR.Tx = Tx1;
		pParaY->TxR.Tx = Tx2;
		pParaX->TxR.Ty = Ty1;
		pParaY->TxR.Ty = Ty2;
	}
	CDialog::OnCancel();
}

void CAlignment::OnResetangle() {
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		CMotorParam* pParaX = &pMtr->pMX->Param;
		pParaX->TxR.Tr = 0;
		pMtr->pMX->vSaveParameter("XMotor");
		UpdateDialog();
	}
}

void CAlignment::OnKillfocusOffsetangle() {
	UpdateData(TRUE);
}

void CAlignment::OnCalculatex() {
	UpdateData(TRUE);

	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return;
	}
	if (AfxMessageBox("System coordinate will be change, Proceed?", MB_YESNO) == IDYES) {
		float posX = (m_RightXE + m_LeftXE) / 2.0f;
		CMotorParam* pParaX = &pMtr->pMX->Param;
		//if (pParaX->bPolarity)
		//{
		//	pParaX->HomePos1 += posX;
		//	UpdateFixPosX(posX);
		//}
		//else
		//{
		pParaX->HomePos1 -= posX;
		UpdateFixPosX(-posX);
		//}
		pParaX->MaxPos1 = pParaX->HomePos1 + 150.0f;
		pParaX->MinPos1 = pParaX->HomePos1 - 150.0f;

		pParaX->TxR.Set(0, 0, 0);

		pMtr->pMX->vSaveParameter("XMotor");

		UpdateDialog();

		pMtr->ResetXY();
		p413App->GotoXYS(0, 0, CCamBase::ECHOCAM);
	}
}

void CAlignment::OnCalculatey() {
	UpdateData(TRUE);

	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return;
	}
	if (AfxMessageBox("System coordinate will be change, Proceed?", MB_YESNO) == IDYES) {
		float posY = (m_RightYE + m_LeftYE) / 2.0f;
		CMotorParam* pParaY = &pMtr->pMY->Param;
		//if (pParaY->bPolarity)
		//{
		//	pParaY->HomePos1 += posY;
		//	UpdateFixPosY(posY);
		//}
		//else
		//{
		pParaY->HomePos1 -= posY;
		UpdateFixPosY(-posY);
		//}
		pParaY->MaxPos1 = pParaY->HomePos1 + 150.0f;
		pParaY->MinPos1 = pParaY->HomePos1 - 150.0f;

		pParaY->TxR.Set(0, 0, 0);

		pMtr->pMY->vSaveParameter("YMotor");

		UpdateDialog();

		pMtr->ResetXY();
		p413App->GotoXYS(0, 0, CCamBase::ECHOCAM);
	}
}

void CAlignment::UpdateFixPosX(float dX) {
	return;	// Do not modify [7/11/2014 Yuen]

	C413Global* g = &p413App->Global;
	g->LoadPos.PX += dX;
	g->DPPos.X += dX;
	g->BGPos.X += dX;
	g->TFPos.X += dX;
	g->fLPosX += dX;
	g->Save();
	g->SaveMotorOption();
}

void CAlignment::UpdateFixPosY(float dY) {
	return;	// Do not modify [7/11/2014 Yuen]

	C413Global* g = &p413App->Global;
	g->LoadPos.PY += dY;
	g->DPPos.Y += dY;
	g->BGPos.Y += dY;
	g->TFPos.Y += dY;
	g->fLPosY += dY;
	g->Save();
	g->SaveMotorOption();
}