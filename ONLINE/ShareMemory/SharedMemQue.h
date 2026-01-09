// SharedMemQue.h: interface for the CSharedMemQue class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "ShareQue.h"

class CEvt;

class CSharedMemQue {
	static CShareQue MemQue;

public:
	int GetSharedQcnt();

	CEvt* ReadQue();
	BOOL WriteQue(CEvt* dwData);

	void lock();
	void Release();

	BOOL CreateMemory(char* dwName, int nMaxcnt, int size);
	BOOL OpenMemory(char* dwName);
	void CloseMemory();

	CSharedMemQue();
	virtual ~CSharedMemQue();
};
