// ProcJob413ProcessingDlg.cpp : implementation file
//

#include "stdafx.h"
// #include <math.h>
#include "strSOp.h"

#include "e95.h"

#include "IPC/Smol.h"
#include "128/CAS/WaferInfo.h"
#include "128/GEM/ObjectsContainer.h"
#include "128/CAS/CasMgr.h"
#include "128/GEM/OLnClt.h"
#include "128/GEM/PrJobList.h"
#include "413/DMCMotorBase.h"
#include "413/EchoScope/EchoProbeBase.h"
#include "413/MotorS2.h"
#include "413/OceanOp/OceanBase.h"
#include "413App.h"
#include "AreUSure.h"
#include "Data.h"
#include "GEMEvt.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "LTW/LTWMgr.h"   // //012324 Masum Added for THEIL 413SA Signal Tower
#include "IncmplPrcsHdlr.h"
#include "InfoPanelMain.h"
#include "IPC/SLPInit.h"
#include "MPoint.h"
#include "ONLINE/GEM/CEID_LIST.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/PRJOB.H"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "PeakParam.h"
#include "SERIAL/MsgItem.h"
#include "SYS/CUMMsg.h"
#include "SYS/Tool.h"
#include "ImgWnd.h"
#include "XTabCtrl.h"

#include "CWLBase.h"
#include "FEM/Rbt/RbtMgr.h"

#include "ProcJob413ProcessingDlg.h"
#include "ProbeSwitching.h"
#include "WaferMapViewDlg.h"
#include "DiceMapTemp.h"
#include "Auto2DGlut.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcJob413ProcessingDlg dialog

#define IDT_SENDPROCESSJOB		9999
#define IDT_AREYOUTHERETOUT		9996
#define IDT_SPC_900MODULE		9995
#define IDT_RECOVERPROCESSJOB	9994
#define IDT_COMPLETECLOSE		9993
#define IDT_DELAYSTART			9992
#define IDT_ERRORABORT			9991
#define IDT_HEARTBEAT			9990

extern float px[], py[];

CProcJob413ProcessingDlg::CProcJob413ProcessingDlg(int IDD, CWnd* pParent /*=NULL*/) :
	CResizableDialog(/*CProcJob413ProcessingDlg::*/ IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJob413ProcessingDlg)
	//}}AFX_DATA_INIT
	type = 3;
	bMeasureOn = FALSE;
	bProcessing = FALSE;
	DispatchTo = NOMODULE;

	m_cWaferMap.dmMode = CWaferMap::dmUSER;
	bEchoMotorStarted = FALSE;
	hProcMonThread = NULL;

	m_hLiveThread1 = NULL;
	m_hLiveThread2 = NULL;

	bPause = FALSE;
	bMonitor = TRUE;

	maxpointtime = 0;

	Show = 1;

	nCjName = 0;
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		dwCjName[i] = "";
	}

	dpTime = 0;

	g = NULL;
	pRcp = NULL;

	m_cVideo.Name = "ProJobProcessing";
}

CProcJob413ProcessingDlg::~CProcJob413ProcessingDlg() {
	ClearAllMemory(); // Just in case [11/14/2007 YUEN]
}

void CProcJob413ProcessingDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJob413ProcessingDlg)
	DDX_Control(pDX, IDC_RECOVER, m_cRecover);
	DDX_Control(pDX, IDC_HEARTBEAT, m_cHeartBeat);
	DDX_Control(pDX, IDC_PAUSEVIDEO, m_cPauseVideo);
	DDX_Control(pDX, IDC_MONITOR, m_cMonitor);
	DDX_Control(pDX, IDC_PROFILE, m_cProfile);
	DDX_Control(pDX, IDC_IMAGE, m_cImgWnd);
	DDX_Control(pDX, IDC_MESSAGE, m_cList);
	DDX_Control(pDX, IDC_REPORT, m_cReport);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_CLOSE, m_cClose);
	DDX_Control(pDX, IDC_ABORT, m_cAbort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJob413ProcessingDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJob413ProcessingDlg)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_COMMAND(ID_IPC_HELLOACK, OnIpcHelloack)
	ON_COMMAND(ID_IPC_HELLONAK, OnIpcHellonak)
	ON_WM_TIMER()
	ON_MESSAGE(ID_PRCS_PROCESSINGSTART, OnPrcsProcessingstart)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PAUSEVIDEO, OnPausevideo)
	ON_BN_CLICKED(IDC_MONITOR, OnMonitor)
	ON_BN_CLICKED(IDC_SWITCHVIEW, OnSwitchview)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_REPORT, OnColumnclickReport)
	ON_COMMAND(UM_RELOADCASSTTE, ReloadCassette)
	ON_MESSAGE(UM_GENMESSAGEINQUE, OnMsgInQue)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(UM_PJSTART, OnPJStart)
	ON_MESSAGE(UM_CJABORT, OnCJAbort)
	ON_MESSAGE(UM_CJSTOP, OnCJStop)
	ON_BN_CLICKED(IDC_RECOVER, OnRecover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJob413ProcessingDlg message handlers
void CProcJob413ProcessingDlg::OnAbort() {
	if (g && g->bAborting) {
		return;
	}
	CAreUSure dlg;
	dlg.m_Message = "Please confirm.\r\nSure to abort?";
	dlg.DoModal();
	if (dlg.bRet) {
		Abort();
	}
}

CGEMEvt SMPJRT[4]; // Prevent crash when this call closes [12/19/2007 YUEN]

void CProcJob413ProcessingDlg::OnClose() {	// Give some time for CCasView to complete processing last scan line data [12/27/2007 YUEN]
	SetTimer(IDT_COMPLETECLOSE, 2000, NULL);
}

LRESULT CProcJob413ProcessingDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = GetCurRecipe();

	if (!bTabSelected) {
		// Must not start or stop camera here [7/24/2013 Yuen]
		bTabSelected = TRUE;
	}
	return 0;
}

LRESULT CProcJob413ProcessingDlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		// Must not start or stop camera here [7/24/2013 Yuen]
		bTabSelected = FALSE;
	}
	return 0;
}

BOOL CProcJob413ProcessingDlg::DestroyWindow() {
	StopCamera();
	return CResizableDialog::DestroyWindow();
}

void CProcJob413ProcessingDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CProcJob413ProcessingDlg::StartCamera(CCamBase::CAMERAID No, CRecipe413* pRcp) {
	if (p413App->StartCamera(&m_cVideo, No, &m_cList, pRcp)) {
		m_cVideo.pWnd = this;
		bPause = TRUE; OnPausevideo();	// Turn video on [3/30/2013 Yuen]
		bMonitor = !pRcp->bEchoPeakMonitor; OnMonitor();
	}
}

void CProcJob413ProcessingDlg::StopCamera() {
	p413App->StopCamera(&m_cVideo, &m_cList);
}

BOOL CProcJob413ProcessingDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	bProcessing = TRUE;

	if (!m_Parent || !pUDev || !pGDev) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_CLOSE, TOP_RIGHT);
	AddAnchor(IDC_ABORT, TOP_RIGHT);
	AddAnchor(IDC_PAUSEVIDEO, TOP_RIGHT);
	AddAnchor(IDC_MONITOR, TOP_RIGHT);
	AddAnchor(IDC_POSITIONECHO, TOP_RIGHT);
	AddAnchor(IDC_POSITIONTHINFILM, TOP_RIGHT);
	AddAnchor(IDC_REPORT, TOP_LEFT, BOTTOM_RIGHT);
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		AddAnchor(IDC_MESSAGE, TOP_LEFT, BOTTOM_LEFT);
	}
	AddAnchor(IDC_WAFERMAP, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_PROFILE, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_SWITCHVIEW, TOP_RIGHT);
	AddAnchor(IDC_RECOVER, BOTTOM_RIGHT);

	if (Var.UIMode > 1) {
		m_cClose.ShowWindow(SW_SHOW);
	}

	RegisterWithSecsSvr(this, ID);

	g = &p413App->Global;
	pRcp = GetCurRecipe();

	if ((!g) || (!pRcp)) {
		ASSERT(0);
		return FALSE;
	}

	g->bStop = FALSE;
	g->bAborting = FALSE;

	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cImgWnd.bScaled = FALSE;

	SetupColumn();
	SetupReportHeader();

	pGDev->PauseDoorMonitor();

	// Start After 1 seconds [11/10/2011 C2C]
	SetTimer(IDT_DELAYSTART, 1000, NULL);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

static int IDSav[4];

void CProcJob413ProcessingDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_DELAYSTART) {
		KillTimer(IDT_DELAYSTART);
		m_cVideo.XX = 2;
		m_cVideo.pWnd = this;
		m_cVideo.SetRecipe(pRcp);
		StartCamera(CCamBase::ECHOCAM, pRcp);
		// 		bPause = FALSE; OnPausevideo();	// Turn video on [3/30/2013 Yuen]
		// 		bMonitor = !pRcp->bEchoPeakMonitor; OnMonitor();
				//UpdateTargets();
		if (bRecovery) {
			// Process wafer recovery [3/30/2013 Yuen]
			Message("Recovery process started");
			SetTimer(IDT_RECOVERPROCESSJOB, 100, NULL);
			return;
		}

		PrepareToStart();

		CString msg;
		msg.Format("Now processing recipe %s", pRcp->RcpeName);
		Message(msg, 2);

		msg.Format("Begin: LP%d, %s, %s", ID + 1, m_pCasMgr->pCas->JobInfo.CarrierID.GetBuffer(), pRcp->RcpeName);
		p413App->RecordEvent(msg);

		for (int j = 0; j < MAXMEPASET; j++) {
			if (pRcp->IsEcho(j)) {
				p413App->ResetEchoSamplingProperty(pRcp, &m_cList);
				break;
			}
		}

		// Normal Processing [3/30/2013 Yuen]
		if (pGDev->AramsE95.GetCurrentState() != SArams::PRODUCTIVE) {
			pGDev->AramsE95.SetCurrentState(SArams::PRODUCTIVE);
			pGDev->AramsE95.Save("Aram413.bin");
		}
		if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
			Message("Remote process started");
			if (!StartControlJob()) {
				// Close dialog and exit [2/9/2012 Administrator]
				// TODO: CJ & PJ need to be cleaned and removed from system [2/9/2012 Administrator]
				Message("Cannot start CJob", 1);
				SetProcessingFail(NULL);
			}
		}
		else {
			Message("Offline process started");
			SetTimer(IDT_SENDPROCESSJOB, 100, NULL);
		}

		return;
	}
	if (nIDEvent == IDT_SENDPROCESSJOB) {
		KillTimer(IDT_SENDPROCESSJOB);

		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		CNCas* pCas = &pInfo->Cas;
		pCas->JobInfo.LPUnit = ID + 1;
		CCarrierObj* cObj = &pGDev->pGEMObjs->Cms[ID].CaObj;
		for (int i = 0; i < cObj->Capacity; i++) {
			CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(i + 1);
			if (!pWI) {
				continue;
			}
			if (pWI->bSelected) {
				// Initialization [8/5/2010 XPMUser]
				cObj->Substrate[i].SubstState = CSubstObj::NO_TR_STATE;
				cObj->Substrate[i].SubstProcState = CSubstObj::NO_PROCSTATE;

				// Begin state change [8/5/2010 XPMUser]
				CGEMEvt Ge;
				Ge.Slot = i + 1;
				Ge.PortID = ID + 1;
				strscpy(Ge.CarrierID, OBJIDLEN, pInfo->Cas.JobInfo.CarrierID);
				strscpy(Ge.SubstID, OBJIDLEN, pWI->WaferID);
				Ge.SuObj.SubstState = CSubstObj::ATSOURCE;
				pGDev->pOLnClt->CSOStateEvent(Ge);
				Ge.SuObj.SubstProcState = CSubstObj::NEEDS_PROCESSING;
				pGDev->pOLnClt->CSOProcStateEvent(Ge);
			}
		}
		RememberPrJob(ID + 1, &pUDev->pProcJobInfo[ID]);
		// Single computer version [7/12/2010 C2C]
		StartDoMeasure();
		return;
	}
	if (nIDEvent == IDT_AREYOUTHERETOUT) {
		OnIpcHellonak();
		return;
	}
	if (nIDEvent == IDT_RECOVERPROCESSJOB) {
		KillTimer(IDT_RECOVERPROCESSJOB);
		ShowReportView();
		return;
	}
	if (nIDEvent == IDT_COMPLETECLOSE) {
		KillTimer(IDT_COMPLETECLOSE);

		StopCamera();

		m_cAbort.ShowWindow(SW_HIDE);
		m_cClose.ShowWindow(SW_HIDE);

		Message("Clear all memory");
		ClearAllMemory(); //  [11/14/2007 YUEN]

		bProcessing = FALSE;
		IDSav[ID] = ID; // Just in case ID get destroyed too earlier [12/18/2007 YUEN]

		clock_t endtime = clock();
		long lott = endtime - lottime;
		CString msg;
		msg.Format("COMPLETE: On %.2f hours  Lot %.2f mins  Max %.1f secs",
			endtime / 3600000.0f,
			lott / 60000.0f,
			maxpointtime / 1000.0f);
		Message(msg, 2);
		p413App->RecordEvent(msg);
		g->bStop = FALSE;
		g->bAborting = FALSE;
		if (g && g->bCanClose) {
			ShowWindow(SW_HIDE);
			if (hProcMonThread) {
				CloseHandle(hProcMonThread);
			}
			m_Parent->PostMessage(UM_TO_PROCJOBCOMPLETE, IDSav[ID], 0);
		}
		else {
			OnToErrorRecovery();
		}
		return;
	}
	if (nIDEvent == IDT_ERRORABORT) {
		KillTimer(IDT_ERRORABORT);
		// Handle error and exit here [2/9/2012 Administrator]
		SetProcessingFail(NULL);
		return;
	}
	if (nIDEvent == IDT_HEARTBEAT) {
		clock_t endtime = clock();
		long lott = endtime - lottime;
		long wafert = (endtime - wafertime) / 1000;	// unit is seconds [3/28/2012 Yuen]
		long pointt = (endtime - pointtime); // unit is micro seconds, pointtime is updated by inner executing loop [2/17/2012 Administrator]
		if (!bMeasureOn) {
			pointt = 0;
		}
		if (pointt > maxpointtime) {
			maxpointtime = pointt;
		}

		CString msg;
		msg.Format("On %.2f hours  Lot %.2f mins Wafer %.2f mins  Point %.1f secs Max %.1f secs [%.1f]",
			endtime / 3600000.0f,
			lott / 60000.0f,
			wafert / 60.0f,
			pointt / 1000.0f,
			maxpointtime / 1000.0f,
			dpTime / 1000.0f
		);
		m_cHeartBeat.SetWindowText(msg);

		if (pRcp->WaferTimeOut > 0) {
			if (wafert > pRcp->WaferTimeOut) {
				KillTimer(IDT_HEARTBEAT);
				Message("Aborting: Reached wafer time limit", 1);
				switch (ID) {
				case 0:
					pGDev->SetAlarm(CAlrmList::LP1WAFERPROCESSINGTOUT);
					break;
				case 1:
					pGDev->SetAlarm(CAlrmList::LP2WAFERPROCESSINGTOUT);
					break;
				}
				// Cannot abort if it is dead [2/17/2012 Administrator]
				Abort();
			}
		}
		if (bMeasureOn && (pRcp->PointTimeOut > 0)) {
			if (pointt > (pRcp->PointTimeOut) / 1000) {
				KillTimer(IDT_HEARTBEAT);
				Message("Aborting: Reached per point time limit", 1);
				switch (ID) {
				case 0:
					pGDev->SetAlarm(CAlrmList::LP1POINTPROCESSINGTOUT);
					break;
				case 1:
					pGDev->SetAlarm(CAlrmList::LP2POINTPROCESSINGTOUT);
					break;
				}
				// Cannot abort if it is dead [2/17/2012 Administrator]
				Abort();
			}
		}
		return;
	}
}

CWnd* CProcJob413ProcessingDlg::GetThis() {
	return this;
}

CWnd* CProcJob413ProcessingDlg::GetDialogItem(UINT ID) {
	return GetDlgItem(ID);
}

void CProcJob413ProcessingDlg::ScrnToClnt(LPRECT lpRect) {
	ScreenToClient(lpRect);
}

LRESULT CProcJob413ProcessingDlg::OnMsgInQue(WPARAM wP, LPARAM lP) {
	return MsgInQue((CMsgItem*)lP);
}

// Do not delete pMsg
BOOL CProcJob413ProcessingDlg::SetProcessingComplete(CMsgItem* pMsg) {
	CString msg;
	Message("Processing Complete");
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
		if (CJobMgr.ActivateNextPRJob(pInfo->CJobName)) {
			Tool.SetCasStateQueued(ID + 1);
			// continue processing [9/1/2010 XPMUser]
			Message("Processing next PJob");
			return FALSE;
		}
	}

	Message("Completing measurement");

	SMPJRT[ID].act = CGEMEvt::EQUIPMENT;
	SMPJRT[ID].reason = CGEMEvt::EQPPROCESSINGCOMPLETE;
	pGDev->pOLnClt->GEMEvent(SMPJRT[ID]);

	msg.Format("Forget PRJob (LP%d)", ID + 1);
	Message(msg);
	DosUtil.ForgetPrJob(ID + 1);
	OnClose();		//  [11/14/2007 YUEN]

	return TRUE;	//  [12/17/2007 YUEN]
}

// Do not delete pMsg
void CProcJob413ProcessingDlg::SetProcessingFail(CMsgItem* pMsg) {
	Message("Processing failed", 2);
	if (pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE) {
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		CJobMgr.Register(this);
		CJobMgr.Abort(pInfo->CJobName.GetBuffer(0));
	}
	else {
		StoppingTool(TRUE);
	}
}

void CProcJob413ProcessingDlg::SetProcessingHalt(CMsgItem* pMsg) {
	StopCamera();

	CString str;
	str.LoadString(IDS_CARRIERHALTED); // do not change this string
	if (pMsg) {
		if (pMsg->pMsg) {
			str += "\n";
			str += pMsg->pMsg;
		}
	}
	pGDev->SetMessage(str);

	Message("Processing halt", 1);
	Message("Clear all memory");
	ClearAllMemory(); //  [11/14/2007 YUEN]
	//  [11/14/2007 YUEN]
	if (m_Parent) {
		str.Format("Forget PRJob (LP%d)", ID + 1);
		Message(str);
		DosUtil.ForgetPrJob(ID + 1);
	}

	clock_t endtime = clock();
	long lott = endtime - lottime;
	long wafert = endtime - wafertime;
	str.Format("HALT: On %.2f hours  Lot %.2f mins  Wafer %.2f mins  Max %.1f secs",
		endtime / 3600000.0f,
		lott / 60000.0f,
		wafert / 60000.0f,
		maxpointtime / 1000.0f);
	p413App->RecordEvent(str);
	if (g && g->bCanClose) {
		ShowWindow(SW_HIDE);
		if (hProcMonThread) {
			CloseHandle(hProcMonThread);
		}
		bProcessing = FALSE;
		m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
	}
	else {
		OnToErrorRecovery();
	}
}

BOOL CProcJob413ProcessingDlg::StartControlJob() {
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];

	CString msg;
	msg.Format("Starting CJ %s", pInfo->CJobName.GetBuffer());
	Message(msg);
	if (CJobMgr.StartCJob(pInfo->CJobName, this)) {
		msg.Format("CJob %s started", pInfo->CJobName.GetBuffer());
		Message(msg);
		return TRUE;
	}
	msg.Format("CJob %s start fail", pInfo->CJobName.GetBuffer());
	Message(msg, 1);
	return FALSE;
}

LRESULT CProcJob413ProcessingDlg::OnPrcsProcessingstart(WPARAM wP, LPARAM lP) {
	if (!lP) {
		return -1;
	}

	CSMOL* pSM = (CSMOL*)lP;

	// Write info in pSM to pProcJobInfo[ID]
	if (!MakeProcJobInfo(ID, pSM)) {
		SetTimer(IDT_ERRORABORT, 100, NULL);
	}
	if (pSM) {
		delete pSM;
	}
	return 0;
}

BOOL CProcJob413ProcessingDlg::MakeProcJobInfo(short ID, CSMOL* pSM) {
	Message("Make PRJob Info");

	CString msg;
	CString PrJID;
	PrJID = pSM->Ca.PrJobID;
	SPcJobExec* pPrJ = PrJobs.GetByPJ(PrJID); // PrJob is remove and deleted from list [9/10/2007 LYF]
	if (!pPrJ) {
		msg.Format("Rejected: No or wrong PJ %s", PrJID.GetBuffer());
		Message(msg, 1);
		return FALSE;
	}
	m_CarrierID = pSM->Ca.CarrierID;
	msg.Format("Starting carrier %s, PJ %s", m_CarrierID.GetBuffer(), PrJID.GetBuffer());
	Message(msg);

	int PortNo = ID + 1;
	if (PortNo == 0) {
		return FALSE;
	}
	CCarrierObj* c = pGDev->pGEMObjs->FindCarrier(m_CarrierID);
	if (!c) {
		msg.Format("Carrier object %s LP%d not found", m_CarrierID.GetBuffer(), PortNo);
		CarrierObjNotFoundEvent(m_CarrierID, PortNo);
		return FALSE;
	}
	pPrJ->PortNo = PortNo;
	if (pGDev->pGEMObjs->GetLPAssociatedCarrier(PortNo, m_CarrierID) != CLPObj::ASSOCIATED) {
		msg.Format("Load port %d is not associated with carrier %s", PortNo, m_CarrierID.GetBuffer());
		return FALSE;
	}

	if (pGDev->pGEMObjs->GetLPReservation(PortNo) != CLPObj::NOTRESERVED) {
		msg.Format("Load port %d is reserved", PortNo);
		pGDev->SetAlarm(CAlrmList::IMPROPERSCENARIOERROR, "");
		return FALSE;
	}

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CWaferInfoMgr* pWMgr = &pInfo->Cas.winfo;

	pWMgr->SetLPStation(ID + 1, ID + 1);

	if (strlen(pPrJ->RecipeName) > OBJIDLEN) {
		ASSERT(0);
		return FALSE;
	}
	strscpy(pInfo->Cas.Recipe413.ObjID, OBJIDLEN, pPrJ->RecipeName);
	int i;
	for (i = 0; i < 25; i++) {
		CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(i + 1);
		if (pWI) {
			pWI->bSelected = FALSE;
		}
	}
	for (i = 0; i < 25; i++) {
		SlotInfo* s = &pPrJ->slot[i]; // slot is 0 offset [8/30/2010 XPMUser]
		if (s->slotno > 0) {
			CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(s->slotno);
			if (pWI) {
				pWI->bSelected = TRUE;
			}
			else {
				ASSERT(0);
			}
		}
	}

	if (pPrJ) {
		delete pPrJ;
	}

	CGEMEvt Ge;
	Ge.act = CGEMEvt::EQUIPMENT;
	Ge.reason = CGEMEvt::EQPPROCESSINGSTART;
	pGDev->pOLnClt->GEMEvent(Ge);

	SetTimer(IDT_SENDPROCESSJOB, 100, NULL);
	return TRUE;
}

