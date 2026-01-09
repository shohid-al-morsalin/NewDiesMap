// IssueAlarmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "IssueAlarmDlg.h"

#include "AlrmItem.h"
#include "..\ONLINE\GEM\CediListCtroller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIssueAlarmDlg dialog

CIssueAlarmDlg::CIssueAlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIssueAlarmDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CIssueAlarmDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CIssueAlarmDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIssueAlarmDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIssueAlarmDlg, CDialog)
	//{{AFX_MSG_MAP(CIssueAlarmDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIssueAlarmDlg message handlers

BOOL CIssueAlarmDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	char* dwCjName[] =
	{
		("Alarm ID"), (" Alarm Name"), ("Status")
	};
	int nSize[] = { 56, 250, 128 };
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_List.InsertColumn(i, &nListColumn);
	}

	RefreshList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CIssueAlarmDlg::DestroyWindow() {
	m_List.DeleteAllItems();

	return CDialog::DestroyWindow();
}

void CIssueAlarmDlg::RefreshList() {
	CEIDCtlr.m_alarm.LoadList(m_List);
}

void CIssueAlarmDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CString Var = this->m_List.GetItemText(nSel, 0);

	SetAlarm(atoi(Var));

	*pResult = 0;
}

void CIssueAlarmDlg::SetAlarm(int alID) {
	CAlrmItem* pA = CEIDCtlr.m_alarm.GetAlarm(alID);
	if (pA) {
		// 		if (pA->alCD & CAlrmItem::SET)
		// 		{
		// 			CEIDCtlr.m_alarm.ClearAlarm(pA->alID);
		// 		}
		// 		else
		// 		{
		CEIDCtlr.m_alarm.SetAlarm(pA->alID);
		// 		}
	}
}