// ProcJobDlg.cpp : implementation file
//

#include "stdafx.h"
#include "strSOp.h"
#include "E95.h"

#include "IPC/Smol.h"
#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "128/GEM/PrJobList.h"
#include "413App.h"
#include "FEM/IDR/InputCarrierID.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IncmplPrcsHdlr.h"
#include "InfoPanelMain.h"
#include "IPC/SLPInit.h"
#include "IPC/SMCHdr.h"
#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "SYS/Tool.h"
#include "TitlePanel.h"
#include "NavigationPanel.h"
#include "ProcJobDlg.h"

#define  MAXLOGITEMS			2048

#define IDT_REGISTERWIN			9999
#define IDT_LATCHTIMEOUT		9998
#define IDT_OPENTIMEOUT			9996
#define IDT_CANSTART			9995
#define IDT_AREYOUTHERETIMEOUT	9994
#define IDT_DATAREQ_TIMEOUT		9993
#define IDT_CJMON				9992
#define IDT_SLOTMAPTIMEOUT		9991
#define IDT_IDWAITFORHOSTTOUT	9990
#define IDT_SMWAITFORHOSTTOUT	9989
#define IDT_PJWAITTIMEOUT		9988
#define IDT_REFRESH				9987

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcJobDlg dialog

// TODO:
// Need to process perimeter trip and etc.

CProcJobDlg::CProcJobDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobDlg)
	m_RecipeName = _T("");
	//}}AFX_DATA_INIT

	pXTab = NULL;
	bIncomAlrm = FALSE;
}

CProcJobDlg::~CProcJobDlg() {}

void CProcJobDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobDlg)
	DDX_Control(pDX, IDC_RECOVER, m_cRecover);
	DDX_Control(pDX, IDC_LOAD, m_cLoad);
	DDX_Control(pDX, IDC_ABORT, m_cAbort);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_LOADPOS, m_cLoadPos);
	DDX_Control(pDX, IDC_BROWSE, m_cIcon);
	DDX_Control(pDX, IDC_PROCEED, m_cProceed);
	DDX_Control(pDX, IDC_CANCEL, m_cCancel);
	DDX_Control(pDX, IDC_CARRIERID, m_cCarrierID);
	DDX_Text(pDX, IDC_RECIPENAME, m_RecipeName);
	DDX_Text(pDX, IDC_CARRIERID, m_CarrierID);
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