void CProcJob413ProcessingDlg::CarrierObjNotFoundEvent(CString& CarrierID, short port) {
	pGDev->SetAlarm(CAlrmList::CARRIEROBJECTNOTFOUND, "Carrier not found");
}

void CProcJob413ProcessingDlg::ScanDataArrived(CEchoData* sd) {}

void CProcJob413ProcessingDlg::ReloadCassette() {
	if (!bProcessing) {
		return;
	}
	bTreeUnderconstruction = TRUE;
	ShowTreeView();
	ShowView(REPORT);
	bTreeUnderconstruction = FALSE;
}

// Not use in 413 [5/14/2013 user]
void CProcJob413ProcessingDlg::OnIpcHelloack() {
	KillTimer(IDT_AREYOUTHERETOUT);

	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		CJobMgr.Register(this);
		CJobMgr.Abort(pInfo->CJobName.GetBuffer(0));
	}
	else {
		StoppingTool(TRUE);
	}
}

// Not use in 413 [5/14/2013 user]
void CProcJob413ProcessingDlg::OnIpcHellonak() {
	KillTimer(IDT_AREYOUTHERETOUT);

	if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE)) {
		if (AfxMessageBox("128 module not responding.\nDo you still want to abort?", MB_YESNO) == IDYES) {
			OnClose();
		}
	}
}

void CProcJob413ProcessingDlg::OnSize(UINT nType, int cx, int cy) {
	CResizableDialog::OnSize(nType, cx, cy);

	CRect rc;
	CButton* p = ((CButton*)GetDlgItem(IDC_PLOTAREA));
	if (p) {
		p->GetWindowRect(&rc);
		ScreenToClient(&rc);
	}
}

void CProcJob413ProcessingDlg::OnDestroy() {
	ClearAllMemory();
	CResizableDialog::OnDestroy();
}

//  [9/5/2007 LYF]
LRESULT CProcJob413ProcessingDlg::OnPJStart(WPARAM wP, LPARAM lP) {
	if (!lP) //200619
	{
		return -1;
	}
	CPRJob* p = (CPRJob*)lP;
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CNCas* pCas = &pInfo->Cas;
	if (pCas) {
		CString msg;
		msg.Format("PJob %s starts", pInfo->CJobName.GetBuffer());
		Message(msg);
		CString rcp = p->m_Recipe.c_str();
		rcp = "\\" + rcp;
		rcp = DosUtil.GetRecipeDir() + rcp;
		msg.Format("Load recipe: %s", rcp.GetBuffer());
		Message(msg);
		pCas->Recipe413.Load(rcp);
		p413App->ResetEchoSamplingProperty(&pCas->Recipe413, &m_cList); //200619
	}
	SetTimer(IDT_SENDPROCESSJOB, 100, NULL);
	return 0;
}

//  [9/5/2007 LYF]
LRESULT CProcJob413ProcessingDlg::OnCJAbort(WPARAM wP, LPARAM lP) {
	Message("Abort CJob", 1);

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CJobMgr.Register(this);
	CJobMgr.Aborted(pInfo->CJobName.GetBuffer(0));

	StoppingTool(TRUE);
	return 0;
}

//  [9/5/2007 LYF]
LRESULT CProcJob413ProcessingDlg::OnCJStop(WPARAM wP, LPARAM lP) {
	Message("Stop CJob");
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CJobMgr.Register(this);
	CJobMgr.Stopped(pInfo->CJobName.GetBuffer(0));

	StoppingTool(FALSE);
	return 0;
}

// Processing already aborted, this is clean up phase [2/19/2012 Administrator]
void CProcJob413ProcessingDlg::StoppingTool(BOOL bIsAbort) {
	StopCamera();

	CString str;
	str.LoadString(IDS_CARRIERABORTED); // do not change this string
	pGDev->SetMessage(str);

	Message("Clear all memory");
	ClearAllMemory(); //  [11/14/2007 YUEN]

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	str.Format("Carrier %s aborted", pInfo->Cas.JobInfo.CarrierID.GetBuffer());
	Message(str);

	CGEMEvt Ge;
	Ge.act = CGEMEvt::EQUIPMENT;
	Ge.reason = CGEMEvt::EQPPROCESSINGSTOP;
	pGDev->pOLnClt->GEMEvent(Ge);

	clock_t endtime = clock();
	long lott = endtime - lottime;
	long wafert = endtime - wafertime;  // pointtime is updated by inner executing loop [2/17/2012 Administrator]
	str.Format("STOP: On %.2f hours   Lot %.2f mins Wafer %.2f  Max %.1f secs",
		endtime / 3600000.0f,
		lott / 60000.0f,
		wafert / 60000.0f,
		maxpointtime / 1000.0f);
	p413App->RecordEvent(str);
	if (g && g->bCanClose) {
		ShowWindow(SW_HIDE);
		str.Format("Forget PRJob (LP%d)", ID + 1);
		Message(str);
		DosUtil.ForgetPrJob(ID + 1);
		if (hProcMonThread) {
			CloseHandle(hProcMonThread);
		}
		bProcessing = FALSE;
		m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_DISABLECANCEL);
	}
	else {
		OnToErrorRecovery();
	}
}

void CProcJob413ProcessingDlg::ClearAllMemory() {}

void CProcJob413ProcessingDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "PJobPrcs");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

// Static function [1/10/2013 FSM413]
// This is a separate thread [5/30/2012 Yuen]
// Entry point to 413 basic [7/16/2010 C2C]
DWORD CProcJob413ProcessingDlg::DoMeasurement(LPVOID pObj) {
	if (!pObj) {
		ASSERT(0);
		pGDev->ContinueDoorMonitor();
		AfxEndThread(-1);
		return -1;
	}

	C413Global* g = &p413App->Global;
	g->bStop = FALSE;

	short ret = 0;
	CProcJob413ProcessingDlg* pDlg = (CProcJob413ProcessingDlg*)pObj;

	short unit = pDlg->ID + 1;
	Tool.SetCasStateSelected(unit);
	if (!Tool.IsAnyCasInProcessState(unit)) {
		if (Tool.SetCasStateProcessing(unit)) {
			// Execute measurement [7/11/2010 C2C]
			ret = pDlg->Measure();
		}
		else {
			g->bStop = TRUE;
			pGDev->ContinueDoorMonitor();
			AfxEndThread(1);
			return 1;
		}
	}
	else {
		g->bStop = TRUE;
		pGDev->ContinueDoorMonitor();
		AfxEndThread(1);
		return 1;
	}

	g->bStop = TRUE;
	//here

	switch (ret) {
	case meCOMPLETED:	// No more wafer [3/1/2012 Administrator]
	case meABORTED:
		if (g->bAborting) {
			//pGDev->SetAlarm(CAlrmList::MEASUREMENTABORTED,"Abort measurement");
			Tool.SetCasStateAborted(unit);
			pDlg->SetProcessingFail(NULL);
			pGDev->ContinueDoorMonitor();
			AfxEndThread(-6);
			return -6;
		}
		else {
			if (Tool.GetCasState(unit) == CCasMgr::QUEUED) {
			}
			else {
				Tool.SetCasStateCompleted(unit);
				pDlg->SetProcessingComplete(NULL);
				pGDev->ContinueDoorMonitor();
				AfxEndThread(1);
				return 1;
			}
		}
		break;
	default:
		//pGDev->SetAlarm(CAlrmList::MEASUREMENTABORTED,"Abort measurement");
		Tool.SetCasStateAborted(unit);
		pDlg->SetProcessingFail(NULL);
		pGDev->ContinueDoorMonitor();
		AfxEndThread(-7);
		return -7;
		break;
	}

	pGDev->ContinueDoorMonitor();
	AfxEndThread(1);
	return 1;
}

void CProcJob413ProcessingDlg::StartDoMeasure() {
	Message("StartDoMeasurement Thread");
	pointtime = wafertime = lottime = clock();
	DWORD id = 88;
	hProcMonThread = CreateThread(NULL, 0, CProcJob413ProcessingDlg::DoMeasurement, this, 0, &id);
	SetTimer(IDT_HEARTBEAT, 1000, NULL); // Check every 1 second [2/17/2012 Administrator]
}

void CProcJob413ProcessingDlg::StopDoMeasure() {
	if (g->bStop == TRUE) {
		// Already stopped [7/12/2010 C2C]
		return;
	}
	g->bAborting = TRUE;
	while (g->bStop) {
		Sleep(20);
	}

	Message("Stop DoMeasurement Thread");
}

short CProcJob413ProcessingDlg::Measure() {
	if (g->CassetteRepeat < 1) {
		g->CassetteRepeat = 1;
	}

	CString msg;
	msg.Format("Cassette repeat %d times", g->CassetteRepeat);
	Message(msg);

	for (int i = 0; i < g->CassetteRepeat; i++) {
		msg.Format("Repeat # %d", i + 1);
		Message(msg);

		CObjectsContainer* pGEMObjs = pGDev->pGEMObjs;
		pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::INACCESS);

		MEASURE1RET ret = MeasureOne();

		StopEchoMotor();	// Stop it even if it is not started for safety [10/15/2012 Yuen]

		pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
		CPattern* pPattern = &pRcp->Patt;

		switch (ret) {
		case erOK:
			if (g->bAborting) {
				//SetProcessingFail(NULL);
				return meABORTED;
			}
			else {
				if (i == (g->CassetteRepeat - 1)) {
					//SetProcessingComplete(NULL);
					return meCOMPLETED;
				}
			}
			break;
		case erSTAGE:
			Message("Stage Error", 1);
			if (g) {
				g->bCanClose = FALSE;
			}
			//SetProcessingFail(NULL);
			return meSTAGEERR;
			break;
		case erROBOTOUT:
		case erROBOTIN:
			Message("Robot handler error", 1);
			if (g) {
				g->bCanClose = FALSE;
			}
			//SetProcessingFail(NULL);
			return meROBOTERR;
			break;
		case erMEMORY:
			//SetProcessingFail(NULL);
			return meMEMORYERR;
			break;
		case erABORTING:
			return meABORTED;
			break;
		default:
			//SetProcessingFail(NULL);
			return meUNKNOWNERR;
			break;
		}
	}
	return meUNKNOWNERR;
}

CProcJob413ProcessingDlg::MEASURE1RET CProcJob413ProcessingDlg::MeasureOne() {
	if (!g || !pRcp) {
		ASSERT(0);
		return erMEMORY;  // Should not happen [2/19/2012 Administrator]
	}
	CString msg;
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return erMEMORY; // Fatal error, will not continue [9/12/2012 Administrator]
	}
	pMtr->SetFFU(FALSE); ////20240814
	Message("Begin Processing", 2);

	// [02022024 ZHIMING
	if (pGDev->SystemModel == pGDev->FSM413SA) {
		if (pGDev->pLTMgr) pGDev->pLTMgr->Set_Run();
		// [01242024 ZHIMING
		if (p413App->pMtr) {
			if (!pRcp->IsEchoBowWarpMeasurement() && !pRcp->IsConfocalWarpMeasurement() && !pRcp->IsRoughnessMeasurement() && !pRcp->IsConfocalDualWarpMeasurement() && !pRcp->IsConfocalRoughnessMeasurement()) {//23102024 //12102024
				//[ 04182024 Tushar
				p413App->pMtr->SetChuckVacuum(TRUE, pRcp->Wp.size);
				p413App->Yld(1000);
				if (p413App->Global.bCheckWaferPresence) {
					if (!p413App->pMtr->IsWaferPresent()) {
						Message(_T("Wafer not in the chuck or vacuum too low"), 2);
						pGDev->SetAlarm(CAlrmList::INCOMMINGVACUUMLOW);
						//p413App->pMtr->SetChuckVacuum(FALSE);
						return erSTAGE;
					}
				}
			}
		}
	}
	else if (pGDev->SystemModel == pGDev->FSM413C2C) {
		if (p413App->Global.bCheckWaferPresence) {
			bool isWaferOnArm = pGDev->pRbtMgr->IsWaferOnArm();
			if (isWaferOnArm) {
				Message(_T("Wafer present on robto arm"), 2);
				pGDev->SetAlarm(CAlrmList::WAFERPRESENTONROBOTARM);
				return erROBOTOUT;
			}
			bool isWaferOnAligner = pGDev->pRbtMgr->IsWaferOnAligner();
			if (isWaferOnAligner) {
				Message(_T("Wafer present on aligner"), 2);
				pGDev->SetAlarm(CAlrmList::WAFERPRESENTONALIGNER);
				return erROBOTOUT;
			}
			bool isWaferOnStage = false;
			p413App->WaferOnStageCheck(pRcp, &isWaferOnStage, &m_cVideo, &m_cList);
			if (isWaferOnStage) {
				Message(_T("Wafer present on stage"), 2);
				pGDev->SetAlarm(CAlrmList::WAFERPRESENTONSTAGE);
				return erROBOTOUT;
			}
		}
	}
	// ]

	// Will switch echo motor off during thin film measurement [9/12/2012 Administrator]
	if (!pRcp->IsConfocalMeasurement() && !pRcp->IsRoughnessMeasurement()) {
		if (!StartEchoMotor()) {
			// Error, need to quit. Could not start echo motor [2/19/2012 Administrator]
			Message("Echo motor start failed", 2);
			pGDev->SetAlarm(CAlrmList::ECHOMOTORERROR);
			pMtr->SetFFU(TRUE); ////20240814
			return erECHOMOTOR;	// Echo motor error [2/19/2012 Administrator]
		}
		Message("Activating Echo Probe");
		p413App->pEcho->StartGetPeaks(NULL);	// Dummy measurement, to exercise the digitizer card [5/30/2012 Yuen]
	}

	// Reset stage after each 36 hours [2/19/2012 Administrator]
	ULONGLONG timeT = (GetTickCount64() - g->ResetLapTime) / 1000;	// Unit is in second [9/12/2012 Administrator]
	if (timeT < 0) {
		// GetTickCount64 warp around, do a reset [6/14/2013 Yuen]
		g->ResetLapTime = 0;
	}
	timeT = 0;
	if ((timeT > (g->ResetFreq * 60 * 60)) || (g->ResetLapTime == 0))  // reset every 36 hours [9/12/2012 Administrator]
	{
		BOOL bOK = FALSE;
		//g->bConfirmStage = FALSE;
		msg = "Reset X,Y,Z1,Z2";
		Message(msg);
		short nT = 2;
		for (int i = 0; i < nT; i++) {
			if (g->bAborting) {
				pMtr->SetFFU(TRUE); ////20240814
				return erABORTING;
			}
			Message("Reset all motors");
			p413App->Global.isDuringMes = true; //20112025_Tushar
			if (pMtr->ResetAll()) {
				Message("Goto Origin");
				if (pMtr->GotoXY(0, 0)) {
					Message("Set 0,0");
					pMtr->SetHome();
					bOK = TRUE;
					//g->bConfirmStage = FALSE;
					break;
				}
			}
			p413App->Global.isDuringMes = false; //20112025_Tushar
		}
		bOK = TRUE;
		if (!bOK) {
			msg = "Reset stage failed";
			Message(msg, 2);
			pGDev->SetAlarm(CAlrmList::RESETSTAGEFAILED);
			pMtr->SetFFU(TRUE); ////20240814
			return erSTAGE; // Stage error, will not continue [9/12/2012 Administrator]
		}
		g->ResetLapTime = GetTickCount64();
	}

	if (!pMtr->GetPosAll(&g->LocXY.X, &g->LocXY.Y, &g->LocEchoTopZ, &g->LocRoughZ, &g->LocCWLTopZ, &g->LocEchoBottomZ)) return erSTAGE;

	if (g->bAborting) {
		pMtr->SetFFU(TRUE); ////20240814
		return erABORTING;
	}
	SDoPattrnMatching Param;
	Param.pList = &m_cList;
	Param.pVideo = &m_cVideo;
	Param.pImgWnd = &m_cImgWnd;
	Param.pRcp = pRcp;
	Param.MeSet = 0;
	Param.PaSet = 0;
	Param.pMP = NULL;
	Param.pVideo->PaOffX = Param.pVideo->PaOffY = 0;
	Param.SearchDepth = 4;
	Param.Par.dT = 3;
	Param.Par.dI = 3;
	Param.Par.GoF = 0;
	Param.MatchRes = 12;
	Param.bMoveCenter = TRUE;

	if (!pRcp->IsConfocalWarpMeasurement() && !pRcp->IsRoughnessMeasurement() && !pRcp->IsConfocalDualWarpMeasurement() && !pRcp->IsConfocalRoughnessMeasurement()) { //23102024 //12102024
		if (!p413App->ConfirmLoadPos(Param)) {
			Message("Stage position verification FAILED");
			pGDev->SetAlarm(CAlrmList::STAGEMOTORERROR);
			pMtr->SetFFU(TRUE); ////20240814
			return erSTAGE; // Stage error, will not continue [9/12/2012 Administrator]
		}
		else {
			if (pRcp->IsThinFilmMeasurement()) { StopEchoMotor(); }//02162022 ZHIMING ADD FOR THIN FILM MEASUREMENT NO ECHO MOTOR CONSUMPTION
		}
	}

	if (g->bAborting) {
		pMtr->SetFFU(TRUE); ////20240814
		return erABORTING;
	}
	//20251320_1----------
	//msg = "Move stage to load position";
	//Message(msg);

	//if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
	//	Message("Stage didn't move to Load position correctly", 2);
	//	pGDev->SetAlarm(CAlrmList::STAGEMOTORERROR);
	//	pMtr->SetFFU(TRUE); ////20240814
	//	return erSTAGE;
	//}
	//----------------------
	if (g->bAborting) {
		pMtr->SetFFU(TRUE); ////20240814
		return erABORTING;
	}

	BOOL bIsEcho = pRcp->IsEchoMeasurement() || pRcp->IsEdgeMeasurement() || pRcp->IsMultiPointMeasurement();
	if (bIsEcho) {
		p413App->GotoPositionechoNonBlocking(); // Move P motor to echo position. Need to be non-blocking [7/13/2010 C2C]
		// No completion test because following steps take longer to complete [4/20/2013 Administrator]
	}
	if (g->bAborting) {
		pMtr->SetFFU(TRUE); ////20240814
		return erABORTING;
	}

	g->bCanClose = TRUE;

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CNCas* pCas = &pInfo->Cas;
	CCarrierObj* cObj = &pGDev->pGEMObjs->Cms[ID].CaObj;
	for (int i = 0; i < cObj->Capacity; i++) {
		if (g->bAborting) {
			pMtr->SetFFU(TRUE); ////20240814
			break;
		}
		pCas->JobInfo.LPUnit = ID + 1;
		CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(i + 1);
		if (!pWI) {
			continue;
		}

		// [01242024 ZHIMING
		CFPoint COffEcho = g->COffEcho;
		CFPoint COffTF = g->COffTF;
		// ]

		if (pWI->bSelected) {
			wafertime = clock();
			msg.Format("PROCESSING: LP%d, S%d, %s, %s", ID + 1, i + 1, m_pCasMgr->pCas->JobInfo.CarrierID.GetBuffer(), pRcp->RcpeName);
			p413App->RecordEvent(msg);
			Message(msg, 2);

			if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
				if (g->bAborting) {
					pMtr->SetFFU(TRUE); ////20240814
					continue;
				}
				short SlotNo = i + 1;
				if (TransferCas2Stage(SlotNo, TRUE)) {
					if (g->WaferRepeat < 2) {
						g->WaferRepeat = 1;
					}
					g->BevelWidth = 0;	// Do not delete, BevelWidth is set in DoBevelMeasurement() and used in DoTrimMeasurement, must set to zero here [5/31/2012 Yuen
					g->COffEcho.Clear();
					g->COffTF.Clear();
					if (pRcp->IsEdgeMeasurement() || pRcp->IsThinFilmMeasurement() || pRcp->bUseWaferAlign) {
						float Cx = 0, Cy = 0, Cr = 0;
						if (!CenteringWafer(Cx, Cy, Cr)) {
							g->WaferRepeat = 0;
							msg.Format("Abort: Fail wafer alignment");
							Message(msg, 1);
							switch (ID) {
							case 0:
								pGDev->SetAlarm(CAlrmList::LP1CENTERINGFAIL, "LP1 wafer centering fail");
								break;
							case 1:
								pGDev->SetAlarm(CAlrmList::LP2CENTERINGFAIL, "LP2 wafer centering fail");
								break;
							}

							if (!TransferStage2Cas(SlotNo)) {
								g->bAborting = TRUE;
								pMtr->SetFFU(TRUE); ////20240814
								g->WaferRepeat = 0;
								return erROBOTOUT;
							}
							ReloadRecipe();
							m_cWaferMap.Redraw();
							m_cReport.DeleteAllItems();
							continue;  // start next wafer [7/29/2013 Yuen]
						}
						else {
							if (pRcp->bUseWaferAlign) {
								msg.Format("Found wafer center - Cx:%.3f  Cy:%.3f  Cr:%.3f", Cx, Cy, Cr);
								Message(msg, 5);
								//pRcp->TrimWidthOuterRadius = Cr;  // amend outer radius to reflect actual radius [6/6/2013 Yuen]
							}
						}
					}

					// Confirm that p motor had stopped [9/12/2012 Administrator]
					pMtr->WaitP();	// Wait for P axis to stop [8/5/2013 Yuen]
					repeatCount = 0; // 08152025
					p413App->standardresult.clear(); // // MAHEDI 10292025_MH
					for (int r = 0; r < g->WaferRepeat; r++) {
						//repeatCount++;

						msg.Format("Measuring slot %d of LP%d (repeat:%d)", SlotNo, ID + 1, r + 1);
						Message(msg, 2);
						g->waferLotID= pWI->LotID;
						if (MeasureOneWafer(pRcp, pWI->WaferID, SlotNo)) {
							CTime endTime = CTime::GetCurrentTime(); //20251030_Mohir
							SaveData(pRcp, SlotNo, pWI->WaferID, pWI->LotID, !g->bAborting, pWI->WaferType, pWI->WaferSize, pWI->Station, startTime, endTime); //20251022_Mohir
 							SendWaferDataToHost(pRcp, SlotNo);

						}
						else {
							CTime endTime = CTime::GetCurrentTime(); //20251030_Mohir
							SaveData(pRcp, SlotNo, pWI->WaferID, pWI->LotID, !g->bAborting, pWI->WaferType, pWI->WaferSize, pWI->Station, startTime, endTime); //20251022_Mohir
						}
						if (!g->bAborting) {
							msg.Format("Repeat %d, Slot %d of LP%d", r + 2, SlotNo, ID + 1);
							Message(msg);
						}
						ReloadRecipe();
						m_cWaferMap.Redraw();
						m_cReport.DeleteAllItems();
					}

					// Wafer is confirmed on stage at this moment [4/18/2013 Administrator]
					// After measurement, returning wafer to cassette [4/18/2013 Administrator]
					if (!TransferStage2Cas(SlotNo)) {
						g->bAborting = TRUE;
						pMtr->SetFFU(TRUE); ////20240814
						g->WaferRepeat = 0;
						return erROBOTOUT;
					}
					ReloadRecipe();
					m_cWaferMap.Redraw();
					m_cReport.DeleteAllItems();
				}
				else {
					g->bAborting = TRUE;
					pMtr->SetFFU(TRUE); ////20240814
					g->WaferRepeat = 0;
					return erROBOTIN;
				}
			}
			else {
				g->BevelWidth = 0;	// Do not delete, BevelWidth is set in DoBevelMeasurement() and used in DoTrimMeasurement, must set to zero here [5/31/2012 Yuen]
				g->COffEcho.Clear();
				g->COffTF.Clear();
				//g->CXOffsetEcho = g->CXOffsetTF = 0;
				//g->CYOffsetEcho = g->CYOffsetTF = 0;
				if (pRcp->IsEdgeMeasurement() || pRcp->IsThinFilmMeasurement() || pRcp->bUseWaferAlign) {
					float Cx = 0, Cy = 0, Cr = 0;
					if (!CenteringWafer(Cx, Cy, Cr)) {
						g->WaferRepeat = 0;
						msg.Format("Abort Wafer: Fail wafer centering");
						Message(msg, 1);
						ReloadRecipe();
						m_cWaferMap.Redraw();
						m_cReport.DeleteAllItems();
						pMtr->SetFFU(TRUE); ////20240814
						return erOK;
					}
					else {
						msg.Format("Found wafer center - Cx:%.3f  Cy:%.3f  Cr:%.3f", Cx, Cy, Cr);
						Message(msg, 5);
						//pRcp->TrimWidthOuterRadius = Cr;  // amend outer radius to reflect actual radius [6/6/2013 Yuen]
					}
				}
				// None C2C operation [2/19/2012 Administrator]
				if (g->WaferRepeat < 1) {
					g->WaferRepeat = 1;
				}
				short run = 1;
				repeatCount = 0; // 08152025
				p413App->standardresult.clear(); // // MAHEDI 10292025_MH
				while (g->WaferRepeat--) {
					repeatCount++; // 08152025
					msg.Format("Run %d, Recipe %s.RCP", run, pRcp->RcpeName);
					Message(msg, 2);
					// [01242024 ZHIMING
					g->COffEcho.Set(COffEcho.X, COffEcho.Y);
					g->COffTF.Set(COffTF.X, COffTF.Y);
					// ]
					MeasureOneWafer(pRcp, pWI->WaferID, run);
					CTime endTime = CTime::GetCurrentTime(); //20251030_Mohir
					// >>> FIX: Copy live data to slot 0 for rendering
					CopyMeasuredDataToSlot0(pRcp);
					// <<< END FIX
				 
					SaveData(pRcp, run, pWI->WaferID, pWI->LotID, TRUE, pWI->WaferType, pWI->WaferSize, pWI->Station, startTime, endTime); //20251022_Mohir
					pCas->GeneratePDFFromCSV();//20251011_Mohir....
					Export3D();
					//CaptureToPDF();
					//p413App->PdfExporter.SaveAndClosePDF(); //20251215_Mohir
					SendWaferDataToHost(pRcp, run);
					ReloadRecipe();
					m_cWaferMap.Redraw();
					m_cReport.DeleteAllItems();
					run++;
					if (g->bAborting) {
						pMtr->SetFFU(TRUE); ////20240814
						break;
					}

					//20251219_Mahmudul
					if (bIsDiceType == TRUE) {
					//	dlgDice.m_cDiceMap.CaptureToPDF();
						dlgDice.DoModal(); 
					}	
					p413App->PdfExporter.SaveAndClosePDF(); //20251215_Mohir
				}
			}
			clock_t endtime = clock();
			long lott = endtime - lottime;
			long wafert = endtime - wafertime;
			msg.Format("DONE: On %.2f hours  Lot %.2f mins Wafer %.2f mins   Max %.1f secs",
				endtime / 3600000.0f, lott / 60000.0f, wafert / 60000.0f, maxpointtime / 1000.0f);
			Message(msg, 2);
			p413App->RecordEvent(msg);
		}
	}
	g->WaferRepeat = 0;
	pMtr->SetFFU(TRUE); ////20240814

	return erOK;
}

