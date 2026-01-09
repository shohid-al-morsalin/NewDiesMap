// ProcJobCancelCarrierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "strSOp.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "413App.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/SMCHdr.h"
#include "NavigationPanel.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "TitlePanel.h"
#include "XTabCtrl.h"

#include "ProcJobCancelCarrierDlg.h"

#define  MAXLOGITEMS			2048

#define IDT_UNLATCHTIMEOUT		9999
#define IDT_CLOSETIMEOUT		9998
#define IDT_CLOSEANDQUIT		9997
#define IDT_START				9996
#define IDT_REFRESH				9995

// CProcJobCancelCarrierDlg dialog

CProcJobCancelCarrierDlg::CProcJobCancelCarrierDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobCancelCarrierDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobCancelCarrierDlg)
	//}}AFX_DATA_INIT
	ID = 0;
	m_Parent = NULL;

	Next = UM_TO_PROCJOBPWC2;
}

CProcJobCancelCarrierDlg::~CProcJobCancelCarrierDlg() {}

void CProcJobCancelCarrierDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobCancelCarrierDlg)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_UNLOAD, m_cUnload);
	DDX_Control(pDX, IDC_RECOVERY, m_cRecover);
	DDX_Control(pDX, IDC_CLOSE, m_cClose);
	DDX_Control(pDX, IDC_CANCEL, m_cCancel);
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

BEGIN_MESSAGE_MAP(CProcJobCancelCarrierDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobCancelCarrierDlg)
	ON_BN_CLICKED(IDC_PROCEED, OnProceed)
	ON_COMMAND(ID_COM_BUTTONPUSHED, OnComButtonpushed)
	ON_COMMAND(ID_LP_UNCLAMPOK, OnLpUnclampok)
	ON_COMMAND(ID_LP_UNCLAMPFAIL, OnLpUnclampfail)
	ON_COMMAND(ID_COM_RELEASECMPL, OnComReleasecmpl)
	ON_COMMAND(ID_COM_CARRIERREMOVED, OnComCarrierremoved)
	ON_COMMAND(ID_COM_NONHSMSCARRIERREMOVED, OnComNonHSMSCarrierremoved)
	ON_WM_TIMER()
	ON_COMMAND(ID_LP_FOUPCLOSEOK, OnLpFoupcloseok)
	ON_COMMAND(ID_LP_FOUPCLOSEFAIL, OnLpFoupclosefail)
	ON_COMMAND(ID_COM_ABORTCLOSE, OnComAbortclose)
	ON_COMMAND(ID_COM_ENDCLOSE, OnComEndClose)
	ON_COMMAND(ID_COM_CLOSENORELEASECMPL, OnComClosenoreleasecmpl)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_RECOVERY, OnRecovery)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_UNLOAD, OnUnload)
	ON_BN_CLICKED(ID_COM_UNLOADCOMPLETED, OnUnloadComplete)

	ON_COMMAND(ID_COM_NOVACUUM, OnComNovacuum)
	ON_COMMAND(ID_COM_WAFEROUT, OnComWaferout)
	ON_COMMAND(ID_COM_VACUUMLOST, OnComVacuumlost)
	ON_COMMAND(ID_COM_SENSORTRIP, OnComSensortrip)
	ON_MESSAGE(ID_COM_TP1TIMEOUT, OnComTp1timeout)
	ON_MESSAGE(ID_COM_TP2TIMEOUT, OnComTp2timeout)
	ON_MESSAGE(ID_COM_TP3TIMEOUT, OnComTp3timeout)
	ON_MESSAGE(ID_COM_TP4TIMEOUT, OnComTp4timeout)
	ON_MESSAGE(ID_COM_TP5TIMEOUT, OnComTp5timeout)
	ON_MESSAGE(ID_COM_TP6TIMEOUT, OnComTp6timeout)
	ON_MESSAGE(ID_COM_LOSTE84SIGNAL, OnComLoste84signal)
	ON_MESSAGE(ID_COM_UNEXPECTEDE84SIGNAL, OnComUnexpectede84signal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CProcJobCancelCarrierDlg message handlers

void CProcJobCancelCarrierDlg::OnProceed() {
	if (m_Parent) {
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBUNLOADING, ID, 0);
	}
}

