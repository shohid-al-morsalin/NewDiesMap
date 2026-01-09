// ProcJobCompleteManual.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ProcJobCompleteManual.h"
#include "GlobalDeviceContainer.h"
#include "LTW/LTWMgr.h"  //012324 Masum Added for THEIL 413SA Signal Tower

#include "..\SYS\CUMMsg.h"

#include "413App.h"
#include "InfoPanelMain.h"
#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXLOGITEMS	2048

#define IDT_START	9999

/////////////////////////////////////////////////////////////////////////////
// CProcJobCompleteManual dialog

CProcJobCompleteManual::CProcJobCompleteManual(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobCompleteManual::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobCompleteManual)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ID = 0;
	m_Parent = NULL;
}

CProcJobCompleteManual::~CProcJobCompleteManual() {}

void CProcJobCompleteManual::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobCompleteManual)
	DDX_Control(pDX, IDC_DONE, m_cDone);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobCompleteManual, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobCompleteManual)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DONE, OnDone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobCompleteManual message handlers

void CProcJobCompleteManual::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CProcJobCompleteManual::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_DONE, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	SetTimer(IDT_START, 500, NULL);  // change from 200 to 500 [11/14/2007 YUEN]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobCompleteManual::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		ProcessingComplete();
	}
}

void CProcJobCompleteManual::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "PJComManual");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobCompleteManual::Message(CString msg, short lvl) {
	int n = m_cList.GetCount();
	if (n > MAXLOGITEMS) {
		for (int i = n; i >= 17; i--) {
			m_cList.DeleteString(i);
		}
	}
	Log(msg, lvl);
	msg = pGDev->GetTimeStamp() + msg;
	m_cList.InsertString(0, msg);
}

void CProcJobCompleteManual::ProcessingComplete() {
	Message("Processing complete");

	Message("Move stage to load position");

	// Move stage to load position [8/13/2011 FSMT]
	if (!p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) // 20140223 SCOTT ADD : CHECK RETURN VALUE
	{
		p413App->StageMoveError();
	}

	// Switch off vacuum [8/13/2011 FSMT]
	if (p413App->pMtr) p413App->pMtr->SetChuckVacuum(FALSE);

	Message("Please remove the wafer");

	// [012324 Masum Added for THEIL 413SA Signal Tower
	if (pGDev->SystemModel == pGDev->FSM413SA) {
		if (pGDev->pLTMgr) pGDev->pLTMgr->Set_Idle();
	}
	// ]

	m_cDone.ShowWindow(SW_SHOW);

	pGDev->SetARAMSState(SArams::STANDBY);

	OnDone();
}

void CProcJobCompleteManual::OnDone() {
	if (m_Parent) {
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBMANUALDLG, ID, 0);
	}
}