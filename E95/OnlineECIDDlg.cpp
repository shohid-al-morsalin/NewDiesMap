// OnlineECIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "OnlineECIDDlg.h"
#include "..\ONLINE\mfc/VarDisp.h"
#include "..\ONLINE\GEM\GemFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnlineECIDDlg dialog

COnlineECIDDlg::COnlineECIDDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineECIDDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineECIDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void COnlineECIDDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineECIDDlg)
	DDX_Control(pDX, IDC_LIST_ECID, m_ECIDList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineECIDDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineECIDDlg)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ECID, OnDblclkListEcid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineECIDDlg message handlers

BOOL COnlineECIDDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST_ECID, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_TEXT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_REFRESH, TOP_RIGHT);

	char* dwCjName[] =
	{
		("  NO"), ("  ECID Name")
	};
	int nSize[] = { 50, 250 };
	m_ECIDList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;
	for (int i = 0; i < 2; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_ECIDList.InsertColumn(i, &nListColumn);
	}

	OnButtonRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineECIDDlg::OnButtonRefresh() {
	char buf[126] = { 0 };
	m_ECIDList.DeleteAllItems();
	SVID_LIST* p = &CEIDCtlr.m_SVList;
	for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
		if (p->m_Svid[i].m_bVarType == SVID_ITEM::EC_TYPE) {
			int pos = m_ECIDList.InsertItem(i, _itoa(p->m_Svid[i].m_svidno, buf, 10));
			m_ECIDList.SetItemText(pos, 1, p->m_Svid[i].m_svname);
		}
	}
}

void COnlineECIDDlg::OnDblclkListEcid(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_ECIDList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CString Var = this->m_ECIDList.GetItemText(nSel, 0);
	CVarDisp VarDat;
	VarDat.m_Type = 3;
	VarDat.m_VarNo = atoi(Var);
	VarDat.DoModal();

	*pResult = 0;
}

void COnlineECIDDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}