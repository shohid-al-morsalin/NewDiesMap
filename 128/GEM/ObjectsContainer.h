// ObjectsContainer.h: interface for the CObjectsContainer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <afxmt.h>

#include "GEMEvt.h"
#include "ONLINE/GEM/Cms.h"
#include "ONLINE/ShareMemory/Cidrw.h"
#include "PortParam.h"
#include "StateModels.h"

class CCms;
class COLnClt;
class CSecsSvr;
class CPortParam;

class CObjectsContainer {
	CCriticalSection CSLCSM, CSCOSM, CSCO;

public:
	CStateModels StMd;		// State Models
	CCidrw CidrSystem;
	CSecsSvr* pSecsSvr;
	CCms Cms[4];
public:
	BOOL BindInstantiateCarrierObject(CString& CarrierID, short PortID);

	void Log(CString msg, short lvl = 5);
	CGEMEvt::REASON RemoveCarrierObject(CString& CarrierID);
	CGEMEvt::REASON InstCarrierObject(CString& CarrierID, int PortNo);
	CGEMEvt::REASON COSMChange(
		CString& CarrierID,
		CCarrierObj::ACCESSINGSTATUS AccessStatus,
		CCarrierObj::IDSTATUS IDStatus,
		CCarrierObj::SLOTMAPSTATUS SlotmapStatus);
	CGEMEvt::REASON LCSMChange(
		short port,
		CLPObj::TRANSFERSTATUS TransferStatus,
		CLPObj::ACCESSMODE AccessMode,
		CLPObj::ASSOCIATION Association,
		CLPObj::RESERVATION Reservation,
		CLPObj::OCCUPANCY Occupancy);

	BOOL SetLPReservation(short PortID, CLPObj::RESERVATION state);
	BOOL SetLPAssociation(CString& CarrierID, short port, CLPObj::ASSOCIATION state);
	BOOL SetLPAccessMode(short port, CLPObj::ACCESSMODE state);
	BOOL SetLPOccupancy(short port, CLPObj::OCCUPANCY state);
	BOOL SetLPTransferStatus(short port, CLPObj::TRANSFERSTATUS state);

public:
	CSubstObj* RemoveScnSLObj(short PortNo);
	CSubstObj* RemoveSclSLObj(short PortNo);
	CSubstObj* RemoveAlgSLObj(short PortNo);
	CSubstObj* RemoveCaSLObj(CCarrierObj* c, short slot, short PortNo);
	BOOL SetScnSLObj(short PortNo, CSubstObj* s);
	BOOL SetSclSLObj(short PortNo, CSubstObj* s);
	BOOL SetAlgSLObj(short PortNo, CSubstObj* s);
	BOOL SetCaSLObj(CCarrierObj* c, short slot, short PortNo, CSubstObj* s);

	void ClearCarrierObjList();
	CCarrierObj* FindCarrier(char* CarrierID);
	CCarrierObj* FindCarrier(CString& CarrierID);

	CObjectsContainer();
	virtual ~CObjectsContainer();

public:
	BOOL IsOnline();
	BOOL RegisterWithSecsSvr(CWnd* pWnd, short ID);
	short GetAssociatedPort(CString& CarrierID);
	BOOL DeAssociate(short Port);
	BOOL Associate(CString& CarrierID, short Port);
	BOOL GetCarrierAtPort(short Port, CString& carrierID);
	BOOL SetLpSM(short Port, CLPObj& Lp);
	BOOL GetLpSM(short Port, CLPObj& Lp);
	void SetIDRService(short unit, CIDRObj::SERVICESTATUS status);
	void SetIDRBypass(short unit, CIDRObj::BYPASSSTATUS status);
	CIDRObj::BYPASSSTATUS GetIDRBypass(short unit);
	CIDRObj* GetIDRObj(short unit);
	BOOL IsLPAccessModeAuto(short unit);
	BOOL SendLPReservation(short PortID, CLPObj::RESERVATION state, BOOL bTransition = TRUE);
	BOOL SendLPOccupancy(short PortID, CLPObj::OCCUPANCY state, BOOL bTransition = TRUE);
	BOOL SendLPAssociation(CString& CarrierID, short PortID, CLPObj::ASSOCIATION state, BOOL bTransition = TRUE);
	BOOL SendLPTransferStatus(short PortID, CLPObj::TRANSFERSTATUS state, BOOL bTransition = TRUE);
	BOOL SendLPAccessMode(short port, CLPObj::ACCESSMODE state, BOOL bTransition = TRUE);
	void ListCaObjs();
	void ListLPObjs();
	BOOL UpdateLPAssociatedCarrierID(CString& CarrierID, short PortID);
	CLPObj::OCCUPANCY GetLPOccupancy(short port);
	CLPObj* GetLPObj(short PortID);
	short GetPortOfCarrier(CString& CarrierID);
	CIDRObj* GetBCRObj(short PortID);
	CString GetLineModeStr();
	void SetLineMode(EqpStatus::CONTROLSTATE LineMode);
	EqpStatus::CONTROLSTATE GetLineMode();
	int GGetLineMode();
	CString GetCOAccessStatusStr(CCarrierObj::ACCESSINGSTATUS accs);
	CString GetCOSlotmapStatusStr(CCarrierObj::SLOTMAPSTATUS sm);
	CString GetCOIDStatusStr(CCarrierObj::IDSTATUS id);
	short FindPortWithCarrier(CString& Carrier);
	BOOL CancelCarrierObj(CString& CarrierID);
	BOOL SelfCancelBind(CCarrierObj* c);
	short GetSubstrateCount(short port);
	BOOL NotificationInstantiateCarrierObject(CString& CarrierID, short PortID);
	BOOL SelfBindCarrierObject(CString& carrierID, short port);
	BOOL CarrierLocChanged(CPortParam port, CGEMEvt::REASON reason);
	CCarrierObj* GetCarrierAtPort(short port);
	BOOL CancelReserve(CString& CarrierID);
	BOOL IsCarrierAssociatedWithPort(CString& CarrierID, short port);
	BOOL CancelAssociationAtPort(short port);
	CGEMEvt::REASON RemoveCarrierObjectAtPort(short port);
	CLPObj::ACCESSMODE GetLPAccessMode(short port);
	CLPObj::RESERVATION GetLPReservation(short port);
	CLPObj::ASSOCIATION GetLPAssociation(short port);
	CLPObj::ASSOCIATION GetLPAssociatedCarrier(short port, CString& CarrierID);
	CLPObj::TRANSFERSTATUS GetLPTransferStatus(short port);

	CCarrierObj::IDSTATUS GetCOIDStatus(CString& CarrierID);
	CCarrierObj::SLOTMAPSTATUS GetCOSlotmapStatus(CString& CarrierID);
	CCarrierObj::ACCESSINGSTATUS GetCOAccessStatus(CString& CarrierID);
	BOOL SetCOIDStatus(CString& CarrierID, short PortID, CCarrierObj::IDSTATUS status);
	BOOL SetCOAccessingStatus(CString& CarrierID, CCarrierObj::ACCESSINGSTATUS status);
	BOOL SetCOSlotmapStatus(CString& CarrierID, short PortID, CString Slotmap, CCarrierObj::SLOTMAPSTATUS status);
};
