// LocationProp.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "LocationProp.h"
#include "SelectDlg.h"
#include "CWaferParam.h"
#include "Recipe413.h"
#include "DefGrid2Dlg.h"
#include "DefineCircular.h"
#include "LocDefineDice.h"
#include "..\SRC\DOSUtil.h"
#include "SetupProp14Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocationProp dialog

CLocationProp::CLocationProp(CWnd* pParent /*=NULL*/)
	: CDialog(CLocationProp::IDD, pParent) {
	//{{AFX_DATA_INIT(CLocationProp)
	m_ZPos = 0.0f;
	m_BaselineFile = _T("");
	//}}AFX_DATA_INIT

	pRcp = NULL;
	SelPat = 0;
	hWnd = NULL;
}

void CLocationProp::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocationProp)
	DDX_Control(pDX, IDC_BASELINEFILE, m_cBaselineFile);
	DDX_Control(pDX, IDC_MASKFILENAME, m_cMaskFilename);
	DDX_Control(pDX, IDC_WAFERSIZE, m_cWaferSize);
	DDX_Control(pDX, IDC_MAPSIZE, m_cMapSize);
	DDX_Text(pDX, IDC_ZPOS, m_ZPos);
	DDX_Text(pDX, IDC_BASELINEFILE, m_BaselineFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLocationProp, CDialog)
	//{{AFX_MSG_MAP(CLocationProp)
	ON_BN_CLICKED(IDC_BUTTON_MAP_SIZE, OnButtonMapSize)
	ON_BN_CLICKED(IDC_BUTTON_WAFER_SZ, OnButtonWaferSz)
	ON_BN_CLICKED(IDC_DEFINE, OnDefine)
	ON_BN_CLICKED(IDC_DEFINE2, OnDefine2)
	ON_BN_CLICKED(IDC_DEFINE3, OnDefine3)
	ON_BN_CLICKED(IDC_DEFINE4, OnDefine4)
	ON_EN_KILLFOCUS(IDC_BASELINEFILE, OnKillfocusBaselinefile)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDOK, &CLocationProp::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocationProp message handlers

