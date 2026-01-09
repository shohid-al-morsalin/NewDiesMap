#include "stdafx.h"
#include "e95.h"
#include "PosHt.h"
#include "413App.h"
#include "413/EchoScope/EchoProbeBase.h"

#include "DefineSimuPeaks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CDefineSimuPeaks* g_cList = NULL;

CDefineSimuPeaks::CDefineSimuPeaks(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineSimuPeaks::IDD, pParent) {
	Set = 1;
	Num = 1;
	OSet = 0;
	ONum = 0;
}

void CDefineSimuPeaks::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineSimuPeaks)
	DDX_Control(pDX, IDC_TREE1, m_Tree1);
	DDX_Control(pDX, IDC_TREE2, m_Tree2);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineSimuPeaks, CDialog)
	//{{AFX_MSG_MAP(CDefineSimuPeaks)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, OnSelchangedTree2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineSimuPeaks message handlers

BOOL CDefineSimuPeaks::OnInitDialog() {
	CDialog::OnInitDialog();

	int i;
	CString str;
	for (i = 1; i <= 16; i++) {
		str.Format("%d               ", i);
		m_Tree1.InsertItem(str);
	}
	for (i = 1; i <= 10; i++) {
		str.Format("%d               ", i);
		m_Tree2.InsertItem(str);
	}

	//if you use OnSelChanged, you must provide a PFN_OnSelChanged function point;
	g_cList = this;
	m_cList.SetPFNOnSelChanged(pfnOnSelChangedPropertyListDefine);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void pfnOnSelChangedPropertyListDefine(const CPropertyItem* pItem) {
	if (g_cList) {
		g_cList->SetDlgItemText(IDC_STATIC, pItem->m_propName + " = " + pItem->m_curValue);
	}
}

void CDefineSimuPeaks::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM node = pNMTreeView->itemNew.hItem;
	Set = atoi(m_Tree1.GetItemText(node));

	DoDataEntry();

	*pResult = 0;
}

void CDefineSimuPeaks::OnSelchangedTree2(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM node = pNMTreeView->itemNew.hItem;

	CString str;
	str = m_Tree2.GetItemText(node);
	Num = atoi(str);

	DoDataEntry();

	*pResult = 0;
}

void CDefineSimuPeaks::DoDataEntry() {
	ParseEntry();
	AddItemToList();
}

void CDefineSimuPeaks::AddItemToList() {
	CPosHtItem* p;
	CString Cat, Itm, n;

	m_cList.DeleteAll();

	Cat.Format("TOP:%02d.%02d", Set, Num);

	p = &p413App->pEcho->PosT[Set - 1].Pos[Set - 1];
	n.Format("%.3f", p->Position);
	m_cList.AddPropItem(Cat, "1. Position", n, PIT_EDIT, "");
	n.Format("%.3f", p->dP);
	m_cList.AddPropItem(Cat, "2. DPosition", n, PIT_EDIT, "");
	n.Format("%.3f", p->Height);
	m_cList.AddPropItem(Cat, "3. Height", n, PIT_EDIT, "");
	n.Format("%.3f", p->dH);
	m_cList.AddPropItem(Cat, "4. DHeight", n, PIT_EDIT, "");

	Cat.Format("BOT:%02d.%02d", Set, Num);

	p = &p413App->pEcho->PosB[Set - 1].Pos[Set - 1];
	n.Format("%.3f", p->Position);
	m_cList.AddPropItem(Cat, "1. Position", n, PIT_EDIT, "");
	n.Format("%.3f", p->dP);
	m_cList.AddPropItem(Cat, "2. DPosition", n, PIT_EDIT, "");
	n.Format("%.3f", p->Height);
	m_cList.AddPropItem(Cat, "3. Height", n, PIT_EDIT, "");
	n.Format("%.3f", p->dH);
	m_cList.AddPropItem(Cat, "4. DHeight", n, PIT_EDIT, "");
}

void CDefineSimuPeaks::ParseEntry() {
	CString m, n, o;
	for (POSITION pos = m_cList.GetHead(); pos != NULL;) {
		CPropertyItem* pItem = (CPropertyItem*)m_cList.GetNext(pos);
		if (pItem->m_nItemType == PIT_CATALOG) {
			continue;
		}
		m = pItem->m_catalog;
		n = m.Mid(4, 2);
		OSet = atoi(n);
		n = m.Mid(7, 2);
		ONum = atoi(n);
		CPosHtItem* p;
		if (m.Find("TOP:") != -1) {
			p = &p413App->pEcho->PosT[OSet - 1].Pos[ONum - 1];
		}
		else {
			p = &p413App->pEcho->PosB[OSet - 1].Pos[ONum - 1];
		}

		if (pItem->IsProp("Position")) {
			p->Position = atof(pItem->m_curValue);
			continue;
		}
		if (pItem->IsProp("DPosition")) {
			p->dP = atof(pItem->m_curValue);
			continue;
		}
		if (pItem->IsProp("Height")) {
			p->Height = atof(pItem->m_curValue);
			continue;
		}
		if (pItem->IsProp("DHeight")) {
			p->dH = atof(pItem->m_curValue);
			continue;
		}
	}
}