BEGIN_MESSAGE_MAP(CProcJobDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobDlg)
	ON_WM_TIMER()
	ON_COMMAND(ID_COM_LOADCOMPLETED, OnComLoadcompleted)
	ON_COMMAND(ID_COM_CARRIERARRIVED, OnComCarrierarrived)
	ON_COMMAND(ID_COM_BUTTONPUSHED, OnComButtonpushed)
	ON_COMMAND(ID_COM_CARRIERREMOVED, OnComCarrierremoved)
	ON_COMMAND(ID_CID_READBYPASSED, OnComCIDReadBypass)
	ON_COMMAND(ID_CID_READFAILED, OnComCIDReadFailed)
	ON_COMMAND(ID_LP_LATCHOK, OnLatchFoupOK)
	ON_COMMAND(ID_LP_LATCHFAILED, OnLatchFoupFail)
	ON_COMMAND(ID_CID_READOK, OnCidReadok)
	ON_COMMAND(ID_COM_NONHSMSCARRIERARRIVED, OnComNonhsmscarrierarrived)
	ON_COMMAND(ID_COM_EQPOUTOFSERVICE, OnComEqpoutofservice)
	ON_COMMAND(ID_COM_LPOUTOFSERVICE, OnComLpoutofservice)
	ON_BN_CLICKED(IDC_PROCEED, OnBnClickedProceed)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_COM_NONHSMSCARRIERREMOVED, OnComNonhsmscarrierremoved)
	ON_COMMAND(ID_COM_ABORTAPHDLATCH, OnComAbortaphdlatch)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_CHANGE(IDC_CARRIERID, OnChangeCarrierid)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_LOADPOS, OnLoadpos)
	ON_COMMAND(ID_LP_FOUPOPENOK, OnLpFoupopenok)
	ON_COMMAND(ID_LP_FOUPOPENFAIL, OnLpFoupopenfail)
	ON_COMMAND(ID_COM_ENDOPEN, OnComEndopen)
	ON_COMMAND(ID_COM_ABORTOPEN, OnComAbortopen)
	ON_COMMAND(ID_COM_EXITHOME, OnComExithome)
	ON_BN_CLICKED(IDC_PROCEED, OnProceed)
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
	ON_COMMAND(ID_IPC_HELLOACK, OnIpcHelloack)
	ON_MESSAGE(ID_IPC_DATAREQACK1, OnIpcDatareqack)
	ON_MESSAGE(ID_IPC_DATAREQNAK1, OnIpcDatareqnak)
	ON_MESSAGE(ID_COM_SLOTMAP, OnComSlotMap)
	ON_MESSAGE(ID_COM_SLOTMAPFAIL, OnComSlotMapFail)
	//ON_MESSAGE(WM_XTABSELECTED,OnTabSelected)
	ON_MESSAGE(ID_SVR_PROCEEDWITHCARRIERID, OnSvrProceedwithcarrierid)
	ON_MESSAGE(ID_SVR_PROCEEDWITHCARRIERSM, OnSvrProceedwithcarriersm)
	ON_MESSAGE(ID_SVR_PJDEFINE, OnSvrPjdefine)
	ON_MESSAGE(ID_SVR_CJDEFINE, OnSvrCjdefine)
	ON_MESSAGE(ID_SVR_CANCELCARRIER, OnSvrCancelcarrier)
	ON_MESSAGE(ID_SVR_CANCELCARRIERATPORT, OnSvrCancelcarrieratport)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
	ON_COMMAND(ID_REGISTERLP, RegisterLP)
	ON_COMMAND(ID_COM_LATCHFAIL, OnComLatchfail)
	ON_COMMAND(ID_COM_NOPOD, OnComNopod)
	ON_COMMAND(ID_COM_LATCHCMPL, OnLatchCmpl)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_RECOVER, OnRecover)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_CID_READIDOK, OnComCIDReadOK)
	ON_MESSAGE(UM_CARRIERREMOVED, OnCarrierRemoved)
	ON_MESSAGE(UM_CARRIERARRIVED, OnCarrierArrived)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobDlg message handlers
long CProcJobDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (!bTabSelected) {
		CString msg;
		msg.Format("Jobs\n%s", (char*)lP);
		pUDev->SetCurrentView(msg);

		RegisterLP();

		if (pUDev->IncompleteProcessDetection(ID + 1)) {
			if (!bIncomAlrm) {
				if (pXTab) {
					CString str;
					str.Format("Recover Load Port #%d", ID + 1);
					pXTab->ChangeCaption(str);
				}
				bIncomAlrm = TRUE;
				pGDev->SetAlarm(CAlrmList::POSSIBLEWAFERINTOOL);

				Message("Incomplete process detected. There maybe wafer in tool, please recover wafer", 1);
				m_cLoad.ShowWindow(SW_HIDE);
				m_cRecover.ShowWindow(SW_SHOW);
			}
		}

		bTabSelected = TRUE;
	}

	return 0;
}

long CProcJobDlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
	}
	bTabSelected = FALSE;
	return 0;
}

void CProcJobDlg::OnBnClickedProceed() {
	UpdateData(FALSE);
	if (m_CarrierID.GetLength() < 1) {
		AfxMessageBox("Carrier ID is needed");
		m_cLoad.EnableWindow(TRUE);
		return;
	}
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID; // Carrier ID Confirmed, could have been changed by user
	strscpy(pUDev->pProcJobInfo[ID].Cas.Recipe413.RcpeName, RCPNAMELEN, m_RecipeName.GetBuffer(0));

	ProceedToOpenFOUP();
}

void CProcJobDlg::OnBnClickedCancel() {
	Message("Cancel carrier received", 5);
	CancelAndQuit();
}