// 21102025_1
//CString CProcJob413ProcessingDlg::Generate2DMapBasePath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID)
//{
//	if (!pRcp) return _T("");
//
//	CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
//	if (!pJobInfo) return _T("");
//
//	// Build lot/batch/timestamp 
//	CString lotOrCarrier = LotID;
//	if (lotOrCarrier.IsEmpty()) {
//		lotOrCarrier = pJobInfo->Cas.JobInfo.CarrierID;
//	}
//	lotOrCarrier += _T("-") + pJobInfo->Cas.JobInfo.batchname;
//	CTime t = CTime::GetCurrentTime();
//	CString timestamp = t.Format(_T("-%d%H%M%S"));
//	lotOrCarrier += timestamp;
//
//	//*********
//	// Use MakeCassetteName to get correct base path structure
//	CCasMgr CasMgr;
//	CasMgr.SetCassette(&pJobInfo->Cas);
//	CString dummyC2C;
//	CString OpName = _T("FSM");
//	if (strlen(pGDev->username) != 0) OpName = pGDev->username;
//
//	// Generate a dummy C2C filename to get the proper directory structure
//	CasMgr.MakeCassetteName(dummyC2C, pRcp->RcpeName, lotOrCarrier, pJobInfo->Cas.JobInfo.CarrierID, FALSE, FALSE, OpName);
//
//	// Extract the directory path from the generated C2C filename
//	CString basePath = DosUtil.DirSys.ExtractFilePath(dummyC2C);
//
//	// Remove the "C2C" folder from the path and replace with "TXT\2D"
//	int pos = basePath.ReverseFind(_T('\\'));
//	if (pos != -1) {
//		basePath = basePath.Left(pos); // Remove "C2C" folder
//
//		// Now basePath should be in format: Root\Operator\Recipe\Year\Month
//		// Create TXT\2D subdirectories
//		CString txtPath = basePath + _T("\\TXT\\");
//		CString folder2D = txtPath + _T("2D\\");
//
//		// Create directories using GotoDirec (which calls CreateDir)
//		DosUtil.DirSys.GotoDirec(folder2D);
//
//		// Return the 2D folder path with the appropriate filename
//		return folder2D + _T("_") + lotOrCarrier;
//	}
//
//	// Fallback: Create path manually using the same logic as MakeCurC2CPath
//	CString fallbackPath;
//	CString monthPath;
//	CString OpName2 = _T("FSM");
//	if (strlen(pGDev->username) != 0) OpName2 = pGDev->username;
//
//	// Use MakeCurC2CPath to get the base path structure
//	CString baseC2CPath;
//	DosUtil.DirSys.MakeCurC2CPath(baseC2CPath, pRcp->RcpeName, OpName2);
//
//	// Remove "C2C" and add "TXT\2D"
//	int c2cPos = baseC2CPath.ReverseFind(_T('\\'));
//	if (c2cPos != -1) {
//		baseC2CPath = baseC2CPath.Left(c2cPos); // Remove "C2C"
//		CString txt2DPath = baseC2CPath + _T("\\TXT\\2D\\");
//		DosUtil.DirSys.GotoDirec(txt2DPath);
//		return txt2DPath + _T("_") + lotOrCarrier;
//	}
//
//	// Ultimate fallback - use current directory
//	CString currentDir;
//	::GetCurrentDirectory(MAX_PATH, currentDir.GetBuffer(MAX_PATH));
//	currentDir.ReleaseBuffer();
//	CString fallback = currentDir + _T("\\Data\\TXT\\2D\\");
//	DosUtil.DirSys.GotoDirec(fallback);
//	return fallback + _T("_") + lotOrCarrier;
//}
//
//[20251215_Mohir_Comment unecessary functions....
//CString CProcJob413ProcessingDlg::Generate2DMapBasePath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID)
//{
//	if (!pRcp) return _T("");
//
//	CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
//	if (!pJobInfo) return _T("");
//
//	// Build lot/batch/timestamp 
//	CString lotOrCarrier = LotID;
//	if (lotOrCarrier.IsEmpty()) {
//		lotOrCarrier = pJobInfo->Cas.JobInfo.CarrierID;
//	}
//	lotOrCarrier += _T("-") + pJobInfo->Cas.JobInfo.batchname;
//	CTime t = CTime::GetCurrentTime();
//	CString timestamp = t.Format(_T("-%d%H%M%S"));
//	lotOrCarrier += timestamp;
//
//	//*********
//	// Use MakeCassetteName to get correct base path structure
//	CCasMgr CasMgr;
//	CasMgr.SetCassette(&pJobInfo->Cas);
//	CString dummyC2C;
//	CString OpName = _T("FSM");
//	if (strlen(pGDev->username) != 0) OpName = pGDev->username;
//
//	// Generate a dummy C2C filename to get the proper directory structure
//	CasMgr.MakeCassetteName(dummyC2C, pRcp->RcpeName, lotOrCarrier, pJobInfo->Cas.JobInfo.CarrierID, FALSE, FALSE, OpName);
//
//	// Extract the directory path from the generated C2C filename
//	CString basePath = DosUtil.DirSys.ExtractFilePath(dummyC2C);
//
//	// Remove the "C2C" folder from the path and replace with "TXT\2D"
//	int pos = basePath.ReverseFind(_T('\\'));
//	if (pos != -1) {
//		basePath = basePath.Left(pos); // Remove "C2C" folder
//
//		// Now basePath should be in format: Root\Operator\Recipe\Year\Month
//		// Create TXT\2D subdirectories
//		CString txtPath = basePath + _T("\\TXT\\");
//		CString folder2D = txtPath + _T("2D\\");
//
//		// Create directories using GotoDirec (which calls CreateDir)
//		DosUtil.DirSys.GotoDirec(folder2D);
//
//		// NEW CODE: Find the next available BMP filename
//		CString bmpFilename = FindNextAvailableBmpFile(folder2D);
//
//		// Return the 2D folder path with the BMP filename
//		return folder2D + bmpFilename;
//	}
//
//	// Fallback: Create path manually using the same logic as MakeCurC2CPath
//	CString fallbackPath;
//	CString monthPath;
//	CString OpName2 = _T("FSM");
//	if (strlen(pGDev->username) != 0) OpName2 = pGDev->username;
//
//	// Use MakeCurC2CPath to get the base path structure
//	CString baseC2CPath;
//	DosUtil.DirSys.MakeCurC2CPath(baseC2CPath, pRcp->RcpeName, OpName2);
//
//	// Remove "C2C" and add "TXT\2D"
//	int c2cPos = baseC2CPath.ReverseFind(_T('\\'));
//	if (c2cPos != -1) {
//		baseC2CPath = baseC2CPath.Left(c2cPos); // Remove "C2C"
//		CString txt2DPath = baseC2CPath + _T("\\TXT\\2D\\");
//		DosUtil.DirSys.GotoDirec(txt2DPath);
//
//		// NEW CODE: Find the next available BMP filename for fallback path
//		CString bmpFilename = FindNextAvailableBmpFile(txt2DPath);
//		return txt2DPath + bmpFilename;
//	}
//
//	// Ultimate fallback - use current directory
//	CString currentDir;
//	::GetCurrentDirectory(MAX_PATH, currentDir.GetBuffer(MAX_PATH));
//	currentDir.ReleaseBuffer();
//	CString fallback = currentDir + _T("\\Data\\TXT\\2D\\");
//	DosUtil.DirSys.GotoDirec(fallback);
//
//	// NEW CODE: Find the next available BMP filename for ultimate fallback path
//	CString bmpFilename = FindNextAvailableBmpFile(fallback);
//	return fallback + bmpFilename;
//}
//
//// NEW HELPER FUNCTION: Finds the next available BMP filename
//CString CProcJob413ProcessingDlg::FindNextAvailableBmpFile(const CString& folderPath)
//{
//	int maxNumber = 0;
//
//	// Search pattern for BMP files
//	CString searchPattern = folderPath + _T("*.bmp");
//
//	WIN32_FIND_DATA findFileData;
//	HANDLE hFind = FindFirstFile(searchPattern, &findFileData);
//
//	if (hFind != INVALID_HANDLE_VALUE) {
//		do {
//			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//				CString filename = findFileData.cFileName;
//
//				// Remove .bmp extension and try to convert to number
//				filename = filename.Left(filename.GetLength() - 4); // Remove ".bmp"
//
//				// Check if the filename is a number
//				BOOL isNumber = TRUE;
//				for (int i = 0; i < filename.GetLength(); i++) {
//					if (!_istdigit(filename[i])) {
//						isNumber = FALSE;
//						break;
//					}
//				}
//
//				if (isNumber) {
//					int currentNumber = _ttoi(filename);
//					if (currentNumber > maxNumber) {
//						maxNumber = currentNumber;
//					}
//				}
//			}
//		} while (FindNextFile(hFind, &findFileData));
//
//		FindClose(hFind);
//	}
//
//	// Increment to get next available number
//	int nextNumber = maxNumber + 1;
//
//	// Format the filename with .bmp extension
//	CString bmpFilename;
//	bmpFilename.Format(_T("%d.bmp"), nextNumber);
//
//	return bmpFilename;
//}
//]]
//20251210_Mohir
//void CProcJob413ProcessingDlg::Export3D() {
//	// if measurement is not finished, do not export 3D data. Only successful measurement will export
//	if (g->bAborting) {
//		Message("Measurement aborted, do not export 3D data", 2);
//	}
//	
//	CString exportPath;
//	 
//	DosUtil.DirSys.GotoDirec(sAutoSavePath);
//	for (int i = 0; i < nCjName - 3; i++)
//	{
//		exportPath.Format(_T("%s%s.bmp"), sAutoSavePath, dwCjName[i + 3]);
//		m_cWaferMap.Export3DMap(i, exportPath);
//		p413App->Yld(300);
//	}
//}
// 
void CProcJob413ProcessingDlg::Export3D() {
	// if measurement is not finished, do not export 3D data. Only successful measurement will export
	if (g->bAborting) {
		Message("Measurement aborted, do not export 3D data", 2);
	}
	p413App->Global.callFrom3DAutoSave = true;  
	CString exportPath;
	//DosUtil.DirSys.GotoDirec(sAutoSavePath);
	for (int i = 0; i < nCjName - 3; i++)
	{
		//[12112025_Tushar
		auto it = p413App->Global.interpolationData_map.find(i);
		if (it != p413App->Global.interpolationData_map.end()) {
			// Copy to GeneralInterpolatedGrid for compatibility with Export3DMap
			p413App->Global.GeneralInterpolatedGrid = it->second;
		}//]

		exportPath.Format(_T("%s%s.bmp"), sAutoSavePath, dwCjName[i + 3]);
		m_cWaferMap.Export2DMap(i, exportPath); // for 2d image (glut)
		m_cWaferMap.Export3DMap(i, exportPath);
		p413App->Yld(300);
	}
}
//]



void CProcJob413ProcessingDlg::CopyMeasuredDataToSlot0(CRecipe413* pRcp)
{
	if (!pRcp) return;
	CPattern* pPatt = &pRcp->Patt;
	for (short i = 0; i < pPatt->MP.GetCount(); i++) {
		CMPoint* pMP = pPatt->MP.Get(i);
		if (!pMP || pMP->MeSet <= 0) continue;

		// Source: slot = MeSet - 1
		CData* pSrc = pMP->GetData(pMP->MeSet - 1, FALSE);
		// Destination: slot 0 (for DrawColorMap)
		CData* pDst = pMP->GetData(0, TRUE); // create if needed

		if (pSrc && pDst) {
			*pDst = *pSrc; // deep copy
		}
	}
}

