// EchoProbeProp.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EchoProbeProp.h"
#include "Recipe413.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEchoProbeProp dialog

CEchoProbeProp::CEchoProbeProp(CWnd* pParent /*=NULL*/)
	: CDialog(CEchoProbeProp::IDD, pParent) {
	//{{AFX_DATA_INIT(CEchoProbeProp)
	m_NAve = 1;
	//}}AFX_DATA_INIT

	m_ThresholdFactor[0] = m_ThresholdFactor[1] = 10.0f;
	m_Level[0] = m_Level[1] = 20;
	m_PeakWidth[0] = m_PeakWidth[1] = 80;

	pRcp = NULL;

	MeSet = 0;
	MapCol = 0;

	for (int i = 0; i < MaXPA; i++) {
		m_L[i] = 0;
		m_H[i] = 0;
		m_F[i] = 1;
		m_T[i] = 1;
	}
}

void CEchoProbeProp::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEchoProbeProp)
	DDX_Text(pDX, IDC_NAVE, m_NAve);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_THRESHOLDFACTOR1, m_ThresholdFactor[0]);
	DDX_Text(pDX, IDC_THRESHOLDFACTOR2, m_ThresholdFactor[1]);
	DDX_Text(pDX, IDC_RCP_LEVEL1, m_Level[0]);
	DDX_Text(pDX, IDC_RCP_LEVEL2, m_Level[1]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH1, m_PeakWidth[0]);
	DDX_Text(pDX, IDC_RCP_PEAKWIDTH2, m_PeakWidth[1]);

	DDX_Text(pDX, IDC_L1, m_L[0]);
	DDX_Text(pDX, IDC_L2, m_L[1]);
	DDX_Text(pDX, IDC_L3, m_L[2]);
	DDX_Text(pDX, IDC_L4, m_L[3]);
	DDX_Text(pDX, IDC_L5, m_L[4]);
	DDX_Text(pDX, IDC_L6, m_L[5]);
	DDX_Text(pDX, IDC_L7, m_L[6]);
	DDX_Text(pDX, IDC_L8, m_L[7]);

	DDX_Text(pDX, IDC_H1, m_H[0]);
	DDX_Text(pDX, IDC_H2, m_H[1]);
	DDX_Text(pDX, IDC_H3, m_H[2]);
	DDX_Text(pDX, IDC_H4, m_H[3]);
	DDX_Text(pDX, IDC_H5, m_H[4]);
	DDX_Text(pDX, IDC_H6, m_H[5]);
	DDX_Text(pDX, IDC_H7, m_H[6]);
	DDX_Text(pDX, IDC_H8, m_H[7]);

	DDX_Text(pDX, IDC_FROM1, m_F[0]);
	DDV_MinMaxInt(pDX, m_F[0], 1, 10);
	DDX_Text(pDX, IDC_FROM2, m_F[1]);
	DDV_MinMaxInt(pDX, m_F[1], 1, 10);
	DDX_Text(pDX, IDC_FROM3, m_F[2]);
	DDV_MinMaxInt(pDX, m_F[2], 1, 10);
	DDX_Text(pDX, IDC_FROM4, m_F[3]);
	DDV_MinMaxInt(pDX, m_F[3], 1, 10);
	DDX_Text(pDX, IDC_FROM5, m_F[4]);
	DDV_MinMaxInt(pDX, m_F[4], 1, 10);
	DDX_Text(pDX, IDC_FROM6, m_F[5]);
	DDV_MinMaxInt(pDX, m_F[5], 1, 10);
	DDX_Text(pDX, IDC_FROM7, m_F[6]);
	DDV_MinMaxInt(pDX, m_F[6], 1, 10);
	DDX_Text(pDX, IDC_FROM8, m_F[7]);
	DDV_MinMaxInt(pDX, m_F[7], 1, 10);

	DDX_Text(pDX, IDC_PROBE1, m_P[0]);
	DDV_MinMaxInt(pDX, m_P[0], 0, 9);
	DDX_Text(pDX, IDC_PROBE2, m_P[1]);
	DDV_MinMaxInt(pDX, m_P[1], 0, 9);
	DDX_Text(pDX, IDC_PROBE3, m_P[2]);
	DDV_MinMaxInt(pDX, m_P[2], 0, 9);
	DDX_Text(pDX, IDC_PROBE4, m_P[3]);
	DDV_MinMaxInt(pDX, m_P[3], 0, 9);
	DDX_Text(pDX, IDC_PROBE5, m_P[4]);
	DDV_MinMaxInt(pDX, m_P[4], 0, 9);
	DDX_Text(pDX, IDC_PROBE6, m_P[5]);
	DDV_MinMaxInt(pDX, m_P[5], 0, 9);
	DDX_Text(pDX, IDC_PROBE7, m_P[6]);
	DDV_MinMaxInt(pDX, m_P[6], 0, 9);
	DDX_Text(pDX, IDC_PROBE8, m_P[7]);
	DDV_MinMaxInt(pDX, m_P[7], 0, 9);

	DDX_Text(pDX, IDC_TO1, m_T[0]);
	DDV_MinMaxInt(pDX, m_T[0], 1, 10);
	DDX_Text(pDX, IDC_TO2, m_T[1]);
	DDV_MinMaxInt(pDX, m_T[1], 1, 10);
	DDX_Text(pDX, IDC_TO3, m_T[2]);
	DDV_MinMaxInt(pDX, m_T[2], 1, 10);
	DDX_Text(pDX, IDC_TO4, m_T[3]);
	DDV_MinMaxInt(pDX, m_T[3], 1, 10);
	DDX_Text(pDX, IDC_TO5, m_T[4]);
	DDV_MinMaxInt(pDX, m_T[4], 1, 10);
	DDX_Text(pDX, IDC_TO6, m_T[5]);
	DDV_MinMaxInt(pDX, m_T[5], 1, 10);
	DDX_Text(pDX, IDC_TO7, m_T[6]);
	DDV_MinMaxInt(pDX, m_T[6], 1, 10);
	DDX_Text(pDX, IDC_TO8, m_T[7]);
	DDV_MinMaxInt(pDX, m_T[7], 1, 10);

	DDX_Text(pDX, IDC_NAME1, m_N[0]);
	DDX_Text(pDX, IDC_NAME2, m_N[1]);
	DDX_Text(pDX, IDC_NAME3, m_N[2]);
	DDX_Text(pDX, IDC_NAME4, m_N[3]);
	DDX_Text(pDX, IDC_NAME5, m_N[4]);
	DDX_Text(pDX, IDC_NAME6, m_N[5]);
	DDX_Text(pDX, IDC_NAME7, m_N[6]);
	DDX_Text(pDX, IDC_NAME8, m_N[7]);

	DDX_Text(pDX, IDC_RI1, m_R[0]);
	DDX_Text(pDX, IDC_RI2, m_R[1]);
	DDX_Text(pDX, IDC_RI3, m_R[2]);
	DDX_Text(pDX, IDC_RI4, m_R[3]);
	DDX_Text(pDX, IDC_RI5, m_R[4]);
	DDX_Text(pDX, IDC_RI6, m_R[5]);
	DDX_Text(pDX, IDC_RI7, m_R[6]);
	DDX_Text(pDX, IDC_RI8, m_R[7]);

	DDX_Text(pDX, IDC_DECIMAL1, m_D[0]);
	DDV_MinMaxInt(pDX, m_D[0], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL2, m_D[1]);
	DDV_MinMaxInt(pDX, m_D[1], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL3, m_D[2]);
	DDV_MinMaxInt(pDX, m_D[2], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL4, m_D[3]);
	DDV_MinMaxInt(pDX, m_D[3], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL5, m_D[4]);
	DDV_MinMaxInt(pDX, m_D[4], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL6, m_D[5]);
	DDV_MinMaxInt(pDX, m_D[5], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL7, m_D[6]);
	DDV_MinMaxInt(pDX, m_D[6], 0, 4);
	DDX_Text(pDX, IDC_DECIMAL8, m_D[7]);
	DDV_MinMaxInt(pDX, m_D[7], 0, 4);

	DDX_Text(pDX, IDC_DFLT1, m_DF[0]);
	DDX_Text(pDX, IDC_DFLT2, m_DF[1]);
	DDX_Text(pDX, IDC_DFLT3, m_DF[2]);
	DDX_Text(pDX, IDC_DFLT4, m_DF[3]);
	DDX_Text(pDX, IDC_DFLT5, m_DF[4]);
	DDX_Text(pDX, IDC_DFLT6, m_DF[5]);
	DDX_Text(pDX, IDC_DFLT7, m_DF[6]);
	DDX_Text(pDX, IDC_DFLT8, m_DF[7]);

	DDX_Text(pDX, IDC_OFF1, m_Off[0]);
	DDX_Text(pDX, IDC_OFF2, m_Off[1]);
	DDX_Text(pDX, IDC_OFF3, m_Off[2]);
	DDX_Text(pDX, IDC_OFF4, m_Off[3]);
	DDX_Text(pDX, IDC_OFF5, m_Off[4]);
	DDX_Text(pDX, IDC_OFF6, m_Off[5]);
	DDX_Text(pDX, IDC_OFF7, m_Off[6]);
	DDX_Text(pDX, IDC_OFF8, m_Off[7]);

	DDX_Text(pDX, IDC_FORMULA1, m_Calc[0]);
	DDX_Text(pDX, IDC_FORMULA2, m_Calc[1]);
	DDX_Text(pDX, IDC_FORMULA3, m_Calc[2]);
	DDX_Text(pDX, IDC_FORMULA4, m_Calc[3]);
	DDX_Text(pDX, IDC_FORMULA5, m_Calc[4]);
	DDX_Text(pDX, IDC_FORMULA6, m_Calc[5]);
	DDX_Text(pDX, IDC_FORMULA7, m_Calc[6]);
	DDX_Text(pDX, IDC_FORMULA8, m_Calc[7]);
}

