#pragma once

class CLogMsgItem {
public:
	CLogMsgItem();
	~CLogMsgItem();

	CString DateTime;

	CString Source;

	CString Destination;

	CString MessageText;

	short ID;

public:
	short Level;

	BOOL Save(FILE* fp);

	void AddTimeStamp(void);

	void Set(CString Src, CString Des, CString Msg, short Lvl);

	void Set(CString Msg, short Lvl);

	void SetID(short Iden);

	void SetID(short Iden, CString Src, CString Des);
};
