#include "stdafx.h"
//#include "e95.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "GEMEvt.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMCHdr.h"
#include "IPC/Smol.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/GemFactory.h"
#include "ONLINE/GEM/PRJobMgr.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "SYS/Var.h"

#include "OLnSvrObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLnSvrObj::COLnSvrObj() {}

COLnSvrObj::~COLnSvrObj() {}

BOOL COLnSvrObj::ProcessCmd(CSMOL& SM) {
	if (!CHsmsParaMeter::IsOnline()) {
		switch (SM.cmd) {
		case CSMOL::cmdSTDACTION:
			ExecStdAction(SM);
			return TRUE;
			break;
		case CSMOL::cmdGEMEVENTS:
			ASSERT(0); // Should not come here [8/30/2010 XPMUser]
			//GemEvents(SM);
			return TRUE;
			break;
		}
		return TRUE;
	}
	switch (SM.cmd) {
	case CSMOL::cmdGEMEVENTS:
		ASSERT(0); // Should not come here [8/30/2010 XPMUser]
		//GemEvents(SM);
		break;

	case CSMOL::cmdSTDACTION:
		ExecStdAction(SM);
		break;

	case CSMOL::cmdLPSTATUS:
		ExecLdPStatus(SM);
		break;

	case CSMOL::cmdCARRIERACTION:
		ExecCarrierAction(SM);
		break;

	case CSMOL::cmdCARRIERIDINFO:
		CarrierID(SM);
		break;

	case CSMOL::cmdALARM:
		AlarmFunc(SM);
		break;

	case CSMOL::cmdSCANDATA:
		ExecScanData(SM);
		break;

	case CSMOL::cmdCARRIEROBJ:
		ExecCarrierObjStatus(SM);
		break;
	case CSMOL::cmdCJPJACTION:
		ExecCjPjAction(SM);
		break;
	}

	return TRUE;
}

BOOL COLnSvrObj::ExecLdPStatus(CSMOL& SM) {
	CLPStatus* p = &SM.Lp;

	if ((p->PortNo < 1) || (p->PortNo > 4)) {
		ASSERT(0);
		return TRUE;
	}

	if (p->Occupancy != CLPObj::igOCCUPENCY) {
		pGDev->pGEMObjs->Cms[p->PortNo - 1].LPObj.Occupancy = p->Occupancy;
	}
	if (p->TransferStatus != CLPObj::igTRANSFERSTATUS) {
		pGDev->pGEMObjs->Cms[p->PortNo - 1].LPObj.SetTransferStatus(p->TransferStatus, p->PortNo);
	}
	if (p->AccessMode != CLPObj::igACCESSMODE) {
		pGDev->pGEMObjs->Cms[p->PortNo - 1].LPObj.SetAccessMode(p->AccessMode, p->PortNo);
	}
	if (p->Association != CLPObj::igASSOCIATION) {
		pGDev->pGEMObjs->Cms[p->PortNo - 1].LPObj.Association = p->Association;
	}
	if (p->Reservation != CLPObj::igRESERVATION) {
		pGDev->pGEMObjs->Cms[p->PortNo - 1].LPObj.Reservation = p->Reservation;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCarrierAction(CSMOL& SM) {
	CSharedMemQue	EvtQue;
	CString			msg;
	CarrierAction* p = &SM.Ca;

	EvtQue.OpenMemory("EVTMSG");

	switch (p->act) {
	case CarrierAction::READYTOLOAD:
		ASSERT(0);
		msg = "Ready to Load";
		Log(msg, 5);
		break;
	case CarrierAction::OPENCOMPLETE:
		msg = "Open complete";
		Log(msg, 5);
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_FOUPOPEN, p->PortNo, p->CarrierID);
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;

	case CarrierAction::CLAMPCOMPLETE:
		msg = "Clamp complete";
		Log(msg, 5);
		break;
	case CarrierAction::TRANSFERBLOCKED:
		ASSERT(0);
		msg = "Transfer Blocked";
		Log(msg, 5);
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt = new CEvt;
			ASSERT(pEvt);
			pGDev->SetActivePort(p->PortNo);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_TRANSFERBLOACKED, p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;

	case CarrierAction::STARTPROCESSING:
		ASSERT(0);
		msg = "Start processing";
		Log(msg, 5);
		break;

	case CarrierAction::STOPPROCESSING:
		ASSERT(0);
		msg = "Stop processing";
		Log(msg, 5);
		break;

	case CarrierAction::PROCESSINGCOMPLETE:
		ASSERT(0);
		{
			msg = "Processing Completed";
			Log(msg, 5);
			CPRJob* q = PJobMgr.GetPRJob(p->PrJobID);
			if (CHsmsParaMeter::IsOnline()) {
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				if (q) {
					pEvt->PRJobState = q->GetState();
				}
				strscpy(pEvt->ObjID, OBJIDLEN, p->PrJobID);
				pEvt->Set(EVT_PJM_PRJEN, EVT_PJMCPROCESSCOMPLETE, p->PortNo, p->CarrierID);
				EvtQue.WriteQue(pEvt);
				delete pEvt;
			}
		}
		break;

	case CarrierAction::CLOSECOMPLETE:
		msg = "Carrier Close";
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_FOUPCLOSE, p->PortNo, p->CarrierID);
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;

	case CarrierAction::UNCLAMPCOMPLETE:
		msg = "XferBlocked to Ready to Unload(Unclamp Complete)";
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_UNDOCKINGCOMPLETED, p->PortNo, p->CarrierID);
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;

	case CarrierAction::READYTOUNLOAD:
		msg = "Ready to unload";
		Log(msg, 5);
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_UNLOADREQUEST, p->PortNo, p->CarrierID);
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;
	case CarrierAction::UNLOADCOMPLETE:
		msg = "Unload Completed";
		Log(msg, 5);
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_UNLOADCOMPLETE, p->PortNo, p->CarrierID);
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;
	case CarrierAction::CARRIERARRIVED:
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			switch (p->PortNo) {
			case 1:
				pEvt->Set(EVT_CMS, CEID_LIST::LP1CARRIERARRIVED, p->PortNo, p->CarrierID);
				break;
			case 2:
				pEvt->Set(EVT_CMS, CEID_LIST::LP2CARRIERARRIVED, p->PortNo, p->CarrierID);
				break;
			default:
				ASSERT(0);
				break;
			}
			pGDev->SetActivePort(p->PortNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		break;
	}

	EvtQue.CloseMemory();
	return TRUE;
}

