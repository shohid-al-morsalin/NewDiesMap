// PortParam.h: interface for the CPortParam class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CPortParam {
public:
	short PortNo;
	CString CarrierID;		// Carrier ID afer verification
	CString NewCarrierID;	// Carrier ID not yet verified

public:
	CPortParam();
	virtual ~CPortParam();
};
