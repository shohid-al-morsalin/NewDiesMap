// SelectResol.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SelectResol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectResol dialog

CSelectResol::CSelectResol(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectResol::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelectResol)
	m_Height = 0;
	m_Width = 0;
	//}}AFX_DATA_INIT
}

void CSelectResol::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectResol)
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectResol, CDialog)
	//{{AFX_MSG_MAP(CSelectResol)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectResol message handlers

void CSelectResol::OnRadio1() {
	m_Width = 4096;
	m_Height = m_Width * 8 / 9;
	UpdateData(FALSE);
}

void CSelectResol::OnRadio2() {
	m_Width = 3072;
	m_Height = m_Width * 8 / 9;
	UpdateData(FALSE);
}

void CSelectResol::OnRadio3() {
	m_Width = 2048;
	m_Height = m_Width * 8 / 9;
	UpdateData(FALSE);
}

void CSelectResol::OnRadio4() {
	m_Width = 1024;
	m_Height = m_Width * 8 / 9;
	UpdateData(FALSE);
}