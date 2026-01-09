// PeakControl.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "PeakControl.h"
#include "PeakControlItem.h"
#include "413App.h"
// #include "Recipe413.h"
// #include "MeParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPeakControl dialog

CPeakControl::CPeakControl(CWnd* pParent /*=NULL*/)
	: CDialog(CPeakControl::IDD, pParent) {
	//{{AFX_DATA_INIT(CPeakControl)
	m_NPeaks1 = 0;
	m_NPeaks2 = 0;
	//}}AFX_DATA_INIT

	pRcp = NULL;
}

void CPeakControl::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPeakControl)
	DDX_Control(pDX, IDC_ALLORNOTHING, m_cAllOrNothing);
	DDX_Control(pDX, IDC_EQUAL2, m_cEqual2);
	DDX_Control(pDX, IDC_EQUAL1, m_cEqual1);
	DDX_Control(pDX, IDC_LIST2, m_cList2);
	DDX_Control(pDX, IDC_LIST1, m_cList1);
	DDX_Text(pDX, IDC_NUMPEAKS1, m_NPeaks1);
	DDX_Text(pDX, IDC_NUMPEAKS2, m_NPeaks2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPeakControl, CDialog)
	//{{AFX_MSG_MAP(CPeakControl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_DEFAULT2, OnDefault2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeakControl message handlers
BOOL CPeakControl::OnInitDialog() {
	CDialog::OnInitDialog();

	char* dwCjName[] =
	{
		"#", "Width", "Thres", "Nearest", "Method", "Active", "Height", "Distance", "Delta"
	};
	int nSize[] = { 32, 64, 42, 64, 52, 42, 46, 64, 46 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cList1.InsertColumn(i, &nListColumn);
		m_cList2.InsertColumn(i, &nListColumn);
	}
	m_cList1.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_cList2.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	LoadLists();

	// 	CString str;
	// 	str.Format("Peak Control : Measurement Set %d", p413App->Global.MeSet);
	// 	SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPeakControl::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cList1.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CPeakShape* PS = &pRcp->MeParam[p413App->Global.CurMeSet - 1].PS1[nSel];	// @@@XXX replace [p413App->Global.CurMeSet-1] with correct info[11/3/2012 Yuen]

	CPeakControlItem dlg;

	dlg.PS = PS;
	dlg.PkNo = nSel + 1;

	dlg.m_Width = PS->Width;
	dlg.m_Thres = PS->Thres;
	dlg.m_Nearest = PS->Near;
	dlg.m_Method = PS->Method;
	dlg.m_Active = PS->bUse;
	dlg.m_Height = PS->Height;
	dlg.m_DeltaDistance = PS->DeltaDistance;
	dlg.m_Distance = PS->Distance;
	if (dlg.DoModal() == IDOK) {
		PS->Width = dlg.m_Width;
		PS->Thres = dlg.m_Thres;
		PS->Near = dlg.m_Nearest;
		PS->Method = dlg.m_Method;
		PS->bUse = dlg.m_Active;
		PS->Height = dlg.m_Height;
		PS->DeltaDistance = dlg.m_DeltaDistance;
		PS->Distance = dlg.m_Distance;
		LoadLists();
	}

	*pResult = 0;
}

void CPeakControl::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) {
	int nSel = m_cList2.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CPeakShape* PS = &pRcp->MeParam[p413App->Global.CurMeSet - 1].PS2[nSel]; 	// @@@XXX replace [p413App->Global.CurMeSet-1] with correct info[11/3/2012 Yuen]

	CPeakControlItem dlg;

	dlg.PS = PS;
	dlg.PkNo = nSel + 1;

	dlg.m_Width = PS->Width;
	dlg.m_Thres = PS->Thres;
	dlg.m_Nearest = PS->Near;
	dlg.m_Method = PS->Method;
	dlg.m_Active = PS->bUse;
	dlg.m_Height = PS->Height;
	dlg.m_DeltaDistance = PS->DeltaDistance;
	dlg.m_Distance = PS->Distance;
	if (dlg.DoModal() == IDOK) {
		PS->Width = dlg.m_Width;
		PS->Thres = dlg.m_Thres;
		PS->Near = dlg.m_Nearest;
		PS->Method = dlg.m_Method;
		PS->bUse = dlg.m_Active;
		PS->Height = dlg.m_Height;
		PS->DeltaDistance = dlg.m_DeltaDistance;
		PS->Distance = dlg.m_Distance;
		LoadLists();
	}

	*pResult = 0;
}

void CPeakControl::OnDefault() {
	if (AfxMessageBox("Set to global values?", MB_YESNO) == IDYES) {
		SetToGlobal(0);
	}
}

void CPeakControl::OnDefault2() {
	if (AfxMessageBox("Set to global values?", MB_YESNO) == IDYES) {
		SetToGlobal(1);
	}
}

