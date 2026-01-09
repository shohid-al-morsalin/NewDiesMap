#include "stdafx.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "FEM/IDR/IDRMgr.h"
#include "FEM/Ldp/LPMgr.h"
#include "FEM/Ldp/LPSvr.h"
#include "FEM/Rbt/RbtMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IPC/SMCHdr.h"
#include "LTW/LTMgrE95.h"
#include "DrLockMgr.h"

#include "InfoPanelWelcome.h"

#define IDT_STARTSYSTEM		9999
#define IDT_SYSTEMSTARTED	9998
#define IDT_COMPLETED		9997
#define IDT_FAILED			9996

// CInfoPanelWelcome

IMPLEMENT_DYNCREATE(CInfoPanelWelcome, CResizableFormView)

CInfoPanelWelcome::CInfoPanelWelcome()
	: CResizableFormView(CInfoPanelWelcome::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelWelcome)
	//}}AFX_DATA_INIT

	pThread = NULL;
	bStartFault = FALSE;
}

CInfoPanelWelcome::~CInfoPanelWelcome() {}

void CInfoPanelWelcome::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelWelcome)
	DDX_Control(pDX, IDC_MESSAGEBOX, m_cMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelWelcome, CResizableFormView)

	ON_WM_TIMER()
	ON_COMMAND(ID_LP_STARTFAIL1, OnLpStartfail1)
	ON_COMMAND(ID_LP_STARTFAIL2, OnLpStartfail2)
	ON_COMMAND(ID_CID_STARTFAILED1, OnCidStartfailed1)
	ON_COMMAND(ID_CID_STARTFAILED2, OnCidStartfailed2)
	ON_COMMAND(ID_RBT_STARTFAIL, OnRbtStartfail)
	ON_COMMAND(ID_RBT_STARTOK, OnRbtStartok)
	ON_COMMAND(ID_CID_STARTED1, OnCidStarted1)
	ON_COMMAND(ID_CID_STARTED2, OnCidStarted2)
	ON_COMMAND(ID_LP_STARTOK1, OnLpStartok1)
	ON_COMMAND(ID_LP_STARTOK2, OnLpStartok2)

END_MESSAGE_MAP()

// CInfoPanelWelcome diagnostics
#ifdef _DEBUG
void CInfoPanelWelcome::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelWelcome::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelWelcome message handlers
void CInfoPanelWelcome::OnInitialUpdate() {
	CResizableFormView::OnInitialUpdate();

	// [01172024 ZHIMING
	CString machine_name = _T("");
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
		machine_name = "FSM 413 C2C";
		break;
	case CGlobalDeviceContainer::FSM413SA:
		machine_name = "FSM 413 SA";
		break;
	case CGlobalDeviceContainer::FSM413MOT:
		machine_name = "FSM 413 MOT";
		break;
	}

	GetDlgItem(IDC_MESSAGE)->SetWindowText(machine_name);
	// ]

	AddResizedControl(IDC_FSM, TOP_LEFT, TOP_RIGHT);
	AddResizedControl(IDC_MESSAGE, TOP_LEFT, TOP_RIGHT);
	AddResizedControl(IDC_MESSAGEBOX, TOP_LEFT, BOTTOM_LEFT);

	AnchorControls();

	SetTimer(IDT_STARTSYSTEM, 500, NULL);
}

UINT CInfoPanelWelcome::StartSystem(LPVOID pObj) {
	if (!pObj) {
		ASSERT(0);
		return -1;
	}

	CInfoPanelWelcome* p = (CInfoPanelWelcome*)pObj;

	int Cnt = p->m_cMessage.GetCount();
	p->m_cMessage.InsertString(Cnt++, pGDev->GetModelStr());
	p->m_cMessage.InsertString(Cnt++, "Initializing ...");
	p->StartHardwareSystem(&p->m_cMessage);
	p->StartMgrs(p->m_hWnd, &p->m_cMessage);
	pUDev->StartUIElements();

	p->SetTimer(IDT_SYSTEMSTARTED, 1000, NULL);
	return 1;
}

void CInfoPanelWelcome::StopSystem() {
	pUDev->StopUIElements();
	StopMgrs();
	p413App->StopSystem();
}

