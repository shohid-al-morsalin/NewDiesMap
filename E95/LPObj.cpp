// LPObj.cpp: implementation of the CLPObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LPObj.h"
#include "..\SRC\DOSUtil.h"
#include "GlobalDeviceContainer.h" //20120926

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLPObj::CLPObj() {
	Preset();
}

CLPObj::~CLPObj() {}

void CLPObj::Preset() {
	Occupancy = CLPObj::ocNOSTATE;
	TransferStatus = CLPObj::tsNOSTATE;
	AccessMode = CLPObj::amNOSTATE;
	Reservation = CLPObj::rsNOSTATE;
	Association = CLPObj::asNOSTATE;
	CarrierLoc = CLPObj::clNOSTATE;
	ZeroMemory(ObjID, OBJIDLEN + 1);
	ZeroMemory(ObjType, OBJTYPELEN + 1);
	ZeroMemory(CarrierID, OBJIDLEN + 1);
}

void CLPObj::Ignor() {
	Occupancy = igOCCUPENCY;
	TransferStatus = igTRANSFERSTATUS;
	AccessMode = igACCESSMODE;
	Reservation = igRESERVATION;
	Association = igASSOCIATION;
}

short CLPObj::GGetOccupency() {
	switch (Occupancy) {
	default:
	case CLPObj::CARRIERNOTPRESENT:
		return 0;
		break;
	case CLPObj::CARRIERPRESENT:
		return 1;
		break;
	}
	return 0;  // default to not occupied [8/1/2010 XPMUser]
}

CString CLPObj::GetOccupencyStr() {
	switch (Occupancy) {
	default:
	case CLPObj::CARRIERNOTPRESENT:
		return "NOT OCCUPIED";
		break;
	case CLPObj::CARRIERPRESENT:
		return "OCCUPIED";
		break;
	}
	return "_";
}
//20120926 SCOTT EDIT -----------------------------------
short CLPObj::GGetAccessMode() {
	if (pGDev->SubModel == CGlobalDeviceContainer::UMC) {
		switch (AccessMode) {
		case CLPObj::MANUAL:
			return 0;
			break;
		case CLPObj::AUTO:
			return 1;
			break;
		}

		return -1;
	}
	else {
		switch (AccessMode) {
		case CLPObj::MANUAL:
			return 1;
			break;
		case CLPObj::AUTO:
			return 2;
			break;
		}
		return -1;
	}
}
//---------------------------------------------------
CString CLPObj::GetAccessModeStr() {
	switch (AccessMode) {
	case CLPObj::MANUAL:
		return "MANUAL";
		break;
	case CLPObj::AUTO:
		return "AUTO";
		break;
	}
	return "_";
}
//20120926 SCOTT EDIT -----------------------------------
short CLPObj::GGetTransferStatus() {
	if (pGDev->SubModel == CGlobalDeviceContainer::UMC) {
		switch (TransferStatus) {
		case CLPObj::READYTOLOAD:
			return 2;
			break;
		case CLPObj::TRANSFERBLOCKED:
			return 1;
			break;
		case CLPObj::READYTOUNLOAD:
			return 3;
			break;
		case CLPObj::OUTOFSERVICE:
			return 4;
			break;
		}
		return 0;
	}
	else {
		switch (TransferStatus) {
		case CLPObj::READYTOLOAD:
			return 1;
			break;
		case CLPObj::TRANSFERBLOCKED:
			return 2;
			break;
		case CLPObj::READYTOUNLOAD:
			return 3;
			break;
		case CLPObj::OUTOFSERVICE:
			return 4;
			break;
		}
		return 0;
	}
}
//-----------------------------------------------
CString CLPObj::GetTransferStatusStr() {
	switch (TransferStatus) {
	case CLPObj::READYTOLOAD:
		return "READY TO LOAD";
		break;
	case CLPObj::TRANSFERBLOCKED:
		return "TRANSFER BLOCKED";
		break;
	case CLPObj::READYTOUNLOAD:
		return "READY TO UNLOAD";
		break;
	case CLPObj::OUTOFSERVICE:
		return "OUT OF SERVICE";
		break;
	}
	return "UNKNOWN";
}

CString CLPObj::GetAssociationStr() {
	switch (Association) {
	case CLPObj::ASSOCIATED:
		return "ASSOCIATED";
		break;
	case CLPObj::NOTASSOCIATED:
		return "NOT ASSOCIATED";
		break;
	}
	return "_";
}

short CLPObj::GGetAssociation() {
	switch (Association) {
	case CLPObj::ASSOCIATED:
		return 1;
		break;
	case CLPObj::NOTASSOCIATED:
		return 0;
		break;
	}
	return -1;
}

short CLPObj::GGetReservation() {
	switch (Reservation) {
	case CLPObj::RESERVED:
		return 1;
		break;
	case CLPObj::NOTRESERVED:
		return 0;
		break;
	}
	return -1;
}

CString CLPObj::GetReservationStr() {
	switch (Reservation) {
	case CLPObj::RESERVED:
		return "RESERVED";
		break;
	case CLPObj::NOTRESERVED:
		return "NOT RESERVED";
		break;
	}
	return "_";
}

BOOL CLPObj::IsReadyToLoad() {
	return (TransferStatus == READYTOLOAD);
}

void CLPObj::SetAccessMode(ACCESSMODE mode) {
	AccessMode = mode;
}

void CLPObj::SetAccessMode(ACCESSMODE mode, short port) {
	AccessMode = mode;

	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);
	sect.Format("LPSvr%d", port);
	str.Format("%d", AccessMode);
	WritePrivateProfileString("AccessMode", sect, str, winini);
}

CLPObj::ACCESSMODE CLPObj::GetAccessMode() {
	return AccessMode;
}

CLPObj::ACCESSMODE CLPObj::GetAccessMode(short port) {
	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);

	sect.Format("LPSvr%d", port);
	AccessMode = (ACCESSMODE)GetPrivateProfileInt("AccessMode", sect, AccessMode, winini);

	return AccessMode;
}

CLPObj::TRANSFERSTATUS CLPObj::GetTransferStatus() {
	return TransferStatus;
}

void CLPObj::SetTransferStatus(TRANSFERSTATUS status) {
	TransferStatus = status;
}

CLPObj::TRANSFERSTATUS CLPObj::GetTransferStatus(short port) {
	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);

	sect.Format("LPSvr%d", port);
	TransferStatus = (TRANSFERSTATUS)GetPrivateProfileInt("TransferStatus", sect, TransferStatus, winini);

	return TransferStatus;
}

void CLPObj::SetTransferStatus(TRANSFERSTATUS status, short port) {
	TransferStatus = status;

	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);
	sect.Format("LPSvr%d", port);
	str.Format("%d", TransferStatus);
	WritePrivateProfileString("TransferStatus", sect, str, winini);
}