BOOL CProcJob413ProcessingDlg::MeasureOneWafer(CRecipe413* pRcp, CString WaferID, short SlotNo) {
	startTime = CTime::GetCurrentTime(); //20251030_Mohir
	if (!g || !pRcp) {
		ASSERT(0);
		return FALSE;
	}
	if (g->bAborting) {
		return FALSE;
	}

	g->DLoc.ClearAll();

	// Passing some transient variables into recipe object [11/6/2011 Administrator]
	pRcp->SlotID = SlotNo;
	pRcp->WaferID = WaferID;
	pRcp->CarrierID = pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID + CTime::GetCurrentTime().Format("-%d%H%M%S");

	CString msg;
	msg.Format("WaferID: %s, SlotNo: %d", WaferID.GetBuffer(), SlotNo);
	Message(msg, 2);

	p413App->RecordEvent(msg);

	// Getting ready to measure [7/12/2010 128 NT]
	// Set all points to NOTMEASURE [6/14/2011 XPMUser]
	CPtrList* pMPList = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			p->ProgressSM = CMPoint::NOTMEASURE;
		}
	}

	// Measurement commences: Echo Probe First [7/12/2010 128 NT]

	//////////////////////////////////////////////////////////////////////////
	// Measure Thickness [6/14/2011 XPMUser]
	//////////////////////////////////////////////////////////////////////////

	// [20231227 ZHIMING
	if (pRcp->IsConfocalMeasurement()) {

		MParam p = pRcp->MeParam->MPa[0];
		MParam::PRBNUM Pb = p.Prb;
		//[ CHOOSING 008 OR 030 AND MAKING ADJUSTMENT BEFORE MEASUEREMNT START
		if (Pb == MParam::CONFOCAL_ROUGHNESS) { // ONLY ROUGHNESS USE 008
			//[ FOCUS 008 -> ADJUST THE 008 IF CONFOCAL_ROUGHNESS IS DEFINED --> ONLY FOR SMALL ADJUSTMENT, WON'T WORK FOR BIG Z ERROR
			p413App->pMtr->GotoXY(0, 0);
			p413App->pPSwitch->MoveToCWL008prb();
			p413App->ZFocusPos008 = pRcp->fCWLTopZ2;
			p413App->ZFocusDevRenge008 = 1;

			if (p413App->runningWaferScanCWLThread == FALSE) {
				p413App->runningWaferScanCWLThread = TRUE;
				double curZ = pRcp->fCWLTopZ2;
				for (int i = 0; i < 4; i++) { // Z POSITION ADJUSTMENT LOOP
					double topCWLVal;
					p413App->pCWL->GetMeasuredValueMRO(1, topCWLVal);
					// confocal -up/ +down
					curZ -= topCWLVal;
					p413App->pMtr->MoveCWLTopZ(curZ);
					pRcp->fCWLTopZ2 = curZ;
					Sleep(60);
				}
				p413App->runningWaferScanCWLThread = FALSE;
			}

			p413App->pMtr->GetCWLTopZPosDev(p413App->ZFocusPos008);
			pRcp->fCWLTopZ2 = p413App->ZFocusPos008; // UPDATE Z POSITION
			//]
		}
		else { // OTHER MEASUREMET USE 030
			p413App->pMtr->GotoXY(0, 0);
			p413App->pPSwitch->MoveToCWL030prb();
			p413App->ZFocusPos030 = pRcp->fCWLTopZ;
			p413App->ZFocusDevRenge030 = 1;
		}
		//]
		DoConfocalMeasurement(pRcp, pMPList, SlotNo);
	}
	// ]

	// [ 24102024/MORSALIN
	if (pRcp->IsConfocalLocalMeasurement()) {
		p413App->pPSwitch->MoveToCWL030prb();
		DoConfocalLocalMeasurement(pRcp, pMPList, SlotNo);
	}
	// ]

	if (pRcp->IsEchoMeasurement()) {
		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->EchoNormalExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		m_cImgWnd.ClearTextInfo();
		p413App->pPSwitch->MoveToECHOprb();
		DoThicknessBowWarpMeasurement(pRcp, pMPList, SlotNo);
	}
	//[ Mahedi Kamal - 03202024 
	if (pRcp->IsRoughnessMeasurement()) {
		m_cImgWnd.ClearTextInfo();
		p413App->bIsRa = pRcp->bIsRa_rcp; // 07102024 -> Mahedi Kamal
		p413App->pPSwitch->MoveToROUGHNESSprb();
		DoRoughnessMeasurement(pRcp, pMPList, SlotNo);
	}
	//]

	//[ Mahedi Kamal - 08132024 
	if (pRcp->IsRingWidthMeasurement()) {
		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->TRWMeasProp.Exposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		m_cImgWnd.ClearTextInfo();
		p413App->pPSwitch->MoveToECHOprb();
		DoRingWidthMeasurement(pRcp, pMPList, SlotNo);
	}
	//]

	//////////////////////////////////////////////////////////////////////////
	// Measure Bevel before Trim, bevel width will be use as an offset to trim width [5/31/2012 Yuen]
	if (pRcp->IsBevelMeasurement()) {
		// [6/14/2022 ZHIMING]
		if (pRcp->IsBevelUsingTF()) {
			p413App->SwitchCameraSetExposure(CCamBase::THINFCAM, pRcp->TWExposureBevel + g->BaseExpoTF, &m_cVideo, &m_cList, pRcp);
		}
		else {
			p413App->pPSwitch->MoveToECHOprb();
			p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->TWExposureBevel + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		}
		// ]
		m_cImgWnd.ClearTextInfo();
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]

		DoBevelMeasurement(pRcp, pMPList, SlotNo);
	}

	//////////////////////////////////////////////////////////////////////////
	// Measure Step and bump height
	//////////////////////////////////////////////////////////////////////////
	if (pRcp->IsHeightMeasurement()) {
		p413App->pPSwitch->MoveToCWL030prb(); //30122024
		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->EchoNormalExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		m_cImgWnd.ClearTextInfo();
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
		DoTrimMeasurement(pRcp, pMPList, SlotNo);
	}

	//////////////////////////////////////////////////////////////////////////
	// Measure Trim Width and Trim Height
	//////////////////////////////////////////////////////////////////////////
	//if (pRcp->IsEdgeMeasurement()) {
	//	p413App->pPSwitch->MoveToECHOprb(); //30122024
	//	p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->TWExposureInner + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
	//	m_cImgWnd.ClearTextInfo();
	//	bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
	//	DoTrimMeasurement(pRcp, pMPList, SlotNo);
	//}

	//////////////////////////////////////////////////////////////////////////
	// Measure Thin Film
	//////////////////////////////////////////////////////////////////////////
	if (pRcp->IsThinFilmMeasurement()) {
		p413App->SwitchCameraSetExposure(CCamBase::THINFCAM, pRcp->TFNormalExposure + g->BaseExpoTF, &m_cVideo, &m_cList, pRcp);
		m_cImgWnd.ClearTextInfo();
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
		/*DoThinFilmMeasurement(pRcp, pMPList, SlotNo);*/
		DoThinFilmMeasurement_v2(pRcp, pMPList, SlotNo);					//20220613 / yukchiu / thinfilm measurement with marker
	}

	//////////////////////////////////////////////////////////////////////////
	// Measure Multi Point
	//////////////////////////////////////////////////////////////////////////
	if (pRcp->IsMultiPointMeasurement()) {
		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->EchoNormalExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		m_cImgWnd.ClearTextInfo();
		DoMultiPointMeasurement(pRcp, pMPList, SlotNo);
	}

	StopCamera();
	pRcp->endTime = CTime::GetCurrentTime(); // 20251014_Mohir
	msg = "MeasureOneWafer done";
	Message(msg, 2);

	//[20251210_Mohir

	//// >>> AUTO-SAVE 2D MAPS FOR ALL VALID COLUMNS // 21102025_1
	if (pRcp && pRcp->Patt.MP.GetCount() > 0) {
		short start = 0;
		int totalCols = MAXMEPASET * MaXPA;

		// Get the base folder path once (without filename)
		CString folderPath = "";// Get2DMapFolderPath(pRcp, SlotNo, WaferID, g->waferLotID);
		

		for (int colIndex = 0; colIndex < totalCols; colIndex++) {
			int meSet = colIndex / MaXPA;
			int probeIdx = colIndex % MaXPA;

			if (meSet >= MAXMEPASET) continue;
			if (pRcp->MeParam[meSet].MPa[probeIdx].Prb <= 0) continue;

			// [ 10222025 MORSALIN
			if (pRcp->MeParam[meSet].MPa[probeIdx].Prb == MParam::CONFOCAL_LTV_MIN ||
				pRcp->MeParam[meSet].MPa[probeIdx].Prb == MParam::CONFOCAL_LTV_MAX ||
				pRcp->MeParam[meSet].MPa[probeIdx].Prb == MParam::CONFOCAL_LTV_AVG)
				continue;
			// ]

			// Sanitize probe name
			CString safeName = pRcp->MeParam[meSet].MPa[probeIdx].Name;
			safeName.Replace(_T(" "), _T("_"));
			safeName.Replace(_T("/"), _T("_"));
			safeName.Replace(_T("\\"), _T("_"));

			// Find the next available filename for this probe name
			CString fileName = "";// GetNextAvailableBmpNameForProbe(folderPath, safeName);

			// Create dialog and configure
			CWaferMapViewDlg dlg;
			dlg.m_waferMap.pRcp = pRcp;
			dlg.m_waferMap.m_nImageType = 1;
			dlg.m_waferMap.dmMode = CWaferMap::dmUSER;
			dlg.m_waferMap.bSiteView = m_cWaferMap.bSiteView;
			dlg.m_waferMap.bDrawPoint = FALSE;
			dlg.m_waferMap.bDrawValues = FALSE;
			dlg.m_waferMap.bDrawText = TRUE;
			dlg.m_waferMap.bDrawBackground = TRUE;
			dlg.m_waferMap.DataCol = colIndex;
			dlg.m_waferMap.MapCol = probeIdx;
			dlg.m_waferMap.Title = pRcp->MeParam[meSet].MPa[probeIdx].Name;
			dlg.m_waferMap.m_nWaferSize = pRcp->Wp.size;
			p413App->Global.currentDataCol = colIndex;// 12112025_Tushar

			// Sanitize probe name
			//CString safeName = pRcp->MeParam[meSet].MPa[probeIdx].Name;
			safeName.Replace(_T(" "), _T("_"));
			safeName.Replace(_T("/"), _T("_"));
			safeName.Replace(_T("\\"), _T("_"));

			//		// Fill stats legend (Max, Min, etc.)
			//		FillWaferMapStats(dlg.m_waferMap, pRcp);

			//		// Assign save path
			//		dlg.m_strSavePath = fileName;

					// Fill stats legend (Max, Min, etc.)
			FillWaferMapStats(dlg.m_waferMap, pRcp);

			// Assign save path
			dlg.m_strSavePath = fileName;

			p413App->Global.callFrom2DAutoSave = true;// 12112025_Tushar
			p413App->Global.isFromT = true;

			// Show for 500ms and auto-save
			dlg.DoModal();
			p413App->Global.isFromT = false;
			p413App->Global.interpolationData_map[colIndex] = p413App->Global.GeneralInterpolatedGrid;
			///+++++++++++
			/*Auto2DGlut dlg2D;
			dlg2D.DoModal();*/

			////---------


			//DiceMapTemp dlgDice;

			// Check if this is dice-type (LTV/STIR)
			
			for (int j = 0; j < MAXMEPASET && !bIsDiceType; j++) {
				CMeParam* pMeP = &pRcp->MeParam[j];
				for (int i = 0; i < MaXPA; i++) {
					if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV ||
						pMeP->MPa[i].Prb == MParam::CONFOCAL_STIR) {
						bIsDiceType = TRUE;
						break;
					}
				}
			}

			//dlg.m_cDiceMap.SetDiceType(bIsDiceType);

			if (bIsDiceType == true) {
				CString diceFolderPath = GetDiceMapFolderPath(pRcp, SlotNo, WaferID, g->waferLotID);
				CString diceFileName = GetNextAvailableBmpNameForProbe(diceFolderPath, safeName);
				StoreData();
				/*CString testSavePath = _T("C:\\DATA\\FSM\\0_30mm_LTV_STIR_TEST\\2025\\11\\TXT\\DiceMap\\STIR.bmp");
				dlgDice.m_strSavePath = testSavePath;*/
				dlgDice.m_strSavePath = diceFileName;
				g->diceMapSavePath = diceFileName;
				// Replicate GenerateMap() logic here
				//dlg.m_cDiceMap.ClearCells();

				float WaferSize = pRcp->Wp.size;
				float m_Radius = WaferSize / 2.0f;

				// Find LTV and STIR indices
				int ltvIdx = -1, stirIdx = -1;
				for (int j = 0; j < MAXMEPASET; j++) {
					CMeParam* pMeP = &pRcp->MeParam[j];
					for (int i = 0; i < MaXPA; i++) {
						if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV) ltvIdx = i;
						if (pMeP->MPa[i].Prb == MParam::CONFOCAL_STIR) stirIdx = i;
					}
				}

				// Copy global measurement strings (stt)
				//dlg.m_cDiceMap.SetMeasurementData(p413App->Global.stt);

				// Dice grid parameters
				float Ax = pRcp->AnchorX;
				float Ay = pRcp->AnchorY;
				float dieW = pRcp->DieWidth;
				float dieH = pRcp->DieHeight;

				//if (dieW <= 0 || dieH <= 0) return;

				int idx = 1;
				for (int j = -50; j <= 50; j++) {
					for (int i = -50; i <= 50; i++) {
						float bx = Ax + i * dieW;
						float by = Ay + j * dieH;

						// Check if die is on wafer
						auto isInside = [&](float x, float y) {
							return sqrt(x * x + y * y) <= m_Radius;
						};
						int cnt = 0;
						cnt += isInside(bx, by);
						cnt += isInside(bx + dieW, by);
						cnt += isInside(bx + dieW, by + dieH);
						cnt += isInside(bx, by + dieH);

						if (cnt > 0) {
							if (!pRcp->m_bIncludeOn && cnt != 4) continue;
							dlgDice.m_cDiceMap.AddCellWithData(bx, by, dieW, dieH, idx, m_Radius, ltvIdx, stirIdx, "", pRcp->MeParam[0].MPa[0].D);
							//m_cDiceMap.AddCellWithData(bx, by, dieW, dieH, idx, m_Radius, ltvIdx, stirIdx, "", pRcp->MeParam[0].MPa[0].D);
							idx++;
						}
					}
				}

				//dlgDice.m_cDiceMap.RedrawWithData();
				//dlgDice.DoModal();
			}

		}
	}

	
	// >>>


	return TRUE;
}


void CProcJob413ProcessingDlg::StoreData() {
	p413App->Global.stt.clear();
	//CPtrList* pMPList = GetMPList();
	//if (!pMPList) {
	//	return;
	//}

	//int i, j;
	//CMaxMinPa MM;
	//CRecipe413* pRcp = GetCurrentRecipe();
	//POSITION opos;
	//POSITION pos = pMPList->GetHeadPosition();
	//while (pos) {
	//	opos = pos;
	//	BOOL bDel = FALSE;
	//	CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
	//	float xPos = p->Co.x;
	//	float yPos = p->Co.y;
	//	vector<string> temp;
	//	temp.emplace_back(std::to_string(xPos));
	//	temp.emplace_back(std::to_string(yPos));

	//	if (p) {
	//		CData* d = p->GetData(0, FALSE);
	//		for (i = 0; i < MAXMEPASET; i++) {
	//			int ii = i * MaXPA;
	//			CMeParam* Me = &pRcp->MeParam[i];
	//			for (j = 0; j < MaXPA; j++) {
	//				if (Me->MPa[j].IsConfocalLocalMeasurement() ) {	
	//					if (d->Get(i * MaXPA + j) < 0) {
	//						bDel = TRUE;
	//					}
	//					else {
	//						if (Me->MPa[i].Prb == MParam::CONFOCAL_LTV) {
	//							string head = "LTV";
	//							string datapart = std::to_string(d->Get(i * MaXPA + j));
	//							string finalLTVdata = head + datapart;
	//							temp.emplace_back(finalLTVdata);
	//							//p413App->Global.stt.emplace_back(temp);
	//						}
	//						else {
	//							string head = "LTV";
	//							string datapart = "-99999.00";
	//							string finalLTVdata = head + datapart;
	//							temp.emplace_back(finalLTVdata);
	//							//p413App->Global.stt.emplace_back(temp);
	//						}

	//						if (Me->MPa[i].Prb == MParam::CONFOCAL_STIR) {
	//							string head = "STIR";
	//							string datapart = std::to_string(d->Get(i * MaXPA + j));
	//							string finalSTIRdata = head + datapart;
	//							temp.emplace_back(finalSTIRdata);
	//							//p413App->Global.stt.emplace_back(temp);
	//						}
	//						else {
	//							string head = "STIR";
	//							string datapart = "-99999.00";
	//							string finalSTIRdata = head + datapart;
	//							temp.emplace_back(finalSTIRdata);
	//							/*p413App->Global.stt.emplace_back(temp);*/
	//						}
	//						//float data = d->Get(i * MaXPA + j);
	//						/*temp.emplace_back(data);
	//						p413App->Global.stt.emplace_back(temp);*/

	//						p413App->Global.stt.emplace_back(temp);
	//					}
	//				}
	//			}
	//		}
	//		if (bDel) {
	//			pMPList->RemoveAt(opos);
	//			delete p;
	//		}
	//	}
	//	temp.clear();
	//}


	//CRecipe413* pRcp = GetCurrentRecipe();
	//CPtrList* pMPList = GetMPList();
	CPtrList* pMPList = &pRcp->Patt.MP.MPLst;
	if (!pMPList) {
		return;
	}
	//ltvIdx = -1, stirIdx = -1;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		float xPos = p->Co.x;
		float yPos = p->Co.y;
		vector<string> temp;
		temp.emplace_back(std::to_string(xPos));
		temp.emplace_back(std::to_string(yPos));
		if (!pRcp->IsValidMeSet(p)) {
			if (p->magic > 9) {
				return;
			}
		}

		// Patch [9/24/2012 Administrator]
		if (p->magic == 9) {
			p->MeSet = 1;
		}
		CData* d = p->GetData(0, FALSE);
		if (!d) {
			return;
		}
		short MeSetX = MaXPA * (p->MeSet - 1);
		CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
		//for (int j = 0; j < MaXPA; j++) {
		//	if (Me->MPa[j].Prb > 0) {
		//		for (int i = 3; i < nCjName; i++) {
		//			if (Me->MPa[j].Name.GetLength() > 0) {
		//				if (dwCjName[i] == Me->MPa[j].Name) {
		//					float fdata = d->Get(MeSetX + j);

		//					if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {
		//						string head = "LTV";
		//						string datapart = std::to_string(d->Get(MeSetX + j));
		//						string finalLTVdata = head + datapart;
		//						temp.emplace_back(finalLTVdata);
		//					}
		//					else {
		//						string head = "LTV";
		//						string datapart = "-99999.00";
		//						string finalLTVdata = head + datapart;
		//						temp.emplace_back(finalLTVdata);
		//						/*p413App->Global.stt.emplace_back(temp);*/
		//					}

		//					if (Me->MPa[j+1].Prb == MParam::CONFOCAL_STIR) {
		//						string head = "STIR";
		//						string datapart = std::to_string(d->Get(MeSetX + j+1));
		//						string finalSTIRdata = head + datapart;
		//						temp.emplace_back(finalSTIRdata);
		//					}
		//					else {
		//						string head = "STIR";
		//						string datapart = "-99999.00";
		//						string finalSTIRdata = head + datapart;
		//						temp.emplace_back(finalSTIRdata);
		//						/*p413App->Global.stt.emplace_back(temp);*/
		//					}
		//					p413App->Global.stt.emplace_back(temp);

		//				}
		//			}
		//			temp.clear();
		//		}
		//	}
		//}


		for (int j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				for (int i = 3; i < nCjName; i++) {
					if (Me->MPa[j].Name.GetLength() > 0) {
						if (dwCjName[i] == Me->MPa[j].Name) {
							/*if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {
								if (ltvIdx == -1) {
									ltvIdx = j;
								}

							}
							if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {
								if (stirIdx == -1) {
									stirIdx = j;
								}
							}*/
							//float fdata = d->Get(MeSetX + j);
							if ((Me->MPa[j].Prb == MParam::CONFOCAL_LTV) && (Me->MPa[j + 1].Prb == MParam::CONFOCAL_STIR)) {
								if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {

									string datapart = std::to_string(d->Get(MeSetX + j));
									string datapart2 = std::to_string(d->Get(MeSetX + j + 2));
									string datapart3 = std::to_string(d->Get(MeSetX + j + 3));
									string datapart4 = std::to_string(d->Get(MeSetX + j + 4));

									temp.emplace_back(datapart);
								}
								else {

									string datapart = "-99999.00";

									temp.emplace_back(datapart);
									/*p413App->Global.stt.emplace_back(temp);*/
								}

								if (Me->MPa[j + 1].Prb == MParam::CONFOCAL_STIR) {

									string datapart = std::to_string(d->Get(MeSetX + j + 1));
									temp.emplace_back(datapart);
								}
								else {

									string datapart = "-99999.00";

									temp.emplace_back(datapart);
									/*p413App->Global.stt.emplace_back(temp);*/
								}
							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_STIR) && (Me->MPa[j + 1].Prb == MParam::CONFOCAL_LTV))
							{

								if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {

									string datapart = std::to_string(d->Get(MeSetX + j));

									temp.emplace_back(datapart);
								}
								else {

									string datapart = "-99999.00";
									temp.emplace_back(datapart);
									/*p413App->Global.stt.emplace_back(temp);*/
								}

								if (Me->MPa[j + 1].Prb == MParam::CONFOCAL_LTV) {

									string datapart = std::to_string(d->Get(MeSetX + j + 1));

									temp.emplace_back(datapart);
								}
								else {

									string datapart = "-99999.00";

									temp.emplace_back(datapart);
									/*p413App->Global.stt.emplace_back(temp);*/
								}

							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_STIR) && (Me->MPa[j + 1].Prb != MParam::CONFOCAL_LTV))
							{

								if (Me->MPa[j].Prb == MParam::CONFOCAL_STIR) {

									string datapart = std::to_string(d->Get(MeSetX + j));

									temp.emplace_back(datapart);
								}


								if (Me->MPa[j + 1].Prb != MParam::CONFOCAL_LTV) {

									string datapart = "-99999.00";

									temp.emplace_back(datapart);
								}


							}

							if ((Me->MPa[j].Prb == MParam::CONFOCAL_LTV) && (Me->MPa[j + 1].Prb != MParam::CONFOCAL_STIR))
							{

								if (Me->MPa[j].Prb == MParam::CONFOCAL_LTV) {

									string datapart = std::to_string(d->Get(MeSetX + j));
									temp.emplace_back(datapart);
								}


								if (Me->MPa[j + 1].Prb != MParam::CONFOCAL_STIR) {
									string datapart = "-99999.00";
									temp.emplace_back(datapart);
								}


							}


							p413App->Global.stt.emplace_back(temp);

						}
					}
					temp.clear();
				}
			}
		}
	}



}


CString CProcJob413ProcessingDlg::GetDiceMapFolderPath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID)
{
	if (!pRcp) return _T("");
	g->diceRcpName = pRcp->RcpeName;
	g->diceSlotID = SlotID;
	g->diceWaferID = WaferID;
	g->diceLotID = LotID;

	CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
	g->pDiceJobInfo = pJobInfo;
	//if (!pJobInfo) return _T("");

	//// Use MakeCassetteName to get correct base path structure
	CCasMgr CasMgr;
	CasMgr.SetCassette(&pJobInfo->Cas);
	CString dummyC2C;
	CString OpName = _T("FSM");
	if (strlen(pGDev->username) != 0) OpName = pGDev->username;

	// Generate a dummy C2C filename to get the proper directory structure
	CasMgr.MakeCassetteName(dummyC2C, pRcp->RcpeName, _T("dummy"), pJobInfo->Cas.JobInfo.CarrierID, FALSE, FALSE, OpName);

	//// Extract the directory path from the generated C2C filename
	CString basePath = DosUtil.DirSys.ExtractFilePath(dummyC2C);
	g->diceMapSavePath = basePath;

	// Remove the "C2C" folder from the path and replace with "TXT\2D"
	int pos = basePath.ReverseFind(_T('\\'));
	if (pos != -1) {
		basePath = basePath.Left(pos); // Remove "C2C" folder

		// Create TXT\2D subdirectories
		CString txtPath = basePath + _T("\\TXT\\");
		CString folder2D = txtPath + _T("DiceMap\\");

		// Create directories using GotoDirec (which calls CreateDir)
		DosUtil.DirSys.GotoDirec(folder2D);

		return folder2D;
	}

	// Add fallback logic here if needed
	return _T("");
}

//21102025_1