void CInfoPanelWelcome::PostNcDestroy() {
	StopSystem();
	CResizableFormView::PostNcDestroy();
}

void CInfoPanelWelcome::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_STARTSYSTEM) {
		KillTimer(IDT_STARTSYSTEM);
		bStartFault = FALSE;
		pThread = AfxBeginThread(CInfoPanelWelcome::StartSystem, this);
		//StartSystem((LPVOID)this);
		StartBegin = GetTickCount64();
	}
	if (nIDEvent == IDT_COMPLETED) {
		KillTimer(IDT_COMPLETED);
		p413App->bUIEnabled = TRUE;
		CString str;
		//[03062025
		p413App->ResetAtBegin();
		str.Format("Reset Done");
		m_cMessage.InsertString(m_cMessage.GetCount(), str);
		//]
		str.Format("Completed: %d seconds", (GetTickCount64() - StartBegin) / 1000);
		m_cMessage.InsertString(m_cMessage.GetCount(), str);
		
		
	}
	if (nIDEvent == IDT_FAILED) {
		KillTimer(IDT_FAILED);
		CString str;
		if (Var.UIMode > 1) {
			p413App->bUIEnabled = TRUE;
			str = "UI Enabled due to engineering access level";
			m_cMessage.InsertString(m_cMessage.GetCount(), str);
		}
		str.Format("Some component(s) FAILED: %d seconds", (GetTickCount64() - StartBegin) / 1000);
		m_cMessage.InsertString(m_cMessage.GetCount(), str);
	}
	if (nIDEvent == IDT_SYSTEMSTARTED) {
		KillTimer(IDT_SYSTEMSTARTED);

		
		if (!p413App->StartMotionSystem(&m_cMessage)) // Appear not to start if executed in separate thread [7/14/2012 Yuen]
		{
			bStartFault = TRUE;
		}
		// [ 02082024/MORSALIN
		if (!p413App->StartProbeSwitchingSystem(&m_cMessage)) {

		}
		// ]
		p413App->StartDialogBoxes();

		if (AllStarted()) {
			SetTimer(IDT_COMPLETED, 1000, NULL);
		}
		else {
			SetTimer(IDT_FAILED, 100, NULL);
		}
		// 		if ((GetTickCount64() - StartBegin) > 120000)
		// 		{
		// 			p413App->bUIEnabled = TRUE;
		// 			m_cMessage.InsertString(m_cMessage.GetCount(),"Some components FAILED to start.");
		// 		}
		// 		else
		// 		{
		// 			SetTimer(IDT_SYSTEMSTARTED,1000,NULL);
		// 		}
	}
}

