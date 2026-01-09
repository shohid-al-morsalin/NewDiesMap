// ViewFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "ViewFileDlg.h"
#include "InfoPanelView.h"
#include "..\SYS\CUMMsg.h"
#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewFileDlg dialog

CViewFileDlg::CViewFileDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CViewFileDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CViewFileDlg)
	//}}AFX_DATA_INIT
	m_Parent = NULL;

	strFile = "";
	strSearchPath = "";
}

CViewFileDlg::~CViewFileDlg() {}

void CViewFileDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewFileDlg)
	DDX_Control(pDX, IDC_DIRECTORY, m_cDirectory);
	DDX_Control(pDX, IDC_LEFTVIEW, m_cLeft);
	DDX_Control(pDX, IDC_RIGHTVIEW, m_cRight);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewFileDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CViewFileDlg)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LEFTVIEW, OnSelchangedLeftview)
	ON_NOTIFY(NM_CLICK, IDC_RIGHTVIEW, OnClickRightview)
	ON_NOTIFY(NM_DBLCLK, IDC_RIGHTVIEW, OnDblclkRightview)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LOAD2, OnLoad2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewFileDlg message handlers

// Load by file
void CViewFileDlg::OnLoad() {
	if (m_Parent) {
		CString filename;
		if (!DosUtil.DirSys.MakeCurC2CPath(filename)) {
			return;
		}
		CFileDialog dlg(
			TRUE,
			"C2C",
			NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
			"C2C file|*.C2C|");
		dlg.m_ofn.lpstrInitialDir = filename.GetBuffer(0);
		m_cDirectory.SetWindowText(dlg.m_ofn.lpstrInitialDir);
		if (dlg.DoModal() == IDOK) {
			// use standard dialog to get filename for now. Implement tree view later T.W.
			m_Parent->m_FileToLoad = dlg.GetFileName();  // pass filename back to parent
			m_Parent->m_DirToLoad = dlg.GetPathName();
			DosUtil.ExtractPath(m_Parent->m_DirToLoad);
			ShowWindow(SW_HIDE);
			m_Parent->PostMessage(UM_LOADFILE, 413, 0);
		}
	}
}

long CViewFileDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("View\n%s", (char*)lP);
	if (pUDev) {
		pUDev->SetCurrentView(msg);
	}
	return 0;
}

BOOL CViewFileDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LEFTVIEW, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_RIGHTVIEW, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_DIRECTORY, TOP_LEFT, TOP_RIGHT);

	AddAnchor(IDC_LOAD, TOP_RIGHT);
	AddAnchor(IDC_LOAD2, TOP_RIGHT);
	AddAnchor(IDC_REFRESH, BOTTOM_RIGHT);

	m_cLeft.ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);
	m_cRight.ModifyStyle(NULL, LVS_REPORT);
	m_ListImages.Create(IDB_TREE_BMP, 16, 1, RGB(255, 255, 255));
	m_cRight.SetImageList(&m_ListImages, LVSIL_SMALL);

	CreateRoots();
	LoadFolders();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewFileDlg::OnSelchangedLeftview(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = 0;
	// Find out what item is selected in the tree
	HTREEITEM nodSelected = m_cLeft.GetSelectedItem();
	// Get the string of the selected node
	CString strSelected = m_cLeft.GetItemText(nodSelected);

	HTREEITEM nodParentLeague = nodSelected;

	//Build the full path with wild cards
	do {
		nodParentLeague = m_cLeft.GetParentItem(nodParentLeague);
		if (nodParentLeague != NULL) {
			strSelected = m_cLeft.GetItemText(nodParentLeague) + "\\" + strSelected;
		}
	} while (nodParentLeague != NULL);
	strSearchPath = strSelected /*+ "\\*.C2C"*/ + "\\";
	strSearchPath = DosUtil.DirSys.GetRootDir() + "\\" + strSearchPath;
	m_cDirectory.SetWindowText(strSearchPath);

	CString str;
	str = strSearchPath + "*.C2C";
	DisplayFiles(str.GetBuffer(1));

	strSelected = DosUtil.DirSys.GetRootDir() + "\\" + strSelected;
	OpenFolder(strSelected);
}

void CViewFileDlg::OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
	int nItem = m_cRight.GetSelectionMark();
	strFile = m_cRight.GetItemText(nItem, 0);
	strFile.MakeUpper();
	// 	CString str;
	// 	str = strSearchPath + strFile;
	// 	m_cDirectory.SetWindowText(str);
}

void CViewFileDlg::OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
	int nItem = m_cRight.GetSelectionMark();
	CString string = m_cRight.GetItemText(nItem, 0);

		SetFocus();
		//OnRightViewFolderSelected(string, nItem);
		if (m_Parent) {
		m_Parent->m_FileToLoad = string;
			m_Parent->m_DirToLoad = strSearchPath;
			ShowWindow(SW_HIDE);
			m_Parent->PostMessage(UM_LOADFILE, 413, 0);
		}
}

void CViewFileDlg::CreateRoots() {
	m_TreeImages.Create(IDB_TREE_BMP, 16, 1, RGB(255, 255, 255));
}

