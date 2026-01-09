// CalcBeamPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CalcBeamPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalcBeamPathDlg dialog


CCalcBeamPathDlg::CCalcBeamPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalcBeamPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalcBeamPathDlg)
	m_Radius = 20.0f;
	m_Wafer = 2;
	//}}AFX_DATA_INIT
}


void CCalcBeamPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalcBeamPathDlg)
	DDX_Text(pDX, ACTUAL_READIUS, m_Radius);
	DDX_Text(pDX, IDC_WAFER, m_Wafer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalcBeamPathDlg, CDialog)
	//{{AFX_MSG_MAP(CCalcBeamPathDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalcBeamPathDlg message handlers
