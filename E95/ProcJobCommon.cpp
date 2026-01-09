#include "stdafx.h"

#include "IPC/Smol.h"
#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/SMCHdr.h"
#include "NavigationPanel.h"
#include "ProcJobInfo.h"
#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/PRJOB.H"
#include "ProcJob413ProcessingDlg.h"
#include "SYS/CUMMsg.h"

#include "ProcJobCommon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcJobCommon::CProcJobCommon() {
	ID = 0;
	type = 0;
	m_Parent = NULL;
	m_CarrierID = "";

	CJMONTimeoutCounter = 0;
	AreYouThereRetryCount = 0;
	bOutOfServiceAlarmSet = FALSE;
}

CProcJobCommon::~CProcJobCommon() {}

BOOL CProcJobCommon::RegisterWithSecsSvr(CWnd* pWnd, short ID) {
	return pGDev->pGEMObjs->RegisterWithSecsSvr(pWnd, ID);
}

void CProcJobCommon::Log(CString msg, short lvl) {}

void CProcJobCommon::SetContentMap() {
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	pInfo->Cas.JobInfo.LPUnit = ID + 1; // Load port that
	pInfo->Cas.winfo.SetLPStation(1, 1);

	for (int i = 0; i < MAXCAPACITYLEN; i++) {
		CWaferInfo* p = pInfo->Cas.winfo.FindWafer(i + 1);
		if (pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP[i] == '3') {
			if (!p) {
				p = new CWaferInfo;
				p->wafernum = i + 1;
				pInfo->Cas.winfo.Add(p, FALSE);
			}
			if (p) {
				p->bSelected = TRUE;
			}
		}
		else {
			if (p) {
				p->bSelected = FALSE;
			}
		}
	}
	// TEMP: For testing
	//	for (i=0; i<25; i++) {
	//		CWaferInfo *p = pInfo->Cas.winfo.FindWafer(i+1);
	//		if (p) {
	//			p->fthick = 1000.0f;
	//			CString str;
	//			str.Format("Wafer%i",i+1);
	//			p->SubstID=str;
	//			p->wthick = 0.725f;
	//			p->wafernum = i+1;
	//		}
	//	}

	//	p = &pUDev->m_ProcJobInfo[ID].SltInfo[1];
	//	p->m_FilmThickness = 2000.0f;
	//	p->m_WaferID = "Wafer2";
	//	p->m_WaferThickness = 0.725f;
	//	p->m_SlotNum = 2;

	//	p = &pUDev->m_ProcJobInfo[ID].SltInfo[2];
	//	p->m_FilmThickness = 3000.0f;
	//	p->m_WaferID = "Wafer3";
	//	p->m_WaferThickness = 0.725f;
	//	p->m_SlotNum = 2;

	//	p = &pUDev->m_ProcJobInfo[ID].SltInfo[3];
	//	p->m_FilmThickness = 4000.0f;
	//	p->m_WaferID = "Wafer4";
	//	p->m_WaferThickness = 0.725f;
	//	p->m_SlotNum = 2;
	// TEMP: End
}

void CProcJobCommon::Message(CString msg, short lvl) {}

void CProcJobCommon::ProceedToProcessing() {
	if (m_Parent) {
		m_Parent->PostMessage(UM_TO_PROCJOBPROCESSING, ID, type);
	}
}

void CProcJobCommon::ProceedToCancelCarrier() {
	Message("Cancel and quit", 5);
	pGDev->SetMessage("");
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
	if (m_Parent) {
		m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, NULL);
	}
}

CProcJobCommon::CANSTARTSTATE CProcJobCommon::CanStart() {
	int i;
	for (i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {  // Do not start if other load port is in processing mode
			if (pUDev->pProcJob413ProcessingDlg[i]) {
				if (pUDev->pProcJob413ProcessingDlg[i]->bProcessing) {
					return NOTREADY;
				}
			}
		}
	}

	Sleep(1000);

	for (i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {  // Do not start if other load port is in processing mode
			if (pUDev->pProcJob413ProcessingDlg[i]) {
				if (pUDev->pProcJob413ProcessingDlg[i]->bProcessing) {
					return NOTREADY;
				}
			}
		}
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
	return OKTOSTART;
}

CProcJobCommon::RTNCJMON CProcJobCommon::DoCJMonitor() {
	// Look for CJ that has the correct carrier ID and port ID assignment
	// Then start the processing thread, the processing thread will than start the CJ and continue monitoring it
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];

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
							Message("Control job located", 5);
							Message("Waiting for resources", 5);
							pInfo->CJobName = CJobList[0].c_str();	// record CJ name so that RT\HT ProcessingDlg can pick it up
							return pjcCANSTART;	// Tell caller to start IDT_CANSTART timer [5/15/2012 Yuen]
						}
					}
				}
			}
		}
	}
	if (CJMONTimeoutCounter < 120) {
		CJMONTimeoutCounter++;
		return pjcCJMON;	// Tell caller to start IDT_CJMON timer [5/15/2012 Yuen]
		// Must end with return [5/15/2012 Yuen]
	}

	// No CJ arrive after 120 seconds, will abort and dequeue
	pGDev->pOLnClt->GEMEventCarrierWaitForCJTimeout(ID + 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1CJWAITTIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2CJWAITTIMEOUT);
		break;
	}
	return pjcERR;	// Error [5/15/2012 Yuen]
}

