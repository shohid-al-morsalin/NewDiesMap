#include "stdafx.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "128/GEM/PrJobList.h"
#include "FEM/Ldp/LdPTOut.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/Smol.h"
#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ProcJob413ProcessingDlg.h"
#include "SRC/DOSUtil.h"
#include "SYS/CUMMsg.h"
#include "SYS/Tool.h"

#include "ProcJobSMICDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MAXLOGITEMS			2048

#define IDT_REGISTERWIN			9999
#define IDT_LATCHTIMEOUT		9998
#define IDT_AREYOUTHERETIMEOUT	9994
#define IDT_CANSTART			9995
#define IDT_OPENTIMEOUT			9996
#define IDT_CJMON				9992
#define IDT_SLOTMAPTIMEOUT		9991
#define IDT_SMWAITFORHOSTTOUT	9989
#define IDT_PJWAITTIMEOUT		9988
#define IDT_REFRESH				9987

/////////////////////////////////////////////////////////////////////////////
// CProcJobSMICDlg dialog

CProcJobSMICDlg::CProcJobSMICDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobSMICDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobSMICDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CProcJobSMICDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobSMICDlg)
	DDX_Control(pDX, IDC_LOAD, m_cLoad);
	DDX_Control(pDX, IDC_PROCEED, m_cProceed);
	DDX_Control(pDX, IDC_LIST1, m_cList);
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

BEGIN_MESSAGE_MAP(CProcJobSMICDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobSMICDlg)
	ON_COMMAND(ID_REGISTERLP, RegisterLP)
	ON_WM_TIMER()
	ON_COMMAND(ID_COM_CARRIERARRIVED, OnComCarrierarrived)
	ON_COMMAND(ID_COM_ABORTAPHDLATCH, OnComAbortaphdlatch)
	ON_COMMAND(ID_COM_LATCHCMPL, OnLatchCmpl)
	ON_MESSAGE(ID_SVR_PROCEEDWITHCARRIERID, OnSvrProceedwithcarrierid)
	ON_COMMAND(ID_LP_FOUPOPENOK, OnLpFoupopenok)
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	ON_COMMAND(ID_COM_ABORTOPEN, OnComAbortopen)
	ON_COMMAND(ID_COM_EXITHOME, OnComExithome)
	ON_COMMAND(ID_COM_LATCHFAIL, OnComLatchfail)
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotmap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotmapFail)
	ON_MESSAGE(ID_SVR_PROCEEDWITHCARRIERSM, OnSvrProceedwithcarriersm)
	ON_MESSAGE(ID_SVR_PJDEFINE, OnSvrPjdefine)
	ON_MESSAGE(ID_SVR_CJDEFINE, OnSvrCjdefine)
	ON_COMMAND(ID_IPC_HELLOACK, OnIpcHelloack)
	ON_MESSAGE(ID_IPC_DATAREQACK1, OnIpcDatareqack)
	ON_BN_CLICKED(IDC_PROCEED, OnProceed)
	ON_COMMAND(ID_COM_CARRIERREMOVED, OnComCarrierremoved)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobSMICDlg message handlers