BOOL CLocationProp::OnInitDialog() {
	CDialog::OnInitDialog();

	SelPat = 0;
	RefreshDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLocationProp::OnButtonMapSize() {
	SelectCombo(&m_cMapSize);
}

void CLocationProp::OnButtonWaferSz() {
	SelectCombo(&m_cWaferSize);
}

BOOL CLocationProp::SelectCombo(CComboBox* pBox) {
	CSelectDlg dlg;
	dlg.m_nSel = pBox->GetCurSel();
	for (int i = 0; i < pBox->GetCount(); i++) {
		CString str;
		pBox->GetLBText(i, str);
		dlg.m_saItemList.Add(str);
	}
	if (dlg.DoModal() != IDOK) return FALSE;
	pBox->SetCurSel(dlg.m_nSel);
	return TRUE;
}

void CLocationProp::OnDefine() {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	if (pRcp->gridpatt.SetUp(this)) {
		UpdateData(TRUE);
		SelPat = 1;
		if (pRcp->gridpatt.bClearData) {
			if (pRcp->gridpatt.pPatt) {
				pRcp->gridpatt.pPatt->ClearData();
			}
		}

		CString str;
		m_cMapSize.GetWindowText(str);
		float fRadius = (float)atof(str);
		m_cWaferSize.GetWindowText(str);
		fRadius *= (float)atof(str) / 100.f;
		pRcp->gridpatt.Generate(fRadius);
	}
}

BOOL CLocationProp::DestroyWindow() {
	return CDialog::DestroyWindow();
}

void CLocationProp::OnDefine2() {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	CDefGrid2Dlg dlg;
	if (dlg.DoModal() == IDOK) {
		SelPat = 2;
		if (dlg.m_NX % 2 == 0) dlg.m_NX++;
		if (dlg.m_NY % 2 == 0) dlg.m_NY++;
		pRcp->gridpatt.ngridX = dlg.m_NX;
		pRcp->gridpatt.ngridY = dlg.m_NY;
		pRcp->gridpatt.bClearData = dlg.m_DeletePrev;

		CString str;
		m_cMapSize.GetWindowText(str);
		float fRadius = (float)atof(str);
		m_cWaferSize.GetWindowText(str);
		fRadius *= (float)atof(str) / 100.f;
		if (dlg.m_DeletePrev) {
			if (pRcp->gridpatt.pPatt) {
				pRcp->gridpatt.pPatt->Clear();
			}
		}
		pRcp->gridpatt.Generate2(dlg.m_NX, dlg.m_NY, dlg.m_fX0, dlg.m_fY0, dlg.m_fSx, dlg.m_fSy, fRadius, dlg.m_bIgnore);
	}
}

void CLocationProp::OnDefine3() {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	if (pRcp->ccrpatt.pPatt) {
		pRcp->ccrpatt.pPatt->bUseMask = FALSE;
	}
	if (pRcp->ccrpatt.SetUp(this)) {
		SelPat = 3;
		pRcp->ccrpatt.Generate(0);
	}
}

void CLocationProp::RefreshDialog() {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	CString str, str2;
	m_cMaskFilename.SetWindowText(pRcp->m_strMaskFileName);
	m_BaselineFile = pRcp->BaseLineFile;

	str.Format("%d", pRcp->Wp.size);
	int i;
	BOOL bSet = FALSE;
	for (i = 0; i < m_cWaferSize.GetCount(); i++) {
		m_cWaferSize.GetLBText(i, str2);
		if (str.Compare(str2) == 0) {	// 01242024 ZHIMING Find => Compare
			bSet = TRUE;
			m_cWaferSize.SetCurSel(i);
			break;
		}
	}
	if (!bSet) {
		m_cWaferSize.SetWindowText(str);
	}

	// [01242024 ZHIMING
	// str.Format("%d", 100 * pRcp->Wp.mapsize / pRcp->Wp.size);
	str.Format("%d", pRcp->Wp.percentage);
	// ]
	for (i = 0; i < m_cMapSize.GetCount(); i++) {
		m_cMapSize.GetLBText(i, str2);
		if (str.Compare(str2) == 0) {	// 01242024 ZHIMING Find => Compare
			bSet = TRUE;
			m_cMapSize.SetCurSel(i);
		}
	}
	if (!bSet) {
		m_cMapSize.SetWindowText(str);
	}

	m_ZPos = pRcp->fEchoTopZ;

	UpdateData(FALSE);
}

void CLocationProp::OnOK() {
	if (!pRcp) {
		ASSERT(0);
		CDialog::OnOK();
		return;
	}
	if (SelPat == 0) {
		if (AfxMessageBox("No Grid define, continue?", MB_YESNO) == IDNO) {
			return;
		}
	}

	CString str;
	CWaferParam* pParam = pRcp->GetWp();
	if (pParam) {
		m_cWaferSize.GetLBText(m_cWaferSize.GetCurSel(), str);
		pParam->size = (short)atoi(str);

		m_cMapSize.GetLBText(m_cMapSize.GetCurSel(), str);
		pParam->mapsize = (short)((atoi(str) * pParam->size) / 100);

		// [01242024 ZHIMING
		pParam->percentage = atoi(str);
		// ]
	}

	pRcp->ElliminateOutOfChuck();	// 04222022 ZHIMING

	UpdateData(TRUE);

	pRcp->fEchoTopZ = m_ZPos;

	m_cMaskFilename.GetWindowText(pRcp->m_strMaskFileName);
	pRcp->BaseLineFile = m_BaselineFile;
	
	if (hWnd) {
		::PostMessage(hWnd, WM_COMMAND, ID_REFRESHUI, NULL);
	}
	CDialog::OnOK();
}

void CLocationProp::OnDefine4() {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	if (pRcp->ccrpatt.SetUpSeg(this)) {
		SelPat = 3;
		pRcp->ccrpatt.Generate(0);
	}
}

// CLocDefineDice *pDefDice=NULL;
// void CLocationProp::OnDefine5()
// {
// 	if (!pRcp)
// 	{
// 		ASSERT(0);
// 		return ;
// 	}
//
// 	if (pDefDice)
// 	{
// 		delete pDefDice;
// 		pDefDice = NULL;
// 	}
// 	pDefDice = new CLocDefineDice;
// 	if (!pDefDice)
// 	{
// 		return ;
// 	}
// 	pDefDice->pRcp = pRcp;
// 	pDefDice->Create(CLocDefineDice::IDD);
// 	pDefDice->ShowWindow(SW_SHOW);
//
// 	SelPat = 5;
// }

void CLocationProp::OnKillfocusBaselinefile() {
	UpdateData(TRUE);
}

void CLocationProp::OnBrowse() {
	CFileDialog dlg(TRUE,
		"BLN",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Baseline files|*.BLN|");
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "BASELINE";
	dlg.m_ofn.lpstrInitialDir = mdir.GetBuffer(0);
	if (dlg.DoModal() == IDOK) {
		m_BaselineFile = dlg.GetFileName();
		UpdateData(FALSE);
	}
}

//void CLocationProp::OnBnClickedOk()
//{
//	// TODO: Add your control notification handler code here
//	CDialog::OnOK();
//}
