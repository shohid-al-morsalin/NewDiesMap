// SimulatePeaks.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SimulatePeaks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimulatePeaks dialog

CSimulatePeaks::CSimulatePeaks(CWnd* pParent /*=NULL*/)
	: CDialog(CSimulatePeaks::IDD, pParent) {
	//{{AFX_DATA_INIT(CSimulatePeaks)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSimulatePeaks::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimulatePeaks)
	DDX_Control(pDX, IDC_LIST2, m_cList2);
	DDX_Control(pDX, IDC_LIST1, m_cList1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSimulatePeaks, CDialog)
	//{{AFX_MSG_MAP(CSimulatePeaks)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_BN_CLICKED(IDC_DEFAULT2, OnDefault2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimulatePeaks message handlers

BOOL CSimulatePeaks::OnInitDialog() {
	CDialog::OnInitDialog();

	char* dwCjName[] =
	{
		"#", "Position", "dP", "Height", "dH"
	};
	int nSize[] = { 32, 64, 42, 64, 46 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cList1.InsertColumn(i, &nListColumn);
		m_cList2.InsertColumn(i, &nListColumn);
	}
	m_cList1.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_cList2.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSimulatePeaks::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
}

void CSimulatePeaks::OnDefault() {
	// TODO: Add your control notification handler code here
}

void CSimulatePeaks::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CSimulatePeaks::OnDefault2() {}