// TimerMGR.h: interface for the CTimerMGR class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CTimerMGR {
	HWND hWnd;
	int Event;
	short Count;
public:
	BOOL IsSet();
	void Init(HWND hWnd, int Evt);
	BOOL Kill();
	BOOL Set(UINT Elapse);
	CTimerMGR();
	virtual ~CTimerMGR();
};
