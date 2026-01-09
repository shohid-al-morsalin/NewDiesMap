// GetNumber.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "GetNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetNumber dialog


CGetNumber::CGetNumber(CWnd* pParent /*=NULL*/)
	: CDialog(CGetNumber::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetNumber)
	m_Prompt = _T("");
	m_Value = 0.0f;
	//}}AFX_DATA_INIT
}


void CGetNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetNumber)
	DDX_Text(pDX, IDC_PROMPT, m_Prompt);
	DDX_Text(pDX, IDC_EDIT1, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetNumber, CDialog)
	//{{AFX_MSG_MAP(CGetNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetNumber message handlers

BOOL CGetNumber::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetNumber::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}
