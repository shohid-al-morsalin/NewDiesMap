// SpoolMan.cpp: implementation of the CSpoolMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include "SpoolMan.h"
#include "..\inc\define.h"
#include "DosName.h"

//////////////////////////////////////////////////////////////////////
// Static definitions
//////////////////////////////////////////////////////////////////////
CSpoolMan* CSpoolMan::m_pSpool = NULL;
StreamFunctMap CSpoolMan::SpooledMap;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CSpoolMan, CObject, 1)

CSpoolMan::CSpoolMan() {
	MaxSpoolTransmit = 100;
	SpoolCountActual = 0;
	SpoolCountTotal = 0;
	OverWriteSpool = FALSE;
	bEnabled = FALSE;
	bSpoolFull = FALSE;
	bSpoolStarted = FALSE;
	SpoolFullTime = COleDateTime::GetCurrentTime(); // Invalid value
	SpoolStartTime = COleDateTime::GetCurrentTime(); // Invalid value
}

CSpoolMan::~CSpoolMan() {}

CSpoolMan* CSpoolMan::GetSpool() {
	if (NULL == m_pSpool) {
		m_pSpool = new CSpoolMan;
		atexit(DelSpMan);
	}
	return m_pSpool;
}

BOOL CSpoolMan::IsEnabled() {
	return /*GetSpool()->bEnabled*/(SpooledMap.size() > 0);
}

UINT CSpoolMan::TransmitAmount() {
	return CSpoolMan::GetSpool()->MaxSpoolTransmit;
}

UINT CSpoolMan::ActualSpooled() {
	return CSpoolMan::GetSpool()->SpoolCountActual;
}

UINT CSpoolMan::SpooledSoFar() {
	return CSpoolMan::GetSpool()->SpoolCountTotal;
}

void CSpoolMan::SerializeVar(CArchive& ar) {
	//	ar<<
		//Serialize(ar);
}

void CSpoolMan::SpoolStreamFunct(short Stream, vector<int>& FunctList) {
	if (1 == Stream) {
		return;
	}
	StreamFunctMap::iterator theIterator = SpooledMap.find(Stream);
	BOOL bFunctFound = FALSE;
	if (theIterator == SpooledMap.end()) {   // Stream not present so add stream
		//FunctVec.push_back(Function);
		SpooledMap[Stream] = FunctList;
	}
	else {
		theIterator->second = FunctList;
	}
	// Should update the file data also
}

