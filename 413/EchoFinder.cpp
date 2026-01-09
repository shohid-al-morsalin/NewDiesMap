// EchoFinder.cpp: implementation of the CEchoFinder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "EchoFinder.h"
#include "..\E95\413App.h"
#include "..\E95\Recipe413.h"
#include "EchoScope\EchoProbe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEchoFinder::CEchoFinder() {
	hWnd = NULL;
	bEchoStopFlag = FALSE;
	m_hThread = NULL;
	OldPos = 0;
}

CEchoFinder::~CEchoFinder() {}

BOOL CEchoFinder::Initialize() {
	if (!p413App->pEcho) {
		ASSERT(0);
		return FALSE;
	}
	p413App->pEcho->SetCalculatingEndProc(OnCalculatingEnd, this);
	return TRUE;
}

BOOL CEchoFinder::Deinitialize() {
	p413App->pEcho->SetCalculatingEndProc(NULL, NULL);
	return FALSE;
}

void CALLBACK CEchoFinder::OnCalculatingEnd(int nTotalPeaks, LPVOID lpParam) {
	// hand it over to main thread to refresh the screen [3/19/2010 Valued Customer]
	CEchoFinder* pEFdr = (CEchoFinder*)lpParam;
	if (!pEFdr || (!p413App->pEcho)) {
		ASSERT(0);
		return;
	}
	CEchoProbeBase* pEcho = p413App->pEcho;
	if (pEcho->bSignal) {
		CString MsgStr;
		if (pEcho->DoFindPeak(pEcho->GetRecipe(), p413App->Global.CurMeSet, MsgStr))	// @@@ Do not work in Jobs section [11/3/2012 Yuen]
		{
			pEFdr->PostDrawChartMessage(pEFdr);  // pEF2Dlg->PostMessage(ID_DRAWCHART,0,NULL);
		}
		else {
			pEFdr->PostErrorMessage(MsgStr);
		}
		if (pEcho->bSimu) {
			Sleep(35);	// simulation mode only [4/20/2012 Yuen]
		}
	}
	SetEvent(m_hMeasureEndEvent);
}

// This is a separate thread [3/19/2010 Valued Customer]
DWORD CEchoFinder::MeasureProcEcho(LPVOID pParam) {
	CEchoFinder* pEFdr = (CEchoFinder*)pParam;

	if (!m_hCalculatingEvent) {
		m_hCalculatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	if (!m_hMeasureEndEvent) {
		m_hMeasureEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	SetEvent(m_hMeasureEndEvent);
	SetEvent(m_hCalculatingEvent);

	while (pEFdr->bEchoStopFlag == FALSE) {
		// No waiting so that the loop is running and able to detect bStopFlag [3/19/2010 Valued Customer]
		if (WaitForSingleObject(m_hMeasureEndEvent, 0) == WAIT_OBJECT_0) {
			ResetEvent(m_hMeasureEndEvent);
			p413App->pEcho->StartMeasurement(FALSE, 0, 0);	// @@@ Do not work in Jobs section [11/3/2012 Yuen]
			if (p413App->pEcho->GetStatus() == 0) {
				pEFdr->bEchoStopFlag = TRUE;
			}
		}
		Sleep(20);  // release some CPU cycles for main thread [3/21/2010 Valued Customer]
	}
	return 1;
}

BOOL CEchoFinder::StartEchoThread() {
	// 	p413App->pEcho->SetBoardNumber(p413App->MCBoardNo);
	p413App->pEcho->ClearEcho(p413App->MCBoardNo);
	int ErrStatus;
	if (!p413App->pEcho->SetupEvents(ErrStatus, p413App->MCBoardNo)) {
		return FALSE;
	}

	DWORD dwID;
	if (m_hThread) {
		bEchoStopFlag = TRUE;
		WaitForSingleObject(m_hThread, 5000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	bEchoStopFlag = FALSE;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MeasureProcEcho, this, 0, &dwID);
	return TRUE;
}

BOOL CEchoFinder::StopEchoThread() {
	if (m_hThread) {
		bEchoStopFlag = TRUE;
		WaitForSingleObject(m_hThread, 60000);
		Sleep(500);  // very important wait till last reading end [7/21/2010 C2C]
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	return TRUE;
}