void CProcJobCancelCarrierDlg::OnCancel() {
	OnClose();
}

BOOL CProcJobCancelCarrierDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return FALSE;
	}

	if (!m_Parent || !pGDev || !pUDev) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_UNLOAD, BOTTOM_RIGHT);
	AddAnchor(IDC_PROCEED, TOP_RIGHT);
	AddAnchor(IDC_CANCEL, TOP_RIGHT);
	AddAnchor(IDC_CLOSE, TOP_RIGHT);
	AddAnchor(IDC_RECOVERY, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	// ID is zero offset
	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	if (Next == UM_DISABLECANCEL) {
		m_cCancel.EnableWindow(FALSE);
	}

	SetTimer(IDT_START, 500, NULL);  // change from 200 to 500 [11/14/2007 YUEN]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CProcJobCancelCarrierDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	p413App->Global.bAborting = FALSE;
	return 0;
}

void CProcJobCancelCarrierDlg::OnComButtonpushed() {
	Unclamp();	// Do unclamp if it is clamped
}

void CProcJobCancelCarrierDlg::Unclamp() {
	CString msg;
	//m_Message += "\nUnclamping carrier";
	//UpdateData(FALSE);
	Message("Unclamping carrier", 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_UNCLAMPOK;
	SM.Nak = ID_LP_UNCLAMPFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_UNLATCHTIMEOUT, UNLATCHFOUPTIMEOUT, NULL);
			return;
		}
		//m_Message += "Unclamp carrier send fail";
		//UpdateData(FALSE);
		Message("Unclamp carrier send fail", 2);
		PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
		return;
	}
	Message("LPMgr not found", 0);
	PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
}

void CProcJobCancelCarrierDlg::OnLpUnclampok() {
	Message("Unclamp carrier command send OK", 5);
}

void CProcJobCancelCarrierDlg::OnLpUnclampfail() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	//m_Message += "\nUnclamp command send fail";
	//UpdateData(FALSE);
	Message("Unclamp command send fail", 0);
}

void CProcJobCancelCarrierDlg::OnComReleasecmpl() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	//m_Message += "\n\nWaiting for the carrier to leave the tool";
	//m_Message += "\nCarrier unclamped";
	//UpdateData(FALSE);
	pGDev->SetLPState(ID + 1, PrcsMgrServices::UNCLAMPPED);
	pGDev->pOLnClt->GEMEventFoupUnclamped(ID + 1);
	Message("Carrier unclamped", 5);
	//	OnProceed();
}

void CProcJobCancelCarrierDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		SetTimer(IDT_REFRESH, 1000, NULL);

		CString str;
		CString msg = "";
		msg = pGDev->GetMessage();

		str.LoadString(IDS_CARRIERCANCELED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_FOUPOPENABORTED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			Sleep(2000); // wait is necessary for LP to reset itself
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_FOUPOPENFAILED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			Sleep(2000); // wait is necessary for LP to reset itself
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_SLOTMAPREADFAILED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_SLOTMAPFAILED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_CIDREADFAILED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_CARRIERABORTED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			CloseFoup();
			goto DONE;
		}
		str.LoadString(IDS_CARRIERHALTED);
		if (msg.Find(str) != -1) {
			m_cCancel.ShowWindow(SW_HIDE);
			m_cClose.ShowWindow(SW_HIDE);
			m_cRecover.ShowWindow(SW_SHOW);
			goto DONE;
		}
		m_cCancel.ShowWindow(SW_HIDE);
		CloseFoup();

	DONE:
		//msg.MakeUpper();
		//m_Message = msg;
		UpdateData(FALSE);
	}
	if (nIDEvent == IDT_UNLATCHTIMEOUT) {
		KillTimer(IDT_UNLATCHTIMEOUT);
		Message("Unclamp carrier timeout", 0);
		//m_Message += "\nUnclamp foup timeout";
		//UpdateData(FALSE);
		PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, NULL);
	}
	if (nIDEvent == IDT_CLOSETIMEOUT) {
		KillTimer(IDT_CLOSETIMEOUT);
		Message("Close carrier timeout", 0);
		//m_Message += "\nClose foup timeout";
		//UpdateData(FALSE);
		PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAIL, NULL);
	}
	if (nIDEvent == IDT_CLOSEANDQUIT) {
		KillTimer(IDT_CLOSEANDQUIT);
		if (pUDev->pNavigationPanel) {
			pUDev->pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_STOPBLINKJOBBUTTON, NULL);
		}
		m_Parent->PostMessage(UM_TO_PROCJOBUNLOADCOMPLETE, ID, 0);
	}
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		Refresh();
		SetTimer(IDT_REFRESH, 1000, NULL);
	}
}