BOOL CProcJobDlg::OnInitDialog() {
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
	AddAnchor(IDC_LOADPOS, BOTTOM_RIGHT);
	AddAnchor(IDC_PROCEED, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_BROWSE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_CANCEL, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_ABORT, BOTTOM_RIGHT);
	AddAnchor(IDC_RECOVER, TOP_RIGHT, TOP_RIGHT);

	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		m_cLoadPos.ShowWindow(SW_HIDE);
	}

	// Wait till GDev fully initialized during startup
	SetTimer(IDT_REGISTERWIN, 1000, NULL);
	SetTimer(IDT_REFRESH, 3000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobDlg::OpCarrierremoved() {
	Message("Carrier removed", 5);

	m_CarrierID = "";
	UpdateData(FALSE);
	m_cCarrierID.EnableWindow(FALSE);
	m_cProceed.EnableWindow(FALSE);
	m_cCancel.EnableWindow(FALSE);
}

void CProcJobDlg::OnComCarrierarrived() {
	SetMaterialRecieved();
	p413App->Yld(500);
	// [04262022 ZHIMING
	// 11102022 ADDED CONDITION FOR DOUBLE ENTRANCE TO LATCH FOUP CAUSE LATCHTIMEOUT IN AUTO MODE
	if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) != CLPObj::AUTO && pGDev->bAutoStartLatching) {
		OnComButtonpushed();
	}
	// ]
}

void CProcJobDlg::OnComCarrierremoved() {
	SetMaterialRemoved();
}

void CProcJobDlg::OnComNonhsmscarrierarrived() {
	SetMaterialRecieved();
}

void CProcJobDlg::OnComNonhsmscarrierremoved() {
	SetMaterialRemoved();
}

// OHT load trigger [2/12/2012 Administrator]
void CProcJobDlg::OnComLoadcompleted() {
	SetOHTLoadComplete();
	m_cAbort.EnableWindow(TRUE);

	short LPort = pUDev->IncompleteProcessDetection(0);
	if (LPort > 0) {
		if (LPort == (ID + 1)) {
			m_cLoad.ShowWindow(SW_HIDE);
			m_cRecover.ShowWindow(SW_SHOW);
		}
		else {
			m_cLoad.EnableWindow(TRUE);
		}
		Message("Incomplete job detected, loading aborted");
		Message("There maybe wafer in tool, Please recover wafer", 1);
		if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
			pGDev->SetAlarm(CAlrmList::INCOMPLETEPROCESS);
			m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
		}
		return;
	}
	else {
		m_cLoad.EnableWindow(FALSE);
		LatchFoup();
	}
}

// Manual load complete trigger [2/12/2012 Administrator]
void CProcJobDlg::OnComButtonpushed() {
	Message("Load button pushed", 5);
	OnComLoadcompleted();
}

long CProcJobDlg::OnCarrierArrived(WPARAM wP, LPARAM lP) {
	Message("Carrier arrived (2)", 5);
	return 0;
}

long CProcJobDlg::OnCarrierRemoved(WPARAM wP, LPARAM lP) {
	Message("Carrier removed (2)", 5);
	return 0;
}

void CProcJobDlg::OnLatchFoupOK() {}

void CProcJobDlg::OnComAbortaphdlatch() {
	m_cLoad.EnableWindow(TRUE);
	Message("Latch FOUP aborted", 5);
	opAbortAphdLatch();
}

void CProcJobDlg::OnLatchFoupFail() {
	OnComAbortaphdlatch();
}

void CProcJobDlg::OnComLatchfail() {
	OnComAbortaphdlatch();
}

void CProcJobDlg::opAbortAphdLatch() {
	KillTimer(IDT_LATCHTIMEOUT);
	Message("Latch carrier failed", 0);
	AlarmAndCancelCarrier(FALSE);
}

void CProcJobDlg::OnComAbortopen() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);
	OnLpFoupopenfail();
}

void CProcJobDlg::OnLpFoupopenfail() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);

	CString msg;
	// Note: Do not change the wording below, decoded elsewhere
	CString str;
	str.LoadString(IDS_FOUPOPENFAILED);

	Message(str, 0);

	AlarmAndCancelCarrier(FALSE);
}

