// LocDefineDice.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "LocDefineDice.h"
#include "Recipe413.h"
#include "413App.h"
#include "EditPoint.h"
#include "..\413\MotorS2.h"
#include "MotionControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocDefineDice dialog

XYPair CLocDefineDice::P[3];

CLocDefineDice::CLocDefineDice(CWnd* pParent /*=NULL*/)
	: CDialog(CLocDefineDice::IDD, pParent) {
	//{{AFX_DATA_INIT(CLocDefineDice)
	//}}AFX_DATA_INIT
}

void CLocDefineDice::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocDefineDice)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_POSITION3, m_cPos[2]);
	DDX_Control(pDX, IDC_POSITION2, m_cPos[1]);
	DDX_Control(pDX, IDC_POSITION1, m_cPos[0]);
}

BEGIN_MESSAGE_MAP(CLocDefineDice, CDialog)
	//{{AFX_MSG_MAP(CLocDefineDice)
	ON_BN_CLICKED(IDC_GOTO1, OnGoto1)
	ON_BN_CLICKED(IDC_GOTO2, OnGoto2)
	ON_BN_CLICKED(IDC_GOTO3, OnGoto3)
	ON_BN_CLICKED(IDC_SETP1, OnSetp1)
	ON_BN_CLICKED(IDC_SETP2, OnSetp2)
	ON_BN_CLICKED(IDC_SETP3, OnSetp3)
	ON_BN_CLICKED(IDC_AddP1, OnAddP1)
	ON_BN_CLICKED(IDC_AddP2, OnAddP2)
	ON_BN_CLICKED(IDC_AddP3, OnAddP3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocDefineDice message handlers

void CLocDefineDice::OnGoto1() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(P[0].x, P[0].y);
}

void CLocDefineDice::OnGoto2() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(P[1].x, P[1].y);
}

void CLocDefineDice::OnGoto3() {
	if (p413App->pMtr) p413App->pMtr->GotoXY(P[2].x, P[2].y);
}

void CLocDefineDice::OnSetp1() {
	if (p413App->pMtr) p413App->pMtr->GetXYPos(&P[0].x, &P[0].y);
	UpdatePointPos(0);
}

void CLocDefineDice::OnSetp2() {
	if (p413App->pMtr) p413App->pMtr->GetXYPos(&P[1].x, &P[1].y);
	UpdatePointPos(1);
}

void CLocDefineDice::OnSetp3() {
	if (p413App->pMtr) p413App->pMtr->GetXYPos(&P[2].x, &P[2].y);
	UpdatePointPos(2);
}

BOOL CLocDefineDice::OnInitDialog() {
	CDialog::OnInitDialog();

	RefreshDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLocDefineDice::RefreshDialog() {
	UpdatePointPos(0);
	UpdatePointPos(1);
	UpdatePointPos(2);
}

void CLocDefineDice::UpdatePointPos(short n) {
	CString str;
	P[n].Get(str);
	m_cPos[n].SetWindowText(str);
}

void CLocDefineDice::OnAddP1() {
	CEditPoint dlg;
	dlg.m_XCoor = P[0].x;
	dlg.m_YCoor = P[0].y;
	if (dlg.DoModal() == IDOK) {
		P[0].x = dlg.m_XCoor;
		P[0].y = dlg.m_YCoor;
		UpdatePointPos(0);
	}
}

void CLocDefineDice::OnAddP2() {
	CEditPoint dlg;
	dlg.m_XCoor = P[1].x;
	dlg.m_YCoor = P[1].y;
	if (dlg.DoModal() == IDOK) {
		P[1].x = dlg.m_XCoor;
		P[1].y = dlg.m_YCoor;
		UpdatePointPos(1);
	}
}

void CLocDefineDice::OnAddP3() {
	CEditPoint dlg;
	dlg.m_XCoor = P[2].x;
	dlg.m_YCoor = P[2].y;
	if (dlg.DoModal() == IDOK) {
		P[2].x = dlg.m_XCoor;
		P[2].y = dlg.m_YCoor;
		UpdatePointPos(2);
	}
}