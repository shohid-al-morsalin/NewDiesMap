// OptimizeEchoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "OptimizeEchoDlg.h"
#include "413App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptimizeEchoDlg dialog

COptimizeEchoDlg::COptimizeEchoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptimizeEchoDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COptimizeEchoDlg)
	//}}AFX_DATA_INIT
}

void COptimizeEchoDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptimizeEchoDlg)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptimizeEchoDlg, CDialog)
	//{{AFX_MSG_MAP(COptimizeEchoDlg)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptimizeEchoDlg message handlers

BOOL COptimizeEchoDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// Insert a column. This override is the most convenient.
	m_cList.InsertColumn(0, _T("Message           "), LVCFMT_LEFT);
	m_cList.SetColumnWidth(0, 196);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void COptimizeEchoDlg::OnStop() {
	p413App->Global.bStop = TRUE;
}