CString CProcJob413ProcessingDlg::Get2DMapFolderPath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID)
{
if (!pRcp) return _T("");

CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
if (!pJobInfo) return _T("");

// Use MakeCassetteName to get correct base path structure
CCasMgr CasMgr;
CasMgr.SetCassette(&pJobInfo->Cas);
CString dummyC2C;
CString OpName = _T("FSM");
if (strlen(pGDev->username) != 0) OpName = pGDev->username;

// Generate a dummy C2C filename to get the proper directory structure
CasMgr.MakeCassetteName(dummyC2C, pRcp->RcpeName, _T("dummy"), pJobInfo->Cas.JobInfo.CarrierID, FALSE, FALSE, OpName);

// Extract the directory path from the generated C2C filename
CString basePath = DosUtil.DirSys.ExtractFilePath(dummyC2C);

// Remove the "C2C" folder from the path and replace with "TXT\2D"
int pos = basePath.ReverseFind(_T('\\'));
if (pos != -1) {
	basePath = basePath.Left(pos); // Remove "C2C" folder

	// Create TXT\2D subdirectories
	CString txtPath = basePath + _T("\\TXT\\");
	CString folder2D = txtPath + _T("2D\\");

	// Create directories using GotoDirec (which calls CreateDir)
	DosUtil.DirSys.GotoDirec(folder2D);

	return folder2D;
}

// Add fallback logic here if needed
return _T("");
}

//[20251210_Mohir
// Returns the next available filename for a specific probe name
//CString CProcJob413ProcessingDlg::GetNextAvailableBmpNameForProbe(const CString& folderPath, const CString& probeName)
//{
//	CString fileName;
//
//	// Check if the base filename exists (without number)
//	CString baseFile = folderPath + probeName + _T(".bmp");
//	if (!PathFileExists(baseFile)) {
//		// First file - no number
//		fileName = baseFile;
//	}
//	else {
//		// Find the highest number for this probe name
//		int maxNumber = 0;
//		CString searchPattern = folderPath + probeName + _T("_*.bmp");
//
//		WIN32_FIND_DATA findFileData;
//		HANDLE hFind = FindFirstFile(searchPattern, &findFileData);
//
//		if (hFind != INVALID_HANDLE_VALUE) {
//			do {
//				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//					CString filename = findFileData.cFileName;
//
//					// Remove the probe name and extension to get the number
//					// Format: {probeName}_{number}.bmp
//					CString numberPart = filename;
//					numberPart.Replace(probeName + _T("_"), _T("")); // Remove probe name
//					numberPart.Replace(_T(".bmp"), _T("")); // Remove extension
//
//					// Check if it's a valid number
//					BOOL isNumber = TRUE;
//					for (int i = 0; i < numberPart.GetLength(); i++) {
//						if (!_istdigit(numberPart[i])) {
//							isNumber = FALSE;
//							break;
//						}
//					}
//
//					if (isNumber) {
//						int currentNumber = _ttoi(numberPart);
//						if (currentNumber > maxNumber) {
//							maxNumber = currentNumber;
//						}
//					}
//				}
//			} while (FindNextFile(hFind, &findFileData));
//
//			FindClose(hFind);
//		}
//
//		// Use the next number
//		fileName.Format(_T("%s%s_%d.bmp"), folderPath, probeName, maxNumber + 1);
//	}
//
//	return fileName;
//}

///////////////////////////////////////////////////////////////////////////////////////
CString CProcJob413ProcessingDlg::GetNextAvailableBmpNameForProbe(const CString& folderPath, const CString& probeName)
{
	CString fileName;

	// FIRST check for PNG files (since that's what you want now)
	CString baseFilePNG = folderPath + probeName + _T(".png");
	if (!PathFileExists(baseFilePNG)) {
		// First PNG file - no number
		fileName = baseFilePNG;
	}
	else {
		// Find the highest number for PNG files
		int maxNumber = 0;
		CString searchPatternPNG = folderPath + probeName + _T("_*.png");

		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(searchPatternPNG, &findFileData);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					CString filename = findFileData.cFileName;

					// Remove the probe name and extension to get the number
					// Format: {probeName}_{number}.png
					CString numberPart = filename;
					numberPart.Replace(probeName + _T("_"), _T("")); // Remove probe name
					numberPart.Replace(_T(".png"), _T("")); // Remove extension

					// Check if it's a valid number
					BOOL isNumber = TRUE;
					for (int i = 0; i < numberPart.GetLength(); i++) {
						if (!_istdigit(numberPart[i])) {
							isNumber = FALSE;
							break;
						}
					}

					if (isNumber) {
						int currentNumber = _ttoi(numberPart);
						if (currentNumber > maxNumber) {
							maxNumber = currentNumber;
						}
					}
				}
			} while (FindNextFile(hFind, &findFileData));

			FindClose(hFind);
		}

		// Use the next number
		if (maxNumber == 0) {
			fileName.Format(_T("%s%s_1.png"), folderPath, probeName);
		}
		else {
			fileName.Format(_T("%s%s_%d.png"), folderPath, probeName, maxNumber + 1);
		}
	}

	return fileName;
}



void CProcJob413ProcessingDlg::FillWaferMapStats(CWaferMap& map, CRecipe413* pRcp)
{
	if (!pRcp || map.DataCol < 0) return;

	map.strList.RemoveAll();
	CPattern* pPatt = &pRcp->Patt;
	if (!pPatt) return;

	bool isRangeMeasurement = true;
	if (pRcp->IsConfocalThicknessMeasurement() || pRcp->IsEchoThicknessMeasurement()) {
		isRangeMeasurement = false; 
	}

	// Find which MeSet and probe index this DataCol belongs to
	int meSet = map.DataCol / MaXPA;
	int probeIdx = map.DataCol % MaXPA;

	if (meSet >= MAXMEPASET) return;
	CMeParam* pMeP = &pRcp->MeParam[meSet];
	if (probeIdx >= MaXPA || pMeP->MPa[probeIdx].Prb <= 0) return;

	CStat& stat = pPatt->MP.Stats[map.DataCol];
	CString str, str2;
	CString dec1;
	dec1.Format(_T("%%.%df"), pMeP->MPa[probeIdx].D);

	// Max
	str.Format(dec1, stat.m_fMax);
	str2.Format(_T("Max=%s"), str);
	map.strList.AddTail(str2);

	// Min
	str.Format(dec1, stat.m_fMin);
	str2.Format(_T("Min=%s"), str);
	map.strList.AddTail(str2);

	// Ave
	str.Format(dec1, stat.m_fAverage);
	str2.Format(_T("Ave=%s"), str);
	map.strList.AddTail(str2);

	// StDev
	str.Format(dec1, stat.m_fSTD);
	str2.Format(_T("StDev=%s"), str);
	map.strList.AddTail(str2);

	// TTV / Range
	bool isRange = isRangeMeasurement; // or use global flag if needed
	if (isRange) {
		str.Format(dec1, stat.m_fTTV);
		str2.Format(_T("Range=%s"), str);
	}
	else {
		str.Format(dec1, stat.m_fTTV);
		str2.Format(_T("TTV=%s"), str);
	}
	map.strList.AddTail(str2);

	
	str.Format(dec1, stat.m_fBow);
	str2.Format(_T("Bow=%s"), str);
	map.strList.AddTail(str2);

	str.Format(dec1, stat.m_fWarp);
	str2.Format(_T("Warp=%s"), str);
	map.strList.AddTail(str2);
	
}
//

BOOL CProcJob413ProcessingDlg::TransferCas2Stage(short slot, BOOL bVacOn) {
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return FALSE;
	}

	CString str/*, str2*/;
	str.Format("Load wafer %d from LP%d to stage", slot, ID + 1);
	Message(str);

	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
		Message("Stage didn't move to Load position correctly", 2);
		pGDev->SetAlarm(CAlrmList::STAGEMOTORERROR);
		return FALSE;
	}

	// Set before transfer for security [5/14/2013 user]
	if (p413App->CanTransfer2Stage(ID + 1, slot)) {
		g->WaferOnStage = ID + 1; // Temporary set it to true [10/7/2012 Yuen]
		Message("Set WaferOnStage");
		DosUtil.SetWaferOnStage(g->WaferOnStage);

		Message("Switch off chuck vacuum");
		pMtr->SetChuckVacuum(FALSE);

		// 		str2.Format("XFER %d SLOT %d 3 SLOT 1 ALGN", g->LPNo[ID], slot, TRUE);
		// 		Message(str2);

		CSubstObj* pS = NULL;
		if (pGDev->pOLnClt) {
			CCarrierObj* c = &pGDev->pGEMObjs->Cms[ID].CaObj;
			pS = pGDev->pOLnClt->RemoveCO(ID + 1, c, slot);
		}

		if (pGDev->TransferWafer1(g->LPNo[ID], slot, 3, 1, TRUE)) {
			if (pS) {
				pGDev->pOLnClt->ScnPlace(ID + 1, pS);
			}
			str += " done";
			Message(str);
			Message("Switch on chuck vacuum");  // Will be turn off during bow warp measurement
			SwitchOnChuckVacuum(pRcp);
			// Wafer is now confirmed to be on stage [10/7/2012 Yuen]
			return TRUE;
		}
		else {
			str += " failed";
			Message(str, 2);
			// Transfer failed, still set bWaferOnStage for safety [10/7/2012 Yuen]
			pGDev->SetAlarm(CAlrmList::WAFERHANDLERERROR);
		}
	}
	else {
		Message("Transfer not allowed", 2);
	}
	return FALSE;
}

BOOL CProcJob413ProcessingDlg::TransferStage2Cas(short slot) {
	CMotorS2* pMtr = p413App->pMtr;
	if (!pMtr) {
		return FALSE;
	}

	CString str/*, str2*/;
	str.Format("Return wafer %d to LP%d", slot, ID + 1);
	Message(str);

	if (!p413App->GotoXYS(g->LoadPos.PX, g->LoadPos.PY, CCamBase::ECHOCAM)) {
		Message("Stage didn't move to Load position correctly");
		pGDev->SetAlarm(CAlrmList::STAGEMOTORERROR);
		return FALSE;
	}

	if (p413App->CanTransfer2Cas(ID + 1, slot)) {
		Message("Switch off chuck vacuum");
		pMtr->SetChuckVacuum(FALSE);

		// 		str2.Format("XFER 3 SLOT 1 %d SLOT %d", g->LPNo[ID] + 1, slot);
		// 		Message(str2);
		CSubstObj* pS = NULL;
		if (pGDev->pOLnClt) {
			pS = pGDev->pOLnClt->RemoveScn(ID + 1);
		}
		if (pGDev->TransferWafer(3, 1, g->LPNo[ID] + 1, slot, FALSE)) {
			if (pS) {
				CCarrierObj* c = &pGDev->pGEMObjs->Cms[ID].CaObj;
				pGDev->pOLnClt->COPlace(ID + 1, slot, c, pS);
			}
			str += " done";
			Message(str);
			Message("Clear WaferOnStage");
			g->WaferOnStage = 0;	// No more wafer on stage [10/7/2012 Yuen]
			DosUtil.SetWaferOnStage(g->WaferOnStage);
			return TRUE;
		}
		else {
			str += " failed";
			Message(str);
			// Transfer failed, still assume that wafer is on stage [10/7/2012 Yuen]
			pGDev->SetAlarm(CAlrmList::WAFERHANDLERERROR);
		}
	}
	else {
		Message("Transfer not allowed");
	}
	return FALSE;
}

void CProcJob413ProcessingDlg::Message(CString msg, int lvl) {
	p413App->Message(&m_cList, msg, lvl);
}

void CProcJob413ProcessingDlg::SetupColumn() {
	int idx = 0;

	// Insert a column. This override is the most convenient.
	m_cList.InsertColumn(idx, _T("Time       "), LVCFMT_LEFT);
	m_cList.SetColumnWidth(idx++, 96 /*LVSCW_AUTOSIZE_USEHEADER*/);

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("Message");
	m_cList.InsertColumn(idx, &col);
	m_cList.SetColumnWidth(idx++, LVSCW_AUTOSIZE_USEHEADER);
}

void CProcJob413ProcessingDlg::ClearList() {
	if (m_cList.GetItemCount() > 0) {
		m_cList.DeleteAllItems();
	}
}

// BOOL CProcJob413ProcessingDlg::CanTransfer2Stage(short Lp, short slot)
// {
// 	if (pGDev->GetLPState(Lp) != PrcsMgrServices::OPENED)
// 	{
// 		Message("CanTransfer2Stage: Not possible. Carrier not open",1);
// 		return FALSE;
// 	}
// 	// Stage ready? [7/12/2010 128 NT]
// 	if (g->WaferOnStage>0)
// 	{
// 		Message("CanTransfer2Stage: Not possible. Wafer on stage",1);
// 		return FALSE;
// 	}
// 	Message("Transfer2Stage allowed");
// 	return TRUE;
// }
//
// BOOL CProcJob413ProcessingDlg::CanTransfer2Cas(short Lp, short slot)
// {
// 	CString str;
// 	if (pGDev->GetLPState(Lp) != PrcsMgrServices::OPENED)
// 	{
// 		Message("CanTransfer2Cas: Not possible. Carrier not open",1);
// 		return FALSE;
// 	}
// 	// Stage ready? [7/12/2010 128 NT]
// 	if (g->WaferOnStage == 0)
// 	{
// 		Message("CanTransfer2Cas: Not possible. No wafer on stage",1);
// 		return FALSE;
// 	}
// 	Message("Transfer2Cassette allowed");
// 	return TRUE;
// }

BOOL CProcJob413ProcessingDlg::StartEchoMotor() {
	if (bEchoMotorStarted) {
		Message("Echo motor already started");
		return TRUE;
	}
	Message("Start Echo motor");
	if (p413App->pDMC->Run()) {
		bEchoMotorStarted = TRUE;
		Message("Echo motor started");
		return TRUE;
	}
	bEchoMotorStarted = FALSE;
	Message("Echo motor start failed", 1);
	return FALSE;
}

BOOL CProcJob413ProcessingDlg::StopEchoMotor() {
	Message("Stop Echo probe");
	if (!bEchoMotorStarted) {
		Message("Echo motor already stopped");
		return TRUE;
	}
	if (p413App->pDMC->Stop()) {
		bEchoMotorStarted = FALSE;
		Message("Echo motor stopped");
		return TRUE;
	}
	Message("Echo motor stop failed", 1);
	return FALSE;
}

CRecipe413* CProcJob413ProcessingDlg::GetCurRecipe() {
	return &pUDev->pProcJobInfo[ID].Cas.Recipe413;
}

void CProcJob413ProcessingDlg::UpdateUI(CMPoint* p) {
	m_cWaferMap.Redraw();	//12012025/HAQUE/FOR SLOW REDRAW OF THE MAP
	UpdateReport(p);
}

void CProcJob413ProcessingDlg::UpdateUILocal(CMPoint* p) {
	UpdateReport(p);
}

void CProcJob413ProcessingDlg::SetupReportHeader() {
	int i, j, nSize[MAXMEPASET * MaXPA];
	int n = 0;
	dwCjName[n] = "No";		nSize[n++] = 42;
	dwCjName[n] = "X mm";	nSize[n++] = 56;
	dwCjName[n] = "Y mm";	nSize[n++] = 56;
	nCjName = 3;

	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &pRcp->MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				dwCjName[n] = Me->MPa[j].Name;
				nSize[n++] = 86;
				nCjName++;
			}
		}
	}

	m_cReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;
	for (i = 0; i < sizeof(nSize) / sizeof(nSize[0]); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i].GetBuffer(0);
		m_cReport.InsertColumn(i, &nListColumn);
	}
}

void CProcJob413ProcessingDlg::UpdateReport(CMPoint* p) {
	int cnt = m_cReport.GetItemCount();
	CString str;
	str.Format("%d", p->Co.n);
	int idx = m_cReport.InsertItem(cnt, str);

	m_cReport.SetItemText(idx, 1, p->GetXCoorStr());
	m_cReport.SetItemText(idx, 2, p->GetYCoorStr());
	CData* d = p->GetData(0, FALSE);
	short MeSetX = MaXPA * (p->MeSet - 1);
	CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
	for (int j = 0; j < MaXPA; j++) {
		if (Me->MPa[j].Prb > 0) {
			for (int i = 3; i < nCjName; i++) {
				if (dwCjName[i] == Me->MPa[j].Name) {
					float fdata = d->Get(MeSetX + j);
					if (fdata != BADDATANEG) {
						CString dec1;
						dec1.Format("%%.%df", Me->MPa[j].D);
						str.Format(dec1, fdata);
					}
					else {
						str = "N/A";
					}
					m_cReport.SetItemText(idx, i, str);
					break;
				}
			}
		}
	}
	m_cReport.EnsureVisible(idx, FALSE);
}

BOOL CProcJob413ProcessingDlg::SaveData(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID, BOOL bCompleted, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) 
{
	CCasMgr CasMgr;
	CString filename;

	CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
	if (!pJobInfo) {
		ASSERT(0);
		return FALSE;
	}
	CasMgr.SetCassette(&pJobInfo->Cas);
	//[[20251020_Mohir
	/*if (pRcp->IsStressMeasurement()) {
		CasMgr.AutoSave(filename, SlotID, WaferID, LotID, bCompleted, FALSE); 
	}
	else {
		CasMgr.AutoSave(filename, SlotID, WaferID, LotID, bCompleted, TRUE); 
	}*/
	if (pRcp->IsStressMeasurement()) {
		CasMgr.AutoSave(filename, SlotID, WaferID, LotID, bCompleted, FALSE, WaferType, WaferSize, Station, startTime, endTime); //20251030_Mohir
	}
	else {
		CasMgr.AutoSave(filename, SlotID, WaferID, LotID, bCompleted, FALSE, WaferType, WaferSize, Station, startTime, endTime); //20251030_Mohir
  	}

	m_strLastSavedC2CPath = filename;
	//]]
 

	Message("Saving ...");
	Message(filename);

	sAutoSavePath = filename.Left(filename.ReverseFind('\\')); // For easy access 3D export
	sAutoSavePath = sAutoSavePath.Left(sAutoSavePath.ReverseFind('\\')); // to get to folder that contains C2C and TXT folder
	sAutoSavePath = sAutoSavePath + "\\TXT\\3D\\"; // to get to TXT folder
 


	return TRUE;
}

BOOL CProcJob413ProcessingDlg::ReloadRecipe() {
	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	BOOL OK = FALSE;
	CString filename;

	if (strlen(pRcp->RcpeName) > 0) {
		filename = DosUtil.GetRecipeDir();
		filename += "\\";
		filename += pRcp->RcpeName;
		filename += ".RCP";
		CString str;
		str.Format("Reload recipe %s", filename.GetBuffer());
		Message(str);
		if (pRcp->Load(filename)) {
			OK = TRUE;
		}
		else if (pRcp->LoadRecipe()) {
			OK = TRUE;
		}
	}
	else if (pRcp->LoadRecipe()) {
		OK = TRUE;
	}

	if (!OK) {
		Message("Could not load recipe", 1);
	}
	return OK;
}

// void CProcJob413ProcessingDlg::SwitchCameraSetExposure(CCamBase::CAMERAID view, CRecipe413 *pRcp, int Exposure)
// {
// 	p413App->SwitchCameraSetExposure(view,Exposure,&m_cVideo,&m_cList,pRcp);
// 	//UpdateTargets();
// }

void CProcJob413ProcessingDlg::SendPointToHost(CRecipe413* pRcp, CMPoint* pMP, short SlotNo) {
	if (CHsmsParaMeter::IsOnline()) {
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		CMeParam* pMe = &pRcp->MeParam[pMP->MeSet - 1];
		for (int i = 0; i < MaXPA; i++) {
			// If there is some item to send [9/5/2012 Administrator]
			if ((pMe->MPa[i].Prb != 0) && (pMe->MPa[i].bHost)) {
				// GemFactory will decide what to send [9/5/2012 Administrator]
				CEvt* pEvt = new CEvt;
				pEvt->pMeParam = pMe;
				pEvt->pMPoint = pMP;
				pEvt->pRcp = pRcp;
				pEvt->Set(EVT_CMS, CEID_LIST::RESULTSINGLE);
				pGDev->SetActivePort(ID + 1);
				pGDev->SetActiveSubstrate(SlotNo);
				CSharedMemQue EvtQue;
				EvtQue.WriteQue(pEvt);
				pGDev->ClearActiveSubstrate();
				pGDev->ClearActivePort();
				break;	// Break out because GemFactory would have send all the rest [9/5/2012 Administrator]
			}
		}
	}
}

void CProcJob413ProcessingDlg::SendWaferDataToHost(CRecipe413* pRcp, short SlotNo) {
	if (CHsmsParaMeter::IsOnline()) {
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		CEvt* pEvt = new CEvt;
		pEvt->pRcp = pRcp;
		pEvt->Set(EVT_CMS, CEID_LIST::RESULTWAFER);
		pGDev->SetActivePort(ID + 1);
		pGDev->SetActiveSubstrate(SlotNo);
		CSharedMemQue EvtQue;
		EvtQue.WriteQue(pEvt);
		pGDev->ClearActiveSubstrate();
		pGDev->ClearActivePort();
	}
}

void CProcJob413ProcessingDlg::OnPausevideo() {
	if (bPause) {
		bPause = FALSE;
		m_cPauseVideo.SetWindowText("Pause\nVideo");
	}
	else {
		bPause = TRUE;
		m_cPauseVideo.SetWindowText("Start\nVideo");
	}
	m_cVideo.bPause = bPause;
}

void CProcJob413ProcessingDlg::OnMonitor() {
	if (bMonitor) {
		bMonitor = FALSE;
		m_cMonitor.SetWindowText("Start Monitor");
		m_cPauseVideo.EnableWindow(TRUE);
	}
	else {
		bPause = TRUE; OnPausevideo(); // Must not pause during monitoring [5/17/2011 C2C]

		bMonitor = TRUE;
		m_cMonitor.SetWindowText("Stop Monitor");
		m_cPauseVideo.EnableWindow(FALSE);
	}
	if (pRcp) {
		pRcp->bEchoPeakMonitor = bMonitor;
	}
}

