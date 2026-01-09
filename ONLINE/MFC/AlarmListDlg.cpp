#include "stdafx.h"
#include "resource.h"

#include <vector>

#include "GlobalUIContainer.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "XTabCtrl.h"

#include "AlarmListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_CLEARCONTENT 9999

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDlg dialog

CAlarmListDlg::CAlarmListDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CAlarmListDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CAlarmListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAlarmListDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmListDlg)
	DDX_Control(pDX, IDC_ALRAMLIST, m_AlarmDisplay);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlarmListDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CAlarmListDlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDlg message handlers

BOOL CAlarmListDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_ALRAMLIST, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDOK, TOP_RIGHT);
	AddAnchor(IDC_CLEAR, TOP_RIGHT);
	AddAnchor(IDCANCEL, TOP_RIGHT);

	m_Brush.CreateSolidBrush(RGB(191, 201, 249));
	m_Font.CreateFont(-15, -10, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FF_SWISS, "Courier");

	CListBox* pList = (CListBox*)GetDlgItem(IDC_ALRAMLIST);
	pList->SetFont(&m_Font);

	ClearContent();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CAlarmListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	CListBox* pList = (CListBox*)GetDlgItem(IDC_ALRAMLIST);
	if (nCtlColor == CTLCOLOR_LISTBOX && pList->GetSafeHwnd() == pWnd->GetSafeHwnd()) {
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(191, 201, 249));
		pDC->SetTextColor(RGB(255, 0, 0));
		return m_Brush;
	}
	HBRUSH hbr = CResizableDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CAlarmListDlg::OnOk() {
	ClearContent();
}

void CAlarmListDlg::OnClear() {
	CString str, AlrNo, AlrText;
	int sel = m_AlarmDisplay.GetCurSel();
	if (sel == LB_ERR) {
		return;
	}
	m_AlarmDisplay.GetText(m_AlarmDisplay.GetCurSel(), str);
	if (str.GetLength() < 1) {
		return;
	}
	int idx = str.Find(":", 0);
	AlrNo = str.Left(idx - 1);

	UINT AlarmCode = atoi(AlrNo.GetBuffer(0));
	CEIDCtlr.m_alarm.ClearAlarm(AlarmCode); //  [9/16/2007 LYF]
	SetTimer(IDT_CLEARCONTENT, 1000, NULL);
}

void CAlarmListDlg::ClearContent() {
	m_AlarmDisplay.ResetContent();
	std::vector<UINT> AlSet;
	std::vector<CString> AlSetTx;

	CEIDCtlr.m_alarm.GetAlarmSetID(AlSet, AlSetTx); //  [9/16/2007 LYF]

	CString AlTxt, FullTxt;
	for (UINT nAlarms = 0; nAlarms < AlSet.size(); nAlarms++) {
		CAlrmItem* pA = CEIDCtlr.m_alarm.GetAlarm(AlSet[nAlarms]);
		FullTxt.Format("%s: %4.0d, %s", CTime::GetCurrentTime().Format("%y/%m/%d %H:%M:%S"), pA->alID, pA->alTX); //  [9/16/2007 LYF]
		m_AlarmDisplay.AddString(FullTxt);
	}
}

LRESULT CAlarmListDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Alarms\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	return 0;
}

void CAlarmListDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_CLEARCONTENT) {
		KillTimer(IDT_CLEARCONTENT);
		ClearContent();
	}
}

void CAlarmListDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CAlarmListDlg::OnDestroy() {
	m_Font.DeleteObject();
	m_Brush.DeleteObject();
	CResizableDialog::OnDestroy();
}