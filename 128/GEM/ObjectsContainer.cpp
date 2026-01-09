// ObjectsContainer.cpp: implementation of the CObjectsContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"

#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "IFC/SecsSvrF.h"
#include "IPC/SMCHdr.h"
#include "OLnClt.h"
#include "SYS/Var.h"

#include "ObjectsContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectsContainer::CObjectsContainer() {
	pSecsSvr = new CSecsSvr;
	if (!pSecsSvr) {
		ASSERT(0);
	}
	Cms[0].Init(1);
	Cms[1].Init(2);
	Cms[2].Init(3);
	Cms[3].Init(4);
}

CObjectsContainer::~CObjectsContainer() {
	ClearCarrierObjList();

	if (pSecsSvr) {
		delete pSecsSvr;
		pSecsSvr = NULL;
	}
}

void CObjectsContainer::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "ObjCon");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CObjectsContainer::ClearCarrierObjList() {
	for (int i = 0; i < 4; i++) {
		Cms[i].ClearCarrierObj();
	}
}

CGEMEvt::REASON CObjectsContainer::InstCarrierObject(CString& CarrierID, int PortNo) {
	CString msg;

	if (FindCarrier(CarrierID)) {
		return CGEMEvt::DUPLICATEDCARRIER;
	}

	CSingleLock Lock(&CSCO);
	Lock.Lock();
	CCarrierObj* p = &Cms[PortNo - 1].CaObj;
	p->SetID(CarrierID);
	strcpy(p->ObjType, "Carrier");
	p->Capacity = Var.MaxSlot;
	p->IDStatus = CCarrierObj::IDNOSTATE;
	p->SlotmapStatus = CCarrierObj::SMNOSTATE;
	p->AccessingStatus = CCarrierObj::NOTACCESSED;
	CString port;
	port.Format("LP%d", PortNo);
	strcpy(p->LocationID, port);
	strcpy(p->Usage, "");

	p->SubstrateCount = 0;
	for (int i = 0; i < MAXCAPACITYLEN; i++) {
		p->CtMap[i].Preset();
		p->Substrate[i].Preset();
		p->SlotMaP[i] = CSubstObj::SM_EMPTY;
	}

	CSMOL SM;
	SM.CoS.portID = PortNo;
	SM.CoS.Co = *p;
	if (!pGDev->pOLnClt->UpdateCarrierObject(SM)) {
		return CGEMEvt::CREATEFAIL;
	}
	return CGEMEvt::CREATEOK;
}

CCarrierObj* CObjectsContainer::FindCarrier(char* CarrierID) {
	CString Carrier;
	Carrier = CarrierID;
	return FindCarrier(Carrier);
}

CCarrierObj* CObjectsContainer::FindCarrier(CString& CarrierID) {
	for (int i = 0; i < 4; i++) {
		if (strcmp(Cms[i].CaObj.ObjID, CarrierID) == 0) {
			return &Cms[i].CaObj;
		}
	}
	return NULL;
}

CGEMEvt::REASON CObjectsContainer::RemoveCarrierObject(CString& CarrierID) {
	for (int i = 0; i < 4; i++) {
		if (strcmp(Cms[i].CaObj.ObjID, CarrierID) == 0) {
			Cms[i].ClearCarrierObj();
			return CGEMEvt::REMOVEOK;
		}
	}
	return CGEMEvt::CARRIEROBJNOTFOUND;
}

CGEMEvt::REASON CObjectsContainer::COSMChange(CString& CarrierID,
	CCarrierObj::ACCESSINGSTATUS AccessStatus,
	CCarrierObj::IDSTATUS IDStatus,
	CCarrierObj::SLOTMAPSTATUS SlotmapStatus) {
	CString msg;
	CSingleLock Lock(&CSCOSM);
	Lock.Lock();
	CCarrierObj* p = FindCarrier(CarrierID);
	if (p) {
		if (AccessStatus != CCarrierObj::igACCESSINGSTATUS) {
			p->AccessingStatus = AccessStatus;
			msg.Format("%s access status changed", CarrierID);
			Log(msg, 5);
		}
		if (SlotmapStatus != CCarrierObj::igSMSTATUS) {
			p->SlotmapStatus = SlotmapStatus;
			msg.Format("%s slotmap status changed", CarrierID);
			Log(msg, 5);
		}
		if (IDStatus != CCarrierObj::igIDSTATUS) {
			p->IDStatus = IDStatus;
			msg.Format("%s ID status changed", CarrierID);
			Log(msg, 5);
		}

		CGEMEvt Ge;
		Ge.act = CGEMEvt::COSMCHANGED;
		strcpy(Ge.CarrierID, CarrierID);
		Ge.CaObj.AccessingStatus = AccessStatus;
		Ge.CaObj.IDStatus = IDStatus;
		Ge.CaObj.SlotmapStatus = SlotmapStatus;

		pGDev->pOLnClt->GEMEvent(Ge);

		return CGEMEvt::COSMCHANGEOK;
	}
	return CGEMEvt::COSMCHANGEFAIL;
}

