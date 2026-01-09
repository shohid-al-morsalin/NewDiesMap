// EditWaferDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EditWaferDlg.h"

#include "..\MFC\keypad.h"
#include "..\MFC\keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWaferDlg dialog


CEditWaferDlg::CEditWaferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditWaferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditWaferDlg)
	m_ID = _T("");
	m_note = _T("");
//	m_stressconst = 0.0;
	//}}AFX_DATA_INIT
   m_wafernum = -1;
}

void CEditWaferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWaferDlg)
	DDX_Text(pDX, IDC_WAFERID, m_ID);
	DDX_Text(pDX, IDC_COMMENT, m_note);
//	DDX_Text(pDX, IDC_STRESSCONST, m_stressconst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditWaferDlg, CDialog)
	//{{AFX_MSG_MAP(CEditWaferDlg)
	ON_BN_CLICKED(IDC_WAFERIDBUT, OnWaferidbut)
	ON_BN_CLICKED(IDC_COMMENTBUT, OnCommentbut)
//	ON_BN_CLICKED(IDC_STRESSCONSTBUT, OnStressconstbut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditWaferDlg message handlers

void CEditWaferDlg::OnWaferidbut() 
{
	CKeyboard dlg;
	dlg.m_Value = m_ID;
	if (dlg.DoModal () == IDOK) {
		m_ID = dlg.m_Value;
		UpdateData (FALSE);
	}
}

BOOL CEditWaferDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_wafernum != -1) {
		CString msg;
		msg.Format ("Edit wafer %d", m_wafernum);
		SetWindowText (msg);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditWaferDlg::OnCommentbut() 
{
	CKeyboard dlg;
	dlg.m_Value = m_note;
	if (dlg.DoModal () == IDOK) {
		m_note = dlg.m_Value;
		UpdateData (FALSE);
	}
}

//void CEditWaferDlg::OnStressconstbut() 
//{
//	if (CKeyPad::Edit (m_stressconst) == TRUE) {
//		UpdateData (FALSE);
//	}
//}
