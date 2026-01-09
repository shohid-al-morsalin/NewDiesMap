#include "stdafx.h"
#include "E95.h"
#include "DefineGrid.h"
#include "MFC/KeyPad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDefineGrid::CDefineGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineGrid::IDD, pParent) {
	//{{AFX_DATA_INIT(CDefineGrid)
	m_lineX = 0.0f;
	m_lineY = 0.0f;
	m_DeletePrev = FALSE;
	m_UseMask = FALSE;
	//}}AFX_DATA_INIT
}

void CDefineGrid::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineGrid)
	DDX_Text(pDX, IDC_LINEX, m_lineX);
	DDX_Text(pDX, IDC_LINEY, m_lineY);
	DDV_MinMaxFloat(pDX, m_lineY, 0.0f, 125.f);
	DDX_Check(pDX, IDC_CHECK_DELETE_PREVIOUS, m_DeletePrev);
	DDX_Check(pDX, IDC_USEMASK, m_UseMask);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineGrid, CDialog)
	//{{AFX_MSG_MAP(CDefineGrid)
	ON_BN_CLICKED(IDC_LINESX, OnLinesX)
	ON_BN_CLICKED(IDC_LINESY, OnLinesY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineGrid message handlers

void CDefineGrid::OnLinesX() {
	// TODO: Add your control notification handler code here

	float val = m_lineX;
	if (TRUE == CKeyPad::Edit(val)) {
		m_lineX = val;
		CString strTemp;
		strTemp.Format(_T("%f"), m_lineX);
		GetDlgItem(IDC_LINEX)->SetWindowText(strTemp);
	}
}

void CDefineGrid::OnLinesY() {
	// TODO: Add your control notification handler code here

	float val = m_lineY;
	if (TRUE == CKeyPad::Edit(val)) {
		m_lineY = val;
		CString strTemp;
		strTemp.Format(_T("%f"), m_lineY);
		GetDlgItem(IDC_LINEY)->SetWindowText(strTemp);
	}
}

void CDefineGrid::OnOK() {
	// TODO: Add extra validation here
	UpdateData(TRUE);
	int LineX = int(m_lineX), LineY = int(m_lineY);
	if ((!(LineX % 2)) || (!(LineY % 2))) {
		AfxMessageBox("Only Odd points are allowed");
		return;
	}
	CDialog::OnOK();
}