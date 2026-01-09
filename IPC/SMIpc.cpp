// SMIpc.cpp: implementation of the CSMIpc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SMHdr.h"
#include "SMIpc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMIpc::CSMIpc() {
	smsize1 = 0;
	smsize2 = 0;
	pSM = NULL;
	hMap = NULL;
	pSM2 = NULL;
	hMap2 = NULL;
	hBusy = NULL;
	hDone = NULL;
	hExec = NULL;
	hFree = NULL;
	ProcessID = NULL;
	appname = "";
	bServer = FALSE;
	bLogSvrPresent = TRUE;
}

CSMIpc::~CSMIpc() {
	Destroy();
}

BOOL CSMIpc::IsPresent(CString app) {
	CString name = app + "mem";
	HANDLE map = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	if (map) {
		CloseHandle(map);
		return TRUE;
	}
	return FALSE;
}

void CSMIpc::Destroy() {
	CloseSM();
	CloseSM2();
}

// Send with confirmation of receive
BOOL CSMIpc::Send(void* p, long size, long wait) {
	if (!p) {
		ASSERT(0);
		return FALSE;
	}
	if (smsize1 <= 0) {
		ASSERT(0);
		return FALSE;
	}
	if (size > smsize1) {
		ASSERT(0);
		return FALSE;
	}
	if (!pSM) {
		ASSERT(0);
		return FALSE;
	}
	// artificial restriction, may be removed in future
	if (bServer) {
		// Server is not allowed to send messages
		ASSERT(0);;
		return FALSE;
	}
	BOOL ret = FALSE;
	if (WaitForSingleObject(hFree, wait) == WAIT_OBJECT_0) {
		//Sleep(25);	// Waiting for the other party to reset hBusy
		if (WaitForSingleObject(hBusy, wait) == WAIT_OBJECT_0) {
			memcpy(pSM, p, size);
			//ReleaseMutex(hBusy);	//YFL2
			SetEvent(hExec);
			ReleaseMutex(hBusy);
			//	Sleep(50);	// Wait for the other party to reset hDone
			if (WaitForSingleObject(hDone, INFINITE) == WAIT_OBJECT_0) {
				ret = TRUE;
			}
		}
		ReleaseMutex(hFree);
	}
	return ret;
}

// Request perform a Send() and GetSM() under locking condition
BOOL CSMIpc::Request(void* p, long size, long wait) {
	if (!p) {
		ASSERT(0);
		return FALSE;
	}
	if (smsize1 <= 0) {
		ASSERT(0);
		return FALSE;
	}
	if (size > smsize1) {
		ASSERT(0);
		return FALSE;
	}
	if (!pSM) {
		ASSERT(0);
		return FALSE;
	}
	// artificial restriction, may be removed in future
	if (bServer) {
		// Server is not allowed to send messages
		ASSERT(0);;
		return FALSE;
	}
	//	BOOL brep = FALSE;
	BOOL ret = FALSE;
	if (WaitForSingleObject(hFree, wait) == WAIT_OBJECT_0) {
		//	Sleep(10);
		if (WaitForSingleObject(hBusy, 0) == WAIT_OBJECT_0) {
			memcpy(pSM, p, size);
			ReleaseMutex(hBusy);
			SetEvent(hExec);
			//	Sleep(10);
			if (WaitForSingleObject(hDone, INFINITE) == WAIT_OBJECT_0) {
				memcpy(p, pSM, size);
				ret = TRUE;
			}
		}
		ReleaseMutex(hFree);
	}
	return ret;
}

// Need to send done event at completion of process
BOOL CSMIpc::Read(void* p, long size, long wait) {
	if (!hExec || !hBusy) {
		ASSERT(0);
		return FALSE;
	}
	if (size > smsize1) {
		ASSERT(0);
		return FALSE;
	}
	BOOL ret = FALSE;
	if (!pSM || !p || (size <= 0)) {
		return ret;
	}
	if (WaitForSingleObject(hExec, wait) == WAIT_OBJECT_0) {
		if (WaitForSingleObject(hBusy, INFINITE) == WAIT_OBJECT_0) {
			memcpy(p, pSM, size);
			ret = TRUE;
		}
		//	SetEvent(hDone);
	}
	return ret;
}

