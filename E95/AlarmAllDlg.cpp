#include "stdafx.h"
#include "e95.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "ONLINE/GEM/CediListCtroller.h"

#include "AlarmAllDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmAllDlg dialog

CAlarmAllDlg::CAlarmAllDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CAlarmAllDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CAlarmAllDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAlarmAllDlg::~CAlarmAllDlg() {}

void CAlarmAllDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmAllDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlarmAllDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CAlarmAllDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmAllDlg message handlers

LRESULT CAlarmAllDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Alarms\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	return 0;
}

void CAlarmAllDlg::SetupColumn() {
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List.SetHeadings("ALID,48;ALTX,450");
	m_List.LoadColumnInfo();

	CString val;
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		val.Format("%d", CEIDCtlr.m_alarm.Alarms[i].alID);
		m_List.AddItem(val, CEIDCtlr.m_alarm.Alarms[i].alTX);
	}
}

BOOL CAlarmAllDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);

	SetupColumn();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

// This function is not called [12/14/2007 YUEN]
void CAlarmAllDlg::OnDestroy() {
	//	m_List.SaveColumnInfo();
	m_List.DeleteAllItems();
	CResizableDialog::OnDestroy();
}

void CAlarmAllDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}