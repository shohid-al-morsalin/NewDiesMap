#include "stdafx.h"

#include "LogMessage.h"
#include "GlobalDeviceContainer.h"

#include "ComLog.h"

CComLog	m_Log;

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComLog::CComLog() {}

CComLog::~CComLog() {}

BOOL CComLog::Write_Log(BYTE* pData, int DataLen, MsgSubsystem System, MsgType MsgTyp, MsgUnit Unit, char* Key) {
	if (pGDev) {
		//pGDev->SECSContent((char *)pData,DataLen); //  [9/10/2007 LYF]
	}
	return TRUE;
}

BOOL CComLog::WriteStartDateToFile(CFile& LogFile, const SYSTEMTIME& Time) {
	LogFile.Seek(0, CFile::begin);
	return WriteDateToFile(LogFile, Time);
}

BOOL CComLog::WriteEndDateToFile(CFile& LogFile, const SYSTEMTIME& Time) {
	LogFile.Seek(14, CFile::begin);
	return WriteDateToFile(LogFile, Time);
}

BOOL  CComLog::WriteMsgToFile(CFile& File, const SYSTEMTIME& Tm, const BYTE* pData, UINT DataLen, MsgSubsystem System, MsgType MsgTyp, MsgUnit Unit, char* Key) {
	UINT nNumMsg;
	File.Seek(28L, CFile::begin);
	File.Read(&nNumMsg, 4);
	nNumMsg++;
	File.Seek(-4, CFile::current);
	File.Write(&nNumMsg, 4);
	File.SeekToEnd();
	WriteDateToFile(File, Tm);
	WORD Temp;
	Temp = System;
	File.Write(&Temp, 2);
	Temp = MsgTyp;
	File.Write(&Temp, 2);
	Temp = Unit;
	File.Write(&Temp, 2);
	Temp = WORD(strlen(Key));
	File.Write(&Temp, 2);
	File.Write(Key, Temp);
	File.Write(&DataLen, 4);
	File.Write(pData, DataLen);

	return File.GetPosition();
}

BOOL CComLog::WriteDateToFile(CFile& LogFile, const SYSTEMTIME& Time) {
	WORD Temp;
	Temp = Time.wYear;
	LogFile.Write(&Temp, 2);
	Temp = Time.wMonth;
	LogFile.Write(&Temp, 2);
	Temp = Time.wDay;
	LogFile.Write(&Temp, 2);
	Temp = Time.wHour;
	LogFile.Write(&Temp, 2);
	Temp = Time.wMinute;
	LogFile.Write(&Temp, 2);
	Temp = Time.wSecond;
	LogFile.Write(&Temp, 2);
	Temp = Time.wMilliseconds;
	LogFile.Write(&Temp, 2);
	return LogFile.GetPosition();
}

BOOL CComLog::ReadLogStartTime(CFile& Log, SYSTEMTIME& Time) {
	if (Log.m_hFile == /*(UINT)*/CFile::hFileNull) {
		return FALSE;
	}
	Log.Seek(0L, CFile::begin);
	return ReadTime(Log, Time);
}

BOOL CComLog::ReadLogEndTime(CFile& Log, SYSTEMTIME& Time) {
	Log.Seek(14L, CFile::begin);
	return ReadTime(Log, Time);
}

BOOL CComLog::ReadTime(CFile& Log, SYSTEMTIME& Time) {
	if (Log.m_hFile == /*(UINT)*/CFile::hFileNull) {
		return FALSE;
	}
	WORD Temp;
	Log.Read(&Temp, 2);
	Time.wYear = Temp;
	Log.Read(&Temp, 2);
	Time.wMonth = Temp;
	Log.Read(&Temp, 2);
	Time.wDay = Temp;
	Log.Read(&Temp, 2);
	Time.wHour = Temp;
	Log.Read(&Temp, 2);
	Time.wMinute = Temp;
	Log.Read(&Temp, 2);
	Time.wSecond = Temp;
	Log.Read(&Temp, 2);
	Time.wMilliseconds = Temp;
	return Log.GetPosition();
}

void CComLog::MakeLogFileName(CString& FileName) {
	CString Folder;
	FolderName(Folder);
	FileName = Folder + FileName + ".LOG";
}

void CComLog::FolderName(CString& FolderName) {
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[_MAX_PATH];

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);
	FolderName.Empty();
	FolderName = drive;
	FolderName += dir;
	FolderName += "ONL\\";
}