void CProcJob413ProcessingDlg::OnSwitchview() {
	switch (Show) {
	case 0:
		Show = 1;
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
		break;
	case 1:
		Show = 2;
		m_cWaferMap.m_nImageType = 1;
		m_cWaferMap.MapCol = 0;
		//m_cWaferMap.DataCol = 0;
		m_cWaferMap.dmMode = CWaferMap::dmUSER;
		break;
	default:
		Show = 0;
		m_cWaferMap.m_nImageType = 0;
		m_cWaferMap.dmMode = CWaferMap::dmENGINNER;
		break;
	}
	m_cWaferMap.Redraw();
}

void CProcJob413ProcessingDlg::OnColumnclickReport(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iSubItem >= 3) {
		short start = 0;
		m_cWaferMap.MapCol = pNMListView->iSubItem - 3;

		int i = 0;
		do {
			m_cWaferMap.DataCol = pRcp->MeParam[i].GetDataCol(m_cWaferMap.MapCol, start) + i * MaXPA;
		} while (m_cWaferMap.DataCol == -1 && i < MAXMEPASET);

		if (m_cWaferMap.DataCol != -1) {
			m_cWaferMap.Title = dwCjName[m_cWaferMap.MapCol + 3];
		}
		else {
			m_cWaferMap.Title = "";
		}
		m_cWaferMap.Redraw();
	}

	*pResult = 0;
}

void CProcJob413ProcessingDlg::Abort() {
	if (g) {
		g->bAborting = TRUE;
		g->WaferRepeat = 0;
		m_cAbort.SetWindowText("Aborting");
		Message("Aborting", 2);
	}
}

// Record an event in event log
// void CProcJob413ProcessingDlg::RecordEvent(CString strEvent)
// {
// 	CString mdir;
// 	DosUtil.GetProgramDir(mdir);
// 	mdir += "PROCESSEVENT.LOG";
// 	FILE * fp;
// 	fp = fopen(mdir, "ab");
// 	if (fp)
// 	{
// 		fprintf(fp, "%s %s\r\n", p413App->GetTimeStamp().GetBuffer(0), strEvent.GetBuffer(0));
// 		fclose(fp);
// 	}
// }

BOOL CProcJob413ProcessingDlg::InitializeEchoMeasurement(CRecipe413* pRcp, BOOL& bDoNotMoveZProbe, CMPoint* p, MParam::PRBNUM& measurement) {
	CString msg;
	CPattern* pPatt = &pRcp->Patt;

	// First point of measurement [2/28/2012 Administrator]
	Message("Thickness measurement start", 2);
	// Probe goto echo position [7/13/2010 C2C]
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		Message("Move probe to Echo Position");
		if (!p413App->GotoPositionecho()) {
			Message("P Motor error", 0);
			pGDev->SetAlarm(CAlrmList::PMOTORERROR);
			Abort();
			return FALSE;
		}
	}

	if (g->bAborting) {
		return FALSE;
	}

	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		Message("Turn on illumination");
		pMtr->SetMicroscopeLight(TRUE);
	}

	// Perform dual probe calibration if total thickness measurement [2/28/2012 Administrator]
	if (pRcp->IsTotalThickness()) {
		bDoNotMoveZProbe = TRUE;
		Message("Total thickness measurement");
		// perform dual probe calibration [8/10/2010 XPMUser]
		Message("Reposition probe");
		float vZ1 = pRcp->fEchoTopZ;
		//float vZBottom = pRcp->fEchoBottomZ;
		if (pRcp->IsAutoFocus()) {
			vZ1 = g->CalZPosEcho;
		}
		if (pMtr) {
			pMtr->MoveEchoTopZ(vZ1);
			//pMtr->MoveEchoBottomZ(vZBottom);
			if (g->bAborting) {
				return FALSE;
			}
			Message("Move to calibration position");
			p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::ECHOCAM);
			pMtr->WaitEchoTopZ();
			//pMtr->WaitEchoBottomZ();
		}
		Message("Perform dual probe calibration");
		CPeakParam PParam1;
		PParam1.Clear();
		PParam1.nPeakT = 2;
		PParam1.HeightT[0].Set(0, 256);
		PParam1.HeightT[1].Set(1, 256);
		PParam1.nPeakB = 2;
		PParam1.HeightB[0].Set(0, 256);
		PParam1.HeightB[1].Set(1, 192);
		PParam1.Method = CPeakParam::cmGREATER;
		if (g->bAborting) {
			return FALSE;
		}
		float ZPos = 0;
		p413App->pMtr->GetEchoTopZPosDev(ZPos);	// Get from encoder [5/2/2013 user]
		long start = clock();
		p413App->DoDualProbeCalibration(g->DPGageThickness, ZPos, pRcp, p->MeSet, &m_cList, &m_cVideo, &PParam1, FALSE, 0);
		dpTime = clock() - start;
		if (pRcp->IsAutoFocus()) {
			bDoNotMoveZProbe = FALSE;
			//pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
		}
		else {
			bDoNotMoveZProbe = TRUE;
		}
	}

	// First time profile measurement initialization [2/28/2012 Administrator]
	if (pRcp->IsProfileMeasurement()) {
		if (pRcp->IsEchoBowWarpMeasurement()) {
			measurement = MParam::BOWWARP1;
			Message("Profile (Bow & warp) measurement");
		}
		if (!bDoNotMoveZProbe) {
			bDoNotMoveZProbe = TRUE;
			Message("Reposition probe");
			if (pMtr && (pRcp->fEchoTopZ != p413App->fCurEchoTopZPos)) {
				pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
			}
		}
	}
	if (pRcp->bSaveVideo) {
		// Select a new file directory [2/28/2012 Administrator]
		m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("E%m%d%H%M%S").GetBuffer());
	}
	return TRUE;
}

// Entry point to TrimMeasurement [4/17/2013 Yuen]
void CProcJob413ProcessingDlg::DoTrimMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	int n = 1;
	CString msg;
	BOOL bFirst = TRUE;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		// Only allow TRIM measurement to pass [5/30/2012 Yuen]
		if (!p || !(pRcp->IsEdgeMeasurement(p) || pRcp->IsHeightMeasurement(p))) {
			continue;
		}
		n++;
		// For simulation purposes only [10/27/2011 Administrator]
		if (p413App->pEcho->bSimu) {
			p413App->DefinePeakPosition3(p, 0);
		}
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
		if (bFirst) {
			bFirst = FALSE;
			Message("Trim measurement start", 2);
			if (pMtr) {
				Message("Turn on microscope light");
				pMtr->SetMicroscopeLight(TRUE);
			}

			if (pRcp->IsProbe(MParam::TRENCHBOTPRB) || pRcp->IsProbe(MParam::TRENCHTOPPRB) ||
				pRcp->IsProbe(MParam::BUMPHEIGHTBPRB) || pRcp->IsProbe(MParam::BUMPHEIGHTTPRB)) {
				Message("Trench and Bump measurement");
				if (pMtr) {
					msg.Format("Move to fEchoTopZ %.3f", pRcp->fEchoTopZ);
					Message(msg);
					if (!pMtr->MoveEchoTopZ(pRcp->fEchoTopZ, FALSE)) {
						Message("Move Z1 FAILED");
						Message("DoTrimMeasurement aborted");
						return;
					}
					pMtr->WaitEchoTopZ();
				}
			}

			BOOL bIsTrimW = pRcp->IsProbe(MParam::TRIMWHPROB);

			if (bIsTrimW || pRcp->IsProbe(MParam::STEPHEIGHTCOMPRB)) {
				float ZPos = 0;
				Message("TrimWH or StepHeightCombo measurement");
				// DualProbe Calibration required [4/18/2013 Administrator]
				if (pMtr) {
					msg.Format("Move to fEchoTopZ+TrimHeightZOffset: %.3f", pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
					Message(msg);
					if (!pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + pRcp->TrimHeightZOffset, FALSE)) {
						Message("Move Z1 FAILED");
						Message("DoTrimMeasurement aborted");
						return;
					}
					msg.Format("Move to calibration position %.4f, %.4f", g->DPPos.X, g->DPPos.Y);
					Message(msg);
					p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::ECHOCAM);
					pMtr->WaitEchoTopZ();
					if (g->bAborting) {
						return;
					}
					pMtr->GetEchoTopZPosDev(ZPos);
				}
				Message("Perform dual probe calibration");
				CPeakParam PParam1;
				PParam1.Clear();
				PParam1.nPeakT = 2;
				PParam1.HeightT[0].Set(0, 256);
				PParam1.HeightT[1].Set(1, 256);
				PParam1.nPeakB = 2;
				PParam1.HeightB[0].Set(0, 256);
				PParam1.HeightB[1].Set(1, 192);
				PParam1.Method = CPeakParam::cmGREATER;

				long start = clock();
				Message("Perform dual probe calibration");
				p413App->DoDualProbeCalibration(g->DPGageThickness, ZPos, pRcp, p->MeSet, &m_cList, &m_cVideo, &PParam1, (bIsTrimW && g->bResetX), 0);
				dpTime = clock() - start;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//p413App->pEcho->OpenDump(&gpDump,n);
		m_cVideo.ScanLineReady = 1; // Setting will disappear when dialog is destroyed
		pointtime = clock();
		bMeasureOn = TRUE;
		//m_cImgWnd.bScaled = TRUE;
		// Z1 is at pRcp->fEchoTopZ+pRcp->TrimHeightZOffset [6/30/2014 C2C]
		if (!p413App->MeasureTrim(pRcp, p, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cProfile, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					Message("Aborting: Reached Fail Count limit", 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		// post measurement calculation [3/11/2010 Valued Customer]
		DoMathCalculation(pRcp, pMPList);
		Message("Report to host");
		SendPointToHost(pRcp, p, SlotNo);
		UpdateUI(p);
	}
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}
	Message("Trim measurement end", 2);
}

// Measures all points [5/16/2013 user]
void CProcJob413ProcessingDlg::DoThinFilmMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	int n = 0;
	CString msg;
	BOOL bFirst = TRUE;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	// [01242024 ZHIMING
	if (p413App->pMtr) {
		p413App->pMtr->SetChuckVacuum(TRUE, pRcp->Wp.size);
	}
	// ]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsThinFilmMeasurement(p))) {
			continue;
		}
		n++;
		if (bFirst) {
			m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
			Message("Thin Film measurement start", 2);
			bFirst = FALSE;
			
			// [20220607/yukchiu / use echo probe update points location
			if (!DoEchoProbePositionMarkers_v2(pRcp, pMPList, pPatt, SlotNo)) {
				Message("Could not located marker, abort measurement");
				Abort();
				continue;
			}
			// ]

			// Probe goto thin film position [7/13/2010 C2C]
			if (pMtr) {
				Message("Turn on spectrometer source");
				pMtr->SetSpectroSourceLight(TRUE);
				//Message("Turn off spectrometer illumination");
				//pMtr->SetSpectroIllumLight(FALSE);
			}
			Message("Move to thin film position");
			if (!p413App->GotoPositionthinfilm()) {
				Message("Motion error", 0);
				pGDev->SetAlarm(CAlrmList::PMOTORERROR);
				Abort();
				continue;
			}

			if (pMtr) {
				Message("Move to CalTFPos");
				pMtr->MoveThinFilmZ(g->CalZPosTF);
			}

			CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
			p413App->pOceanOp->SetParam(Me->Ta.Method, 1, Me->Ta.IntegTime, Me->Ta.Boxcar, g->TFScalingFactor);

			// alway take background and reference just in case [10/25/2012 FSM413]
			msg.Format("Take background: Exposure %d", pRcp->TFTakeSpecExposure + g->BaseExpoTF);
			Message(msg);
			if (!p413App->TakeBackground(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
				m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
				Message("Could not take background");
				Abort();
				continue;
			}
			msg.Format("Take reference: Exposure %d", pRcp->TFNormalExposure + g->BaseExpoTF);
			Message(msg);
			if (!p413App->TakeReference(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
				m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
				Message("Could not take reference");
				Abort();
				continue;
			}
			m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);

			// [20220502/yukchiu
			pMtr->SetSpectroSourceLight(FALSE);
			if (!DoThinFilmPositionMarkers_v2(pRcp, pMPList, pPatt, SlotNo)) {
				Message("Could not located marker, abort measurement");
				Abort();
				continue;
			} 
		}

		p->ProgressSM = CMPoint::NOTMEASURE;
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point [10/25/2012 FSM413]
		msg.Format("TF: Measuring point %d (%d)", p->Co.n, n);

		//20130827-1 SCOTT ADD -------------------------------------------------
		// Register coordinate of first point to calculate other posint's offset
		//if( p->Co.n == 1) {
		//	p413App->Global.fGPatOrgX = p->Co.x;
		//	p413App->Global.fGPatOrgY = p->Co.y;
		//}
		//--------------------------------------------------------------------

		Message(msg);
		p413App->bLoadDIB = TRUE;
		if (!p413App->MeasureThinFilm_v2(pRcp, p, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					Message("Aborting: Reached Fail Count limit", 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
				}
			}
			msg.Format("Fail to measure point %.3f,%.3f", p->Co.x, p->Co.y);
			Message(msg);
			Message("Continue to next location");
			continue;
		}
		bMeasureOn = FALSE;
		// post measurement calculation [3/11/2010 Valued Customer]
		DoMathCalculation(pRcp, pMPList);
		//////////////////////////////////////////////////////////////////////////
		Message("Report to host");
		SendPointToHost(pRcp, p, SlotNo);
		UpdateUI(p);
	}

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}

	if (pMtr) {
		pMtr->SetSpectroSourceLight(FALSE);
		pMtr->SetSpectroIllumLight(FALSE);
	} // Just in case [5/7/2011 FSMT]
	Message("Thin Film measurement end", 2);
}


//20220613 / yukchiu / thinfilm measurement with marker
void CProcJob413ProcessingDlg::DoThinFilmMeasurement_v2(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	int n = 0;
	CString msg;
	BOOL bFirst = TRUE;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	SwitchOnChuckVacuum(pRcp);					//20220221 / yukchiu

	//20220613 / yukchiu

	if (!DoEchoProbePositionMarkers_v2(pRcp, pMPList, pPatt, SlotNo)) {		
		Message("Could not located marker, abort measurement");
		Abort();
	}

	p413App->SwitchCameraSetExposure(CCamBase::THINFCAM, pRcp->TFNormalExposure + g->BaseExpoTF, &m_cVideo, &m_cList, pRcp);
	//=================

	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsThinFilmMeasurement(p))) {
			continue;
		}
		n++;
		if (bFirst) {
			m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
			Message("Thin Film measurement start", 2);
			bFirst = FALSE;

			// Probe goto thin film position [7/13/2010 C2C]
			if (pMtr) {
				Message("Turn on spectrometer source");
				pMtr->SetSpectroSourceLight(TRUE);
			}
			Message("Move to thin film position");
			if (!p413App->GotoPositionthinfilm()) {
				Message("Motion error", 0);
				pGDev->SetAlarm(CAlrmList::PMOTORERROR);
				Abort();
				continue;
			}

			if (pMtr) {
				Message("Move to CalTFPos");
				pMtr->MoveThinFilmZ(g->CalZPosTF);
			}

			CMeParam* Me = &pRcp->MeParam[p->MeSet - 1];
			p413App->pOceanOp->SetParam(Me->Ta.Method, 1, Me->Ta.IntegTime, Me->Ta.Boxcar, g->TFScalingFactor);

			// alway take background and reference just in case [10/25/2012 FSM413]
			msg.Format("Take background: Exposure %d", pRcp->TFTakeSpecExposure + g->BaseExpoTF);
			Message(msg);
			if (!p413App->TakeBackground(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
				m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
				Message("Could not take background");
				Abort();
				continue;
			}
			msg.Format("Take reference: Exposure %d", pRcp->TFNormalExposure + g->BaseExpoTF);
			Message(msg);
			if (!p413App->TakeReference(pRcp, &m_cVideo, pRcp->TFTakeSpecExposure + g->BaseExpoTF, pRcp->TFNormalExposure + g->BaseExpoTF)) {
				m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);
				Message("Could not take reference");
				Abort();
				continue;
			}
			m_cVideo.SetExposure(pRcp->TFNormalExposure + g->BaseExpoTF);

			pMtr->SetSpectroSourceLight(FALSE);				//20220502/yukchiu
		}

		p->ProgressSM = CMPoint::NOTMEASURE;
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point [10/25/2012 FSM413]
		msg.Format("TF: Measuring point %d (%d)", p->Co.n, n);

		Message(msg);
		p413App->bLoadDIB = TRUE;
		if (!p413App->MeasureThinFilm_v2(pRcp, p, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					Message("Aborting: Reached Fail Count limit", 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
				}
			}
			msg.Format("Fail to measure point %.3f,%.3f", p->Co.x, p->Co.y);
			Message(msg);
			Message("Continue to next location");
			continue;
		}
		bMeasureOn = FALSE;
		// post measurement calculation [3/11/2010 Valued Customer]
		DoMathCalculation(pRcp, pMPList);
		//////////////////////////////////////////////////////////////////////////
		Message("Report to host");
		SendPointToHost(pRcp, p, SlotNo);
		UpdateUI(p);
	}

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}

	if (pMtr) {
		pMtr->SetSpectroSourceLight(FALSE);
		pMtr->SetSpectroIllumLight(FALSE);
	} // Just in case [5/7/2011 FSMT]
	Message("Thin Film measurement end", 2);
}
//[ Mahedi Kamal 03202024
BOOL CProcJob413ProcessingDlg::DoRoughnessMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	MParam::PRBNUM measurement = MParam::NOPRB;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		BOOL bDoNotMoveZProbe = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsRoughnessMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		if (bFirst) {
			if (pMtr) pMtr->MoveRoughZ(pRcp->fRoughZ);
			p413App->PerformRoughnessCalibration(pRcp, &m_cList);
			bFirst = FALSE;
		}

		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		Message(msg);

		//////////////////////////////////////////////////////////////////////////
		//p413App->pEcho->OpenDump(&gpDump,n);
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		//if (!p413App->MeasureOnePointEcho(pRcp, p, code, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, bDoNotMoveZProbe)) {
		if (!p413App->MeasureOnePointRough(pRcp, p, code, pRcp->nTrialEcho, &m_cVideo, /*64*/ 64, &m_cImgWnd, &m_cList, TRUE)) {    /* pRcp->nTrialEcho need to be changed --- mahedi kamal  */
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Reached Fail Count limit";
					Message(msg, 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement calculation, updated every point for live effect only [3/11/2010 Valued Customer]
			DoMathCalculation(pRcp, pMPList);
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//////////////////////////////////////////////////////////////////////////

		UpdateUI(p);
	}

	pMtr->MoveRoughZ(0);

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	// All point measured [2/28/2012 Administrator]
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}

	msg = "Roughness measurement end";
	Message(msg, 2);
	return TRUE;
}
//]

//08142024 - START
BOOL CProcJob413ProcessingDlg::DoRingWidthMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	if (!bEchoMotorStarted) StartEchoMotor();
	p413App->pEcho->bSignal = TRUE;

	if (!p413App->PreMeasureRingWidth(pRcp, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList)) {
		msg = "Pre Processing And Bevel Width Measurement Failed";
		Message(msg, 2);
		return FALSE;
	}

	g->TaikoPointCount = 0;
	POSITION pos = pMPList->GetHeadPosition();
	CMPoint* p = NULL;
	while (pos && (!g->bAborting)) {
		p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsRingWidthMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		Message(msg);

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		if (!p413App->MeasureOnePointRingWidth(pRcp, p, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, code)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Reached Fail Count limit";
					Message(msg, 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement calculation, updated every point for live effect only [3/11/2010 Valued Customer]
			DoMathCalculation(pRcp, pMPList);
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//////////////////////////////////////////////////////////////////////////

		UpdateUI(p);
	}

	if (bEchoMotorStarted) StopEchoMotor();
	p413App->pEcho->bSignal = FALSE;

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	p413App->PostMeasureRingWidth(&m_cList);

	// All point measured [2/28/2012 Administrator]
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}

	if (!pMtr->GotoXY(0, 0)) { msg = "Goto Origin FAILED"; Message(&m_cList, msg, 2); return FALSE; };
	msg = "Ring Width Measurement End";
	Message(msg, 2);
	return TRUE;
}

//08142024 - END

