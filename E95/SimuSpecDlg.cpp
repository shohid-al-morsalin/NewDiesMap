// SimuSpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SimuSpecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimuSpecDlg dialog

CSimuSpecDlg::CSimuSpecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimuSpecDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSimuSpecDlg)
	//}}AFX_DATA_INIT

	Spec = 0;
	for (int i = 0; i < 4; i++) {
		m_Amp[i] = 1;
		m_Cycle[i] = 0;
		m_Phase[i] = 0;
	}
}

void CSimuSpecDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimuSpecDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_CYCLE1, m_Cycle[0]);
	DDX_Text(pDX, IDC_CYCLE2, m_Cycle[1]);
	DDX_Text(pDX, IDC_CYCLE3, m_Cycle[2]);
	DDX_Text(pDX, IDC_CYCLE4, m_Cycle[3]);
	DDX_Text(pDX, IDC_PHASE1, m_Phase[0]);
	DDX_Text(pDX, IDC_PHASE2, m_Phase[1]);
	DDX_Text(pDX, IDC_PHASE3, m_Phase[2]);
	DDX_Text(pDX, IDC_PHASE4, m_Phase[3]);
	DDX_Text(pDX, IDC_AMPL1, m_Amp[0]);
	DDX_Text(pDX, IDC_AMPL2, m_Amp[1]);
	DDX_Text(pDX, IDC_AMPL3, m_Amp[2]);
	DDX_Text(pDX, IDC_AMPL4, m_Amp[3]);
}

BEGIN_MESSAGE_MAP(CSimuSpecDlg, CDialog)
	//{{AFX_MSG_MAP(CSimuSpecDlg)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_OK2, OnOk2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimuSpecDlg message handlers

void CSimuSpecDlg::OnOk() {
	UpdateData(TRUE);
	Spec = 0;
	CDialog::OnOK();
}

void CSimuSpecDlg::OnOk2() {
	UpdateData(TRUE);
	Spec = 1;
	CDialog::OnOK();
}