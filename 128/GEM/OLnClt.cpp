// OLnClt.cpp: implementation of the COLnClt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"

#include "413App.h"
#include "GEMEvt.h"
#include "128/GEM/ObjectsContainer.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMHdr.h"
#include "OLnSvrObj.h"
#include "ONLINE/GEM/CEID_LIST.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "OnlineMgr.h"
// #include "PrJobList.h"

#include "OLnClt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLnClt::COLnClt() {}

COLnClt::~COLnClt() {}

BOOL COLnClt::GEMEvent(CGEMEvt& Ge) {
	if (!CanSend()) {
		return FALSE;
	}

	BOOL ret = pGDev->pOLnMgr->pOLnSvrObj->GemEvents(Ge);
	if (!ret) {
		Log("GEMEvent failed", 1);
	}
	return ret;
}

BOOL COLnClt::CAEvent(CSMOL& SM) {
	if (!CanSend()) {
		return FALSE;
	}

	SM.cmd = CSMOL::cmdCARRIERACTION;
	BOOL ret = pGDev->pOLnMgr->pOLnSvrObj->ExecCarrierAction(SM);
	if (!ret) {
		Log("CAEvent failed", 1);
	}
	return ret;
}

void COLnClt::InitLPUpdate(short PortNo, short Present) {
	CLPObj::ACCESSMODE mode = pGDev->pGEMObjs->GetLPAccessMode(PortNo);
	if (mode == CLPObj::MANUAL) {
		pGDev->pGEMObjs->SetLPAccessMode(PortNo, CLPObj::AUTO); // To toggle inner SetLPAccessMode protection
	}
	else {
		pGDev->pGEMObjs->SetLPAccessMode(PortNo, CLPObj::MANUAL);
	}
	pGDev->pGEMObjs->SetLPAccessMode(PortNo, mode);
}

void COLnClt::InitEqpUpdate() {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTUPDATEARAMS;
	SM.sub[0] = 1;	// Function type
	pGDev->LogFile.Log(SM);
}

