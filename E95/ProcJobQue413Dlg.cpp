// ProcJobQue413Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "128/GEM/PrJobList.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/Smol.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/PRJOB.H"
#include "ProcJobInfo.h"
#include "SYS/CUMMsg.h"
#include "SYS/Tool.h"

#include "ProcJobQue413Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_CANSTART			9999
#define IDT_AREYOUTHERETIMEOUT	9998
#define IDT_DATAREQ_TIMEOUT		9997
#define IDT_CJMON				9996

/////////////////////////////////////////////////////////////////////////////
// CProcJobQue413Dlg dialog

CProcJobQue413Dlg::CProcJobQue413Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobQue413Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobQue413Dlg)
	m_Status = _T("");
	//}}AFX_DATA_INIT

	ID = 0;
	m_Parent = NULL;
	CJMONTimeout = 0;
	AreYouThereRetryCount = 0;
	bOutOfServiceAlarmSet = FALSE;
}

void CProcJobQue413Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobQue413Dlg)
	DDX_Control(pDX, IDC_PROCEED, m_cProceed);
	DDX_Control(pDX, IDC_CANCEL, m_cCancel);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobQue413Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobQue413Dlg)
	ON_BN_CLICKED(IDC_PROCEED, OnProceed)
	ON_WM_TIMER()
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_COMMAND(ID_IPC_HELLOACK, OnIpcHelloack)
	ON_MESSAGE(ID_IPC_DATAREQACK1, OnIpcDatareqack)
	ON_MESSAGE(ID_IPC_DATAREQNAK1, OnIpcDatareqnak)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_MESSAGE(ID_SVR_PJDEFINE, OnSvrPjdefine)
	ON_MESSAGE(ID_SVR_CJDEFINE, OnSvrCjdefine)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SVR_CANCELCARRIER, OnSvrCancelcarrier)
	ON_MESSAGE(ID_SVR_CANCELCARRIERATPORT, OnSvrCancelcarrieratport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobQue413Dlg message handlers

void CProcJobQue413Dlg::OnCancel() {
	KillTimer(IDT_AREYOUTHERETIMEOUT);
	// Note: Do not change the wording, its decoded elsewhere
	CString str;
	str.LoadString(IDS_CARRIERCANCELED);
	pGDev->SetMessage(str);
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_TO_PROCJOBQUEUED);
}

void CProcJobQue413Dlg::OnProceed() {
	ShowWindow(SW_HIDE);
	int type = 3;
	m_Parent->PostMessage(UM_TO_PROCJOBPROCESSING, ID, type);
}