void CSMIpc::Done() {
	if (hDone) {
		SetEvent(hDone);
	}
	if (hBusy) {
		ReleaseMutex(hBusy);
	}
}

long CSMIpc::GetProcessID() {
	return ProcessID;
}

// Get Share Memory
// Read operation by client, make sure the client has access to shared memory before write
BOOL CSMIpc::GetSM(void* p, long size) {
	if (size > smsize1) {
		return FALSE;
	}
	if (!pSM) {
		return FALSE;
	}
	memcpy(p, pSM, size);
	return TRUE;
}

// Get Share Memory 2
// Read operation by client, make sure the client has access to shared memory before write
BOOL CSMIpc::GetSM2(void* p, long size) {
	if (size > smsize2) {
		return FALSE;
	}
	if (!pSM2) {
		return FALSE;
	}
	memcpy(p, pSM2, size);
	return TRUE;
}

BOOL CSMIpc::GetSM2Off(void* p, long size, long offset) {
	// Ignor size check
	if (!pSM2) {
		return FALSE;
	}
	memcpy(p, (char*)pSM2 + offset, size);
	return TRUE;
}

// Write operation by server
// Do not need to lock SM because this write operation is a request
// from client which has locked the SM
BOOL CSMIpc::WriteSM(void* p, long size) {
	if (size > smsize1) {
		return FALSE;
	}
	if (pSM && (size > 0)) {
		memcpy(pSM, p, size);
		return TRUE;
	}
	return FALSE;
}

// Write operation by server
// Do not need to lock SM because this write operation is a request
// from client which has locked the SM
BOOL CSMIpc::WriteSM2(void* p, long size) {
	if (size > smsize2) {
		return FALSE;
	}
	if (pSM2 && (size > 0)) {
		memcpy(pSM2, p, size);
		return TRUE;
	}
	return FALSE;
}

BOOL CSMIpc::CreateSM(CString app, long size) {
	CString appstr;
	ProcessID = ::GetCurrentProcessId();
	// Try to create file mapping object (assume that this is the server)
	if (!hMap) {
		appstr = app + "mem";
		hMap = ::CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, size, appstr);
		if (!hMap) {
			ASSERT(0);
			return FALSE;
		}
	}
	if (!pSM && hMap) {
		pSM = (void*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
	}
	if (!pSM) {
		ASSERT(0);
		CloseHandle(hMap);
		hMap = NULL;
		return FALSE;
	}
	if (!hFree) {
		appstr = app + "free";
		hFree = ::CreateMutex(NULL, TRUE, appstr);
	}
	if (!hExec) {
		appstr = app + "exec";
		hExec = ::CreateEvent(NULL, FALSE, FALSE, appstr);
	}
	if (!hDone) {
		appstr = app + "done";
		hDone = ::CreateEvent(NULL, FALSE, FALSE, appstr);
	}
	if (!hBusy) {
		appstr = app + "busy";
		hBusy = ::CreateMutex(NULL, FALSE, appstr);
	}
	smsize1 = size;
	appname = app;
	ReleaseMutex(hFree);

	return TRUE;
}

