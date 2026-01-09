// EditStressConstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "..\MFC\Keypad.h"
#include "EditStressConstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditStressConstDlg dialog


CEditStressConstDlg::CEditStressConstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditStressConstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditStressConstDlg)
	m_stressconst = 0.0;
	//}}AFX_DATA_INIT
	wafernum = 0;
}


void CEditStressConstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditStressConstDlg)
	DDX_Control(pDX, IDC_STRESSTABLE, m_stresstable);
	DDX_Text(pDX, IDC_STRESSCONST, m_stressconst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditStressConstDlg, CDialog)
	//{{AFX_MSG_MAP(CEditStressConstDlg)
	ON_BN_CLICKED(IDC_STRESSCONSTBUT, OnStressconstbut)
	ON_CBN_SELCHANGE(IDC_STRESSTABLE, OnSelchangeStresstable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditStressConstDlg message handlers

void CEditStressConstDlg::OnOK() 
{
	CDialog::OnOK();
}

void CEditStressConstDlg::OnStressconstbut() 
{
   if (CKeyPad::Edit (m_stressconst) == TRUE) {
      UpdateData (FALSE);
   }
}

void CEditStressConstDlg::OnSelchangeStresstable() 
{
	UpdateData (TRUE);
	CString str;
	m_stresstable.GetLBText(m_stresstable.GetCurSel(), str) ;
	CString str2;
	str2 = str.Right(str.GetLength()-str.Find('>')-2);
	m_stressconst = atof (str2.GetBuffer(16));
	UpdateData (FALSE);
}

BOOL CEditStressConstDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString str;
	str.Format("Edit stress constant for wafer number %d", wafernum);
	SetWindowText(str);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
