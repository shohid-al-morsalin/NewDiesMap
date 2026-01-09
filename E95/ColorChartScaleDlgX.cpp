// ColorChartScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ColorChartScaleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorChartScaleDlg dialog


CColorChartScaleDlg::CColorChartScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorChartScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorChartScaleDlg)
	m_maxValue = 0.0f;
	m_minValue = 0.0f;
	//}}AFX_DATA_INIT
}


void CColorChartScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorChartScaleDlg)
	DDX_Text(pDX, IDC_UPPERLIMIT, m_maxValue);
	DDX_Text(pDX, IDC_LOWERLIMIT, m_minValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorChartScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CColorChartScaleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorChartScaleDlg message handlers
