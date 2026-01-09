#include "stdafx.h"
#include "e95.h"

#include "413App.h"
// #include "Recipe413.h"

#include "CrossHair.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCrossHair::CCrossHair(CWnd* pParent /*=NULL*/)
	: CDialog(CCrossHair::IDD, pParent) {
	//{{AFX_DATA_INIT(CCrossHair)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	n = 3;
	pPX[3] = &x;
	pPY[3] = &Y;
}

void CCrossHair::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrossHair)
	DDX_Control(pDX, IDC_CHECK1, m_cShowCrossHair);
	DDX_Control(pDX, IDC_SPEED, m_cSpeed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCrossHair, CDialog)
	//{{AFX_MSG_MAP(CCrossHair)
	ON_BN_CLICKED(IDC_UPLEFT, OnUpleft)
	ON_BN_CLICKED(IDC_FIRSTTOP, OnFirsttop)
	ON_BN_CLICKED(IDC_UPRIGHT, OnUpright)
	ON_BN_CLICKED(IDC_FIRSTRIGHT, OnFirstright)
	ON_BN_CLICKED(IDC_BOTRIGHT, OnBotright)
	ON_BN_CLICKED(IDC_FIRSTBOTTOM, OnFirstbottom)
	ON_BN_CLICKED(IDC_BOTLEFT, OnBotleft)
	ON_BN_CLICKED(IDC_FIRSTLEFT, OnFirstleft)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	ON_LBN_KILLFOCUS(IDC_SPEED, OnKillfocusSpeed)
	ON_BN_CLICKED(IDC_CHECK1, OnOnOffCursor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrossHair message handlers

BOOL CCrossHair::OnInitDialog() {
	CDialog::OnInitDialog();

	C413Global* g = &p413App->Global;
	CRecipe413* pRcp = &p413App->Global.RcpSetup;
	pPX[0] = &pRcp->fTgtGRnX[p413App->Global.CurPaSet][0];
	pPY[0] = &pRcp->fTgtGRnY[p413App->Global.CurPaSet][0];
	switch (g->ActiveCamera) {
	case CCamBase::THINFCAM:
		pPX[1] = &g->fTgtRedTF.X;
		pPY[1] = &g->fTgtRedTF.Y;
		break;
	// [05072024 ZHIMING
	case CCamBase::CWL:
		pPX[1] = &g->fTgtRedCWL.X;
		pPY[1] = &g->fTgtRedCWL.Y;
		break;
	case CCamBase::ROUGH:
		pPX[1] = &g->fTgtRedRough.X;
		pPY[1] = &g->fTgtRedRough.Y;
		break;
	// ]
	default:
		pPX[1] = &g->fTgtRedECHO.X;
		pPY[1] = &g->fTgtRedECHO.Y;
		break;
	}
	pPX[2] = &pRcp->fTgtBLuX[p413App->Global.CurPaSet][0];
	pPY[2] = &pRcp->fTgtBLuY[p413App->Global.CurPaSet][0];

	dx = 0.001f;

	m_cSpeed.AddString("1");
	m_cSpeed.AddString("2");
	m_cSpeed.AddString("3");
	m_cSpeed.AddString("4");
	m_cSpeed.AddString("5");
	m_cSpeed.AddString("6");

	//	m_cShowCrossHair.SetCheck(p413App->Global.bShowCrossHair);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCrossHair::OnUpleft() {
	SetX(n, -dx);
	SetY(n, -dx);
}

void CCrossHair::OnFirsttop() {
	SetY(n, -dx);
}

void CCrossHair::OnUpright() {
	SetX(n, dx);
	SetY(n, -dx);
}

void CCrossHair::OnFirstright() {
	SetX(n, dx);
}

void CCrossHair::OnBotright() {
	SetX(n, dx);
	SetY(n, dx);
}

void CCrossHair::OnFirstbottom() {
	SetY(n, dx);
}

void CCrossHair::OnBotleft() {
	SetX(n, -dx);
	SetY(n, dx);
}

void CCrossHair::OnFirstleft() {
	SetX(n, -dx);
}

void CCrossHair::OnRed() {
	n = 0;
}

void CCrossHair::OnGreen() {
	n = 1;
}

void CCrossHair::OnBlue() {
	n = 2;
}

void CCrossHair::OnKillfocusSpeed() {
	CString str;
	int i = m_cSpeed.GetCurSel();
	if (i > -1) {
		i++;
		i *= i;
		dx = i * 0.001;
	}
}

void CCrossHair::SetY(short n, float dx) {
	C413Global* g = &p413App->Global;
	switch (n) {
	case 1:
		g->RcpSetup.fTgtGRnY[g->CurPaSet][0] += dx;
		break;
	case 0:
		switch (g->ActiveCamera) {
		case CCamBase::THINFCAM:
			g->fTgtRedTF.Y += dx;
			break;
		// [05072024 ZHIMING
		case CCamBase::CWL:
			g->fTgtRedCWL.Y += dx;
			break;
		case CCamBase::ROUGH:
			g->fTgtRedRough.Y += dx;
			break;
		// ]
		default:
			g->fTgtRedECHO.Y += dx;
			break;
		}
		break;
	case 2:
		g->RcpSetup.fTgtBLuY[g->CurPaSet][0] += dx;
		break;
	}
}

void CCrossHair::SetX(short n, float dx) {
	C413Global* g = &p413App->Global;
	switch (n) {
	case 1:
		g->RcpSetup.fTgtGRnX[g->CurPaSet][0] += dx;
		break;
	case 0:
		switch (g->ActiveCamera) {
		case CCamBase::THINFCAM:
			g->fTgtRedTF.X += dx;
			break;
		// [05072024 ZHIMING
		case CCamBase::CWL:
			g->fTgtRedCWL.X += dx;
			break;
		case CCamBase::ROUGH:
			g->fTgtRedRough.X += dx;
			break;
		// ]
		default:
			g->fTgtRedECHO.X += dx;	// 05072024 ZHIMING change TF -> ECHO
			break;
		}
		break;
	case 2:
		g->RcpSetup.fTgtBLuX[g->CurPaSet][0] += dx;
		break;
	}
}

void CCrossHair::OnOnOffCursor() {
	// 	if (p413App->Global.bShowCrossHair)
	// 	{
	// 		p413App->Global.bShowCrossHair = FALSE;
	// 	}
	// 	else
	// 	{
	// 		p413App->Global.bShowCrossHair = TRUE;
	// 	}
}