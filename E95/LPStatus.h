// LPStatus.h: interface for the CLPStatus class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "LPObj.h"

class CLPStatus {
public:
	enum ACTION {
		READ, WRITE, NEW
	};

	/* Client perspetives */

	ACTION act;

	/* Action */

	int PortNo;
	int MaxLoadPort;

	// CLPObj member variable could be condense into the object [8/29/2010 XPMUser]

	/* CARRIERNOTPRESENT, CARRIERPRESENT */

	CLPObj::OCCUPANCY Occupancy;

	/* READYTOLOAD, TRANSFERBLOCKED, READYTOUNLOAD, OUTOFSERVICE */

	CLPObj::TRANSFERSTATUS TransferStatus;

	/* MANUAL, AUTO */

	CLPObj::ACCESSMODE AccessMode;

	/* NOTRESERVED, RESERVED */

	CLPObj::RESERVATION Reservation;

	/* NOTASSOCIATED, ASSOCIATED */

	CLPObj::ASSOCIATION Association;

public:

	void Preset(void) {
		Association = CLPObj::igASSOCIATION;
		Reservation = CLPObj::igRESERVATION;
		AccessMode = CLPObj::igACCESSMODE;
		TransferStatus = CLPObj::igTRANSFERSTATUS;
		Occupancy = CLPObj::igOCCUPENCY;
	}

	// 	CLPStatus();
	// 	virtual ~CLPStatus();
};