void CProcJobCancelCarrierDlg::CloseFoup() {
	//m_Message += "\nClosing foup";
	Message("Closing carrier", 5);
	//UpdateData(FALSE);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	if (/*DosUtil.GetClampState()*/Var.bClampOnUnload) {
		SM.Pm.act = PrcsMgrServices::CLOSENORELEASE;
	}
	else {
		SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	}
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_CLOSETIMEOUT, CLOSEFOUPTIMEOUT + 15000, NULL);
			return;
		}
		Message("Close carrier send fail", 0);
		PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
		return;
	}
	PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
	return;
}

void CProcJobCancelCarrierDlg::OnLpFoupcloseok() {
	Message("Close carrier command OK", 5);
}

void CProcJobCancelCarrierDlg::OnLpFoupclosefail() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose carrier command fail";
	Message("Close carrier command fail", 0);
	//UpdateData(FALSE);

	m_cUnload.EnableWindow(TRUE);   // Allow recovery [5/16/2011 C2C]
}

void CProcJobCancelCarrierDlg::OnComAbortclose() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose carrier aborted";
	Message("Close carrier aborted", 0);
	//UpdateData(FALSE);
}

LRESULT CProcJobCancelCarrierDlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	char NSlotMap[MAXCAPACITYLEN + 1];
	strscpy(NSlotMap, MAXCAPACITYLEN, (char*)(lP));
	if (strncmp(NSlotMap, pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, MAXCAPACITYLEN) != 0) {
		CCarrierObj* pCarrier = pGDev->pGEMObjs->GetCarrierAtPort(ID + 1);
		if (pCarrier) {
			if (pCarrier->SlotmapStatus == CCarrierObj::SMVERIFICATIONOK) {
				Message("Slot map mismatch", 1);
			}
		}
	}
	else {
		DosUtil.RememberSM(ID + 1, "");
	}
	if (lP) {
		strscpy(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, MAXCAPACITYLEN, (char*)(lP + 1));
	}
	else {
		memset(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, '0', MAXCAPACITYLEN);
	}
	return 0;
}

void CProcJobCancelCarrierDlg::OnComClosenoreleasecmpl() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose foup complete";
	Message("Close carrier complete", 5);
	//m_Message += "\nWaiting for foup to leave";
	//UpdateData(FALSE);

	pGDev->pOLnClt->GEMEventFoupClosed(ID + 1);

	pGDev->SetLPState(ID + 1, PrcsMgrServices::UNDOCKED);
	pGDev->pOLnClt->GEMEventFoupUndocked(ID + 1);

	// 	pGDev->SetLPState(ID+1,PrcsMgrServices::UNCLAMPPED);
	// 	pGDev->pOLnClt->GEMEventFoupUnclamped(ID+1);

	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::MANUAL) {
		EnableButtonReport();
	}
	else {
		//m_Message += "\nWaiting for carrier to leave equipment";
		Message("Waiting for carrier to leave equipment", 5);
	}

	m_cUnload.EnableWindow(TRUE);
}

