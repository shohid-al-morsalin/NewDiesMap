// EditThickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "..\MFC\keypad.h"
#include "EditThickDlg.h"

#include "..\Sys\Var.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditThickDlg dialog

CEditThickDlg::CEditThickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditThickDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CEditThickDlg)
	m_film = 0.0;
	m_wafer = 0.0;
	m_stressconst = 0.0;
	//}}AFX_DATA_INIT
	Title = "";
	bNoWaferThk = FALSE;
	m_mode = ScanParam::RTMODE;
}

void CEditThickDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditThickDlg)
	DDX_Control(pDX, IDC_WAFERTHICKNESS, m_cWaferThk);
	DDX_Control(pDX, IDC_WAFERTHICKNESSBUT, m_cWaferThkBut);
	DDX_Control(pDX, IDC_FTUNIT, m_cftunit);
	DDX_Text(pDX, IDC_FILMTHICKNESS, m_film);
	DDX_Text(pDX, IDC_WAFERTHICKNESS, m_wafer);
	DDX_Text(pDX, IDC_STRESSCONST, m_stressconst);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditThickDlg, CDialog)
	//{{AFX_MSG_MAP(CEditThickDlg)
	ON_BN_CLICKED(IDC_FILMTHICKNESSBUT, OnFilmthicknessbut)
	ON_BN_CLICKED(IDC_WAFERTHICKNESSBUT, OnWaferthicknessbut)
	ON_BN_CLICKED(IDC_HEAT, OnHeat)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditThickDlg message handlers

void CEditThickDlg::OnFilmthicknessbut() {
	// TODO: Add your control notification handler code here
/* 	CKeyPad dlg;
   dlg.m_Value.Format ("%f", m_film);
   if (dlg.DoModal () == IDOK) {
	  m_film = atof(dlg.m_Value);
	  UpdateData (FALSE);
   }
*/
	if (CKeyPad::Edit(m_film) == TRUE) {
		UpdateData(FALSE);
	}
}

void CEditThickDlg::OnWaferthicknessbut() {
	// TODO: Add your control notification handler code here
/* 	CKeyPad dlg;
   dlg.m_Value.Format ("%f", m_wafer);
   if (dlg.DoModal () == IDOK) {
	  m_wafer = atof(dlg.m_Value);
	  UpdateData (FALSE);
   }
*/
	if (CKeyPad::Edit(m_wafer) == TRUE) {
		UpdateData(FALSE);
	}
}

void CEditThickDlg::OnHeat() {
	m_mode = ScanParam::HTMODE;
}

void CEditThickDlg::OnNormal() {
	m_mode = ScanParam::RTMODE;
}

BOOL CEditThickDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	if (Title.GetLength()) {
		SetWindowText(Title);
	}
	m_cftunit.SetWindowText(Var.FilmThicknessUnit);

	((CButton*)GetDlgItem(IDC_NORMAL))->SetCheck(TRUE);

	if (bNoWaferThk) {
		m_cWaferThk.EnableWindow(FALSE);
		m_cWaferThkBut.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditThickDlg::OnOK() {
	UpdateData(TRUE);
	m_film /= Var.FilmThicknessFactor;
	UpdateData(FALSE);
	CDialog::OnOK();
}