CGEMEvt::REASON CObjectsContainer::LCSMChange(short port,
	CLPObj::TRANSFERSTATUS TransferStatus,
	CLPObj::ACCESSMODE AccessMode,
	CLPObj::ASSOCIATION Association,
	CLPObj::RESERVATION Reservation,
	CLPObj::OCCUPANCY Occupancy) {
	CString msg;
	CSingleLock Lock(&CSLCSM);
	Lock.Lock();
	if (port >= 1 && port <= 4) {
		CLPObj* p = GetLPObj(port);
		if (p) {
			if (TransferStatus != CLPObj::igTRANSFERSTATUS) {
				p->SetTransferStatus(TransferStatus, port);
				SetLPTransferStatus(port, p->GetTransferStatus());
				msg.Format("Load port %d transfer status changed", port);
				Log(msg, 5);
			}
			if (AccessMode != CLPObj::igACCESSMODE) {
				p->SetAccessMode(AccessMode);
				SetLPAccessMode(port, p->GetAccessMode());
				msg.Format("Load port %d access mode changed", port);
				Log(msg, 5);
			}
			if (Association != CLPObj::igASSOCIATION) {
				p->Association = Association;
				CCarrierObj* c;
				CString CarrierID = "";
				c = GetCarrierAtPort(port);
				if (c) {
					CarrierID = c->ObjID;
					SetLPAssociation(CarrierID, port, p->Association);
				}
				else {
					SetLPAssociation(CarrierID, port, p->Association);
				}
				msg.Format("Load port %d association status changed", port);
				Log(msg, 5);
			}
			if (Reservation != CLPObj::igRESERVATION) {
				p->Reservation = Reservation;
				SetLPReservation(port, p->Reservation);
				msg.Format("Load port %d reservation status changed", port);
				Log(msg, 5);
			}
			if (Occupancy != CLPObj::igOCCUPENCY) {
				p->Occupancy = Occupancy;
				SetLPOccupancy(port, p->Occupancy);
				msg.Format("Load port %d occupancy status changed", port);
				Log(msg, 5);
			}
		}
		else {
			return CGEMEvt::LOADPORTOBJNOTFOUND;
		}
	}
	else {
		ASSERT(0);
		return CGEMEvt::INVALIDPORTID;
	}
	return CGEMEvt::LCSMCHANGEOK;
}

CLPObj::TRANSFERSTATUS CObjectsContainer::GetLPTransferStatus(short port) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		return Lp.GetTransferStatus();
	}
	return CLPObj::igTRANSFERSTATUS;
}

CLPObj::OCCUPANCY CObjectsContainer::GetLPOccupancy(short port) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		return Lp.Occupancy;
	}
	return CLPObj::igOCCUPENCY;
}

CLPObj::ASSOCIATION CObjectsContainer::GetLPAssociation(short port) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		return Lp.Association;
	}
	return CLPObj::igASSOCIATION;
}

CLPObj::ASSOCIATION CObjectsContainer::GetLPAssociatedCarrier(short port, CString& CarrierID) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		CarrierID = Lp.CarrierID;
		return Lp.Association;
	}
	return CLPObj::igASSOCIATION;
}

CLPObj::RESERVATION CObjectsContainer::GetLPReservation(short port) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		return Lp.Reservation;
	}
	return CLPObj::igRESERVATION;
}

CLPObj::ACCESSMODE CObjectsContainer::GetLPAccessMode(short port) {
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		return Lp.GetAccessMode();
	}
	return CLPObj::igACCESSMODE;
}

CGEMEvt::REASON CObjectsContainer::RemoveCarrierObjectAtPort(short port) {
	CCarrierObj* p = GetCarrierAtPort(port);
	if (p) {
		CString carrierID = p->ObjID;
		CGEMEvt::REASON ret = RemoveCarrierObject(carrierID);
		if (ret == CGEMEvt::REMOVEOK) {
			SetLPAssociation(carrierID, port, CLPObj::NOTASSOCIATED);
			SetLPReservation(port, CLPObj::NOTRESERVED);
		}
		return ret;
	}
	return CGEMEvt::CARRIEROBJNOTFOUND;
}

BOOL CObjectsContainer::CancelAssociationAtPort(short port) {
	return TRUE;
}

BOOL CObjectsContainer::IsCarrierAssociatedWithPort(CString& CarrierID, short port) {
	CString str;
	CLPObj Lp;
	if (GetLpSM(port, Lp)) {
		str = Lp.CarrierID;
		if (str == CarrierID) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CObjectsContainer::CancelReserve(CString& CarrierID) {
	return TRUE;
}

CCarrierObj* CObjectsContainer::GetCarrierAtPort(short port) {
	if ((port < 1) || (port > 4)) {
		ASSERT(0);
		return NULL;
	}
	return &Cms[port - 1].CaObj;
}

BOOL CObjectsContainer::SetCOSlotmapStatus(CString& CarrierID, short PortID, CString SlotMap, CCarrierObj::SLOTMAPSTATUS status) {
	CString msg;
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		return FALSE;
	}

	if (p->SlotmapStatus == status) {
		return FALSE;
	}

	msg.Format("LP%d: Carrier %s SlotmapStatus set to ", PortID, CarrierID);
	switch (status) {
	case CCarrierObj::SMNOTREAD:
		msg += "SLOTMAPNOTREAD";
		Log(msg, 5);
		break;
	case CCarrierObj::SMWAITINGFORHOST:
		// need to supply slotmap info
		msg += "SMWAITINGFORHOST";
		Log(msg, 5);
		break;
	case CCarrierObj::SMVERIFICATIONOK:
		msg += "SLOTMAPVERIFICATIONOK";
		Log(msg, 5);
		break;
	case CCarrierObj::SMVERIFICATIONFAIL:
		msg += "SLOTMAPVERIFICATIONFAIL";
		Log(msg, 1);
		break;
	}

	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLOTMAPSTATUSCHANGED;
	Ge.CaObj.SlotmapStatus = status;
	Ge.PortID = PortID;
	strcpy(Ge.CarrierID, CarrierID);

	strcpy(Ge.SlotMap, SlotMap);
	pGDev->pOLnClt->GEMEvent(Ge);

	return TRUE;
}

CSubstObj* CObjectsContainer::RemoveCaSLObj(CCarrierObj* c, short slot, short PortNo) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SOSMCHANGED;
	Ge.PortID = PortNo;
	Ge.SuObj.SubstProcState = CSubstObj::IN_PROCESS;
	Ge.SuObj.SubstState = CSubstObj::ATWORK;
	Ge.SubstLocState = CSubstLocObj::UNOCCUPIED;
	Ge.Slot = slot;
	CString msg;
	msg.Format("%d:%d", PortNo, slot);
	strcpy(Ge.LocID, msg);
	memcpy(Ge.CarrierID, c->ObjID, sizeof(Ge.CarrierID));
	memcpy(Ge.SubstID, c->Substrate[slot - 1].ObjID, sizeof(Ge.SubstID));
	pGDev->pOLnClt->GEMEvent(Ge);

	// 	Ge.act=CGEMEvt::COSLCHANGED;
	// 	Ge.PortID = PortNo;
	// 	Ge.Slot=slot;
	// 	Ge.SuObj.SubstState = CSubstObj::ATSOURCE;
	//
	// 	msg.Format("%d:%d",PortNo,slot);
	// 	strcpy(Ge.LocID,msg);
	// 	strcpy(Ge.CarrierID,c->ObjID);
	// 	pGDev->pOLnClt->GEMEvent(Ge);

	return &c->Substrate[slot - 1];
}