void CInfoPanelWelcome::OnLpStartfail1() {
	pGDev->pGEMObjs->SetLPTransferStatus(1, CLPObj::OUTOFSERVICE);
	CString str;
	str = "Load port #1 start FAILED";
	Log(str, 1);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnLpStartfail2() {
	pGDev->pGEMObjs->SetLPTransferStatus(2, CLPObj::OUTOFSERVICE);
	CString str;
	str = "Load port #2 start FAILED";
	Log(str, 1);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnCidStartfailed1() {
	pGDev->pGEMObjs->SetIDRService(1, CIDRObj::OUTOFSERVICE);
	CString str;
	str = "CIDR #1 start FAILED";
	Log(str, 1);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnCidStartfailed2() {
	pGDev->pGEMObjs->SetIDRService(2, CIDRObj::OUTOFSERVICE);
	CString str;
	str = "CIDR #2 start FAILED";
	Log(str, 1);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnRbtStartfail() {
	pGDev->pGEMObjs->StMd.SetRbtServ(CRbtObj::OUTOFSERVICE);
	CString str;
	str = "Robot unit start FAILED";
	pGDev->SetAlarm(CAlrmList::ROBOTSTARTFAILURE, str);
	Log(str, 1);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnRbtStartok() {
	pGDev->pGEMObjs->StMd.SetRbtServ(CRbtObj::INSERVICE);
	CString str;
	str = "Robot unit started OK";
	Log(str);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnCidStarted1() {
	pGDev->pGEMObjs->SetIDRService(1, CIDRObj::INSERVICE);

	CString str;
	str = "CIDR #1 started OK";
	Log(str);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnCidStarted2() {
	pGDev->pGEMObjs->SetIDRService(2, CIDRObj::INSERVICE);

	CString str;
	str = "CIDR #2 started OK";
	Log(str);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnLpStartok1() {
	CString str;
	str = "Load port #1 started OK";
	Log(str);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::OnLpStartok2() {
	CString str;
	str = "Load port #2 started OK";
	Log(str);
	m_cMessage.InsertString(m_cMessage.GetCount(), str);
}

void CInfoPanelWelcome::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CInfoPaneWelcome");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CInfoPanelWelcome::StartHardwareSystem(CListBox* pMessage) {
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
	case CGlobalDeviceContainer::FSM413MOT:
	case CGlobalDeviceContainer::FSM413SA:
		if (!p413App->StartSystem(pMessage)) {
			bStartFault = TRUE;
		}
		break;
	default:
		pMessage->InsertString(pMessage->GetCount(), "Incorrect model specified");
		break;
	}
}
CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartRbtMgr() {
	if (pGDev->pRbtMgr) {
		Log("Robot Manager already started", 2);
		ASSERT(0);
		return CMgrStatus::ALREADYINSTANTIATED;
	}
	else {
		CString str;
		pGDev->pRbtMgr = new CRbtMgr;
		if (!pGDev->pRbtMgr) {
			str = "Robot Manager start FAILED";
			Log(str, 2);
			m_cMessage.InsertString(m_cMessage.GetCount(), str);
			bStartFault = TRUE;
			return CMgrStatus::OBJECTCREATEFAIL;
		}

		if (!CSMClt::StartSvrThread(pGDev->pRbtMgr)) {
			str = "Robot Manager start FAILED";
			Log(str, 2);
			m_cMessage.InsertString(m_cMessage.GetCount(), str);
			bStartFault = TRUE;
			return CMgrStatus::STARTTHREADFAIL;
		}
		while (!pGDev->pRbtMgr->bStarted) {
			Sleep(10);
		}
		// 		str = "Robot Manager started OK";
		// 		Log(str, 5);
		// 		m_cMessage.InsertString(m_cMessage.GetCount(),str);
		return StartRbtUnit();
	}
	// Leaked
	ASSERT(0);
	return CMgrStatus::FUNCTIONLEAK;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StopRbtMgr() {
	if (pGDev->pRbtMgr) {
		if (CSMClt::StopSvrThread(pGDev->pRbtMgr->SvrName, pGDev->pRbtMgr->type)) {
			pGDev->pRbtMgr = NULL;
			Log("Robot Manager stopped", 10);
			return CMgrStatus::STOPOK;
		}
		Log("Robot Manager fail to stop", 10);
		return CMgrStatus::STOPTHREADFAIL;
	}
	return CMgrStatus::NOTINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartLpMgr(HWND hWnd) {
	CString str;
	if (pGDev->pLPMgr) {
		Log("Load Port Manager already started");
		ASSERT(0);	// Should not call this function again [3/16/2012 Administrator]
		return CMgrStatus::ALREADYINSTANTIATED;
	}
	str = "Load Port Manager start FAILED";

	pGDev->pLPMgr = new CLPMgr;
	if (!pGDev->pLPMgr) {
		Log(str, 1);
		m_cMessage.InsertString(m_cMessage.GetCount(), str);
		return CMgrStatus::OBJECTCREATEFAIL;
	}
	if (!CSMClt::StartSvrThread(pGDev->pLPMgr)) {
		Log(str, 1);
		m_cMessage.InsertString(m_cMessage.GetCount(), str);
		return CMgrStatus::STARTTHREADFAIL;
	}
	ULONGLONG tick = GetTickCount64();
	while (!pGDev->pLPMgr->bStarted) {
		if (((GetTickCount64() - tick) / 1000) > 60) {
			// Too long, time out [3/16/2012 Administrator]
			Log(str, 1);
			m_cMessage.InsertString(m_cMessage.GetCount(), str);
			return CMgrStatus::STARTMGRFAIL;
		}
		Sleep(10);
	}
	// 	str = "Load Port Manager started OK";
	// 	Log(str);
	// 	m_cMessage.InsertString(m_cMessage.GetCount(),str);

	return StartLpUnits(hWnd);
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StopLpMgr() {
	if (pGDev->pLPMgr) {
		if (CSMClt::StopSvrThread(pGDev->pLPMgr->SvrName, pGDev->pLPMgr->type)) {
			pGDev->pLPMgr = NULL;
			Log("Load Port Manager stopped");
			return CMgrStatus::STOPOK;
		}
		Log("Load Port Manager fail to stop");
		return CMgrStatus::STOPTHREADFAIL;
	}
	Log("Load Port Manager not available");
	return CMgrStatus::NOTINSTANTIATED;
}

// Only return success if all LP Units started OK [3/16/2012 Administrator]
CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartLpUnits(HWND hWnd) {
	int i;
	CString Msg;
	short nLoadPort = DosUtil.GetNumLP();
	for (i = 0; i < nLoadPort; i++) {
		if ((!pGDev) || (!pGDev->pLPMgr) || pGDev->pLPMgr->pLPSvr[i]) {
			ASSERT(0); // Already started or initialization problem ? [3/16/2012 Administrator]
			continue;
		}
		CSMOL SM;
		CSMClt ipc;
		SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
		SM.HWin = hWnd;
		switch (i) {
		case 0:
			SM.Ack = ID_LP_STARTOK1;
			SM.Nak = ID_LP_STARTFAIL1;
			break;
		case 1:
			SM.Ack = ID_LP_STARTOK2;
			SM.Nak = ID_LP_STARTFAIL2;
			break;
		default:
			break;
		}
		SM.Pm.act = PrcsMgrServices::STARTUNIT;
		SM.Pm.unit = i + 1;
		memset(SM.Pm.text, 0, TEXTLEN);
		SM.Pm.port = DosUtil.GetLPPort(i + 1);
		if (DosUtil.GetLPUnitSimu(i + 1)) {
			SM.Pm.port = 0;
		}
		if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
			if (!ipc.Send(&SM)) {
				Msg.Format("Fail to send Start LP%s", i + 1);
				Log(Msg, 0);
				bStartFault = TRUE;
				return CMgrStatus::SENDMSGFAIL;
			}
		}
		else {
			bStartFault = TRUE;
			return CMgrStatus::STARTUNITFAIL;
		}
	}

	// Will not reach here if any start operation fail [3/16/2012 Administrator]
	ULONGLONG tick2 = GetTickCount64();
	BOOL bLPStarted[4];
	for (i = 0; i < 4; i++) {
		bLPStarted[i] = FALSE;
	}
	while (1) {
		int j;
		for (j = 0; j < nLoadPort; j++) {
			if (pGDev->pLPMgr->pLPSvr[j]) {
				ULONGLONG tick = GetTickCount64();
				while (!pGDev->pLPMgr->pLPSvr[j]->bStarted) {
					Sleep(10);
					if (((GetTickCount64() - tick) / 1000) > 180) {
						// tout of 3 minutes [3/16/2012 Administrator]
						// Load port initialization timeout [3/16/2012 Administrator]
						bStartFault = TRUE;
						return CMgrStatus::STARTUNITFAIL;
					}
				}
				bLPStarted[j] = TRUE;
			}
		}
		BOOL bAllStarted = TRUE;
		for (j = 0; j < nLoadPort; j++) {
			if (!bLPStarted[j]) {
				bAllStarted = FALSE;
			}
		}

		if (bAllStarted) {
			return CMgrStatus::STARTOK;
		}
		if (((GetTickCount64() - tick2) / 1000) > 300) {
			// tout of 5 minutes [3/16/2012 Administrator]
			// Load port initialization timeout [3/16/2012 Administrator]
			bStartFault = TRUE;
			return CMgrStatus::STARTUNITFAIL;
		}
		Sleep(1000);  // Wait a while for all thread to start [3/16/2012 Administrator]
	}

	// should not come here [3/16/2012 Administrator]
	return CMgrStatus::STARTUNITFAIL;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartRbtUnit() {
	CString Msg;
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = pGDev->MgrStatus.hRpyWnd;
	SM.Ack = ID_RBT_STARTOK;
	SM.Nak = ID_RBT_STARTFAIL;
	SM.Pm.act = PrcsMgrServices::STARTUNIT;
	SM.Pm.port = DosUtil.GetRobotPort();
	if (DosUtil.GetRbtUnitSimu()) {
		SM.Pm.port = 0;
	}
	CSMClt ipc;
	if (ipc.Start1(pGDev->pRbtMgr->SvrName, sizeof(CSMOL))) {
		if (!ipc.Send(&SM)) {
			Log("Fail to send Start RbtUnit", 0);
			ASSERT(0);
			bStartFault = TRUE;
			return CMgrStatus::STARTUNITFAIL;
		}
		Log("Robot unit start send");

		ULONGLONG tick = GetTickCount64();
		while ((GetTickCount64() - tick) < 60000) {
			if ((pGDev->pRbtMgr->RbtSvrState != CRbtMgr::NOTSTARTED)) {
				// Its done [7/21/2013 Yuen]
				break;
			}
		}
		return CMgrStatus::STARTOK;
	}
	ASSERT(0);
	return CMgrStatus::STARTMGRFAIL;
}

void CInfoPanelWelcome::StartMgrs(HWND hWnd, CListBox* pMessage) {
	pGDev->MgrStatus.hRpyWnd = hWnd; // All reply message should be routed here, progressively replaced with more direct method [3/16/2012 Administrator]

	pGDev->AramsE95.Init("Aram413.bin");

	pGDev->MgrStatus.LTWMgr = StartLTWMgr();
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		pGDev->MgrStatus.RbtMgr = StartRbtMgr();
		if (pGDev->MgrStatus.RbtMgr != CMgrStatus::STARTOK) {
			Log("RbtMgr fail to start", 0);
		}

		pGDev->MgrStatus.LpMgr = StartLpMgr(hWnd);
		if (pGDev->MgrStatus.LpMgr != CMgrStatus::STARTOK) {
			Log("LpMgr fail to start", 0);
		}

		pGDev->MgrStatus.IDRMgr = StartIDRMgr();
		if (pGDev->MgrStatus.IDRMgr != CMgrStatus::STARTOK) {
			Log("IDRMgr fail to start", 0);
		}

		pGDev->MgrStatus.DrLockMgr = StartDoorLockMgr();
		if (pGDev->MgrStatus.DrLockMgr != CMgrStatus::STARTOK) {
			Log("DrLockMgr fail to start", 0);
		}
	}
}

void CInfoPanelWelcome::StopMgrs() {
	pGDev->MgrStatus.RbtMgr = StopRbtMgr();
	pGDev->MgrStatus.IDRMgr = StopIDRMgr();
	pGDev->MgrStatus.LpMgr = StopLpMgr();
	pGDev->MgrStatus.LTWMgr = StopLTWMgr();
	pGDev->MgrStatus.DrLockMgr = StopDoorLockMgr();

	pGDev->AramsE95.Deinit("Aram413.bin");
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartLTWMgr() {
	if (!pGDev->pLTMgr) {
		pGDev->pLTMgr = new CLTMgrE95;
		if (!pGDev->pLTMgr) {
			m_cMessage.InsertString(m_cMessage.GetCount(), "Light Tower Controller start FAILED");
			return CMgrStatus::OBJECTCREATEFAIL;
		}
		m_cMessage.InsertString(m_cMessage.GetCount(), "Light Tower Controller started OK");
		return CMgrStatus::STARTOK;
	}
	return CMgrStatus::ALREADYINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StopLTWMgr() {
	if (pGDev->pLTMgr) {
		delete pGDev->pLTMgr;
		pGDev->pLTMgr = NULL;
		return CMgrStatus::STOPOK;
	}
	return CMgrStatus::NOTINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartDoorLockMgr() {
	if (!pGDev->pDrLockMgr) {
		pGDev->pDrLockMgr = new CDrLockMgr;
		if (!pGDev->pDrLockMgr) {
			m_cMessage.InsertString(m_cMessage.GetCount(), "Door Lock Mgr start FAILED");
			return CMgrStatus::OBJECTCREATEFAIL;
		}
		m_cMessage.InsertString(m_cMessage.GetCount(), "Door Lock Mgr started OK");
		return CMgrStatus::STARTOK;
	}
	return CMgrStatus::ALREADYINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StopDoorLockMgr() {
	if (pGDev->pDrLockMgr) {
		delete pGDev->pDrLockMgr;
		pGDev->pDrLockMgr = NULL;
		return CMgrStatus::STOPOK;
	}
	return CMgrStatus::NOTINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StopIDRMgr() {
	if (pGDev->pIDRMgr) {
		if (CSMClt::StopSvrThread(pGDev->pIDRMgr->SvrName, pGDev->pIDRMgr->type)) {
			pGDev->pIDRMgr = NULL;
			Log("IDRMgr stopped", 10);
			return CMgrStatus::STOPOK;
		}
		return CMgrStatus::STOPTHREADFAIL;
	}
	return CMgrStatus::NOTINSTANTIATED;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartIDRMgr() {
	if (pGDev->pIDRMgr) {
		Log("CIDRMgr already exist", 2);
		ASSERT(0);
		return CMgrStatus::ALREADYINSTANTIATED;
	}
	else {
		CString str;
		pGDev->pIDRMgr = new CIDRMgr;
		if (!pGDev->pIDRMgr) {
			Log("CIDRMgr alloc error", 2);
			return CMgrStatus::OBJECTCREATEFAIL;
		}
		if (!CSMClt::StartSvrThread(pGDev->pIDRMgr)) {
			str = "CIDRMgr start FAILED";
			Log(str, 0);
			m_cMessage.InsertString(m_cMessage.GetCount(), str);
			return CMgrStatus::STARTTHREADFAIL;
		}
		// 		str = "CIDRMgr started OK";
		// 		Log(str, 10);
		// 		m_cMessage.InsertString(m_cMessage.GetCount(),str);
		while (!pGDev->pIDRMgr->bStarted) {
			Sleep(10);
		}

		return StartIDRUnits();
	}
	return CMgrStatus::FUNCTIONLEAK;
}

CMgrStatus::STATUSFLAG CInfoPanelWelcome::StartIDRUnits() {
	CSMOL SM;
	CSMClt ipc;
	BOOL ret = TRUE;

	CString msg;
	// Number of CIDR must match number of LP
	for (int i = 0; i < DosUtil.GetNumLP(); i++) {
		SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
		SM.Pm.act = PrcsMgrServices::STARTUNIT;
		SM.HWin = pGDev->MgrStatus.hRpyWnd;
		switch (i) {
		case 0:
			SM.Ack = ID_CID_STARTED1;
			SM.Nak = ID_CID_STARTFAILED1;
			break;
		case 1:
			SM.Ack = ID_CID_STARTED2;
			SM.Nak = ID_CID_STARTFAILED2;
			break;
		default:
			ASSERT(0);
			break;
		}
		SM.Pm.unit = i + 1;
		SM.Pm.port = DosUtil.GetCIDRPort(i + 1);
		if (DosUtil.GetCIDRSimu(i + 1)) {
			SM.Pm.port = 0;
		}
		if (ipc.Start1(pGDev->pIDRMgr->SvrName, sizeof(CSMOL))) {
			if (ipc.Send(&SM)) {
				msg.Format("CIDRSvr %d started", i + 1);
			}
			else {
				ret = FALSE;
				msg.Format("Fail to start CIDSvr %d", i + 1);
			}
			Log(msg);
		}
		else {
			ret = FALSE;
			msg.Format("Fail to start CIDRSvr %d", i + 1);
			Log(msg);
		}
	}
	if (ret) {
		for (int j = 0; j < DosUtil.GetNumLP(); j++) {
			while (!pGDev->pIDRMgr->pCIDRSvr[j]) {
				Sleep(100);
			}
		}
		return CMgrStatus::STARTOK;
	}
	return CMgrStatus::STARTUNITFAIL;
}

BOOL CInfoPanelWelcome::AllStarted() {
	return !bStartFault;
}