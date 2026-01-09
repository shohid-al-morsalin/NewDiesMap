#include "stdafx.h"
#include "strSOp.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "NavigationPanel.h"
#include "SRC/DOSUtil.h"
#include "SYS/CUMMsg.h"
#include "IPC/SMCHdr.h"

#include "ProcJobCompleteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MAXLOGITEMS 2048

#define IDT_START				9999
#define IDT_CLOSETIMEOUT		9998
#define IDT_UNLATCHTIMEOUT		9997
#define IDT_CLOSEANDQUIT		9997
#define IDT_REFRESH				9996

/////////////////////////////////////////////////////////////////////////////
// CProcJobCompleteDlg dialog

CProcJobCompleteDlg::CProcJobCompleteDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobCompleteDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobCompleteDlg)
	//}}AFX_DATA_INIT
	ID = 0;
	bOK = TRUE;
	m_Parent = NULL;
}

CProcJobCompleteDlg::~CProcJobCompleteDlg() {}

void CProcJobCompleteDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobCompleteDlg)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_UNLOAD, m_cUnload);
	//DDX_Control(pDX, IDC_PROCEED, m_cProceed);
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

BEGIN_MESSAGE_MAP(CProcJobCompleteDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobCompleteDlg)
	ON_COMMAND(ID_COM_BUTTONPUSHED, OnComButtonpushed)
	ON_COMMAND(ID_LP_UNCLAMPOK, OnLpUnclampok)
	ON_COMMAND(ID_LP_UNCLAMPFAIL, OnLpUnclampfail)
	ON_COMMAND(ID_COM_RELEASECMPL, OnComReleasecmpl)
	ON_COMMAND(ID_COM_CARRIERREMOVED, OnComCarrierremoved)
	ON_WM_TIMER()
	ON_COMMAND(ID_LP_FOUPCLOSEOK, OnLpFoupcloseok)
	ON_COMMAND(ID_LP_FOUPCLOSEFAIL, OnLpFoupclosefail)
	ON_COMMAND(ID_COM_ABORTCLOSE, OnComAbortclose)
	ON_COMMAND(ID_COM_ENDCLOSE, OnComEndClose)
	ON_COMMAND(ID_COM_CLOSENORELEASECMPL, OnComClosenoreleasecmpl)
	ON_COMMAND(ID_COM_ABORTAPHDRELEASE, OnComAbortaphdrelease)
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
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobCompleteDlg message handlers

// void CProcJobCompleteDlg::OnProceed()
// {
// 	if (bOK)
// 	{
// 		opProceed();
// 	}
// 	else
// 	{
// 		opAbort();
// 	}
// }

BOOL CProcJobCompleteDlg::OnInitDialog() {
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
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	SetTimer(IDT_START, 500, NULL);  // change from 200 to 500 [11/14/2007 YUEN]

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobCompleteDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		SetTimer(IDT_REFRESH, 2000, NULL);
		CloseFoup();
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
		SetTimer(IDT_REFRESH, 3000, NULL);
	}
}

void CProcJobCompleteDlg::CloseFoup() {
	//m_Message = "Closing foup";
	Message("Closing carrier", 5);
	//UpdateData(FALSE);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPCLOSEOK;
	SM.Nak = ID_LP_FOUPCLOSEFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	if (DosUtil.GetClampState()) {
		SM.Pm.act = PrcsMgrServices::CLOSENORELEASE;
	}
	else {
		SM.Pm.act = PrcsMgrServices::CLOSEFOUP;
	}
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_CLOSETIMEOUT, CLOSEFOUPTIMEOUT + 8000, NULL);
			return;
		}
		Message("Close carrier send fail", 0);
		PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
		return;
	}
	PostMessage(WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
	return;
}

void CProcJobCompleteDlg::OnLpFoupcloseok() {
	//m_Message += "\nClose foup command OK";
	Message("Close carrier command OK", 5);
	//UpdateData(FALSE);
}

void CProcJobCompleteDlg::OnLpFoupclosefail() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose foup command fail";
	Message("Close carrier command fail", 0);
	//UpdateData(FALSE);
	bOK = FALSE;
	//m_cProceed.ShowWindow(SW_SHOW);
}

void CProcJobCompleteDlg::OnComAbortclose() {
	KillTimer(IDT_CLOSETIMEOUT);
	//m_Message += "\nClose foup aborted";
	Message("Close carrier aborted", 0);
	//UpdateData(FALSE);
	bOK = FALSE;
	//m_cProceed.ShowWindow(SW_SHOW);
}

void CProcJobCompleteDlg::OnComEndClose() {
	KillTimer(IDT_CLOSETIMEOUT);

	//m_Message += "\nClose foup complete";
	Message("Close carrier complete", 5);

	//m_Message += "\nRemove this carrier";
	//UpdateData(FALSE);

	m_cUnload.EnableWindow(TRUE);
}

long CProcJobCompleteDlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	if (!m_Parent) {
		ASSERT(0);
		return -1;
	}
	char NSlotMap[MAXCAPACITYLEN + 1];
	strscpy(NSlotMap, MAXCAPACITYLEN, (char*)(lP));
	NSlotMap[MAXCAPACITYLEN] = 0;
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
	NSlotMap[MAXCAPACITYLEN] = 0;   // slotmap now starts from 0 offset, change from lP+1 to lP [11/14/2007 YUEN]
	if (lP) {
		strscpy(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, MAXCAPACITYLEN, (char*)(lP));
	}
	else {
		memset(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, '0', MAXCAPACITYLEN);
	}
	return 0;
}

