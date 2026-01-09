// LoadPortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"

#include "FEM/IDR/IDRMgr.h"
#include "FEM/IDR/IDRSvr.h"
#include "FEM/IDR/IDRUnit.h"
#include "FEM/Ldp/LPMgr.h"
#include "128/GEM/ObjectsContainer.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IPC/SMCHdr.h"
#include "IPC/SMHdr.h"
#include "LoadPortDlg.h"
#include "LTW/LTMgrE95.h"
#include "SRC/DOSUtil.h"
#include "SRC/LogInDataNew.h"
#include "TitlePanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_REFRESH		9999

/////////////////////////////////////////////////////////////////////////////
// CLoadPortDlg dialog

CLoadPortDlg::CLoadPortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadPortDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CLoadPortDlg)
	//	m_TransferStatus = _T("");
	//	m_AccessingStatus = _T("");
	m_SlotMapStatus = _T("");
	m_IDStatus = _T("");
	m_AccessMode = _T("");
	//	m_Association = _T("");
	//	m_Reservation = _T("");
	m_CIDRStatus = _T("");
	m_Bypass = _T("");
	//}}AFX_DATA_INIT
	ID = 0;

	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
}

void CLoadPortDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadPortDlg)
	DDX_Control(pDX, IDC_ACTIVECARRIER, m_cActiveCarrier);
	DDX_Control(pDX, IDC_CLEARBLOCK, m_cClearBlock);
	DDX_Control(pDX, IDC_MODULE, m_cModule);
	DDX_Control(pDX, IDC_CARRIERID, m_cCarrierID);
	DDX_Control(pDX, IDC_SERVICE2, m_cService2);
	DDX_Control(pDX, IDC_BYPASSCIDR2, m_cBypass);
	DDX_Control(pDX, IDC_ACCESSMODEB, m_cAccessModeB);
	DDX_Control(pDX, IDC_SERVICE, m_cService);
	DDX_Text(pDX, IDC_CIDRSTATUS, m_CIDRStatus);
	DDX_Text(pDX, IDC_CIDRBYPASS, m_Bypass);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_ASSOCIATION, Xch.m_Association);
	DDX_Text(pDX, IDC_ACCESSMODE, Xch.m_AccessMode);
	DDX_Text(pDX, IDC_RESERVATION, Xch.m_Reservation);
	DDX_Text(pDX, IDC_TRANSFERSTATUS, Xch.m_TransferStatus);
	DDX_Text(pDX, IDC_SLOTMAPSTATUS, Xch.m_SlotMapStatus);
	DDX_Text(pDX, IDC_IDSTATUS, Xch.m_IDStatus);
	DDX_Text(pDX, IDC_ACCESSINGSTATUS, Xch.m_AccessingStatus);
	DDX_Text(pDX, IDC_SUBSTCOUNT, Xch.m_SubstCount);
	DDX_Text(pDX, IDC_OBJID, Xch.m_CarrierID);
}

BEGIN_MESSAGE_MAP(CLoadPortDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadPortDlg)
	ON_BN_CLICKED(IDC_ACCESSMODEB, OnAccessmodeb)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SERVICE, OnService)
	ON_BN_CLICKED(IDC_BYPASSCIDR2, OnBypasscidr)
	ON_BN_CLICKED(IDC_SERVICE2, OnService2)
	ON_BN_CLICKED(IDC_CLEARBLOCK, OnClearblock)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_READCIDR2, OnReadcidr2)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_HOAVBL, OnHoavbl)
	ON_BN_CLICKED(IDC_UNCLAMP, OnUnclamp)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_CANCELCARRIER, OnCancelcarrier)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadPortDlg message handlers

