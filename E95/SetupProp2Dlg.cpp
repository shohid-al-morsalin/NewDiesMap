#include "stdafx.h"
#include "strSOp.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "FEM/Rbt/selport.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IPC/SMCHdr.h"
#include "SRC/DOSUtil.h"
#include "XTabCtrl.h"

#include "SetupProp2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_OPENTIMEOUT			9999
#define IDT_CLOSETIMEOUT		9998
#define IDT_UNCLAMPTIMEOUT		9997

/////////////////////////////////////////////////////////////////////////////
// CSetupProp2Dlg dialog

CSetupProp2Dlg::CSetupProp2Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp2Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp2Dlg)
	m_RecipeName = _T("");
	m_CarrierID = _T("");
	m_Message = _T("");
	//}}AFX_DATA_INIT

	Port = 0;

	for (int i = 0; i < 5; i++) {
		m_sl[i] = FALSE;
	}
}

CSetupProp2Dlg::~CSetupProp2Dlg() {}

void CSetupProp2Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp2Dlg)
	DDX_Control(pDX, IDC_REPORTVIEW, m_cRptButton);
	DDX_Control(pDX, IDC_REPORT, m_cReport);
	DDX_Control(pDX, IDC_CLOSECARRIER, m_cClose);
	DDX_Control(pDX, IDC_OPENCARRIER, m_cOpen);
	DDX_Control(pDX, IDC_PROCEED, m_cProceed);
	DDX_Control(pDX, IDC_RIGHTVIEW, m_cRight);
	DDX_Control(pDX, IDC_LEFTVIEW, m_cLeft);
	DDX_Text(pDX, IDC_RECIPE, m_RecipeName);
	DDX_Text(pDX, IDC_CARRIERID, m_CarrierID);
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_SLOT1, m_sl[0]);
	DDX_Check(pDX, IDC_SLOT2, m_sl[1]);
	DDX_Check(pDX, IDC_SLOT3, m_sl[2]);
	DDX_Check(pDX, IDC_SLOT4, m_sl[3]);
	DDX_Check(pDX, IDC_SLOT5, m_sl[4]);

	DDX_Control(pDX, IDC_WTHICK1, m_cwtk[0]);
	DDX_Control(pDX, IDC_WTHICK2, m_cwtk[1]);
	DDX_Control(pDX, IDC_WTHICK3, m_cwtk[2]);
	DDX_Control(pDX, IDC_WTHICK4, m_cwtk[3]);
	DDX_Control(pDX, IDC_WTHICK5, m_cwtk[4]);

	DDX_Control(pDX, IDC_FTHICK1, m_cftk[0]);
	DDX_Control(pDX, IDC_FTHICK2, m_cftk[1]);
	DDX_Control(pDX, IDC_FTHICK3, m_cftk[2]);
	DDX_Control(pDX, IDC_FTHICK4, m_cftk[3]);
	DDX_Control(pDX, IDC_FTHICK5, m_cftk[4]);
}

BEGIN_MESSAGE_MAP(CSetupProp2Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp2Dlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LEFTVIEW, OnSelchangedLeftview)
	ON_NOTIFY(NM_CLICK, IDC_RIGHTVIEW, OnClickRightview)
	ON_NOTIFY(NM_DBLCLK, IDC_RIGHTVIEW, OnDblclkRightview)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_BN_CLICKED(IDC_PROCEED, OnProceed)
	ON_BN_CLICKED(IDC_128MODULE, On128module)
	ON_BN_CLICKED(IDC_900MODULE, On900module)
	ON_BN_CLICKED(IDC_OPENCARRIER, OnOpencarrier)
	ON_BN_CLICKED(IDC_CLOSECARRIER, OnClosecarrier)
	ON_BN_CLICKED(IDC_REPORTVIEW, OnReportview)
	ON_WM_TIMER()
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	ON_COMMAND(ID_LP_FOUPOPENOK, OnLpFoupopenok)
	ON_COMMAND(ID_COM_EXITHOME, OnComExithome)
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_COMMAND(ID_COM_ABORTOPEN, OnComAbortopen)
	ON_COMMAND(ID_COM_NOVACUUM, OnComNovacuum)
	ON_COMMAND(ID_COM_WAFEROUT, OnComWaferout)
	ON_COMMAND(ID_COM_VACUUMLOST, OnComVacuumlost)
	ON_COMMAND(ID_COM_SENSORTRIP, OnComSensortrip)
	ON_COMMAND(ID_COM_TP1TIMEOUT, OnComTp1timeout)
	ON_COMMAND(ID_COM_TP2TIMEOUT, OnComTp2timeout)
	ON_COMMAND(ID_COM_TP3TIMEOUT, OnComTp3timeout)
	ON_COMMAND(ID_COM_TP4TIMEOUT, OnComTp4timeout)
	ON_COMMAND(ID_COM_TP5TIMEOUT, OnComTp5timeout)
	ON_COMMAND(ID_COM_TP6TIMEOUT, OnComTp6timeout)
	ON_COMMAND(ID_LP_FOUPCLOSEOK, OnLpFoupcloseok)
	ON_COMMAND(ID_LP_FOUPCLOSEFAIL, OnLpFoupclosefail)
	ON_COMMAND(ID_COM_ABORTCLOSE, OnComAbortclose)
	ON_COMMAND(ID_COM_ENDCLOSE, OnComEndclose)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SAVERESULT, OnSaveresult)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotMapFail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp2Dlg message handlers

