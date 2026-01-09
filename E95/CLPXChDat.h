#pragma once

class CLPXChDat {
public:
	int ID;
	BOOL bRdy, bPst, bErr;
	CString m_Association;
	CString m_Reservation;
	CString m_AccessMode;
	CString m_TransferStatus;
	CString m_IDStatus;
	CString m_SlotMapStatus;
	CString m_AccessingStatus;
	CString m_CarrierID;
	CString m_SubstCount;
public:
	CLPXChDat();
};