BOOL CProcJobSMICDlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CProcJobSMICDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CProcJobSMICDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	// Sanity check [2/12/2012 Administrator]
	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return FALSE;
	}

	if (!pGDev || !pUDev || !m_Parent) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_LOAD, BOTTOM_RIGHT);
	AddAnchor(IDC_PROCEED, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	// Wait till GDev fully initialized during startup
	SetTimer(IDT_REGISTERWIN, 1000, NULL);
	SetTimer(IDT_REFRESH, 3000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobSMICDlg::SetDataReqTimeOut(long dur) {
	// Should not call [5/14/2012 Yuen]
	ASSERT(0);
}

int CProcJobSMICDlg::GetHWnd() {
	return (int)m_hWnd;
}

void CProcJobSMICDlg::RegisterLP() {
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);
	pGDev->RegisterWinWithIDRMgr(m_hWnd, ID + 1);
}

void CProcJobSMICDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		Refresh();
		SetTimer(IDT_REFRESH, 1000, NULL);
	}
	else if (nIDEvent == IDT_LATCHTIMEOUT) {
		KillTimer(IDT_LATCHTIMEOUT);
		Message("Latch FOUP Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_LATCHFAILED, NULL);
	}
	else if (nIDEvent == IDT_OPENTIMEOUT) {
		KillTimer(IDT_OPENTIMEOUT);
		Message("Open FOUP Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}
	else if (nIDEvent == IDT_SLOTMAPTIMEOUT) {
		KillTimer(IDT_SLOTMAPTIMEOUT);
		Message("Slot map Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}
	else if (nIDEvent == IDT_SMWAITFORHOSTTOUT) {
		KillTimer(IDT_SMWAITFORHOSTTOUT);
		Message("Slot map waiting for host Timeout", 2);
		SMWaitingForHostTimeout();
	}
	else if (nIDEvent == IDT_PJWAITTIMEOUT) {
		KillTimer(IDT_PJWAITTIMEOUT);
		Message("Waiting for PJ timeout", 2);
		WaitingForPJTimeOut();
	}
	else if (nIDEvent == IDT_CJMON) {
		KillTimer(IDT_CJMON);
		switch (DoCJMonitor()) {
		case pjcCANSTART:
			SetTimer(IDT_CANSTART, 200, NULL);
			break;
		case pjcCJMON:
			SetTimer(IDT_CJMON, 1000, NULL);
			break;
		case pjcERR:
			CancelAndQuit();
			break;
		}
	}
	else if (nIDEvent == IDT_CANSTART) {
		KillTimer(IDT_CANSTART);
		switch (CanStart()) {
		case OKTOSTART:
			Message("Resource available", 5);
			Message("Contacting 413 unit", 5);
			if (AreYouThere()) {
				SetTimer(IDT_AREYOUTHERETIMEOUT, 3000, NULL);
			}
			else {
				Message("Could not contacting 413 unit", 5);
				OnCancel();
			}
			break;
		case NOTREADY:
			SetTimer(IDT_CANSTART, 500, NULL);
			break;
		case ABORT:
			OnCancel();
			break;
		}
	}
	if (nIDEvent == IDT_AREYOUTHERETIMEOUT) {
		KillTimer(IDT_AREYOUTHERETIMEOUT);
		CString str;
		str = "413 unit fail to response";
		Message(str, 0);
		if (AreYouThereRetryCount < 12) {
			SetTimer(IDT_CANSTART, 5000, NULL);
		}
		else {
			Message("413 Unit not online", 5);
			pGDev->SetAlarm(CAlrmList::C413NOTONLINE);
			Message("Abort retry", 5);
			Message("PLEASE DEQUE", 5);
		}
		AreYouThereRetryCount++;
	}
	else if (nIDEvent == IDT_REGISTERWIN) {
		if (!pGDev) {
			// Try again later
			return;
		}
		if (pGDev->MgrStatus.LpMgr < CMgrStatus::STARTOK) {
			// Try again later
			return;
		}
		if (pGDev->MgrStatus.IDRMgr < CMgrStatus::STARTOK) {
			// Try again later
			return;
		}

		KillTimer(IDT_REGISTERWIN);

		RegisterWithSecsSvr(this, ID);

		// Connect LPSvr and this
		if ((ID > -1) && (ID < MAX_LPSUPPORTED)) {
			// ID is zero offset
			// Connect LPSvr and this
			RegisterLP();
		}
		else {
			ASSERT(0);
			return;
		}
		SetupLPStatus();
	}
}

void CProcJobSMICDlg::Refresh() {
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

void CProcJobSMICDlg::SetupLPStatus() {}

void CProcJobSMICDlg::Message(CString msg, short lvl) {
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

void CProcJobSMICDlg::OnComCarrierarrived() {
	SetMaterialRecieved();

	if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) {
		switch (OpenFOUP(m_hWnd, ID_LP_FOUPOPENOK, ID_LP_FOUPOPENFAIL)) {
		case ofOK:
			SetTimer(IDT_SLOTMAPTIMEOUT, OPENFOUPTIMEOUT + 2000, NULL);
			SetTimer(IDT_OPENTIMEOUT, OPENFOUPTIMEOUT + 8000, NULL);
			break;
		case ofFAIL:
		case ofOBJERR:
			PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
			break;
		}
	}
}

void CProcJobSMICDlg::OnComCarrierremoved() {
	SetMaterialRemoved();
}

void CProcJobSMICDlg::LatchFoup() {
	switch (LatchFOUP(m_hWnd, ID_LP_LATCHOK, ID_LP_LATCHFAILED)) {
	case ofOK:
		SetTimer(IDT_LATCHTIMEOUT, LATCHFOUPTIMEOUT, NULL);
		break;
	case ofFAIL:
	case ofOBJERR:
		PostMessage(WM_COMMAND, ID_LP_LATCHFAILED, NULL);
		break;
	}
}

void CProcJobSMICDlg::opAbortAphdLatch() {
	KillTimer(IDT_LATCHTIMEOUT);

	SetLatchFail();

	AlarmAndCancelCarrier();
}

void CProcJobSMICDlg::OnLatchCmpl() {
	KillTimer(IDT_LATCHTIMEOUT);

	SetLatchCmpl();

	pGDev->pGEMObjs->SetLPAssociation(m_CarrierID, ID + 1, CLPObj::ASSOCIATED);
	pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDNOTREAD);
	pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDWAITINGFORHOST);
	pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, "", CCarrierObj::SMNOTREAD);
}

void CProcJobSMICDlg::OnComLatchfail() {
	Message("Latch FOUP aborted", 5);
	opAbortAphdLatch();
}

void CProcJobSMICDlg::OnComAbortaphdlatch() {
	Message("Latch FOUP aborted", 5);
	opAbortAphdLatch();
}

void CProcJobSMICDlg::AlarmAndCancelCarrier() {
	// Do something here like setting alarm and rejecting FOUP
	switch (ID) {
	case 0:
		pGDev->SetAlarm(CAlrmList::LP1OPENFAILURE);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2OPENFAILURE);
		break;
	}
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

LRESULT  CProcJobSMICDlg::OnSvrProceedwithcarrierid(WPARAM wP, LPARAM lP) {
	Message("Proceed with carrier (ID)", 5);
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	if (lP) {
		CSMOL* pSM = (CSMOL*)lP;
		pInfo->Cas.JobInfo.CarrierID = pSM->CoS.Co.ObjID;
		for (int i = 0; i < pSM->CoS.Co.Capacity; i++) {
			pInfo->Cas.winfo.SetContentMap(i + 1, pSM->CoS.Co.CtMap[i]);
		}
		delete pSM;
	}
	pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);

	ShowContentMap();

	switch (OpenFOUP(m_hWnd, ID_LP_FOUPOPENOK, ID_LP_FOUPOPENFAIL)) {
	case ofOK:
		SetTimer(IDT_SLOTMAPTIMEOUT, OPENFOUPTIMEOUT + 2000, NULL);
		SetTimer(IDT_OPENTIMEOUT, OPENFOUPTIMEOUT + 8000, NULL);
		break;
	case ofFAIL:
	case ofOBJERR:
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
		break;
	}

	return 0;
}

void CProcJobSMICDlg::OnLpFoupopenok() {}

void CProcJobSMICDlg::OnLpFoupopenfail() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);

	CString msg;
	// Note: Do not change the wording below, decoded elsewhere
	CString str;
	str.LoadString(IDS_FOUPOPENFAILED);

	Message(str, 0);

	AlarmAndCancelCarrier();
}