long CSetupProp2Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	return 0;
}

BOOL CSetupProp2Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LEFTVIEW, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_RIGHTVIEW, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_OPENCARRIER, TOP_RIGHT);
	AddAnchor(IDC_CLOSECARRIER, TOP_RIGHT);
	AddAnchor(IDC_PROCEED, TOP_RIGHT);
	AddAnchor(IDC_REPORT, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_REPORTVIEW, TOP_RIGHT);
	AddAnchor(IDC_SAVERESULT, TOP_RIGHT);
	AddAnchor(IDC_MESSAGE, TOP_LEFT, TOP_RIGHT);

	m_cLeft.ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

	m_cRight.ModifyStyle(NULL, LVS_REPORT);
	m_ListImages.Create(IDB_TREE_BMP, 16, 1, RGB(255, 255, 255));
	m_cRight.SetImageList(&m_ListImages, LVSIL_SMALL);

	CreateRoots();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp2Dlg::OnSelchangedLeftview(NMHDR* pNMHDR, LRESULT* pResult) {
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
	CString strSearchPath = strSelected + "\\*.C2C";
	strSearchPath = DosUtil.DirSys.GetRootDir() + "\\" + strSearchPath;

	DisplayFiles(strSearchPath.GetBuffer(1));

	strSelected = DosUtil.DirSys.GetRootDir() + "\\" + strSelected;
	OpenFolder(strSelected);
}

void CSetupProp2Dlg::OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	int nItem = m_cRight.GetSelectionMark();
	CString string = m_cRight.GetItemText(nItem, 0);
	string.MakeUpper();
	int idx = string.Find(".C2C");
	if (idx != -1) {
		string = string.Left(idx);
		idx = string.ReverseFind('~');
		if (idx != -1) {
			m_RecipeName = string.Left(idx - 1) + ".RCP";
			m_CarrierID = string.Mid(idx + 1);
		}
		else {
			m_CarrierID = string;
		}
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CSetupProp2Dlg::OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	int nItem = m_cRight.GetSelectionMark();
	CString string = m_cRight.GetItemText(nItem, 0);

	SetFocus();
	OnRightViewFolderSelected(string, nItem);

	*pResult = 0;
}

void CSetupProp2Dlg::DisplayFiles(LPTSTR Path) {
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
				SYSTEMTIME ST;
				FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &ST);
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

