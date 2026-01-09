// LTMgrE95.cpp: implementation of the CLTMgrE95 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LTMgrE95.h"
#include "LTWMgr.h"
#include "TCtrlDB.h"
#include "TCtrlBrk.h"
#include "FSMLightTower.h"
#include "TCtrlPatLitSerial.h"
#include "..\E95\TitlePanel.h"
//#include "..\Sys\Var.h"
#include "..\Src\Arams.h"
//#include "..\Src\DosUtil.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLTMgrE95::CLTMgrE95() {
	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
}

CLTMgrE95::~CLTMgrE95() {}

CLPObj::ACCESSMODE CLTMgrE95::GetLPAccessMode(short port) {
	return CLPObj::ACCESSMODE(0);
}

PrcsMgrServices::LPBSTATE CLTMgrE95::GetLPBState(short port) {
	return pGDev->GetLPState(port);
}

SArams::ARAMSSTATE CLTMgrE95::GetOpMode() {
	return pGDev->GetARAMSState();
}

/*
*		// Support for Signal Tower FSM
bFSMLightTower = DosUtil.IsFSMLightTower();
DosUtil.SetFSMLightTower(bFSMLightTower);
if (bFSMLightTower)
{
bTowerInit =FSMTower.Init();
}
// End Signal Tower FSM

 */

void CLTMgrE95::MakeLight(char Rd, char Am, char Gr, char Bl) {
	if (pGDev->SubModel == CGlobalDeviceContainer::TSMC) {
		LTstate[0] = 'r';
		LTstate[1] = 'g';
		LTstate[2] = 'o';
		LTstate[3] = 'b';
	}
	else {
		switch (Rd) {
		case '0':
			LTstate[0] = 'r';
			break;
		case '1':
			LTstate[0] = 'R';
			break;
		case '2':
			LTstate[0] = 'E';
			break;
		}
		switch (Gr) {
		case '0':
			LTstate[1] = 'g';
			break;
		case '1':
			LTstate[1] = 'G';
			break;
		case '2':
			LTstate[1] = 'N';
			break;
		}
		switch (Am) {
		case '0':
			LTstate[2] = 'o';
			break;
		case '1':
			LTstate[2] = 'O';
			break;
		case '2':
			LTstate[2] = 'Y';
			break;
		}
		switch (Bl) {
		case '0':
			LTstate[3] = 'b';
			break;
		case '1':
			LTstate[3] = 'B';
			break;
		case '2':
			LTstate[3] = 'L';
			break;
		}
	}
	ULONGLONG tick = GetTickCount64();
	while (!pGDev || !pUDev->pTitlePanel || (!pUDev->pTitlePanel->m_hWnd)) {
		// wait till main frame starts up
		Sleep(1000);
		if ((GetTickCount64() - tick) > 60000) return;
	}
	PostMessage(pUDev->pTitlePanel->m_hWnd, ID_LTW_SHOW, 0, (LPARAM)LTstate);
}