BOOL COLnClt::GEMEventFoupClosed(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::FOUPCLOSEDOK;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: FOUP closed", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventMaterialRecieved(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::MATERIALMOVEMENT;
	Ge.reason = CGEMEvt::MATERIALRECEIVED;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Material Received", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventFoupOpened(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::FOUPOPENEDOK;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: FOUP opened", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventFoupClamped(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::CLAMPOK;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: FOUP clamped", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventFoupUnclamped(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::UNCLAMPOK;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: FOUP unclamped", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventSlotmapVerificationFailed(short PortNo, CString CarrierID) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLOTMAPVERIFICATION;
	Ge.reason = CGEMEvt::SLOTMAPVERIFICATIONFAIL;
	Ge.PortID = PortNo;
	strscpy(Ge.CarrierID, OBJIDLEN, CarrierID);
	if (pGDev) {
		switch (PortNo) {
		case 1:
			pGDev->SetAlarm(CAlrmList::SLOTMAP1VERIFICATIONFAIL);
			break;
		case 2:
			pGDev->SetAlarm(CAlrmList::SLOTMAP2VERIFICATIONFAIL);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d:%s Slot map verification failed", PortNo, CarrierID);
		Log(strMsg, 1);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventSlotmapVerificationOK(short PortNo, CString CarrierID) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLOTMAPVERIFICATION;
	Ge.reason = CGEMEvt::SLOTMAPVERIFIED;
	Ge.PortID = PortNo;
	strscpy(Ge.CarrierID, OBJIDLEN, CarrierID);
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d:%s Slot map verified", PortNo, CarrierID);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventMaterialRemoved(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::MATERIALMOVEMENT;
	Ge.reason = CGEMEvt::MATERIALREMOVED;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Material removed", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventCarrierObjNotFound(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::CARRIEROBJECT;
	Ge.reason = CGEMEvt::CARRIEROBJNOTFOUND;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Carrier Object not found", PortNo);
		Log(strMsg, 1);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventCarrierWaitForCJTimeout(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::CARRIEROBJECT;
	Ge.reason = CGEMEvt::CARRIERWAITFORCJTIMEOUT;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Carrier Object not found", PortNo);
		Log(strMsg, 1);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::CSOProcStateEvent(CGEMEvt& Ge) {
	// Send this event to host
	Ge.act = CGEMEvt::SOSMCHANGED;
	//Ge.SuObj.SubstProcState = Ge.SuObj.SubstProcState;
	Ge.SuObj.SubstState = CSubstObj::igSUBSTSTATE;
	//Ge.Slot = Ge.Slot;
	//Ge.PortID = Ge.PortID;
	//strncpy(Ge.CarrierID,CarrierID,OBJIDLEN);
	//strcpy(Ge.SubstID,Ge.SubstID);
	if (GEMEvent(Ge)) {
		strMsg.Format("Carrier %s slot %d ProcState is ", Ge.CarrierID, Ge.Slot);
		switch (Ge.SuObj.SubstProcState) {
		case CSubstObj::NEEDS_PROCESSING:
			strMsg += "NEEDS_PROCESSING";
			Log(strMsg, 10);
			break;
		case CSubstObj::IN_PROCESS:
			strMsg += "IN_PROCESS";
			Log(strMsg, 10);
			break;
		case CSubstObj::PROCESSED:
			strMsg += "PROCESSED";
			Log(strMsg, 10);
			break;
		case CSubstObj::ABORTED:
			strMsg += "ABORTED";
			Log(strMsg, 10);
			break;
		case CSubstObj::STOPPED:
			strMsg += "STOPPED";
			Log(strMsg, 10);
			break;
		case CSubstObj::REJECTED:
			strMsg += "REJECTED";
			Log(strMsg, 10);
			break;
		}
		return TRUE;
	}
	Log("Fail to send SOSMCHANGED", 2);
	return FALSE;
}

BOOL COLnClt::CSOStateEvent(CGEMEvt& Ge) {
	// Send this event to host
	Ge.act = CGEMEvt::SOSMCHANGED;
	Ge.SuObj.SubstProcState = CSubstObj::igPROCSTATE;
	//Ge.SuObj.SubstState = Ge.SuObj.SubstState;
	//Ge.Slot = Ge.Slot;
	//Ge.PortID = Ge.PortID;
	//strncpy(Ge.CarrierID,CarrierID,OBJIDLEN);
	//strcpy(Ge.SubstID,Ge.SubstID);

	strMsg.Format("%d:%d", Ge.PortID, Ge.Slot);
	strcpy(Ge.LocID, strMsg);
	if (GEMEvent(Ge)) {
		strMsg.Format("Carrier %s slot %d substrate %s state is ", Ge.CarrierID, Ge.Slot, Ge.SubstID);
		switch (Ge.SuObj.SubstState) {
		case CSubstObj::ATSOURCE:
			strMsg += "ATSOURCE";
			Log(strMsg, 10);
			break;
		case CSubstObj::ATWORK:
			strMsg += "ATWORK";
			Log(strMsg, 10);
			break;
		case CSubstObj::ATDESTINATION:
			strMsg += "ATDESTINATION";
			Log(strMsg, 10);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::COSLOccupancyEvent(CGEMEvt& Ge) {
	// Send this event to host
	Ge.act = CGEMEvt::COSLCHANGED;
	//Ge.PortID = Ge.PortID;
	//Ge.Slot=Ge.Slot;
	//Ge.SuObj.SubstState = Ge.SuObj.SubstState;
	//strncpy(Ge.CarrierID,CarrierID,OBJIDLEN);

	strMsg.Format("%d:%d", Ge.PortID, Ge.Slot);
	strscpy(Ge.LocID, SUBSTLOCIDLEN, strMsg);
	if (GEMEvent(Ge)) {
		strMsg.Format("Carrier %s slot %d is ", Ge.CarrierID, Ge.Slot);
		switch (Ge.SuObj.SubstState) {
		case CSubstObj::SM_UNDEFINED:
			strMsg += "UNDEFINED";
			Log(strMsg, 10);
			break;
		case CSubstObj::SM_EMPTY:
			strMsg += "NOTOCCUPIED";
			Log(strMsg, 10);
			break;
		case CSubstObj::SM_CORRECTLY_OCCUPIED:
			strMsg += "OCCUPIED";
			Log(strMsg, 10);
			break;
		case CSubstObj::SM_DOUBLE_SLOTTED:
			strMsg += "DOUBLESLOTTED";
			Log(strMsg, 1);
			break;
		case CSubstObj::SM_CROSS_SLOTTED:
			strMsg += "CROSSSLOTTED";
			Log(strMsg, 1);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GoRemote() {
	Log("Go Remote", 5);
	pGDev->pGEMObjs->SetLineMode(EqpStatus::REMOTE);

	pGDev->SetARAMSState(pGDev->GetARAMSState());

	//Log("GoRemote:Clear All Pending Process Job",2);
	//PrJobs.Clear();

	return TRUE;
}

BOOL COLnClt::GoOfflineHost() {
	Log("Go Offline (Host)", 5);
	pGDev->pGEMObjs->SetLineMode(EqpStatus::OFFLINEHOST);
	return TRUE;
}

BOOL COLnClt::GoOfflineEqp() {
	Log("Go Offline (Equipment)", 5);
	pGDev->pGEMObjs->SetLineMode(EqpStatus::OFFLINEEQP);
	return TRUE;
}

BOOL COLnClt::GoLocal() {
	Log("Go Local", 5);

	pGDev->pGEMObjs->SetLineMode(EqpStatus::LOCAL);
	pGDev->SetARAMSState(pGDev->GetARAMSState());

	//Log("Go Local: Clear All Pending Process Job",5);
	//PrJobs.Clear();

	return TRUE;
}

BOOL COLnClt::SetLPStatus(CLPStatus& Lp) {
	CCarrierObj* c;
	CString CarrierID;

	if (Lp.Association != CLPObj::igASSOCIATION) {
		c = pGDev->pGEMObjs->GetCarrierAtPort(Lp.PortNo);
		if (c) {
			CarrierID = c->ObjID;
			return pGDev->pGEMObjs->SetLPAssociation(CarrierID, Lp.PortNo, Lp.Association);
		}
		else {
			CarrierID = "";
			return pGDev->pGEMObjs->SetLPAssociation(CarrierID, Lp.PortNo, Lp.Association);
		}
	}
	if (Lp.AccessMode != CLPObj::igACCESSMODE) {
		if (pGDev->pGEMObjs->GetLPTransferStatus(Lp.PortNo) != CLPObj::OUTOFSERVICE) {
			return pGDev->pGEMObjs->SetLPAccessMode(Lp.PortNo, Lp.AccessMode);
		}
		return FALSE;
	}
	if (Lp.Occupancy != CLPObj::igOCCUPENCY) {
		return pGDev->pGEMObjs->SetLPOccupancy(Lp.PortNo, Lp.Occupancy);
	}
	if (Lp.Reservation != CLPObj::igRESERVATION) {
		return pGDev->pGEMObjs->SetLPReservation(Lp.PortNo, Lp.Reservation);
	}
	if (Lp.TransferStatus != CLPObj::igTRANSFERSTATUS) {
		return pGDev->pGEMObjs->SetLPTransferStatus(Lp.PortNo, Lp.TransferStatus);
	}
	return FALSE;
}

void COLnClt::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "OLnClt");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

CSubstObj* COLnClt::RemoveCO(short PortNo, CCarrierObj* c, short slot) {
	if (!c || (slot > MAXCAPACITYLEN)) {
		ASSERT(0);
		return NULL;
	}

	strMsg.Format("LP%d: Substrate %s removed from source cassette", PortNo, c->Substrate[slot - 1].ObjID);
	Log(strMsg, 5);

	c->Substrate[slot - 1].SlotNo = slot;  // Appears to be redundant but crucial [8/5/2010 XPMUser]
	c->SlotMaP[slot - 1] = CSubstObj::SM_EMPTY;

	CSubstObj* pSObj = pGDev->pGEMObjs->RemoveCaSLObj(c, slot, PortNo);

	strMsg.Format("LP%d: Substrate %s is ATWORK amd IN_PROCESS", PortNo, c->Substrate[slot - 1].ObjID);
	Log(strMsg, 5);

	return pSObj;
}

// YFTD
BOOL COLnClt::AlgPlace(short PortNo, CSubstObj* s) {
	if (!s) {
		ASSERT(0);
		return FALSE;
	}
	strMsg.Format("LP%d: Substrate %s placed on aligner", PortNo, s->ObjID);
	Log(strMsg, 5);
	return pGDev->pGEMObjs->SetAlgSLObj(PortNo, s);
}

CSubstObj* COLnClt::RemoveAlg(short PortNo) {
	CSubstObj* s = pGDev->pGEMObjs->RemoveAlgSLObj(PortNo);
	strMsg.Format("LP%d: Substrate %s removed from aligner", PortNo, s->ObjID);
	Log(strMsg, 5);
	return s;
}

BOOL COLnClt::SclPlace(short PortNo, CSubstObj* s) {
	if (!s) {
		ASSERT(0);
		return FALSE;
	}
	strMsg.Format("LP%d: Substrate %s placed on scale", PortNo, s->ObjID);
	Log(strMsg, 5);
	return pGDev->pGEMObjs->SetSclSLObj(PortNo, s);
}

CSubstObj* COLnClt::RemoveScl(short PortNo) {
	CSubstObj* s = pGDev->pGEMObjs->RemoveSclSLObj(PortNo);
	strMsg.Format("LP%d: Substrate %s removed from scale", PortNo, s->ObjID);
	Log(strMsg, 5);
	return s;
}

BOOL COLnClt::ScnPlace(short PortNo, CSubstObj* s) {
	if (!s) {
		ASSERT(0);
		return FALSE;
	}
	strMsg.Format("LP%d: Substrate %s placed on scanner", PortNo, s->ObjID);
	Log(strMsg, 5);
	return pGDev->pGEMObjs->SetScnSLObj(PortNo, s);
}

CSubstObj* COLnClt::RemoveScn(short PortNo) {
	CSubstObj* s = pGDev->pGEMObjs->RemoveScnSLObj(PortNo);
	strMsg.Format("LP%d: Substrate %s removed from scanner", PortNo, s->ObjID);
	Log(strMsg, 5);
	return s;
}

BOOL COLnClt::COPlace(short PortNo, short slot, CCarrierObj* c, CSubstObj* s) {
	if (!s || (slot > MAXCAPACITYLEN)) {
		ASSERT(0);
		return FALSE;
	}

	strMsg.Format("LP%d: Substrate %s returned to destination cassette", PortNo, s->ObjID);
	Log(strMsg, 5);
	BOOL ret = pGDev->pGEMObjs->SetCaSLObj(c, slot, PortNo, s);
	c->SlotMaP[slot - 1] = CSubstObj::SM_CORRECTLY_OCCUPIED;
	return TRUE;
}

BOOL COLnClt::UpdateCarrierObject(CSMOL& SM) {
	if (!p413App->Global.Confg.bSecsMod) return TRUE;
	if (!CanSend()) {
		return FALSE;
	}

	SM.cmd = CSMOL::cmdCARRIEROBJ;
	SM.CoS.act = CarrierObjStatus::CARRIEROBJUPDATE;
	return pGDev->pOLnMgr->pOLnSvrObj->ExecCarrierObjStatus(SM);
}

BOOL COLnClt::CJStop(short unit) {
	if (!CanSend()) {
		return FALSE;
	}

	CSMOL SM;
	SM.cmd = CSMOL::cmdCJPJACTION;
	SM.Cp.act = CjPjCmd::STOPPING;
	SM.Cp.unit = unit;
	return pGDev->pOLnMgr->pOLnSvrObj->ExecCjPjAction(SM);
}

BOOL COLnClt::CJAbort(short unit) {
	if (!CanSend()) {
		return FALSE;
	}

	CSMOL SM;
	SM.cmd = CSMOL::cmdCJPJACTION;
	SM.Cp.act = CjPjCmd::ABORTING;
	SM.Cp.unit = unit;
	return pGDev->pOLnMgr->pOLnSvrObj->ExecCjPjAction(SM);
}

void COLnClt::GEMEventAccessModeViolation(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::LP1ACCESSMODEVIOLATION);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::LP2ACCESSMODEVIOLATION);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void COLnClt::GEMEventFoupClampFail(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::CLAMP1FAILURE);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::CLAMP2FAILURE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void COLnClt::GEMEventCIDReadOK(short PortNo) {}

void COLnClt::GEMEventCIDReadFail(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::CARRIER1IDREADFAIL);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::CARRIER2IDREADFAIL);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void COLnClt::GEMEventCIDBypassed(short PortNo) {}

void COLnClt::GEMEventIDWaitingForHost(short PortNo) {}

void COLnClt::GEMEventFoupOpenFail(short PortNo) {}

void COLnClt::GEMEventSlotmapReadFail(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::SLOTMAP1READFAILURE);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::SLOTMAP2READFAILURE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void COLnClt::GEMEventSMWaitingForHost(short PortNo, CString CarrierID, CString SlotMap) {
	pGDev->pGEMObjs->SetCOSlotmapStatus(CarrierID, PortNo, SlotMap, CCarrierObj::SMWAITINGFORHOST);
}

void COLnClt::GEMEventFoupCloseFailed(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::LP1CLOSEFAILURE);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::LP2CLOSEFAILURE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void COLnClt::GEMEventFoupUnclampFail(short PortNo) {
	switch (PortNo) {
	case 1:
		pGDev->SetAlarm(CAlrmList::UNCLAMP1FAILURE);
		break;
	case 2:
		pGDev->SetAlarm(CAlrmList::UNCLAMP2FAILURE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

BOOL COLnClt::GEMEventFoupDocked(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::DOCKED;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Foup docked", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventFoupUndocked(short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::LOADPORTOBJECT;
	Ge.reason = CGEMEvt::UNDOCKED;
	Ge.PortID = PortNo;
	if (GEMEvent(Ge)) {
		strMsg.Format("LP%d: Foup undocked", PortNo);
		Log(strMsg, 5);
		return TRUE;
	}
	return FALSE;
}

BOOL COLnClt::GEMEventData(short which, CMPoint* mp, CMeParam* me) {
	CSharedMemQue pQue;
	CEvt* pEvt = new CEvt;
	switch (which) {
	case 1:
		pEvt->Set(EVT_CMS, CEID_LIST::RESULTSINGLE);
		pEvt->pMPoint = mp;
		pEvt->pMeParam = me;
		break;
	default:
		pEvt->Set(EVT_CMS, CEID_LIST::RESULTWAFER);
		pEvt->pMPoint = mp;
		pEvt->pMeParam = me;
		break;
	}
	pQue.OpenMemory("EVTMSG");
	pQue.WriteQue(pEvt);
	pQue.CloseMemory();
	return TRUE;
}

BOOL COLnClt::CanSend() {
	if (!pGDev || !pGDev->pOLnMgr || !pGDev->pOLnMgr->pOLnSvrObj) {
		return FALSE;
	}
	return TRUE;
}

BOOL COLnClt::GEMEventSlotMapAvailable(short PortNo, CString SlotMap) {
	CEvt* pEvt = new CEvt;
	switch (PortNo) {
	case 1:
		pEvt->Set(EVT_CMS, CEID_LIST::LP1SLOTMAPAVAILABLE);
		break;
	case 2:
		pEvt->Set(EVT_CMS, CEID_LIST::LP2SLOTMAPAVAILABLE);
		break;
	default:
		ASSERT(0);
		break;
	}
	pGDev->SetActivePort(PortNo);
	CSharedMemQue EvtQue;
	EvtQue.WriteQue(pEvt);
	pGDev->ClearActivePort();
	return TRUE;
}