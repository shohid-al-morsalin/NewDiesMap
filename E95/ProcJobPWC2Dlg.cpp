#include "stdafx.h"
#include "strSOp.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IPC/SMCHdr.h"
#include "SRC/DOSUtil.h"
#include "SYS/CUMMsg.h"

#include "ProcJobPWC2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_START		9999
#define IDT_OPENTIMEOUT	9998

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWC2Dlg dialog

CProcJobPWC2Dlg::CProcJobPWC2Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobPWC2Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobPWC2Dlg)
	m_Message = _T("");
	m_ContentMap = _T("");
	//}}AFX_DATA_INIT

	ID = 0;
	m_Parent = NULL;

	Step = 0;
}

CProcJobPWC2Dlg::~CProcJobPWC2Dlg() {}

void CProcJobPWC2Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobPWC2Dlg)
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_CONTENTMAP, m_ContentMap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobPWC2Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobPWC2Dlg)
	ON_WM_TIMER()
	ON_COMMAND(ID_LP_FOUPOPENOK, OnLpFoupopenok)
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_COMMAND(ID_COM_ABORTOPEN, OnComAbortopen)
	ON_COMMAND(ID_COM_EXITHOME, OnComExithome)
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
	ON_BN_CLICKED(IDC_CANCELCARRIER, OnCancelcarrier)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SVR_PROCEEDWITHCARRIERSM, OnSvrProceedwithcarriersm)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotMapFail)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(ID_SVR_CANCELCARRIER, OnSvrCancelcarrier)
	ON_MESSAGE(ID_SVR_CANCELCARRIERATPORT, OnSvrCancelcarrieratport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWC2Dlg message handlers

BOOL CProcJobPWC2Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	if ((ID < 0) || (ID > 3)) {
		ASSERT(0);
		return FALSE;
	}
	if (!m_Parent || !pGDev || !pUDev) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_MESSAGE, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_CONTENTMAP, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_CANCELCARRIER, BOTTOM_RIGHT);

	RegisterWithSecsSvr(this, ID);

	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];

	m_CarrierID = m_ContentMap = pInfo->Cas.JobInfo.CarrierID;
	m_ContentMap += "\n";
	for (int i = 1; i <= 25; i++) {
		CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(i);
		if (pWI) {
			m_ContentMap += pWI->LotID;
			m_ContentMap += " ";
			m_ContentMap += pWI->WaferID;
			m_ContentMap += "\n";
		}
	}

	SetTimer(IDT_START, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobPWC2Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		OpenFoup();
	}
	if (nIDEvent == IDT_OPENTIMEOUT) {
		KillTimer(IDT_OPENTIMEOUT);
		Log("Open Foup Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}
}

void CProcJobPWC2Dlg::OpenFoup() {
	m_Message = "Docking foup";
	Log("Docking foup", 5);
	UpdateData(FALSE);

	// Open Foup
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_FOUPOPENOK;
	SM.Nak = ID_LP_FOUPOPENFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::OPENFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_OPENTIMEOUT, OPENFOUPTIMEOUT + 8000, NULL);
			return;
		}
		Log("Open foup command send fail", 2);
		PostMessage(WM_COMMAND, SM.Nak, NULL);
		return;
	}
	Log("LPMgr not found", 2);
	PostMessage(WM_COMMAND, SM.Nak, NULL);
}

void CProcJobPWC2Dlg::OnLpFoupopenok() {
	Log("Open foup command accepted", 5);
}

void CProcJobPWC2Dlg::OnComAbortopen() {
	OnLpFoupopenfail();
}

void CProcJobPWC2Dlg::OnLpFoupopenfail() {
	KillTimer(IDT_OPENTIMEOUT);

	CString msg;
	// Note: Do not change the wording below, decoded elsewhere
	CString str;
	str.LoadString(IDS_FOUPOPENFAILED);
	msg.Format("%s (%d)", str, ID + 1);
	Log(msg, 2);

	UpdateData();
	pGDev->SetMessage(msg);
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
}

