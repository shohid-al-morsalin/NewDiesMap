// OnlineEventsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlineEventsDlg.h"
#include ".\onlineeventsdlg.h"
#include "..\ONLINE\gem/CediListCtroller.h"
#include "..\ONLINE\mfc/VarDisp.h"

#include "GlobalDeviceContainer.h"
#include "..\ONLINE\GEM\CediListCtroller.h"

COnlineEventsDlg::COnlineEventsDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineEventsDlg::IDD, pParent) {}

COnlineEventsDlg::~COnlineEventsDlg() {}

void COnlineEventsDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CEID, m_List);
}

BEGIN_MESSAGE_MAP(COnlineEventsDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineEventsDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CEID, OnLvnItemchangedListCeid)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CEID, OnNMDblclkListCeid)
	ON_BN_CLICKED(IDC_ENABLEALLCEID, OnEnableallceid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnlineEventsDlg message handlers
BOOL COnlineEventsDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST_CEID, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_TEXT, TOP_RIGHT);
	AddAnchor(IDC_REFRESH, TOP_RIGHT);
	AddAnchor(IDC_ENABLEALLCEID, TOP_RIGHT);

	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List.SetHeadings(_T("Item,50;ID,50;Collection event description,360;Enable flag,120"));
	m_List.LoadColumnInfo();

	CEIDCtlr.LoadEnaDisCEIDAll();

	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void COnlineEventsDlg::Refresh() {
	m_List.DeleteAllItems();

	CEID_LIST* p = &CEIDCtlr.m_CEList;
	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		CEID_ITEM* q = &p->m_Ceid[i];
		CString str1, str2;
		str1.Format("%d", i + 1);
		str2.Format("%d", q->m_ceid);
		//if (i==CEID_LIST::AMSM1AUTOTOMANUAL)
		//{
		m_List.AddItem(str1, str2, q->m_cename, q->m_reportflag ? "Enabled" : "Disabled");
		//}
		//else
		//{
		//	m_List.AddItem(str1,str2,q->m_cename,q->m_reportflag?"Enabled":"Disabled");
		//}
	}
}

void COnlineEventsDlg::OnNMDblclkListCeid(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CString Var1 = m_List.GetItemText(nSel, 0);		// Item: Start from 1 [4/28/2013 Administrator]
	CString Var2 = m_List.GetItemText(nSel, 1);
	CVarDisp VarDat;
	VarDat.m_Type = 2;
	VarDat.m_VarNo = atoi(Var1);
	VarDat.DoModal();
	*pResult = 0;
}

void COnlineEventsDlg::OnLvnItemchangedListCeid(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;
}

void COnlineEventsDlg::OnBnClickedRefresh() {
	Refresh();
}

void COnlineEventsDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "COnlineEventsDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineEventsDlg::OnDestroy() {
	m_List.DeleteAllItems();
	CDialog::OnDestroy();
}

void COnlineEventsDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void COnlineEventsDlg::OnEnableallceid() {
	CEIDCtlr.evtrpt_all_setting(TRUE);
	CEIDCtlr.SaveEnaDisCEIDAll();
	Refresh();
}