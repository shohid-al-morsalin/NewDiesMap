// SetupProp42Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "e95.h"
#include "SetupProp42Dlg.h"

#include "XTabCtrl.h"
#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"

#include "413App.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"

#include "SimuSpecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_START	9999

/////////////////////////////////////////////////////////////////////////////
// CSetupProp42Dlg dialog

CSetupProp42Dlg::CSetupProp42Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp42Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp42Dlg)
	m_NumberOfMappedPoints = 65536;
	m_TFScalingFactor = 0.0f;
	m_nCycle = 1;
	m_Difference = 0.0f;
	m_Difference2 = 0.0f;
	m_Smooth = 1;
	m_Expand = 1;
	m_Method = 0;
	//}}AFX_DATA_INIT

	int i;

	ParentDirc = ".";

	CurNo = 0;

	for (i = 0; i < 4; i++) {
		Amp[i] = 1;
		Cycle[i] = Phase[i] = 0;
	}
	for (i = 0; i < MAXTHKVLU; i++) {
		m_FThkMax[i] = 10.0f;
		m_FThkMin[i] = 0.0f;
		m_RI[i] = 3.69f;
	}

	bakname = "";
}

void CSetupProp42Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp42Dlg)
	DDX_Control(pDX, IDC_THKRANGE, m_cThkRange);
	DDX_Control(pDX, IDC_SHOWSPEC, m_cShowSpec);
	DDX_Control(pDX, IDC_CYCLEFIRST, m_cCycleFirst);
	DDX_Control(pDX, IDC_MESSAGE2, m_cMessage1);
	DDX_Control(pDX, IDC_MESSAGE3, m_cMessage2);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_GENERATEREPORT, m_cGenerateReport);
	DDX_Control(pDX, IDC_TOP, m_cTop);
	DDX_Control(pDX, IDC_LEFT, m_cLeft);
	DDX_Control(pDX, IDC_RIGHTVIEW, m_cRight);
	DDX_Control(pDX, IDC_SPEC4, m_cSpec4);
	DDX_Control(pDX, IDC_SPEC3, m_cSpec3);
	DDX_Control(pDX, IDC_SPEC2, m_cSpec2);
	DDX_Control(pDX, IDC_SPEC1, m_cSpec1);
	DDX_Text(pDX, IDC_NUMBEROFMAPPEDPOINTS, m_NumberOfMappedPoints);
	DDX_Text(pDX, IDC_TFSCALEFACTOR, m_TFScalingFactor);
	DDX_Text(pDX, IDC_NCYCLE, m_nCycle);
	DDX_Text(pDX, IDC_DIFFERENCE, m_Difference);
	DDX_Text(pDX, IDC_DIFFERENCE2, m_Difference2);
	DDX_Text(pDX, IDC_SMOOTH, m_Smooth);
	DDX_Text(pDX, IDC_EXPAND, m_Expand);
	DDX_Text(pDX, IDC_METHOD_SP4, m_Method);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FTHKMAX_SP4, m_FThkMax[0]);
	DDX_Text(pDX, IDC_FTHKMAX_SP5, m_FThkMax[1]);
	DDX_Text(pDX, IDC_FTHKMAX_SP6, m_FThkMax[2]);
	DDX_Text(pDX, IDC_FTHKMAX_SP7, m_FThkMax[3]);
	DDX_Text(pDX, IDC_FTHKMAX_SP8, m_FThkMax[4]);
	DDX_Text(pDX, IDC_FTHKMAX_SP9, m_FThkMax[5]);
	DDX_Text(pDX, IDC_FTHKMIN_SP4, m_FThkMin[0]);
	DDX_Text(pDX, IDC_FTHKMIN_SP5, m_FThkMin[1]);
	DDX_Text(pDX, IDC_FTHKMIN_SP6, m_FThkMin[2]);
	DDX_Text(pDX, IDC_FTHKMIN_SP7, m_FThkMin[3]);
	DDX_Text(pDX, IDC_FTHKMIN_SP8, m_FThkMin[4]);
	DDX_Text(pDX, IDC_FTHKMIN_SP9, m_FThkMin[5]);
	DDX_Text(pDX, IDC_RI1, m_RI[0]);
	DDX_Text(pDX, IDC_RI2, m_RI[1]);
	DDX_Text(pDX, IDC_RI3, m_RI[2]);
	DDX_Text(pDX, IDC_RI4, m_RI[3]);
	DDX_Text(pDX, IDC_RI5, m_RI[4]);
	DDX_Text(pDX, IDC_RI6, m_RI[5]);
}