//20130821 SCOTT ADD ------------------------------------------

CProcJobCommon::RTNCODE CProcJobCommon::UnlatchFOUP(HWND hWnd) {
	if (pGDev->GetARAMSState() == SArams::STANDBY) {
		pGDev->SetARAMSState(SArams::PRODUCTIVE);
	}

	Message("Unlatching carrier", 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	SM.HWin = hWnd;
	SM.Ack = NULL;//ReplyOK;
	SM.Nak = NULL;//ReplyFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return ofOK;
		}
		Message("Unlatch carrier command send fail", 0);
		return ofFAIL;
	}
	Message("LPMgr not found", 0);
	return ofOBJERR;
}

//-----------------------------------------------------------

CProcJobCommon::RTNCODE CProcJobCommon::OpenFOUP(HWND hWnd, int ReplyOK, int ReplyFAIL) {
	Message("Opening carrier", 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = hWnd;
	SM.Ack = ReplyOK;
	SM.Nak = ReplyFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = ID + 1;
	SM.Pm.act = PrcsMgrServices::OPENFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return ofOK;
		}
		Message("Open FOUP command send fail", 2);
		return ofFAIL;
	}
	Message("LPMgr not found", 2);
	return ofOBJERR;
}

void CProcJobCommon::ShowContentMap() {
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];

	CString m_ContentMap;
	m_CarrierID = pInfo->Cas.JobInfo.CarrierID;
	Message(m_CarrierID);
	for (int i = 1; i <= 25; i++) {
		CWaferInfo* pWI = pInfo->Cas.winfo.FindWafer(i);
		if (pWI) {
			m_ContentMap.Format("Slot %d: ", i);
			m_ContentMap += pWI->LotID;
			m_ContentMap += " ";
			m_ContentMap += pWI->WaferID;
			Message(m_ContentMap);
		}
	}
}

BOOL CProcJobCommon::NoOtherProcJobRuning() {
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {	// Do not start if other load port is in processing mode
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

void CProcJobCommon::SetMaterialRecieved() {
	Message("Material received (1)", 5);
	pGDev->pOLnClt->GEMEventMaterialRecieved(ID + 1);
	pGDev->pGEMObjs->Cms[ID].CaObj.Preset();  // Added [11/25/2010 XPMUser]
	pGDev->SetLPState(ID + 1, PrcsMgrServices::FOUPPRESENT);
	if (pUDev->pNavigationPanel) {
		pUDev->pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_BLINKJOBBUTTON, NULL);
	}
}

void CProcJobCommon::SetMaterialRemoved() {
	Message("Material removed (1)", 5);
	pGDev->pOLnClt->GEMEventMaterialRemoved(ID + 1);
	pGDev->SetLPState(ID + 1, PrcsMgrServices::NOFOUP);
	if (pUDev->pNavigationPanel) {
		pUDev->pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_STOPBLINKJOBBUTTON, NULL);
	}
}

CProcJobCommon::RTNCODE CProcJobCommon::LatchFOUP(HWND hWnd, int ReplyOK, int ReplyFAIL) {
	if (pGDev->GetARAMSState() == SArams::STANDBY) {
		pGDev->SetARAMSState(SArams::PRODUCTIVE);
	}

	Message("Latching carrier", 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::LATCHFOUP;
	SM.HWin = hWnd;
	SM.Ack = ReplyOK;
	SM.Nak = ReplyFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return ofOK;
		}
		Message("Latch carrier command send fail", 0);
		return ofFAIL;
	}
	Message("LPMgr not found", 0);
	return ofOBJERR;
}

