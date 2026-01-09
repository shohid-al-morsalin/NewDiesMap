// GlobalLock.cpp: implementation of the GlobalLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lock.h"

#include <afxmt.h>

CDMLock::CDMLock(const CString& name) {
	cnt = 0;
	mutex = CreateMutex(NULL, 0, name);
}

CDMLock::~CDMLock() {
	CloseHandle(mutex);
}

// Release lock.
bool CDMLock::Unlock() {
	if (mutex) {
		if (ReleaseMutex(mutex)) {
			cnt--;
			return true;
		}
	}
	else {
		ASSERT(0);
	}
	return false;
}

// Get lock. If timeout expires than return fail.
bool CDMLock::Lock(DWORD timeout) {
	if (mutex) {
		if (WaitForSingleObject(mutex, timeout) == WAIT_OBJECT_0) {
			cnt++;
			return true;
		}
	}
	else {
		ASSERT(0);
	}
	return false;
}