void CComLog::GetMatchingFiles(SYSTEMTIME& Start, SYSTEMTIME& End, vector<string>& FileList) {
	string FileName;
	FileList.clear();
	vector<SYSTEMTIME> FileTimes;
	WIN32_FIND_DATA FindFileData;
	SYSTEMTIME STime, ETime;
	CString strTemp = "*";
	MakeLogFileName(strTemp);
	UINT nMsgCnt, nSkip;
	// Get names of all files
	HANDLE hFind;
	hFind = ::FindFirstFile(strTemp, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		//TRACE ("Invalid File Handle. Get Last Error reports %d\n", GetLastError ());
		return;
	}
	FileName = FindFileData.cFileName;
	CFile LogFile;
	CFileException Ex;
	CString FullPath;
	FolderName(FullPath);
	strTemp = FullPath + FindFileData.cFileName;
	vector<string> CandidateFiles;
	if (LogFile.Open(strTemp, CFile::modeRead, &Ex)) {
		ReadLogStartTime(LogFile, STime);
		ReadLogEndTime(LogFile, ETime);
		if ((Start < STime && ETime < End) ||
			(ETime > Start && ETime < End) ||
			(Start > STime && Start < ETime)) {
			CandidateFiles.push_back(FindFileData.cFileName);
		}
		LogFile.Close();
	}
	while (TRUE == ::FindNextFile(hFind, &FindFileData)) {
		strTemp = FullPath + FindFileData.cFileName;
		if (LogFile.Open(strTemp, CFile::modeRead, &Ex)) {
			ReadLogStartTime(LogFile, STime);
			ReadLogEndTime(LogFile, ETime);
			if ((Start < STime && ETime < End) ||
				(ETime > Start && ETime < End) ||
				(Start > STime && Start < ETime)) {
				CandidateFiles.push_back(FindFileData.cFileName);
			}
			LogFile.Close();
		}
	}
	::FindClose(hFind);
	for (int nIndex = 0; nIndex<int(CandidateFiles.size()); nIndex++) {
		strTemp = FullPath + CandidateFiles[nIndex].c_str();
		if (LogFile.Open(strTemp, CFile::modeRead, &Ex)) {
			ReadLogStartTime(LogFile, STime);
			nMsgCnt = nSkip = 0;
			GetNumMessages(LogFile, nMsgCnt, nSkip, Start, End);
			if (nMsgCnt) {
				FileList.push_back(CandidateFiles[nIndex]);
				FileTimes.push_back(STime);
			}
			LogFile.Close();
		}
	}
	int nMaxFiles, nCycle;
	nMaxFiles = nCycle = FileList.size();
	string TempFName;
	for (int nOut = 0, nIn = 0; nOut < (nMaxFiles - 1); nOut++) {
		for (nIn = 0; nIn < (nCycle - 1); nIn++) {
			if (FileTimes[nIn] > FileTimes[nIn + 1]) {
				STime = FileTimes[nIn + 1];
				TempFName = FileList[nIn + 1];
				FileTimes[nIn + 1] = FileTimes[nIn];
				FileList[nIn + 1] = FileList[nIn];
				FileTimes[nIn] = STime;
				FileList[nIn] = TempFName;
			}
		}
		nCycle--;
	}
}

bool CComLog::GetNumMessages(CFile& LogFile, UINT& nNumMsg) {
	LogFile.Seek(28L, CFile::begin);
	if (LogFile.Read(&nNumMsg, 4)) {
		return true;
	}
	return false;
}

bool CComLog::GetNumMessages(CFile& LogFile, UINT& nNumMsg, UINT& nSkipMsg, SYSTEMTIME& Start, SYSTEMTIME& End) {
	short Temp; UINT MsgLen;
	SYSTEMTIME STime, ETime, CurTime;
	ReadLogStartTime(LogFile, STime);
	ReadLogEndTime(LogFile, ETime);
	int nIndex;
	int MsgCnt = 0;
	int SkipCount = 0, MsgCount = 0;
	LogFile.Seek(28L, CFile::begin);
	if (!LogFile.Read(&MsgCnt, 4)) {
		return false;
	}
	int nNum = 0;
	if ((Start < STime) && (ETime < End)) {
		nNumMsg = MsgCnt;
		return true;
	}
	else if ((End < STime) || (ETime < Start)) {
		nNumMsg = nSkipMsg = 0;
		return false;
	}
	LogFile.Seek(34, CFile::begin);
	for (nIndex = 0; nIndex < MsgCnt; nIndex++) {
		ReadTime(LogFile, CurTime);
		LogFile.Seek(6L, CFile::current);
		LogFile.Read(&Temp, 2);
		LogFile.Seek(Temp, CFile::current);
		LogFile.Read(&MsgLen, 4);
		LogFile.Seek(MsgLen, CFile::current);
		if (CurTime < Start) {
			nSkipMsg++;
			continue;
		}
		if (CurTime > End) {
			break;
		}
		nNumMsg++;
	}
	return true;
}

