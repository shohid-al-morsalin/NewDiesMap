#ifndef __GENERAL_UTILITIES_FSM_INCLUDED_H__
#define __GENERAL_UTILITIES_FSM_INCLUDED_H__

#include <vector>

enum TOKEN_RES { COMMENT, LINE_ERROR, LINE_TOO_LONG, OK };
TOKEN_RES Tokenize(char* Str, UINT MaxLineLen, std::vector<std::string>& Tokens, const char Sep, const char Comment);
time_t		Convert2Time(std::string&);
SYSTEMTIME	Convert2SysTime(std::string&);

enum MsgSubsystem {
	ALL_SUB_SYSTEM = 0,
	E30, E32, E36, E37, E38, E39,
	E40, E41, E42, E50, E53,
	E54, E58, E81, E82, E84,
	E86, E87, E88, E90, E93,
	E94, E95, E96, E97, E98,
	E99, E102, GUI
};
enum MsgUnit {
	NA = 0, STATE_MODEL
};

enum MsgType {
	SECS_MESSAGE = 0,
	INFO_MESSAGE,
	WARNING_MESSAGE,
	ERROR_MESSAGE,
	FATAL_ERROR_MESSAGE
};

enum EditCol { EDT_GREEN, EDT_BLUE, EDT_RED, EDT_ORANGE, EDT_WHITE, EDT_PINK };

bool operator>(SYSTEMTIME LHS, SYSTEMTIME RHS);
bool operator<(SYSTEMTIME LHS, SYSTEMTIME RHS);
void ByteSwap(void* pVoid, int nNumBytes);
void TimeStamp(SYSTEMTIME TM, std::string& Time);
void TimeStamp(CTime& TM, std::string& Time);
void ConvertTime(std::string& TimeStamp, const CTime& Time);

#endif __GENERAL_UTILITIES_FSM_INCLUDED_H__
