#pragma once

struct CSMHdr;

class CHdrList {
	CCriticalSection CS;
	CPtrList Ptr;
public:
	void Set(CSMHdr* pHdr);
	CSMHdr* GetHead();
	void Clear();
	CHdrList();
	virtual ~CHdrList();
};

extern CHdrList MsgList;