BOOL CLoadPortDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	//	if (pGDev->pGEMObjs->GetLPTransferStatus(ID+1) == CLPObj::OUTOFSERVICE)
	//	{
	//		m_cService.SetWindowText("In Service");
	//	}
	//	else {
	//		m_cService.SetWindowText("Out of Service");
	//	}

	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::MANUAL) {
		m_cAccessModeB.SetWindowText("Auto");
	}
	else {
		m_cAccessModeB.SetWindowText("Manual");
	}

	if (pGDev->pGEMObjs->GetIDRBypass(ID + 1) == CIDRObj::BYPASSED) {
		m_cBypass.SetCheck(TRUE);
	}
	else {
		m_cBypass.SetCheck(FALSE);
	}

	//	if (pGDev->pGEMObjs->StMd.GetIDRService(ID+1) == CIDRObj::INSERVICE)
	//	{
	//		m_cService2.SetWindowText("Out of Service");
	//	}
	//	else {
	//		m_cService2.SetWindowText("In Service");
	//	}

	OnCheck();

	SetTimer(IDT_REFRESH, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadPortDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		Refresh();
		SetTimer(IDT_REFRESH, 3000, NULL);
	}
}

void CLoadPortDlg::Refresh() {
	Xch.ID = ID + 1;
	pGDev->GetLPState(Xch);
	m_AccessMode = Xch.m_AccessMode;

	if (m_AccessMode == "AUTO") {
		m_cAccessModeB.SetWindowText("Manual");
	}
	else {
		m_cAccessModeB.SetWindowText("Auto");
	}
	// 	m_TransferStatus = Xch.m_TransferStatus;
	// 	m_Association = Xch.m_Association;
	// 	m_AccessingStatus = Xch.m_AccessingStatus;
	// 	m_Reservation = Xch.m_Reservation;

	CCarrierObj* pCarrier = pGDev->pGEMObjs->GetCarrierAtPort(ID + 1);

	if (pCarrier) {
		Xch.m_IDStatus = pCarrier->GetIDStatusStr();
		Xch.m_SlotMapStatus = pCarrier->GetSlotmapStatusStr();
		Xch.m_CarrierID = pCarrier->ObjID;
		Xch.m_SubstCount.Format("%d", pCarrier->SubstrateCount);
	}
	if (pGDev->pGEMObjs->GetLPTransferStatus(ID + 1) == CLPObj::OUTOFSERVICE) {
		m_cService.SetWindowText("In Service");
	}
	else {
		m_cService.SetWindowText("Out of Service");
	}
	CIDRObj* p = pGDev->pGEMObjs->GetIDRObj(ID + 1);
	if (p) {
		if (p->GetServiceStatus() == CIDRObj::OUTOFSERVICE) {
			m_cService2.SetWindowText("In Service");
		}
		else {
			m_cService2.SetWindowText("Out of Service");
		}
	}
	m_CIDRStatus = pGDev->GetIDRStatusStr(ID + 1);
	m_Bypass = pGDev->GetIDRBypassStr(ID + 1);

	UpdateData(FALSE);
}

void CLoadPortDlg::OnService() {
	if (pGDev->pGEMObjs->GetLPTransferStatus(ID + 1) == CLPObj::OUTOFSERVICE) {
		//		m_cService.SetWindowText("Out of Service");
		PrcsMgrServices::LPBSTATE LpS = pGDev->GetLPState(ID + 1);
		if (LpS <= PrcsMgrServices::FOUPERROR) {
			//TODO: check load port for carrier
		}
		else if (LpS == PrcsMgrServices::NOFOUP) {
			pGDev->pGEMObjs->SetLPTransferStatus(ID + 1, CLPObj::READYTOLOAD);
		}
		else {
			pGDev->pGEMObjs->SetLPTransferStatus(ID + 1, CLPObj::READYTOUNLOAD);
		}
	}
	else {
		if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::RUNRCP) {
			short AuthenLevel;
			if (pGDev->AuthenticateUser(AuthenLevel)) {
				if (AuthenLevel >= CLogInDataNew::RUNRCP) {
					pGDev->pGEMObjs->SetLPTransferStatus(ID + 1, CLPObj::OUTOFSERVICE);
				}
			}
		}
		else {
			pGDev->pGEMObjs->SetLPTransferStatus(ID + 1, CLPObj::OUTOFSERVICE);
		}
	}
	pGDev->pLTMgr->SetLight();
}

