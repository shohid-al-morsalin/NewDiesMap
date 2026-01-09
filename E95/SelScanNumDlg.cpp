// SelScanNumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SelScanNumDlg.h"

#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelScanNumDlg dialog

CSelScanNumDlg::CSelScanNumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelScanNumDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelScanNumDlg)
	m_CarrierID = _T("");
	m_WaferID = _T("");
	//}}AFX_DATA_INIT
	ScanNum = 0;
}

void CSelScanNumDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelScanNumDlg)
	DDX_Text(pDX, IDC_CARRIERID, m_CarrierID);
	DDX_Text(pDX, IDC_WAFERID, m_WaferID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelScanNumDlg, CDialog)
	//{{AFX_MSG_MAP(CSelScanNumDlg)
	ON_BN_CLICKED(IDC_SCANFIRST, OnScanfirst)
	ON_BN_CLICKED(IDC_SCANSECOND, OnScansecond)
	ON_BN_CLICKED(IDC_SCANTHIRD, OnScanthird)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelScanNumDlg message handlers

void CSelScanNumDlg::OnScanfirst() {
	ScanNum = 0;
	ExitCheck();
}

void CSelScanNumDlg::OnScansecond() {
	ScanNum = 1;

	CString month;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y%m");
	month += "\\C2C";

	CFileDialog dlg(TRUE,
		"C2C",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Data file|*.C2C|");

	filename = "";
	CString root = DosUtil.DirSys.GetRootDir() + "\\" + month;
	dlg.m_ofn.lpstrInitialDir = root;

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		ExtractIDs(m_CarrierID, m_WaferID, filename);
		UpdateData(FALSE); // This is necessary [3/1/2012 Administrator]
		OnOK();
	}
}

void CSelScanNumDlg::OnScanthird() {
	ScanNum = 2;

	CString month;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y%m");
	month += "\\C2C";

	CFileDialog dlg(TRUE,
		"C2C",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Data file|*.C2C|");

	filename = "";
	CString root = DosUtil.DirSys.GetRootDir() + "\\" + month;
	dlg.m_ofn.lpstrInitialDir = root;

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		ExtractIDs(m_CarrierID, m_WaferID, filename);
		UpdateData(FALSE); // This is necessary [3/1/2012 Administrator]
		OnOK();
	}
}

void CSelScanNumDlg::ExitCheck() {
	UpdateData(TRUE);
	if ((m_CarrierID.GetLength() < 1) || (m_WaferID.GetAllocLength() < 1)) {
		AfxMessageBox("Carrier ID and Wafer ID needed");
		return;
	}
	OnOK();
}

CString CSelScanNumDlg::GetC2CFile() {
	return "";
}

void CSelScanNumDlg::ExtractIDs(CString& CarrierID, CString& WaferID, CString filename) {
	int idx;
	CString str;

	idx = filename.ReverseFind('\\');
	str = filename.Mid(idx + 1);
	idx = str.Find('~');
	CarrierID = str.Left(idx);
	filename = str.Mid(idx + 1);
	idx = filename.Find('~');
	WaferID = filename.Left(idx);
}