BEGIN_MESSAGE_MAP(CSetupProp42Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp42Dlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LEFT, OnDblclkLeft)
	ON_NOTIFY(NM_DBLCLK, IDC_TOP, OnDblclkTop)
	ON_NOTIFY(NM_DBLCLK, IDC_RIGHTVIEW, OnDblclkRightview)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_EN_KILLFOCUS(IDC_NUMBEROFMAPPEDPOINTS, OnKillfocusNumberofmappedpoints)
	ON_BN_CLICKED(IDC_GENERATEREPORT, OnGeneratereport)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP4, OnKillfocusFthkmaxSp4)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP4, OnKillfocusFthkminSp4)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP5, OnKillfocusFthkmaxSp5)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP5, OnKillfocusFthkminSp5)
	ON_EN_KILLFOCUS(IDC_TFSCALEFACTOR, OnKillfocusTfscalefactor)
	ON_EN_KILLFOCUS(IDC_NCYCLE, OnKillfocusNcycle)
	ON_BN_CLICKED(IDC_SAVETOP, OnSavetop)
	ON_BN_CLICKED(IDC_SAVEBOTTOM, OnSavebottom)
	ON_BN_CLICKED(IDC_SAVESPECTRUM, OnSavespectrum)
	ON_NOTIFY(NM_CLICK, IDC_RIGHTVIEW, OnClickRightview)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RIGHTVIEW, OnItemchangedRightview)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LOADSPECTRUM, OnLoadspectrum)
	ON_EN_KILLFOCUS(IDC_RI1, OnKillfocusRi1)
	ON_EN_KILLFOCUS(IDC_RI2, OnKillfocusRi2)
	ON_BN_CLICKED(IDC_LOADSPECTRUM2, OnLoadspectrum2)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio7)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio8)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio9)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio10)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO11, OnRadio11)
	ON_BN_CLICKED(IDC_SIMULATE, OnSimulate)
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP6, OnKillfocusFthkminSp6)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP6, OnKillfocusFthkmaxSp6)
	ON_EN_KILLFOCUS(IDC_RI3, OnKillfocusRi3)
	ON_BN_CLICKED(IDC_LOADSPECTRUM3, OnLoadspectrum3)
	ON_EN_KILLFOCUS(IDC_SMOOTH, OnKillfocusSmooth)
	ON_BN_CLICKED(IDC_CYCLEFIRST, OnCyclefirst)
	ON_BN_CLICKED(IDC_SHOWSPEC, OnShowspec)
	ON_EN_KILLFOCUS(IDC_EXPAND, OnKillfocusExpand)
	ON_BN_CLICKED(IDC_LOADREF, OnLoadref)
	ON_EN_KILLFOCUS(IDC_METHOD_SP4, OnKillfocusMethod)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP7, OnKillfocusFthkminSp7)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP8, OnKillfocusFthkminSp8)
	ON_EN_KILLFOCUS(IDC_FTHKMIN_SP9, OnKillfocusFthkminSp9)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP7, OnKillfocusFthkmaxSp7)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP8, OnKillfocusFthkmaxSp8)
	ON_EN_KILLFOCUS(IDC_FTHKMAX_SP9, OnKillfocusFthkmaxSp9)
	ON_EN_KILLFOCUS(IDC_RI4, OnKillfocusRi4)
	ON_EN_KILLFOCUS(IDC_RI5, OnKillfocusRi5)
	ON_EN_KILLFOCUS(IDC_RI6, OnKillfocusRi6)
	ON_BN_CLICKED(IDC_SET1, OnSet1)
	ON_BN_CLICKED(IDC_SET2, OnSet2)
	ON_BN_CLICKED(IDC_SET3, OnSet3)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp42Dlg message handlers

void CSetupProp42Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

long CSetupProp42Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	if (!bTabSelected) {
		OpenFolder(Dirc);

		m_cSpec1.RSpec.Clear();
		m_cSpec2.RSpec.Clear();
		m_cSpec3.RSpec.Clear();
		m_cSpec4.RSpec.Clear();

		RecipeToLocal();

		UpdateData(FALSE);

		m_cCycleFirst.SetCheck(g->bCycleFirst);
		m_cShowSpec.SetCheck(g->bShowSpec);

		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp42Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		UpdateData(TRUE);
		LocalToRecipe();
	}

	bTabSelected = FALSE;

	return 0;
}