void CProcJobCancelCarrierDlg::OnComEndClose() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose carrier complete";
	Message("Close carrier complete", 5);

	//m_Message += "\nRemove this carrier";
	//UpdateData(FALSE);

	pGDev->SetLPState(ID + 1, PrcsMgrServices::UNDOCKED);
	pGDev->pOLnClt->GEMEventFoupUndocked(ID + 1);

	pGDev->SetLPState(ID + 1, PrcsMgrServices::UNCLAMPPED);
	pGDev->pOLnClt->GEMEventFoupUnclamped(ID + 1);

	m_cUnload.EnableWindow(TRUE);
}

void CProcJobCancelCarrierDlg::ProcessCarrierRemoved() {
	CString CarrierID = "";
	pGDev->pGEMObjs->SetLPAssociation(CarrierID, ID + 1, CLPObj::NOTASSOCIATED);
	pGDev->pOLnClt->GEMEventMaterialRemoved(ID + 1);
	pGDev->SetLPState(ID + 1, PrcsMgrServices::NOFOUP);

	//m_Message += "\nCarrier has left the tool";
	//UpdateData(FALSE);
	Message("Carrier has left the tool", 5);

	if (NoOtherProcJobRuning()) {
		pGDev->SetARAMSState(SArams::STANDBY);
	}

	ShowWindow(SW_HIDE);
}

void CProcJobCancelCarrierDlg::OnComCarrierremoved() {
	Message("Material removed", 5);
	ProcessCarrierRemoved();
	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::MANUAL) {
		OnUnloadComplete();
	}
}

void CProcJobCancelCarrierDlg::OnComNonHSMSCarrierremoved() {
	Message("Material removed (NONHSMS)", 5);
	ProcessCarrierRemoved();
	OnUnloadComplete();
}

BOOL CProcJobCancelCarrierDlg::NoOtherProcJobRuning() {
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {  // Do not start if other load port is in processing mode
			if (pUDev->pProcJob413ProcessingDlg[i]) {
				return FALSE;
			}
			if (pUDev->pProcJobQueued413Dlg[i]) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CProcJobCancelCarrierDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobCancelCarrierDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobCancelCarrierDlg::OnClose() {
	CloseFoup();
}

void CProcJobCancelCarrierDlg::OnRecovery() {
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::CHGSETTING) {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel <= CLogInDataNew::CHGSETTING) {
			return;
		}
	}
	AfxMessageBox("Make sure system is not processing");

	if (m_Parent) {
		int type = 3;
		m_cRecover.ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_WAFERRECOVERY, type, (LPARAM)ID);
	}
	m_cClose.ShowWindow(SW_SHOW);
}

void CProcJobCancelCarrierDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CProcJobCancelCarrierDlg::OnUnload() {
	//	OnClose();
	OnComCarrierremoved();
}

void CProcJobCancelCarrierDlg::EnableButtonReport() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	SM.Pm.act = PrcsMgrServices::ENABLEBUTTONREPORT;
	if (pGDev->pLPMgr) {
		CSMClt ipc;
		if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
			if (!ipc.Send(&SM)) {
				ASSERT(0);
			}
			else {
				Message("\nHit 'Unload' to unclamp this carrier", 5);
				UpdateData(FALSE);
				m_cUnload.EnableWindow(TRUE);
			}
		}
	}
}

void CProcJobCancelCarrierDlg::OnUnloadComplete() {
	SetTimer(IDT_CLOSEANDQUIT, 100, NULL);
}

void CProcJobCancelCarrierDlg::OnComNovacuum() {
	Message("No vacuum error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1NOVACUUM);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2NOVACUUM);
		break;
	}
	//CancelAndQuit();
}

void CProcJobCancelCarrierDlg::OnComWaferout() {
	Message("Wafer protrusion error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1EXCESSIVEWAFEROUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2EXCESSIVEWAFEROUT);
		break;
	}
	//CancelAndQuit();
}

