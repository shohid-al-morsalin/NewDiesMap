// ProcJobManageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "XTabCtrl.h"
#include "ProcJobManageDlg.h"
#include "OnlineCJDlg.h"
#include "OnlinePJDlg.h"
#include "ProcJobDlg.h"
#include "InfoPanelMain.h"

#include "GlobalUIContainer.h"
// #include "GlobalDeviceContainer.h"

// CProcJobManageDlg dialog

IMPLEMENT_DYNAMIC(CProcJobManageDlg, CDialog)
CProcJobManageDlg::CProcJobManageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcJobManageDlg::IDD, pParent)
	, m_CJDlg(NULL)
	, m_PJDlg(NULL) {
	//{{AFX_DATA_INIT(CProcJobManageDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProcJobManageDlg::~CProcJobManageDlg() {}

void CProcJobManageDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobManageDlg)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobManageDlg, CDialog)
	//{{AFX_MSG_MAP(CProcJobManageDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CProcJobManageDlg message handlers
BOOL CProcJobManageDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	int nTab = 0;

	m_CJDlg = new COnlineCJDlg;
	if (m_CJDlg) {
		m_CJDlg->Create(COnlineCJDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_CJDlg, "Control Jobs", nTab++);
	}

	m_PJDlg = new COnlinePJDlg;
	if (m_PJDlg) {
		m_PJDlg->Create(COnlinePJDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_PJDlg, "Process Jobs", nTab++);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CProcJobManageDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CProcJobManageDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}

void CProcJobManageDlg::OnDestroy() {
	if (m_CJDlg) {
		m_CJDlg->DestroyWindow();
		m_CJDlg = NULL;
	}
	if (m_PJDlg) {
		m_PJDlg->DestroyWindow();
		m_PJDlg = NULL;
	}
	CDialog::OnDestroy();
}