BOOL CProcJob413ProcessingDlg::DoThicknessBowWarpMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	MParam::PRBNUM measurement = MParam::NOPRB;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	// Loading baseline information [11/2/2011 Administrator]
	if (pRcp->IsProfileMeasurement()) {
		if (pRcp->BaseLineFile.GetLength() > 0) {
			msg.Format("Add baseline info (%s) to recipe", pRcp->BaseLineFile.GetBuffer());
		}
		else {
			msg = "No baseline file specified";
		}
		Message(msg);
		pRcp->AddBaseline();
	}

	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		BOOL bDoNotMoveZProbe = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsEchoMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		// Found one point for Echo measurement [6/14/2011 XPMUser]
		if (bFirst) {
			bFirst = FALSE;
			if (!InitializeEchoMeasurement(pRcp, bDoNotMoveZProbe, p, measurement)) {
				msg = "Fail Initialization";
				Message(msg);
				return FALSE;
			}
			// [20220603/yukchiu
			/*DoEchoProbePositionMarkers(pRcp, pMPList, pPatt, SlotNo);*/
			DoEchoProbePositionMarkers_v2(pRcp, pMPList, pPatt, SlotNo);
			// ]
		}
		// First point initialization end [2/28/2012 Administrator]

		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		//20130827-1 SCOTT ADD -------------------------------------------------
		// Register coordinate of first point to calculate other posint's offset
		//if( p->Co.n == 1) {
		//	p413App->Global.fGPatOrgX = p->Co.x;
		//	p413App->Global.fGPatOrgY = p->Co.y;
		//}
		//--------------------------------------------------------------------
		Message(msg);

		// For simulation purposes only [10/27/2011 Administrator]
		if (p413App->pEcho->bSimu) {
			p413App->DefinePeakPosition0(0);
		}

		//////////////////////////////////////////////////////////////////////////
		//p413App->pEcho->OpenDump(&gpDump,n);
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		if (!p413App->MeasureOnePointEcho(pRcp, p, code, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, bDoNotMoveZProbe)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Reached Fail Count limit";
					Message(msg, 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement calculation, updated every point for live effect only [3/11/2010 Valued Customer]
			DoMathCalculation(pRcp, pMPList);
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		UpdateUI(p);
	}

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	// All point measured [2/28/2012 Administrator]
	// p413App->pEcho->SimuPkNum = 0; // For simulation only [10/27/2011 Administrator]
	// Adjust bow warp height [8/21/2011 FSMT]
	FILE* fp = NULL;
	//fp = fopen("C:\\TEMP\\BOW.CSV","wb");
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				if (pMeP->MPa[i].IsEchoBowWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					if (pMeP->MPa[i].Prb == MParam::BOWWARP1) {
						pPatt->MP.InvertSurfaceToTopProbeA(ix, fp);
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				else if (pMeP->MPa[i].IsProbeToSurface() && pMeP->MPa[i].Prb == MParam::ECHO1PRB) {
					pPatt->MP.InvertSurfaceToTopProbeA(ix, fp);
				}
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}
	if (fp) {
		fclose(fp);
	}

	msg = "Thickness measurement end";
	Message(msg, 2);

	return TRUE;
}

BOOL CProcJob413ProcessingDlg::DoThicknessBowWarpMeasurement_v2(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	MParam::PRBNUM measurement = MParam::NOPRB;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	// Loading baseline information [11/2/2011 Administrator]
	if (pRcp->IsProfileMeasurement()) {
		if (pRcp->BaseLineFile.GetLength() > 0) {
			msg.Format("Add baseline info (%s) to recipe", pRcp->BaseLineFile.GetBuffer());
		}
		else {
			msg = "No baseline file specified";
		}
		Message(msg);
		pRcp->AddBaseline();
	}

	//20220601/yukchiu / change to if statement & run 2 marker
	if (!DoEchoProbePositionMarkers_v2(pRcp, pMPList, pPatt, SlotNo)) {
		Message("Could not located marker, abort measurement");
		Abort();
	}

	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		BOOL bDoNotMoveZProbe = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsEchoMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		if (bFirst) {
			bFirst = FALSE;
			if (!InitializeEchoMeasurement(pRcp, bDoNotMoveZProbe, p, measurement)) {
				msg = "Fail Initialization";
				Message(msg);
				return FALSE;
			}
		}
		// First point initialization end [2/28/2012 Administrator]
		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		Message(msg);

		// For simulation purposes only [10/27/2011 Administrator]
		if (p413App->pEcho->bSimu) {
			p413App->DefinePeakPosition0(0);
		}
		
		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		if (!p413App->MeasureOnePointEcho(pRcp, p, code, pRcp->nTrialEcho, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, bDoNotMoveZProbe)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Reached Fail Count limit";
					Message(msg, 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement calculation, updated every point for live effect only [3/11/2010 Valued Customer]
			DoMathCalculation(pRcp, pMPList);
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		UpdateUI(p);
	}

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	// All point measured [2/28/2012 Administrator]
	// p413App->pEcho->SimuPkNum = 0; // For simulation only [10/27/2011 Administrator]
	// Adjust bow warp height [8/21/2011 FSMT]
	FILE* fp = NULL;
	//fp = fopen("C:\\TEMP\\BOW.CSV","wb");
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				if (pMeP->MPa[i].IsEchoBowWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					if (pMeP->MPa[i].Prb == MParam::BOWWARP1) {
						pPatt->MP.InvertSurfaceToTopProbeA(ix, fp);
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				else if (pMeP->MPa[i].IsProbeToSurface() && pMeP->MPa[i].Prb == MParam::ECHO1PRB) {
					pPatt->MP.InvertSurfaceToTopProbeA(ix, fp);
				}
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}
	if (fp) {
		fclose(fp);
	}

	msg = "Thickness measurement end";
	Message(msg, 2);

	return TRUE;
}

// [ 24102024/MORSALIN
BOOL CProcJob413ProcessingDlg::DoConfocalLocalMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo)
{
	p413App->pPSwitch->MoveToCWL030prb();
	p413App->pMtr->MoveCWLTopZ(pRcp->fCWLTopZ);

	if (pRcp->IsConfocalLocalMeasurement())
	{
		if (p413App->Global.bAborting)
			return FALSE;
		if (p413App->pMtr) {
			Message("Move to calibration position");
			p413App->GotoXYS(0.0, 0.0, CCamBase::CWL);
			
			p413App->pMtr->WaitCWLTopZ();
			Message("Set zero plane");
			p413App->pCWL2->SetDisplacementMode(1);
		//	p413App->pCWL2->SetZero(1); //MASRUR 20250317 SETZERO

			if (g->bDualConfocal) {
				p413App->pCWL2->SetDisplacementMode(2);
				//p413App->pCWL2->SetZero(2); //MASRUR 20250317 SETZERO
				// set constant k which is defined like echo calibration thickness;
				double CWLVal, finalCWL = 0.0;
				for (int i = 0; i < 8; i++) {
					p413App->pCWL2->GetMeasuredValueMRO(1, CWLVal);
					finalCWL += CWLVal;
				}
				p413App->pCWL2->SetCALKValue((finalCWL / 8.0) / 1000.0);
			}
		}
		Message("Initialization for local confocal measurement done.");
	}

	vector<double> LTV, STIR;
	double TTV;
	//p413App->Global.finalLTV.clear();
	
	p413App->Global.stopScan = false;
	p413App->Global.LineCoordinate.clear();
	Message("Start full wafer scan.");
	p413App->Global.m_LSFOn = pRcp->m_LSFOn;
	p413App->FullWaferScan(pRcp);
	/*while (p413App->Global.finalLTV.size()<4) {
		continue;
	}*/
	LTV = p413App->Global.finalLTV;
	STIR = p413App->Global.finalSTIR;

	//LTV = p413App->CalculateLTV();
	//TTV = p413App->CalculateTTV();
	//STIR = p413App->CalculateSTIR();


	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	MParam::PRBNUM measurement = MParam::NOPRB;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;


	// Loading baseline information [11/2/2011 Administrator]
	if (pRcp->IsProfileMeasurement()) {
		if (pRcp->BaseLineFile.GetLength() > 0) {
			msg.Format("Add baseline info (%s) to recipe", pRcp->BaseLineFile.GetBuffer());
		}
		else {
			msg = "No baseline file specified";
		}
		Message(msg);
		pRcp->AddBaseline();
	}

	// [ 10222025 MORSALIN
	for (int i = 0;; i++) {
		if (!pRcp->MeParam[0].MPa[i].Prb) {
			pRcp->MeParam[0].MPa[i].Prb = MParam::CONFOCAL_LTV_MIN;
			pRcp->MeParam[0].MPa[i + 1].Prb = MParam::CONFOCAL_LTV_MAX;
			pRcp->MeParam[0].MPa[i + 2].Prb = MParam::CONFOCAL_LTV_AVG;
			break;
		}
	}
	// ]

	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		BOOL bDoNotMoveZProbe = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsConfocalLocalMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		// Found one point for confocal measurement [6/14/2011 XPMUser]
		bFirst = FALSE; // Morsalin for skip Local measurement. 
		if (bFirst) {
			bFirst = FALSE;
			if (!InitializeConfocalMeasurement(pRcp, p, measurement)) {
				msg = "Fail Confocal Initialization";
				Message(msg);
				return FALSE;
			}
		}
		// First point initialization end [2/28/2012 Administrator]

		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		Message(msg);

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		if (!p413App->MeasureOnePointConfocal(pRcp, p, code, pRcp->nTrialEcho, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Confocal Reached Fail Count limit";
					Message(msg, 1);
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement, updated every point for live effect only [3/11/2010 Valued Customer]
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		UpdateUILocal(p);
	}

	m_cWaferMap.Redraw();

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	// All point measured [2/28/2012 Administrator]
	// Adjust bow warp height [8/21/2011 FSMT]
	FILE* fp = NULL;
	fp = fopen("C:\\TEMP\\BOW.CSV", "wb");
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				if (pMeP->MPa[i].IsConfocalWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				//[23102024
				if (pMeP->MPa[i].IsConfocalDualWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				//]
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}
	if (fp) {
		fclose(fp);
	}

	msg = "Confocal measurement end";
	Message(msg, 2);

	return TRUE;
}
// ]

BOOL CProcJob413ProcessingDlg::DoConfocalMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {

	CString msg;
	int FailCount = 0;
	CPattern* pPatt = &pRcp->Patt;
	MParam::PRBNUM measurement = MParam::NOPRB;
	CMotorS2* pMtr = p413App->pMtr;

	short CurMeSetBak = p413App->Global.CurMeSet;
	short CurPaSetBak = p413App->Global.CurPaSet;

	// Loading baseline information [11/2/2011 Administrator]
	if (pRcp->IsProfileMeasurement()) {
		if (pRcp->BaseLineFile.GetLength() > 0) {
			msg.Format("Add baseline info (%s) to recipe", pRcp->BaseLineFile.GetBuffer());
		}
		else {
			msg = "No baseline file specified";
		}
		Message(msg);
		pRcp->AddBaseline();
	}

	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();

	//[ // MAHEDI 10292025_MH -- initializatio for roughness value check
	p413App->pointIdx = 0; 
	//]
	while (pos && (!g->bAborting)) {
		BOOL bDoNotMoveZProbe = FALSE;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsConfocalMeasurement(p))) {
			continue;
		}
		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;
		
		// Found one point for confocal measurement [6/14/2011 XPMUser]
		if (bFirst) {
			bFirst = FALSE;
			if (!InitializeConfocalMeasurement(pRcp, p, measurement)) {
				msg = "Fail Confocal Initialization";
				Message(msg);
				return FALSE;
			}
		}
		// First point initialization end [2/28/2012 Administrator]

		msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr().GetBuffer());
		Message(msg);

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		// Measure one point only [5/15/2013 user]
		if (!p413App->MeasureOnePointConfocal(pRcp, p, code, pRcp->nTrialEcho, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					msg = "Aborting: Confocal Reached Fail Count limit";
					Message(msg, 1);
					Abort();
					continue;
				}
			}
			//20130903-1 SCOTT ADD ------------------------------------
			if (DosUtil.GetSendAllPoint()) {
				Message("Report to host even though there was N/A field.");
				SendPointToHost(pRcp, p, SlotNo);
			}
			//---------------------------------------------------------
		}
		else {
			// post measurement, updated every point for live effect only [3/11/2010 Valued Customer]
			Message("Report to host");
			SendPointToHost(pRcp, p, SlotNo);
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		UpdateUI(p);
		p413App->pointIdx++; // // MAHEDI 10292025_MH
	}

	p413App->Global.CurMeSet = CurMeSetBak;
	p413App->Global.CurPaSet = CurPaSetBak;

	// All point measured [2/28/2012 Administrator]
	// Adjust bow warp height [8/21/2011 FSMT]
	FILE* fp = NULL;
	fp = fopen("C:\\TEMP\\BOW.CSV", "wb");
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {
				short ix = j * MaXPA + i;
				if (pMeP->MPa[i].IsConfocalWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				//[23102024
				if (pMeP->MPa[i].IsConfocalDualWarpMeasurement()) {
					if (fp) {
						fprintf(fp, "%s\n", pMeP->MPa[i].Name.GetBuffer(0));
					}
					switch (pRcp->BFPlane) {
					case CRecipe413::bfBESTFIT:
						pPatt->MP.NormalizeData(ix, FALSE, fp);
						break;
					case CRecipe413::bfZEROMIN:
						pPatt->MP.NormalizeData(ix, TRUE, fp);
						break;
					case CRecipe413::bf3POINTS:
						pPatt->MP.NormalizeData(ix, pRcp->RefPt1, pRcp->RefPt2, pRcp->RefPt3, fp);
						break;
					}
					if (fp) {
						fprintf(fp, "\n");
					}
				}
				//]
				pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
			}
		}
	}
	if (fp) {
		fclose(fp);
	}

	msg = "Confocal measurement end";
	Message(msg, 2);

	return TRUE;
}

void Calibratoin()
{
	// [ TODO: Calibrate 

	//goto calibration position
	if (p413App->pMtr) {
		p413App->pMtr->GotoXY(-88.4, -101.25);
	}
	double topCal, botCal, topDispl, botDispl, CalThk = 501.2;

	// focus scan or go to Abs pos
	/*if (p413App)
		p413App->TMPfocus030();*/
	//p413App->pMtr->MoveCWLTopZ(pRcp->fCWLTopZ);

	if (p413App->pMtr) {
		p413App->pCWL2->SetDisplacementMode(1);
		p413App->pCWL2->SetZero(1);
	}

	if (p413App->pMtr) {
		//p413App->pMtr->MoveRXY(56, 0);
		p413App->pCWL2->SetDisplacementMode(2);
		p413App->pCWL2->SetZero(2);
	}


	//p413App->pCWL->SetCALKValue_SL(g->fConfocalConstantK / 1000.1);
	p413App->pCWL2->SetCALKValue(CalThk / 1000.0);

	//]
}

//[ Mahedi Kmala 10242025_M
BOOL CProcJob413ProcessingDlg::IsConfocalSignalOK() {
	if (pRcp->IsConfocalRoughnessMeasurement()) {
		return TRUE;
	}
	//030
	double intensity = 0;

	for (int i = 0; i < 8; i++) {
		double its;
		p413App->pCWL2->GetMeasuredValueMFO(1, 1, its);
		intensity += its;
	}intensity /= 8;

	if (intensity < 5) {
		return FALSE;
	} 

	//040
	intensity = 0;
	for (int i = 0; i < 8; i++) {
		double its;
		p413App->pCWL2->GetMeasuredValueMFO(1, 2, its);
		intensity += its;
	}intensity /= 8;
	
	if (intensity < 5) {
		return FALSE;
	}

	return TRUE;
}
//]
BOOL CProcJob413ProcessingDlg::InitializeConfocalMeasurement(CRecipe413* pRcp, CMPoint* p, MParam::PRBNUM& measurement) {

	// 08152025===============
	if (repeatCount > 1 && !pRcp->IsConfocalLocalMeasurement() && !pRcp->IsConfocalRoughnessMeasurement()) { // 08152025

		//return TRUE;
		p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::CWL);
		p413App->pCWL2->SetDisplacementMode(1);
		Sleep(10);
		p413App->pCWL2->SetZero(1);
		Sleep(10);
		p413App->pCWL2->SetDisplacementMode(2);
		Sleep(10);
		p413App->pCWL2->SetZero(2);
		Sleep(10);
		p413App->pCWL2->SetCALKValue(g->fConfocalConstantK / 1000.0);
		Sleep(10);


		if (IsConfocalSignalOK()) // 10242025_M
			return TRUE;
		else return FALSE;
	}
	//==========================

	g->isCombinedCWLMes = false; // 04092025
	g->combinedBotData = 0.0;// 04092025
	g->combinedTopData = 0.0;// 04092025

	CString msg;
	CPattern* pPatt = &pRcp->Patt;
	Message("Confocal measurement start", 2);
	if (g->bAborting) {
		return FALSE;
	}
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		if (pRcp->IsConfocalRoughnessMeasurement())
		{
			p413App->pPSwitch->MoveToCWL008prb();
			pMtr->MoveCWLTopZ(pRcp->fCWLTopZ2);
		}
		else
			pMtr->MoveCWLTopZ(pRcp->fCWLTopZ);
		if (g->bAborting) {
			return FALSE;
		}
		Message("Move to calibration position");
		if (pRcp->IsConfocalLocalMeasurement() || pRcp->IsConfocalRoughnessMeasurement()) {
			p413App->GotoXYS(0.0, 0.0, CCamBase::CWL);
			if (pRcp->IsConfocalRoughnessMeasurement())
			{
				p413App->pCWL->SetDisplacementMode(1);
				p413App->pCWL->SetZero(1);
			}
		}
		else {
			p413App->GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::CWL);
			pMtr->MoveCWLTopZ(pRcp->fCWLTopZ);//20251120
		}
		
		//p413App->GotoXYS(-88.4, -101.25, CCamBase::CWL);
		//Calibratoin();
		pMtr->WaitCWLTopZ();
	}

	if (g->bAborting) { return FALSE; }

	if (!pRcp->IsConfocalRoughnessMeasurement()) //20250319// MASRUR// BYPASS FOR ROUGHNESS 
	{
		// TODO: CONFOCAL CALIBRATION ROUTINE  // Commented Morsalin for testing
		Message("Set zero plane");
		p413App->pCWL2->SetDisplacementMode(1);
		Sleep(10);
		p413App->pCWL2->SetZero(1); //MASRUR 20250317 SETZERO
		Sleep(10);

		if (g->bDualConfocal) {
			p413App->pCWL2->SetDisplacementMode(2);
			Sleep(10);
			p413App->pCWL2->SetZero(2); //MASRUR 20250317 SETZERO
			Sleep(10);
			// set constant k which is defined like echo calibration thickness;
			p413App->pCWL2->SetCALKValue(g->fConfocalConstantK / 1000.0);
			Sleep(10);
		}
		// --

		//p413App->AutoFocusToCallibrate(p413App);//03182025
		//AfxBeginThread(C413AppCWL::FocusScanCWL030, p413App);
		//AfxBeginThread(C413AppCWL::QuickFocusScanCWL030, p413App);
		//[03192025
		if (pRcp->isCalibrationAutoFocusNeeded == TRUE) {
			p413App->runningWaferScanCWLThread = TRUE;
			
				//p413App->QuickFo cusScanCWL030_MatchConstantK(p413App);
			p413App->QuickFocusScanCWL030_(p413App);
			Message("Focus 030 done");
			p413App->pCWL2->SetZero(1);
			Sleep(100);
			p413App->pCWL2->SetZero(2);
			Sleep(100);
		}
		//

	}
	if (IsConfocalSignalOK()) // 10242025_M
		return TRUE;
	else return FALSE;
}

void CProcJob413ProcessingDlg::DoMultiPointMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	int n = 1;
	BOOL bFirst = TRUE;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p || (!pRcp->IsMultiPointMeasurement(p))) {
			n++;
			continue;
		}
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
		if (bFirst) {
			bFirst = FALSE;
			Message("Multi point measurement start", 2);
			Message("Move camera to Echo Position");
			if (!p413App->GotoPositionecho()) {
				Message("Motion error", 0);
				pGDev->SetAlarm(CAlrmList::PMOTORERROR);
				Abort();
				continue;
			}
			Message("Reposition probe");
			if (pMtr) {
				pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
			}

			if (pMtr) {
				Message("Turn on microscope light");
				pMtr->SetMicroscopeLight(TRUE);
			}
		}

		CString str;
		str.Format("Measure point %d", n++);
		Message(str);

		//////////////////////////////////////////////////////////////////////////
		pointtime = clock();
		bMeasureOn = TRUE;
		if (!p413App->MeasureMultiPoint(pRcp, p, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cProfile)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					Message("Aborting: Reached Fail Count limit", 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
		}
		bMeasureOn = FALSE;
		//////////////////////////////////////////////////////////////////////////

		// post measurement calculation [3/11/2010 Valued Customer]
		DoMathCalculation(pRcp, pMPList);
		Message("Report to host");
		SendPointToHost(pRcp, p, SlotNo);
		UpdateUI(p);
	}
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}
	Message("Multi point measurement end", 2);
}

void CProcJob413ProcessingDlg::DoBevelMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo) {
	int n = 1;
	CString msg;
	BOOL bFirst = TRUE;
	CPattern* pPatt = &pRcp->Patt;
	CMotorS2* pMtr = p413App->pMtr;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* pMP = (CMPoint*)pMPList->GetNext(pos);
		// Only allow BEVEL measurement to pass [5/30/2012 Yuen]
		if (!pMP || !pRcp->IsBevelMeasurement(pMP)) {
			continue;
		}
		n++;
		if (pMtr) pMtr->GotoXYZ1(pMP->Co.x, pMP->Co.y, pRcp->fEchoTopZ);

		if (m_cVideo.IsSimu()) {
			CString dir;
			DosUtil.GetProgramDir(dir);
			dir = dir + "BEVEL1.BMP";
			m_cVideo.LoadSimuDIB(dir);
		}
		bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
		if (bFirst) {
			bFirst = FALSE;
			Message("Bevel measurement start", 2);
			Message("Move camera to Echo Position");
			if (!p413App->GotoPositionecho()) {
				Message("Motion error", 0);
				pGDev->SetAlarm(CAlrmList::PMOTORERROR);
				Abort();
				continue;
			}
			if (pMtr) {
				Message("Turn on microscope light");
				pMtr->SetMicroscopeLight(TRUE);
				Message("Turn on TF illumination light");
				pMtr->SetSpectroIllumLight(TRUE);
			}
		}
		CString str;
		str.Format("Measure point %d", n);
		Message(str);

		//////////////////////////////////////////////////////////////////////////
		//p413App->pEcho->OpenDump(&gpDump,n);
		m_cVideo.ScanLineReady = 1; // Setting will disappear when dialog is destroyed
		pointtime = clock();
		bMeasureOn = TRUE;
		if (!p413App->MeasureBevel(pRcp, pMP, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cProfile, &m_cList)) {
			bMeasureOn = FALSE;
			FailCount++;
			if (pRcp->FailCountTimeOut > 0) {
				if (FailCount > pRcp->FailCountTimeOut) {
					Message("Aborting: Reached Fail Count limit", 1);
					switch (ID) {
					case 0:
						pGDev->SetAlarm(CAlrmList::LP1TOOMANYFAILPOINT);
						break;
					case 1:
						pGDev->SetAlarm(CAlrmList::LP2TOOMANYFAILPOINT);
						break;
					}
					Abort();
					continue;
				}
			}
		}
		bMeasureOn = FALSE;
		//p413App->pEcho->CloseDump(gpDump); gpDump = NULL;
		//////////////////////////////////////////////////////////////////////////

		// post measurement calculation [3/11/2010 Valued Customer]
		DoMathCalculation(pRcp, pMPList);
		Message("Report to host");
		SendPointToHost(pRcp, pMP, SlotNo);
		UpdateUI(pMP);
	}
	for (int j = 0; j < MAXMEPASET; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			pPatt->MP.CalcStat(j, i, pMeP->MPa[i], FALSE);
		}
	}
	Message("Bevel measurement end", 2);
}

