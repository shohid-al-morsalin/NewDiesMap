#pragma once

// #include "ONLINE/general_utils.h"

class CLogMessage;

class CComLog {
public:
	// CTor DTor
	CComLog();
	virtual ~CComLog();
	// Read
	bool ReadMessage(CFile& LogFile, CLogMessage& Msg, int NsgNo, int Skip);
	bool ReadMessage(const CString LogFileName, CLogMessage& Msg, int NsgNo, int Skip);
	bool GetNumMessages(const CString LogFileName, UINT& nNumMsg);
	bool GetNumMessages(CFile& LogFile, UINT& nNumMsg);
	bool GetNumMessages(CFile& LogFile, UINT& nNumMsg, UINT& nSkip, SYSTEMTIME& Start, SYSTEMTIME& End);
	void GetMatchingFiles(SYSTEMTIME& Start, SYSTEMTIME& End, std::vector<std::string>& FileList);
	// Write
	BOOL Write_Log(BYTE* pData, int pCnt, MsgSubsystem System = ALL_SUB_SYSTEM, MsgType MsgTyp = SECS_MESSAGE, MsgUnit Unit = NA, char* Code = "Unk:0");
	// General Utils
	BOOL ReadLogTime(CString LogName, SYSTEMTIME& Start, SYSTEMTIME& End);
	bool GetNumMessages(const CString LogName, UINT& nNumMsg, UINT& nSkip, SYSTEMTIME& Start, SYSTEMTIME& End);
private:
	CCriticalSection Lock;
	void MakeLogFileName(CString& FileName);
	void FolderName(CString& FolderName);
	BOOL ReadLogEndTime(CFile& Log, SYSTEMTIME& Tm);
	BOOL WriteStartDateToFile(CFile& File, const SYSTEMTIME& Tm);
	BOOL WriteEndDateToFile(CFile& File, const SYSTEMTIME& Tm);
	BOOL WriteDateToFile(CFile& File, const SYSTEMTIME& Tm);
	BOOL WriteMsgToFile(CFile& File, const SYSTEMTIME& Tm, const BYTE* pData, UINT DataLen,
		MsgSubsystem System, MsgType MsgTyp, MsgUnit Unit, char* Key);
	BOOL ReadLogStartTime(CFile& Log, SYSTEMTIME& Tm);
	BOOL ReadTime(CFile& Log, SYSTEMTIME& Tm);
};

extern CComLog m_Log;
