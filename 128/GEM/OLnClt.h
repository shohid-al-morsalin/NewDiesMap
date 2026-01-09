#pragma once

class CCarrierObj;
class CGEMEvt;
class CLPStatus;
class CMeParam;
class CMPoint;
class CSubstObj;
struct CSMOL;

/**
 * Take care of communication with OLnSvr.
 */
class COLnClt {
	CString strMsg;
	CCriticalSection CS;

	void Log(CString msg, short lvl = 5);

public:
	BOOL GEMEventSlotMapAvailable(short PortNo, CString SlotMap);
	BOOL CanSend();
	BOOL GEMEventData(short port, CMPoint* p, CMeParam* me);
	BOOL GEMEventFoupUndocked(short PortNo);
	BOOL GEMEventFoupDocked(short PortNo);
	void GEMEventFoupUnclampFail(short PortNo);
	void GEMEventFoupCloseFailed(short PortNo);
	void GEMEventSMWaitingForHost(short PortNo, CString CarrierID, CString Slotmap);
	void GEMEventSlotmapReadFail(short PortNo);
	void GEMEventFoupOpenFail(short PortNo);
	void GEMEventIDWaitingForHost(short PortNo);
	void GEMEventCIDBypassed(short PortNo);
	void GEMEventCIDReadFail(short PortNo);
	void GEMEventCIDReadOK(short PortNo);
	void GEMEventFoupClampFail(short PortNo);
	void GEMEventAccessModeViolation(short PortNo);
	BOOL CJAbort(short unit);
	BOOL CJStop(short unit);
	BOOL UpdateCarrierObject(CSMOL& SM);

	BOOL GEMEventMaterialRemoved(short PortNo);
	BOOL GEMEventCarrierObjNotFound(short PortNo);
	BOOL GEMEventCarrierWaitForCJTimeout(short PortNo);

	BOOL CSOStateEvent(CGEMEvt& Ge);
	BOOL COSLOccupancyEvent(CGEMEvt& Ge);
	BOOL CSOProcStateEvent(CGEMEvt& Ge/*short PortNo, CString &CarrierID, CString SubstID, short slot,  SubstObj::SUBSTPROCSTATE state*/);

	BOOL GEMEventSlotmapVerificationOK(short PortNo, CString CarrierID);
	BOOL GEMEventSlotmapVerificationFailed(short PortNo, CString CarrierID);
	BOOL GEMEventFoupOpened(short PortNo);
	BOOL GEMEventMaterialRecieved(short PortNo);
	BOOL GEMEventFoupClosed(short PortNo);
	BOOL GEMEventFoupClamped(short PortNo);
	BOOL GEMEventFoupUnclamped(short PortNo);
	void InitEqpUpdate();
	void InitLPUpdate(short PortNo, short Present);
	BOOL CAEvent(CSMOL& SM);
	BOOL GEMEvent(CGEMEvt& Ge);

	BOOL COPlace(short PortNo, short slot, CCarrierObj* c, CSubstObj* s);
	BOOL AlgPlace(short PortNo, CSubstObj* s);
	BOOL SclPlace(short PortNo, CSubstObj* s);
	BOOL ScnPlace(short PortNo, CSubstObj* s);

	CSubstObj* RemoveScn(short PortNo);
	CSubstObj* RemoveScl(short PortNo);
	CSubstObj* RemoveAlg(short PortNo);
	CSubstObj* RemoveCO(short PortNo, CCarrierObj* c, short slot);

	BOOL SetLPStatus(CLPStatus& Lp);
	BOOL GoRemote();
	BOOL GoLocal();
	BOOL GoOfflineEqp();
	BOOL GoOfflineHost();

	COLnClt();
	virtual ~COLnClt();
};