void CProcJob413ProcessingDlg::ConvertBadDataNegToBadData(CRecipe413* pRcp, CPtrList* pMPList) {
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (!p) {
			continue;
		}
		CData* d = p->GetData(0, FALSE);
		if (!d) {
			continue;
		}
		for (int j = 0; j < MAXMEPASET; j++) {
			CMeParam* pMeP = &pRcp->MeParam[j];
			for (int i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb > 0) {
					if (d->DVal[j * MAXMEPASET + i] == BADDATANEG) {
						d->DVal[j * MAXMEPASET + i] = BADDATA;
					}
				}
			}
		}
	}
}

void CProcJob413ProcessingDlg::DoMathCalculation(CRecipe413* pRcp, CPtrList* pMPList) {
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* pMP = (CMPoint*)pMPList->GetNext(pos);
		if (!pMP) {
			continue;
		}
		CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
		CData* d = pMP->GetData(0, FALSE);
		if (!d) {
			continue;
		}
		for (int j = 0; j < MAXMEPASET; j++) {
			CMeParam* pMeP1 = &pRcp->MeParam[j];
			for (int i = 0; i < MaXPA; i++) {
				if (pMeP1->MPa[i].Prb == MParam::MATHPRB) {
					d->Set(j * MaXPA + i, DoMaths(d, j * MaXPA, &pMeP1->MPa[i], pMeP1));
				}
				else if (pMeP1->MPa[i].Prb == MParam::BESTVALUEPRB) {
					d->Set(j * MaXPA + i, DoBestValue(d, j * MaXPA, &pMeP1->MPa[i], pMeP1));
				}
			}
		}
	}
}

// return BADDATANEG if any one of the value is BADDATANEG [9/5/2012 Administrator]
float CProcJob413ProcessingDlg::DoMaths(CData* d, short off, MParam* p, CMeParam* pMeP) {
	float value = 0, fData;
	short ix, n = 0;
	CString* pstr = &p->Formula;
	for (int k = 0; k < ((pstr->GetLength() + 1) / 2); k++) {
		if (n == 0) {
			ix = GetItemIndex(pstr, n);
			if ((ix >= 0) && (ix < MaXPA)) {
				value = d->Get(off + ix);
				if (value == BADDATANEG) {
					return BADDATANEG;
				}
			}
			n++;
		}
		else {
			char ch = pstr->GetAt(n);
			n++;
			ix = GetItemIndex(pstr, n);
			if ((ix >= 0) && (ix < MaXPA)) {
				fData = d->Get(off + ix);
				if (fData == BADDATANEG) {
					return BADDATANEG;
				}
				switch (ch) {
				case '+':
					value += fData;
					break;
				case '-':
					value -= fData;
					break;
				case '*':
					value *= fData;
					break;
				case '/':
					if (fData) {
						value /= fData;
					}
					else {
						return BADDATA;
					}
					break;
				}
			}
			n++;
		}
	}
	return value;
}

short CProcJob413ProcessingDlg::GetItemIndex(CString* pstr, short n) {
	switch (pstr->GetAt(n)) {
	case '1':
		return 0;
		break;
	case '2':
		return 1;
		break;
	case '3':
		return 2;
		break;
	case '4':
		return 3;
		break;
	case '5':
		return 4;
		break;
	case '6':
		return 5;
		break;
	case '7':
		return 6;
		break;
	case '8':
		return 7;
		break;
	case '9':
		return 8;
		break;
	case 'A':
	case 'a':
		return 9;
		break;
	case 'B':
	case 'b':
		return 10;
		break;
	case 'C':
	case 'c':
		return 11;
		break;
	case 'D':
	case 'd':
		return 12;
		break;
	case 'E':
	case 'e':
		return 13;
		break;
	case 'F':
	case 'f':
		return 14;
		break;
	case 'G':
	case 'g':
		return 15;
		break;
	}
	return 0;
}

void CProcJob413ProcessingDlg::OnRecover() {
	SLPInit LpInit;
	LpInit.unit = ID + 1;
	LpInit.action = CIncmplPrcsHdlr::STARTRECOVERY;
	LpInit.bNoClose[ID] = pUDev->PerformAction(LpInit.action, LpInit.unit, LpInit.CarrierID);
}

void CProcJob413ProcessingDlg::OnToErrorRecovery() {
	m_cRecover.ShowWindow(SW_SHOW);
	m_cClose.ShowWindow(SW_HIDE);
	m_cAbort.ShowWindow(SW_HIDE);
}

BOOL CProcJob413ProcessingDlg::DoThinFilmPositionMarkers(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo) {
	if (!g || !pRcp) {
		Message("DoThinFilmPositionMarkers fault", 2);
		return FALSE;
	}
	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		short Marker = pRcp->IsThinFilmMarker(p);
		if (!p || (!Marker)) {
			continue;
		}
		if (bFirst) {
			bFirst = FALSE;
			m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
			Message("Get Thin Film marker");
			Message("Move to thin film position");
			if (!p413App->GotoPositionthinfilm()) {
				Message("Motion error", 0);
				pGDev->SetAlarm(CAlrmList::PMOTORERROR);
				Abort();
				continue;
			}
			//			SwitchCameraSetExposure(CCamBase::THINFCAM,pRcp,0);
			p413App->SwitchCameraSetExposure(CCamBase::THINFCAM, pRcp->TFNormalExposure + g->BaseExpoTF, &m_cVideo, &m_cList, pRcp);
		}
		short nTrial = pRcp->nTrialEcho;
		if (nTrial < 9) {
			nTrial = 9;
		}
		if (!p413App->GetThinFilmMarker(Marker, pRcp, p, nTrial, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, ID)) {
			return FALSE;
		}
	}
	return TRUE;
}


//20220607/yukchiu / new version use echo probe for marker PR
BOOL CProcJob413ProcessingDlg::DoThinFilmPositionMarkers_v2(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo) {
	CString msg;

	if (!g || !pRcp) {
		Message("DoThinFilmPositionMarkers fault", 2);
		return FALSE;
	}
	BOOL bFirst = TRUE;

	//20220607/yukchiu / new for v2 version
	short MeSetBakup = p413App->Global.CurMeSet;
	short PaSetBakup = p413App->Global.CurPaSet;
	CMotorS2* pMtr = p413App->pMtr;

	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		short Marker = pRcp->IsEchoMarker(p);
		/*short Marker = pRcp->IsThinFilmMarker(p);*/
		if (!p || (!Marker)) {
			continue;
		}

		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
		Message("Get Echo marker %d - Thin Film measurement", Marker);

		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->EchoNormalExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
		
		short nTrial = pRcp->nTrialEcho;
		if (nTrial < 9) {
			nTrial = 9;
		}

		//20220607/yukchiu
		// gotoxy gotoz

		float X, Y;
		CString str;

		BOOL bMatched = FALSE;
		X = p->Co.x;
		Y = p->Co.y;
		if (pRcp->fEchoTopZ != p413App->fCurEchoTopZPos) {
			MsgStr = "Reposition probe";
			Message(&m_cList, MsgStr);
			//20220607/yukchiu
			if (Marker == 1) {
				str = "Get Echo Marker 1";
			}
			else if (Marker == 2) {
				str = "Get Echo Marker 2";
			}
			//===============
			Message(&m_cList, str);
			pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
		}

		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
		pMtr->WaitEchoTopZ();

		// end gotoxy gotoz

		//20220607/yukchiu
		// TODO: get current position and store it to a variable
		if (Marker == 1) {
			fX1Old = g->LocXY.X, fY1Old = g->LocXY.Y;
		}
		else if (Marker == 2) {
			fX2Old = g->LocXY.X, fY2Old = g->LocXY.Y;
		}
		//===============

		if (!p413App->GetEchoMarker(Marker, pRcp, p, nTrial, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, ID)) {
			return FALSE;	// OK
		}

		//20220607/yukchiu
		// auto adjust
		m_cVideo.Level = pRcp->AutoExposeEcho;
		m_cVideo.AutoAdjust();

		MsgStr.Format("Trial:1 (%s)", p->Co.GetXYStr());
		Message(&m_cList, MsgStr);
		// end auto adjust

		//20220607/yukchiu
		// get after PR position and store it to a variable
		if (Marker == 1) {
			fX1New = g->LocXY.X, fY1New = g->LocXY.Y;
		}
		else if (Marker == 2) {
			fX2New = g->LocXY.X, fY2New = g->LocXY.Y;
		}
		//===============
	}

	p413App->Global.CurMeSet = MeSetBakup;
	p413App->Global.CurPaSet = PaSetBakup;


	//20220607/yukchiu
	// recalculate function plus update points
	RecalculateLocations(fX1Old, fY1Old, fX2Old, fY2Old, fX1New, fY1New, fX2New, fY2New);

	Message("Recalculate locations");

	pos = pMPList->GetHeadPosition();
	int i = 0;
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		/*BOOL bEchoMeasure = TRUE;*/
		short Marker = pRcp->IsEchoMarker(p);
		MParam* pM = &pRcp->MeParam[p->MeSet - 1].MPa[i];
		while (pM->Prb != pM->NOPRB)
		{
			if (p && pM->IsThicknessMeasurement()) {						//20220613/yukchiu / check if is thinkfilm measurement
				p->Co.x = PrC * p->Co.x - PrS * p->Co.y + PrSx;
				p->Co.y = PrC * p->Co.y + PrS * p->Co.x + PrSy;
				break;
			}
			i++;
			pM = &pRcp->MeParam[p->MeSet - 1].MPa[i];
		}
		i = 0;
	}

	Message("Recalculate locations done");
	//================

	return TRUE;
}


BOOL CProcJob413ProcessingDlg::DoEchoProbePositionMarkers(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo) {
	CString msg;

	if (!g || !pRcp) {
		msg = "DoEchoProbePositionMarker error";
		Message(msg, 2);
		return FALSE;
	}
	BOOL bFirst = TRUE;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		short Marker = pRcp->IsEchoMarker(p);
		if (!p || (Marker < 1)) {
			continue;
		}
		if (bFirst) {
			bFirst = FALSE;
			m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
			msg.Format("Get Echo marker %d", Marker);
			Message(msg);
		}
		else {
			msg.Format("Get Echo marker %d", Marker);
			Message(msg);
		}
	}
}

//20220601/yukchiu / change from void to BOOL
BOOL CProcJob413ProcessingDlg::DoEchoProbePositionMarkers_v2(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo) {
	CString msg;

	if (!g || !pRcp) {
		msg = "DoEchoProbePositionMarker error";
		Message(msg, 2);
		return FALSE;
	}
	BOOL bFirst = TRUE;

	short MeSetBakup = p413App->Global.CurMeSet;
	short PaSetBakup = p413App->Global.CurPaSet;
	CMotorS2* pMtr = p413App->pMtr;

	POSITION pos = pMPList->GetHeadPosition();
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		short Marker = pRcp->IsEchoMarker(p);
		if (!p || (Marker < 1)) {								
			continue;
		}

		p413App->Global.CurMeSet = p->MeSet;
		p413App->Global.CurPaSet = p->PatSet;

		m_cVideo.FileDirc.Format("C:\\TEMP\\%s", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer()); // Select a base name [7/28/2011 XPMUser]
		msg.Format("Get Echo marker %d", Marker);
		Message(msg);

		p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, pRcp->EchoNormalExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);

		short nTrial = pRcp->nTrialEcho;
		if (nTrial < 9) {
			nTrial = 9;
		}

		//20220603/yukchiu
		// TODO: gotoxy gotoz

		float X, Y;
		CString str;	
		
		BOOL bMatched = FALSE;
		X = p->Co.x;
		Y = p->Co.y;
		if (pRcp->fEchoTopZ != p413App->fCurEchoTopZPos) {
			MsgStr = "Reposition probe";
			Message(&m_cList, MsgStr);
			//20220602/yukchiu
			if (Marker == 1) {
				str = "Get Echo Marker 1";
			}
			else if (Marker == 2) {
				str = "Get Echo Marker 2";
			}
			//===============
			Message(&m_cList, str);
			pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
		}

		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
		pMtr->WaitEchoTopZ();

		// end gotoxy gotoz


		//20220603/yukchiu
		// TODO: get current position and store it to a variable
		if (Marker == 1) {
			fX1Old = g->LocXY.X, fY1Old = g->LocXY.Y;
		}
		else if (Marker == 2) {
			fX2Old = g->LocXY.X, fY2Old = g->LocXY.Y;
		}
		//===============


		if (!p413App->GetEchoMarker(Marker, pRcp, p, nTrial, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, ID)) {
			return FALSE;	// OK
		}

		//20220603/yukchiu
		// TODO: auto adjust
		m_cVideo.Level = pRcp->AutoExposeEcho;
		m_cVideo.AutoAdjust();

		MsgStr.Format("Trial:1 (%s)", p->Co.GetXYStr());
		Message(&m_cList, MsgStr);
		// end auto adjust

		//20220603/yukchiu
		// TODO: get after PR position and store it to a variable
		if (Marker == 1) {
			fX1New = g->LocXY.X, fY1New = g->LocXY.Y;
		}
		else if (Marker == 2) {
			fX2New = g->LocXY.X, fY2New = g->LocXY.Y;
		}
		//===============

	}
	p413App->Global.CurMeSet = MeSetBakup;
	p413App->Global.CurPaSet = PaSetBakup;


	//20220603/yukchiu
	// TODO: recalculate function plus update points
	RecalculateLocations(fX1Old, fY1Old, fX2Old, fY2Old, fX1New, fY1New, fX2New, fY2New);

	Message("Recalculate locations");

	pos = pMPList->GetHeadPosition();
	int i = 0;
	while (pos && (!g->bAborting)) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		/*BOOL bEchoMeasure = TRUE;*/
		short Marker = pRcp->IsEchoMarker(p);
		MParam* pM = &pRcp->MeParam[p->MeSet - 1].MPa[i];
		while (pM->Prb != pM->NOPRB)
		{
			if (p && pM->IsEchoMeasurement()) {
				p->Co.x = PrC * p->Co.x - PrS * p->Co.y + PrSx;
				p->Co.y = PrC * p->Co.y + PrS * p->Co.x + PrSy;
				break;
			}
			else if (p && pM->IsThinFilmMeasurement()) {					//20220613 / yukchiu / update thinfilm position
				p->Co.x = PrC * p->Co.x - PrS * p->Co.y + PrSx;
				p->Co.y = PrC * p->Co.y + PrS * p->Co.x + PrSy;
				break;
			}
			i++;
			pM = &pRcp->MeParam[p->MeSet - 1].MPa[i];
		}
		i = 0;
	}

	Message("Recalculate locations done");
	//================

	return TRUE;
}

float CProcJob413ProcessingDlg::DoBestValue(CData* d, short off, MParam* p, CMeParam* pMeP) {
	float value = BADDATANEG/*, fData*/;
	short ix, n = 0;
	CString* pstr = &p->Formula;
	for (int k = 0; k < pstr->GetLength(); k++) {
		ix = GetItemIndex(pstr, n);
		if ((ix >= 0) && (ix < MaXPA)) {
			value = d->Get(off + ix);
			if (value != BADDATANEG) {
				return value;
			}
		}
	}
	return BADDATANEG;
}

// void CProcJob413ProcessingDlg::UpdateTargets()
// {
// 	if (g && pRcp)
// 	{
// 		m_cVideo.fTgtRedX = pRcp->fTGtRedX;
// 		m_cVideo.fTgtRedY = pRcp->fTGtRedY;
// 		m_cVideo.fTgtGrnX = pRcp->fTGtGrnX[g->CurPaSet-1];
// 		m_cVideo.fTgtGrnY = pRcp->fTGtGrnY[g->CurPaSet-1];
// 		m_cVideo.fTgtBluX = pRcp->fTGtBluX[g->CurPaSet-1];
// 		m_cVideo.fTgtBluY = pRcp->fTGtBluY[g->CurPaSet-1];
// 	}
// 	else
// 	{
// 		Message("UpdateTargets fault",2);
// 	}
// }

void CProcJob413ProcessingDlg::SwitchOnChuckVacuum(CRecipe413* pRcp) {
	CString msg, str;
	CMotorS2* pMtr = p413App->pMtr;
	if (pRcp->IsEchoBowWarpMeasurement()) {
		str = "off (Bow Warp)";
		pMtr->SetChuckVacuum(FALSE);
	}
	else {
		str = "on";
		p413App->pMtr->SetChuckVacuum(TRUE, pRcp->Wp.size);
	}
	msg.Format("Turn chuck vacuum %s", str.GetBuffer());
	Message(msg);
}

BOOL CProcJob413ProcessingDlg::CenteringWafer(float& Cx, float& Cy, float& Cr) {
	CString msg;

	if (!pRcp->bUseWaferAlign) {
		Cx = Cy = Cr = 0;	// No offset [6/10/2014 Yuen]
		msg.Format("SKIPPED: Wafer alignment using wafer edge");
		Message(msg, 5);
		return TRUE;
	}

	msg.Format("Wafer alignment using wafer edge");
	Message(msg, 5);

	p413App->GotoPositionecho(); // Move P motor to echo position.[7/13/2010 C2C]
	p413App->SwitchCameraSetExposure(CCamBase::ECHOCAM, p413App->Global.AlgnExposure + g->BaseExpoEcho, &m_cVideo, &m_cList, pRcp);
	bPause = TRUE; OnPausevideo(); // Cannot pause when measuring trim [5/17/2011 C2C]
	m_cImgWnd.ClearTextInfo();
	BOOL bFE = FALSE;
	SFINDCENTER SFC;
	SFC.pCXOffset = &Cx;
	SFC.pCYOffset = &Cy;
	SFC.pCRadius = &Cr;
	SFC.pRcp = pRcp;
	SFC.pVideo = &m_cVideo;
	SFC.pImgWnd = &m_cImgWnd;
	SFC.pList = &m_cList;
	SFC.Method = pRcp->FindEdgeType;
	SFC.bTest = pRcp->bTestFindEdgeResult;
	switch (pRcp->FindEdgeMethod) {
	case 2:
		Message("Method 2", 5);
		bFE = p413App->FindWaferCenterUsingEdgeDetectionFast(SFC);
		break;
	default:
		Message("Default method", 5);
		bFE = p413App->FindWaferCenterUsingEdgeDetection(SFC);
		break;
	}
	return bFE;
}
//20220603/yukchiu
void CProcJob413ProcessingDlg::RecalculateLocations(float fRefX1Old, float fRefY1Old, float fRefX2Old, float fRefY2Old,
	float fRefX1New, float fRefY1New, float fRefX2New, float fRefY2New) {
	//	float x,y;

	double eps = 1e-4;

	double fDist = ((fRefX2Old - fRefX1Old) * (fRefX2Old - fRefX1Old)
		+ (fRefY2Old - fRefY1Old) * (fRefY2Old - fRefY1Old));

	if (fDist <= eps)
	{
		PrC = 1;
		PrS = 0;
		PrSx = 0;
		PrSy = 0;
	}
	else
	{
		PrS = ((fRefY2New - fRefY1New) * (fRefX2Old - fRefX1Old) - (fRefY2Old - fRefY1Old) * (fRefX2New - fRefX1New)) / fDist;
		PrC = ((fRefX2New - fRefX1New) * (fRefX2Old - fRefX1Old) + (fRefY2New - fRefY1New) * (fRefY2Old - fRefY1Old)) / fDist;
		PrSx = fRefX1New - PrC * fRefX1Old + PrS * fRefY1Old;
		PrSy = fRefY1New - PrS * fRefX1Old - PrC * fRefY1Old;
	}
}


//20220603/yukchiu
void CProcJob413ProcessingDlg::Message(CListCtrl* pList, CString& str, short lvl) {
	Log(str, lvl);
	if (!pList) {
		return;
	}

	short idx = pList->InsertItem(0, CTime::GetCurrentTime().Format("%y:%m:%d %H:%M:%S"));
	pList->SetItemText(idx, 1, str);
	short len = pList->GetItemCount();
	if (len > 1024) {
		for (int i = 0; i < 1024 / 5; i++) {
			pList->DeleteItem(len - i);
		}
	}
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

CString CProcJob413ProcessingDlg::Get2DFolderFromC2CPath(const CString& c2cPath)
{
	if (c2cPath.IsEmpty()) return _T("");

	// Start from real C2C path: ...\C2C\<Recipe>\<File>.C2C
	CString path = c2cPath;

	// Remove filename
	int pos = path.ReverseFind(_T('\\'));
	if (pos == -1) return _T("");
	path = path.Left(pos); // ...\C2C\<Recipe>

	// Remove recipe folder
	pos = path.ReverseFind(_T('\\'));
	if (pos == -1) return _T("");
	path = path.Left(pos); // ...\C2C

	// Replace C2C with TXT\2D
	pos = path.ReverseFind(_T('\\'));
	if (pos == -1) return _T("");
	CString baseDir = path.Left(pos); // ...\DATA\FSM\<Recipe>\<Year>\<Month>
	CString folder2D = baseDir + _T("\\TXT\\2D\\");

	::CreateDirectory(baseDir + _T("\\TXT\\"), NULL);
	::CreateDirectory(folder2D, NULL);

	return folder2D;
}