BOOL CSMIpc::OpenSM(CString app, long size) {
	ProcessID = ::GetCurrentProcessId();
	CString appstr;
	if (!hMap) {
		appstr = app + "mem";
		hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, appstr);
		if (!hMap) {
			return FALSE;
		}
	}
	if (!pSM && hMap) {
		pSM = (void*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
	}
	if (!pSM) {
		ASSERT(0);
		CloseHandle(hMap);
		hMap = NULL;
		return FALSE;
	}
	if (!hFree) {
		appstr = app + "free";
		hFree = ::CreateMutex(NULL, FALSE, appstr);
	}
	if (!hExec) {
		appstr = app + "exec";
		hExec = ::CreateEvent(NULL, FALSE, FALSE, appstr);
	}
	if (!hDone) {
		appstr = app + "done";
		hDone = ::CreateEvent(NULL, FALSE, FALSE, appstr);
	}
	if (!hBusy) {
		appstr = app + "busy";
		hBusy = ::CreateMutex(NULL, FALSE, appstr);
	}
	smsize1 = size;
	appname = app;

	return TRUE;
}

BOOL CSMIpc::CloseSM() {
	if (hDone) {
		CloseHandle(hDone);
	}
	hDone = NULL;
	if (hBusy) {
		CloseHandle(hBusy);
	}
	hBusy = NULL;
	if (hExec) {
		CloseHandle(hExec);
	}
	hExec = NULL;
	if (hFree) {
		CloseHandle(hFree);
	}
	hFree = NULL;
	if (pSM) {
		UnmapViewOfFile(pSM);
	}
	pSM = NULL;
	if (hMap) {
		CloseHandle(hMap);
	}
	hMap = NULL;
	smsize1 = 0;

	return TRUE;
}

void* CSMIpc::GetSM2Ptr() {
	return pSM2;
}

// Not thread synchronization implimented
BOOL CSMIpc::CreateSM2(CString app, long size) {
	CString appstr;
	if (hMap2) {
		CloseSM2();
	}
	if (!hMap2) {
		appstr = app + "mem";
		hMap2 = ::CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, size, appstr);
		if (!hMap2) {
			return FALSE;
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
		}
	}
	if (!pSM2 && hMap2) {
		pSM2 = (void*)::MapViewOfFile(hMap2, FILE_MAP_ALL_ACCESS, 0, 0, size);
	}
	if (!pSM2) {
		ASSERT(0);
		CloseHandle(hMap2);
		hMap2 = NULL;
		return FALSE;
	}
	smsize2 = size;
	return TRUE;
}

// Not thread synchronization implimented
BOOL CSMIpc::OpenSM2(CString app, long size) {
	CString appstr;
	if (hMap2) {
		CloseSM2();
	}
	if (!hMap2) {
		appstr = app + "mem";
		hMap2 = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, appstr);
		if (!hMap2) {
			// No create
			return FALSE;
		}
	}
	if (!pSM2 && hMap2) {
		pSM2 = (void*)::MapViewOfFile(hMap2, FILE_MAP_ALL_ACCESS, 0, 0, size);
	}
	if (!pSM2) {
		ASSERT(0);
		CloseHandle(hMap2);
		hMap2 = NULL;
		return FALSE;
	}
	smsize2 = size;
	return TRUE;
}

BOOL CSMIpc::CloseSM2() {
	if (pSM2) {
		UnmapViewOfFile(pSM2);
	}
	pSM2 = NULL;
	if (hMap2) {
		CloseHandle(hMap2);
	}
	hMap2 = NULL;
	smsize2 = 0;
	return TRUE;
}

void CSMIpc::ReleaseSM2() {}

void CSMIpc::LogMsg(CString src, CString des, CString msg, short level, short iden) {
	if (!bLogSvrPresent) {
		return;
	}
	CSMHdr SM;
	SM.cmd = CSMHdr::CTLOGMSG;
	SM.sub[0] = level;
	SM.sub[1] = iden;
	sprintf(SM.text, "%s\t%s\t%s\t%s", (CTime::GetCurrentTime().Format("%y:%m:%d  %H:%M:%S")).GetBuffer(0), src.GetBuffer(0), des.GetBuffer(0), msg.GetBuffer(0));
	if (!Send(&SM, sizeof(CSMHdr), 8000)) {
		bLogSvrPresent = FALSE;
	}
}

void CSMIpc::ReenableLog() {
	bLogSvrPresent = TRUE;
}