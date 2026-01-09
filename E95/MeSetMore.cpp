// MeSetMore.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "MeSetMore.h"

#include "413App.h"
#include "Recipe413.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeSetMore dialog

CMeSetMore::CMeSetMore(CWnd* pParent /*=NULL*/)
	: CDialog(CMeSetMore::IDD, pParent) {
	//{{AFX_DATA_INIT(CMeSetMore)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	MeS = 1;

	for (int i = 0; i < MaXPA; i++) {
		m_D[i] = 0;
	}
}

void CMeSetMore::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeSetMore)
	DDX_Control(pDX, IDC_MEASUREMENTSET, m_cMeSet);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_DECIMAL1, m_D[0]);
	DDV_MinMaxInt(pDX, m_D[0], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL2, m_D[1]);
	DDV_MinMaxInt(pDX, m_D[1], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL3, m_D[2]);
	DDV_MinMaxInt(pDX, m_D[2], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL4, m_D[3]);
	DDV_MinMaxInt(pDX, m_D[3], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL5, m_D[4]);
	DDV_MinMaxInt(pDX, m_D[4], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL6, m_D[5]);
	DDV_MinMaxInt(pDX, m_D[5], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL7, m_D[6]);
	DDV_MinMaxInt(pDX, m_D[6], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL8, m_D[7]);
	DDV_MinMaxInt(pDX, m_D[7], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL9, m_D[8]);
	DDV_MinMaxInt(pDX, m_D[8], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL10, m_D[9]);
	DDV_MinMaxInt(pDX, m_D[9], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL11, m_D[10]);
	DDV_MinMaxInt(pDX, m_D[10], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL12, m_D[11]);
	DDV_MinMaxInt(pDX, m_D[11], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL13, m_D[12]);
	DDV_MinMaxInt(pDX, m_D[12], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL14, m_D[13]);
	DDV_MinMaxInt(pDX, m_D[13], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL15, m_D[14]);
	DDV_MinMaxInt(pDX, m_D[14], 0, 7);
	DDX_Text(pDX, IDC_DECIMAL16, m_D[15]);
	DDV_MinMaxInt(pDX, m_D[15], 0, 7);

	DDX_Text(pDX, IDC_DFLT1, m_DF[0]);
	DDX_Text(pDX, IDC_DFLT2, m_DF[1]);
	DDX_Text(pDX, IDC_DFLT3, m_DF[2]);
	DDX_Text(pDX, IDC_DFLT4, m_DF[3]);
	DDX_Text(pDX, IDC_DFLT5, m_DF[4]);
	DDX_Text(pDX, IDC_DFLT6, m_DF[5]);
	DDX_Text(pDX, IDC_DFLT7, m_DF[6]);
	DDX_Text(pDX, IDC_DFLT8, m_DF[7]);
	DDX_Text(pDX, IDC_DFLT9, m_DF[8]);
	DDX_Text(pDX, IDC_DFLT10, m_DF[9]);
	DDX_Text(pDX, IDC_DFLT11, m_DF[10]);
	DDX_Text(pDX, IDC_DFLT12, m_DF[11]);
	DDX_Text(pDX, IDC_DFLT13, m_DF[12]);
	DDX_Text(pDX, IDC_DFLT14, m_DF[13]);
	DDX_Text(pDX, IDC_DFLT15, m_DF[14]);
	DDX_Text(pDX, IDC_DFLT16, m_DF[15]);

	DDX_Text(pDX, IDC_OFF1, m_Off[0]);
	DDX_Text(pDX, IDC_OFF2, m_Off[1]);
	DDX_Text(pDX, IDC_OFF3, m_Off[2]);
	DDX_Text(pDX, IDC_OFF4, m_Off[3]);
	DDX_Text(pDX, IDC_OFF5, m_Off[4]);
	DDX_Text(pDX, IDC_OFF6, m_Off[5]);
	DDX_Text(pDX, IDC_OFF7, m_Off[6]);
	DDX_Text(pDX, IDC_OFF8, m_Off[7]);
	DDX_Text(pDX, IDC_OFF9, m_Off[8]);
	DDX_Text(pDX, IDC_OFF10, m_Off[9]);
	DDX_Text(pDX, IDC_OFF11, m_Off[10]);
	DDX_Text(pDX, IDC_OFF12, m_Off[11]);
	DDX_Text(pDX, IDC_OFF13, m_Off[12]);
	DDX_Text(pDX, IDC_OFF14, m_Off[13]);
	DDX_Text(pDX, IDC_OFF15, m_Off[14]);
	DDX_Text(pDX, IDC_OFF16, m_Off[15]);

	DDX_Text(pDX, IDC_NPEAK1, m_NP[0]);
	DDX_Text(pDX, IDC_NPEAK2, m_NP[1]);
	DDX_Text(pDX, IDC_NPEAK3, m_NP[2]);
	DDX_Text(pDX, IDC_NPEAK4, m_NP[3]);
	DDX_Text(pDX, IDC_NPEAK5, m_NP[4]);
	DDX_Text(pDX, IDC_NPEAK6, m_NP[5]);
	DDX_Text(pDX, IDC_NPEAK7, m_NP[6]);
	DDX_Text(pDX, IDC_NPEAK8, m_NP[7]);
	DDX_Text(pDX, IDC_NPEAK9, m_NP[8]);
	DDX_Text(pDX, IDC_NPEAK10, m_NP[9]);
	DDX_Text(pDX, IDC_NPEAK11, m_NP[10]);
	DDX_Text(pDX, IDC_NPEAK12, m_NP[11]);
	DDX_Text(pDX, IDC_NPEAK13, m_NP[12]);
	DDX_Text(pDX, IDC_NPEAK14, m_NP[13]);
	DDX_Text(pDX, IDC_NPEAK15, m_NP[14]);
	DDX_Text(pDX, IDC_NPEAK16, m_NP[15]);

	DDX_Text(pDX, IDC_L1, m_L[0]);
	DDX_Text(pDX, IDC_L2, m_L[1]);
	DDX_Text(pDX, IDC_L3, m_L[2]);
	DDX_Text(pDX, IDC_L4, m_L[3]);
	DDX_Text(pDX, IDC_L5, m_L[4]);
	DDX_Text(pDX, IDC_L6, m_L[5]);
	DDX_Text(pDX, IDC_L7, m_L[6]);
	DDX_Text(pDX, IDC_L8, m_L[7]);
	DDX_Text(pDX, IDC_L9, m_L[8]);
	DDX_Text(pDX, IDC_L10, m_L[9]);
	DDX_Text(pDX, IDC_L11, m_L[10]);
	DDX_Text(pDX, IDC_L12, m_L[11]);
	DDX_Text(pDX, IDC_L13, m_L[12]);
	DDX_Text(pDX, IDC_L14, m_L[13]);
	DDX_Text(pDX, IDC_L15, m_L[14]);
	DDX_Text(pDX, IDC_L16, m_L[15]);

	DDX_Text(pDX, IDC_H1, m_H[0]);
	DDX_Text(pDX, IDC_H2, m_H[1]);
	DDX_Text(pDX, IDC_H3, m_H[2]);
	DDX_Text(pDX, IDC_H4, m_H[3]);
	DDX_Text(pDX, IDC_H5, m_H[4]);
	DDX_Text(pDX, IDC_H6, m_H[5]);
	DDX_Text(pDX, IDC_H7, m_H[6]);
	DDX_Text(pDX, IDC_H8, m_H[7]);
	DDX_Text(pDX, IDC_H9, m_H[8]);
	DDX_Text(pDX, IDC_H10, m_H[9]);
	DDX_Text(pDX, IDC_H11, m_H[10]);
	DDX_Text(pDX, IDC_H12, m_H[11]);
	DDX_Text(pDX, IDC_H13, m_H[12]);
	DDX_Text(pDX, IDC_H14, m_H[13]);
	DDX_Text(pDX, IDC_H15, m_H[14]);
	DDX_Text(pDX, IDC_H16, m_H[15]);

	DDX_Text(pDX, IDC_OX1, m_OffX[0]);
	DDX_Text(pDX, IDC_OX2, m_OffX[1]);
	DDX_Text(pDX, IDC_OX3, m_OffX[2]);
	DDX_Text(pDX, IDC_OX4, m_OffX[3]);
	DDX_Text(pDX, IDC_OX5, m_OffX[4]);
	DDX_Text(pDX, IDC_OX6, m_OffX[5]);
	DDX_Text(pDX, IDC_OX7, m_OffX[6]);
	DDX_Text(pDX, IDC_OX8, m_OffX[7]);
	DDX_Text(pDX, IDC_OX9, m_OffX[8]);
	DDX_Text(pDX, IDC_OX10, m_OffX[9]);
	DDX_Text(pDX, IDC_OX11, m_OffX[10]);
	DDX_Text(pDX, IDC_OX12, m_OffX[11]);
	DDX_Text(pDX, IDC_OX13, m_OffX[12]);
	DDX_Text(pDX, IDC_OX14, m_OffX[13]);
	DDX_Text(pDX, IDC_OX15, m_OffX[14]);
	DDX_Text(pDX, IDC_OX16, m_OffX[15]);

	DDX_Text(pDX, IDC_OY1, m_OffY[0]);
	DDX_Text(pDX, IDC_OY2, m_OffY[1]);
	DDX_Text(pDX, IDC_OY3, m_OffY[2]);
	DDX_Text(pDX, IDC_OY4, m_OffY[3]);
	DDX_Text(pDX, IDC_OY5, m_OffY[4]);
	DDX_Text(pDX, IDC_OY6, m_OffY[5]);
	DDX_Text(pDX, IDC_OY7, m_OffY[6]);
	DDX_Text(pDX, IDC_OY8, m_OffY[7]);
	DDX_Text(pDX, IDC_OY9, m_OffY[8]);
	DDX_Text(pDX, IDC_OY10, m_OffY[9]);
	DDX_Text(pDX, IDC_OY11, m_OffY[10]);
	DDX_Text(pDX, IDC_OY12, m_OffY[11]);
	DDX_Text(pDX, IDC_OY13, m_OffY[12]);
	DDX_Text(pDX, IDC_OY14, m_OffY[13]);
	DDX_Text(pDX, IDC_OY15, m_OffY[14]);
	DDX_Text(pDX, IDC_OY16, m_OffY[15]);

	DDX_Text(pDX, IDC_FORMULA1, m_Calc[0]);
	DDX_Text(pDX, IDC_FORMULA2, m_Calc[1]);
	DDX_Text(pDX, IDC_FORMULA3, m_Calc[2]);
	DDX_Text(pDX, IDC_FORMULA4, m_Calc[3]);
	DDX_Text(pDX, IDC_FORMULA5, m_Calc[4]);
	DDX_Text(pDX, IDC_FORMULA6, m_Calc[5]);
	DDX_Text(pDX, IDC_FORMULA7, m_Calc[6]);
	DDX_Text(pDX, IDC_FORMULA8, m_Calc[7]);
	DDX_Text(pDX, IDC_FORMULA9, m_Calc[8]);
	DDX_Text(pDX, IDC_FORMULA10, m_Calc[9]);
	DDX_Text(pDX, IDC_FORMULA11, m_Calc[10]);
	DDX_Text(pDX, IDC_FORMULA12, m_Calc[11]);
	DDX_Text(pDX, IDC_FORMULA13, m_Calc[12]);
	DDX_Text(pDX, IDC_FORMULA14, m_Calc[13]);
	DDX_Text(pDX, IDC_FORMULA15, m_Calc[14]);
	DDX_Text(pDX, IDC_FORMULA16, m_Calc[15]);

	DDX_Check(pDX, IDC_CHECK1, m_Rpt[0]);
	DDX_Check(pDX, IDC_CHECK2, m_Rpt[1]);
	DDX_Check(pDX, IDC_CHECK3, m_Rpt[2]);
	DDX_Check(pDX, IDC_CHECK4, m_Rpt[3]);
	DDX_Check(pDX, IDC_CHECK5, m_Rpt[4]);
	DDX_Check(pDX, IDC_CHECK6, m_Rpt[5]);
	DDX_Check(pDX, IDC_CHECK7, m_Rpt[6]);
	DDX_Check(pDX, IDC_CHECK8, m_Rpt[7]);
	DDX_Check(pDX, IDC_CHECK9, m_Rpt[8]);
	DDX_Check(pDX, IDC_CHECK10, m_Rpt[9]);
	DDX_Check(pDX, IDC_CHECK11, m_Rpt[10]);
	DDX_Check(pDX, IDC_CHECK12, m_Rpt[11]);
	DDX_Check(pDX, IDC_CHECK13, m_Rpt[12]);
	DDX_Check(pDX, IDC_CHECK14, m_Rpt[13]);
	DDX_Check(pDX, IDC_CHECK15, m_Rpt[14]);
	DDX_Check(pDX, IDC_CHECK16, m_Rpt[15]);
}

