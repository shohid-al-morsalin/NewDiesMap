// EqpState.h: interface for the CEqpState class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
// #include "..\IPC\SMHdr.h"
#include "..\IPC\SMOL.h"

class CEqpState {
public:
	PrcsMgrServices::LPBSTATE GetLPState(short unit);
	PrcsMgrServices::LPBSTATE lpState[4];

	void SetLPState(short unit, PrcsMgrServices::LPBSTATE state);

	CEqpState();
	virtual ~CEqpState();
};