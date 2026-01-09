// PeakControlItem.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "PeakControlItem.h"
#include "PeakShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPeakControlItem dialog

CPeakControlItem::CPeakControlItem(CWnd* pParent /*=NULL*/)
	: CDialog(CPeakControlItem::IDD, pParent) {
	//{{AFX_DATA_INIT(CPeakControlItem)
	m_Width = 0.0f;
	m_Method = 0;
	m_Nearest = 0;
	m_Thres = 0;
	m_Height = 0;
	m_Distance = 0.0f;
	m_DeltaDistance = 0.0f;
	m_Active = FALSE;
	//}}AFX_DATA_INIT

	PS = NULL;
	PkNo = 0;
}

void CPeakControlItem::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPeakControlItem)
	DDX_Control(pDX, IDC_PEAKNUMBER, m_PeakNumber);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDX_Text(pDX, IDC_METHOD, m_Method);
	DDX_Text(pDX, IDC_NEAREST, m_Nearest);
	DDX_Text(pDX, IDC_THRESHOLD, m_Thres);
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_DISTANCE2, m_DeltaDistance);
	DDX_Check(pDX, IDC_ACTIVE, m_Active);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPeakControlItem, CDialog)
	//{{AFX_MSG_MAP(CPeakControlItem)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_THRESHOLD, OnKillfocusThreshold)
	ON_EN_KILLFOCUS(IDC_NEAREST, OnKillfocusNearest)
	ON_EN_KILLFOCUS(IDC_METHOD, OnKillfocusMethod)
	ON_BN_CLICKED(IDC_ACTIVE, OnActive)
	ON_EN_KILLFOCUS(IDC_DISTANCE, OnKillfocusDistance)
	ON_EN_KILLFOCUS(IDC_DISTANCE2, OnKillfocusDistance2)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeakControlItem message handlers

BOOL CPeakControlItem::OnInitDialog() {
	CDialog::OnInitDialog();

	if (!PS) {
		ASSERT(0);
		return FALSE;
	}

	CString str;

	str.Format("Peak # %d", PkNo);
	m_PeakNumber.SetWindowText(str.GetBuffer(0));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPeakControlItem::OnKillfocusWidth() {
	UpdateData(TRUE);
}

void CPeakControlItem::OnKillfocusThreshold() {
	UpdateData(TRUE);
}

void CPeakControlItem::OnKillfocusNearest() {
	UpdateData(TRUE);
}

void CPeakControlItem::OnKillfocusMethod() {
	UpdateData(TRUE);
}

void CPeakControlItem::OnActive() {
	UpdateData(TRUE);
}

void CPeakControlItem::OnKillfocusDistance() {
	UpdateData(TRUE);
	m_Active = TRUE;
	UpdateData(FALSE);
}

void CPeakControlItem::OnKillfocusDistance2() {
	UpdateData(TRUE);
	m_Active = TRUE;
	UpdateData(FALSE);
}

void CPeakControlItem::OnKillfocusHeight() {
	UpdateData(TRUE);
	m_Active = TRUE;
	UpdateData(FALSE);
}