CProcJobCommon::RTNCODE CProcJobCommon::ReadCarrierID(HWND hWnd, int ReplyOK, int ReplyFail) {
	Message("Reading carrier ID", 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.act = PrcsMgrServices::READCARRIERID;
	SM.HWin = hWnd;
	SM.Ack = ID_CID_READOK;
	SM.Nak = ID_CID_READFAILED;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	CSMClt ipc;
	if (ipc.Start1("CIDRMgr", sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			return ofOK;
		}
		Message("ReadCID command send fail", 0);
		return ofFAIL;
	}
	// No CIDRMgr
	Message("No CIDRMgr", 0);
	return ofOBJERR;
}

void CProcJobCommon::SetLatchCmpl() {
	Message("Latch carrier complete", 5);
	//	pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::INACCESS);
}

void CProcJobCommon::SetLatchFail() {
	Message("Latch carrier failed", 0);
}

void CProcJobCommon::SetSMVerificationOK() {
	if (pGDev->pGEMObjs->IsOnline()) {
		pGDev->pGEMObjs->SetCOSlotmapStatus(
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID, ID + 1,
			pGDev->pGEMObjs->Cms[ID].CaObj.SlotMaP, CCarrierObj::SMVERIFICATIONOK);
	}
}

void CProcJobCommon::SetCarrierIDReadOK(char* carrierID) {
	m_CarrierID = carrierID;
	pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;

	pGDev->pGEMObjs->Cms[ID].CaObj.SetID(carrierID);
	pGDev->pGEMObjs->SetLPAssociation(m_CarrierID, ID + 1, CLPObj::ASSOCIATED);
}

CProcJobCommon::RTNCODE1 CProcJobCommon::CarrierIDVerification() {
	CString msgstr;
	Message("ID verification", 5);

	switch (VerifyID()) {
	case CCarrierObj::IDVERIFICATIONOK:
	{
		pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
		msgstr.Format("Carrier ID Verified: %s", m_CarrierID);
		Message(msgstr, 5);
		switch (pGDev->pGEMObjs->GetLineMode()) {
		case EqpStatus::REMOTE:
			return ofRTN4;
			break;
		default:
			return ofRTN3;
			break;
		}
	}
	break;
	case CCarrierObj::IDWAITINGFORHOST:		// effective during online only [12/24/2007 YUEN]
		msgstr.Format("Carrier ID Need Verification from Host: %s", m_CarrierID);
		Message(msgstr, 5);
		return ofRTN2;
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:	// effective during online only [12/24/2007 YUEN]
		return ofERR;
		break;
	default:
		ASSERT(0);
		break;
	}
	return ofRTN1;
}

CCarrierObj::IDSTATUS CProcJobCommon::VerifyID() {
	Message("Verifying carrier ID", 5);

	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)) {
		//20120920 SCOTT TEMP EDIT pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID,ID+1,CCarrierObj::IDNOTREAD);
		if (pGDev->SubModel == CGlobalDeviceContainer::UMC) //201620 REFER TO
		{
			pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDNOSTATE);
		}
		else {
			pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDNOTREAD);
		}
		pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, "", CCarrierObj::SMNOTREAD);
		return VerifyCarrierID();
	}
	else if (pGDev->pGEMObjs->GetLineMode() == EqpStatus::LOCAL) //  [10/27/2008 Yuen]
	{
		pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
		pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, "", CCarrierObj::SMNOTREAD);
		return CCarrierObj::IDVERIFICATIONOK;
	}
	else {
		// Manual mode, simply return verification OK [12/24/2007 YUEN]
		pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
		pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, "", CCarrierObj::SMNOTREAD);
		return CCarrierObj::IDVERIFICATIONOK;
	}
}

