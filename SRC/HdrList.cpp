#include "stdafx.h"

#include "HdrList.h"
//#include "IPC/SMHdr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CHdrList MsgList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHdrList::CHdrList() {}

CHdrList::~CHdrList() {
	Clear();
}

void CHdrList::Clear() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = Ptr.GetHeadPosition();
	while (pos) {
		CSMHdr* p = (CSMHdr*)Ptr.GetNext(pos);
		if (p) {
			delete p;
		}
	}
}

CSMHdr* CHdrList::GetHead() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (Ptr.GetCount()) {
		return (CSMHdr*)Ptr.RemoveHead();
	}
	return NULL;
}

void CHdrList::Set(CSMHdr* pHdr) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pHdr) {
		Ptr.AddTail(pHdr);
	}
}