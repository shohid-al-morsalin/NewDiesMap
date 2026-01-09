#include "stdafx.h"
#include "resource.h"

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "XTabCtrl.h"

#include "System413Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystem413Dlg dialog

#define IDT_REFRESH				9999
#define IDT_DATAREQ_TIMEOUT		9998

CSystem413Dlg::CSystem413Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSystem413Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSystem413Dlg)
	m_Service = _T("");
	m_Status = _T("");
	//}}AFX_DATA_INIT

	if (!pGDev || !pGDev->pGEMObjs) {
		ASSERT(0);
	}
	//	bStop = FALSE;
}

void CSystem413Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystem413Dlg)
	DDX_Control(pDX, IDC_CONNECTION, m_cConnection);
	DDX_Control(pDX, IDC_SERVICE, m_cService);
	DDX_Text(pDX, IDC_SERVICESTATUS, m_Service);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSystem413Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSystem413Dlg)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_BN_CLICKED(IDC_SERVICE, OnService)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_WM_TIMER()
	ON_MESSAGE(ID_IPC_DATAREQACK, OnIpcDatareqack)
	ON_MESSAGE(ID_IPC_DATAREQNAK, OnIpcDatareqnak)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystem413Dlg message handlers

LRESULT CSystem413Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("System\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CSystem413Dlg::OnService() {
	C413Obj* p = pGDev->pGEMObjs->StMd.Get413Obj();
	if (p->Service == C413Obj::INSERVICE) {
		if (pGDev->AramsE95.GetCurrentState() == SArams::PRODUCTIVE) {
			AfxMessageBox("PRODUCTIVE: State change not allowed");
		}
		// 		if (pGDev->AuthenticateUser())
		// 		{
		p->Service = C413Obj::OUTOFSERVICE;
		m_cService.SetWindowText("In Service");
		// 		}
	}
	else {
		p->Service = C413Obj::INSERVICE;
		m_cService.SetWindowText("Out of Service");
	}
	pGDev->pGEMObjs->StMd.Set413Serv(p->Service);
	m_Service = pGDev->pGEMObjs->StMd.Get413ServStr();
	UpdateData(FALSE);
}

void CSystem413Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		if (!p413App->Global.bStop) Refresh();
		SetTimer(IDT_REFRESH, 2000, NULL);
	}
	if (nIDEvent == IDT_DATAREQ_TIMEOUT) {
		KillTimer(IDT_DATAREQ_TIMEOUT);
		m_Status = "413 Unit fail to response";
		UpdateData(FALSE);
	}
}

void CSystem413Dlg::Refresh() {
	C413Obj* p = pGDev->pGEMObjs->StMd.Get413Obj();
	if (p->Service == C413Obj::INSERVICE) {
		m_cService.SetWindowText("Out of Service");
	}
	else {
		m_cService.SetWindowText("In Service");
	}
	m_cConnection.SetWindowText(p->GetOpStatusStr());
	m_Service = pGDev->pGEMObjs->StMd.Get413ServStr();
	UpdateData(FALSE);
}

void CSystem413Dlg::OnUpdate() {
	ASSERT(0);
	m_Status = "";
	UpdateData(FALSE);

	// 	if (!StartReq413(FALSE))
	// 	{
	// 		m_Status = "413 Unit fail to response";
	// 		UpdateData(FALSE);
	// 	}
}

LRESULT CSystem413Dlg::OnIpcDatareqack(WPARAM wP, LPARAM lP) {
	if (wP != 999) {
		ASSERT(0);
		return -1;
	}
	DataReqAck413(lP);

	m_Status = m_Message;

	UpdateData(FALSE);

	return 0;
}

LRESULT CSystem413Dlg::OnIpcDatareqnak(WPARAM wP, LPARAM lP) {
	SetTimer(IDT_DATAREQ_TIMEOUT, 10, NULL);
	//	if (wP != 999)
	//	{
	//		ASSERT(0);
	//		return -1;
	//	}

	// No need to delete lP

	return 0;
}

int CSystem413Dlg::GetHWnd() {
	return (int)m_hWnd;
}

//  [10/27/2008 Yuen]
void CSystem413Dlg::SetDataReqTimeOut(long dur) {
	SetTimer(IDT_DATAREQ_TIMEOUT, dur, NULL);
}

void CSystem413Dlg::Stop() {
	p413App->Global.bStop = TRUE;
	//	KillTimer(IDT_REFRESH);
	//	KillTimer(IDT_DATAREQ_TIMEOUT);
}

void CSystem413Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSystem413Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_SERVICE, TOP_RIGHT);
	AddAnchor(IDC_UPDATE, BOTTOM_RIGHT);
	AddAnchor(IDC_STATUS, TOP_LEFT, BOTTOM_RIGHT);

	Refresh();

	SetTimer(IDT_REFRESH, 3000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}