BEGIN_MESSAGE_MAP(CEchoProbeProp, CDialog)
	//{{AFX_MSG_MAP(CEchoProbeProp)
	ON_BN_CLICKED(IDC_RD1, OnRadio1)
	ON_BN_CLICKED(IDC_RD2, OnRadio2)
	ON_BN_CLICKED(IDC_RD3, OnRadio3)
	ON_BN_CLICKED(IDC_RD4, OnRadio4)
	ON_BN_CLICKED(IDC_RD5, OnRadio5)
	ON_BN_CLICKED(IDC_RD6, OnRadio6)
	ON_BN_CLICKED(IDC_RD7, OnRadio7)
	ON_BN_CLICKED(IDC_RD8, OnRadio8)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	ON_BN_CLICKED(IDC_SET4, OnSet4)
	ON_BN_CLICKED(IDC_SET5, OnSet5)
	ON_BN_CLICKED(IDC_SET6, OnSet6)
	ON_BN_CLICKED(IDC_SET7, OnSet7)
	ON_BN_CLICKED(IDC_SET8, OnSet8)
	ON_BN_CLICKED(IDC_SET9, OnSet9)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoProbeProp message handlers
BOOL CEchoProbeProp::OnInitDialog() {
	CDialog::OnInitDialog();

	if (!pRcp) {
		ASSERT(0);
		return TRUE;
	}

	LoadDialog(MeSet);

	CButton* pButton;
	switch (MapCol) {
	case 0:
		pButton = (CButton*)GetDlgItem(IDC_RD1);
		pButton->SetCheck(TRUE);
		break;
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_RD2);
		pButton->SetCheck(TRUE);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_RD3);
		pButton->SetCheck(TRUE);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_RD4);
		pButton->SetCheck(TRUE);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_RD5);
		pButton->SetCheck(TRUE);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_RD6);
		pButton->SetCheck(TRUE);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_RD7);
		pButton->SetCheck(TRUE);
		break;
	case 7:
		pButton = (CButton*)GetDlgItem(IDC_RD8);
		pButton->SetCheck(TRUE);
		break;
	}

	switch (MeSet) {
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_SET1);
		pButton->SetCheck(TRUE);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_SET2);
		pButton->SetCheck(TRUE);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_SET3);
		pButton->SetCheck(TRUE);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_SET4);
		pButton->SetCheck(TRUE);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_SET5);
		pButton->SetCheck(TRUE);
		break;
	case 6:
		pButton = (CButton*)GetDlgItem(IDC_SET6);
		pButton->SetCheck(TRUE);
		break;
	case 7:
		pButton = (CButton*)GetDlgItem(IDC_SET7);
		pButton->SetCheck(TRUE);
		break;
	case 8:
		pButton = (CButton*)GetDlgItem(IDC_SET8);
		pButton->SetCheck(TRUE);
		break;
	case 9:
		pButton = (CButton*)GetDlgItem(IDC_SET9);
		pButton->SetCheck(TRUE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEchoProbeProp::OnRadio1() {
	MapCol = 0;
}

void CEchoProbeProp::OnRadio2() {
	MapCol = 1;
}

void CEchoProbeProp::OnRadio3() {
	MapCol = 2;
}

void CEchoProbeProp::OnRadio4() {
	MapCol = 3;
}

void CEchoProbeProp::OnRadio5() {
	MapCol = 4;
}

void CEchoProbeProp::OnRadio6() {
	MapCol = 5;
}

void CEchoProbeProp::OnRadio7() {
	MapCol = 6;
}

void CEchoProbeProp::OnRadio8() {
	MapCol = 7;
}

void CEchoProbeProp::OnOK() {
	SaveDialog(MeSet);
	CDialog::OnOK();
}

void CEchoProbeProp::OnSet1() {
	SwapMeSet(1);
}

void CEchoProbeProp::OnSet2() {
	SwapMeSet(2);
}

void CEchoProbeProp::OnSet3() {
	SwapMeSet(3);
}

void CEchoProbeProp::OnSet4() {
	SwapMeSet(4);
}

void CEchoProbeProp::OnSet5() {
	SwapMeSet(5);
}

void CEchoProbeProp::OnSet6() {
	SwapMeSet(6);
}

void CEchoProbeProp::OnSet7() {
	SwapMeSet(7);
}

void CEchoProbeProp::OnSet8() {
	SwapMeSet(8);
}

void CEchoProbeProp::OnSet9() {
	SwapMeSet(9);
}

void CEchoProbeProp::SwapMeSet(short next) {
	SaveDialog(MeSet);
	MeSet = next;
	LoadDialog(MeSet);
}

void CEchoProbeProp::LoadDialog(short MeS) {
	int i;
	MeS--;
	for (i = 0; i < MaXPA; i++) {
		m_P[i] = pRcp->MeParam[MeS].MPa[i].Prb;
		m_F[i] = pRcp->MeParam[MeS].MPa[i].Fr;
		m_T[i] = pRcp->MeParam[MeS].MPa[i].To;
		m_R[i] = pRcp->MeParam[MeS].MPa[i].RI;
		m_L[i] = pRcp->MeParam[MeS].MPa[i].Lo;
		m_H[i] = pRcp->MeParam[MeS].MPa[i].Hi;
		m_D[i] = pRcp->MeParam[MeS].MPa[i].D;
		m_DF[i] = pRcp->MeParam[MeS].MPa[i].DF;
		m_Off[i] = pRcp->MeParam[MeS].MPa[i].Off;
		m_N[i] = pRcp->MeParam[MeS].MPa[i].Name;
		m_Calc[i] = pRcp->MeParam[MeS].MPa[i].Formula;
	}
	MapCol = pRcp->Desc.MapCol;

	for (i = 0; i < 2; i++) {
		m_ThresholdFactor[i] = pRcp->MeParam[MeS].PSG[i].Thres;
		m_Level[i] = pRcp->MeParam[MeS].PSG[i].Level;
		m_PeakWidth[i] = pRcp->MeParam[MeS].PSG[i].Width;
	}

	m_NAve = pRcp->MeParam[MeS].NAve;

	UpdateData(FALSE);
}

void CEchoProbeProp::SaveDialog(short MeS) {
	UpdateData(TRUE);
	int i;
	MeS--;
	for (i = 0; i < MaXPA; i++) {
		pRcp->MeParam[MeS].MPa[i].Prb = (MParam::PRBNUM)m_P[i];
		pRcp->MeParam[MeS].MPa[i].Fr = m_F[i];
		pRcp->MeParam[MeS].MPa[i].To = m_T[i];
		pRcp->MeParam[MeS].MPa[i].RI = m_R[i];
		pRcp->MeParam[MeS].MPa[i].Lo = m_L[i];
		pRcp->MeParam[MeS].MPa[i].Hi = m_H[i];
		pRcp->MeParam[MeS].MPa[i].D = m_D[i];
		pRcp->MeParam[MeS].MPa[i].DF = m_DF[i];
		pRcp->MeParam[MeS].MPa[i].Off = m_Off[i];
		pRcp->MeParam[MeS].MPa[i].Name = m_N[i];
		pRcp->MeParam[MeS].MPa[i].Formula = m_Calc[i];
	}
	pRcp->Desc.Clear();
	pRcp->Desc.MapCol = MapCol;

	for (i = 0; i < 2; i++) {
		pRcp->MeParam[MeS].PSG[i].Thres = m_ThresholdFactor[i];
		pRcp->MeParam[MeS].PSG[i].Level = m_Level[i];
		pRcp->MeParam[MeS].PSG[i].Width = m_PeakWidth[i];
	}

	pRcp->MeParam[MeS].NAve = m_NAve;
}

void CEchoProbeProp::OnClear() {}