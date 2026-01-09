/*! \file CAG_Thread.cpp
	\brief implementation of the CAG_Thread class
	\author AMER GERZIC

	Implementation of the CAG_Thread class
*/
#include "stdafx.h"
#include "NThread.h"

NThread::NThread() : m_hThread(NULL),
m_hStopEvent(NULL),
m_hMainWnd(NULL),
m_dwThreadID(0) {}

NThread::~NThread() {
	StopThread();
}

BOOL NThread::StartThread(HWND hWnd) {
	// if thread already running do not start another one
	if (GetThreadStatus())
		return FALSE;

	// create stop event
	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hStopEvent == NULL)
		return FALSE;

	// create thread
	DWORD dwThreadId;
	m_hThread = ::CreateThread(NULL, 0, ThreadMain, this, 0, &dwThreadId);
	if (m_hThread == NULL)
		return FALSE;

	m_dwThreadID = dwThreadId;

	return TRUE;
}

BOOL NThread::StopThread() {
	// signal stop event and close all threads
	if (::SetEvent(m_hStopEvent)) {
		// if the Thread Main function is not overridden correctly
		// this will freeze the application
		::WaitForSingleObject(m_hThread, INFINITE);
		::CloseHandle(m_hThread);
		::CloseHandle(m_hStopEvent);
		return TRUE;
	}

	return FALSE;
}

BOOL NThread::GetThreadStatus() {
	// check is the thread still active
	DWORD dwExitCode = 0;
	GetExitCodeThread(m_hThread, &dwExitCode);
	return(dwExitCode == STILL_ACTIVE);
}