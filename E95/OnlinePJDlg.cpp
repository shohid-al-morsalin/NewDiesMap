// OnlinePJDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlinePJDlg.h"
#include ".\onlinepjdlg.h"
#include "../online/mfc/VarDisp.h"
#include "..\ONLINE\inc\event.h"
#include "..\ONLINE\sharememory\DataBank.h"
#include "..\ONLINE\gem\CtrlJobMgr.h"

#include "GlobalDeviceContainer.h"

// COnlinePJDlg dialog

//IMPLEMENT_DYNAMIC(COnlinePJDlg, CResizableDialog)
COnlinePJDlg::COnlinePJDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlinePJDlg::IDD, pParent) {}

COnlinePJDlg::~COnlinePJDlg() {}

void COnlinePJDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PRJOB, m_PjList);
	DDX_Control(pDX, IDC_PRJOBNAME, m_CurPJName);
}

BEGIN_MESSAGE_MAP(COnlinePJDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CLEARPJ, OnButtonClearPJ)
END_MESSAGE_MAP()
//ON_BN_CLICKED(IDC_PRJOBNAME, OnPrjobname)
//ON_NOTIFY(NM_DBLCLK, IDC_LIST_PRJOB, OnNMDblclkListPrjob)

// COnlinePJDlg message handlers

//void COnlinePJDlg::OnBnClickedButtonRefresh()
//{
//	// TODO: Add your control notification handler code here
//}

//void COnlinePJDlg::OnNMDblclkListPrjob(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	int nSel = m_PjList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
//	CString Var  = m_PjList.GetItemText(nSel, 1);
//	ReqPJData(Var);
//	*pResult = 0;
//}

//void COnlinePJDlg::ReqPJData(CString &PjName)
//{
//	if(PjName.GetLength())
//	{
//		CVarDisp VarDat;
//		VarDat.m_Type = REQ_PJDATA;
//		//	VarDat.m_VarNo = atoi(Var);
//		VarDat.m_VarName = PjName.GetBuffer(0);
//		VarDat.DoModal();
//	}
//}

BOOL COnlinePJDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_PRJOBNAME, TOP_LEFT);
	AddAnchor(IDC_TEXT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_LIST_PRJOB, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_CLEARPJ, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_REFRESH, TOP_RIGHT);

	int i = 0;
	char* dwPjName[] =
	{
		("  NO"), ("  Process Job Name")
	};
	int nSize[] = { 50, 250 };

	m_PjList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	for (i = 0; i < 2; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwPjName[i];
		m_PjList.InsertColumn(i, &nListColumn);
	}

	CString str = "";
	CString tmp;
	CPRJob pJ;
	for (i = CPRJob::QUEUED_POOLED; i <= CPRJob::ABORTING; i++) {
		tmp.Format("%s = %d\n", pJ.GetJobStateStr((CPRJob::PRJOBSTATE)i), i);
		str += tmp;
	}
	((CStatic*)GetDlgItem(IDC_PRJOBSTATE))->SetWindowText(str);

	OnButtonRefresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlinePJDlg::OnButtonRefresh() {
	// TODO: Add your control notification handler code here
	string ID;
	vector<string> IDList;
	char Buf[126] = { 0 };
	int Position;
	m_PjList.DeleteAllItems();

	ID.erase();
	CPRJob* p = CJobMgr.GetActivePRJob();
	if (p) {
		ID = p->m_ObjID;
	}
	else {
		ID = "";
	}
	m_CurPJName.SetWindowText(ID.c_str());
	IDList.clear();
	pGDev->GetAllPRJobIDs(IDList);
	for (int nIndex = 0; nIndex<int(IDList.size()); nIndex++) {
		Position = m_PjList.InsertItem(0, _itoa(nIndex + 1, Buf, 10));
		m_PjList.SetItemText(Position, 1, IDList[nIndex].c_str());
	}
}

//void COnlinePJDlg::OnPrjobname()
//{
//	// TODO: Add your control notification handler code here
//	CString PjName ;
//	m_CurPJName.GetWindowText(PjName);
//	ReqPJData(PjName);
//}

void COnlinePJDlg::OnButtonClearPJ() {
	pGDev->ClearJobs();
	m_PjList.DeleteAllItems();
}

void COnlinePJDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}