void CProcJobDlg::OnLatchCmpl() {
	KillTimer(IDT_LATCHTIMEOUT);

	SetLatchCmpl();

	m_cCancel.EnableWindow(TRUE);

	switch (ReadCarrierID(m_hWnd, ID_CID_READOK, ID_CID_READFAILED)) {
	case ofOK:
		break;
	case ofFAIL:
	case ofOBJERR:
		PostMessage(WM_COMMAND, ID_CID_READFAILED, NULL);
		break;
	}
	Message("Reading carrier ID", 5);
}

long CProcJobDlg::OnComCIDReadOK(WPARAM wP, LPARAM lP) {
	Message("Carrier ID read OK", 5);
	opCIDReadOK((char*)lP);
	return 0;
}

void CProcJobDlg::OnComCIDReadFailed() {
	Message("Carrier ID read failed", 5);
	opCIDReadFail();
}

void CProcJobDlg::OnComCIDReadBypass() {
	Message("Carrier ID read bypassed", 5);
	opCIDReadBypass();
}

void CProcJobDlg::opCIDReadOK(char* carrierID) {
	SetCarrierIDReadOK(carrierID);
	m_CarrierID = carrierID;

	int v;
	CString msgstr;
	switch (CarrierIDVerification()) {
	case ofRTN4:	// Remote verification OK [5/15/2012 Yuen]
		m_cCancel.EnableWindow();
		OnBnClickedProceed();
		break;
	case ofRTN3:	// Non remote verification OK [5/15/2012 Yuen]
		m_cCarrierID.EnableWindow();
		m_cProceed.EnableWindow();
		m_cCancel.EnableWindow();
		OnBnClickedProceed();
		break;
	case ofRTN2:	// Remote ID waiting for host [5/15/2012 Yuen]
		v = 120000;
		msgstr.Format("CID Waiting for Host (TOUT: %d secs)", v / 1000);
		Message(msgstr);
		SetTimer(IDT_IDWAITFORHOSTTOUT, v, NULL);
		break;
	case ofERR:		// ID verification fail [5/15/2012 Yuen]
		IDVerificationFail();
		break;
	}
}

void CProcJobDlg::opCIDReadFail() {
	CString msg, str;
	str.LoadString(IDS_CIDREADFAILED);
	msg.Format("%s (%d)", str, ID + 1);
	Message(str, 0);

	pGDev->pOLnClt->GEMEventCIDReadFail(ID + 1);

	if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE)) {
		CInputCarrierID dlg;
		dlg.m_CarrierID = "UNKNOWN";
		dlg.m_Prompt = "Fail to read from CIDR\nPlease input manually";
		if (dlg.DoModal() == IDOK) {
			m_CarrierID = dlg.m_CarrierID;
			UpdateData(FALSE);
			m_cCarrierID.EnableWindow();
			m_cProceed.EnableWindow();
			m_cCancel.EnableWindow();
			return;
		}
	}

	// Line mode is remote or user cancel manual carrier input request [2/12/2012 Administrator]
	// Equipment issue CancelCarrier automatically [2/12/2012 Administrator]
	UpdateData();
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID.Format("LP%d:READFAIL", ID + 1);
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

void CProcJobDlg::opCIDReadBypass() {
	Message("CID Read Bypassed", 2);
	pGDev->pOLnClt->GEMEventCIDBypassed(ID + 1);
	pGDev->pOLnClt->GEMEventIDWaitingForHost(ID + 1);
	Message("CID Waiting for Host");
}

void CProcJobDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_CANSTART) {
		KillTimer(IDT_CANSTART);
		switch (CanStart()) {
		case OKTOSTART:
			Message("Resource available");
			Message("Contacting 413 unit");
			if (AreYouThere()) {
				SetTimer(IDT_AREYOUTHERETIMEOUT, 3000, NULL);
			}
			else {
				Message("Could not contacting 413 unit");
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
			Message("413 Unit not online");
			pGDev->SetAlarm(CAlrmList::C413NOTONLINE);
			Message("Abort retry");
			Message("PLEASE DEQUE");
		}
		AreYouThereRetryCount++;
	}
	if (nIDEvent == IDT_CJMON) {
		KillTimer(IDT_CJMON);
		switch (DoCJMonitor()) {
		case pjcCANSTART:
			SetTimer(IDT_CANSTART, 200, NULL);
			break;
		case  pjcCJMON:
			SetTimer(IDT_CJMON, 1000, NULL);
			break;
		case pjcERR:
			CancelAndQuit();
			break;
		}
	}
	if (nIDEvent == IDT_DATAREQ_TIMEOUT) {
		KillTimer(IDT_DATAREQ_TIMEOUT);
		SetTimer(IDT_AREYOUTHERETIMEOUT, 10, NULL);
	}
	if (nIDEvent == IDT_OPENTIMEOUT) {
		KillTimer(IDT_OPENTIMEOUT);
		Message("Open FOUP Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}
	if (nIDEvent == IDT_SLOTMAPTIMEOUT) {
		KillTimer(IDT_SLOTMAPTIMEOUT);
		Message("Slot map Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_FOUPOPENFAIL, NULL);
	}
	if (nIDEvent == IDT_IDWAITFORHOSTTOUT) {
		KillTimer(IDT_IDWAITFORHOSTTOUT);
		Message("ID waiting for host Timeout", 2);
		IDWaitingForHostTimeout();
	}
	if (nIDEvent == IDT_SMWAITFORHOSTTOUT) {
		KillTimer(IDT_SMWAITFORHOSTTOUT);
		Message("Slot map waiting for host Timeout", 2);
		SMWaitingForHostTimeout();
	}
	if (nIDEvent == IDT_LATCHTIMEOUT) {
		KillTimer(IDT_LATCHTIMEOUT);
		Message("Latch FOUP Timeout", 2);
		PostMessage(WM_COMMAND, ID_LP_LATCHFAILED, NULL);
	}
	if (nIDEvent == IDT_PJWAITTIMEOUT) {
		KillTimer(IDT_PJWAITTIMEOUT);
		Message("Waiting for PJ timeout", 2);
		WaitingForPJTimeOut();
	}
	if (nIDEvent == IDT_REGISTERWIN) {
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
		if ((ID > -1) && (ID < 2)) {
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

	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		Refresh();
		SetTimer(IDT_REFRESH, 1000, NULL);
	}
}

void CProcJobDlg::OnCidReadok() {}

// LRESULT CProcJobDlg::OnTabSelected(WPARAM wP, LPARAM lP)
// {
// 	CString msg;
// 	msg.Format("Jobs\n%s",(char *)lP);
// 	pUDev->SetCurrentView(msg);
//
// 	RegisterLP();
//
// 	return 0;
// }

void CProcJobDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobDlg::OnComEqpoutofservice() {
	Message("Equipment out of service", 5);
	pGDev->SetAlarm(CAlrmList::EQPOUTOFSERVICE);
}

void CProcJobDlg::OnComLpoutofservice() {
	Message("out of service", 0);
	switch (ID + 1) {
	case 1:
		pGDev->SetAlarm(CAlrmList::LP1OUTOFSERVICE);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::LP2OUTOFSERVICE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

CInfoPanelMain* CProcJobDlg::GetParentWnd() {
	return m_Parent;
}

void CProcJobDlg::OnComNovacuum() {
	Message("No vacuum error", 1);
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

void CProcJobDlg::OnComWaferout() {
	Message("Wafer protrusion error", 1);
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

void CProcJobDlg::OnComVacuumlost() {
	Message("Vacuum lost error", 1);
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

void CProcJobDlg::OnComSensortrip() {
	Message("Vacuum lost error", 1);
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

LRESULT CProcJobDlg::OnComTp1timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComTp2timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComTp3timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComTp4timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComTp5timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComTp6timeout(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComLoste84signal(WPARAM wP, LPARAM lP) {
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

LRESULT CProcJobDlg::OnComUnexpectede84signal(WPARAM wP, LPARAM lP) {
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
int CProcJobDlg::TPErrorRecovery(short errCode, BOOL bCarrierPresent) {
	CString msg;
	msg.Format("TPRecovery carrier: carrier %s present", bCarrierPresent ? "" : "not");
	Message(msg, 1);

	if (bCarrierPresent) {
		if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::AUTO) {
			Sleep(1000);  // Wait till LP thread to finish [3/31/2012 Yuen]
			pGDev->pGEMObjs->SetLPTransferStatus(ID + 1, CLPObj::TRANSFERBLOCKED);
			OnComLoadcompleted();
		}
	}
	else {
		if (pGDev->pGEMObjs->GetLPAccessMode(ID + 1) == CLPObj::AUTO) {
			OnHoavbl();
		}
	}

	return 0;
}

void CProcJobDlg::CancelAndQuit() {
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

LRESULT CProcJobDlg::OnSvrProceedwithcarrierid(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_IDWAITFORHOSTTOUT);
	Message("Proceed with carrier (ID)", 5);
	if (lP) {
		// pSM.CoS contains data
		CSMOL* pSM = (CSMOL*)lP;
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		pInfo->Cas.JobInfo.CarrierID = pSM->CoS.Co.ObjID;
		for (int i = 0; i < pSM->CoS.Co.Capacity; i++) {
			pInfo->Cas.winfo.SetContentMap(i + 1, pSM->CoS.Co.CtMap[i]);
		}
		delete pSM;
	}
	SetIDVerificationOK();
	OnBnClickedProceed();
	return 0;
}

void CProcJobDlg::OnClear() {
	m_RecipeName = "";
	UpdateData(FALSE);
}

void CProcJobDlg::OnChangeCarrierid() {
	UpdateData(TRUE);
	int idx = m_CarrierID.FindOneOf("\\/:?*<>\"|");
	if (idx != -1) {
		AfxMessageBox("Illegal charecter \\/:?*<>\"|");
		m_CarrierID.SetAt(idx, 0);
		UpdateData(FALSE);
	}
}

LRESULT CProcJobDlg::OnSvrCancelcarrier(WPARAM wP, LPARAM lP) {
	Message("Cancel carrier", 5);

	CSMOL* pSM = (CSMOL*)lP;
	if (!pSM) {
		Message("OnSvrCancelcarrier error", 2);
		ASSERT(0);
		return -2;
	}

	if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE)) {
		Message("Cancel Carrier Executed outside REMOTE mode", 2);
		if (pSM) {
			delete pSM;
		}
		return -1;
	}
	// LYF Verify carrier id is identical [8/16/2007 LYF]
	if (pSM->CoS.Co.ObjID != m_CarrierID) {
		Message("Carrier ID missmatch", 2);
		if (pSM) {
			delete pSM;
		}
		return -3;
	}

	UpdateData();
	pGDev->SetMessage("Cancel Carrier");
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();

	delete pSM;
	return 0;
}

LRESULT CProcJobDlg::OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP) {
	return OnSvrCancelcarrier(wP, lP);
}

BOOL CProcJobDlg::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CProcJobDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CProcJobDlg::SetupLPStatus() {}

BOOL CProcJobDlg::IsActiveJob(short port) {
	ASSERT(0);
	return TRUE;
}

void CProcJobDlg::OnLoad() {
	//[09092024
	if (p413App->Global.CheckRecovery()) {
		AfxMessageBox("Need Recovering");
		if (pUDev->pInfoPanelMain) {
			pUDev->pInfoPanelMain->PostMessage(UM_WAFERRECOVERY, 3, 0);
			pUDev->pNavigationPanel->OnBnClickedJobs();
		}
		return;
	}//]
	if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::RUNRCP) {
		pUDev->pTitlePanel->AskLogin();
	}
	if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
		m_cLoad.EnableWindow(FALSE);
		OnComCarrierarrived();
		OnComButtonpushed();
	}
}

void CProcJobDlg::LatchFoup() {
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

void CProcJobDlg::OnLoadpos() {
	m_cLoadPos.EnableWindow(FALSE);
	if (!p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) {
		p413App->StageMoveError();
	}
	m_cLoadPos.EnableWindow(TRUE);
}

void CProcJobDlg::ProceedToOpenFOUP() {
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

void CProcJobDlg::OnLpFoupopenok() {}

void CProcJobDlg::OnComEndopen() {
	KillTimer(IDT_OPENTIMEOUT);
	KillTimer(IDT_SLOTMAPTIMEOUT);

	Message("Open FOUP complete", 5);
	pGDev->pGEMObjs->Cms[ID].LPObj.CarrierLoc = CLPObj::clDOCKED;

	UpdateData(FALSE);
}

void CProcJobDlg::OnComExithome() {
	pGDev->pOLnClt->GEMEventFoupDocked(ID + 1);
	Message("Exit home");
	Message("Dock complete", 5);
	Message("Opening FOUP", 5);
	UpdateData(FALSE);
}

LRESULT CProcJobDlg::OnComSlotMap(WPARAM wP, LPARAM lP) {
	CString str = (char*)(lP);
	if ((str.Find('2') != -1) ||
		(str.Find('4') != -1) ||
		(str.Find('5') != -1)) {
		OnComSlotMapFail(wP, lP);
		return 0;
	}

	strscpy(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, MAXCAPACITYLEN, str);

	Message("Slot map read", 5);
	Message(pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, 5);

	DosUtil.RememberSM(ID + 1, pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);

	SetContentMap();

	ShowContentMap();

	SlotmapVerification();

	return 0;
}

LRESULT CProcJobDlg::OnComSlotMapFail(WPARAM wP, LPARAM lP) {
	Message("Slot map read fail");
	if (pGDev->pGEMObjs->IsOnline()) {
		pGDev->pOLnClt->GEMEventSlotmapReadFail(ID + 1);
	}

	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		Message("SM Waiting for host");
		UpdateData(FALSE);
		break;
	default:
		UpdateData();
		// Note: Do not change the working
		ShowWindow(SW_HIDE);
		ProceedToCancelCarrier();
		break;
	}
	return 0;
}

void CProcJobDlg::SlotmapVerification() {
	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::REMOTE:
	{
		pGDev->pOLnClt->GEMEventSMWaitingForHost(ID + 1,
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP);
		int v = 1200000; //200619 TEMPORARY
		CString msgstr;
		msgstr.Format("SM Waiting for host (TOUT: %d secs)", v / 1000);
		Message(msgstr);
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

LRESULT CProcJobDlg::OnSvrProceedwithcarriersm(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_SMWAITFORHOSTTOUT);

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

void CProcJobDlg::OnCancel() {
	KillTimer(IDT_AREYOUTHERETIMEOUT);

	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

void CProcJobDlg::OnProceed() {
	ShowWindow(SW_HIDE);
	type = 3;
	ProceedToProcessing();
}

void CProcJobDlg::ProceedProcessJobQueue() {
	m_CarrierID = pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID;

	srID = ID;
	srParent = m_Parent;

	Tool.SetCasStateQueued(ID + 1);

	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
		m_cProceed.EnableWindow(FALSE);
		m_cCancel.EnableWindow(FALSE);
		Message("Waiting to start");
		int v = 120000;
		CString msgstr;
		msgstr.Format("Waiting for CJob (TOUT: %d secs)", v / 1000);
		Message(msgstr);
		SetTimer(IDT_PJWAITTIMEOUT, v, NULL);
		SetTimer(IDT_CJMON, 50, NULL);	// Look for CJ that has the correct carrier ID and port ID assignment
	}
	else {
		Message("Waiting for resources");
		SetTimer(IDT_CANSTART, 50, NULL);
	}
}

void CProcJobDlg::OnIpcHelloack() {
	KillTimer(IDT_AREYOUTHERETIMEOUT);
	StartProcessRequest();
}

int CProcJobDlg::GetHWnd() {
	return (int)m_hWnd;
}

void CProcJobDlg::Message(CString msg, short lvl) {
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

void CProcJobDlg::SetDataReqTimeOut(long dur) {
	SetTimer(IDT_DATAREQ_TIMEOUT, dur, NULL);
}

LRESULT CProcJobDlg::OnIpcDatareqack(WPARAM wP, LPARAM lP) {
	// bypass distributed computing [8/5/2010 XPMUser]
	OnProceed();  // allowed to continue [8/5/2010 XPMUser]
	return 0;
}

LRESULT CProcJobDlg::OnIpcDatareqnak(WPARAM wP, LPARAM lP) {
	SetTimer(IDT_DATAREQ_TIMEOUT, 10, NULL);

	// No need to delete lP
	return 0;
}

void CProcJobDlg::StartProcessRequest() {
	PostMessage(ID_IPC_DATAREQACK1, 0, NULL);
	return;
}

LRESULT CProcJobDlg::OnSvrPjdefine(WPARAM wP, LPARAM lP) {
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
	Message(m_Status);

	m_Status = "Recipe: ";
	m_Status += pSM->Pj.RecipeName;
	Message(m_Status);

	Message("Slots:");
	for (i = 0; i < MAXCAPACITYLEN; i++) {
		SlotInfo* s = &pSM->Pj.slot[i];
		short slotno = s->slotno;
		if (slotno != -1) {
			CString str;
			str.Format("%d", slotno);
			Message(str);
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

LRESULT CProcJobDlg::OnSvrCjdefine(WPARAM wP, LPARAM lP) {
	if (!lP) {
		return -1;
	}
	CSMOL* pSM = (CSMOL*)lP;

	CCtrlJob* pJ = CJobMgr.GetCJob(pSM->Pj.PrJID);
	if (pJ) {
		Message(pSM->Pj.PrJID);
		CString m_Status;
		for (int n = 0; n < pJ->CarrierInputSpec.GetSize(); n++) {
			m_Status = "Carriers : <";
			m_Status += pJ->CarrierInputSpec.GetAt(n);
			m_Status += "> ";
			Message(m_Status);
		}
	}
	delete pSM;

	UpdateData(FALSE);

	return 0;
}

BOOL CProcJobDlg::AreYouThere() {
	// bypass distributed implementation [8/5/2010 XPMUser]
	PostMessage(WM_COMMAND, ID_IPC_HELLOACK, 0);
	return TRUE;
}

void CProcJobDlg::AlarmAndCancelCarrier(BOOL bCancelCarrier) {
	// Do something here like setting alarm and rejecting FOUP
	switch (ID) {
	case 0:
		pGDev->SetAlarm(CAlrmList::LP1OPENFAILURE);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2OPENFAILURE);
		break;
	}
	// 	pGDev->pLTMgr->SetGenAlarm(TRUE);
	if (bCancelCarrier) {
		ShowWindow(SW_HIDE);
		ProceedToCancelCarrier();
	}
}

void CProcJobDlg::IDVerificationFail() {
	CString msgstr;
	msgstr.Format("Carrier ID Failed Verification: %s ", m_CarrierID);
	Message(msgstr, 0);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::ID1VERIFICATIONFAIL);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::ID2VERIFICATIONFAIL);
		break;
	}
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

void CProcJobDlg::IDWaitingForHostTimeout() {
	CString msgstr;
	msgstr.Format("ID waiting for host timeout: %s ", m_CarrierID);
	Message(msgstr, 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1IDWAITFORHOSTTIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2IDWAITFORHOSTTIMEOUT);
		break;
	}
	ShowWindow(SW_HIDE);
	ProceedToCancelCarrier();
}

void CProcJobDlg::SMWaitingForHostTimeout() {
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

void CProcJobDlg::WaitingForPJTimeOut() {
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

void CProcJobDlg::OnAbort() {
	CancelAndQuit();
}

void CProcJobDlg::RegisterLP() {
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);
	pGDev->RegisterWinWithIDRMgr(m_hWnd, ID + 1);
}

void CProcJobDlg::Refresh() {
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

void CProcJobDlg::OnHoavbl() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = NULL/*m_hWnd*/;
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

void CProcJobDlg::OnComNopod() {
	Message("No Pod on load port", 5);
}

void CProcJobDlg::OnRecover() {
	if (pUDev->pTitlePanel->AuthenticateLevel >= CLogInDataNew::RUNRCP) {
		SLPInit LpInit;
		LpInit.unit = ID + 1;
		LpInit.action = CIncmplPrcsHdlr::STARTRECOVERY;
		LpInit.bNoClose[ID] = pUDev->PerformAction(LpInit.action, LpInit.unit, LpInit.CarrierID);
	}
	else pUDev->pTitlePanel->AskLogin();
}