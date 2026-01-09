// EditColor.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EditColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditColor dialog


CEditColor::CEditColor(CWnd* pParent /*=NULL*/)
	: CDialog(CEditColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditColor)
	m_Blue = 0.0f;
	m_Green = 0.0f;
	m_Red = 0.0f;
	//}}AFX_DATA_INIT
}


void CEditColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditColor)
	DDX_Text(pDX, IDC_BLUE, m_Blue);
	DDV_MinMaxFloat(pDX, m_Blue, 0.f, 1.f);
	DDX_Text(pDX, IDC_GREEN, m_Green);
	DDV_MinMaxFloat(pDX, m_Green, 0.f, 1.f);
	DDX_Text(pDX, IDC_RED, m_Red);
	DDV_MinMaxFloat(pDX, m_Red, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditColor, CDialog)
	//{{AFX_MSG_MAP(CEditColor)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditColor message handlers
