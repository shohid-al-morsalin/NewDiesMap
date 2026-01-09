// EqpState.cpp: implementation of the CEqpState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "EqpState.h"
#include "..\LTW\LTWMgr.h"
#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEqpState::CEqpState() {
	for (int i = 0; i < 4; i++) {
		lpState[i] = PrcsMgrServices::NOFOUP;
	}
}

CEqpState::~CEqpState() {}

void CEqpState::SetLPState(short unit, PrcsMgrServices::LPBSTATE state) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return;
	}
	lpState[unit - 1] = state;
}

PrcsMgrServices::LPBSTATE CEqpState::GetLPState(short unit) {
	if ((unit < 1) || (unit > 4)) {
		ASSERT(0);
		return PrcsMgrServices::NOFOUP;
	}
	return lpState[unit - 1];
}