BOOL CSetupProp42Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_LOADRECIPE, TOP_RIGHT);
	AddAnchor(IDC_GENERATEREPORT, TOP_RIGHT);
	AddAnchor(IDC_LEFT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_RIGHTVIEW, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_SAVESPECTRUM, TOP_RIGHT);
	AddAnchor(IDC_LOADSPECTRUM, TOP_RIGHT);
	AddAnchor(IDC_LOADSPECTRUM2, TOP_RIGHT);
	AddAnchor(IDC_LOADSPECTRUM3, TOP_RIGHT);
	AddAnchor(IDC_LOADREF, TOP_RIGHT);
	AddAnchor(IDC_SIMULATE, BOTTOM_RIGHT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	m_cTop.ModifyStyle(NULL, LVS_REPORT);
	m_cLeft.ModifyStyle(NULL, LVS_REPORT);
	m_cRight.ModifyStyle(NULL, LVS_REPORT);

	if (m_cTop.GetNumColumn() != 1) {
		m_cTop.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		m_cTop.SetHeadings(_T("Drives,210"));
		m_cTop.LoadColumnInfo();
	}
	if (m_cLeft.GetNumColumn() != 1) {
		m_cLeft.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		m_cLeft.SetHeadings(_T("Folders,210"));
		m_cLeft.LoadColumnInfo();
	}
	if (m_cRight.GetNumColumn() != 3) {
		m_cRight.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		m_cRight.SetHeadings(_T("File Names,256;Date,120;Size,52"));
		m_cRight.LoadColumnInfo();
	}

	size_t lAllDriveStrings = GetLogicalDriveStrings(0, NULL);

	_TCHAR* m_pDriveStrings;	// list of drive strings: A:\ B:\ etc.
	m_pDriveStrings = new _TCHAR[lAllDriveStrings + sizeof(_T(""))]; // + for trailer
	if (GetLogicalDriveStrings(lAllDriveStrings, m_pDriveStrings) == lAllDriveStrings - 1) {
		_TCHAR* pDriveString = m_pDriveStrings;
		size_t  lDriveString = strlen(pDriveString);
		while (lDriveString > 0) {
			// retrieve display string
			SHFILEINFO FileInfo;
			DWORD r = SHGetFileInfo(pDriveString, 0, &FileInfo, sizeof(FileInfo), SHGFI_DISPLAYNAME);
			if (r != 0) // failure - which can be ignored
			{
				CString str = FileInfo.szDisplayName;
				if (str.Find("C:") > 0) {
					m_cTop.AddItem(pDriveString, 0);
				}
				else if (str.Find("D:" > 0)) {
					m_cTop.AddItem(pDriveString, 0);
				}
				else if (str.Find("E:" > 0)) {
					m_cTop.AddItem(pDriveString, 0);
				}
				else if (str.Find("F:" > 0)) {
					m_cTop.AddItem(pDriveString, 0);
				}
			}
			// setup for next drive string (next round in loop)
			pDriveString += lDriveString + 1;
			lDriveString = strlen(pDriveString);
		}
	}
	if (m_pDriveStrings) {
		delete[] m_pDriveStrings;
	}

	Dirc = "C:\\TEMP\\";

	SetTimer(IDT_START, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp42Dlg::OpenFolder(CString CStrPath) {
	HANDLE m_hStopEventLeft = 0;

	m_cLeft.DeleteAllItems();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int len = CStrPath.GetLength();
	if (CStrPath.GetAt(len - 1) != '\\') {
		CStrPath += "\\";
	}
	CStrPath += "*.*";

	hFind = ::FindFirstFile(CStrPath, &FindFileData);

	do {
		long lFSize = FindFileData.nFileSizeLow;
		CString strFSize = "";

		//Want folders that aren't . and ..
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
			/*&& FindFileData.cFileName != CString (".")*/
			&& FindFileData.cFileName != CString("..")) {
			if (FindFileData.cFileName == CString(".")) {
				m_cLeft.AddItem(ParentDirc);
			}
			else {
				m_cLeft.AddItem(FindFileData.cFileName);
			}
		}
	} while ((::WaitForSingleObject(m_hStopEventLeft, 0) != WAIT_OBJECT_0) && (::FindNextFile(hFind, &FindFileData)));
	::FindClose(hFind);

	DisplayFiles(CStrPath.GetBuffer(0));
}

void CSetupProp42Dlg::DisplayFiles(LPTSTR Path) {
	m_cRight.DeleteAllItems();
	UpdateWindow();

	HANDLE m_hStopEventRight = 0;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(Path, &FindFileData);
	int n = 0;

	if (hFind != INVALID_HANDLE_VALUE) {
		hFind = FindFirstFile(Path, &FindFileData);
		do {
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				CString str;
				str = FindFileData.cFileName;
				str.MakeUpper();
				if ((str.Find("-0_1.CSV") != -1) || (str.Find("-0_2.CSV") != -1)) {
					int len = str.ReverseFind('.');
					if (len > 0) {
						str = str.Mid(len);
						str.MakeUpper();
						if (str == ".CSV") {
							CString strFDate = "";
							SYSTEMTIME ST;
							FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &ST);
							strFDate.Format("%02d/%02d/%04d %02d:%02d:%02d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond);

							long lFSize = FindFileData.nFileSizeLow;
							CString strFSize = "";
							strFSize.Format("%d", lFSize);
							m_cRight.AddItem(_T(FindFileData.cFileName), _T(strFDate.GetBuffer(0)), _T(strFSize.GetBuffer(0)));
						}
					}
				}
			}
		} while ((::WaitForSingleObject(m_hStopEventRight, 0) != WAIT_OBJECT_0) && (::FindNextFile(hFind, &FindFileData)));
		::FindClose(hFind);
	}
}

void CSetupProp42Dlg::OnDblclkLeft(NMHDR* pNMHDR, LRESULT* pResult) {
	int  nItem = m_cLeft.GetSelectionMark();
	if (nItem > -1) {
		CString string = m_cLeft.GetItemText(nItem, 0);

		if (string != ParentDirc) {
			if (Dirc.GetAt(Dirc.GetLength() - 1) != '\\') {
				Dirc += "\\";
			}
			Dirc += string;
			Dirc += "\\";
			OpenFolder(Dirc);
		}
		else {
			int len = Dirc.GetLength();
			if (Dirc.GetAt(len - 1) == '\\') {
				Dirc = Dirc.Left(len - 1);
			}
			int n = Dirc.ReverseFind('\\');
			if (n > 1) {
				Dirc = Dirc.Left(n);
				OpenFolder(Dirc);
			}
		}
	}

	*pResult = 0;
}

void CSetupProp42Dlg::OnDblclkTop(NMHDR* pNMHDR, LRESULT* pResult) {
	int  nItem = m_cTop.GetSelectionMark();
	if (nItem > -1) {
		Dirc = m_cTop.GetItemText(nItem, 0);
		OpenFolder(Dirc);
	}

	*pResult = 0;
}