void CPeakControl::SetToGlobal(short PrbNo) {
	CPeakShape* PS = NULL;
	CListCtrl* pList = NULL;
	switch (PrbNo) {
	case 0:
		pList = &m_cList1;
		PS = &pRcp->MeParam[p413App->Global.CurMeSet - 1].PSG[0];	// @@@ [11/3/2012 Yuen]
		break;
	default:
		pList = &m_cList2;
		PS = &pRcp->MeParam[p413App->Global.CurMeSet - 1].PSG[1];	// @@@ [11/3/2012 Yuen]
		break;
	}

	pList->DeleteAllItems();
	for (int i = 0; i < MAXPEAKCOUNT; i++) {
		CString str;
		str.Format("%d", i + 1);
		int n = pList->GetItemCount();
		int idx = pList->InsertItem(n, str);
		str.Format("%.2f", PS->Width);
		pList->SetItemText(idx, 1, str);
		str.Format("%d", PS->Thres);
		pList->SetItemText(idx, 2, str);
		str.Format("%d", PS->Near);
		pList->SetItemText(idx, 3, str);
		str.Format("%d", PS->Method);
		pList->SetItemText(idx, 4, str);
		str.Format("%d", PS->bUse);
		pList->SetItemText(idx, 5, str);
	}

	AcceptChanges();
}

void CPeakControl::OnOK() {
	UpdateData(TRUE);

	AcceptChanges();

	CDialog::OnOK();
}

void CPeakControl::LoadLists() {
	CString str;

	CMeParam* p = &pRcp->MeParam[p413App->Global.CurMeSet - 1];	// @@@ [11/3/2012 Yuen]
	m_NPeaks1 = p->NPeak[0];
	m_NPeaks2 = p->NPeak[1];

	m_cEqual1.SetCheck(p->bEqual[0]);
	m_cEqual2.SetCheck(p->bEqual[1]);
	m_cAllOrNothing.SetCheck(pRcp->bAllOrNothing);

	UpdateData(FALSE);

	m_cList1.DeleteAllItems();
	m_cList2.DeleteAllItems();

	CMeParam* Me = &pRcp->MeParam[p413App->Global.CurMeSet - 1];	// @@@ [11/3/2012 Yuen]
	short m;
	for (int i = 0; i < MAXPEAKCOUNT; i++) {
		str.Format("%d", i + 1);
		int n = m_cList1.GetItemCount();
		int idx = m_cList1.InsertItem(n, str);
		CPeakShape* PS = &Me->PS1[i];
		m = 1;
		str.Format("%.2f", PS->Width);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%d", PS->Thres);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%d", PS->Near);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%d", PS->Method);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%d", PS->bUse);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%d", PS->Height);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%.3f", PS->Distance);
		m_cList1.SetItemText(idx, m++, str);
		str.Format("%.3f", PS->DeltaDistance);
		m_cList1.SetItemText(idx, m++, str);

		str.Format("%d", i + 1);
		n = m_cList2.GetItemCount();
		idx = m_cList2.InsertItem(n, str);
		PS = &Me->PS2[i];
		m = 1;
		str.Format("%.2f", PS->Width);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%d", PS->Thres);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%d", PS->Near);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%d", PS->Method);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%d", PS->bUse);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%d", PS->Height);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%.3f", PS->Distance);
		m_cList2.SetItemText(idx, m++, str);
		str.Format("%.3f", PS->DeltaDistance);
		m_cList2.SetItemText(idx, m++, str);
	}
}

void CPeakControl::AcceptChanges() {
	int i;
	CPeakShape* PS = NULL;
	CListCtrl* pList = NULL;

	CMeParam* p = &pRcp->MeParam[p413App->Global.CurMeSet - 1];	// @@@ [11/3/2012 Yuen]

	p->NPeak[0] = m_NPeaks1;
	p->NPeak[1] = m_NPeaks2;
	p->bEqual[0] = m_cEqual1.GetCheck();
	p->bEqual[1] = m_cEqual2.GetCheck();
	pRcp->bAllOrNothing = m_cAllOrNothing.GetCheck();

	pList = &m_cList1;

	short m;
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS = &p->PS1[i];
		CString str;
		m = 1;
		str = pList->GetItemText(i, m++);
		PS->Width = atof(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Thres = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Near = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Method = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->bUse = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Height = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Distance = atof(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->DeltaDistance = atof(str.GetBuffer(0));
	}

	pList = &m_cList2;

	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS = &p->PS2[i];
		CString str;
		m = 1;
		str = pList->GetItemText(i, m++);
		PS->Width = atof(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Thres = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Near = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Method = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->bUse = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Height = atoi(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->Distance = atof(str.GetBuffer(0));
		str = pList->GetItemText(i, m++);
		PS->DeltaDistance = atof(str.GetBuffer(0));
	}
}