void CProcJobPWC2Dlg::OnComEndopen() {
	KillTimer(IDT_OPENTIMEOUT);

	m_Message += "\nOpen foup complete";
	Log("Open foup complete", 5);
	pGDev->pGEMObjs->Cms[ID].LPObj.CarrierLoc = CLPObj::clDOCKED;

	UpdateData(FALSE);
}

long CProcJobPWC2Dlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	CString str = (char*)(lP);
	if ((str.Find('2') != -1) ||
		(str.Find('4') != -1) ||
		(str.Find('5') != -1)) {
		OnComSlotMapFail(wP, lP);
		return 0;
	}

	CCarrierObj* pCObj = &pGDev->pGEMObjs->Cms[ID].CaObj;
	strscpy(pCObj->SlotMaP, MAXCAPACITYLEN, str);
	pCObj->SlotMaP[MAXCAPACITYLEN] = 0; // Make it a proper string [3/19/2012 Yuen]
	//	pUDev->pProcJobInfo[ID].SlotMap[0] = '3'; // FOR TESTING ONLY
	//	pUDev->pProcJobInfo[ID].SlotMap[1] = '3'; // FOR TESTING ONLY
	//	pUDev->pProcJobInfo[ID].SlotMap[2] = '3'; // FOR TESTING ONLY

	m_Message += "\nSlot map read\n";
	m_Message += pCObj->SlotMaP;
	Log("Slot map read", 5);

	DosUtil.RememberSM(ID + 1, pCObj->SlotMaP);
	SetContentMap();
	SlotmapVerification();

	return 0;
}

long CProcJobPWC2Dlg::OnComSlotMapFail(WPARAM wP, LPARAM lP) {
	CString str;
	str.LoadString(IDS_SLOTMAPREADFAILED);
	m_Message += "\n" + str;

	Log(str, 2);
	UpdateData(FALSE);

	// 	if (!pUDev->pProcJobInfo[ID].bRemoteOverride)
	// 	{
	if (pGDev->pGEMObjs->IsOnline()) {
		pGDev->pOLnClt->GEMEventSlotmapReadFail(ID + 1);
	}
	// 	}

	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
		// 		if (!pUDev->pProcJobInfo[ID].bRemoteOverride)
		// 		{
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		m_Message += "\nWaiting for host";
		UpdateData(FALSE);
		break;
		// 		}
				// No break intended [9/12/2007 LYF]
	default:
		UpdateData();
		// Note: Do not change the working
		pGDev->SetMessage(str);
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
		break;
	}
	return 0;
}

void CProcJobPWC2Dlg::OnComExithome() {
	m_Message += "\nDock complete\nOpen foup";
	Log("Dock complete", 5);
	Log("Opening foup", 5);
	UpdateData(FALSE);
}

long CProcJobPWC2Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CProcJobPWC2Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobPWC2Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

CInfoPanelMain* CProcJobPWC2Dlg::GetParentWnd() {
	return m_Parent;
}

void CProcJobPWC2Dlg::OnComNovacuum() {
	//Message("No vacuum error",1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1NOVACUUM);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2NOVACUUM);
		break;
	}
	CancelAndQuit();
}

void CProcJobPWC2Dlg::OnComWaferout() {
	//Message("Wafer protrusion error",1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1EXCESSIVEWAFEROUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2EXCESSIVEWAFEROUT);
		break;
	}
	CancelAndQuit();
}

void CProcJobPWC2Dlg::OnComVacuumlost() {
	//Message("Vacuum lost error",1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1VACUUMLOST);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2VACUUMLOST);
		break;
	}
	CancelAndQuit();
}

void CProcJobPWC2Dlg::OnComSensortrip() {
	//Message("Vacuum lost error",1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1PERISENSORTRIPED);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2PERISENSORTRIPED);
		break;
	}
	CancelAndQuit();
}

