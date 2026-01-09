// ShareMemory.h: interface for the CShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
const int SHAREDMAXSIZE = 0XFFFFFF;

class CShareMemory {
public:
	HANDLE   m_hSharedMemory;
	HANDLE	 m_hSharedMemoryMutex;
	char     sMutex0[100];

public:
	void Clear(long size);
	void release();
	void lock();
	void CloseSharedMemory();
	BOOL ReadMemory(char* pBuf, long offset, long size);
	BOOL WriteMemory(char* pBuf, long offset, long size);
	BOOL OpenMemory(char* pName);
	BOOL CreateMemory(char* pName);
	CShareMemory();
	virtual ~CShareMemory();
};