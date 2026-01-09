// OnlineCEIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlineCEIDDlg.h"
#include ".\onlineceiddlg.h"
#include "..\online\gem\CediListCtroller.h"
#include "..\online\mfc\VarDisp.h"

#include "GlobalDeviceContainer.h"

#define IDT_START	9999

// COnlineCEIDDlg dialog

COnlineCEIDDlg::COnlineCEIDDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineCEIDDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineCEIDDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COnlineCEIDDlg::~COnlineCEIDDlg() {}

void COnlineCEIDDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineCEIDDlg)
	DDX_Control(pDX, IDC_LIST_CEID, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineCEIDDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineCEIDDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CEID, OnNMDblclkListCeid)
	ON_BN_CLICKED(IDC_ENABLEALLCEID, OnEnableallceid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnlineCEIDDlg message handlers

void COnlineCEIDDlg::OnBnClickedRefresh() {
	Refresh();
}

void COnlineCEIDDlg::Refresh() {
	m_List.DeleteAllItems();

	FILE* fp;
	//int nRowNo = 1;
	char dwBuf[200];
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];
	vector < string> Tokens;

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "CEID", "PAR");

	if ((fp = fopen(inifile, "r")) == NULL) {
		Log("CEID. Parameter File Open Fail");
		return;
	}
	else {
		string Token = dwBuf;
		char nBuf[126] = { 0 };
		int BufInd = 0;
		int nLen = 0;
		//int DataType= 0;
		while (fgets(dwBuf, 200, fp)) {
			BufInd = 0;
			Tokens.clear();
			Token = dwBuf;
			// Check for comments
			if (string::npos != Token.find(';')) {
				continue;
			}
			if (string::npos == Token.find(',')) {
				continue;
			}
			// Tokenize
			nLen = strlen(dwBuf);
			for (int i = 0; i < nLen; i++) {
				if (dwBuf[i] == ',') {
					nBuf[BufInd] = 0;
					Token = nBuf;
					Tokens.push_back(Token);
					memset(nBuf, 0, sizeof nBuf);
					BufInd = 0;
					continue;
				}
				nBuf[BufInd++] = dwBuf[i];
			}
			// Last token
			nBuf[BufInd - 1] = 0;
			Token = nBuf;
			Tokens.push_back(Token);

			CString Flag;
			if (CEIDCtlr.check_rptflag(atoi(Tokens[0].c_str()))) {
				Flag = "Enabled";
			}
			else {
				Flag = "Disabled";
			}
			m_List.AddItem(Tokens[0].c_str(), Tokens[1].c_str(), Flag);

			//nRowNo++;
		}
		if (fclose(fp)) {
			AfxMessageBox("close fail");
		}
	}
}

BOOL COnlineCEIDDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST_CEID, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_REFRESH, TOP_RIGHT);
	AddAnchor(IDC_ENABLEALLCEID, TOP_RIGHT);
	AddAnchor(IDC_MSG, BOTTOM_LEFT);

	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List.SetHeadings(_T("ID,50;Collection event description,360;Enable flag,120"));
	m_List.LoadColumnInfo();

	SetTimer(IDT_START, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineCEIDDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		Refresh();
	}
}

void COnlineCEIDDlg::OnNMDblclkListCeid(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	CString Var = m_List.GetItemText(nSel, 0);
	CVarDisp VarDat;
	VarDat.m_Type = 2;
	VarDat.m_VarNo = atoi(Var);
	VarDat.DoModal();

	*pResult = 0;
}

void COnlineCEIDDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "COnlineCEIDDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineCEIDDlg::OnDestroy() {
	//	m_List.SaveColumnInfo();
	//	m_List.DeleteAllItems();
	CResizableDialog::OnDestroy();
}

BOOL COnlineCEIDDlg::DestroyWindow() {
	m_List.DeleteAllItems();

	return CResizableDialog::DestroyWindow();
}

void COnlineCEIDDlg::OnEnableallceid() {
	CEIDCtlr.evtrpt_all_setting(TRUE);
	CEIDCtlr.SaveEnaDisCEIDAll();
	Refresh();
}