void CSetupProp42Dlg::OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	int  nItem = m_cRight.GetSelectionMark();
	if (nItem > -1) {
		CString spec;
		if (Dirc.GetAt(Dirc.GetLength() - 1) != '\\') {
			Dirc += "\\";
		}
		spec = Dirc + m_cRight.GetItemText(nItem, 0);
		DoCalculationB(spec);
	}

	*pResult = 0;
}

BOOL CSetupProp42Dlg::OpenSpecFile(CString SpecName, short n) {
	COceanBase* pOcean = p413App->pOceanOp;
	if (!pOcean) {
		return FALSE;
	}
	CMeParam* p;

	CSpec* pIn;
	CSpec* pOut;

	switch (n) {
	case 0:
		pIn = &ISpec1;
		break;
	case 1:
		pIn = &ISpec2;
		break;
	default:
		n = 2;
		pIn = &ISpec3;
		break;
	}
	if (!p413App->OpenSpecFile(SpecName, pIn)) return FALSE;

	CurNo = n;
	p = &pRcp->MeParam[g->CurMeSet - 1];
	switch (n) {
	case 0:
		pOut = &OSpec1;
		break;
	case 1:
		pOut = &OSpec1;
		break;
	default:
		n = 2;
		pOut = &OSpec2;
		break;
	}
	float Poly[2] = { 0, 0 };
	pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
	pOcean->ClearPolyThk(pIn);
	pOcean->SetActivatePoly(pIn, 0, p->Ta.ThkMin[n], p->Ta.ThkMax[n], p->Ta.RI[n]);
	pOcean->Result(pIn, pOut, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
	DisplaySpectrum(n, SpecName, pOut);

	return TRUE;
}

void CSetupProp42Dlg::OnLoadrecipe() {
	RecipeIO();
	// 	if (LoadRcpSetup())
	// 	{
	// 		RecipeToLocal();
	// 	}
}

// pthk array terminates with a zero [11/20/2012 Yuen]
void CSetupProp42Dlg::DisplaySpectrum(short n, CString name, CSpec* pRSpec) {
	COceanBase* pOcean = p413App->pOceanOp;
	if (g && pOcean) {
		CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];
		CSpecPlot* pSpec;
		if (n == 0) {
			pSpec = &m_cSpec1;
		}
		else if (n == 1) {
			pSpec = &m_cSpec2;
		}
		else if (n == 2) {
			pSpec = &m_cSpec3;
		}
		else {
			pSpec = &m_cSpec4;
		}
		pSpec->RSpec.Dup(*pRSpec);
		pSpec->method = p->Ta.Method;
		CString msg = "";
		msg.Format("%s [%d %d %.4f]", name, m_NumberOfMappedPoints, m_nCycle, g->TFScalingFactor);
		pSpec->msgStr = msg;
		pSpec->Refresh();
	}

	UpdateData(FALSE);
}

void CSetupProp42Dlg::OnKillfocusNumberofmappedpoints() {
	UpdateData(TRUE);

	if (m_NumberOfMappedPoints >= 65536) {
		m_NumberOfMappedPoints = 65536;
	}
	else if (m_NumberOfMappedPoints >= 32768) {
		m_NumberOfMappedPoints = 32768;
	}
	else if (m_NumberOfMappedPoints >= 16384) {
		m_NumberOfMappedPoints = 16384;
	}
	else if (m_NumberOfMappedPoints >= 8192) {
		m_NumberOfMappedPoints = 8192;
	}
	else if (m_NumberOfMappedPoints >= 4096) {
		m_NumberOfMappedPoints = 4096;
	}
	else if (m_NumberOfMappedPoints >= 2048) {
		m_NumberOfMappedPoints = 2048;
	}
	else if (m_NumberOfMappedPoints >= 1024) {
		m_NumberOfMappedPoints = 1024;
	}
	else {
		m_NumberOfMappedPoints = 512;
	}
	UpdateData(FALSE);

	g->RcpSetup.nMappedPoints = m_NumberOfMappedPoints;
	if (p413App->pOceanOp) {
		p413App->pOceanOp->nFFTSP = m_NumberOfMappedPoints / p413App->pOceanOp->nFFTFac;
	}
	RecalculateSpectrum(CurNo);
}

void CSetupProp42Dlg::OnGeneratereport() {
	m_cGenerateReport.EnableWindow(FALSE);
	CString v, Filename, Filebase;
	char str[65];

	m_cRight.GetItemText(0, 0, str, 64);
	Filename = str;
	Filebase = Filename.Left(Filename.Find("-", 0));

	v = CTime::GetCurrentTime().Format("%H%M%S");
	Filename.Format("%sRP%s.CSV", Dirc, v);

	FILE* fp = fopen(Filename, "wb");
	if (fp) {
		fprintf(fp, "No,OD,STI,OD-STI\n");
		CString str1, str2, str3;
		for (int i = 0; i < m_cRight.GetItemCount() / 2 + 1; i++) {
			str1.Format("%03d-1.TXT", i);
			if (!OpenSpecFile(Dirc + str1, 0)) continue;
			str3.Format("%s%.3d", Dirc, i);
			m_cSpec1.SaveBitmapA(str3, 1);
			while (m_cSpec1.bSaveBitmap) {
				Wait();
			}
			str2.Format("%03d-2.TXT", i);
			if (!OpenSpecFile(Dirc + str2, 1)) continue;
			fprintf(fp, "%d,%s,%s\n", i, str1.GetBuffer(0), str2.GetBuffer(0));
			m_cSpec2.SaveBitmapA(str3, 2);
			while (m_cSpec2.bSaveBitmap) {
				Wait();
			}
			str2.Format("%03d-3.TXT", i);
			if (!OpenSpecFile(Dirc + str2, 1)) continue;
			fprintf(fp, "%d,%s,%s\n", i, str1.GetBuffer(0), str2.GetBuffer(0));
			m_cSpec3.SaveBitmapA(str3, 2);
			while (m_cSpec3.bSaveBitmap) {
				Wait();
			}
			Sleep(25);
		}
		fclose(fp);
	}
	m_cGenerateReport.EnableWindow(TRUE);
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp4() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[0] = m_FThkMax[0];
	RecalculateSpectrum(0);
}