void CSetupProp2Dlg::OpenFolder(CString CStrPath) {
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

void CSetupProp2Dlg::OnRightViewFolderSelected(CString strPath, UINT index) {
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
void CSetupProp2Dlg::ResetFiles() {
	m_cRight.DeleteAllItems();
	UpdateWindow();
}

void CSetupProp2Dlg::CreateRoots() {
	// If there is anything in the tree, remove it
	m_cLeft.DeleteAllItems();

	m_TreeImages.Create(IDB_TREE_BMP, 16, 1, RGB(255, 255, 255));

	m_cLeft.SetImageList(&m_TreeImages, TVSIL_NORMAL);

	HTREEITEM hRoot;
	OpenFolder(DosUtil.DirSys.GetRootDir());
	return;

	//char * strBuffer= NULL;
	CString strMessage;

	int nPos = 0;
	//UINT nCount = 0;
	CString strDrive = "?:\\";

	DWORD dwDriveList = ::GetLogicalDrives();

	CString cTmp;

	while (dwDriveList) {
		if (dwDriveList & 1) {
			cTmp = strDrive;
			strDrive.SetAt(0, 0x41 + nPos);

			strDrive = strDrive.Left(2);
			hRoot = m_cLeft.InsertItem(strDrive, 0, 1);
		}
		dwDriveList >>= 1;
		nPos++;
	}
}

void CSetupProp2Dlg::OnLoadrecipe() {
	CString name;
	if (LoadRecipe(name)) {
		m_RecipeName = name;
		UpdateData(FALSE);
	}
}

BOOL CSetupProp2Dlg::LoadRecipe(CString& name) {
	CFileDialog dlg(TRUE,
		"RCP",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Recipe file|*.RCP|");

	name = "";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();

	if (dlg.DoModal() == IDOK) {
		name = dlg.GetPathName();
		DosUtil.DirSys.ExtractFileName(name);
		return TRUE;
	}
	return FALSE;
}

void CSetupProp2Dlg::On128module() {}

void CSetupProp2Dlg::On900module() {}

void CSetupProp2Dlg::OnOpencarrier() {
	// Select which load port to open
	CSelPort dlg;
	if (dlg.DoModal() != IDOK) {
		return;
	}
	Port = dlg.Port;

	pGDev->RegisterWinWithLPMgrEx(m_hWnd, Port);

	m_cOpen.EnableWindow(FALSE);
	m_cClose.EnableWindow(TRUE);

	// Open Carrier and read slotmap
	m_sl[0] = TRUE;
	m_sl[4] = TRUE;

	UpdateData(FALSE);

	OpenFoup();

	// If everything checks up, enable Proceed button
	m_cProceed.EnableWindow(TRUE);
}

void CSetupProp2Dlg::OnProceed() {
	m_cClose.EnableWindow(FALSE);
	m_cOpen.EnableWindow(FALSE);
	m_cProceed.EnableWindow(FALSE);

	BOOL OK = TRUE;
	CString str;
	for (int i = 0; i < 5; i++) {
		if (m_sl[i]) {
			m_cftk[i].GetWindowText(str);
			if (atof(str) == 0) {
				OK = FALSE;
			}
		}
	}

	if (!OK) {
		AfxMessageBox("Data error");
		m_cClose.EnableWindow(TRUE);
		m_cProceed.EnableWindow(TRUE);
	}
}

void CSetupProp2Dlg::OnClosecarrier() {
	m_cClose.EnableWindow(FALSE);

	CloseFoup();

	m_cOpen.EnableWindow(TRUE);
	m_cProceed.EnableWindow(FALSE);
}

void CSetupProp2Dlg::OnReportview() {
	static BOOL bRpt = FALSE;

	if (!bRpt) {
		bRpt = TRUE;
		m_cLeft.ShowWindow(SW_HIDE);
		m_cRight.ShowWindow(SW_HIDE);
		m_cRptButton.SetWindowText("View Files");
	}
	else {
		bRpt = FALSE;
		m_cLeft.ShowWindow(SW_SHOW);
		m_cRight.ShowWindow(SW_SHOW);
		m_cRptButton.SetWindowText("View Report");
	}
}

void CSetupProp2Dlg::OpenFoup() {
	m_Message = "Docking foup";
	UpdateData(FALSE);

	// Open Foup
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPOPENOK;
	SM.Nak = ID_LP_FOUPOPENFAIL;
	SM.Pm.unit = Port;
	SM.Pm.nunit = Port;
	SM.Pm.act = PrcsMgrServices::OPENFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_OPENTIMEOUT, OPENFOUPTIMEOUT + 8000, NULL);
			return;
		}
		PostMessage(WM_COMMAND, SM.Nak, NULL);
		return;
	}
	PostMessage(WM_COMMAND, SM.Nak, NULL);
}

void CSetupProp2Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_OPENTIMEOUT) {
		KillTimer(IDT_OPENTIMEOUT);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}

	CResizableDialog::OnTimer(nIDEvent);
}

void CSetupProp2Dlg::OnLpFoupopenfail() {
	KillTimer(IDT_OPENTIMEOUT);

	CString msg;
	// Note: Do not change the wording below, decoded elsewhere
	CString str;
	str.LoadString(IDS_FOUPOPENFAILED);
	msg.Format("%s (%d)", str, Port);
}

