// TelnetMsgList.cpp: implementation of the CTelnetMsgList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TelnetMsgList.h"
#include "TelnetMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetMsgList::CTelnetMsgList()
{
	MsgCnt = 0;
}

CTelnetMsgList::~CTelnetMsgList()
{
	Clear();
}

CTelnetMsg * CTelnetMsgList::Remove()
{
	CSingleLock Lock(&CS);
	Lock.Lock();

	if (List.GetCount() > 0)
	{
		return (CTelnetMsg *)List.RemoveHead();
	}
	return NULL;
}

void CTelnetMsgList::Clear()
{
	CSingleLock Lock(&CS);
	Lock.Lock();

	POSITION pos = List.GetHeadPosition();
	while (pos)
	{
		CTelnetMsg *pmsg = (CTelnetMsg *)List.GetNext(pos);
		if (pmsg)
		{
			delete pmsg;
		}
	}
	List.RemoveAll();
}

CTelnetMsg * CTelnetMsgList::Add(CString &msg)
{
	CSingleLock Lock(&CS);
	Lock.Lock();

	CTelnetMsg *pmsg = new CTelnetMsg;
	pmsg->msg = msg;
	pmsg->idx = MsgCnt++;
	List.AddTail((void *)pmsg);

	return pmsg;
}

void CTelnetMsgList::Add(char *msg)
{
	CSingleLock Lock(&CS);
	Lock.Lock();

	CTelnetMsg *pmsg = new CTelnetMsg;
	pmsg->msg = msg;
	pmsg->idx = MsgCnt++;
	List.AddTail((void *)pmsg);
}

void CTelnetMsgList::Add(CTelnetMsg *pmsg)
{
	CSingleLock Lock(&CS);
	Lock.Lock();

	pmsg->idx = MsgCnt++;
	List.AddTail((void *)pmsg);
}

CTelnetMsg * CTelnetMsgList::RemoveTail()
{
	return (CTelnetMsg *)List.RemoveTail();
}


// Returned pointer need to be deleted externally [6/4/2012 Administrator]
CTelnetMsg * CTelnetMsgList::GetMatch(char *msg)
{
	int len;
	len = strlen(msg);

	POSITION pos = List.GetHeadPosition();
	POSITION opos;
	while (pos)
	{
		opos = pos;
		CTelnetMsg *pmsg = (CTelnetMsg *) List.GetNext(pos);
		if (pmsg)
		{
			if (len==1)
			{
				if (pmsg->ret == ":?")
				{
					List.RemoveAt(opos);
					return pmsg;
				}
			}
			else
			{
				CString sub, sub1;
				sub = msg;
				int n = sub.Find(" ");
				if (n != -1)
				{
					sub1 = sub.Left(n);
				}
				else
				{
					sub1 = sub;
				}
				if (pmsg->ret == sub1)
				{
					List.RemoveAt(opos);
					return pmsg;
				}
			}
		}
	}
	return NULL;
}
