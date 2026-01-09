// LogMessage.h: interface for the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include "../general_utils.h"
#include "comlog.h"

class CLogMessage {
public:
	void WaitBit(std::string& WtBit);
	void SysByte(std::string& Data);
	void SreamFunction(std::string& Stream, std::string& Function, std::string& Wbit);
	void MessageSystem(std::string& Subsystem);
	void MessageType(std::string& Type);
	int  MessageType();
	friend bool CComLog::ReadMessage(CFile&, CLogMessage&, int, int);
	bool TimeStamp(std::string& Data);
	bool Decode(std::string& Data, bool bAnnotate = true);
	CLogMessage();
	virtual ~CLogMessage();
	std::string m_Key;
private:
	short   m_Version;
	MsgType m_Type;
	MsgUnit m_Unit;
	void Init();
	char* m_Data;
	UINT m_MsgLen;
	MsgSubsystem m_SubSystem;
	SYSTEMTIME m_MsgTime;
};
