// Monitor.cpp: implementation of the Monitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Monitor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern HWND hWnd;                                 // GLOBAL 변수  View에 선언

CBufMon	m_BufMon;

Monitor::Monitor()                                // 유의 사항 : constructor 의 mutex사용
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
}

Monitor::~Monitor() {
	CloseHandle(hMutex);
}

void Condition::wait() {
	semacount++;
	monitor->release();
	semaphore.P();
	monitor->lock();
	semacount--;
}

void Condition::signal() {
	if (semacount > 0) {
		semaphore.V();
	}
}

CBufMon::CBufMon() {
	notEmpty = new Condition(this);
	notFull = new Condition(this);
	empty = TRUE;
	count = head = tail = 0;
}

CBufMon::~CBufMon() {
	delete notEmpty;
	delete notFull;
}

/*********************************************************************
 * FILE      :  MONITOR.CPP
 * REVISION  :  A -1
 * Date      :  1998.8.28
 * Purpose   : buffer data를 저장한다
 * Revision
 * history  : A - 1
 *	           Created by Kang woon sik  1999.8.28
 *				Changed to store the system time at receipt of the message  @@Vijay
 *
 ************************************************************************/
void CBufMon::put_event(BYTE* Value, int len, const SYSTEMTIME& time) {
	lock();
	if (count == MAX_QBUFFSIZE) {
		notFull->wait();
	}

	if (MonData[tail].Alloc(len)) {
		MonData[tail].time = time;
		MonData[tail].Set(Value, len);
		tail = (tail + 1) % MAX_QBUFFSIZE;
		count++;
	}
	notEmpty->signal();
	release();
}

/*********************************************************************
 * FILE      :  MONITOR.CPP
 * REVISION  :  A -1
 * Date      :  1998.8.28
 * Purpose   : buffer에 저장된 data를 얻어 온다
 * Revision
 * history  : A - 1
 *	           Created by Kang woon sik  1999.8.28
 ********************************************************************/
 // Note: buffer overun if alloc of value < MonData[head].len [8/9/2012 Administrator]
void CBufMon::get_event(BYTE* Value, int& len, SYSTEMTIME& Time) {
	lock();
	if (count == 0) {
		notEmpty->wait();
	}
	if (MonData[head].pdata) {
		Time = MonData[head].time;
		len = MonData[head].len;
		memcpy(Value, MonData[head].pdata, MonData[head].len);
		head = (head + 1) % MAX_QBUFFSIZE;
		count--;
	}
	notFull->signal();
	release();
}