void CSetupProp42Dlg::OnKillfocusFthkminSp4() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[0] = m_FThkMin[0];
	RecalculateSpectrum(0);
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp5() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[1] = m_FThkMax[1];
	RecalculateSpectrum(1);
}

void CSetupProp42Dlg::OnKillfocusFthkminSp5() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[1] = m_FThkMin[1];
	RecalculateSpectrum(1);
}

void CSetupProp42Dlg::RecipeToLocal() {
	CString str;

	str.Format("Thickness Range (um): PaSet %d", g->CurPaSet);
	m_cThkRange.SetWindowText(str);

	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < MAXTHKVLU; i++) {
		m_FThkMax[i] = pRcp->MeParam[MeS].Ta.ThkMax[i];
		m_FThkMin[i] = pRcp->MeParam[MeS].Ta.ThkMin[i];
		m_RI[i] = pRcp->MeParam[MeS].Ta.RI[i];
	}

	CMeParam* p = &pRcp->MeParam[MeS];
	m_Method = p->Ta.Method;

	m_TFScalingFactor = g->TFScalingFactor;

	m_nCycle = pRcp->nCycle;
	m_Smooth = pRcp->NSmooth;
	m_Expand = pRcp->nExpand;
	m_NumberOfMappedPoints = pRcp->nMappedPoints;
	UpdateData(FALSE);

	COceanBase* pOcean = p413App->pOceanOp;
	if (pOcean) {
		pOcean->nFFTSP = m_NumberOfMappedPoints / pOcean->nFFTFac;
	}

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(FALSE);
	switch (pRcp->FFTFilter[g->CurMeSet - 1]) {
	case CSpec::NONE:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		break;
	case CSpec::HANN:
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(TRUE);
		break;
	case CSpec::HAMMING:
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(TRUE);
		break;
	case CSpec::GAUSIAN:
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(TRUE);
		break;
	case CSpec::EXPONENT:
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(TRUE);
		break;
	case CSpec::BARTLETT:
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
		break;
	case CSpec::BLACKMAN:
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(TRUE);
		break;
	}

	UpdateData(FALSE);
}

void CSetupProp42Dlg::LocalToRecipe() {
	UpdateData(TRUE);

	short MeS = g->CurMeSet - 1;
	for (int i = 0; i < MAXTHKVLU; i++) {
		pRcp->MeParam[MeS].Ta.ThkMax[i] = m_FThkMax[i];
		pRcp->MeParam[MeS].Ta.ThkMin[i] = m_FThkMin[i];
		pRcp->MeParam[MeS].Ta.RI[i] = m_RI[i];
	}
	pRcp->nCycle = m_nCycle;
	pRcp->NSmooth = m_Smooth;
	pRcp->nExpand = m_Expand;
	pRcp->nMappedPoints = m_NumberOfMappedPoints;

	CMeParam* p = &pRcp->MeParam[MeS];
	p->Ta.Method = m_Method;
}

void CSetupProp42Dlg::OnKillfocusTfscalefactor() {
	UpdateData(TRUE);
	g->TFScalingFactor = m_TFScalingFactor;

	// Need to save this [6/7/2011 XPMUser]
	g->SaveThinFilmOption();
	RecalculateSpectrum(CurNo);
}

void CSetupProp42Dlg::OnKillfocusNcycle() {
	UpdateData(TRUE);
	if (m_nCycle < 1) {
		m_nCycle = 1;
		UpdateData(FALSE);
	}
	g->RcpSetup.nCycle = m_nCycle;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnSavetop() {
	CString name;
	name.Format("C:\\TEMP\\%s", SpecName);
	m_cSpec1.SaveBitmapA(name, 1);
}

void CSetupProp42Dlg::OnSavebottom() {
	CString name;
	name.Format("C:\\TEMP\\%s", SpecName);
	m_cSpec2.SaveBitmapA(name, 2);
	m_cSpec3.SaveBitmapA(name, 3);
}

void CSetupProp42Dlg::OnSavespectrum() {
	CString name;
	name.Format("C:\\TEMP\\%s", SpecName);
	m_cSpec1.SaveBitmapA(name, 1);
	m_cSpec2.SaveBitmapA(name, 2);
	m_cSpec3.SaveBitmapA(name, 3);
}

void CSetupProp42Dlg::OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	OnDblclkRightview(pNMHDR, pResult);
	*pResult = 0;
}