BOOL CObjectsContainer::SetCaSLObj(CCarrierObj* c, short slot, short PortNo, CSubstObj* s) {
	CGEMEvt Ge;

	CString msg;
	Ge.act = CGEMEvt::SOSMCHANGED;
	Ge.PortID = PortNo;
	Ge.SuObj.SubstProcState = CSubstObj::PROCESSED;
	Ge.SuObj.SubstState = CSubstObj::ATDESTINATION;
	Ge.SubstLocState = CSubstLocObj::OCCUPIED;
	Ge.Slot = slot;
	strcpy(Ge.CarrierID, c->ObjID);
	strcpy(Ge.SubstID, s->ObjID);
	msg.Format("%d:%d", PortNo, slot);
	strcpy(Ge.LocID, msg);
	pGDev->pOLnClt->GEMEvent(Ge);

	msg.Format("LP%d: Substrate %s is AT DESTINATION and PROCESSED", PortNo, s->ObjID);
	Log(msg, 5);

	// 	Ge.act=CGEMEvt::COSLCHANGED;
	// 	Ge.PortID = PortNo;
	// 	Ge.Slot=s->SlotNo;
	// 	Ge.SuObj.SubstState = CSubstObj::ATDESTINATION;
	//
	// 	msg.Format("%d:%d",PortNo,s->SlotNo);
	// 	strcpy(Ge.LocID,msg);
	// 	strcpy(Ge.CarrierID,c->ObjID);
	// 	pGDev->pOLnClt->GEMEvent(Ge);

	return TRUE;
}

BOOL CObjectsContainer::SetScnSLObj(short PortNo, CSubstObj* s) {
	CSubstLocObj* Stage;
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
	case CGlobalDeviceContainer::FSM413MOT:
	case CGlobalDeviceContainer::FSM413SA:
		Stage = StMd.Get413SubstLocObj();
		break;
	default:
		return FALSE;
		break;
	}
	Stage->pSubstrate = s;

	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = Stage->pSubstrate->SlotNo;

	strcpy(Ge.LocID, "SCANNER");
	Ge.SubstLocState = CSubstLocObj::OCCUPIED;
	memcpy(Ge.SubstID, Stage->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);

	return TRUE;
}

CSubstObj* CObjectsContainer::RemoveScnSLObj(short PortNo) {
	CSubstLocObj* Stage;
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
	case CGlobalDeviceContainer::FSM413MOT:
	case CGlobalDeviceContainer::FSM413SA:
		Stage = StMd.Get413SubstLocObj();
		break;
	default:
		return FALSE;
		break;
	}
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = Stage->pSubstrate->SlotNo;

	strcpy(Ge.LocID, "SCANNER");
	Ge.SubstLocState = CSubstLocObj::UNOCCUPIED;
	memcpy(Ge.SubstID, Stage->pSubstrate->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);

	return Stage->pSubstrate;
}

BOOL CObjectsContainer::SetAlgSLObj(short PortNo, CSubstObj* s) {
	CSubstLocObj* Algn = StMd.GetAlgnSubstLocObj();
	Algn->SubstLocState = CSubstLocObj::OCCUPIED;
	Algn->pSubstrate = s;
	CGEMEvt Ge;
	// Location changed
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = s->SlotNo;
	strcpy(Ge.LocID, "ALIGNER");
	Ge.SubstLocState = Algn->SubstLocState;
	memcpy(Ge.LocID, Algn->ObjID, (OBJIDLEN < SUBSTLOCIDLEN) ? OBJIDLEN : SUBSTLOCIDLEN);
	memcpy(Ge.SubstID, s->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);
	return TRUE;
}

CSubstObj* CObjectsContainer::RemoveAlgSLObj(short PortNo) {
	CSubstLocObj* Algn = StMd.GetAlgnSubstLocObj();
	Algn->SubstLocState = CSubstLocObj::UNOCCUPIED;
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = Algn->pSubstrate->SlotNo;
	strcpy(Ge.LocID, "ALIGNER");
	Ge.SubstLocState = Algn->SubstLocState;
	memcpy(Ge.LocID, Algn->ObjID, (OBJIDLEN < SUBSTLOCIDLEN) ? OBJIDLEN : SUBSTLOCIDLEN);
	memcpy(Ge.SubstID, Algn->pSubstrate->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);

	return Algn->pSubstrate;
}

BOOL CObjectsContainer::SetSclSLObj(short PortNo, CSubstObj* s) {
	CSubstLocObj* Scale = StMd.GetScaleSubstLocObj();
	Scale->SubstLocState = CSubstLocObj::OCCUPIED;
	Scale->pSubstrate = s;
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = Scale->pSubstrate->SlotNo;
	strcpy(Ge.LocID, "SCALE");
	Ge.SubstLocState = Scale->SubstLocState;
	memcpy(Ge.LocID, Scale->ObjID, (OBJIDLEN < SUBSTLOCIDLEN) ? OBJIDLEN : SUBSTLOCIDLEN);
	memcpy(Ge.SubstID, s->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);

	return TRUE;
}