CCarrierObj::IDSTATUS CProcJobCommon::VerifyCarrierID() {
	CCarrierObj* c = pGDev->pGEMObjs->FindCarrier(m_CarrierID);
	if (c) {
		// Carrier has been instantiated
		if (pGDev->pGEMObjs->GetPortOfCarrier(m_CarrierID) == ID + 1) {
			CCarrierObj::IDSTATUS otherStatus;
			otherStatus = pGDev->pGEMObjs->GetCOIDStatus(m_CarrierID);
			if (otherStatus < CCarrierObj::IDWAITINGFORHOST) {
				// CarrierID Verified since we can find the carrier ID. Carrier must have been previously binded
				pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDWAITINGFORHOST);
				return CCarrierObj::IDWAITINGFORHOST;
			}
			else {
				// CarrierID Verified since we can find the carrier ID. Carrier must have been previously binded
				pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
				return CCarrierObj::IDVERIFICATIONOK;
			}
		}
		else {
			// Carrier has been associated with other port
			CCarrierObj::IDSTATUS otherStatus;
			otherStatus = pGDev->pGEMObjs->GetCOIDStatus(m_CarrierID);
			// Check for commitment
			if (otherStatus >= CCarrierObj::IDWAITINGFORHOST) {
				// Carrier has been committed
				switch (ID) {
				case 0:
					pGDev->SetAlarm(CAlrmList::LP1DUPLICATEDCARRIERID);
					break;
				case 1:
					pGDev->SetAlarm(CAlrmList::LP2DUPLICATEDCARRIERID);
					break;
				default:
					ASSERT(0);
					break;
				}
				Message("There are 2 carrier with same ID", 0);
				Message("Scenario will stop", 0);
				pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONFAIL);
				return CCarrierObj::IDVERIFICATIONFAIL;
			}
			else {
				// Carrier has been associated with other port but has not been committed
				short otherPort = pGDev->pGEMObjs->GetPortOfCarrier(m_CarrierID);
				if (!otherPort) {
					// Logic error, could not continue
					ASSERT(0);
					return CCarrierObj::IDVERIFICATIONFAIL;
				}
				if (pGDev->pGEMObjs->GetLPAssociation(otherPort) == CLPObj::ASSOCIATED) {
					// This port is associated
					CString str;
					if (pGDev->pGEMObjs->GetLPAssociatedCarrier(ID + 1, str) == CLPObj::ASSOCIATED) {
						// Carrier can only be binded at this stage, physically not possible to load 2 cassette on one load port
						CCarrierObj* c2;
						c2 = pGDev->pGEMObjs->FindCarrier(str);
						// Remove any bind to this port
						pGDev->pGEMObjs->SelfCancelBind(c2);
						// Bind carrier to this port
						pGDev->pGEMObjs->SetLPAssociation(m_CarrierID, ID + 1, CLPObj::ASSOCIATED);
						pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
						pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDWAITINGFORHOST);
						// CarrierID Verified
						return CCarrierObj::IDWAITINGFORHOST;
					}
					else {
						// This port is not associated
						str = "";
						pGDev->pGEMObjs->SetLPAssociation(str, otherPort, CLPObj::NOTASSOCIATED);
						str = "";
						pGDev->pGEMObjs->SetLPAssociation(str, ID + 1, CLPObj::ASSOCIATED);
						pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
						pGDev->pGEMObjs->SetCOIDStatus(str, ID + 1, CCarrierObj::IDWAITINGFORHOST);
						// CarrierID need Verification
						return CCarrierObj::IDWAITINGFORHOST;
					}
				}
				else {
					// This port is associated
					CString str;
					if (pGDev->pGEMObjs->GetLPAssociatedCarrier(ID + 1, str) == CLPObj::ASSOCIATED) {
						CCarrierObj* c2;
						c2 = pGDev->pGEMObjs->FindCarrier(str);
						// Remove any bind to this port
						pGDev->pGEMObjs->SelfCancelBind(c2);
						// Bind carrier to this port
						pGDev->pGEMObjs->SetLPAssociation(m_CarrierID, ID + 1, CLPObj::ASSOCIATED);
						pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
						pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
						// CarrierID Verified
						return CCarrierObj::IDVERIFICATIONOK;
					}
					else {
						// This port is not associated
						str = "";
						pGDev->pGEMObjs->SetLPAssociation(str, otherPort, CLPObj::NOTASSOCIATED);
						str = "";
						pGDev->pGEMObjs->SetLPAssociation(str, ID + 1, CLPObj::ASSOCIATED);
						pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
						pGDev->pGEMObjs->SetCOIDStatus(str, ID + 1, CCarrierObj::IDWAITINGFORHOST);
						// CarrierID need Verification
						return CCarrierObj::IDWAITINGFORHOST;
					}
				}
			}
		}
	}
	else {
		// Carrier has not been instantiated
		CString str;
		// Find out if load port has association
		c = pGDev->pGEMObjs->GetCarrierAtPort(ID + 1);
		if (c) {
			// Load port associated with other carrier
			// Remove any bind to this port
			str = c->ObjID;
			if (str.GetLength() > 0) {
				pGDev->pGEMObjs->RemoveCarrierObject(str);
				pGDev->pGEMObjs->SetLPAssociation(str, ID + 1, CLPObj::NOTASSOCIATED);
			}
		}
		// Instantiate a new carrier object
		if (pGDev->pGEMObjs->SelfBindCarrierObject(m_CarrierID, ID + 1)) {
			// Carrier object instantiated successfully
			// Waiting for host
			pGDev->pGEMObjs->SetLPAssociation(m_CarrierID, ID + 1, CLPObj::ASSOCIATED);
			pGDev->pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::NOTACCESSED);
			pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDWAITINGFORHOST);
			// CarrierID need verification
			return CCarrierObj::IDWAITINGFORHOST;
		}
		else {
			// Fail to instantiate carrier object
			Message("Scenario could not proceed", 10);
			pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONFAIL);
			return CCarrierObj::IDVERIFICATIONFAIL;
		}
	}
	// Leak
	ASSERT(0);
	pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONFAIL);
	return CCarrierObj::IDVERIFICATIONFAIL;
}

void CProcJobCommon::SetIDVerificationOK() {
	pGDev->pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
}

void CProcJobCommon::SetOHTLoadComplete() {
	Message("Load completed received (1)", 5);

	pGDev->pGEMObjs->Cms[ID + 1].LoadComplete(ID + 1);
}