// EditLegendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeTextDlg.h"
#include "EditLegendDlg.h"
#include "..\Plt\CDataList.h"
#include "..\Plt\CLnData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLegendDlg dialog


CEditLegendDlg::CEditLegendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditLegendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditLegendDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pP = NULL;
}


void CEditLegendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditLegendDlg)
	DDX_Control(pDX, IDC_LEGEND, m_legend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditLegendDlg, CDialog)
	//{{AFX_MSG_MAP(CEditLegendDlg)
	ON_LBN_DBLCLK(IDC_LEGEND, OnDblclkLegend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditLegendDlg message handlers

void CEditLegendDlg::OnDblclkLegend() 
{
	int sel;
	CString str;

	sel = m_legend.GetCurSel();
	m_legend.GetText(sel, str);

	CChangeTextDlg dlg;
	dlg.m_from = str;
	if (dlg.DoModal() == IDOK) {
		m_legend.DeleteString(sel);
		m_legend.InsertString(sel, dlg.m_to);
	}
}

BOOL CEditLegendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	for (int i = 0; i < MAXPLOTABLELINE; i++) lut[i] = -1;
	
	short inc = 0;
	i = 0;
	if (pP)
	{
		POSITION pos = pP->Ln.GetHeadPosition();
		while(pos) 
		{
			CLnData *p = (CLnData *)pP->Ln.GetNext(pos);
			if (p) 
			{
				if (p->ID.GetLength()) 
				{
					m_legend.AddString(p->ID);
					lut[inc] = i;
					i++;
				}
				inc++;
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditLegendDlg::OnOK() 
{
	CString str;
	int i = 0, inc = 0;
	
	if (pP)
	{
		POSITION pos = pP->Ln.GetHeadPosition();
		while(pos) 
		{
			CLnData *p = (CLnData *)pP->Ln.GetNext(pos);
			if (p) 
			{
				if (lut[inc] != -1)
				{
					m_legend.GetText(i, str);
					p->ID = str;
					i++;
				}
				inc++;
			}
		}
	}
#if 0	
	for (int i = 0; i < m_legend.GetCount(); i++) {
		m_legend.GetText(i, str);
		pP->dat[lut[i]].legend = str;
	}
#endif
	CDialog::OnOK();
}
