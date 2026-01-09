// OnlineCJDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlineCJDlg.h"
#include ".\onlinecjdlg.h"
#include "..\online\inc\event.h"
#include "..\online\sharememory\DataBank.h"
#include "..\online\gem\CtrlJobMgr.h"

#include "GlobalDeviceContainer.h"

// COnlineCJDlg dialog

//IMPLEMENT_DYNAMIC(COnlineCJDlg, CResizableDialog)
COnlineCJDlg::COnlineCJDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineCJDlg::IDD, pParent) {}

COnlineCJDlg::~COnlineCJDlg() {}

void COnlineCJDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CJOB, m_CjobList);
	DDX_Control(pDX, IDC_CJOBNAME, m_CurCJName);
}

BEGIN_MESSAGE_MAP(COnlineCJDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineCJDlg)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CLEARCJ, OnButtonClearCJ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnlineCJDlg message handlers

//void COnlineCJDlg::OnBnClickedButtonRefresh()
//{
//	string ID;
//	vector<string> IDList;
//	char Buf[126] = {0};
//	int Position;
//	m_CjobList.DeleteAllItems();
//
//	pGDev->pDataBank->GetCurrentCJobID(ID);
//	m_CurCJName.SetWindowText(ID.c_str());
//	ID.erase();
//	CPRJob *p = CJobMgr.GetActivePRJob();
//	if (p) {
//		ID = p->m_ObjID;
//	}
//	else {
//		ID = "";
//	}
//	pGDev->pDataBank->GetAllCJobIDs(IDList);
//	for(int nIndex = 0;nIndex<IDList.size();nIndex++)
//	{
//		Position= 	m_CjobList.InsertItem(0,_itoa(nIndex+1,Buf,10));
//		m_CjobList.SetItemText(Position,1,IDList[nIndex].c_str());
//	}
//}

BOOL COnlineCJDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_CJOBNAME, TOP_LEFT);
	AddAnchor(IDC_TEXT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_LIST_CJOB, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_CLEARCJ, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_REFRESH, TOP_RIGHT);

	char* dwCjName[] =
	{
		("  NO"), ("  Control Job Name")
	};
	int nSize[] = { 50, 250 };
	m_CjobList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	for (int i = 0; i < 2; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_CjobList.InsertColumn(i, &nListColumn);
	}

	OnButtonRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void COnlineCJDlg::OnButtonRefresh() {
	string ID;
	vector<string> IDList;
	char Buf[126] = { 0 };
	int Position;
	m_CjobList.DeleteAllItems();

	pGDev->GetCurrentCJobID(ID);
	m_CurCJName.SetWindowText(ID.c_str());
	ID.erase();
	CPRJob* p = CJobMgr.GetActivePRJob();
	if (p) {
		ID = p->m_ObjID;
	}
	else {
		ID = "";
	}
	pGDev->GetAllCJobIDs(IDList);
	for (int nIndex = 0; nIndex<int(IDList.size()); nIndex++) {
		Position = m_CjobList.InsertItem(0, _itoa(nIndex + 1, Buf, 10));
		m_CjobList.SetItemText(Position, 1, IDList[nIndex].c_str());
	}
}

void COnlineCJDlg::OnButtonClearCJ() {
	pGDev->ClearJobs();
	m_CjobList.DeleteAllItems();
}

void COnlineCJDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}