// ShareMemory.cpp: implementation of the CShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"
#include "ShareMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShareMemory::CShareMemory() {
	m_hSharedMemory = NULL;
	m_hSharedMemoryMutex = NULL;
	memset(sMutex0, 0, sizeof sMutex0);
}

CShareMemory::~CShareMemory() {
	if (m_hSharedMemory != NULL) {
		CloseHandle(m_hSharedMemory);
		m_hSharedMemory = NULL;
		CloseHandle(m_hSharedMemoryMutex);
		m_hSharedMemoryMutex = NULL;
	}
}

BOOL CShareMemory::CreateMemory(char* pName) {
	CString nError("");
	if ((strlen(pName) >= sizeof(sMutex0))) {
		ASSERT(0);
		return FALSE;
	}
	strscpy(sMutex0, sizeof(sMutex0), pName);
	strcat(sMutex0, "Mtx0");

	m_hSharedMemoryMutex = CreateMutex(NULL, FALSE, sMutex0);

	if ((m_hSharedMemory = ::CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		SHAREDMAXSIZE,
		pName)) == NULL) {
		nError.Format("Error : %s ShreadMemory Creation Fail", pName);
		AfxMessageBox(nError);
		CloseHandle(m_hSharedMemory);
		m_hSharedMemory = NULL;
		return FALSE;
	}
	if (m_hSharedMemory != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
		nError.Format("Error : %s ShreadMemory exist", pName);
		AfxMessageBox(nError);
		CloseHandle(m_hSharedMemory);
		m_hSharedMemory = NULL;
	}

	return TRUE;
}

BOOL CShareMemory::OpenMemory(char* pName) {
	BOOL bRet = TRUE;
	CString nError("");
	lock();
	if ((m_hSharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS,
		TRUE,
		pName)) == NULL) {
		nError.Format("Error : %s SharedMemory Open Error", pName);
		//AfxMessageBox( nError );
//		TRACE0(nError);
		CloseHandle(m_hSharedMemory);
		m_hSharedMemory = NULL;
		bRet = FALSE;
	}
	release();
	return bRet;
}

BOOL CShareMemory::WriteMemory(char* pBuf, long offset, long size) {
	BOOL bRet = TRUE;
	char* nHandle = NULL;
	lock();
	nHandle = (char*)MapViewOfFile(m_hSharedMemory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0);
	if (nHandle) {
		memcpy(&nHandle[offset], pBuf, size);
		UnmapViewOfFile(nHandle);
	}
	else {
		ASSERT(0);
	}
	release();
	return bRet;
}

BOOL CShareMemory::ReadMemory(char* pBuf, long offset, long size) {
	BOOL bRet = TRUE;
	char* nHandle = NULL;
	lock();
	nHandle = (char*)MapViewOfFile(m_hSharedMemory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0);
	if (!nHandle) return FALSE;

	memcpy(pBuf, &nHandle[offset], size);

	UnmapViewOfFile(nHandle);
	release();
	return bRet;
}

void CShareMemory::CloseSharedMemory() {
	CloseHandle(m_hSharedMemory);
	CloseHandle(m_hSharedMemoryMutex);
	m_hSharedMemory = NULL;
	m_hSharedMemoryMutex = NULL;
}

void CShareMemory::lock() {
	WaitForSingleObject(m_hSharedMemoryMutex, INFINITE);
}

void CShareMemory::release() {
	ReleaseMutex(m_hSharedMemoryMutex);
}

void CShareMemory::Clear(long size) {
	char* buf = new char[size];
	ReadMemory(buf, 0, size);
	ZeroMemory(buf, size);
	WriteMemory(buf, 0, size);
	delete[] buf;
}