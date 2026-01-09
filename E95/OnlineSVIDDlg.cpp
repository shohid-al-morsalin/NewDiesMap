// OnlineSVIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlineSVIDDlg.h"
#include ".\onlinesviddlg.h"
#include "..\ONLINE\ShareMemory\Evt.h"
#include "..\ONLINE\ShareMemory\SharedMemQue.h"
#include "..\ONLINE\inc\event.h"
#include "..\ONLINE\Inc\define.h"
#include "..\ONLINE\inc\event.h"
#include "..\ONLINE\Inc\define.h"
#include "..\ONLINE\mfc/VarDisp.h"

#include "GlobalDeviceContainer.h"
#include "..\ONLINE\GEM\SVID_LIST.h"
#include "..\ONLINE\GEM\CediListCtroller.h"

using std::vector; using std::string;

COnlineSVIDDlg::COnlineSVIDDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineSVIDDlg::IDD, pParent) {}

COnlineSVIDDlg::~COnlineSVIDDlg() {}

void COnlineSVIDDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VAR_IDS, m_List);
}

BEGIN_MESSAGE_MAP(COnlineSVIDDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineSVIDDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_VID, OnBnClickedButtonSaveVid)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VAR_IDS, OnNMDblclkListVarIds)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnlineSVIDDlg message handlers
BOOL COnlineSVIDDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST_VAR_IDS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_TEXT, BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_SAVE_VID, BOTTOM_RIGHT);

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_List.SetHeadings(_T("ID,40;Parameter Name,280;Value,100;Data Length,100;Type,96"));
	m_List.LoadColumnInfo();

	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineSVIDDlg::OnBnClickedButtonSaveVid() {
	CFile nLogfile;
	CFileDialog fd(FALSE, "TXT", NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		"SecsLog file|*.TXT|");
	if (fd.DoModal() == IDOK) {
		CString path = fd.GetPathName();
		CSharedMemQue EventQ;
		CEvt* pEvt = new CEvt;
		pEvt->Set((short)EVT_UI_DATA_REQUEST, (short)DUMP_VAR_DATA, path.GetBuffer(0), (short)path.GetLength());
		EventQ.OpenMemory("EVTMSG");
		EventQ.WriteQue(pEvt);
		EventQ.CloseMemory();
	}
}

string COnlineSVIDDlg::GetParamType(int DataType) {
	switch (DataType) {
	case svLIST:
		return "List";
		break;
	case svASCII:
		return "ASCII";
		break;
	case svBINARY:
		return "Binary";
		break;
	case svBOOLEAN:
		return "Boolean";
		break;
	case svUNSIGN1:
		return "Unsigned int (1 byte)";
		break;
	case svUNSIGN2:
		return "Unsigned int (2 byte)";
		break;
	case svUNSIGN4:
		return "Unsigned int (4 byte)";
		break;
	case svUNSIGN8:
		return "Unsigned int (8 byte)";
		break;
	case svINT1:
		return "Int (1 byte)";
		break;
	case svINT2:
		return "Int (2 byte)";
		break;
	case svINT4:
		return "Int (4 byte)";
		break;
	case svINT8:
		return "Int (8 byte)";
		break;
	case svFLOAT4:
		return "Floating Point (4 byte)";
		break;
	case svFLOAT8:
		return "Floating Point (8 byte)";
		break;
	}
	return "";
}

void COnlineSVIDDlg::GetParamType(int DataType, string& Str) {
	string Temp;
	switch (DataType) {
	case svASCII:
		Temp = Str;
		Str = "Max " + Str;
		break;
	case svLIST:
		Str = "UnKnown";
		break;
	}
}

void COnlineSVIDDlg::OnNMDblclkListVarIds(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CString Var = this->m_List.GetItemText(nSel, 0);
	CVarDisp VarDat;
	VarDat.m_Type = 1;
	VarDat.m_VarNo = atoi(Var);
	VarDat.DoModal();

	*pResult = 0;
}

void COnlineSVIDDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "COnlineSVIDDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineSVIDDlg::OnDestroy() {
	m_List.DeleteAllItems();
	CDialog::OnDestroy();
}

void COnlineSVIDDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void COnlineSVIDDlg::Refresh() {
	SVID_LIST* p = &CEIDCtlr.m_SVList;
	for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
		SVID_ITEM* q = &p->m_Svid[i];
		if (q->m_bVarType != SVID_ITEM::EC_TYPE) {
			m_List.AddItem(q->GetSVIDStr(), q->m_svname, q->GetVATypeStr(), q->GetLengthStr(), q->GetTypeStr());
		}
	}
}