BEGIN_MESSAGE_MAP(CMeSetMore, CDialog)
	//{{AFX_MSG_MAP(CMeSetMore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeSetMore message handlers

BOOL CMeSetMore::OnInitDialog() {
	CDialog::OnInitDialog();

	CRecipe413* pRcp = &p413App->Global.RcpSetup;
	for (int i = 0; i < MaXPA; i++) {
		MParam* mp = &pRcp->MeParam[MeS - 1].MPa[i];
		m_D[i] = mp->D;
		m_DF[i] = mp->DF;
		m_Off[i] = mp->Off;
		m_OffX[i] = mp->OffX;
		m_OffY[i] = mp->OffY;
		m_NP[i] = mp->NPeak;
		m_L[i] = mp->Lo;
		m_H[i] = mp->Hi;
		m_Calc[i] = mp->Formula;
		m_Rpt[i] = mp->bHost;
	}
	UpdateData(FALSE);

	CString str;

	str.Format("Measurement Set %d", p413App->Global.CurMeSet);
	m_cMeSet.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMeSetMore::OnOK() {
	UpdateData(TRUE);

	CRecipe413* pRcp = &p413App->Global.RcpSetup;
	for (int i = 0; i < MaXPA; i++) {
		MParam* mp = &pRcp->MeParam[MeS - 1].MPa[i];
		mp->D = m_D[i];
		mp->DF = m_DF[i];
		mp->Off = m_Off[i];
		mp->OffX = m_OffX[i];
		mp->OffY = m_OffY[i];
		mp->NPeak = m_NP[i];
		mp->Lo = m_L[i];
		mp->Hi = m_H[i];
		mp->Formula = m_Calc[i];
		mp->bHost = m_Rpt[i];
	}
	CDialog::OnOK();
}