void CViewFileDlg::OpenFolder(CString CStrPath) {
	HTREEITEM hRoot;
	HTREEITEM hFolder;

	hRoot = m_cLeft.GetSelectedItem();

	HTREEITEM hChild = m_cLeft.GetChildItem(hRoot);

	// Clear the selected node
	while (hChild != 0) {
		m_cLeft.DeleteItem(hChild);
		hChild = m_cLeft.GetChildItem(hRoot);
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	CStrPath = CStrPath + "\\*.*";
	hFind = FindFirstFile(CStrPath, &FindFileData);

	do {
		//long lFSize = FindFileData.nFileSizeLow;
		CString strFSize = "";

		//Want folders that aren't . and ..
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
			&& FindFileData.cFileName != CString(".")
			&& FindFileData.cFileName != CString("..")) {
			hFolder = m_cLeft.InsertItem(FindFileData.cFileName, 2, 3, hRoot);
		}
	} while ((::WaitForSingleObject(m_hStopEventLeft, 0) != WAIT_OBJECT_0) && (::FindNextFile(hFind, &FindFileData)));
	::FindClose(hFind);;
}

void CViewFileDlg::DisplayFiles(LPTSTR Path) {
	ResetFiles();

	if (m_cRight.GetNumColumn() != 3) {
		m_cRight.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		m_cRight.SetHeadings(_T("File Name,360;Date,120;Size,52"));
		m_cRight.LoadColumnInfo();
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(Path, &FindFileData);
	//int n = 0;

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
				&& FindFileData.cFileName != CString(".")
				&& FindFileData.cFileName != CString("..")) {
				m_cRight.AddItem(FindFileData.cFileName, "", "");
			}
		} while ((::WaitForSingleObject(m_hStopEventRight, 0) != WAIT_OBJECT_0) && (::FindNextFile(hFind, &FindFileData)));
		::FindClose(hFind);;

		hFind = FindFirstFile(Path, &FindFileData);
		do {
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				CString strFDate = "";
				FILETIME FT;
				FileTimeToLocalFileTime(&FindFileData.ftLastWriteTime, &FT);
				SYSTEMTIME ST;
				FileTimeToSystemTime(&FT, &ST);
				strFDate.Format("%02d/%02d/%04d %02d:%02d:%02d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond);

				long lFSize = FindFileData.nFileSizeLow;
				CString strFSize = "";
				strFSize.Format("%d", lFSize);

				m_cRight.AddItem(_T(FindFileData.cFileName), _T(strFDate.GetBuffer(0)), _T(strFSize.GetBuffer(0)));
			}
		} while ((::WaitForSingleObject(m_hStopEventRight, 0) != WAIT_OBJECT_0) && (::FindNextFile(hFind, &FindFileData)));
		::FindClose(hFind);;
	}
}

void CViewFileDlg::ResetFiles() {
	m_cRight.DeleteAllItems();
	UpdateWindow();
}

void CViewFileDlg::OnRightViewFolderSelected(CString strPath, UINT index) {
	// Find out what item is selected in the tree
	HTREEITEM nodSelected = m_cLeft.GetSelectedItem();

	//Open up the branch
	m_cLeft.Expand(nodSelected, TVE_EXPAND);

	int count = 0;
	HTREEITEM nodChild;
	nodChild = m_cLeft.GetChildItem(nodSelected);
	if (index > 0) {
		do {
			nodChild = m_cLeft.GetNextItem(nodChild, TVGN_NEXT);
			++count;
		} while (count < (int)index);
	}

	if (nodChild != NULL) {
		m_cLeft.SelectItem(nodChild);
		m_cLeft.Expand(nodChild, TVE_EXPAND);

		nodSelected = nodChild;

		// Get the string of the selected node
		CString strSelected = m_cLeft.GetItemText(nodSelected);

		HTREEITEM nodParentLeague = nodSelected;

		//Build the full path with wild cards
		do {
			nodParentLeague = m_cLeft.GetParentItem(nodParentLeague);
			if (nodParentLeague != NULL) {
				strSelected = m_cLeft.GetItemText(nodParentLeague) + "\\" + strSelected;
			}
		} while (nodParentLeague != NULL);
		CString strSearchPath = strSelected + "\\*.C2C";
		strSearchPath = DosUtil.DirSys.GetRootDir() + "\\" + strSearchPath;
		DisplayFiles(strSearchPath.GetBuffer(1));
	}
}

void CViewFileDlg::OnRefresh() {
	LoadFolders();

	CString str;
	str = strSearchPath + "*.C2C";
	//DisplayFiles(str.GetBuffer(1));
	ResetFiles();
}

void CViewFileDlg::LoadFolders() {
	// If there is anything in the tree, remove it
	m_cLeft.DeleteAllItems();
	m_cLeft.SetImageList(&m_TreeImages, TVSIL_NORMAL);

	OpenFolder(DosUtil.DirSys.GetRootDir());
}

void CViewFileDlg::OnDestroy() {
	m_cLeft.DeleteAllItems();
	m_cRight.DeleteAllItems();
	CResizableDialog::OnDestroy();
}

void CViewFileDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CViewFileDlg::OnLoad2() {
	// TODO: Add your control notification handler code here
}