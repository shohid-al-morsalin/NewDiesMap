// ViewData.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ViewData.h"
#include "Recipe413.h"
#include "..\128\cas\NCas.h"
#include "..\128\cas\CasMgr.h"
#include "413App.h"
#include "MPoint.h"
#include "Data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewData dialog

CViewData::CViewData(CWnd* pParent /*=NULL*/)
	: CDialog(CViewData::IDD, pParent) {
	//{{AFX_DATA_INIT(CViewData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		dwCjName[i] = "";
	}
}

void CViewData::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewData)
	DDX_Control(pDX, IDC_REPORT, m_cReport);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewData, CDialog)
	//{{AFX_MSG_MAP(CViewData)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewData message handlers

BOOL CViewData::OnInitDialog() {
	CDialog::OnInitDialog();

	int i, j, n = 0;
	int nSize[MAXMEPASET * MaXPA];
	dwCjName[n] = "No";    nSize[n++] = 42;
	dwCjName[n] = "X mm";  nSize[n++] = 56;
	dwCjName[n] = "Y mm";  nSize[n++] = 56;
	nCjName = 3;

	CRecipe413* pRcp = &p413App->Global.RcpSetup;

	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &pRcp->MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				dwCjName[n] = Me->MPa[j].Name;
				nSize[n++] = 56;
				nCjName++;
			}
		}
	}
	m_cReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	nListColumn.fmt = LVCFMT_LEFT;
	nListColumn.iSubItem = 0;
	nListColumn.cx = 68;
	nListColumn.pszText = "Name";

	for (i = 0; i < n; i++) {
		nListColumn.cx = nSize[i];
		nListColumn.pszText = dwCjName[i].GetBuffer(0);
		m_cReport.InsertColumn(i, &nListColumn);
	}

	UpdateReport();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewData::UpdateReport() {
	m_cReport.DeleteAllItems();
	CRecipe413* pRcp = &p413App->Global.RcpSetup;

	CPtrList* pMPList = GetMPList();
	if (pMPList) {
		POSITION pos = pMPList->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
			if (p) {
				UpdateReportPoint(pRcp, p);
			}
		}

		m_cReport.EnsureVisible(m_cReport.GetItemCount(), FALSE);
	}
}

CPtrList* CViewData::GetMPList() {
	CRecipe413* pRcp = &p413App->Global.RcpSetup;
	if (pRcp) {
		CPattern* pPatt = &pRcp->Patt;
		if (pPatt) {
			return &pPatt->MP.MPLst;
		}
	}
	return NULL;
}

void CViewData::UpdateReportPoint(CRecipe413* pRcp, CMPoint* p) {
	if (!pRcp->IsValidMeSet(p)) {
		if (p->magic > 9) {
			return;
		}
	}

	// Patch [9/24/2012 Administrator]
	if (p->magic == 9) {
		p->MeSet = 1;
	}
	int cnt = m_cReport.GetItemCount();
	CString str;
	str.Format("%d", p->Co.n);
	int idx = m_cReport.InsertItem(cnt, str);
	m_cReport.SetItemText(idx, 1, p->GetXCoorStr());
	m_cReport.SetItemText(idx, 2, p->GetYCoorStr());
	CData* d = p->GetData(0, FALSE);
	if (!d) {
		return;
	}
	short MeSetX = MaXPA * (p->MeSet - 1);
	CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
	for (int j = 0; j < MaXPA; j++) {
		if (Me->MPa[j].Prb > 0) {
			for (int i = 3; i < nCjName; i++) {
				if (Me->MPa[j].Name.GetLength() > 0) {
					if (dwCjName[i] == Me->MPa[j].Name) {
						float fdata = d->Get(MeSetX + j);
						if (fdata != BADDATANEG) {
							CString dec1;
							dec1.Format("%%.%df", Me->MPa[j].D);
							str.Format(/*"%.3f"*/dec1, fdata);
						}
						else {
							str = "N/A";
						}
						m_cReport.SetItemText(idx, i, str);
						break;
					}
				}
			}
		}
	}
}

void CViewData::OnClear() {
	CRecipe413* pRcp = &p413App->Global.RcpSetup;
	pRcp->ClearData();
	UpdateReport();
}