void CLoadPortDlg::OnAccessmodeb() {
	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::AUTO) {
		//TODO: switch load port mode accordingly
		pGDev->pGEMObjs->SetLPAccessMode(ID + 1, CLPObj::MANUAL);
		m_cAccessModeB.SetWindowText("Auto");
	}
	else {
		pGDev->pGEMObjs->SetLPAccessMode(ID + 1, CLPObj::AUTO);
		m_cAccessModeB.SetWindowText("Manual");
	}
}

void CLoadPortDlg::OnBypasscidr() {
	CIDRObj* p = pGDev->pGEMObjs->GetIDRObj(ID + 1);
	if (!p) {
		return;
	}
	if (p->GetBypassStatus() == CIDRObj::BYPASSED) {
		pGDev->pGEMObjs->SetIDRBypass(ID + 1, CIDRObj::INUSE);
	}
	else {
		pGDev->pGEMObjs->SetIDRBypass(ID + 1, CIDRObj::BYPASSED);
	}
}

void CLoadPortDlg::OnService2() {
	CIDRObj* p = pGDev->pGEMObjs->GetIDRObj(ID + 1);
	if (!p) {
		return;
	}
	if (p->GetServiceStatus() == CIDRObj::INSERVICE) {
		if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::RUNRCP) {
			short AuthenLevel;
			if (pGDev->AuthenticateUser(AuthenLevel)) {
				if (AuthenLevel >= CLogInDataNew::RUNRCP) {
					pGDev->pGEMObjs->SetIDRService(ID + 1, CIDRObj::OUTOFSERVICE);
				}
			}
		}
		else {
			pGDev->pGEMObjs->SetIDRService(ID + 1, CIDRObj::OUTOFSERVICE);
		}
	}
	else {
		pGDev->pGEMObjs->SetIDRService(ID + 1, CIDRObj::INSERVICE);
	}
}

void CLoadPortDlg::OnClearblock() {
	DosUtil.ForgetPrJob(ID + 1);
	OnCheck();
}

void CLoadPortDlg::OnCheck() {
	CString CarrierID, Recipe;
	DosUtil.GetRememberedPrJob(ID + 1, CarrierID, Recipe);

	m_cModule.SetWindowText("413 Module");
	if (CarrierID.GetLength() > 0) {
		m_cActiveCarrier.SetWindowText(CarrierID.GetBuffer(0));
		if ((pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) ||
			(pGDev->SystemModel == CGlobalDeviceContainer::FSM413MOT) ||
			(pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA)) {
			m_cModule.SetWindowText("413 Module");
		}
		m_cClearBlock.EnableWindow(TRUE);
	}
	else {
		m_cClearBlock.EnableWindow(FALSE);
		m_cActiveCarrier.SetWindowText("");
		m_cModule.SetWindowText("");
	}
}

void CLoadPortDlg::OnReadcidr2() {
	if (pGDev->pIDRMgr && pGDev->pIDRMgr->pCIDRSvr[ID]) {
		CIDRUnit* pIDR = pGDev->pIDRMgr->pCIDRSvr[ID]->IDRUnit;
		if (pIDR) {
			m_cCarrierID.SetWindowText(pIDR->ReadCID());
		}
	}
}

void CLoadPortDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}

void CLoadPortDlg::OnOpen() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL/*m_hWnd*/;
	SM.Ack = ID_LP_FOUPOPENOK;
	SM.Nak = ID_LP_FOUPOPENFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::OPENFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CLoadPortDlg::OnClose() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL/*m_hWnd*/;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CLoadPortDlg::OnHoavbl() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.port = ID + 1;
	SM.Pm.act = PrcsMgrServices::ENABLEHOAVBL;
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CLoadPortDlg::OnUnclamp() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL/*m_hWnd*/;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CLoadPortDlg::OnReset() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL/*m_hWnd*/;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::RESET;
	CSMClt ipc;
	CString msg;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return;
		}
	}
}

void CLoadPortDlg::OnCancelcarrier() {}