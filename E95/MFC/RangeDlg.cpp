// RangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Keypad.h"
#include "RangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRangeDlg dialog


CRangeDlg::CRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRangeDlg)
	m_max = 0.0;
	m_min = 0.0;
	m_div = 0;
	//}}AFX_DATA_INIT

	ID = 0;
	lock = 0;
}


void CRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRangeDlg)
	DDX_Text(pDX, IDC_MAXIMUM, m_max);
	DDX_Text(pDX, IDC_MINIMUM, m_min);
	DDX_Text(pDX, IDC_DIVISION, m_div);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRangeDlg, CDialog)
	//{{AFX_MSG_MAP(CRangeDlg)
	ON_BN_CLICKED(IDC_MAXIMUMBUT, OnMaximumbut)
	ON_BN_CLICKED(IDC_MINIMUMBUT, OnMinimumbut)
	ON_BN_CLICKED(IDC_DIVISIONBUT, OnDivisionbut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRangeDlg message handlers

void CRangeDlg::OnMaximumbut() 
{
	if (CKeyPad::Edit (m_max) == TRUE)
	{
		UpdateData (FALSE);
	}
}

void CRangeDlg::OnMinimumbut() 
{
	if (CKeyPad::Edit (m_min) == TRUE)
	{
		UpdateData (FALSE);
	}
}

void CRangeDlg::OnDivisionbut() 
{
	if (CKeyPad::Edit (m_div) == TRUE)
	{
		UpdateData (FALSE);
	}
}

void CRangeDlg::OnOK() 
{
	UpdateData(TRUE);
	if (ID == 1)
	{
		if (bk_max != m_max) { lock |= 0x01; }
		if (bk_min != m_min) { lock |= 0x01; }
	}
	if (ID == 2)
	{
		if (bk_max != m_max) { lock |= 0x02; }
		if (bk_min != m_min) { lock |= 0x02; }
	}
	
	CDialog::OnOK();
}

BOOL CRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	bk_max = m_max;
	bk_min = m_min;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
