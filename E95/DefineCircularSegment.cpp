#include "stdafx.h"
#include "e95.h"
#include "DefineCircularSegment.h"
#include "MFC/KeyPad.h"
#include "CRing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDefineCircularSegment::CDefineCircularSegment(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineCircularSegment::IDD, pParent) {
	//{{AFX_DATA_INIT(CDefineCircularSegment)
	m_angle = 0.0f;
	m_distance = 0.0f;
	m_npoint = 0;
	m_distance2 = 0.0f;
	//}}AFX_DATA_INIT
}

void CDefineCircularSegment::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineCircularSegment)
	DDX_Control(pDX, IDC_LIST, m_clist);
	DDX_Text(pDX, IDC_ANGLE, m_angle);
	DDX_Text(pDX, IDC_DISTANCE, m_distance);
	DDX_Text(pDX, IDC_POINT, m_npoint);
	DDX_Text(pDX, IDC_DISTANCE2, m_distance2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineCircularSegment, CDialog)
	//{{AFX_MSG_MAP(CDefineCircularSegment)
	ON_BN_CLICKED(IDC_CIRC_C_DIST, OnCircCDist)
	ON_BN_CLICKED(IDC_CIRC_ST_ANGLE, OnCircStAngle)
	ON_BN_CLICKED(IDC_CIRC_NUM_OF_POINTS, OnCircNumOfPoints)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_CIRC_C_DIST2, OnCircCDist2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineCircularSegment message handlers

void CDefineCircularSegment::Clear() {
	POSITION pos = rings.GetHeadPosition();
	while (pos) {
		CRing* p = (CRing*)rings.GetNext(pos);
		if (p) delete p;
	}
	rings.RemoveAll();
}

void CDefineCircularSegment::OnAdd() {
	UpdateData(TRUE);
	CString str;
	int i = m_clist.GetItemCount();
	str.Format("%.3f     ", m_distance);
	int idx = m_clist.InsertItem(i, str);
	str.Format("%.3f", m_distance2);
	m_clist.SetItemText(idx, 1, str);
	str.Format("%d", m_npoint);
	m_clist.SetItemText(idx, 2, str);
	str.Format("%.2f", m_angle);
	m_clist.SetItemText(idx, 3, str);
}

void CDefineCircularSegment::OnDelete() {
	POSITION pos = m_clist.GetFirstSelectedItemPosition();
	if (!pos) return;
	int nItem = m_clist.GetNextSelectedItem(pos);
	m_clist.DeleteItem(nItem);
}

void CDefineCircularSegment::OnOK() {
	CString str;
	Clear();
	int cnt = m_clist.GetItemCount();
	for (int i = 0; i < cnt; i++) {
		CRing* p = new CRing;
		if (!p) return;
		int n = 1;
		str = m_clist.GetItemText(i, 0);
		p->radius = float(atof(str));
		str = m_clist.GetItemText(i, n++);
		p->radius2 = float(atof(str));
		str = m_clist.GetItemText(i, n++);
		p->npoint = (short)atoi(str);
		str = m_clist.GetItemText(i, n++);
		p->angle = float(atof(str));
		rings.AddTail(p);
	}
	CDialog::OnOK();
}

BOOL CDefineCircularSegment::OnInitDialog() {
	CDialog::OnInitDialog();

	int n = 0;
	LVCOLUMN pColumn;
	pColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	pColumn.cx = 9 * 7;
	pColumn.pszText = "Start Dist";
	m_clist.InsertColumn(n++, &pColumn);
	pColumn.cx = 9 * 7;
	pColumn.pszText = "End Dist";
	m_clist.InsertColumn(n++, &pColumn);
	pColumn.cx = 8 * 7;
	pColumn.pszText = "Points";
	m_clist.InsertColumn(n++, &pColumn);
	pColumn.cx = 10 * 7;
	pColumn.pszText = "Angle (deg)";
	m_clist.InsertColumn(n++, &pColumn);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDefineCircularSegment::OnClear() {
	m_clist.DeleteAllItems();
}

void CDefineCircularSegment::OnCircCDist() {
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_distance)) {
		CString str;
		str.Format(_T("%f"), m_distance);
		//		GetDlgItem(IDC_DISTANCE)->Invalidate(TRUE);
		GetDlgItem(IDC_DISTANCE)->SetWindowText(str);
		//		GetDlgItem(IDC_DISTANCE)->Invalidate(TRUE);
	}
}

void CDefineCircularSegment::OnCircCDist2() {
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_distance2)) {
		CString str;
		str.Format(_T("%f"), m_distance2);
		//		GetDlgItem(IDC_DISTANCE2)->Invalidate(TRUE);
		GetDlgItem(IDC_DISTANCE2)->SetWindowText(str);
		//		GetDlgItem(IDC_DISTANCE2)->Invalidate(TRUE);
	}
}

void CDefineCircularSegment::OnCircStAngle() {
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_angle)) {
		CString str;
		str.Format(_T("%f"), m_angle);
		//		GetDlgItem(IDC_ANGLE)->Invalidate(TRUE);
		GetDlgItem(IDC_ANGLE)->SetWindowText(str);
		//		GetDlgItem(IDC_ANGLE)->Invalidate(TRUE);
	}
}

void CDefineCircularSegment::OnCircNumOfPoints() {
	UpdateData(FALSE);
	if (TRUE == CKeyPad::Edit(m_npoint)) {
		CString str;
		str.Format(_T("%d"), m_npoint);
		//		GetDlgItem(IDC_POINT)->Invalidate(TRUE);
		// 		CSpinButtonCtrl *pCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_SPINPOINTS);
		// 		pCtrl->SetPos(m_npoint-1);
		GetDlgItem(IDC_POINT)->SetWindowText(str);
		//		GetDlgItem(IDC_POINT)->Invalidate(TRUE);
	}
}

void CDefineCircularSegment::Generate() {}