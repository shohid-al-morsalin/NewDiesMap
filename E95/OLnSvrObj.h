#pragma once

class CGEMEvt;
struct CSMOL;

// This class is used by the message dispatcher only [8/30/2010 XPMUser]
class COLnSvrObj {
public:
	BOOL GemEvents(CGEMEvt& Ge);

	BOOL ExecCarrierAction(CSMOL& SM);
	BOOL ExecLdPStatus(CSMOL& SM);
	//	BOOL EquipmentStatus(CSMOL &SM);
	//	BOOL ExecAgvStatus( CSMOL &SM );
	BOOL ExecBcrStatus(CSMOL& SM);
	BOOL ProcessCmd(CSMOL& SM);
	//BOOL GemEvents(CSMOL &SM);
	BOOL ExecCjPjAction(CSMOL& SM);
	void Log(CString msg, short lvl = 5);
	void CleanUp();
	BOOL ExecEqpStatus(CGEMEvt& Ge);
	BOOL DoSlotmapVerification(CGEMEvt& Ge);
	BOOL ExecCarrierObjStatus(CSMOL& SM);
	BOOL OtherCmds(CSMOL& SM);
	BOOL ExecSlotmapVerification(CGEMEvt& Ge);
	BOOL DoAccessStatusChanged(CGEMEvt& Ge);
	BOOL SubstStateSMGhanged(CGEMEvt& Ge);
	BOOL SubstProcStateSMChanged(CGEMEvt& Ge);
	BOOL ExecMaterialMovement(CGEMEvt& Ge);
	BOOL DoCarrierObjSMChanged(CGEMEvt& Ge);
	BOOL DoSubstLocSMChanged(CGEMEvt& Ge);
	BOOL DoSlotmapStatusChanged(CGEMEvt& Ge);
	BOOL DoIDStatusChanged(CGEMEvt& Ge);
	BOOL ExecLPAssociationSMChanged(CGEMEvt& Ge);
	BOOL ExecLPOccupancySMChanged(CGEMEvt& Ge);
	BOOL ExecLPReservationSMChanged(CGEMEvt& Ge);
	BOOL ExecLPAccessModeSMChanged(CGEMEvt& Ge);
	BOOL ExecBindServices(CGEMEvt& Ge);
	BOOL ExecSelfBind(CGEMEvt& Ge);
	BOOL ExecCarrierObject(CGEMEvt& Ge);
	BOOL ExecLoadPortObject(CGEMEvt& Ge);
	BOOL ExecCarrierObjSubstLocChanged(CGEMEvt& Ge);
	BOOL ExecSubstObjSMChanged(CGEMEvt& Ge);
	BOOL ExecCancelBind(CGEMEvt& Ge);
	BOOL ExecCancelCarrier(CGEMEvt& Ge);
	BOOL ExecCancelNotification(CGEMEvt& Ge);
	BOOL ExecReservationAtPort(CGEMEvt& Ge);
	BOOL ExecCancelReservationAtPort(CGEMEvt& Ge);
	BOOL ExecTransferStatus(CGEMEvt& Ge);
	BOOL ExecStdAction(CSMOL& SM);
	BOOL CarrierID(CSMOL& SM);
	BOOL ExecScanData(CSMOL& SM);
	BOOL AlarmFunc(CSMOL& SM);
	BOOL CarrierBind(CSMOL& SM);
	BOOL DoProceedWithCarrierIDAck(CGEMEvt& Ge);
	BOOL DoProceedWithCarrierSMAck(CGEMEvt& Ge);

public:
	COLnSvrObj();
	virtual ~COLnSvrObj();
};