// PtrListEx.h: interface for the CPtrListEx class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <afxmt.h>

class CPtrListEx {
	friend class CWaitMsgList;

	/** List of CQueMsgs */
	CPtrList QueList;

	/** @link dependency */

	/*# CQueMsgs lnkCQueMsgs; */

public:
	CCriticalSection CS;
public:
	virtual void Dealloc(void* p) = 0;
	BOOL IsEmpty();
	int GetCount();
	void* RemoveHead();
	void AddTail(void* pMsg);
	void Clear();
	CPtrListEx();
	virtual ~CPtrListEx();
};