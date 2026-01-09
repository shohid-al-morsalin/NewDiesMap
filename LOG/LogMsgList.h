// HdrList.h: interface for the CHdrList class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <afxmt.h>
#include "CLogMsgItem.h"

#define MAX_LOGMSGITEM	1024

class CLogMsgList {
	CCriticalSection CS;
	CPtrList Ptr;
private:

	/** @link dependency */
	/*# CLogMsgItem lnkCLogMsgItem; */

private:
public:
	void Set(CLogMsgItem* pMsg);
	CLogMsgItem* GetTail();
	void Clear();
	CLogMsgList();
	virtual ~CLogMsgList();

	// 	short LoadLogCtrl(CListCtrl & m_List, short level, short ID = 0);
	// 	short LoadLastLog(CListCtrl & m_List, short level, short ID = 0);
};
