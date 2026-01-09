// TimerMGR.cpp: implementation of the CTimerMGR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimerMGR.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimerMGR::CTimerMGR() {
	hWnd = 0;
	Event = 0;
	Count = 0;
}

CTimerMGR::~CTimerMGR() {
	if (Count) {
		if (hWnd) {
			KillTimer(hWnd, Event);
		}
		else {
			ASSERT(0);
		}
	}
}

BOOL CTimerMGR::Set(UINT Elapse) {
	if (!Event) {
		ASSERT(0);
		return FALSE;
	}
	if (Count == 0) {
		Count++;
		short tout = 16;
		while (tout--) {
			if (SetTimer(hWnd, Event, Elapse, NULL)) {
				break;
			}
			Sleep(200);
		}
		if (tout < 0) {
			ASSERT(0);
			return FALSE;
		}
	}
	else if (Count > 0) {
		Count++;
	}
	else {
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

BOOL CTimerMGR::Kill() {
	if (!Event) {
		ASSERT(0);
		return FALSE;
	}
	if (Count == 1) {
		KillTimer(hWnd, Event);
		Count--;
	}
	else if (Count > 1) {
		Count--;
	}
	else {
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

void CTimerMGR::Init(HWND hWin, int Evt) {
	if (Count) {
		while (Count) {
			Kill();
		}
		//ASSERT(0);
	}
	hWnd = hWin;
	Event = Evt;
	Count = 0;
}

BOOL CTimerMGR::IsSet() {
	if (Count) {
		return TRUE;
	}
	return FALSE;
}