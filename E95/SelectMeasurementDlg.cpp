// SelectMeasurementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SelectMeasurementDlg.h"
#include "MParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectMeasurementDlg dialog

CSelectMeasurementDlg::CSelectMeasurementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectMeasurementDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelectMeasurementDlg)
	m_fValOffset = 0.0f;	// added value offset [10/6/2020]
	m_MeSet = 0;
	m_PatSet = 0;
	m_ApplyAll = FALSE;
	m_Range = _T("");
	//}}AFX_DATA_INIT

	pMP = NULL;
	PtNo = 0;
	bClearPoint = FALSE;
	bDeletePoint = FALSE;
}

void CSelectMeasurementDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectMeasurementDlg)
	DDX_Text(pDX, IDC_MESET, m_MeSet);
	DDV_MinMaxInt(pDX, m_MeSet, 0, MAXMEPASET);
	DDX_Text(pDX, IDC_PATSET, m_PatSet);
	DDV_MinMaxInt(pDX, m_PatSet, 0, 9);
	DDX_Check(pDX, IDC_APPLYALL, m_ApplyAll);
	DDX_Text(pDX, IDC_EDIT1, m_Range);
	DDX_Text(pDX, IDC_VALUE_OFFSET, m_fValOffset);	// added value offset [10/6/2020]
	DDV_MinMaxFloat(pDX, m_fValOffset, -100.0f, 100.0f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectMeasurementDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectMeasurementDlg)
	ON_BN_CLICKED(IDC_DELETEPOINT, OnDeletepoint)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)

	//}}AFX_MSG_MAP
	ON_EN_KILLFOCUS(IDC_VALUE_OFFSET, &CSelectMeasurementDlg::OnKillfocusValueOffset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectMeasurementDlg message handlers

void CSelectMeasurementDlg::OnOK() {
	UpdateData(TRUE);
	if (pMP) {
		pMP->MeSet = m_MeSet;
		pMP->PatSet = m_PatSet;
		pMP->m_fvalOffset = m_fValOffset;	// added value offset [10/6/2020]
	}
	CDialog::OnOK();
}

BOOL CSelectMeasurementDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	bClearPoint = FALSE;
	bDeletePoint = FALSE;

	if (pMP) {
		m_MeSet = pMP->MeSet;
		m_PatSet = pMP->PatSet;
		m_fValOffset = pMP->m_fvalOffset;	// added value offset [10/6/2020]
	}

	CString str;
	str.Format("Select Measurement: Point # %d", PtNo);
	SetWindowText(str);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectMeasurementDlg::OnDeletepoint() {
	bDeletePoint = TRUE;
	CDialog::OnOK();
}

void CSelectMeasurementDlg::OnClear() {
	bClearPoint = TRUE;
	CDialog::OnOK();
}

void CSelectMeasurementDlg::OnKillfocusEdit1() {
	UpdateData(TRUE);
}

void CSelectMeasurementDlg::OnKillfocusValueOffset() {
	UpdateData(TRUE);
}