BOOL COLnSvrObj::CarrierID(CSMOL& SM) {
	CString			msg;
	CarrierIDInfo* p = &SM.Ci;

	msg = p->CarrierID;
	Log(msg, 5);

	if (CHsmsParaMeter::IsOnline()) {
		CSharedMemQue	EvtQue;
		EvtQue.OpenMemory("EVTMSG");
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_CMS, EVT_CMSPHASE_LOADCOMPLETE, p->PortNo, p->CarrierID);
		pGDev->SetActivePort(p->PortNo);
		EvtQue.WriteQue(pEvt);
		pGDev->ClearActivePort();
		EvtQue.CloseMemory();
	}

	return TRUE;
}

BOOL COLnSvrObj::ExecStdAction(CSMOL& SM) {
	switch (SM.Sa.cmd) {
	case StdAction::ROBOTHOMED:
		Log("Robot Homed received", 2);
		break;
	case StdAction::READMSG:
		ASSERT(0); // to be continue
		break;
	case StdAction::GOONLINEREMOTE:
	{
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue m_gEvtQue;
			m_gEvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE);
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		return FALSE;
	}
	break;
	case StdAction::GOONLINELOCAL:
	{
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue m_gEvtQue;
			m_gEvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_LOCAL);
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		return FALSE;
	}
	break;
	case StdAction::GOOFFLINEEQP:
	{
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue m_gEvtQue;
			m_gEvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_OFFLINE);
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
	}
	break;
	}

	return TRUE;
}

