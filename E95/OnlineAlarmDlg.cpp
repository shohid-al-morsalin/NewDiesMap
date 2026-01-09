// OnlineAlarmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "OnlineAlarmDlg.h"
#include "..\ONLINE\GEM\CediListCtroller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnlineAlarmDlg dialog

COnlineAlarmDlg::COnlineAlarmDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineAlarmDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineAlarmDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void COnlineAlarmDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineAlarmDlg)
	DDX_Control(pDX, IDC_LIST_ALARM, m_AlarmList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineAlarmDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineAlarmDlg)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_ENABLEALLALARMS, OnEnableallalarms)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineAlarmDlg message handlers

BOOL COnlineAlarmDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST_ALARM, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_TEXT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_REFRESH, TOP_RIGHT);
	AddAnchor(IDC_ENABLEALLALARMS, TOP_RIGHT);

	char* dwCjName[] =
	{
		("Alarm ID"), (" Alarm Name"), ("Status")
	};
	int nSize[] = { 56, 250, 128 };
	m_AlarmList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_AlarmList.InsertColumn(i, &nListColumn);
	}

	OnButtonRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineAlarmDlg::OnButtonRefresh() {
	char buf[126] = { 0 };
	m_AlarmList.DeleteAllItems();
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		int pos = m_AlarmList.InsertItem(i, _itoa(CEIDCtlr.m_alarm.Alarms[i].alID, buf, 10));
		m_AlarmList.SetItemText(pos, 1, CEIDCtlr.m_alarm.Alarms[i].alTX);
		m_AlarmList.SetItemText(pos, 2, (CEIDCtlr.m_alarm.Alarms[i].alED) ? "Enabled" : "Disabled");
	}
}

void COnlineAlarmDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void COnlineAlarmDlg::OnEnableallalarms() {
	CEIDCtlr.m_alarm.EnableAllAlarms(); //  [5/29/2011 XPMUser]
	OnButtonRefresh();
}