void CSpoolMan::RemoveAllStreamFunct(void) {
	SpooledMap.clear();
	//// Should update the file data also
}
BOOL CSpoolMan::RemoveStream(short Stream) {
	if (1 == Stream) {
		return FALSE;
	}
	StreamFunctMap::iterator theIterator = SpooledMap.find(Stream);
	if (theIterator == SpooledMap.end()) {// Non existant stream
		return FALSE;
	}
	else {
		SpooledMap.erase(Stream);
		return TRUE;
	}
	// Should update the file data also
}
// Assumption : Enabling all functions for a steam means add the stream too.
BOOL CSpoolMan::EnableAllFunctions(short Stream) {
	if (1 == Stream) {
		return FALSE;
	}
	StreamFunctMap::iterator theIterator = SpooledMap.find(Stream);
	vector<int> FunctVec;
	FunctVec.push_back(-1);					// Enable all functions
	if (theIterator == SpooledMap.end()) {	// Non existant stream
		SpooledMap[Stream] = FunctVec;		// Add whole map entry
		return TRUE;
	}
	else {
		theIterator->second = FunctVec;		// Modify the function signature
		return TRUE;
	}
	// Should update the file data also
}
BOOL CSpoolMan::CanSpool(short Stream, short Function) {
	if (1 == Stream || (!(Function % 2))) { // Invalid stream or function
		return FALSE;
	}
	if (SpooledMap.find(-1) != SpooledMap.end()) { // All stream and functions are enabled
		return TRUE;
	}
	StreamFunctMap::iterator theIterator = SpooledMap.find(Stream);
	if (theIterator == SpooledMap.end()) {	// Non existant stream
		return FALSE;
	}
	else {
		if (-1 == theIterator->second[0]) {
			return TRUE;
		}
		else {
			for (UINT nIndex = 0; nIndex < theIterator->second.size(); nIndex++) {
				if (Function == theIterator->second[nIndex]) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

/**
		Writes the current state of the variables to the file

**/
BOOL CSpoolMan::MakeNewSpoolData() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];

	for (int nIndex = 0; nIndex < (sizeof(Clock)); nIndex++) {
		Clock[nIndex] = '0';
	}
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	_makepath(filename, drive, dir, "Spool", "var");

	UINT nNumStream = SpooledMap.size();
	StreamFunctMap::iterator theIterator = SpooledMap.begin();
	UINT nNumFunctions = 0;
	int FunctionCode = 0;
	int StreamCode = 0;

	//int hFile =	 _open(filename,_O_CREAT|_O_BINARY|_O_RDWR,_S_IWRITE);
	int hFile = _creat(filename, _S_IREAD | _S_IWRITE);
	if (hFile != -1) {
		_close(hFile);
	}
	hFile = _open(filename, _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE);
	if (hFile != -1) {
		// Write the GEM Variables
		// SpoolCountActual
		_write(hFile, &(SpoolCountActual), sizeof(SpoolCountActual));
		// SpoolCountTotal
		_write(hFile, &(SpoolCountTotal), sizeof(SpoolCountTotal));
		// OverWriteSpool
		_write(hFile, &(OverWriteSpool), sizeof(OverWriteSpool));

		// Store Boolean variables
		_write(hFile, &(bSpoolStarted), sizeof(bSpoolStarted));
		// SpoolStartTime
		GetSpoolStartTime(Clock);
		_write(hFile, (Clock), sizeof(Clock));	//
		_write(hFile, &(bSpoolFull), sizeof(bSpoolFull));
		GetSpoolFullTime(Clock);
		// SpoolFullTime
		_write(hFile, (Clock), sizeof(Clock));	// No value yet
		// .......Gem variables done

		// Store the stream and functions
		// Total number of streams just for cross checking
		_write(hFile, &nNumStream, sizeof(nNumStream));
		//Write
		for (; theIterator != SpooledMap.end(); theIterator++) {
			// Write the stream value first
			StreamCode = theIterator->first;
			_write(hFile, &StreamCode, sizeof(StreamCode));

			// Get the number of functions for this stream
			nNumFunctions = theIterator->second.size();
			_write(hFile, &nNumFunctions, sizeof(nNumFunctions));

			// Store the number of function in the file
			for (UINT nTemp = 0; nTemp < nNumFunctions; nTemp++) {
				// store the functions in the file
				FunctionCode = theIterator->second[nTemp];
				_write(hFile, &FunctionCode, sizeof(FunctionCode));
			}
		}
		_close(hFile);
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL CSpoolMan::ReadSpoolData() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	char sTemp[16];
	for (int nIndex = 0; nIndex < (sizeof(Clock)); nIndex++) {
		Clock[nIndex] = '0';
	}
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	_makepath(filename, drive, dir, "Spool", "var");

	UINT nNumStream;//= SpooledMap.size();
	UINT nNumFunctions = 0;
	int FunctionCode = 0;
	int StreamCode = 0;
	UINT nYear, nMonth, nDay, nHour, nMin, nSec;
	int hFile = _open(filename, _O_RDONLY);
	vector<int> FunctionVec;
	if (hFile != -1) {
		// Read the GEM Variables
		// SpoolCountActual
		_read(hFile, &(SpoolCountActual), sizeof(SpoolCountActual));
		// SpoolCountTotal
		_read(hFile, &(SpoolCountTotal), sizeof(SpoolCountTotal));
		// OverWriteSpool
		_read(hFile, &(OverWriteSpool), sizeof(OverWriteSpool));
		// Store Boolean variables
		_read(hFile, &(bSpoolStarted), sizeof(bSpoolStarted));
		// SpoolStartTime
		//GetSpoolStartTime(Clock);
		_read(hFile, (Clock), sizeof(Clock));	//
		strncpy(sTemp, Clock, 4);			// Year
		nYear = atoi(sTemp);
		strncpy(sTemp, &Clock[3], 2);		// Month
		nMonth = atoi(sTemp);
		strncpy(sTemp, &Clock[5], 2);		// Date
		nDay = atoi(sTemp);
		strncpy(sTemp, &Clock[7], 4);		// Hour
		nHour = atoi(sTemp);
		strncpy(sTemp, &Clock[9], 2);		// Min
		nMin = atoi(sTemp);
		strncpy(sTemp, &Clock[11], 2);		// sec
		nSec = atoi(sTemp);
		SetSpoolStartTime(nYear, nMonth, nDay, nHour, nMin, nSec);

		_read(hFile, &(bSpoolFull), sizeof(bSpoolFull));

		// SpoolFullTime
		_read(hFile, (Clock), sizeof(Clock));	//
		strncpy(sTemp, Clock, 4);			// Year
		nYear = atoi(sTemp);
		strncpy(sTemp, &Clock[3], 2);		// Month
		nMonth = atoi(sTemp);
		strncpy(sTemp, &Clock[5], 2);		// Date
		nDay = atoi(sTemp);
		strncpy(sTemp, &Clock[7], 4);		// Hour
		nHour = atoi(sTemp);
		strncpy(sTemp, &Clock[9], 2);		// Min
		nMin = atoi(sTemp);
		strncpy(sTemp, &Clock[11], 2);	// sec
		nSec = atoi(sTemp);
		SetSpoolStartTime(nYear, nMonth, nDay, nHour, nMin, nSec);

		// .......Gem variables done

		// Store the stream and functions
		// Total number of streams just for cross checking
		_read(hFile, &nNumStream, sizeof(nNumStream));
		//Read
		for (UINT nTemp1 = 0; nTemp1 < nNumStream; nTemp1++) {
			// Read the stream value first
			//StreamCode = theIterator->first;
			_read(hFile, &StreamCode, sizeof(StreamCode));

			// Get the number of functions for this stream
			//nNumFunctions = theIterator->second.size();
			_read(hFile, &nNumFunctions, sizeof(nNumFunctions));

			// Store the number of function in the file
			for (UINT nTemp2 = 0; nTemp2 < nNumFunctions; nTemp2++) {
				// store the functions in the file
				//FunctionCode = theIterator->second[nIndex];
				_read(hFile, &FunctionCode, sizeof(FunctionCode));
				FunctionVec.push_back(FunctionCode);
			}
			SpooledMap[StreamCode] = FunctionVec;
			FunctionVec.clear();
		}
		_close(hFile);
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL CSpoolMan::MakeNewSpoolLog() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);

	// Now the message file itself
	UINT nNumMsg = 0;		// Number of messages
	UINT nSkipCount = 0;	// Number of deleted (so called )messages
	_makepath(filename, drive, dir, "Spool", "log");
	//int hFile =	 _open(filename,_O_CREAT|_O_BINARY|_O_RDWR,_S_IWRITE);
	int hFile = _creat(filename, _S_IREAD | _S_IWRITE);
	if (hFile != -1) {
		_close(hFile);
	}
	hFile = _open(filename, _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE);
	if (hFile != -1) {
		// Store the number of messages
		_write(hFile, &nNumMsg, sizeof(nNumMsg));
		// Store skip count
		_write(hFile, &nSkipCount, sizeof(nSkipCount));
		_close(hFile);
		// reset the variables to zero value
		// Assumption : both files are enerated together
/*
		GetSpool()->bSpoolFull = FALSE;
		GetSpool()->bSpoolStarted = FALSE;
		GetSpool()->MaxSpoolItems = 0;
		GetSpool()->SpoolCountActual = 0;
		GetSpool()->MakeNewSpoolData();*/
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL CSpoolMan::WriteMessage(void* pData, UINT nSize) {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	for (int nIndex = 0; nIndex < (sizeof(Clock)); nIndex++) {
		Clock[nIndex] = '0';
	}
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	if (TRUE == GetSpool()->bSpoolFull) {
		// Increase SpoolCountTotal
		GetSpool()->SpoolCountTotal++;
		return FALSE;
	}
	// Now the message file itself
	UINT nNumMsg = 0;		// Number of messages
	_makepath(filename, drive, dir, "Spool", "log");
	BOOL bWritten = FALSE;
	int pos = 0;
	int hFile = _open(filename, _O_BINARY | _O_RDWR);
	if (hFile != -1) {
		pos = _lseek(hFile, 0L, SEEK_END);
		if (-1 != pos) {
			// Write the size first
			_write(hFile, &nSize, sizeof(nSize));
			// Followed by data itself
			bWritten = (nSize == _write(hFile, pData, nSize));
		}
		if (bWritten) {
			pos = _lseek(hFile, 0L, SEEK_SET);
			if (-1 != pos) {     // Correct position
				_read(hFile, &nNumMsg, sizeof(nNumMsg));
				if (0 == nNumMsg) {
					// Set spool start time to current time
					// ... or should it be done at the time of receipt of the message
					this->SpoolStartTime = COleDateTime::GetCurrentTime();
					this->bSpoolStarted = TRUE;
				}
				nNumMsg++;
				pos = _lseek(hFile, 0L, SEEK_SET);
				if (-1 != pos) {
					_write(hFile, &nNumMsg, sizeof(nNumMsg));
				}
				GetSpool()->SpoolCountActual++;
				GetSpool()->SpoolCountTotal++;
			}
		}
		else {
			// Some error in writing the file. Probably the log file is full
			this->SpoolFullTime = COleDateTime::GetCurrentTime();
			// Set Spool full time to the current value
			GetSpool()->bSpoolFull = TRUE;
		}
		_close(hFile);
		GetSpool()->UpdateMsgCount();
	}
	else {
		return FALSE;
	}
	return TRUE;
}
void CSpoolMan::UpdateMsgCount() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	for (int nIndex = 0; nIndex < (sizeof(Clock)); nIndex++) {
		Clock[nIndex] = '0';
	}
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	_makepath(filename, drive, dir, "Spool", "var");

	UINT nNumStream = SpooledMap.size();
	StreamFunctMap::iterator theIterator = SpooledMap.begin();
	UINT nNumFunctions = 0;
	int FunctionCode = 0;
	int StreamCode = 0;

	int hFile = _open(filename, _O_BINARY | _O_RDWR);
	if (hFile != -1) {
		// SpoolCountActual
		_write(hFile, &(SpoolCountActual), sizeof(SpoolCountActual));
		// SpoolCountTotal
		_write(hFile, &(SpoolCountTotal), sizeof(SpoolCountTotal));
		// OverWriteSpool

		_close(hFile);
	}
	else {
	}
	return;
}

BOOL CSpoolMan::GetSpoolStartTime(char* pData) {
	if (bSpoolStarted) {
		COleDateTime startTime = SpoolStartTime;
		sprintf(pData, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", startTime.GetYear(), startTime.GetMonth(),
			startTime.GetDay(), startTime.GetHour(),
			startTime.GetMinute(), startTime.GetSecond(),
			0);
		return TRUE;
	}
	else {
		//sprintf(pData,"%16c",66);
		memset(pData, ' ', 16);
		return FALSE;
	}
}

BOOL CSpoolMan::GetSpoolFullTime(char* pData) {
	if (bSpoolFull) {
		COleDateTime FullTime = SpoolFullTime;
		sprintf(pData, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", FullTime.GetYear(), FullTime.GetMonth(),
			FullTime.GetDay(), FullTime.GetHour(),
			FullTime.GetMinute(), FullTime.GetSecond(),
			0);
		return TRUE;
	}
	else {
		//sprintf(pData,"%16c",65);
		memset(pData, ' ', 16);
		return FALSE;
	}
}

int	CSpoolMan::SetSpoolStartTime(int nYear, int Month, int Day, int Hour, int Min, int Sec) {
	return SpoolStartTime.SetDateTime(nYear, Month, Day, Hour, Min, Sec);
}
int	CSpoolMan::SetSpoolFullTime(int nYear, int Month, int Day, int Hour, int Min, int Sec) {
	return SpoolFullTime.SetDateTime(nYear, Month, Day, Hour, Min, Sec);
}

int	CSpoolMan::GetNextSpooledMessage(void* pData) {
	// Open the file
	CDosName* pDosName = new CDosName;
	if (!pDosName) {
		ASSERT(0);
		return -1;
	}

	// Check for valid pointer
	CString str;
	int RetVal = 0;
	// 	char filename[_MAX_PATH];
	// 	char drive[_MAX_DRIVE];
	// 	char dir[_MAX_DIR];
	// 	char fname[_MAX_FNAME];
	// 	char ext[_MAX_EXT];
	BYTE* pBuf/*[MAX_RECVSTRING]*/ = new BYTE[MAX_RECVSTRING];
	if (!pBuf) {
		ASSERT(0);
		return -2;
	}
	// Generate the filename
	GetModuleFileName(NULL, pDosName->filename, _MAX_PATH);
	_splitpath(pDosName->filename, pDosName->drive, pDosName->dir, pDosName->fname, pDosName->ext);
	// Now the message file itself
	_makepath(pDosName->filename, pDosName->drive, pDosName->dir, "Spool", "log");

	UINT nNumMsg = 0;	// Number of messages
	UINT nSkipCount = 0;	// Number of deleted (so called )messages
	UINT nSize = 0;	// size of each message
	UINT ToSkip = 0;	// size of each message

	int hFile = _open(pDosName->filename, _O_BINARY | _O_RDWR);
	if (hFile != -1) {// File opened successfully
		// Get the skipped message
		_read(hFile, &nNumMsg, sizeof(nNumMsg));
		_read(hFile, &nSkipCount, sizeof(nSkipCount));
		if (nNumMsg == nSkipCount) {
			// ????
			bSpoolStarted = FALSE;
			RetVal = -1;
		}
		else {
			// Skip the messages
			for (UINT nIndex = 0; nIndex < nSkipCount; nIndex++) {
				//_read(hFile,&ToSkip,sizeof(ToSkip));
				_read(hFile, &nSize, sizeof(nSize));
				//ASSERT(1 ==ToSkip);                 // Additional safety for transmitted message
				_read(hFile, pBuf, nSize);
			}
			ASSERT(0 == ToSkip);
			//_read(hFile,Buf,nSize);
			// get the next available message
			// get length
			//_read(hFile,&ToSkip,sizeof(ToSkip));
			_read(hFile, &nSize, sizeof(nSize));
			_read(hFile, pBuf, nSize);
			// Copy to the buffer
			memcpy(pData, pBuf, nSize);
			//ToSkip = 0;
			//_lseek(hFile,sizeof(nSize) +sizeof(ToSkip)+ nSize ,SEEK_CUR );
			//_write(hFile,&ToSkip,sizeof(ToSkip));
			// Go to the beginning of the file & write the new SkipCount
			_lseek(hFile, /*sizeof(nNumMsg)*/0L, SEEK_SET);
			_read(hFile, &nNumMsg, sizeof(nNumMsg));
			nSkipCount++;
			_write(hFile, &nSkipCount, sizeof(nSkipCount));
			// Hopefully everything is OK !
			// Reduce spool count Actual;
			this->SpoolCountActual--;
			this->SpoolCountTotal--;
			this->UpdateMsgCount();
			RetVal = nSkipCount;
		}
		_close(hFile);
	}
	// 	else
	// 	{
	// 		return RetVal;
	// 	}
	delete pDosName;
	delete[] pBuf;
	return RetVal;
}
#if 0
void CSpoolMan::SaveVariables() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	// Now the message file itself
	_makepath(filename, drive, dir, "Spool", "log");

	CFile theFile;
	theFile.Open(filename, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&theFile, CArchive::store);
	//SerializeVar(archive);
	archive << this->bEnabled;
	archive << this->bSpoolFull;
	archive.Close();
	theFile.Close();
}
void CSpoolMan::ReadVariables() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	// Now the message file itself
	_makepath(filename, drive, dir, "Spool", "log");

	CFile theFile;
	theFile.Open(filename, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&theFile, CArchive::store);
	//SerializeVar(archive);
	archive << this->bEnabled;
	archive << this->bSpoolFull;
	archive.Close();
	theFile.Close();
}

void CSpoolMan::SaveMessage() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	// Now the message file itself
	_makepath(filename, drive, dir, "Spool", "log");

	CFile theFile;
	theFile.Open(filename, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&theFile, CArchive::store);
	//SerializeVar(archive);
	archive << this->bEnabled;
	archive << this->bSpoolFull;
	archive.Close();
	theFile.Close();
}

void CSpoolMan::ReadMessage(void* pData) {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Clock[16];
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);
	// Now the message file itself
	_makepath(filename, drive, dir, "Spool", "log");

	CFile theFile;
	theFile.Open(filename, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&theFile, CArchive::store);
	//SerializeVar(archive);
	archive << this->bEnabled;
	archive << this->bSpoolFull;
	archive.Close();
	theFile.Close();
}
#endif

