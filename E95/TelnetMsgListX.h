// TelnetMsgList.h: interface for the CTelnetMsgList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELNETMSGLIST_H__047C08A7_D51C_413B_8384_71F8F989B896__INCLUDED_)
#define AFX_TELNETMSGLIST_H__047C08A7_D51C_413B_8384_71F8F989B896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTelnetMsg;

class CTelnetMsgList  
{
	unsigned int MsgCnt;
	CCriticalSection CS;
public:
	CTelnetMsg * GetMatch(char *msg);
	CTelnetMsg * RemoveTail();
	void Clear();
	void Add(char *msg);
	CTelnetMsg * Add(CString &msg);
	void Add(CTelnetMsg *msg);
	CTelnetMsg * Remove();

	CTelnetMsgList();
	virtual ~CTelnetMsgList();

private:
	CPtrList List;
};

#endif // !defined(AFX_TELNETMSGLIST_H__047C08A7_D51C_413B_8384_71F8F989B896__INCLUDED_)
