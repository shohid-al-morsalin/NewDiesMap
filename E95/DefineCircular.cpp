#include "stdafx.h"
#include "E95.h"
#include "DefineCircular.h"
#include "MFC/KeyPad.h"
#include "CRing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDefineCircular::CDefineCircular(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineCircular::IDD, pParent) {
	//{{AFX_DATA_INIT(CDefineCircular)
	m_angle = 0.0f;
	m_distance = 0.0f;
	m_npoint = 0;
	//}}AFX_DATA_INIT
}

CDefineCircular::~CDefineCircular() {
	Clear();
}

void CDefineCircular::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineCircular)
	DDX_Control(pDX, IDC_LIST, m_clist);
	DDX_Text(pDX, IDC_ANGLE, m_angle);
	DDX_Text(pDX, IDC_DISTANCE, m_distance);
	DDX_Text(pDX, IDC_POINT, m_npoint);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineCircular, CDialog)
	//{{AFX_MSG_MAP(CDefineCircular)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_CIRC_C_DIST, OnCircCDist)
	ON_BN_CLICKED(IDC_CIRC_ST_ANGLE, OnCircStAngle)
	ON_BN_CLICKED(IDC_CIRC_NUM_OF_POINTS, OnCircNumOfPoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineCircular message handlers

void CDefineCircular::Clear() {
	POSITION pos = rings.GetHeadPosition();
	while (pos) {
		CRing* p = (CRing*)rings.GetNext(pos);
		if (p) delete p;
	}
	rings.RemoveAll();
}

void CDefineCircular::OnAdd() {
	UpdateData(TRUE);
	CString str;
	int i = m_clist.GetItemCount();
	str.Format("%.2f     ", m_distance);
	int idx = m_clist.InsertItem(i, str);
	str.Format("%d", m_npoint);
	m_clist.SetItemText(idx, 1, str);
	str.Format("%.2f", m_angle);
	m_clist.SetItemText(idx, 2, str);
}

void CDefineCircular::OnDelete() {
	POSITION pos = m_clist.GetFirstSelectedItemPosition();
	if (!pos) return;
	int nItem = m_clist.GetNextSelectedItem(pos);
	m_clist.DeleteItem(nItem);
}

void CDefineCircular::OnOK() {
	CString str;
	Clear();
	int cnt = m_clist.GetItemCount();
	for (int i = 0; i < cnt; i++) {
		CRing* p = new CRing;
		if (!p) return;
		str = m_clist.GetItemText(i, 0);
		p->radius = float(atof(str));
		str = m_clist.GetItemText(i, 1);
		p->npoint = (short)atoi(str);
		str = m_clist.GetItemText(i, 2);
		p->angle = float(atof(str));
		p->radius2 = 0;
		rings.AddTail(p);
	}
	CDialog::OnOK();
}

BOOL CDefineCircular::OnInitDialog() {
	CDialog::OnInitDialog();

	LVCOLUMN pColumn;
	pColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	pColumn.cx = 10 * 7;
	pColumn.pszText = "Distance";
	m_clist.InsertColumn(0, &pColumn);
	pColumn.cx = 8 * 7;
	pColumn.pszText = "Points";
	m_clist.InsertColumn(1, &pColumn);
	pColumn.cx = 12 * 7;
	pColumn.pszText = "Angle (deg)";
	m_clist.InsertColumn(2, &pColumn);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDefineCircular::OnClear() {
	m_clist.DeleteAllItems();
}

void CDefineCircular::OnCircCDist() {
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_distance)) {
		CString str;
		str.Format(_T("%f"), m_distance);
		GetDlgItem(IDC_DISTANCE)->Invalidate(TRUE);
		GetDlgItem(IDC_DISTANCE)->SetWindowText(str);
		GetDlgItem(IDC_DISTANCE)->Invalidate(TRUE);
	}
}

void CDefineCircular::OnCircStAngle() {
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_angle)) {
		CString str;
		str.Format(_T("%f"), m_angle);
		GetDlgItem(IDC_ANGLE)->Invalidate(TRUE);
		GetDlgItem(IDC_ANGLE)->SetWindowText(str);
		GetDlgItem(IDC_ANGLE)->Invalidate(TRUE);
	}
}

void CDefineCircular::OnCircNumOfPoints() {
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_npoint)) {
		CString str;
		str.Format(_T("%d"), m_npoint);
		GetDlgItem(IDC_POINT)->Invalidate(TRUE);
		CSpinButtonCtrl* pCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPINPOINTS);
		pCtrl->SetPos(m_npoint - 1);
		GetDlgItem(IDC_POINT)->SetWindowText(str);
		GetDlgItem(IDC_POINT)->Invalidate(TRUE);
	}
}

void CDefineCircular::Generate() {}