BOOL CProcJobQue413Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	if ((ID < 0) || (ID > 3)) {
		ASSERT(0);
		return FALSE;
	}

	if (!m_Parent || !pGDev || !pUDev || !pGDev->pGEMObjs) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_PROCEED, TOP_RIGHT);
	AddAnchor(IDC_CANCEL, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_STATUS, TOP_LEFT, BOTTOM_LEFT);

	RegisterWithSecsSvr(this, ID);
	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	m_CarrierID = pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID;

	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE) /*&& (!pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
		m_cProceed.EnableWindow(FALSE);
		m_cCancel.EnableWindow(FALSE);
		Message("Waiting to start");
		SetTimer(IDT_CANSTART, 200, NULL);
		//SetTimer(IDT_CJMON,500,NULL);	// Look for CJ that has the correct carrier ID and port ID assignment
	}
	else {
		Message("Waiting for resources");
		SetTimer(IDT_CANSTART, 200, NULL);
	}

	srID = ID;
	srParent = m_Parent;

	Tool.SetCasStateQueued(ID + 1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcJobQue413Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_CANSTART) {
		KillTimer(IDT_CANSTART);
		switch (CanStart()) {
		case CProcJobQue413Dlg::OKTOSTART:
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
		case CProcJobQue413Dlg::NOTREADY:
			SetTimer(IDT_CANSTART, 500, NULL);
			break;
		case CProcJobQue413Dlg::ABORT:
			OnCancel();
			break;
		}
	}
	if (nIDEvent == IDT_AREYOUTHERETIMEOUT) {
		KillTimer(IDT_AREYOUTHERETIMEOUT);
		CString str;
		str = "413 unit fail to response";
		Message(str, 2);
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
		// Look for CJ that has the correct carrier ID and port ID assignment
		// Then start the processing thread, the processing thread will than start the CJ and continue monitoring it
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		//CCtrlJob *pCJ = NULL;
		vector<string> CJobList;
		if (CJobMgr.CJobIdsFromCarrier(pInfo->Cas.JobInfo.CarrierID, CJobList)) {
			if (CJobList.size()) {
				CCtrlJob* pCJ = CJobMgr.GetCJob(CJobList[0].c_str());
				if (pCJ && pCJ->IsQueued()) {
					// Need to load recipe to see which unit to run
					for (int i = 0; i < pCJ->ProcessingCtrlSpec.GetSize(); i++) {
						CString PJName = pCJ->ProcessingCtrlSpec.GetAt(i);
						CPRJob* pPJ = PJobMgr.GetPRJob(PJName);
						if (pPJ) {
							CString rcp = pPJ->m_Recipe.c_str();
							if (pInfo->Cas.Recipe413.LoadDefDir(rcp)) {
								Message("Control job located");
								Message("Waiting for resources");
								pInfo->CJobName = CJobList[0].c_str();	// record CJ name so that RT\HT ProcessingDlg can pick it up
								//StartProcessRequest();
								SetTimer(IDT_CANSTART, 200, NULL);
								return;
							}
						}
					}
				}
			}
		}
		if (CJMONTimeout < 120) {
			CJMONTimeout++;
			SetTimer(IDT_CJMON, 1000, NULL);
		}
		else {
			// No CJ arrive after 120 seconds, will abort and dequeue
			pGDev->pOLnClt->GEMEventCarrierWaitForCJTimeout(ID + 1);
			OnCancel();
		}
	}
	if (nIDEvent == IDT_DATAREQ_TIMEOUT) {
		KillTimer(IDT_DATAREQ_TIMEOUT);
		SetTimer(IDT_AREYOUTHERETIMEOUT, 10, NULL);
	}
}

CProcJobQue413Dlg::CANSTARTSTATE CProcJobQue413Dlg::CanStart() {
	// 	int i;
	// 	for (i=0; i<MAX_LPSUPPORTED; i++)
	// 	{
	// 		if (i!=ID)
	// 		{  // Do not start if other load port is in processing mode
	// 			if (pUDev->pProcJob413ProcessingDlg[i])
	// 			{
	// 				if (!pUDev->pProcJob413ProcessingDlg[i]->bDone)
	// 				{
	// 					return NOTREADY;
	// 				}
	// 			}
	// 		}
	// 	}

	//	Sleep(1000);

	// 	for (i=0; i<MAX_LPSUPPORTED; i++)
	// 	{
	// 		if (i!=ID)
	// 		{  // Do not start if other load port is in processing mode
	// 			if (pUDev->pProcJob413ProcessingDlg[i])
	// 			{
	// 				if (!pUDev->pProcJob413ProcessingDlg[i]->bDone)
	// 				{
	// 					return NOTREADY;
	// 				}
	// 			}
	// 		}
	// 	}

	if (!pUDev->CanStart(ID)) {
		return NOTREADY;
	}

	C413Obj* p413;
	p413 = pGDev->pGEMObjs->StMd.Get413Obj();
	if (p413) {
		if (p413->Service == C413Obj::OUTOFSERVICE) {
			if (!bOutOfServiceAlarmSet) {
				bOutOfServiceAlarmSet = TRUE;
				pGDev->SetAlarm(CAlrmList::C413OUTOFSERVICE);
				Message("413 Unit Out of Service");
			}
			return NOTREADY;
		}
	}

	if (p413App->Global.WaferOnStage > 0) {
		// Previous wafer has not been removed [10/7/2012 Yuen]
		return NOTREADY;
	}
	return OKTOSTART;
}

LRESULT CProcJobQue413Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

BOOL CProcJobQue413Dlg::AreYouThere() {
	// bypass distributed implementation [8/5/2010 XPMUser]
	PostMessage(WM_COMMAND, ID_IPC_HELLOACK, 0);
	return TRUE;
}

void CProcJobQue413Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobQue413Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobQue413Dlg::OnIpcHelloack() {
	KillTimer(IDT_AREYOUTHERETIMEOUT);
	StartProcessRequest();
}

void CProcJobQue413Dlg::Message(CString msg, short lvl) {
	int n = m_cList.GetCount();
	if (n > 25) {
		for (int i = n; i >= 17; i--) {
			m_cList.DeleteString(i);
		}
	}
	Log(msg, lvl);
	msg = pGDev->GetTimeStamp() + msg;
	m_cList.InsertString(0, msg);
}

int CProcJobQue413Dlg::GetHWnd() {
	return (int)m_hWnd;
}

void CProcJobQue413Dlg::SetDataReqTimeOut(long dur) {
	SetTimer(IDT_DATAREQ_TIMEOUT, dur, NULL);
}

LRESULT CProcJobQue413Dlg::OnIpcDatareqack(WPARAM wP, LPARAM lP) {
	// bypass distributed computing [8/5/2010 XPMUser]
	OnProceed();  // allowed to continue [8/5/2010 XPMUser]
	return 0;

	KillTimer(IDT_DATAREQ_TIMEOUT);

	if (wP != 999) {
		ASSERT(0);
		return -1;
	}

	DataReqAck413(lP);

	m_Status += "\n";
	m_Status += m_Message;

	// Check for abnormality here
	CString tmp; //  [10/27/2008 Yuen]
	// rearranged error message [10/27/2008 Yuen]
	if (SV413.bWaferPresent) {
		tmp = "Can not start: Wafer Present in Chamber";
		m_Status += "\n\n";
		m_Status += tmp;
		Log(tmp, 0);
		UpdateData(FALSE); //  [10/27/2008 Yuen]
		m_Status = "";
		pGDev->SetAlarm(CAlrmList::WAFERINSIDETOOL, CEIDCtlr.m_alarm.Alarms[CAlrmList::WAFERINSIDETOOL].alTX);
		return -2;
	}

	if (SV413.ServiceState == 0) {
		SetTimer(IDT_DATAREQ_TIMEOUT, 10, NULL);
		m_Status += "\nModule 413 not ready";
		UpdateData(FALSE); //  [10/27/2008 Yuen]
		m_Status = "";
		return -6;
	}

	UpdateData(FALSE); //  [10/27/2008 Yuen]

	OnProceed();

	return 0;
}

LRESULT CProcJobQue413Dlg::OnIpcDatareqnak(WPARAM wP, LPARAM lP) {
	SetTimer(IDT_DATAREQ_TIMEOUT, 10, NULL);

	// No need to delete lP

	return 0;
}

void CProcJobQue413Dlg::StartProcessRequest() {
	// bypass distributed computing [8/5/2010 XPMUser]
	PostMessage(ID_IPC_DATAREQACK1, 0, NULL);
	return;
}

LRESULT CProcJobQue413Dlg::OnSvrPjdefine(WPARAM wP, LPARAM lP) {
	int i;

	if (!lP) {
		return -1;
	}
	CSMOL* pSM = (CSMOL*)lP;

	m_Status += pSM->Pj.PrJID;
	m_Status += ": <";
	m_Status += pSM->Pj.CarrierID;
	m_Status += ">\nRecipe: ";
	m_Status += pSM->Pj.RecipeName;

	m_Status += "\nSlots:\n";
	for (i = 0; i < MAXCAPACITYLEN; i++) {
		SlotInfo* s = &pSM->Pj.slot[i];
		short slotno = s->slotno;
		if (slotno != -1) {
			CString str;
			//str.Format("Slot:%d ScanNo:%d\n",slotno,s->scanno);
			str.Format("Slot:%d\n", slotno);
			m_Status += str;
		}
		else {
			break;
		}
	}
	m_Status += "\n";

	SPcJobExec* p = new SPcJobExec;
	*p = pSM->Pj;
	PrJobs.Add(p);

	delete pSM; // Must delete because it is created using new operator

	UpdateData(FALSE);

	return 0;
}

LRESULT CProcJobQue413Dlg::OnSvrCjdefine(WPARAM wP, LPARAM lP) {
	if (!lP) {
		return -1;
	}
	CSMOL* pSM = (CSMOL*)lP;

	CCtrlJob* pJ = CJobMgr.GetCJob(pSM->Pj.PrJID);
	if (pJ) {
		m_Status += pSM->Pj.PrJID;
		m_Status += ": \n";

		for (int n = 0; n < pJ->CarrierInputSpec.GetSize(); n++) {
			m_Status += "Carriers : <";
			m_Status += pJ->CarrierInputSpec.GetAt(n);
			m_Status += "> ";
		}
	}
	delete pSM;

	UpdateData(FALSE);

	return 0;
}

long CProcJobQue413Dlg::OnSvrCancelcarrier(WPARAM wP, LPARAM lP) {
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

long CProcJobQue413Dlg::OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP) {
	return OnSvrCancelcarrier(wP, lP);
}

void CProcJobQue413Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}