bool CComLog::ReadMessage(CFile& LogFile, CLogMessage& Mesg, int MsgNo, int Skip) {
	CSingleLock Sync(&Lock);
	Sync.Lock();
	UINT nNumMsg = 0;
	GetNumMessages(LogFile, nNumMsg);
	if (nNumMsg < UINT(MsgNo + Skip)) {
		return false;
	}
	UINT MsgLen = 0;
	short Temp;
	char Buf[126];
	LogFile.Seek(34, CFile::begin);
	SYSTEMTIME Test;
	for (int nIndex = 0; nIndex < (MsgNo + Skip - 1); nIndex++) {
		ReadTime(LogFile, Test);
		LogFile.Seek(6L, CFile::current);
		LogFile.Read(&Temp, 2);
		LogFile.Read(Buf, Temp);
		LogFile.Read(&MsgLen, 4);
		LogFile.Seek(MsgLen, CFile::current);
	}
	ReadTime(LogFile, Mesg.m_MsgTime);
	LogFile.Read(&Temp, 2);
	Mesg.m_SubSystem = (MsgSubsystem)Temp;
	LogFile.Read(&Temp, 2);
	Mesg.m_Type = (MsgType)Temp;
	LogFile.Read(&Temp, 2);
	Mesg.m_Unit = (MsgUnit)Temp;
	LogFile.Read(&Temp, 2);
	LogFile.Read(Buf, Temp);
	Buf[Temp] = 0;
	Mesg.m_Key = Buf;
	LogFile.Read(&Mesg.m_MsgLen, 4);
	if (Mesg.m_Data) delete[]Mesg.m_Data;
	Mesg.m_Data = NULL;
	Mesg.m_Data = new char[Mesg.m_MsgLen + 1];
	Mesg.m_Data[Mesg.m_MsgLen] = 0;
	LogFile.Read(Mesg.m_Data, Mesg.m_MsgLen);
	Sync.Unlock();
	return true;
}

bool CComLog::ReadMessage(const CString LogName, CLogMessage& Msg, int MsgNo, int Skip) {
	CFile Log;
	CFileException Ex;
	CString Folder;
	FolderName(Folder);
	Folder += LogName;
	bool bRes = false;
	if (Log.Open(Folder, CFile::modeRead, &Ex)) {
		bRes = ReadMessage(Log, Msg, MsgNo, Skip);
		Log.Close();
	}
	return bRes;
}

bool CComLog::GetNumMessages(const CString LogName, UINT& nNumMsg) {
	CFile Log;
	CFileException Ex;
	CString Folder;
	FolderName(Folder);
	Folder += LogName;
	bool bRes = false;
	if (Log.Open(Folder, CFile::modeRead, &Ex)) {
		bRes = GetNumMessages(Log, nNumMsg);
		Log.Close();
	}
	return bRes;
}

bool CComLog::GetNumMessages(const CString LogName, UINT& nNumMsg, UINT& nSkip, SYSTEMTIME& Start, SYSTEMTIME& End) {
	CFile Log;
	CFileException Ex;
	CString Folder;
	FolderName(Folder);
	Folder += LogName;
	nNumMsg = nSkip = 0;
	bool bRes = false;
	if (Log.Open(Folder, CFile::modeRead, &Ex)) {
		bRes = GetNumMessages(Log, nNumMsg, nSkip, Start, End);
		Log.Close();
	}
	return bRes;
}

BOOL CComLog::ReadLogTime(CString LogName, SYSTEMTIME& Start, SYSTEMTIME& End) {
	CFile Log;
	CFileException Ex;
	CString Folder;
	FolderName(Folder);
	Folder += LogName;
	if (Log.Open(Folder, CFile::modeRead, &Ex)) {
		if ((!ReadLogStartTime(Log, Start)) ||
			(!ReadLogEndTime(Log, End))) {
			return FALSE;
		}
		Log.Close();
	}
	return TRUE;
}