void CProcJobSMICDlg::OnComAbortopen() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);
	OnLpFoupopenfail();
}

void CProcJobSMICDlg::OnComExithome() {
	Message("complete", 5);
	Message("Open FOUP", 5);
	Message("Dock complete", 5);
	Message("Opening FOUP", 5);
	UpdateData(FALSE);
}

void CProcJobSMICDlg::OnComEndopen() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);

	Message("Open FOUP complete", 5);
	pGDev->pGEMObjs->Cms[ID].LPObj.CarrierLoc = CLPObj::clDOCKED;

	UpdateData(FALSE);
}

LRESULT CProcJobSMICDlg::OnComSlotmap(WPARAM wP, LPARAM lP) {
	CString str = (char*)(lP);
	if ((str.Find('2') != -1) ||
		(str.Find('4') != -1) ||
		(str.Find('5') != -1)) {
		OnComSlotmapFail(wP, lP);
		return 0;
	}

	strcpy(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, str);

	Message("Slot map read", 5);
	CString SltMp = pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP;
	Message(SltMp, 5);

	DosUtil.RememberSM(ID + 1, SltMp);

	SetContentMap();

	SlotmapVerification();

	return 0;
}

LRESULT CProcJobSMICDlg::OnComSlotmapFail(WPARAM wP, LPARAM lP) {
	CString str;
	str.LoadString(IDS_SLOTMAPREADFAILED);

	Message(str, 5);
	UpdateData(FALSE);

	if (pGDev->pGEMObjs->IsOnline()) {
		pGDev->pOLnClt->GEMEventSlotmapReadFail(ID + 1);
	}

	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		Message("SM Waiting for host", 5);
		UpdateData(FALSE);
		break;
		// No break intended [9/12/2007 LYF]
	default:
		UpdateData();
		// Note: Do not change the working
		pGDev->SetMessage(str);
		ShowWindow(SW_HIDE);
		ProceedToCancelCarrier();
		break;
	}
	return 0;
}

void CProcJobSMICDlg::SlotmapVerification() {
	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
	{
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		int v = 120000;
		CString msgstr;
		msgstr.Format("SM Waiting for host (TOUT: %d secs)", v / 1000);
		Message(msgstr, 5);
		UpdateData(FALSE);
		SetTimer(IDT_SMWAITFORHOSTTOUT, v, NULL);
		break;
	}
	// no break intended [9/12/2007 LYF]
	default:
		pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, CCarrierObj::SMVERIFICATIONOK);
		ShowWindow(SW_HIDE);
		m_Parent->PostMessage(UM_TO_PROCJOBPWC1, ID, (LPARAM)pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		break;
	}
}

