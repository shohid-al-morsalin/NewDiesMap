// FFUThread.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "FFUThread.h"

#include "413App.h"
#include "GlobalUIContainer.h"
#include "..\413\MotorS2.h"

// #include "FFUFailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFFUThread

IMPLEMENT_DYNCREATE(CFFUThread, CWinThread)

float FFUPressNow = 1.2f;

CFFUThread::CFFUThread() {
	bRun = FALSE;
	bStop = FALSE;
}

CFFUThread::~CFFUThread() {}

BOOL CFFUThread::InitInstance() {
	int i, j;
	long dly = 120000;
	ULONGLONG tick = GetTickCount64();
	bRun = TRUE;
	while (bRun) {
		if (p413App->IsOperating()) {
			// Do not check FFU if tool is operating [4/25/2013 Administrator]
			for (i = 0; i < 60; i++) {
				if (!bRun) {
					break;
				}
				for (j = 0; j < 500; j++) {
					if (!bRun) {
						break;
					}
					Sleep(50);
				}
			}
			continue;
		}
		if (!pUDev->CanStart(99)) {
			// Do not check FFU if tool is operating [4/25/2013 Administrator]
			for (int i = 0; i < 60; i++) {
				if (!bRun) {
					break;
				}
				for (j = 0; j < 500; j++) {
					if (!bRun) {
						break;
					}
					Sleep(50);
				}
			}
			continue;
		}
		ULONGLONG tick2 = GetTickCount64();
		if ((tick2 - tick) > dly) {
			FFUPressNow = cb.Read(p413App->pMtr->GetFFUPressure());
			tick = GetTickCount64();
		}
		Sleep(2000);
	}
	bStop = TRUE;
	return TRUE;
}

int CFFUThread::ExitInstance() {
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFFUThread, CWinThread)
	//{{AFX_MSG_MAP(CFFUThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFFUThread message handlers

void CFFUThread::SleepEx(long duration) {
	Sleep(duration);
}