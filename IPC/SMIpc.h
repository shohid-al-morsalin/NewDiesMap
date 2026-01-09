// SMIpc.h: interface for the CSMIpc class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CSMIpc {
	BOOL bLogSvrPresent;

	void SignalExec();

public:
	BOOL GetSM2Off(void* p, long size, long offset);

	void ReenableLog();

	void LogMsg(CString src, CString des, CString msg, short level = 5, short Iden = 0);

	void ReleaseSM2();

	void* GetSM2Ptr();

	CString appname;

	long smsize1, smsize2;

	BOOL CloseSM();

	BOOL OpenSM(CString app, long size);

	BOOL CreateSM(CString app, long size);

	BOOL CloseSM2();

	BOOL CreateSM2(CString app, long size);

	BOOL OpenSM2(CString app, long size);

	BOOL IsPresent(CString app);

	long GetProcessID();

	BOOL GetSM(void* p, long size);

	BOOL GetSM2(void* p, long size);

	BOOL WriteSM(void* p, long size);

	BOOL WriteSM2(void* p, long size);

	void Done();

	void Destroy();
	CSMIpc();

	virtual ~CSMIpc();

protected:
	BOOL Read(void* p, long size, long wait = INFINITE);

	BOOL Send(void* p, long size, long wait);

	BOOL Request(void* p, long size, long wait);

	long ProcessID; // Process ID of containing process

	HANDLE hMap2; // Shared memory

	HANDLE hMap; // Shared memory

	HANDLE hFree; // Mutex to synchronize clients

	HANDLE hBusy; // Server busy signal

	HANDLE hDone; // Server informs client signal

	HANDLE hExec; // Client informs server

	void* pSM2; // Pointer to shared memory object

	void* pSM; // Pointer to shared memory object

	BOOL bServer; // Is it a server or client instance
};