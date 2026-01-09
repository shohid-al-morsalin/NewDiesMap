// RecipeManageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "RecipeManageDlg.h"

#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecipeManageDlg dialog

CRecipeManageDlg::CRecipeManageDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CRecipeManageDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CRecipeManageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	bLock = TRUE;
	if (Var.UIMode != 0) {
		bLock = FALSE;
	}

	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
}

void CRecipeManageDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecipeManageDlg)
	DDX_Control(pDX, IDC_LOGIN, m_cLogin);
	DDX_Control(pDX, IDC_REFRESH, m_cRefresh);
	DDX_Control(pDX, IDC_DELETE, m_cDelete);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRecipeManageDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CRecipeManageDlg)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecipeManageDlg message handlers

long CRecipeManageDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Recipe\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	OnRefresh();

	return 0;
}

void CRecipeManageDlg::OnDelete() {
	CString filename;
	m_cList.GetText(m_cList.GetCurSel(), filename);
	CString msg;
	msg.Format("Delete %s", filename);
	if (AfxMessageBox(msg, MB_YESNO) == IDYES) {
		CString StrPath = DosUtil.GetRecipeDir();
		StrPath += "\\" + filename;
		remove(StrPath);
		OnRefresh();
	}
}

BOOL CRecipeManageDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LOGIN, TOP_RIGHT);
	AddAnchor(IDC_DELETE, TOP_RIGHT);
	AddAnchor(IDC_REFRESH, BOTTOM_RIGHT);

	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecipeManageDlg::OnRefresh() {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	CString CStrPath = DosUtil.GetRecipeDir();
	CStrPath += "\\*.*";

	m_cList.ResetContent();

	hFind = FindFirstFile(CStrPath, &FindFileData);
	do {
		//long lFSize = FindFileData.nFileSizeLow;
		CString strFSize = "";

		//Want folders that aren't . and ..
		if (/*FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY*/
			/*&& */FindFileData.cFileName != CString(".")
			&& FindFileData.cFileName != CString("..")) {
			m_cList.AddString(FindFileData.cFileName);
		}
	} while (::FindNextFile(hFind, &FindFileData));
	::FindClose(hFind);;

	EnaDlgItems(bLock);
}

void CRecipeManageDlg::OnDblclkList1() {
	OnDelete();
}

void CRecipeManageDlg::LockView() {
	bLock = TRUE;
	EnaDlgItems(bLock);
}

void CRecipeManageDlg::EnaDlgItems(BOOL bEn) {
	(HWND*)GetDlgItem(IDC_LIST1)->EnableWindow(!bEn);
	(HWND*)GetDlgItem(IDC_DELETE)->EnableWindow(!bEn);
	(HWND*)GetDlgItem(IDC_REFRESH)->EnableWindow(!bEn);
	(HWND*)GetDlgItem(IDC_LOGIN)->EnableWindow(bEn);
}

void CRecipeManageDlg::OnLogin() {
	if (bLock) {
		short AutheLevel;
		if (pGDev->AuthenticateUser(AutheLevel)) {
			bLock = FALSE;
			EnaDlgItems(bLock);
		}
	}
}

void CRecipeManageDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}