BOOL CSpoolMan::EnableAllStreamFunctions() {
	vector<int> FunctVec;
	// Clear all
	SpooledMap.clear();
	FunctVec.push_back(-1);
	SpooledMap[-1] = FunctVec;
	// Update the files now
	GetSpool()->MakeNewSpoolData();
	return TRUE;
}

BOOL CSpoolMan::InitSpool() {
	CSpoolMan* pSpool = CSpoolMan::GetSpool();
	BOOL res1, res2;
	pSpool->SpoolCountActual = 0;
	pSpool->SpoolCountTotal = 0;
	res1 = pSpool->MakeNewSpoolData();
	res2 = pSpool->MakeNewSpoolLog();
	pSpool->ReadSpoolData();
	pSpool->UpdateMsgCount();
	return res1 | res2;
}

BOOL CSpoolMan::UndoLastTransmit() {
	CString str;
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	// Generate the filename
	GetModuleFileName(NULL, filename, _MAX_PATH);
	_splitpath(filename, drive, dir, fname, ext);

	// Now the message file itself
	UINT nNumMsg = 0;		// Number of messages
	UINT nSkipCount = 0;	// Number of deleted (so called )messages
	_makepath(filename, drive, dir, "Spool", "log");
	int hFile = _open(filename, _O_BINARY | _O_RDWR);
	if (hFile != -1) {
		// Store the number of messages
		_read(hFile, &nNumMsg, sizeof(nNumMsg));
		// Store skip count
		_read(hFile, &nSkipCount, sizeof(nSkipCount));
		nSkipCount--;
		_lseek(hFile, sizeof(nNumMsg), SEEK_SET);
		_write(hFile, &nSkipCount, sizeof(nSkipCount));
		_close(hFile);
		// reset the variables to zero value
		// Assumption : both files are enerated together
/*
		GetSpool()->bSpoolFull = FALSE;
		GetSpool()->bSpoolStarted = FALSE;
		GetSpool()->MaxSpoolItems = 0;
		GetSpool()->SpoolCountActual = 0;
		GetSpool()->MakeNewSpoolData();*/
	}
	else {
		return FALSE;
	}
	return TRUE;
}
void DelSpMan() {
	delete CSpoolMan::GetSpool();
}