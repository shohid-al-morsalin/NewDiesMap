// Monitor.h: interface for the Monitor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
//#include "sevent.h"
#include "..\inc\define.h"

#define MAX_QBUFFSIZE  150

class Monitor {
public:
	Monitor();
	virtual ~Monitor();
protected:
	HANDLE hMutex;
	virtual void lock() {
		WaitForSingleObject(hMutex, INFINITE);
	}
	virtual void release() {
		ReleaseMutex(hMutex);
	}
	friend class Condition;
};

class Semaphore {
protected:
	HANDLE hSemaphore;
public:
	Semaphore() {
		hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	}
	~Semaphore() {
		CloseHandle(hSemaphore);
	}
	void P() {
		WaitForSingleObject(hSemaphore, INFINITE);
	}
	void V() {
		ReleaseSemaphore(hSemaphore, 1, NULL);
	};
};

class Condition {
protected:
	Semaphore semaphore;
	int semacount;
	Monitor* monitor;
public:
	void signal();
	void wait();
	Condition(Monitor* mon) {
		monitor = mon;
		semacount = 0;
	};
	~Condition() {};
};

class CMonData {
public:
	BYTE* pdata;
	int len;
	SYSTEMTIME time;

	CMonData() {
		len = 0;
		pdata = NULL;
	}
	virtual ~CMonData() {
		Clear();
	}
	BOOL Alloc(int length) {
		Clear();
		pdata = new BYTE[length + 1];
		if (pdata) {
			len = length;
			return TRUE;
		}
		return FALSE;
	}
	BOOL Set(BYTE* pBuf, int length) {
		if (length > len) {
			if (!Alloc(length)) {
				ASSERT(0);
				return FALSE;
			}
		}
		len = length;	// if previous alloc where len > length, len will be smaller than actual alloc [8/9/2012 Administrator]
		memcpy(pdata, pBuf, length);
		return TRUE;
	}
	void Clear() {
		len = 0;
		if (pdata) {
			delete[] pdata;
			pdata = NULL;
		}
	}
};

class CBufMon : public  Monitor {
protected:
	Condition* notEmpty, * notFull;
	int empty;
	int head, tail;

public:
	//	BYTE data[MAX_QBUFFSIZE][MAX_RECVSTRING + sizeof(SYSTEMTIME)+100];//2500 |||||||||||||
	CMonData MonData[MAX_QBUFFSIZE];
	CBufMon();
	~CBufMon();
	void put_event(BYTE* value, int len, const SYSTEMTIME& Time);
	void get_event(BYTE* value, int& len, SYSTEMTIME& Time);
	int count;
	//char *tdata;
};

extern CBufMon m_BufMon;