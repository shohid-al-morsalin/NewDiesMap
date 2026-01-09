#include "stdafx.h"
#include "CLogMsgItem.h"

CLogMsgItem::CLogMsgItem() : Level(5) {
	DateTime = "";
	Source = "";
	Destination = "";
	MessageText = "";
	ID = 0;
}

CLogMsgItem::~CLogMsgItem() {}

BOOL CLogMsgItem::Save(FILE* fp) {
	if (!fp) {
		return FALSE;
	}
	CString s;
	//	s.Format("%s\t%s\t%s\t%s\t%d\r\n", DateTime, Source, Destination, MessageText, ID);
	s.Format("%s\t%s\t%s\t%s\r\n", DateTime, Source, Destination, MessageText);
	if (fwrite(s, sizeof(char), s.GetLength(), fp) > 0) {
		return TRUE;
	}
	return FALSE;
}

void CLogMsgItem::AddTimeStamp(void) {
	DateTime = CTime::GetCurrentTime().Format("%y:%m:%d %H:%M:%S");
	//	DateTime = CTime::GetCurrentTime().Format("%b%d %H:%M:%S");
}

void CLogMsgItem::Set(CString Src, CString Des, CString Msg, short Lvl) {
	Source = Src;
	Destination = Des;
	MessageText = Msg;
	Level = Lvl;
}

void CLogMsgItem::Set(CString Msg, short Lvl) {
	MessageText = Msg;
	Level = Lvl;
}

void CLogMsgItem::SetID(short Iden) {
	ID = Iden;
}

void CLogMsgItem::SetID(short Iden, CString Src, CString Des) {
	ID = Iden;
	Source = Src;
	Destination = Des;
}