void CSetupProp2Dlg::OnLpFoupopenok() {}

void CSetupProp2Dlg::OnComExithome() {
	m_Message += "\nDock complete\nOpen foup";
	UpdateData(FALSE);
}

void CSetupProp2Dlg::OnComEndopen() {
	KillTimer(IDT_OPENTIMEOUT);

	m_Message += "\nOpen foup complete";
	//Log("Open foup complete",5);
	pGDev->pGEMObjs->Cms[Port - 1].LPObj.CarrierLoc = CLPObj::clDOCKED;
	UpdateData(FALSE);
}

void CSetupProp2Dlg::CancelAndQuit() {}

void CSetupProp2Dlg::OnComAbortopen() {
	OnLpFoupopenfail();
}

void CSetupProp2Dlg::OnComNovacuum() {
	CancelAndQuit();
}

void CSetupProp2Dlg::OnComWaferout() {
	CancelAndQuit();
}

void CSetupProp2Dlg::OnComVacuumlost() {
	CancelAndQuit();
}

void CSetupProp2Dlg::OnComSensortrip() {
	CancelAndQuit();
}

void CSetupProp2Dlg::OnComTp1timeout() {}

void CSetupProp2Dlg::OnComTp2timeout() {}

void CSetupProp2Dlg::OnComTp3timeout() {}

void CSetupProp2Dlg::OnComTp4timeout() {}

void CSetupProp2Dlg::OnComTp5timeout() {}

void CSetupProp2Dlg::OnComTp6timeout() {}

long CSetupProp2Dlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	CString str = (char*)(lP);
	if ((str.Find('2') != -1) ||
		(str.Find('4') != -1) ||
		(str.Find('5') != -1)) {
		OnComSlotMapFail(wP, lP);
		return 0;
	}

	char sm[26];
	strscpy(sm, 25, (char*)(lP + 1));
	sm[25] = 0;

	m_Message += "\nSlotMap read\n";
	m_Message += sm;

	UpdateData(FALSE);
	return 0;
}

long CSetupProp2Dlg::OnComSlotMapFail(WPARAM wP, LPARAM lP) {
	CString str;
	str.LoadString(IDS_SLOTMAPREADFAILED);
	m_Message += "\n" + str;

	UpdateData(FALSE);

	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
		m_Message += "\nWaiting for host";
		UpdateData(FALSE);
		break;
	default:
		break;
	}
	return 0;
}

void CSetupProp2Dlg::CloseFoup() {
	m_Message = "Closing foup";
	UpdateData(FALSE);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = Port;
	SM.Pm.nunit = Port;
	if (DosUtil.GetClampState()) {
		SM.Pm.act = PrcsMgrServices::CLOSENORELEASE;
	}
	else {
		SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	}
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_CLOSETIMEOUT, CLOSEFOUPTIMEOUT + 8000, NULL);
			return;
		}
		msg.Format("Close foup send fail (%d)", Port);
		PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
		return;
	}
	PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
	return;
}

void CSetupProp2Dlg::OnLpFoupcloseok() {
	m_Message += "\nClose foup command OK";
	UpdateData(FALSE);
}

void CSetupProp2Dlg::OnLpFoupclosefail() {
	KillTimer(IDT_CLOSETIMEOUT);
	m_Message += "\nClose foup command fail";
	UpdateData(FALSE);
}

void CSetupProp2Dlg::OnComAbortclose() {
	KillTimer(IDT_CLOSETIMEOUT);
	m_Message += "\nClose foup aborted";
	UpdateData(FALSE);
}

void CSetupProp2Dlg::OnComEndclose() {
	KillTimer(IDT_CLOSETIMEOUT);
	m_Message += "\nClose foup complete";

	UpdateData(FALSE);
}

void CSetupProp2Dlg::OnComCarrierremoved() {
	m_Message += "\nCarrier has left the tool";
	UpdateData(FALSE);
	pGDev->UnRegisterWinWithLPMgrEx(m_hWnd, Port);
}

void CSetupProp2Dlg::OnSize(UINT nType, int cx, int cy) {
	CResizableDialog::OnSize(nType, cx, cy);
}

void CSetupProp2Dlg::OnSaveresult() {}

void CSetupProp2Dlg::OnDestroy() {
	m_cRight.DeleteAllItems();
	CResizableDialog::OnDestroy();
}

void CSetupProp2Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}