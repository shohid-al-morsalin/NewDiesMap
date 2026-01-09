// ExportChart.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ExportChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportChart dialog


CExportChart::CExportChart(CWnd* pParent /*=NULL*/)
	: CDialog(CExportChart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportChart)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	extype = EXPORTNONE;
}


void CExportChart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportChart)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportChart, CDialog)
	//{{AFX_MSG_MAP(CExportChart)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_TABLE, OnTable)
	ON_BN_CLICKED(IDC_BITMAP, OnBitmap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportChart message handlers

void CExportChart::OnExport() 
{
	extype = EXPORTNORMAL;
	OnOK();
}

void CExportChart::OnTable() 
{
	extype = EXPORTTABLE;
	OnOK();
}

void CExportChart::OnBitmap() 
{
	extype = EXPORTIMAGE;
	OnOK();
}