void CSetupProp42Dlg::OnItemchangedRightview(NMHDR* pNMHDR, LRESULT* pResult) {
	OnDblclkRightview(pNMHDR, pResult);
	*pResult = 0;
}

void CSetupProp42Dlg::Wait() {
	MSG msg;
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CSetupProp42Dlg::OnTimer(UINT nIDEvent) {
	if (!g) {
		return;
	}
	if (nIDEvent == IDT_START) {
		RecipeToLocal();
		COceanBase* pOcean = p413App->pOceanOp;
		if (pOcean) {
			KillTimer(IDT_START);
			pOcean->nFFTSP = m_NumberOfMappedPoints / pOcean->nFFTFac;
		}
	}
}

void CSetupProp42Dlg::OnLoadspectrum() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Spectrum file|*.CSV|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP";

	if (dlg.DoModal() == IDOK) {
		CSpec* pIn = &ISpec1;
		filename = dlg.GetPathName();
		if (p413App->OpenSpecCSV(filename, pIn)) {
			RecalculateSpectrum(0);
		}
	}
}

void CSetupProp42Dlg::OnLoadspectrum2() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Spectrum file|*.CSV|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP";

	if (dlg.DoModal() == IDOK) {
		CSpec* pIn = &ISpec2;
		filename = dlg.GetPathName();
		if (p413App->OpenSpecCSV(filename, pIn)) {
			RecalculateSpectrum(1);
		}
	}
}

float CSetupProp42Dlg::RecalculateSpectrum(short which) {
	UpdateData(TRUE);

	COceanBase* pOcean = p413App->pOceanOp;
	if (!pOcean) {
		return 0;
	}
	CSpec* pIn;
	CSpec* pOut;
	CMeParam* p;
	p = &pRcp->MeParam[g->CurMeSet - 1];
	pIn = &ISpec1;
	switch (which) {
	case 0:
		pOut = &OSpec1;
		p->Ta.RI[which] = m_RI[which];
		p->Ta.ThkMax[which] = m_FThkMax[which];
		p->Ta.ThkMin[which] = m_FThkMin[which];
		break;
	case 1:
		pOut = &OSpec2;
		p->Ta.RI[which] = m_RI[which];
		p->Ta.ThkMax[which] = m_FThkMax[which];
		p->Ta.ThkMin[which] = m_FThkMin[which];
		break;
	default:
		which = 2;
		pOut = &OSpec3;
		p->Ta.RI[which] = m_RI[which];
		p->Ta.ThkMax[which] = m_FThkMax[which];
		p->Ta.ThkMin[which] = m_FThkMin[which];
		break;
	}
	CurNo = which;

	pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
	pOcean->ClearPolyThk(pIn);
	pOcean->SetActivatePoly(pIn, 0, p->Ta.ThkMin[which], p->Ta.ThkMax[which], p->Ta.RI[which]);
	pOcean->Result(pIn, pOut, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
	DisplaySpectrum(which, SpecName, pOut);
	return pOut->Poly[0].fThk;
}

void CSetupProp42Dlg::OnKillfocusRi1() {
	UpdateData(TRUE);
	CurNo = 0;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[0] = m_RI[0];
	RecalculateSpectrum(0);
}

void CSetupProp42Dlg::OnKillfocusRi2() {
	UpdateData(TRUE);
	CurNo = 1;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[1] = m_RI[1];
	RecalculateSpectrum(1);
}

void CSetupProp42Dlg::OnRadio7() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::HANN;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio8() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::HAMMING;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio9() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::GAUSIAN;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio10() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::EXPONENT;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio3() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::BARTLETT;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio1() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::NONE;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnRadio11() {
	pRcp->FFTFilter[g->CurMeSet - 1] = CSpec::BLACKMAN;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnSimulate() {
	int i;
	CSimuSpecDlg dlg;

	for (i = 0; i < 4; i++) {
		dlg.m_Amp[i] = Amp[i];
		dlg.m_Cycle[i] = Cycle[i];
		dlg.m_Phase[i] = Phase[i];
	}
	if (dlg.DoModal() != IDCANCEL) {
		for (i = 0; i < 4; i++) {
			Amp[i] = dlg.m_Amp[i];
			Cycle[i] = dlg.m_Cycle[i];
			Phase[i] = dlg.m_Phase[i];

			GenerateSpectrum(dlg.Spec);
		}
	}
}

BOOL CSetupProp42Dlg::GenerateSpectrum(short which) {
	CSpec* pIn = &ISpec2;
	if (pIn->SimuSpec(4, Cycle, Amp, Phase)) {
		RecalculateSpectrum(which);
		return TRUE;
	}
	return FALSE;
}

// Customize method for UMC thin film measurement [11/20/2012 Yuen]
void CSetupProp42Dlg::OnCalculate() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Spectrum file|*.CSV|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP";

	if (dlg.DoModal() == IDOK) {
		UpdateData(TRUE);
		filename = dlg.GetPathName();
		DoCalculationB(filename);
	}
}

void CSetupProp42Dlg::OnKillfocusFthkminSp6() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[2] = m_FThkMin[2];
	RecalculateSpectrum(2);
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp6() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[2] = m_FThkMax[2];
	RecalculateSpectrum(2);
}