LRESULT CProcJobSMICDlg::OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_SMWAITFORHOSTTOUT);

	SetSMVerificationOK();
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	if (lP) {
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
	SetSMVerificationOK();
	ProceedProcessJobQueue();
	return 0;
}

void CProcJobSMICDlg::SMWaitingForHostTimeout() {
	CString msgstr;
	msgstr.Format("Slot map waiting for host timeout: %s ", m_CarrierID);
	Message(msgstr, 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1SMWAITFORHOSTTIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2SMWAITFORHOSTTIMEOUT);
		break;
	}
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

void CProcJobSMICDlg::ProceedProcessJobQueue() {
	m_CarrierID = pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID;

	srID = ID;
	srParent = m_Parent;

	Tool.SetCasStateQueued(ID + 1);

	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE) /*&& (!pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
		m_cProceed.EnableWindow(FALSE);
		//	m_cCancel.EnableWindow(FALSE);
		Message("Waiting to start", 5);
		int v = 120000;
		CString msgstr;
		msgstr.Format("Waiting for PJob and CJob (TOUT: %d secs)", v / 1000);
		Message(msgstr, 5);
		SetTimer(IDT_PJWAITTIMEOUT, v, NULL);
		SetTimer(IDT_CJMON, 50, NULL);	// Look for CJ that has the correct carrier ID and port ID assignment
	}
	else {
		Message("Waiting for resources", 5);
		SetTimer(IDT_CANSTART, 50, NULL);
	}
}

LRESULT CProcJobSMICDlg::OnSvrCjdefine(WPARAM wP, LPARAM lP) {
	if (!lP) {
		return -1;
	}
	CSMOL* pSM = (CSMOL*)lP;

	CCtrlJob* pJ = CJobMgr.GetCJob(pSM->Pj.PrJID);
	if (pJ) {
		Message(pSM->Pj.PrJID, 5);
		CString m_Status;
		for (int n = 0; n < pJ->CarrierInputSpec.GetSize(); n++) {
			m_Status = "Carriers : <";
			m_Status += pJ->CarrierInputSpec.GetAt(n);
			m_Status += "> ";
			Message(m_Status, 5);
		}
	}
	delete pSM;

	UpdateData(FALSE);

	return 0;
}

LRESULT CProcJobSMICDlg::OnSvrPjdefine(WPARAM wP, LPARAM lP) {
	int i;

	KillTimer(IDT_PJWAITTIMEOUT);

	if (!lP) {
		return -1;
	}
	CSMOL* pSM = (CSMOL*)lP;

	CString m_Status;
	m_Status = pSM->Pj.PrJID;
	m_Status += ": <";
	m_Status += pSM->Pj.CarrierID;
	m_Status += ">";
	Message(m_Status, 5);

	m_Status = "Recipe: ";
	m_Status += pSM->Pj.RecipeName;
	Message(m_Status, 5);

	Message("Slots:", 5);
	for (i = 0; i < MAXCAPACITYLEN; i++) {
		SlotInfo* s = &pSM->Pj.slot[i];
		short slotno = s->slotno;
		if (slotno != -1) {
			CString str;
			str.Format("%d", slotno);
			Message(str, 5);
		}
		else {
			break;
		}
	}

	SPcJobExec* p = new SPcJobExec;
	*p = pSM->Pj;
	PrJobs.Add(p);

	delete pSM; // Must delete because it is created using new operator

	UpdateData(FALSE);

	return 0;
}

void CProcJobSMICDlg::WaitingForPJTimeOut() {
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1PJWAITTIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2PJWAITTIMEOUT);
		break;
	}
	CancelAndQuit();
}

void CProcJobSMICDlg::CancelAndQuit() {
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

BOOL CProcJobSMICDlg::AreYouThere() {
	// bypass distributed implementation [8/5/2010 XPMUser]
	PostMessage(WM_COMMAND, ID_IPC_HELLOACK, 0);
	return TRUE;
}

void CProcJobSMICDlg::OnIpcHelloack() {
	KillTimer(IDT_AREYOUTHERETIMEOUT);
	StartProcessRequest();
}

void CProcJobSMICDlg::StartProcessRequest() {
	PostMessage(ID_IPC_DATAREQACK1, 0, NULL);
	return;
}

LRESULT CProcJobSMICDlg::OnIpcDatareqack(WPARAM wP, LPARAM lP) {
	OnProceed();  // allowed to continue [8/5/2010 XPMUser]
	return 0;
}

void CProcJobSMICDlg::OnProceed() {
	ShowWindow(SW_HIDE);
	type = 3;
	ProceedToProcessing();
}

void CProcJobSMICDlg::OnLoad() {
	m_cLoad.EnableWindow(FALSE);
	OnComCarrierarrived();
}