void CProcJobCompleteDlg::OnComClosenoreleasecmpl() {
	KillTimer(IDT_CLOSETIMEOUT);

	//m_Message += "\nClose foup complete";
	Message("Close carrier completed", 5);

	pGDev->pOLnClt->GEMEventFoupClosed(ID + 1);

	pGDev->SetLPState(ID + 1, PrcsMgrServices::UNDOCKED);
	pGDev->pOLnClt->GEMEventFoupUndocked(ID + 1);

	//	pGDev->SetLPState(ID+1,PrcsMgrServices::UNCLAMPPED);
	//	pGDev->pOLnClt->GEMEventFoupUnclamped(ID+1);

	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::MANUAL) {
		EnableButtonReport();
	}
	else {
		//m_Message += "\nWaiting for carrier to leave equipment";
		Message("Waiting for carrier to leave equipment", 5);
	}
	//UpdateData(FALSE);
}

void CProcJobCompleteDlg::EnableButtonReport() {
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
				Message("\nHit 'load/unload' to unclamp this carrier", 5);
				m_cUnload.EnableWindow(TRUE);
			}
		}
	}
}

void CProcJobCompleteDlg::opProceed() {
	if (m_Parent) {
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBUNLOADING, ID, 0);
	}
}

// come here when foup fail to close or message fail to reach LP
void CProcJobCompleteDlg::opAbort() {
	// ToBeCompleted
	// Raise alarm etc
}

long CProcJobCompleteDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	//	Invalidate(FALSE);
	return 0;
}

void CProcJobCompleteDlg::OnComButtonpushed() {
	if (m_Parent) {
		Unclamp();	// Do unclamp if it is clamped
//		m_Parent->PostMessage(UM_TO_PROCJOBUNLOADCOMPLETE,ID,0); // else just exit
	}
	else {
		ASSERT(0);
	}
}

void CProcJobCompleteDlg::Unclamp() {
	//m_Message += "\nUnclamping carrier";
//	UpdateData(FALSE);
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
			SetTimer(IDT_UNLATCHTIMEOUT, UNLATCHFOUPTIMEOUT + 2000, NULL);
			return;
		}
		//m_Message += "Unclamp foup send fail";
		//UpdateData(FALSE);
		Message("Unclamp carrier send fail", 0);
		PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
		return;
	}
	Message("LPMgr not found", 0);
	PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
}

void CProcJobCompleteDlg::OnLpUnclampok() {
	Message("Unclamp carrier command send OK", 5);
}

void CProcJobCompleteDlg::OnLpUnclampfail() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	//m_Message += "\nUnclamp command send fail";
	//UpdateData(FALSE);
	Message("Unclamp carrier command send fail", 0);
}

void CProcJobCompleteDlg::OnComAbortaphdrelease() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	//m_Message += "\nUnclamp command aborted";
	//UpdateData(FALSE);
	Message("Unclamp carrier command aborted", 0);
}

void CProcJobCompleteDlg::OnComReleasecmpl() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	//m_Message += "\nCarrier unclamped";
	//UpdateData(FALSE);
	Message("Carrier unclamped", 5);
	bOK = TRUE;
	//	OnProceed();
}

void CProcJobCompleteDlg::OnComCarrierremoved() {
	Message("Material removed", 5);

	CString CarrierID = "";
	pGDev->pGEMObjs->SetLPAssociation(CarrierID, ID + 1, CLPObj::NOTASSOCIATED);
	pGDev->pOLnClt->GEMEventMaterialRemoved(ID + 1);
	pGDev->SetLPState(ID + 1, PrcsMgrServices::NOFOUP);

	Message("Carrier has left the tool", 5);

	if (NoOtherProcJobRuning()) {
		pGDev->SetARAMSState(SArams::STANDBY);
	}

	ShowWindow(SW_HIDE);

	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::MANUAL) {
		SetTimer(IDT_CLOSEANDQUIT, 100, NULL);
	}
}

void CProcJobCompleteDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobCompleteDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobCompleteDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}

void CProcJobCompleteDlg::OnUnload() {
	OnComCarrierremoved();
}

void CProcJobCompleteDlg::OnUnloadComplete() {
	SetTimer(IDT_CLOSEANDQUIT, 100, NULL);
}

BOOL CProcJobCompleteDlg::NoOtherProcJobRuning() {
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

void CProcJobCompleteDlg::OnComNovacuum() {
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

void CProcJobCompleteDlg::OnComWaferout() {
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

void CProcJobCompleteDlg::OnComVacuumlost() {
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

void CProcJobCompleteDlg::OnComSensortrip() {
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

LRESULT CProcJobCompleteDlg::OnComTp1timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComTp2timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComTp3timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComTp4timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComTp5timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComTp6timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComLoste84signal(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobCompleteDlg::OnComUnexpectede84signal(WPARAM wP, LPARAM lP) {
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

// TP error implies that load port is in AUTO mode [3/16/2012 Administrator]
int CProcJobCompleteDlg::TPErrorRecovery(short errCode, BOOL bCarrierPresent) {
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

void CProcJobCompleteDlg::Message(CString msg, short lvl) {
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

void CProcJobCompleteDlg::Refresh() {
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