CSubstObj* CObjectsContainer::RemoveSclSLObj(short PortNo) {
	CSubstLocObj* Scale = StMd.GetScaleSubstLocObj();
	Scale->SubstLocState = CSubstLocObj::UNOCCUPIED;
	CGEMEvt Ge;
	Ge.act = CGEMEvt::SLSMCHANGED;
	Ge.PortID = PortNo;
	Ge.Slot = Scale->pSubstrate->SlotNo;
	strcpy(Ge.LocID, "SCALE");
	Ge.SubstLocState = Scale->SubstLocState;
	memcpy(Ge.LocID, Scale->ObjID, (OBJIDLEN < SUBSTLOCIDLEN) ? OBJIDLEN : SUBSTLOCIDLEN);
	memcpy(Ge.SubstID, Scale->pSubstrate->ObjID, OBJIDLEN);
	pGDev->pOLnClt->GEMEvent(Ge);

	return Scale->pSubstrate;
}

BOOL CObjectsContainer::CarrierLocChanged(CPortParam port, CGEMEvt::REASON reason) {
	CGEMEvt Ge;
	Ge.act = CGEMEvt::CARRIEROBJECT;
	Ge.reason = reason;
	Ge.PortID = port.PortNo;
	strcpy(Ge.CarrierID, port.CarrierID);
	pGDev->pOLnClt->GEMEvent(Ge);

	if ((port.PortNo >= 1) && (port.PortNo <= 4)) {
		switch (reason) {
		case CGEMEvt::DOCKED:
			GetLPObj(port.PortNo)->CarrierLoc = CLPObj::clDOCKED;
			break;
		case CGEMEvt::UNDOCKED:
			GetLPObj(port.PortNo)->CarrierLoc = CLPObj::clUNDOCKED;
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	return TRUE;
}

BOOL CObjectsContainer::BindInstantiateCarrierObject(CString& CarrierID, short PortID) {
	CGEMEvt Ge;
	CGEMEvt::REASON	ret;

	ret = InstCarrierObject(CarrierID, PortID);
	CString str;
	switch (ret) {
	case CGEMEvt::CREATEFAIL:
		Ge.act = CGEMEvt::BINDSERVICE;
		Ge.reason = ret;
		Ge.PortID = PortID;
		strcpy(Ge.CarrierID, CarrierID);
		pGDev->pOLnClt->GEMEvent(Ge);
		str.Format("LP%d: Fail to instantiate carrier %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::ALLOCFAIL:
		Ge.act = CGEMEvt::BINDSERVICE;
		Ge.reason = ret;
		Ge.PortID = PortID;
		strcpy(Ge.CarrierID, CarrierID);
		pGDev->pOLnClt->GEMEvent(Ge);
		str.Format("LP%d: Fail to alloc memory for carrier %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::DUPLICATEDCARRIER:
		Ge.act = CGEMEvt::BINDSERVICE;
		Ge.reason = ret;
		Ge.PortID = PortID;
		strcpy(Ge.CarrierID, CarrierID);
		pGDev->pOLnClt->GEMEvent(Ge);
		str.Format("LP%d: Duplicated carrier ID %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::CREATEOK:
		SetLPAssociation(CarrierID, PortID, CLPObj::ASSOCIATED);
		SetLPReservation(PortID, CLPObj::RESERVED);
		return TRUE;
	}

	return FALSE;
}

// This will instantiate a carrier object
BOOL CObjectsContainer::SelfBindCarrierObject(CString& CarrierID, short PortID) {
	CGEMEvt Ge;
	CGEMEvt::REASON	ret;

	ret = InstCarrierObject(CarrierID, PortID);
	CString str;
	switch (ret) {
	case CGEMEvt::CREATEFAIL:
		Ge.act = CGEMEvt::SELFBIND;
		Ge.reason = ret;
		Ge.PortID = PortID;
		strcpy(Ge.CarrierID, CarrierID);
		pGDev->pOLnClt->GEMEvent(Ge);
		str.Format("LP%d: Fail to instantiate carrier %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::ALLOCFAIL:
		Ge.act = CGEMEvt::SELFBIND;
		Ge.reason = ret;
		Ge.PortID = PortID;
		strcpy(Ge.CarrierID, CarrierID);
		pGDev->pOLnClt->GEMEvent(Ge);
		str.Format("LP%d: Fail to alloc memory for carrier %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::DUPLICATEDCARRIER:
		str.Format("LP%d: Duplicated carrier ID %s", PortID, CarrierID);
		Log(str, 1);
		return FALSE;
		break;
	case CGEMEvt::CREATEOK:
		return TRUE;
	}

	return FALSE;
}

BOOL CObjectsContainer::NotificationInstantiateCarrierObject(CString& CarrierID, short PortID) {
	return FALSE;
}

// Note: PortID is send solely for the benefit of ONLINE.EXE only, it is inconsistant logically. It will ]
// continue be send until ONLINE clean itself up.
BOOL CObjectsContainer::SetCOIDStatus(CString& CarrierID, short PortID, CCarrierObj::IDSTATUS status) {
	CString msg;
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		p = GetCarrierAtPort(PortID);
		if (!p) {
			return FALSE;
		}
	}

	msg.Format("LP%d: Carrier %s IDStatus set to ", PortID, CarrierID);
	switch (status) {
	case CCarrierObj::IDNOTREAD:
		msg += "IDNOTREAD";
		Log(msg, 5);
		break;
	case CCarrierObj::IDWAITINGFORHOST:
		msg += "IDWAITINGFORHOST";
		Log(msg, 5);
		break;
	case CCarrierObj::IDVERIFICATIONOK:
		msg += "IDVERIFICATIONOK";
		Log(msg, 5);
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:
		msg += "IDVERIFICATIONFAIL";
		Log(msg, 5);
		break;
	}

	CGEMEvt Ge;
	Ge.act = CGEMEvt::IDSTATUSCHANGED;
	Ge.CaObj.IDStatus = status;
	Ge.PortID = PortID;
	strcpy(Ge.CarrierID, CarrierID);
	pGDev->pOLnClt->GEMEvent(Ge);

	return TRUE;
}

BOOL CObjectsContainer::SetCOAccessingStatus(CString& CarrierID, CCarrierObj::ACCESSINGSTATUS status) {
	CString msg;
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		// Legal in the case of foup on load port during restart
		return TRUE;
	}
	switch (p->AccessingStatus) {
	case  CCarrierObj::NOTACCESSED:
		switch (status) {
		case CCarrierObj::INACCESS:
			break;
		default:
			return FALSE;
			break;
		}
		break;
	case CCarrierObj::INACCESS:
		switch (status) {
		case CCarrierObj::CARRIERCOMPLETE:
		case CCarrierObj::CARRIERSTOPPED:
			break;
		default:
			return FALSE;
			break;
		}
		break;
	case CCarrierObj::CARRIERCOMPLETE:
		return FALSE;
		break;
	case CCarrierObj::CARRIERSTOPPED:
		return FALSE;
		break;
	case CCarrierObj::ASNOSTATE:
		// allowed [12/24/2007 YUEN]
		break;
	default:
		ASSERT(0); // here [12/24/2007 YUEN]
		return FALSE;
		break;
	}

	msg.Format("Carrier %s AccessingStatus set to ", CarrierID);
	switch (status) {
	case CCarrierObj::NOTACCESSED:
		msg += "NOTACCESSED";
		Log(msg, 5);
		break;
	case CCarrierObj::INACCESS:
		msg += "INACCESS";
		Log(msg, 5);
		break;
	case CCarrierObj::CARRIERCOMPLETE:
		msg += "CARRIERCOMPLETE";
		Log(msg, 5);
		break;
	case CCarrierObj::CARRIERSTOPPED:
		msg += "CARRIERSTOPPED";
		Log(msg, 5);
		break;
	}

	CGEMEvt Ge;
	Ge.act = CGEMEvt::ACCESSINGSTATUSCHANGED;
	Ge.CaObj.AccessingStatus = status;
	Ge.PortID = FindPortWithCarrier(CarrierID);
	strcpy(Ge.CarrierID, CarrierID);
	pGDev->pOLnClt->GEMEvent(Ge);
	return TRUE;
}

short CObjectsContainer::GetSubstrateCount(short port) {
	CCarrierObj* c;
	c = GetCarrierAtPort(port);
	if (!c) {
		return 0;
	}
	c->SubstrateCount = 0;
	for (int i = 0; i < c->Capacity; i++) {
		if ((c->SlotMaP[i] == CSubstObj::SM_EMPTY) || (c->SlotMaP[i] == CSubstObj::SM_UNDEFINED)) {
		}
		else {
			c->SubstrateCount++;
		}
	}
	return c->SubstrateCount;
}

BOOL CObjectsContainer::SelfCancelBind(CCarrierObj* c) {
	if (!c) {
		return FALSE;
	}

	CString carrierID;
	carrierID = c->ObjID;
	if (GetLPReservation(GetPortOfCarrier(carrierID)) != CLPObj::RESERVED) {
		return FALSE;
	}

	return RemoveCarrierObject(carrierID);
}

BOOL CObjectsContainer::CancelCarrierObj(CString& CarrierID) {
	return RemoveCarrierObject(CarrierID);
}

CCarrierObj::IDSTATUS CObjectsContainer::GetCOIDStatus(CString& CarrierID) {
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		return CCarrierObj::igIDSTATUS;
	}
	return p->IDStatus;
}

CCarrierObj::ACCESSINGSTATUS CObjectsContainer::GetCOAccessStatus(CString& CarrierID) {
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		return CCarrierObj::igACCESSINGSTATUS;
	}
	return p->AccessingStatus;
}

CCarrierObj::SLOTMAPSTATUS CObjectsContainer::GetCOSlotmapStatus(CString& CarrierID) {
	CCarrierObj* p = FindCarrier(CarrierID);
	if (!p) {
		return CCarrierObj::igSMSTATUS;
	}
	return p->SlotmapStatus;
}

short CObjectsContainer::FindPortWithCarrier(CString& Carrier) {
	for (int i = 0; i < 4; i++) {
		if (strcmp(Cms[i].CaObj.ObjID, Carrier.GetBuffer(0)) == 0) {
			return i + 1;
		}
	}
	return 0;
}

CString CObjectsContainer::GetCOIDStatusStr(CCarrierObj::IDSTATUS id) {
	switch (id) {
	case CCarrierObj::IDNOTREAD:
		return "IDNotRead";
		break;
	case CCarrierObj::IDWAITINGFORHOST:
		return "IDWait4Host";
		break;
	case CCarrierObj::IDVERIFICATIONOK:
		return "IDVerOK";
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:
		return "IDVerFailed";
		break;
	}
	return "";
}

CString CObjectsContainer::GetCOSlotmapStatusStr(CCarrierObj::SLOTMAPSTATUS sm) {
	switch (sm) {
	case CCarrierObj::SMNOTREAD:
		return "SMNotRead";
		break;
	case CCarrierObj::SMWAITINGFORHOST:
		return "SMWait4Host";
		break;
	case CCarrierObj::SMVERIFICATIONOK:
		return "SMVerOK";
		break;
	case CCarrierObj::SMVERIFICATIONFAIL:
		return "SMVerFailed";
		break;
	}
	return "";
}

CString CObjectsContainer::GetCOAccessStatusStr(CCarrierObj::ACCESSINGSTATUS accs) {
	switch (accs) {
	case CCarrierObj::NOTACCESSED:
		return "NotAccessed";
		break;
	case CCarrierObj::INACCESS:
		return "InAccess";
		break;
	case CCarrierObj::CARRIERCOMPLETE:
		return "Completed";
		break;
	case CCarrierObj::CARRIERSTOPPED:
		return "Stopped";
		break;
	}
	return "";
}

EqpStatus::CONTROLSTATE CObjectsContainer::GetLineMode() {
	return StMd.GetEqpStatus()->LineMode;
}

// No need to inform OnLine because it is originated from OnLine Only
void CObjectsContainer::SetLineMode(EqpStatus::CONTROLSTATE LineMode) {
	StMd.GetEqpStatus()->LineMode = LineMode;
}

//  [9/14/2007 LYF]
int CObjectsContainer::GGetLineMode() {
	// UMC: SEMI E5 compliance values
	if (pGDev->SubModel == CGlobalDeviceContainer::UMC) {
		switch (StMd.GetEqpStatus()->LineMode) {
		case EqpStatus::REMOTE:
			return 5;
			break;
		case EqpStatus::LOCAL:
			return 4;
			break;
		case EqpStatus::OFFLINEEQP:
			return 1;
			break;
		case EqpStatus::OFFLINEHOST:
			return 3;
			break;
		case EqpStatus::ATTEMPTONLINE:
			return 2;
			break;
		}
	}
	else {
		// Historical values
		switch (StMd.GetEqpStatus()->LineMode) {
		case EqpStatus::REMOTE:
			return 1;
			break;
		case EqpStatus::LOCAL:
			return 2;
			break;
		case EqpStatus::OFFLINEEQP:
			return 3;
			break;
		case EqpStatus::OFFLINEHOST:
			return 4;
			break;
		case EqpStatus::ATTEMPTONLINE:
			return 5;
			break;
		}
	}
	return 0;
}

CString CObjectsContainer::GetLineModeStr() {
	switch (StMd.GetEqpStatus()->LineMode) {
	case EqpStatus::OFFLINEEQP:
		return "OFFLINE(EQP)";
		break;
	case EqpStatus::OFFLINEHOST:
		return "OFFLINE(HOST).";
		break;
	case EqpStatus::LOCAL:
		return "LOCAL";
		break;
	case EqpStatus::REMOTE:
		return "REMOTE";
		break;
	default:
		ASSERT(0);
		break;
	}
	return "?";
}

CIDRObj* CObjectsContainer::GetBCRObj(short PortID) {
	return &Cms[PortID - 1].BCR;
}

short CObjectsContainer::GetPortOfCarrier(CString& CarrierID) {
	return GetAssociatedPort(CarrierID);
}

CLPObj* CObjectsContainer::GetLPObj(short PortID) {
	if (PortID < 1 || PortID > 4) {
		ASSERT(0);
		return NULL;
	}
	return &Cms[PortID - 1].LPObj;
}

// Note: Does not send event message to host Yuen[2002/08/04 14:29]
BOOL CObjectsContainer::UpdateLPAssociatedCarrierID(CString& CarrierID, short PortID) {
	CLPObj* pLp = GetLPObj(PortID);
	if (!pLp) {
		ASSERT(0);
		return FALSE;
	}
	strcpy(pLp->CarrierID, CarrierID);
	return TRUE;
}

void CObjectsContainer::ListLPObjs() {
	CString str;
	for (int i = 0; i < Var.nLoadPort; i++) {
		CLPObj* p = GetLPObj(i + 1);
		if (p) {
			str.Format("LP#%d", i + 1);
			Log(str, 10);
			str.Format("Object ID: %s", p->ObjID);
			Log(str, 10);
			str.Format("Associated carrier ID: %s", p->CarrierID);
			Log(str, 10);
			str.Format("Access mode: %s", p->GetAccessModeStr());
			Log(str, 10);
			str.Format("Association: %s", p->GetAssociationStr());
			Log(str, 10);
			str.Format("Occupancy: %s", p->GetOccupencyStr());
			Log(str, 10);
			str.Format("Reservation: %s", p->GetReservationStr());
			Log(str, 10);
			str.Format("Transfer Status: %s", p->GetTransferStatusStr());
			Log(str, 10);
		}
	}
}

void CObjectsContainer::ListCaObjs() {
	CString str;
	for (int i = 0; i<int(Var.nLoadPort); i++) {
		CCarrierObj* p = &Cms[i].CaObj;
		str.Format("Carrier: %d", i + 1);
		Log(str, 10);
		str.Format("ObjID: %s", p->ObjID);
		Log(str, 10);
		str.Format("ObjType: %s", p->ObjType);
		Log(str, 10);
		str.Format("IDStatus: %s", p->GetIDStatusStr());
		Log(str, 10);
		str.Format("SMStatus: %s", p->GetSlotmapStatusStr());
		Log(str, 10);
		str.Format("AccessStatus: %s", p->GetAccessingStatusStr());
		Log(str, 10);
		Log("Content Map", 10);
		int j;
		for (j = 0; j < MAXCAPACITYLEN; j++) {
			str.Format("Slot %d: LotID: %s SubstID: %s", j + 1, p->CtMap[j].LotID, p->CtMap[j].SubstID);
			Log(str, 10);
		}
		str = "";
		for (j = 0; j < MAXCAPACITYLEN; j++) {
			switch (p->SlotMaP[j]) {
			case CSubstObj::SM_UNDEFINED:
				str += CSubstObj::GetSMChar(CSubstObj::SM_UNDEFINED);
				break;
			case CSubstObj::SM_EMPTY:
				str += CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
				break;
			case CSubstObj::SM_CORRECTLY_OCCUPIED:
				str += CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
				break;
			case CSubstObj::SM_CROSS_SLOTTED:
				str += CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
				break;
			case CSubstObj::SM_DOUBLE_SLOTTED:
				str += CSubstObj::GetSMChar(CSubstObj::SM_DOUBLE_SLOTTED);
				break;
			}
		}
		Log(str, 10);
	}
}

BOOL CObjectsContainer::SendLPAccessMode(short PortID, CLPObj::ACCESSMODE state, BOOL bTransition) {
	CGEMEvt Ge;
	CString msg;

	Ge.act = CGEMEvt::ACCESSMODE;
	Ge.PortID = PortID;
	Ge.bTransition = bTransition;

	msg.Format("Set load port %d to ", PortID);
	switch (state) {
	case CLPObj::AUTO:
		Ge.reason = CGEMEvt::AUTO;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "AUTO";
		Log(msg, 5);
		return TRUE;
		break;
	case CLPObj::MANUAL:
		Ge.reason = CGEMEvt::MANUAL;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "MANUAL";
		Log(msg, 5);
		return TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

// Set transition change
BOOL CObjectsContainer::SetLPAccessMode(short PortID, CLPObj::ACCESSMODE state) {
	// Send state change to host first then set the appropriate state models
	SendLPAccessMode(PortID, state);

	CLPObj Lp;
	Lp.Ignor();
	Lp.SetAccessMode(state);
	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.Pm.unit = PortID;
	SM.Pm.port = PortID;
	SM.Pm.nunit = 1;
	switch (state) {
	case CLPObj::AUTO:
		SM.Pm.act = PrcsMgrServices::ACCESSMODEAUTO;
		break;
	default:
		SM.Pm.act = PrcsMgrServices::ACCESSMODEMANUAL;
		break;
	}
	if (pGDev->pLPMgr) {
		CSMClt ipc;
		if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
			if (!ipc.Send(&SM)) {
				ASSERT(0);
			}
		}
	}
	if (SetLpSM(PortID, Lp)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::SendLPTransferStatus(short PortID, CLPObj::TRANSFERSTATUS state, BOOL bTransition) {
	CGEMEvt Ge;
	CString msg;

	Ge.act = CGEMEvt::TRANSFERSTATUS;
	Ge.PortID = PortID;
	Ge.bTransition = bTransition;

	msg.Format("Set load port %d to ", PortID);
	switch (state) {
	case  CLPObj::READYTOLOAD:
		Ge.reason = CGEMEvt::READYTOLOAD;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "READYTOLOAD";
		Log(msg, 5);
		Cms[PortID - 1].CaObj.Preset();  // 02-01-11
		return TRUE;
		break;
	case CLPObj::TRANSFERBLOCKED:
		Ge.reason = CGEMEvt::TRANSFERBLOCKED;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "TRANSFERBLOCKED";
		Log(msg, 5);
		return TRUE;
		break;
	case CLPObj::READYTOUNLOAD:
		Ge.reason = CGEMEvt::READYTOUNLOAD;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "READYTOUNLOAD";
		Log(msg, 5);
		return TRUE;
		break;
	case CLPObj::OUTOFSERVICE:
		Ge.reason = CGEMEvt::OUTOFSERVICE;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "OUTOFSERVICE";
		Log(msg, 0);
		return TRUE;
		break;
	case CLPObj::TRANSFERREADY:
		Ge.reason = CGEMEvt::TRANSFERREADY;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "TRANSFERREADY";
		Log(msg, 5);
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CObjectsContainer::SetLPTransferStatus(short PortID, CLPObj::TRANSFERSTATUS state) {
	// Send state change to host first then set the appropriate state models
	SendLPTransferStatus(PortID, state);

	CLPObj Lp;
	Lp.Ignor();
	Lp.SetTransferStatus(state);
	if (SetLpSM(PortID, Lp)) {
		// Watch out for recursive calls YFL [3/17/2003]
		if (state == CLPObj::OUTOFSERVICE) {
			SetLPAccessMode(PortID, CLPObj::MANUAL);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::SendLPAssociation(CString& CarrierID, short PortID, CLPObj::ASSOCIATION state, BOOL bTransition) {
	CGEMEvt Ge;
	CString msg;

	Ge.act = CGEMEvt::ASSOCIATION;
	Ge.PortID = PortID;
	Ge.bTransition = bTransition;
	strcpy(Ge.CarrierID, CarrierID);

	msg.Format("Port %d set to ", PortID);
	switch (state) {
	case CLPObj::ASSOCIATED:
		Ge.reason = CGEMEvt::ASSOCIATED;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "ASSOCIATED";
		Log(msg, 5);
		return TRUE;
		break;
	case CLPObj::asNOSTATE:
	case CLPObj::NOTASSOCIATED:
		Ge.reason = CGEMEvt::NOTASSOCIATED;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "NOTASSOCIATED";
		Log(msg, 5);
		return TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

BOOL CObjectsContainer::SetLPAssociation(CString& CarrierID, short PortID, CLPObj::ASSOCIATION state) {
	// Send state change to host first then set the appropriate state models
	SendLPAssociation(CarrierID, PortID, state);

	CLPObj Lp;

	Lp.Ignor();
	ASSERT(CarrierID.GetLength() < OBJIDLEN);
	strscpy(Lp.CarrierID, OBJIDLEN, CarrierID);

	Lp.Association = state;
	if (SetLpSM(PortID, Lp)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::SendLPOccupancy(short PortID, CLPObj::OCCUPANCY state, BOOL bTransition) {
	CGEMEvt Ge;
	CString msg;

	Ge.act = CGEMEvt::OCCUPANCY;
	Ge.PortID = PortID;
	Ge.bTransition = bTransition;

	msg.Format("Port %d set to ", PortID);
	switch (state) {
	case CLPObj::ocNOSTATE:
	case CLPObj::CARRIERNOTPRESENT:
		Ge.reason = CGEMEvt::CARRIERNOTPRESENT;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "CARRIERNOTPRESENT";
		Log(msg, 5);
		return TRUE;
		break;
	case CLPObj::CARRIERPRESENT:
		Ge.reason = CGEMEvt::CARRIERPRESENT;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "CARRIERPRESENT";
		Log(msg, 5);
		return TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

BOOL CObjectsContainer::SetLPOccupancy(short PortID, CLPObj::OCCUPANCY state) {
	// Send state change to host first then set the appropriate state models
	SendLPOccupancy(PortID, state);

	CLPObj Lp;

	Lp.Ignor();
	Lp.Occupancy = state;
	if (SetLpSM(PortID, Lp)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::SendLPReservation(short PortID, CLPObj::RESERVATION state, BOOL bTransition) {
	CGEMEvt Ge;
	CString msg;
	Ge.act = CGEMEvt::RESERVATION;
	Ge.PortID = PortID;
	Ge.bTransition = bTransition;

	msg.Format("Set load port %d to ", PortID);
	switch (state) {
	case CLPObj::RESERVED:
		Ge.reason = CGEMEvt::RESERVED;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "RESERVED";
		Log(msg, 2);
		return TRUE;
		break;
	case CLPObj::rsNOSTATE:
	case CLPObj::NOTRESERVED:
		Ge.reason = CGEMEvt::NOTRESERVED;
		pGDev->pOLnClt->GEMEvent(Ge);

		msg += "NOTRESERVED";
		Log(msg, 2);
		return TRUE;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

BOOL CObjectsContainer::SetLPReservation(short PortID, CLPObj::RESERVATION state) {
	// Send state change to host first then set the appropriate state models
	CLPObj Lp;
	SendLPReservation(PortID, state);

	Lp.Ignor();
	Lp.Reservation = state;
	if (SetLpSM(PortID, Lp)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::IsLPAccessModeAuto(short unit) {
	if ((unit >= 1) && (unit <= 5)) {
		CLPObj* p = GetLPObj(unit);
		return p->GetAccessMode() == CLPObj::AUTO;
	}
	ASSERT(0);
	return FALSE;
}

CIDRObj* CObjectsContainer::GetIDRObj(short unit) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return NULL;
	}
	return &Cms[unit - 1].BCR;
}

CIDRObj::BYPASSSTATUS CObjectsContainer::GetIDRBypass(short unit) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return CIDRObj::BYPASSED;
	}
	return Cms[unit - 1].BCR.GetBypassStatus();
}

void CObjectsContainer::SetIDRBypass(short unit, CIDRObj::BYPASSSTATUS status) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return;
	}
	Cms[unit - 1].BCR.SetBypassStatus(status, unit);
}

void CObjectsContainer::SetIDRService(short unit, CIDRObj::SERVICESTATUS status) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return;
	}
	Cms[unit - 1].BCR.SetServiceStatus(status, unit);
}

BOOL CObjectsContainer::GetLpSM(short Port, CLPObj& Lp) {
	int p = Port - 1;
	if (p >= 0 && p <= 3) {
		Lp.Occupancy = Cms[p].LPObj.Occupancy;
		Lp.SetTransferStatus(Cms[p].LPObj.GetTransferStatus());
		Lp.SetAccessMode(Cms[p].LPObj.GetAccessMode());
		Lp.Reservation = Cms[p].LPObj.Reservation;
		Lp.Association = Cms[p].LPObj.Association;
		strcpy(Lp.CarrierID, Cms[p].LPObj.CarrierID);
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}

BOOL CObjectsContainer::SetLpSM(short Port, CLPObj& Lp) {
	CGEMEvt Ge;
	CSMClt ipc;
	int p = Port - 1;

	if (p >= 0 && p <= 4) {
		if (Lp.Occupancy != CLPObj::igOCCUPENCY) {
			if (Cms[p].LPObj.Occupancy == Lp.Occupancy) return FALSE;
			Cms[p].LPObj.Occupancy = Lp.Occupancy;
		}
		if (Lp.GetTransferStatus() != CLPObj::igTRANSFERSTATUS) {
			if (Cms[p].LPObj.GetTransferStatus() == Lp.GetTransferStatus()) return FALSE;
			Cms[p].LPObj.SetTransferStatus(Lp.GetTransferStatus(), Port);
		}
		if (Lp.GetAccessMode() != CLPObj::igACCESSMODE) {
			if (Cms[p].LPObj.GetAccessMode() == Lp.GetAccessMode()) return FALSE;
			Cms[p].LPObj.SetAccessMode(Lp.GetAccessMode(), Port);
		}
		if (Lp.Reservation != CLPObj::igRESERVATION) {
			if (Cms[p].LPObj.Reservation == Lp.Reservation) return FALSE;
			Cms[p].LPObj.Reservation = Lp.Reservation;
		}
		if (Lp.Association != CLPObj::igASSOCIATION) {
			Cms[p].LPObj.Association = Lp.Association;
			if (Lp.Association == CLPObj::ASSOCIATED) {
				strcpy(Cms[p].LPObj.CarrierID, Lp.CarrierID);
			}
			else if (Lp.Association == CLPObj::NOTASSOCIATED) {
				strcpy(Cms[p].LPObj.CarrierID, "");
			}
		}
	}
	return TRUE;
}

BOOL CObjectsContainer::GetCarrierAtPort(short Port, CString& carrierID) {
	int p = Port - 1;
	if (p >= 0 && p <= 3) {
		carrierID = Cms[p].LPObj.CarrierID;
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::Associate(CString& CarrierID, short Port) {
	int p = Port - 1;
	if (p >= 0 && p <= 3) {
		strcpy(Cms[p].LPObj.CarrierID, CarrierID);
		return TRUE;
	}
	return FALSE;
}

BOOL CObjectsContainer::DeAssociate(short Port) {
	int p = Port - 1;
	if (p >= 0 && p <= 3) {
		strcpy(Cms[p].LPObj.CarrierID, "");
		return TRUE;
	}
	return FALSE;
}

short CObjectsContainer::GetAssociatedPort(CString& CarrierID) {
	for (int p = 0; p <= 3; p++) {
		CString str;
		str = Cms[p].CaObj.ObjID;
		if (CarrierID == str) {
			return p + 1;
		}
	}
	return 0;
}

BOOL CObjectsContainer::RegisterWithSecsSvr(CWnd* pWnd, short ID) {
	if (pSecsSvr && (ID >= 0) && (ID <= 4)) {
		pSecsSvr->pActWin[ID] = pWnd;
		return TRUE;
	}
	// Should not come here [2/12/2012 Administrator]
	ASSERT(0);
	return FALSE;
}

BOOL CObjectsContainer::IsOnline() {
	return StMd.GetEqpStatus()->IsOnline();
}