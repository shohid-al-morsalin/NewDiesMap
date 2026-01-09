// WaitMsgList.h: interface for the CWaitMsgList class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "PtrListEx.h"

#include "Evt.h"
#include "GemHead.h"

struct WaitMsg {
	CEvt* pEvnt;
	GEM_HEADONLY Header;
	SYSTEMTIME MsgSentTime;
	struct WaitMsg& operator =(const WaitMsg& Rhs) {
		SetEvt(Rhs.pEvnt);	// Required a fresh copy
		//Evnt = Rhs.Evnt;
		memcpy(&Header, &Rhs.Header, sizeof(GEM_HEADONLY));
		memcpy(&(MsgSentTime), &Rhs.MsgSentTime, sizeof(SYSTEMTIME));
		return *this;
	};
	BOOL operator ==(const WaitMsg& Rhs) const {
		BOOL HeaderOk = (0 == memcmp(&(Header), &Rhs.Header, sizeof(GEM_HEADONLY)));
		BOOL TimeOk = (0 == memcmp(&(MsgSentTime), &Rhs.MsgSentTime, sizeof(SYSTEMTIME)));
		return (HeaderOk && TimeOk);
	};
	void CopyEvt(CEvt* p) {
		if (pEvnt) {
			delete pEvnt;
		}
		// pointer copy
		pEvnt = p;
	}
	void SetEvt(CEvt* p) {
		if (pEvnt) {
			delete pEvnt;
		}
		// make a fresh copy
		pEvnt = new CEvt;
		*pEvnt = *p;
	}
	WaitMsg() {
		pEvnt = NULL;
	}
	~WaitMsg() {
		if (pEvnt) {
			delete pEvnt;
		}
	}
};

class CWaitMsgList : public CPtrListEx {
public:
	void RemoveAll();
	WaitMsg* GetTimeoutMsg(CTime& CurrentTime);
	WaitMsg* Get(long SysByte);
	WaitMsg* Remove(long SysByte);
	WaitMsg* Remove();
	void Add(WaitMsg* p);
	void Dealloc(void* p);
	long Get_SysByte(BYTE* pBuf);

	CWaitMsgList();
	virtual ~CWaitMsgList();
};