void CProcJobCancelCarrierDlg::OnComVacuumlost() {
	Message("Vacuum lost error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1VACUUMLOST);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2VACUUMLOST);
		break;
	}
	//CancelAndQuit();
}

void CProcJobCancelCarrierDlg::OnComSensortrip() {
	Message("Sensor trip error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1PERISENSORTRIPED);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2PERISENSORTRIPED);
		break;
	}
	//CancelAndQuit();
}

LRESULT CProcJobCancelCarrierDlg::OnComTp1timeout(WPARAM wP, LPARAM lP) {
	Message("TP1 timeout", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP1TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP1TIMEOUT);
		break;
	}
	return TPErrorRecovery(1, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComTp2timeout(WPARAM wP, LPARAM lP) {
	Message("TP2 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP2TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP2TIMEOUT);
		break;
	}
	return TPErrorRecovery(2, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComTp3timeout(WPARAM wP, LPARAM lP) {
	Message("TP3 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP3TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP3TIMEOUT);
		break;
	}
	return TPErrorRecovery(3, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComTp4timeout(WPARAM wP, LPARAM lP) {
	Message("TP4 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP4TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP4TIMEOUT);
		break;
	}
	return TPErrorRecovery(4, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComTp5timeout(WPARAM wP, LPARAM lP) {
	Message("TP5 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP5TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP5TIMEOUT);
		break;
	}
	return TPErrorRecovery(5, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComTp6timeout(WPARAM wP, LPARAM lP) {
	Message("TP6 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP6TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP6TIMEOUT);
		break;
	}
	return TPErrorRecovery(6, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComLoste84signal(WPARAM wP, LPARAM lP) {
	Message("Lost E84 signal", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1LOSTE84SIGNAL);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2LOSTE84SIGNAL);
		break;
	}
	return TPErrorRecovery(7, wP);
}

LRESULT CProcJobCancelCarrierDlg::OnComUnexpectede84signal(WPARAM wP, LPARAM lP) {
	Message("Unexpected E84 signal", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1UNEXPECTEDE84SIGNAL);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2UNEXPECTEDE84SIGNAL);
		break;
	}
	return TPErrorRecovery(8, wP);
}

void CProcJobCancelCarrierDlg::Message(CString msg, short lvl) {
	// 	Log(msg,lvl);
	// 	m_Message += "\n";
	// 	m_Message += msg;
	// 	UpdateData(FALSE);
	int n = m_cList.GetCount();
	if (n > MAXLOGITEMS) {
		for (int i = n; i >= 17; i--) {
			m_cList.DeleteString(i);
		}
	}
	CString str;
	str = msg;
	msg.Format("%s (LP%d)", str, ID + 1);
	Log(msg, lvl);
	msg = pGDev->GetTimeStamp() + msg;
	m_cList.InsertString(0, msg);
}

// TP error implies that load port is in AUTO mode [3/16/2012 Administrator]
int CProcJobCancelCarrierDlg::TPErrorRecovery(short errCode, BOOL bCarrierPresent) {
	CString msg;
	msg.Format("TPRecovery carrier: carrier %s present", bCarrierPresent ? "" : "not");
	Message(msg, 1);

	if (bCarrierPresent) {
	}
	else {
		if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::AUTO) {
			//OnComCarrierremoved();
			OnUnloadComplete();
		}
	}

	return 0;
}

void CProcJobCancelCarrierDlg::Refresh() {
	Xch.ID = ID + 1;
	pGDev->GetLPState(Xch);

	CCarrierObj* pCarrier = pGDev->pGEMObjs->GetCarrierAtPort(ID + 1);

	if (pCarrier) {
		Xch.m_IDStatus = pCarrier->GetIDStatusStr();
		Xch.m_SlotMapStatus = pCarrier->GetSlotmapStatusStr();
		Xch.m_CarrierID = pCarrier->ObjID;
		Xch.m_SubstCount.Format("%d", pCarrier->SubstrateCount);
	}
	UpdateData(FALSE);
}