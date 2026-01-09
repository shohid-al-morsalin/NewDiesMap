// WaferViewOptions.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "WaferViewOptions.h"

#include "WaferMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaferViewOptions dialog

CWaferViewOptions::CWaferViewOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CWaferViewOptions::IDD, pParent) {
	//{{AFX_DATA_INIT(CWaferViewOptions)
	m_cDrawPattern = FALSE;
	m_bDrawPoint = FALSE;
	m_bDrawValues = FALSE;
	m_bDrawText = FALSE;
	//}}AFX_DATA_INIT
	pWaferMap = NULL;
}

void CWaferViewOptions::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaferViewOptions)
	DDX_Check(pDX, IDC_DRAWPATTERN, m_cDrawPattern);
	DDX_Check(pDX, IDC_DRAWPOINT, m_bDrawPoint);
	DDX_Check(pDX, IDC_DRAWVALUES, m_bDrawValues);
	DDX_Check(pDX, IDC_DRAWTEXT, m_bDrawText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWaferViewOptions, CDialog)
	//{{AFX_MSG_MAP(CWaferViewOptions)
	ON_BN_CLICKED(IDC_DRAWPATTERN, OnDrawpattern)
	ON_BN_CLICKED(IDC_DRAWPOINT, OnDrawpoint)
	ON_BN_CLICKED(IDC_DRAWVALUES, OnDrawvalues)
	ON_BN_CLICKED(IDC_DRAWTEXT, OnDrawtext)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaferViewOptions message handlers

BOOL CWaferViewOptions::OnInitDialog() {
	CDialog::OnInitDialog();

	if (!pWaferMap) {
		ASSERT(0);
		return FALSE;
	}

	m_bDrawPoint = pWaferMap->bDrawPoint;
	m_cDrawPattern = pWaferMap->bDrawPatt;
	m_bDrawValues = pWaferMap->bDrawValues;
	m_bDrawText = pWaferMap->bDrawText;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaferViewOptions::OnDrawpoint() {
	UpdateData(TRUE);
	pWaferMap->bDrawPoint = m_bDrawPoint;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnDrawpattern() {
	UpdateData(TRUE);
	pWaferMap->bDrawPatt = m_cDrawPattern;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnDrawvalues() {
	UpdateData(TRUE);
	pWaferMap->bDrawValues = m_bDrawValues;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnDrawtext() {
	UpdateData(TRUE);
	pWaferMap->bDrawText = m_bDrawText;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnRadio1() {
	pWaferMap->m_nImageType = 0;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnRadio2() {
	pWaferMap->m_nImageType = 1;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnRadio3() {
	pWaferMap->m_nImageType = 2;
	pWaferMap->Redraw();
}

void CWaferViewOptions::OnRadio4() {
	pWaferMap->m_nImageType = 3;
	pWaferMap->Redraw();
}