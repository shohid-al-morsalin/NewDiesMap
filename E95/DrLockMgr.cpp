#include "stdafx.h"

#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "Global413.h"

#include "DrLockMgr.h"

UINT CDrLockMgr::DoorLockProc(LPVOID pMgr) {
	CDrLockMgr* pDLM = (CDrLockMgr*)pMgr;
	if (pDLM->bDoorLockCheck) {
		pDLM->bStarted = TRUE;
		pDLM->bStopped = FALSE;
		pDLM->bPaused = FALSE;

		BOOL bLastState = FALSE;
		while (!pDLM->bStopped) {
			if (pDLM->bPaused) {
				Sleep(1000);
				continue;
			}
			BOOL bCurrentState = p413App->IsDoorInterlockTriggered();
			
			if (bCurrentState) {
				pGDev->SetAlarm(CAlrmList::DOORINTERLOCK);
				pGDev->SetARAMSState(SArams::ARAMSSTATE::UNSCHEDULEDDOWN);
			}
			else {
				if (bLastState) {
					pGDev->ClearAlarm(CAlrmList::DOORINTERLOCK);
					pGDev->SetARAMSState(SArams::ARAMSSTATE::STANDBY);
				}
			}
			bLastState = bCurrentState;
			pDLM->Yld(pDLM->nDoorCheckInterval);
		}
		pDLM->bStarted = FALSE;
	}
	return 0;
}

CDrLockMgr::CDrLockMgr() {
	bDoorLockCheck = p413App->Global.bDoorInterlock;
	nDoorCheckInterval = p413App->Global.nDoorInterlockInterval;

	DWORD dwID = 0;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DoorLockProc, this, 0, &dwID);
}

CDrLockMgr::~CDrLockMgr() {
	bStopped = TRUE;
	while (bStarted) {
		Sleep(50);
	}
	if (hThread) {
		WaitForSingleObject(hThread, 10000);
		Sleep(500);
		CloseHandle(hThread);
		hThread = NULL;
	}
}