void CSetupProp42Dlg::OnKillfocusRi3() {
	UpdateData(TRUE);
	CurNo = 2;
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[2] = m_RI[2];
	RecalculateSpectrum(2);
}

void CSetupProp42Dlg::OnLoadspectrum3() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Spectrum file|*.CSV|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP";
	if (dlg.DoModal() == IDOK) {
		CSpec* pIn = &ISpec3;
		filename = dlg.GetPathName();
		if (p413App->OpenSpecCSV(filename, pIn)) {
			RecalculateSpectrum(2);
		}
	}
}

void CSetupProp42Dlg::DoCalculationA(CString filename) {
	if (p413App->OpenSpecCSV(filename, &ISpec1)) {
		//ISpec2.Dup(ISpec1);
		//ISpec3.Dup(ISpec1);
		float Poly1, Poly2, Poly3;
		Poly1 = RecalculateSpectrum(0);
		m_FThkMin[1] = m_FThkMin[0];
		m_FThkMax[1] = m_FThkMax[0];
		UpdateData(FALSE);
		Poly2 = RecalculateSpectrum(1);
		m_FThkMax[1] = Poly2 - 0.025f;
		UpdateData(FALSE);
		Poly2 = RecalculateSpectrum(1);
		m_Difference = Poly1 - Poly2;
		UpdateData(FALSE);

		m_FThkMin[2] = m_FThkMin[0];
		m_FThkMax[2] = m_FThkMax[0];
		UpdateData(FALSE);
		Poly3 = RecalculateSpectrum(2);

		m_FThkMin[2] = Poly3 + 0.025f;
		UpdateData(FALSE);
		Poly3 = RecalculateSpectrum(2);
		m_Difference2 = Poly3 - Poly1;
		UpdateData(FALSE);
	}
}

void CSetupProp42Dlg::DoCalculationC(CString filename) {
	if (p413App->OpenSpecCSV(filename, &ISpec1)) {
		COceanBase* pOcean = p413App->pOceanOp;
		if (!pOcean) return;
		CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];

		CSpec* pIn = &ISpec1;
		CSpec* pOut = &OSpec1;
		float Poly1, Poly2, Poly3;

		UpdateData(TRUE);

		pOcean->SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, g->TFScalingFactor);
		pOcean->ClearPolyThk(pIn);
		pOcean->SetActivatePoly(pIn, 0, m_FThkMin[0], m_FThkMax[0], m_RI[0]);
		pOcean->Result(pIn, pOut, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
		Poly1 = pOut->Poly[0].fThk;

		m_FThkMin[1] = m_FThkMin[0];
		m_FThkMax[1] = m_FThkMax[0];
		UpdateData(FALSE);

		DisplaySpectrum(0, SpecName, pOut);
		OSpec2.Dup(OSpec1);
		pOut = &OSpec2;

		pOcean->SetActivatePoly(pOut, 0, m_FThkMin[1], m_FThkMax[1], m_RI[1]);
		pOut->GetThicknessInRange(g->TFScalingFactor, pRcp->nCycle);

		m_FThkMax[1] = pOut->Poly[0].fThk - 0.025f;
		UpdateData(FALSE);

		pOcean->SetActivatePoly(pOut, 0, m_FThkMin[1], m_FThkMax[1], m_RI[1]);
		pOut->GetThicknessInRange(g->TFScalingFactor, pRcp->nCycle);
		Poly2 = pOut->Poly[0].fThk;
		m_Difference = Poly1 - Poly2;
		UpdateData(FALSE);

		m_FThkMin[2] = m_FThkMin[0];
		m_FThkMax[2] = m_FThkMax[0];
		UpdateData(FALSE);

		DisplaySpectrum(1, SpecName, pOut);
		OSpec3.Dup(OSpec1);
		pOut = &OSpec3;

		pOcean->SetActivatePoly(pOut, 0, m_FThkMin[2], m_FThkMax[2], m_RI[2]);
		pOut->GetThicknessInRange(g->TFScalingFactor, pRcp->nCycle);

		m_FThkMin[2] = pOut->Poly[0].fThk + 0.025f;
		UpdateData(FALSE);

		pOcean->SetActivatePoly(pOut, 0, m_FThkMin[2], m_FThkMax[2], m_RI[2]);
		pOut->GetThicknessInRange(g->TFScalingFactor, pRcp->nCycle);

		Poly3 = pOut->Poly[0].fThk;
		m_Difference2 = Poly3 - Poly1;
		UpdateData(FALSE);

		DisplaySpectrum(2, SpecName, pOut);
	}
}

