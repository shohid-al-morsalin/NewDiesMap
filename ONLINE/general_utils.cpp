#include "stdafx.h"
#include "general_utils.h"
using namespace std;
extern UINT TimeFormat;
/**
	Note : Only comment at begining at string is allowed

	*/
TOKEN_RES Tokenize(char* Str, UINT MaxLineLen, vector<string>& Tokens, const char Sep, const char Comment) {
	if (!Str) {
		return LINE_ERROR;
	}
	if (MaxLineLen < strlen(Str)) {
		return LINE_TOO_LONG;
	}
	char nBuf[64] = { 0 };
	string Token;
	Tokens.clear();
	int nLen = 0, BufInd = 0;
	// Check for comments
	while (isspace(Str[nLen++]));
	char First = Str[nLen - 1];
	if (Comment == First) {
		return COMMENT;
	}
	Token = Str;
	if (string::npos == Token.find(Sep)) {
		return LINE_ERROR;
	}
	// Tokenize
	nLen = strlen(Str);
	for (int i = 0; i < nLen; i++) {
		if (Str[i] == Sep) {
			nBuf[BufInd] = 0;
			Token = nBuf;
			Tokens.push_back(Token);
			memset(nBuf, 0, sizeof nBuf);
			BufInd = 0;
			continue;
		}
		nBuf[BufInd++] = Str[i];
	}
	// Last token
	if (BufInd > 0) {
		nBuf[BufInd - 1] = 0;
		Token = nBuf;
		Tokens.push_back(Token);
	}
	return OK;
}

time_t	Convert2Time(string& TimeStamp) {
	UINT TimeLen = TimeStamp.size();
	if (!((TimeLen == 12) || (TimeLen == 16))) {
		return 0;
	}
	if (TimeLen == 16) {
		TimeLen = 14;
	}
	TimeLen--;
	tm TimeStr;
	TimeStr.tm_sec = atoi(TimeStamp.substr(TimeLen - 1, TimeLen).c_str());
	TimeStr.tm_min = atoi(TimeStamp.substr(TimeLen - 3, TimeLen - 2).c_str());
	TimeStr.tm_hour = atoi(TimeStamp.substr(TimeLen - 5, TimeLen - 4).c_str());
	TimeStr.tm_wday = atoi(TimeStamp.substr(TimeLen - 7, TimeLen - 6).c_str());
	TimeStr.tm_mon = atoi(TimeStamp.substr(TimeLen - 9, TimeLen - 8).c_str());
	TimeStr.tm_year = atoi(TimeStamp.substr(0, TimeLen - 10).c_str());
	return mktime(&TimeStr);
}
SYSTEMTIME	Convert2SysTime(string& TimeStamp) {
	UINT TimeLen = TimeStamp.size();
	short Ms = 0;
	SYSTEMTIME Temp;
	::ZeroMemory(&Temp, sizeof Temp);
	if (!((TimeLen == 12) || (TimeLen == 16))) {
		ASSERT(0);
		return Temp;
	}
	if (TimeLen == 16) {
		Ms = atoi(TimeStamp.substr(TimeLen - 2, TimeLen - 1).c_str());
		TimeLen = 14;
	}
	TimeLen--;
	Temp.wYear = atoi(TimeStamp.substr(0, TimeLen - 10).c_str());
	Temp.wMonth = atoi(TimeStamp.substr(TimeLen - 9, TimeLen - 8).c_str());
	Temp.wDay = atoi(TimeStamp.substr(TimeLen - 7, TimeLen - 6).c_str());
	Temp.wHour = atoi(TimeStamp.substr(TimeLen - 5, TimeLen - 4).c_str());
	Temp.wMinute = atoi(TimeStamp.substr(TimeLen - 3, TimeLen - 2).c_str());
	Temp.wSecond = atoi(TimeStamp.substr(TimeLen - 1, TimeLen).c_str());
	Temp.wMilliseconds = Ms;
	return Temp;
}

bool operator>(SYSTEMTIME LHS, SYSTEMTIME RHS) {
	FILETIME Lhs, Rhs;
	::SystemTimeToFileTime(&LHS, &Lhs);
	::SystemTimeToFileTime(&RHS, &Rhs);
	long Res = ::CompareFileTime(&Lhs, &Rhs);
	return (Res >= 0);
}
bool operator<(SYSTEMTIME LHS, SYSTEMTIME RHS) {
	FILETIME Lhs, Rhs;
	::SystemTimeToFileTime(&LHS, &Lhs);
	::SystemTimeToFileTime(&RHS, &Rhs);
	long Res = ::CompareFileTime(&Lhs, &Rhs);
	return (Res <= 0);
}

void ByteSwap(void* pVoid, int nNumBytes) {
	BYTE* pByte = (BYTE*)pVoid;
	int mid = nNumBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumBytes - 1 - nIndex];
		pByte[nNumBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
}
void TimeStamp(SYSTEMTIME TM, string& Time) {
	Time.erase();
	char Buffer[18];
	sprintf(Buffer, "%02d/", TM.wMonth);
	Time += Buffer;
	sprintf(Buffer, "%02d/", TM.wDay);
	Time += Buffer;
	sprintf(Buffer, "%04d/", TM.wYear);
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.wHour);
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.wMinute);
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.wSecond);
	Time += Buffer;
	sprintf(Buffer, "%03d", (TM.wMilliseconds));
	Time += Buffer;
}

void TimeStamp(CTime& TM, string& Time) {
	Time.erase();
	if (0 == TM.GetTime()) {
		Time = "00/00/0000/00:00:00:000";
		return;
	}
	char Buffer[18];
	sprintf(Buffer, "%02d/", TM.GetMonth());
	Time += Buffer;
	sprintf(Buffer, "%02d/", TM.GetDay());
	Time += Buffer;
	sprintf(Buffer, "%04d/", TM.GetYear());
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.GetHour());
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.GetMinute());
	Time += Buffer;
	sprintf(Buffer, "%02d:", TM.GetSecond());
	Time += Buffer;
	sprintf(Buffer, "%03d", 0);
	Time += Buffer;
}

void ConvertTime(string& TimeStamp, const CTime& Time) {
	char dtime[25];
	UINT nYear = Time.GetYear();
	if (0 == ::TimeFormat) {
		nYear %= 1000;
	}
	::ZeroMemory(dtime, sizeof dtime);
	if (0 == TimeFormat) {
		// 12 digit format
		sprintf(dtime, "%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(),
			Time.GetSecond());
	}
	else {
		sprintf(dtime, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, Time.GetMonth(), Time.GetDay(), Time.GetHour(),
			Time.GetMinute(), Time.GetSecond(), 0);
	}
	TimeStamp = dtime;
}