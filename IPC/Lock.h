// GlobalLock.h: interface for the GlobalLock class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afxmt.h>

// Basic lock class. The use of the prefix CDM was a semi-random selection.
// You could easily replace CDM with something easier to remember.
class CDMLock {
public:
	long cnt;
	CDMLock(const CString& name);
	virtual ~CDMLock();
	bool Unlock();
	bool Lock(DWORD timeout = INFINITE);

private:
	HANDLE mutex;
};