LRESULT CProcJobPWC2Dlg::OnComTp1timeout(WPARAM wP, LPARAM lP) {
	//Message("TP1 timeout",1);
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

LRESULT CProcJobPWC2Dlg::OnComTp2timeout(WPARAM wP, LPARAM lP) {
	//Message("TP2 time out",1);
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

LRESULT CProcJobPWC2Dlg::OnComTp3timeout(WPARAM wP, LPARAM lP) {
	//Message("TP3 time out",1);
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

LRESULT CProcJobPWC2Dlg::OnComTp4timeout(WPARAM wP, LPARAM lP) {
	//Message("TP4 time out",1);
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

LRESULT CProcJobPWC2Dlg::OnComTp5timeout(WPARAM wP, LPARAM lP) {
	//Message("TP5 time out",1);
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

LRESULT CProcJobPWC2Dlg::OnComTp6timeout(WPARAM wP, LPARAM lP) {
	//Message("TP6 time out",1);
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

LRESULT CProcJobPWC2Dlg::OnComLoste84signal(WPARAM wP, LPARAM lP) {
	//Message("Lost E84 signal",1);
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

LRESULT CProcJobPWC2Dlg::OnComUnexpectede84signal(WPARAM wP, LPARAM lP) {
	//Message("Unexpected E84 signal",1);
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
int CProcJobPWC2Dlg::TPErrorRecovery(short errCode, BOOL bCarrierPresent) {
	// 	CString msg;
	// 	msg.Format("TPRecovery (LP%d) carrier:%d",ID+1,bCarrierPresent);
	// 	Message(msg,1);

	return 0;
}

void CProcJobPWC2Dlg::CancelAndQuit() {
	UpdateData();
	pGDev->SetMessage(/*"Hit cancel to continue loading"*/"");
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_TO_PROCJOBPWC2);
}

void CProcJobPWC2Dlg::SlotmapVerification() {
	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
		// 		if (!pUDev->pProcJobInfo[ID].bRemoteOverride)
		// 		{
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		m_Message += "\nWaiting for host";
		UpdateData(FALSE);
		break;
		// 		}
				// no break intended [9/12/2007 LYF]
	default:
		pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, CCarrierObj::SMVERIFICATIONOK);
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBPWC1, ID, (long)pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		break;
	}
}

long CProcJobPWC2Dlg::OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP) {
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	if (lP) {
		// pSM.CoS contains data
		CSMOL* pSM = (CSMOL*)lP;
		pInfo->Cas.JobInfo.CarrierID = pSM->CoS.Co.ObjID;
		for (int i = 0; i < pSM->CoS.Co.Capacity; i++) {
			if (pSM->CoS.Co.SlotMaP[i] == CSubstObj::SM_CORRECTLY_OCCUPIED) {
				pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP[i] = '3';
			}
			else {
				pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP[i] = '1';
			}
		}
		delete pSM;
	}
	// 	if (!pUDev->pProcJobInfo[ID].bRemoteOverride)
	// 	{
	if (pGDev->pGEMObjs->IsOnline()) {
		pGDev->pGEMObjs->SetCOSlotmapStatus(
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID, ID + 1,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, CCarrierObj::SMVERIFICATIONOK);
	}
	// 	}

	int type = 3; // for automation mode
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBQUEUED, ID, type);
	return 0;
}

void CProcJobPWC2Dlg::OnCancelcarrier() {
	if (AfxMessageBox("Are you sure?", MB_YESNO) == IDYES) {
		UpdateData();
		pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_TO_PROCJOBPWC2);
	}
}
long CProcJobPWC2Dlg::OnSvrCancelcarrier(WPARAM wP, LPARAM lP) {
	CSMOL* pSM = (CSMOL*)lP;
	if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) /*|| (pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
		Log("Cancel Carrier Executed outside REMOTE mode", 2);
		if (pSM) {
			delete pSM;
		}
		return -1;
	}
	// LYF Verify carrier id is identical [8/16/2007 LYF]
	// pSM.CoS contains data
	if (!pSM) {
		Log("OnSvrCancelcarrier error", 2);
		return -2;
	}

	if (pSM->CoS.Co.ObjID != m_CarrierID) {
		Log("Carrier ID missmatch", 2);
		if (pSM) {
			delete pSM;
		}
		return -3;
	}

	UpdateData();
	pGDev->SetMessage("Cancel Carrier");
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
	delete pSM;
	return 0;
}

long CProcJobPWC2Dlg::OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP) {
	return OnSvrCancelcarrier(wP, lP);
}

void CProcJobPWC2Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}