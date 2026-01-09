// LPObj.h: interface for the CLPObj class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "..\IPC\OSSMemS.h"

class CLPObj {
public:
	enum OCCUPANCY {
		// Notes Never change the sequence [11/25/2002]
		CARRIERNOTPRESENT, CARRIERPRESENT, igOCCUPENCY, ocNOSTATE
	};

	static CString GetOccupancyStr(CLPObj::OCCUPANCY state) {
		switch (state) {
		case CARRIERNOTPRESENT:
			return "CARRIER NOT PRESENT";
		case CARRIERPRESENT:
			return "CARRIER PRESENT";
		}
		return "UINKNOWN";
	}

	enum TRANSFERSTATUS {
		// Notes Never change the sequence [11/25/2002]
		tsNOSTATE, READYTOLOAD, TRANSFERBLOCKED, READYTOUNLOAD, OUTOFSERVICE, INSERVICE, TRANSFERREADY, igTRANSFERSTATUS
	};

	static CString GetTransferStatusStr(CLPObj::TRANSFERSTATUS state) {
		switch (state) {
		case tsNOSTATE:
			return "NO STATE";
		case READYTOLOAD:
			return "READY TO LOAD";
		case TRANSFERBLOCKED:
			return "TRANSFER BLOCKED";
		case READYTOUNLOAD:
			return "READY TO UNLOAD";
		case OUTOFSERVICE:
			return "OUT OF SERVICE";
		case INSERVICE:
			return "IN SERVICE";
		case TRANSFERREADY:
			return "TRANSFER READY";
		}
		return "UNKNOWN";
	}

	enum RESERVATION {
		// Notes Never change the sequence [11/25/2002]
		NOTRESERVED, RESERVED, igRESERVATION, rsNOSTATE
	};

	static CString GetReservationStr(RESERVATION state) {
		switch (state) {
		case NOTRESERVED:
			return "NOT RESERVED";
			break;
		case RESERVED:
			return "RESERVED";
			break;
		}
		return "UNKNOWN";
	}

	enum ASSOCIATION {
		// Notes Never change the sequence [11/25/2002]
		NOTASSOCIATED, ASSOCIATED, igASSOCIATION, asNOSTATE
	};

	static CString GetAssociationStr(ASSOCIATION state) {
		switch (state) {
		case NOTASSOCIATED:
			return "NOT ASSOCIATE";
			break;
		case ASSOCIATED:
			return "ASSOCIATED";
			break;
		}
		return "UNKNOWN";
	}

	enum ACCESSMODE {
		// Notes Never change the sequence [11/25/2002]
		MANUAL, AUTO, igACCESSMODE, amNOSTATE
	};

	static CString GetAccessModeStr(ACCESSMODE state) {
		switch (state) {
		case MANUAL:
			return "MANUAL";
			break;
		case AUTO:
			return "AUTO";
			break;
		}
		return "UNKNOWN";
	}

	enum CARRIERLOC {
		// Notes Never change the sequence [11/25/2002]
		clUNDOCKED, clDOCKED, clNOSTATE, clIGNORE
	};

	OCCUPANCY Occupancy;
	RESERVATION Reservation;
	ASSOCIATION Association;
	CARRIERLOC CarrierLoc;
	char ObjID[OBJIDLEN + 1];
	char ObjType[OBJTYPELEN + 1];
	char CarrierID[OBJIDLEN + 1];

private:
	TRANSFERSTATUS TransferStatus;
	ACCESSMODE AccessMode;

public:
	void SetTransferStatus(TRANSFERSTATUS status, short port);
	TRANSFERSTATUS GetTransferStatus(short port);
	void SetTransferStatus(TRANSFERSTATUS status);
	TRANSFERSTATUS GetTransferStatus();
	void SetAccessMode(ACCESSMODE mode);
	ACCESSMODE GetAccessMode(short port);
	ACCESSMODE GetAccessMode();
	void SetAccessMode(ACCESSMODE mode, short port);
	CString GetReservationStr();
	short GGetReservation();
	CString GetTransferStatusStr();
	short GGetTransferStatus();
	CString GetOccupencyStr();
	short GGetOccupency();
	CString GetAssociationStr();
	short GGetAssociation();
	CString GetAccessModeStr();
	short GGetAccessMode();
	void Preset(void);
	void Ignor(void);
	BOOL IsReadyToLoad();

public:
	CLPObj();
	virtual ~CLPObj();
};
