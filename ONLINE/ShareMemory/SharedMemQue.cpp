// SharedMemQue.cpp: implementation of the CSharedMemQue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Evt.h"
#include "SharedMemQue.h"
#include "..\GEM\GemFactory.h"

CShareQue CSharedMemQue::MemQue;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSharedMemQue::CSharedMemQue() {}

CSharedMemQue::~CSharedMemQue() {}

BOOL CSharedMemQue::CreateMemory(char*, int, int) {
	return TRUE;
}

BOOL CSharedMemQue::OpenMemory(char*) {
	return TRUE;
}

void CSharedMemQue::CloseMemory() {}

void CSharedMemQue::lock() {}

void CSharedMemQue::Release() {}

BOOL CSharedMemQue::WriteQue(CEvt* Evt) {
	if (m_pFactory) {
		Evt->pReport = m_pFactory->MakeReport(Evt, Evt->make, Evt->eventid);
		return MemQue.Add(Evt);
	}
	return FALSE;
}

CEvt* CSharedMemQue::ReadQue() {
	CEvt* p = MemQue.Remove();
	if (p) {
		return p;
	}
	return NULL;
}

int CSharedMemQue::GetSharedQcnt() {
	return MemQue.GetCount();
}