void CSetupProp42Dlg::DoCalculationB(CString filename, BOOL bForce) {
	if (filename == "") {
		return;
	}
	if ((bakname == filename) && !bForce) {
		return;
	}
	if (p413App->OpenSpecCSV(filename, &ISpec1)) {
		bakname = filename;
		CString fname;

		fname = filename;
		DosUtil.ExtractFilename(fname);
		DosUtil.ExtractPath(filename);

		COceanBase* pOcean = p413App->pOceanOp;
		if (!pOcean) return;
		CMeParam* p = &pRcp->MeParam[g->CurMeSet - 1];

		CSpec* pIn = &ISpec1;
		CSpec* pOut = &OSpec1;
		float Poly0, Poly1, Poly2, Poly3;
		float /*GOF0,*/GOF1, GOF2, GOF3;
		float /*WHH0,*/WHH1, WHH2, WHH3;

		UpdateData(TRUE);

		TParam* pT = &p->Ta;

		pOcean->SetParam(pT->Method, 1.0f, pT->IntegTime, pT->Boxcar, g->TFScalingFactor);
		pOcean->ClearPolyThk(pIn);
		int n = 0;
		Poly0 = Poly1 = Poly2 = Poly3 = 0;
		for (int i = 0; i < MAXTHKVLU; i++) {
			if (pT->RI[i] > 0) pOcean->SetActivatePoly(pIn, n++, pT->ThkMin[i], pT->ThkMax[i], pT->RI[i]);
		}
		if (n > 0) {
			pOcean->Result(pIn, pOut, (CSpec::SPFILTER)pRcp->FFTFilter[g->CurMeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
			if (pT->RI[0]) {
				Poly1 = pOut->Poly[0].fThk;
				GOF1 = pOut->Poly[0].GOF;
				WHH1 = pOut->Poly[0].WHH;
			}
			if (pT->RI[1]) {
				Poly2 = pOut->Poly[1].fThk;
				GOF2 = pOut->Poly[1].GOF;
				WHH2 = pOut->Poly[1].WHH;
			}
			if (pT->RI[2]) {
				Poly3 = pOut->Poly[2].fThk;
				GOF3 = pOut->Poly[2].GOF;
				WHH3 = pOut->Poly[2].WHH;
			}
		}
		UpdateData(FALSE);

		DisplaySpectrum(0, SpecName, pOut);

		CString str;
		// Making decision [11/21/2012 FSM413]
		str.Format("1. %s Si: %.4f, GOF: %.3f , ILD: %.4f GOF: %.3f", fname, Poly1, GOF1, Poly2, GOF2);

		m_cMessage2.SetWindowText(filename);
		m_cMessage.SetWindowText(str);
	}
}

void CSetupProp42Dlg::OnKillfocusSmooth() {
	UpdateData(TRUE);
	if (m_Smooth < 1) {
		m_Smooth = 1;
		UpdateData(FALSE);
	}
	g->RcpSetup.NSmooth = m_Smooth;
	RecalculateSpectrum(CurNo);
}

void CSetupProp42Dlg::OnCyclefirst() {
	if (g->bCycleFirst) {
		g->bCycleFirst = FALSE;
	}
	else {
		g->bCycleFirst = TRUE;
	}
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnShowspec() {
	if (g->bShowSpec) {
		g->bShowSpec = FALSE;
	}
	else {
		g->bShowSpec = TRUE;
	}
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnKillfocusExpand() {
	UpdateData(TRUE);
	if (m_Expand < 1) {
		m_Expand = 1;
		UpdateData(FALSE);
	}
	g->RcpSetup.nExpand = m_Expand;
	DoCalculationB(bakname, TRUE);
}

void CSetupProp42Dlg::OnLoadref() {
	CFileDialog dlg(TRUE,
		"TXT",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Spectrum file|*.TXT|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP";

	if (dlg.DoModal() == IDOK) {
		CString path;
		path = dlg.GetPathName();
		DosUtil.ExtractPath(path);
		CSpec* pIn = &p413App->pOceanOp->BackGdSpectrum;
		filename = path + p413App->pOceanOp->FileDarkSpec;
		p413App->OpenSpecFile(filename, pIn);
		pIn->Smooth(pRcp->NSmooth);
		pIn = &p413App->pOceanOp->CalibrSpectrum;
		filename = path + p413App->pOceanOp->FileRefSpec;
		p413App->OpenSpecFile(filename, pIn);
		pIn->Smooth(pRcp->NSmooth);
		DoCalculationB(bakname, TRUE);
	}
}

void CSetupProp42Dlg::OnKillfocusMethod() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.Method = m_Method;
}

void CSetupProp42Dlg::OnKillfocusFthkminSp7() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[3] = m_FThkMin[3];
}

void CSetupProp42Dlg::OnKillfocusFthkminSp8() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[4] = m_FThkMin[4];
}

void CSetupProp42Dlg::OnKillfocusFthkminSp9() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMin[5] = m_FThkMin[5];
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp7() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[3] = m_FThkMax[3];
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp8() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[4] = m_FThkMax[4];
}

void CSetupProp42Dlg::OnKillfocusFthkmaxSp9() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.ThkMax[5] = m_FThkMax[5];
}

void CSetupProp42Dlg::OnKillfocusRi4() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[3] = m_RI[3];
}

void CSetupProp42Dlg::OnKillfocusRi5() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[4] = m_RI[4];
}

void CSetupProp42Dlg::OnKillfocusRi6() {
	UpdateData(TRUE);
	pRcp->MeParam[g->CurMeSet - 1].Ta.RI[5] = m_RI[5];
}

void CSetupProp42Dlg::OnSet1() {
	SwapMeSet(1);
}

void CSetupProp42Dlg::OnSet2() {
	SwapMeSet(2);
}

void CSetupProp42Dlg::OnSet3() {
	SwapMeSet(3);
}

void CSetupProp42Dlg::SwapMeSet(short next) {
	LocalToRecipe();
	g->CurMeSet = next;
	RecipeToLocal();
}