BOOL COLnSvrObj::ExecScanData(CSMOL& SM) {
	sScanData* p = &SM.Sd;

	switch (p->act) {
	case sScanData::WRITE:
		if (CHsmsParaMeter::IsOnline()) {
			// VJTD 20May03
			if (m_pFactory) {
				m_pFactory->LastScan = SM.Sd;
			}
			//
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, EVT_CMSPHASE_WAFERFINISHEDMESURE, short(0));
			CopyMemory(&pEvt->scndta, p, sizeof(sScanData));
			pGDev->SetActivePortSubstrate(p->PortNo, p->SlotNo);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePortSubstrate();
			EvtQue.CloseMemory();
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL COLnSvrObj::AlarmFunc(CSMOL& SM) {
	if (CHsmsParaMeter::IsOnline()) {
		CSharedMemQue	EvtQue;
		EvtQue.OpenMemory("EVTMSG");
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		// YFL:  [2/19/2005]
		pEvt->Set(EVT_ALARM, 1, SM.Ar.alCD, SM.Ar.alID, SM.Ar.alTX);
		EvtQue.WriteQue(pEvt);
		EvtQue.CloseMemory();
		CString Temp;
		Temp.Format("Alarm trigger <%d> %s", SM.Ar.alID, SM.Ar.alTX);
		Log(Temp, 0);
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecBcrStatus(CSMOL& SM) {
	return TRUE;
}

BOOL COLnSvrObj::CarrierBind(CSMOL& SM) {
	ASSERT(0);
	switch (SM.CoS.act) {
	case CarrierObjStatus::BIND:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			switch (SM.CoS.portID) {
			case 1:
				pEvt->Set(EVT_EXTRA, 2001);// ***************************** [7/19/2010 Yuen]
				break;
			case 2:
				pEvt->Set(EVT_EXTRA, 2001);// ***************************** [7/19/2010 Yuen]
				break;
			default:
				ASSERT(0);
				break;
			}
			pGDev->SetActivePort(SM.CoS.portID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			EvtQue.CloseMemory();
		}
		break;
	default:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			switch (SM.CoS.portID) {
			case 1:
				pEvt->Set(EVT_EXTRA, 2002);// ***************************** [7/19/2010 Yuen]
				break;
			case 2:
				pEvt->Set(EVT_EXTRA, 2002);// ***************************** [7/19/2010 Yuen]
				break;
			default:
				ASSERT(0);
				break;
			}
			pGDev->SetActivePort(SM.CoS.portID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			EvtQue.CloseMemory();
		}
		break;
	}

	return TRUE;
}

BOOL COLnSvrObj::GemEvents(CGEMEvt& Ge) {
	// 	CGEMEvt Ge;  // make a backup to avoid Ge1 being freed by caller before this function is completed [12/19/2007 YUEN]
	// 	Ge = Ge1;
	// 	if ((Ge.PortID < 1) || (Ge.PortID > 4))
	// 	{
	// 		// Note: Some function may not need PortID, will make exception should it occurs
	// 		switch(Ge.act)
	// 		{
	// 		case CGEMEvt::EQUIPMENT:
	// 			break;
	// 		default:
	// 			ASSERT(0);
	// 			return FALSE;
	// 			break;
	// 		}
	// 	}

	CString str;
	switch (Ge.act) {
	case CGEMEvt::EQUIPMENT:
		str.Format("GemEvent: EQUIPMENT");
		Log(str, 5);
		ExecEqpStatus(Ge);
		break;
	case CGEMEvt::SLOTMAPVERIFICATION:
		str.Format("GemEvent: SLOTMAPVERIFICATION");
		Log(str, 5);
		ExecSlotmapVerification(Ge);
		break;
	case CGEMEvt::TRANSFERSTATUS:
		str.Format("GemEvent: TRANSFERSTATUS");
		Log(str, 5);
		ExecTransferStatus(Ge);//20120926 scott ref
		break;
	case CGEMEvt::CANCELBIND:
		str.Format("GemEvent: CANCELBIND");
		Log(str, 5);
		ExecCancelBind(Ge);
		break;
	case CGEMEvt::CANCELCARRIER:
	case CGEMEvt::CANCELCARRIERATPORT:
		str.Format("GemEvent: CANCELCARRIER/CANCELCARRIERATPORT");
		Log(str, 5);
		ExecCancelCarrier(Ge);
		break;
	case CGEMEvt::SOSMCHANGED:
		str.Format("GemEvent: SOSMCHANGED");
		Log(str, 5);
		ExecSubstObjSMChanged(Ge);
		break;
	case CGEMEvt::COSLCHANGED:
		str.Format("GemEvent: COSLCHANGED");
		Log(str, 5);
		ExecCarrierObjSubstLocChanged(Ge);
		break;
	case CGEMEvt::LOADPORTOBJECT:
		str.Format("GemEvent: LOADPORTOBJECT");
		Log(str, 5);
		ExecLoadPortObject(Ge);
		break;
	case CGEMEvt::CARRIEROBJECT:
		str.Format("GemEvent: CARRIEROBJECT");
		Log(str, 5);
		ExecCarrierObject(Ge);
		break;
	case CGEMEvt::BINDSERVICE:
		str.Format("GemEvent: BINDSERVICE");
		Log(str, 5);
		ExecBindServices(Ge);
		break;
	case CGEMEvt::ACCESSMODE:
		str.Format("GemEvent: ACCESSMODE");
		Log(str, 5);
		ExecLPAccessModeSMChanged(Ge);
		break;
	case CGEMEvt::RESERVATION:
		str.Format("GemEvent: RESERVATION");
		Log(str, 5);
		ExecLPReservationSMChanged(Ge);
		break;
	case CGEMEvt::OCCUPANCY:
		str.Format("GemEvent: OCCUPANCY");
		Log(str, 5);
		ExecLPOccupancySMChanged(Ge);
		break;
	case CGEMEvt::ASSOCIATION:
		str.Format("GemEvent: ASSOCIATION");
		Log(str, 5);
		ExecLPAssociationSMChanged(Ge);
		break;
	case CGEMEvt::IDSTATUSCHANGED:
		str.Format("GemEvent: IDSTATUSCHANGED");
		Log(str, 5);
		DoIDStatusChanged(Ge);
		break;
	case CGEMEvt::SLOTMAPSTATUSCHANGED:
		str.Format("GemEvent: SLOTMAPSTATUSCHANGED");
		Log(str, 5);
		DoSlotmapStatusChanged(Ge);
		break;
	case CGEMEvt::SLSMCHANGED:
		str.Format("GemEvent: SLSMCHANGED");
		Log(str, 5);
		DoSubstLocSMChanged(Ge);
		break;
	case CGEMEvt::COSMCHANGED:
		str.Format("GemEvent: COSMCHANGED");
		Log(str, 5);
		DoCarrierObjSMChanged(Ge);
		break;
	case CGEMEvt::MATERIALMOVEMENT:
		str.Format("GemEvent: MATERIALMOVEMENT");
		Log(str, 5);
		ExecMaterialMovement(Ge);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERID:
		str.Format("GemEvent: PROCEEDWITHCARRIERID");
		Log(str, 5);
		DoProceedWithCarrierIDAck(Ge);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERSM:
		str.Format("GemEvent: PROCEEDWITHCARRIERSM");
		Log(str, 5);
		DoProceedWithCarrierSMAck(Ge);
		break;
	case CGEMEvt::SELFBIND:
		str.Format("GemEvent: SELFBIND");
		Log(str, 5);
		ExecSelfBind(Ge);
		break;
	case CGEMEvt::ACCESSINGSTATUSCHANGED:
		str.Format("GemEvent: ACCESSINGSTATUSCHANGED");
		Log(str, 5);
		DoAccessStatusChanged(Ge);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecTransferStatus(CGEMEvt& Ge) {
	//ASSERT(Ge.PortID);
	if ((Ge.PortID < 1) || (Ge.PortID > 4)) {
		ASSERT(0);
		return FALSE;
	}
	switch (Ge.reason) {
	case CGEMEvt::READYTOLOAD:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetTransferState(Ge, CLPObj::READYTOLOAD);
		break;
	case CGEMEvt::READYTOUNLOAD:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetTransferState(Ge, CLPObj::READYTOUNLOAD);
		break;
	case CGEMEvt::TRANSFERBLOCKED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetTransferState(Ge, CLPObj::TRANSFERBLOCKED);
		break;
	case CGEMEvt::OUTOFSERVICE:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetTransferState(Ge, CLPObj::OUTOFSERVICE);
		break;
	case CGEMEvt::TRANSFERREADY:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetTransferState(Ge, CLPObj::TRANSFERREADY);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCancelReservationAtPort(CGEMEvt& Ge) {
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	memcpy(pEvt->ObjID, Ge.CarrierID, sizeof(Ge.CarrierID));
	memcpy(pEvt->SubstID, Ge.SubstID, sizeof(Ge.SubstID));
	memcpy(pEvt->LocID, Ge.LocID, sizeof(Ge.LocID));

	switch (Ge.reason) {
	case CGEMEvt::INVALIDPORTID:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::INVALIDPORTID);
			EvtQue.WriteQue(pEvt);
			EvtQue.CloseMemory();
			pEvt = NULL;	// YFL:  [4/8/2005]
		}
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::NOTREADYTOLOAD);
			EvtQue.WriteQue(pEvt);
			EvtQue.CloseMemory();
			pEvt = NULL;	// YFL:  [4/8/2005]
		}
		break;
	}
	// YFL:  [4/8/2005]
	if (pEvt) {
		delete pEvt;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecReservationAtPort(CGEMEvt& Ge) {
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	memcpy(pEvt->ObjID, Ge.CarrierID, sizeof(Ge.CarrierID));
	memcpy(pEvt->SubstID, Ge.SubstID, sizeof(Ge.SubstID));
	memcpy(pEvt->LocID, Ge.LocID, sizeof(Ge.LocID));

	switch (Ge.reason) {
	case CGEMEvt::INVALIDPORTID:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::INVALIDPORTID);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::NOTREADYTOLOAD);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::PORTISASSOCIATED:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::PORTISASSOCIATED);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	}
	if (pEvt) {
		delete pEvt;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCancelNotification(CGEMEvt& Ge) {
	CEvt* pEvt = new CEvt;
	ASSERT(pEvt);
	memcpy(pEvt->ObjID, Ge.CarrierID, sizeof(Ge.CarrierID));
	memcpy(pEvt->SubstID, Ge.SubstID, sizeof(Ge.SubstID));
	memcpy(pEvt->LocID, Ge.LocID, sizeof(Ge.LocID));

	switch (Ge.reason) {
	case CGEMEvt::CANCELNOTPOSSIBLE:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::CANCELNOTPOSSIBLE);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::CARRIEROBJNOTFOUND:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::CARRIERNOTFOUND);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::REMOVEFAIL:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::CARRIERREMOVEFAILED);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::REMOVEOK:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::CARRIERREMOVEOK);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::CARRIEROBJDELETED:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_EXTRA, CEID_LIST::CARRIEROBJECTDELETED);
			EvtQue.WriteQue(pEvt);
			pEvt = NULL;
			EvtQue.CloseMemory();
		}
		break;
	}
	if (pEvt) {
		delete pEvt;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCancelBind(CGEMEvt& Ge) {
	int ceid = 0;
	CString strError;
	int nError = 49;
	switch (Ge.reason) {
	case CGEMEvt::CANCELBINDOK:
		ceid = CEID_LIST::CANCELBINDOK;
		nError = 0;
		break;
	case CGEMEvt::INVALIDCARRIERID:
		ceid = CEID_LIST::INVALIDCARRIERID;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::INVALIDPORTID:
		ceid = CEID_LIST::INVALIDPORTID;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		ceid = CEID_LIST::NOTREADYTOLOAD;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::CANCELNOTPOSSIBLE:
		ceid = CEID_LIST::CANCELNOTPOSSIBLE;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::PORTISNOTASSOCIATED:
		ceid = CEID_LIST::PORTISNOTASSOCIATED;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::CARRIEROBJNOTFOUND:
		ceid = CEID_LIST::CARRIERNOTFOUND;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	case CGEMEvt::REMOVEFAIL:
	default:
		ceid = CEID_LIST::CARRIERREMOVEFAILED;
		nError = 49;
		strError = "CancelBind Failed";
		break;
	}
	CCms* p = NULL;
	if (Ge.PortID == 1 || Ge.PortID == 2) {
		p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
		if (p->m_hWait != NULL) {
			SetEvent(p->m_hWait);
		}
	}
	if (ceid) {
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, ceid);
			pEvt->PortNo = Ge.PortID;
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			if (ceid == CEID_LIST::CANCELBINDOK) {
				if (p) p->CaObj.Preset();
				CEvt* pEvt2 = new CEvt;
				ASSERT(pEvt2);
				*pEvt2 = *pEvt;
				switch (Ge.PortID) {
				case 1:
					pEvt->Set(EVT_CMS, CEID_LIST::CST1CARRIERTONOSTATE);  // todo [7/18/2010 Yuen]
					break;
				case 2:
					pEvt->Set(EVT_CMS, CEID_LIST::CST2CARRIERTONOSTATE);  // todo [7/18/2010 Yuen]
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->SetActivePort(Ge.PortID);
				EvtQue.WriteQue(pEvt2);
				pGDev->ClearActivePort();
			}
			EvtQue.CloseMemory();
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCancelCarrier(CGEMEvt& Ge) {
	int ceid = 0;
	CString strError;
	int nError = 0;
	switch (Ge.reason) {
	case CGEMEvt::CANCELCARRIEROK:
		switch (Ge.PortID) {
		case 1:
			ceid = CEID_LIST::LP1CANCELCARRIEROK;
			break;
		case 2:
			ceid = CEID_LIST::LP2CANCELCARRIEROK;
			break;
		default:
			ASSERT(0);
			break;
		}
		nError = 0;
		break;
	default:
		switch (Ge.PortID) {
		case 1:
			ceid = CEID_LIST::LP1CANCELCARRIERFAILED;
			break;
		case 2:
			ceid = CEID_LIST::LP2CANCELCARRIERFAILED;
			break;
		default:
			ASSERT(0);
			break;
		}
		nError = 49;
		strError = "CancelCarrier Fail";
		break;
	}
	CCms* pCms = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
	if (pCms->m_hWait != NULL) {
		SetEvent(pCms->m_hWait);
	}
	if (ceid) {
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, ceid);
			pEvt->PortNo = Ge.PortID;
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			EvtQue.CloseMemory();
			//CCms &CmRef = pGEMObjs->Cms[Ge.PortID - 1];
			if (ceid == CEID_LIST::LP1CANCELCARRIEROK || (ceid == CEID_LIST::LP2CANCELCARRIEROK)) {
				// Takes care if running process
				CJobMgr.CleanupCancelCarrier(pCms->CaObj.ObjID);
				// Carrier State model cleanup
				pCms->ProcessCancelCarrier();
			}
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecSubstObjSMChanged(CGEMEvt& Ge) {
	if (Ge.SuObj.SubstProcState != CSubstObj::igPROCSTATE) {
		SubstProcStateSMChanged(Ge);
	}
	if (Ge.SuObj.SubstState != CSubstObj::igSUBSTSTATE) {
		SubstStateSMGhanged(Ge);
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCarrierObjSubstLocChanged(CGEMEvt& Ge) {
	pGDev->pGEMObjs->Cms[Ge.PortID - 1].SubstLocChange(Ge);
	return TRUE;
}

BOOL COLnSvrObj::ExecLoadPortObject(CGEMEvt& Ge) {
	int nCEID = 0;
	int CEIDs[10];

	switch (Ge.reason) {
	case CGEMEvt::FOUPCLOSEDOK:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERCLOSED;  // CARRIER CLOSED
// 			CEIDs[nCEID++] = CEIDCtlr.GetCEID(CEID_LIST::LP1FOUPCLOSED);  // FOUP CLOSE OK
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERCLOSED;  // CARRIER CLOSED
// 			CEIDs[nCEID++] = CEIDCtlr.GetCEID(CEID_LIST::LP2FOUPCLOSED);  // FOUP CLOSE OK
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::FOUPOPENEDOK:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIEROPENED;	// CARRIER OPENED
// 			CEIDs[nCEID++] = 2301;  // FOUP OPEN OK
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIEROPENED;	// CARRIER OPENED
// 			CEIDs[nCEID++] = 2301;  // FOUP OPEN OK
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::DOCKED:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;	// CARRIER LOCATION CHANGED
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERDOCKED;	// CARRIER DOCKED
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;	// CARRIER LOCATION CHANGED
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERDOCKED;	// CARRIER DOCKED
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::UNDOCKED:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;	// CARRIER LOCATION CHANGED
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERUNDOCKED;	// CARRIER UNDOCKED
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;	// CARRIER LOCATION CHANGED
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERUNDOCKED;	// CARRIER UNDOCKED
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::CLAMPOK:
		// 		CEIDs[nCEID++] = 2417;	// CARRIERCLAMPED
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERCLAMPPED; // PORT#1 CLAMP COMPLETED
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERCLAMPPED; // PORT#2 CLAMP COMPLETED
			break;
		}
		break;
	case CGEMEvt::UNCLAMPOK:
		// 			CEIDs[nCEID++] = 2416; // CARRIERUNCLAMPED
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERUNCLAMPED;  // PORT#1 CARRIER UNCLAMPED
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERUNCLAMPED;  // PORT#2 CARRIER UNCLAMPED
			break;
		}
		break;
	}
	if (0 < nCEID) {
		if (CHsmsParaMeter::IsOnline()) {
			CCms* p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
			CSharedMemQue	EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			for (int i = 0; i < nCEID; i++) {
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->PortNo = Ge.PortID;
				pEvt->SetMsg((BYTE*)p->CaObj.ObjID, strlen(p->CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(p->PortID);
				EvtQue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EvtQue.CloseMemory();
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCarrierObject(CGEMEvt& Ge) {
	CCms* p = NULL;

	int nCEID = 0;
	int CEIDs[10];
	switch (Ge.reason) {
	case CGEMEvt::CARRIERCOMPLETE:
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			strscpy(pEvt->ObjID, OBJIDLEN, Ge.PRJobID);
			pEvt->PRJobState = Ge.PRJobState;
			pEvt->Set(EVT_PJM_PRJEN, EVT_PJMCPROCESSCOMPLETE, Ge.PortID, Ge.CarrierID);
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			EvtQue.CloseMemory();
		}
		break;
	case CGEMEvt::CARRIERSTOPPED:
		Log("PRJob Stopped", 5);
		if (pGDev->pDataBnk->bCJJobControl) {
			CJobMgr.Stopped(Ge.PRJobID);
		}
		else {
			CSMOL SM;
			SM.Ca.act = CarrierAction::PROCESSINGCOMPLETE;
			SM.cmd = CSMOL::cmdCARRIERACTION;
			CPRJob* pr = PJobMgr.GetPRJob(Ge.PRJobID);
			if (pr) {
				strscpy(SM.Ca.CarrierID, OBJIDLEN, pr->m_PRMtlName);
			}
			PJobMgr.Stopped(Ge.PRJobID);
			CSMClt ipc;
			if (ipc.Start1("SecsSvr", sizeof(CSMOL))) {
				if (ipc.Send(&SM)) {
					return TRUE;
				}
			}
		}
		break;
	case CGEMEvt::CARRIERABORTED:
		Log("PRJob Aborted", 5);
		if (pGDev->pDataBnk->bCJJobControl) {
			CJobMgr.Aborted(Ge.PRJobID);
		}
		else {
			CSMOL SM;
			SM.Ca.act = CarrierAction::PROCESSINGCOMPLETE;
			SM.cmd = CSMOL::cmdCARRIERACTION;
			CPRJob* pr = PJobMgr.GetPRJob(Ge.PRJobID);
			if (pr) {
				strscpy(SM.Ca.CarrierID, OBJIDLEN, pr->m_PRMtlName);
			}
			PJobMgr.Aborted(Ge.PRJobID);
			CSMClt ipc;
			if (ipc.Start1("SecsSvr", sizeof(CSMOL))) {
				if (ipc.Send(&SM)) {
					return TRUE;
				}
			}
		}
		break;
	case CGEMEvt::DOCKED:
		// 		CEIDs[nCEID++] = 2251;  // CARRIER DOCKED
		p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
		p->SetLocation(CLPObj::clDOCKED);
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERDOCKED; // CARRIER DOCKED
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;  // CARRIERLOCATIONCHANGE
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERDOCKED; // CARRIER DOCKED
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;  // CARRIERLOCATIONCHANGE
			break;
		}
		break;
	case CGEMEvt::UNDOCKED:
		// 		CEIDs[nCEID++] = 2252;  // CARRIER UNDOCKED
		// 		CEIDs[nCEID++] = 2419;
		p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
		p->SetLocation(CLPObj::clUNDOCKED);
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERUNDOCKED; // CARRIER UNDOCKED
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;  // CARRIERLOCATIONCHANGE
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERUNDOCKED; // CARRIER UNDOCKED
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;  // CARRIERLOCATIONCHANGE
			break;
		}
		//strcpy(pEvt->LocID, p->GetLocation(CLPObj::UNDOCKED));
		break;
	case CGEMEvt::CARRIEROBJNOTFOUND:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::CARRIERNOTFOUND;
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::CARRIERNOTFOUND;
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::CARRIEROBJDELETED:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::CARRIEROBJECTDELETED;
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::CARRIEROBJECTDELETED;
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CGEMEvt::CARRIERWAITFORCJTIMEOUT:
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::CARRIERWAITFORCJTIMEOUT;
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::CARRIERWAITFORCJTIMEOUT;
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	}
	if (0 < nCEID) {
		if (CHsmsParaMeter::IsOnline()) {
			p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			for (int i = 0; i < nCEID; i++) {
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->PortNo = Ge.PortID;
				pEvt->SetMsg((BYTE*)p->CaObj.ObjID, strlen(p->CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(Ge.PortID);
				EvtQue.WriteQue(pEvt);
				pGDev->ClearActivePort();
				Sleep(50);
			}
			EvtQue.CloseMemory();
		}
	}

	return TRUE;
}

BOOL COLnSvrObj::ExecBindServices(CGEMEvt& Ge) {
	int ceid = 0;
	CString strError;
	int nError = 0;

	switch (Ge.reason) {
	case CGEMEvt::BINDOK:
		ceid = CEID_LIST::BINDOK;
		nError = 0;
		break;
	case CGEMEvt::CREATEFAIL:
		// delete the carrier object
		ceid = CEID_LIST::BINDCREATEFAIL;
		nError = 48;	// unknown
		strError = "Failed to create";
		break;
	case CGEMEvt::ALLOCFAIL:
		ceid = CEID_LIST::BINDCREATEFAIL;
		nError = 49;	// unknown
		strError = "Failed to allocate";
		break;
	case CGEMEvt::DUPLICATEDCARRIER:
		ceid = CEID_LIST::DUPLICATEDCARRIER;
		nError = 11;
		strError = "Duplicate ID in use";
		break;
	case CGEMEvt::INVALIDCARRIERID:
		ceid = CEID_LIST::INVALIDCARRIERID;
		nError = 3;
		strError = "Invalid ID";
		break;
	case CGEMEvt::INVALIDPORTID:
		ceid = CEID_LIST::INVALIDPORTID;
		nError = 7;
		strError = "Invalid Port";
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		ceid = CEID_LIST::NOTREADYTOLOAD;
		nError = 49;	// unknown
		strError = "Failed to allocate";
		break;
	case CGEMEvt::PORTISASSOCIATED:
	default:
		ceid = CEID_LIST::PORTISASSOCIATED;
		nError = 49;	// unknown
		strError = "Failed to allocate";
		break;
	}
	if (pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait != NULL) {
		SetEvent(pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait);
	}
	if (ceid) {
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, ceid);
			pEvt->PortNo = Ge.PortID;

			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			EvtQue.CloseMemory();
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecSelfBind(CGEMEvt& Ge) {
	if (Ge.reason == CGEMEvt::CREATEOK) {
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].ClearAll();
		strscpy(pGDev->pGEMObjs->Cms[Ge.PortID - 1].CaObj.ObjID, OBJIDLEN, Ge.CarrierID);
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecLPAccessModeSMChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}
	switch (Ge.reason) {
	case CGEMEvt::AUTO:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetAccessMode(Ge, CLPObj::AUTO, TRUE);
		break;
	case CGEMEvt::MANUAL:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetAccessMode(Ge, CLPObj::MANUAL, TRUE);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecLPReservationSMChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}

	switch (Ge.reason) {
	case CGEMEvt::RESERVED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetReservation(Ge, CLPObj::RESERVED, TRUE);
		break;
	case CGEMEvt::NOTRESERVED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetReservation(Ge, CLPObj::NOTRESERVED, TRUE);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecLPOccupancySMChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}
	switch (Ge.reason) {
	case CGEMEvt::CARRIERNOTPRESENT:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetOccupancy(Ge, CLPObj::CARRIERNOTPRESENT);
		break;
	case CGEMEvt::CARRIERPRESENT:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetOccupancy(Ge, CLPObj::CARRIERPRESENT);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecLPAssociationSMChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}

	switch (Ge.reason) {
	case CGEMEvt::ASSOCIATED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetAssociation(Ge, CLPObj::ASSOCIATED);
		break;
	case CGEMEvt::NOTASSOCIATED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetAssociation(Ge, CLPObj::NOTASSOCIATED);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::DoIDStatusChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}

	switch (Ge.CaObj.IDStatus) {
	case CCarrierObj::IDNOTREAD:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetCarrierIDStaus(CCarrierObj::IDNOTREAD);
		break;
	case CCarrierObj::IDWAITINGFORHOST:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetCarrierIDStaus(CCarrierObj::IDWAITINGFORHOST);
		break;
	case CCarrierObj::IDVERIFICATIONOK:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetCarrierIDStaus(CCarrierObj::IDVERIFICATIONOK);
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetCarrierIDStaus(CCarrierObj::IDVERIFICATIONFAIL);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::DoSlotmapStatusChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}

	int i;
	CString str;
	switch (Ge.CaObj.SlotmapStatus) {
	case CCarrierObj::SMNOTREAD:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMNOTREAD);
		break;
	case CCarrierObj::SMWAITINGFORHOST:
		switch (Ge.PortID) {
		case 1:
			for (i = 0; i < SLOTMAPLEN - 1; i++) {
				pGDev->pDataBnk->LastSM1[i] = Ge.SlotMap[i]; // LYF remove +1 [8/14/2007 LYF]
			}
			pGDev->pDataBnk->LastSM1[i] = 0;
			break;
		case 2:
			for (i = 0; i < SLOTMAPLEN - 1; i++) {
				pGDev->pDataBnk->LastSM2[i] = Ge.SlotMap[i]; // LYF remove +1 [8/14/2007 LYF]
			}
			pGDev->pDataBnk->LastSM2[i] = 0;
			break;
		}
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMap(Ge.SlotMap);
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMWAITINGFORHOST);
		break;
	case CCarrierObj::SMVERIFICATIONOK:
	{
		switch (Ge.PortID) {
		case 1:
			for (i = 0; i < MAXCAPACITYLEN; i++) {
				pGDev->pDataBnk->LastSM1[i] = Ge.SlotMap[i]; // LYF remove +1 [8/14/2007 LYF]
			}
			pGDev->pDataBnk->LastSM1[i] = 0;
			break;
		case 2:
			for (i = 0; i < MAXCAPACITYLEN; i++) {
				pGDev->pDataBnk->LastSM2[i] = Ge.SlotMap[i]; // LYF remove +1 [8/14/2007 LYF]
			}
			pGDev->pDataBnk->LastSM2[i] = 0;
			break;
		}
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMVERIFICATIONOK);
		for (int nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
			if (CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED) == Ge.SlotMap[nIndex]) {
				pGDev->pGEMObjs->Cms[Ge.PortID - 1].CaObj.SlotMaP[nIndex] = CSubstObj::SM_CORRECTLY_OCCUPIED;
			}
		}
	}
	break;
	case CCarrierObj::SMVERIFICATIONFAIL:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMVERIFICATIONFAIL);
		break;
	case CCarrierObj::igSMSTATUS:
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::DoSubstLocSMChanged(CGEMEvt& Ge) {
	switch (Ge.PortID) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}
	if (Ge.Slot < 1 || Ge.Slot > 25) {
		ASSERT(0);
		return TRUE;
	}
	pGDev->pGEMObjs->Cms[Ge.PortID - 1].SubstLocChange(Ge);
	return TRUE;
}

BOOL COLnSvrObj::DoCarrierObjSMChanged(CGEMEvt& Ge) {
	DoIDStatusChanged(Ge);
	DoSlotmapStatusChanged(Ge);

	return TRUE;
}

BOOL COLnSvrObj::ExecMaterialMovement(CGEMEvt& Ge) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	CCms* p = &pGDev->pGEMObjs->Cms[Ge.PortID - 1];

	switch (Ge.reason) {
	case CGEMEvt::MATERIALRECEIVED:
		p->SetLocation(CLPObj::clUNDOCKED);
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERARRIVED;
			//CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOCARRIER;
			//CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOIDNOTREAD;
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;

			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERARRIVED;
			//CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOCARRIER;
			//CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOIDNOTREAD;
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;
			break;
		default:
			ASSERT(0);
			break;
		}
		//p->CaObj.Preset();
		break;
	case CGEMEvt::MATERIALREMOVED:
		p->SetLocation(CLPObj::clNOSTATE);
		switch (Ge.PortID) {
		case 1:
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERREMOVED;
			//CEIDs[nCEID++] = CEID_LIST::CST1CARRIERTONOSTATE;
			CEIDs[nCEID++] = CEID_LIST::LP1CARRIERLOCATIONCHANGED;
			break;
		case 2:
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERREMOVED;
			//CEIDs[nCEID++] = CEID_LIST::CST2CARRIERTONOSTATE;
			CEIDs[nCEID++] = CEID_LIST::LP2CARRIERLOCATIONCHANGED;
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	}
	if (0 < nCEID) {
		if (CHsmsParaMeter::IsOnline()) {
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			for (int i = 0; i < nCEID; i++) {
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->SetMsg((BYTE*)p->CaObj.ObjID, strlen(p->CaObj.ObjID));
				pEvt->PortNo = Ge.PortID;
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(Ge.PortID);
				EvtQue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EvtQue.CloseMemory();
		}
		// YFL: This should be perform irrespective of connection state [4/8/2005]
		CString CarrierID = "";
		for (int i = 0; i < nCEID; i++) {
			int idx = CEIDCtlr.m_CEList.GetIdx(CEIDs[i]);
			switch (idx) {
			case CEID_LIST::LP1CARRIERARRIVED:
				p->CaObj.Preset();
				pGDev->pGEMObjs->InstCarrierObject(CarrierID, 1); //  [12/24/2007 YUEN]
				break;
			case CEID_LIST::LP2CARRIERARRIVED:
				p->CaObj.Preset();
				pGDev->pGEMObjs->InstCarrierObject(CarrierID, 2); //  [12/24/2007 YUEN]
				break;
			}
		}
		// Set carrier SM [8/5/2010 XPMUser]
// 		switch (Ge.reason)
// 		{
// 		case CGEMEvt::MATERIALRECEIVED:
// 			if (p)
// 			{
// 				p->SetCarrierIDStaus(CCarrierObj::IDNOTREAD);
// 			}
// 			break;
// 		case CGEMEvt::MATERIALREMOVED:
// 			if (p)
// 			{
// 				p->SetCarrierIDStaus(CCarrierObj::IDNOTREAD);
// 			}
// 			break;
// 		default:
// 			ASSERT(0);
// 			break;
// 		}
	}
	return TRUE;
}

BOOL COLnSvrObj::SubstProcStateSMChanged(CGEMEvt& Ge) {
	if (Ge.Slot < 1 || Ge.Slot > 25) {
		ASSERT(0);
		return TRUE;
	}

	pGDev->pGEMObjs->Cms[Ge.PortID - 1].STPMChanged(Ge);
	return TRUE;
}

BOOL COLnSvrObj::SubstStateSMGhanged(CGEMEvt& Ge) {
	if (Ge.Slot < 1 || Ge.Slot > 25) {
		ASSERT(0);
		return TRUE;
	}

	pGDev->pGEMObjs->Cms[Ge.PortID - 1].STSMChanged(Ge);
	return TRUE;
}

BOOL COLnSvrObj::DoAccessStatusChanged(CGEMEvt& Ge) {
	pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetCarrierAccessingStatus(Ge.CaObj.AccessingStatus);
	return TRUE;
}

BOOL COLnSvrObj::ExecSlotmapVerification(CGEMEvt& Ge) {
	DoSlotmapVerification(Ge);
	return TRUE;
}

BOOL COLnSvrObj::OtherCmds(CSMOL& SM) {
	return TRUE;
}

#define Message(msg, x) (strError = msg)

BOOL COLnSvrObj::DoProceedWithCarrierIDAck(CGEMEvt& Ge) {
	int ceid = 0;
	CString strError;
	int nError = 49;
	switch (Ge.reason) {
	case CGEMEvt::PROCEEDWITHCARRIERIDOK:
		Message("Proceed with carrier OK", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERIDFAIL:
		Message("Proceed with carrier Fail", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERSMOK:
		Message("Proceed with carrier OK", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERSMFAIL:
		Message("Proceed with carrier Fail", 10);
		break;
	case CGEMEvt::INVALIDCARRIERID:
		Message("InvalidCarrier", 10);
		break;
	case CGEMEvt::INVALIDPORTID:
		Message("InvalidPortID", 10);
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		Message("NotReadToLoad", 10);
		break;
	case CGEMEvt::CANCELNOTPOSSIBLE:
		Message("CancelNotPossible", 10);
		break;
	case CGEMEvt::PORTISNOTASSOCIATED:
		Message("PortIsNotAssociated", 10);
		break;
	case CGEMEvt::CARRIEROBJNOTFOUND:
		Message("CarrierObjectNotFound", 10);
		break;
	case CGEMEvt::REMOVEFAIL:
		Message("RemoveCarrierObjectFail", 10);
		break;
	default:
		ASSERT(0);
		break;
	}
	if (pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait != NULL) {
		SetEvent(pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait);
	}
	if (ceid) {
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, ceid);
			pEvt->PortNo = Ge.PortID;
			pEvt->SetMsg((BYTE*)Ge.CarrierID, strlen(Ge.CarrierID));

			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			if (ceid == CEID_LIST::CANCELBINDOK) {
				CEvt* pEvt2 = new CEvt;
				ASSERT(pEvt2);
				*pEvt2 = *pEvt;
				switch (Ge.PortID) {
				case 1:
					pEvt2->Set(EVT_CMS, CEID_LIST::CST1CARRIERTONOSTATE);
					break;
				case 2:
					pEvt2->Set(EVT_CMS, CEID_LIST::CST2CARRIERTONOSTATE);
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->SetActivePort(Ge.PortID);
				EvtQue.WriteQue(pEvt2);
				pGDev->ClearActivePort();
			}
			EvtQue.CloseMemory();
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::DoProceedWithCarrierSMAck(CGEMEvt& Ge) {
	int ceid = 0;
	CString strError;
	int nError = 49;
	switch (Ge.reason) {
	case CGEMEvt::INVALIDCARRIERID:
		Message("InvalidCarrier", 10);
		break;
	case CGEMEvt::INVALIDPORTID:
		Message("InvalidPortID", 10);
		break;
	case CGEMEvt::NOTREADYTOLOAD:
		Message("NotReadToLoad", 10);
		break;
	case CGEMEvt::CANCELNOTPOSSIBLE:
		Message("CancelNotPossible", 10);
		break;
	case CGEMEvt::PORTISNOTASSOCIATED:
		Message("PortIsNotAssociated", 10);
		break;
	case CGEMEvt::CARRIEROBJNOTFOUND:
		Message("CarrierObjectNotFound", 10);
		break;
	case CGEMEvt::REMOVEFAIL:
		Message("RemoveCarrierObjectFail", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERIDOK:
		Message("Proceed with carrier OK", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERIDFAIL:
		Message("Proceed with carrier Fail", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERSMOK:
		Message("Proceed with carrier OK", 10);
		break;
	case CGEMEvt::PROCEEDWITHCARRIERSMFAIL:
		Message("Proceed with carrier Fail", 10);
		break;
	default:
		ASSERT(0);
		break;
	}
	if (pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait != NULL) {
		SetEvent(pGDev->pGEMObjs->Cms[Ge.PortID - 1].m_hWait);
	}
	if (ceid) {
		if (CHsmsParaMeter::IsOnline()) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, ceid);
			pEvt->PortNo = Ge.PortID;
			pEvt->SetMsg((BYTE*)Ge.CarrierID, strlen(Ge.CarrierID));
			CSharedMemQue EvtQue;
			EvtQue.OpenMemory("EVTMSG");
			pGDev->SetActivePort(Ge.PortID);
			EvtQue.WriteQue(pEvt);
			pGDev->ClearActivePort();
			if (ceid == CEID_LIST::CANCELBINDOK) {
				CEvt* pEvt2 = new CEvt;
				ASSERT(pEvt2);
				*pEvt2 = *pEvt;
				switch (Ge.PortID) {
				case 1:
					pEvt2->Set(EVT_CMS, CEID_LIST::CST1CARRIERTONOSTATE);
					break;
				case 2:
					pEvt2->Set(EVT_CMS, CEID_LIST::CST2CARRIERTONOSTATE);
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->SetActivePort(Ge.PortID);
				EvtQue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EvtQue.CloseMemory();
		}
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecCarrierObjStatus(CSMOL& SM) {
	CarrierObjStatus* p = &SM.CoS;

	switch (p->act) {
	case CarrierObjStatus::CARRIEROBJUPDATE:
		BYTE NumPorts = Var.nLoadPort/*pGDev->pDataBank->m_EStatus.MaxLP*/;
		if (p->portID < 1 || p->portID > NumPorts) {
			ASSERT(0);
			return TRUE;
		}

		// Only update selected fields
		CCarrierObj* c = &pGDev->pGEMObjs->Cms[p->portID - 1].CaObj;
		c->AccessingStatus = p->Co.AccessingStatus;
		c->Capacity = p->Co.Capacity;
		c->IDStatus = p->Co.IDStatus;
		c->SlotmapStatus = p->Co.SlotmapStatus;
		strscpy(c->ObjID, OBJIDLEN, p->Co.ObjID);
		strscpy(c->ObjType, OBJTYPELEN, p->Co.ObjType);
		return TRUE;
		break;
	}
	return TRUE;
}

// VJTD Response to this message
BOOL COLnSvrObj::DoSlotmapVerification(CGEMEvt& Ge) {
	switch (Ge.CaObj.SlotmapStatus) {
	case  CGEMEvt::SLOTMAPVERIFICATIONFAIL:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMVERIFICATIONFAIL);
		break;
	case  CGEMEvt::SLOTMAPVERIFIED:
		pGDev->pGEMObjs->Cms[Ge.PortID - 1].SetSlotMapStatus(CCarrierObj::SMVERIFICATIONOK);
		break;
	}
	return TRUE;
}

BOOL COLnSvrObj::ExecEqpStatus(CGEMEvt& Ge) {
	int CEIDs[10];
	int nCEID = 0;

	switch (Ge.reason) {
	case CGEMEvt::EQPPROCESSINGSTART:
		CEIDs[nCEID++] = CEID_LIST::PROCESSINGSTART;
		break;
	case CGEMEvt::EQPPROCESSINGCOMPLETE:
		CEIDs[nCEID++] = CEID_LIST::PROCESSINGCOMPLETE;
		break;
	case CGEMEvt::EQPPROCESSINGSTOP:
		CEIDs[nCEID++] = CEID_LIST::PROCESSINGSTOPPED;
		break;
	case CGEMEvt::EQPIDLE:
		CEIDs[nCEID++] = CEID_LIST::EQUIPMENTIDLE;
		break;
	case CGEMEvt::EQPWORKING:
		CEIDs[nCEID++] = CEID_LIST::EQUIPMENTWORKING;
		break;
	case CGEMEvt::EQPMAINTANANCE:
		CEIDs[nCEID++] = CEID_LIST::EQUIPMENTMAINTENANCE;
		break;
	}
	if (0 == nCEID) {
		return TRUE;
	}

	if (CHsmsParaMeter::IsOnline()) {
		CSharedMemQue EventQueue;
		EventQueue.OpenMemory("EVTMSG");
		for (int i = 0; i < nCEID; i++) {
			if (CEIDs[i] == 0) {
				ASSERT(0);
				continue;
			}
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_CMS, CEIDs[i]);
			EventQueue.WriteQue(pEvt);
		}
		EventQueue.CloseMemory();
	}
	return TRUE;
}

void COLnSvrObj::CleanUp() {}

void COLnSvrObj::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "OLSvrObj");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL COLnSvrObj::ExecCjPjAction(CSMOL& SM) {
	switch (SM.Cp.act) {
	case CjPjCmd::ABORTING:
		CJobMgr.Abort(SM.Cp.unit);
		return TRUE;
		break;
	case CjPjCmd::STOPPING:
		CJobMgr.Stop(SM.Cp.unit);
		return TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}