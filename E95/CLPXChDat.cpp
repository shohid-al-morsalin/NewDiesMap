#include "stdafx.h"
#include "CLPXChDat.h"

// This structure is used by load port front end to query data from Load port and Online
CLPXChDat::CLPXChDat() {
	bRdy = FALSE;
	bPst = FALSE;
	bErr = FALSE;
	m_Association = "";
	m_Reservation = "";
	m_AccessMode = "";
	m_TransferStatus = "";
	m_IDStatus = "";
	m_SlotMapStatus = "";
	m_AccessingStatus = "";
	m_CarrierID = "";
	m_SubstCount = "";
	ID = -1;
}