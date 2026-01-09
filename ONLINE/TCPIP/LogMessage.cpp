// LogMessage.cpp: implementation of the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogMessage.h"
#include "..\Gem\SecsMessage.h"
#include "..\Gem\Annotator.h"

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogMessage::CLogMessage() {
	m_Data = NULL;
	::ZeroMemory(&m_MsgTime, sizeof(m_MsgTime));
	m_MsgLen = 0;
	m_SubSystem = ALL_SUB_SYSTEM;
	m_Type = SECS_MESSAGE;
	m_Version = 0;
	m_Key = "";
}

CLogMessage::~CLogMessage() {
	if (m_Data != NULL) delete[] m_Data;
	m_Data = NULL;
}

void CLogMessage::Init() {
	if (m_Data == NULL) delete[] m_Data;
	m_Data = NULL;
	::ZeroMemory(&m_MsgTime, sizeof(m_MsgTime));
	m_MsgLen = 0;
	m_SubSystem = ALL_SUB_SYSTEM;
	m_Type = SECS_MESSAGE;
}

bool CLogMessage::Decode(string& Data, bool bAnnotate) {
	Data.erase();
	if ((m_Type == SECS_MESSAGE) && (m_MsgLen)) {
		SecsMessage Msg;
		Msg.DecodeMessage((BYTE*)m_Data + 10, m_MsgLen - 10);
		Msg.PrintSML(Data);
		if (bAnnotate) {
			unsigned char Function, Stream;
			Function = m_Data[3];
			Stream = m_Data[2] & 0x7f;
			Annot.Annotate(Msg, Stream, Function, Data);
		}
		return true;
	}
	else {
		Data = m_Data;
		return true;
	}
	return false;
}

bool CLogMessage::TimeStamp(string& Data) {
	Data.erase();
	::TimeStamp(m_MsgTime, Data);
	return true;
}

void CLogMessage::MessageType(string& Type) {
	Type.erase();
	switch (m_Type) {
	case SECS_MESSAGE:
		Type = "SECS Message";
		break;
	case INFO_MESSAGE:
		Type = "Information";
		break;
	case WARNING_MESSAGE:
		Type = "Warning";
		break;
	case ERROR_MESSAGE:
		Type = "Error";
		break;
	case FATAL_ERROR_MESSAGE:
		Type = "Fatal Error";
		break;
	}
}

int CLogMessage::MessageType(void) {
	return m_Type;
}

void CLogMessage::MessageSystem(string& Subsystem) {
	Subsystem.erase();
	switch (m_SubSystem) {
	case ALL_SUB_SYSTEM: Subsystem = "Not Applicable"; break;
	case E30:             Subsystem = "GEM"; break;
	case E32:             Subsystem = "Material Movement"; break;
	case E36:             Subsystem = "Info Tagging"; break;
	case E37:             Subsystem = "HSMS"; break;
	case E38:             Subsystem = "CTMC"; break;
	case E39:             Subsystem = "OSS"; break;
	case E40:             Subsystem = "Processing Mgmt"; break;
	case E41:             Subsystem = "Exception Mgmt"; break;
	case E42:             Subsystem = "Recipe Mgmt"; break;
	case E50:             Subsystem = "SMIF Indexer"; break;
	case E53:             Subsystem = "Event Reporting"; break;
	case E54:             Subsystem = "Sensor//Actuator Network"; break;
	case E58:             Subsystem = "ARAMS"; break;
	case E81:             Subsystem = "CIM"; break;
	case E82:             Subsystem = "IBSEM"; break;
	case E84:             Subsystem = "PIO"; break;
	case E86:             Subsystem = "CIM Labor"; break;
	case E87:             Subsystem = "Carrier Mgmt"; break;
	case E88:             Subsystem = "Stocker SEM"; break;
	case E90:             Subsystem = "Substrate Mgmt"; break;
	case E93:             Subsystem = "Advanced Process Control"; break;
	case E94:             Subsystem = "Control Job Mgmt"; break;
	case E95:             Subsystem = "Human Interface"; break;
	case E96:             Subsystem = "CIM Tech. Architecture"; break;
	case E97:             Subsystem = "CIM Global Declaration"; break;
	case E98:             Subsystem = "OBEM"; break;
	case E99:             Subsystem = "CIDR//W"; break;
	case E102:            Subsystem = "Matl. Transport , Storage"; break;
	case GUI:             Subsystem = "Genral and or (GUI)"; break;
	}
}

void CLogMessage::SreamFunction(string& Stream, string& Function, string& WBit) {
	Stream.erase(); Function.erase(); WBit.erase();
	switch (m_Type) {
	case SECS_MESSAGE:
	{
		unsigned char Temp;
		char Buf[12];
		Temp = m_Data[3];
		sprintf(Buf, "%u", Temp);
		Function = Buf;
		Temp = m_Data[2] & 0x7f;
		sprintf(Buf, "%u", Temp);
		Stream = Buf;
		Temp = m_Data[2] & 0x80;
		WBit = (Temp) ? "Set" : "Clear";
	}
	break;
	case INFO_MESSAGE:
	case WARNING_MESSAGE:
	case ERROR_MESSAGE:
	case FATAL_ERROR_MESSAGE:
		Stream = Function = WBit = "NA.";
		break;
	}
}

void CLogMessage::SysByte(string& Data) {
	Data.erase();
	switch (m_Type) {
	case SECS_MESSAGE:
	{
		UINT SysByte;
		char Buf[26];
		memcpy(&SysByte, &(m_Data[6]), 4);
		ByteSwap(&SysByte, 4);
		sprintf(Buf, "%d", SysByte);
		Data = Buf;
	}
	break;
	case INFO_MESSAGE:
	case WARNING_MESSAGE:
	case ERROR_MESSAGE:
	case FATAL_ERROR_MESSAGE:
		Data = "Not Applicable";
		break;
	}
}