// LTWMgr.cpp: implementation of the CLTWMgr class.
//
#include "stdafx.h"

#ifdef _E95
#include "TCtrlDBNet.h"
#endif

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "FSMLightTower.h"
#include "GlobalDeviceContainer.h"
#include "SRC/Arams.h"
#include "SRC/DOSUtil.h"
#include "TCtrlBrk.h"
#include "TCtrlDB.h"
#include "TCtrlPatLitSerial.h"

#include "LTWMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CLTWMgr* pLTMgr = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLTWMgr::CLTWMgr() {
	// 	if (!pARAMS)
	// 	{
	// 		ASSERT(0);
	// 	}

	bGenAlarmSet = FALSE;

	DosUtil.SetLTower("RTL-RTL", DosUtil.GetLTower("RTL-RTL", "010"));
	DosUtil.SetLTower("RTL-TBK", DosUtil.GetLTower("RTL-TBK", "001"));
	DosUtil.SetLTower("RTL-RTU", DosUtil.GetLTower("RTL-RTU", "011"));
	DosUtil.SetLTower("RTL-OOS", DosUtil.GetLTower("RTL-OOS", "100"));
	DosUtil.SetLTower("TBK-RTL", DosUtil.GetLTower("TBK-RTL", "001"));
	DosUtil.SetLTower("TBK-TBK", DosUtil.GetLTower("TBK-TBK", "001"));
	DosUtil.SetLTower("TBK-RTU", DosUtil.GetLTower("TBK-RTU", "011"));
	DosUtil.SetLTower("TBK-OOS", DosUtil.GetLTower("TBK-OOS", "100"));
	DosUtil.SetLTower("RTU-RTL", DosUtil.GetLTower("RTU-RTL", "010"));
	DosUtil.SetLTower("RTU-TBK", DosUtil.GetLTower("RTU-TBK", "011"));
	DosUtil.SetLTower("RTU-RTU", DosUtil.GetLTower("RTU-RTU", "011"));
	DosUtil.SetLTower("RTU-OOS", DosUtil.GetLTower("RTU-OOS", "100"));
	DosUtil.SetLTower("OOS-RTL", DosUtil.GetLTower("OOS-RTL", "100"));
	DosUtil.SetLTower("OOS-TBK", DosUtil.GetLTower("OOS-TBK", "100"));
	DosUtil.SetLTower("OOS-RTU", DosUtil.GetLTower("OOS-RTU", "100"));
	DosUtil.SetLTower("OOS-OOS", DosUtil.GetLTower("OOS-OOS", "100"));

	CString method;
	method = DosUtil.GetLTMethod("Simu");
	DosUtil.SetLTMethod(method);
	if (method.Find("BrkRbt") != -1) {
		pTwr = new CTCtrlBrk;
	}
	else if (method.Find("PatLit") != -1) {
		pTwr = new CTCtrlPatLitSerial;
	}
#ifdef _E95
	else if (method.Find("DetBrdNet") != -1) {
		pTwr = new CTCtrlDBNet;
	}
#endif
	else if (method.Find("DetBrd") != -1) {
		pTwr = new CTCtrlDB;
	}
	else if (method.Find("FSMTower") != -1) {
		pTwr = new CFSMLightTower;
		if (pTwr) {
			pTwr->Init();
			pTwr->SetConditionAllOff();
			Set_Idle();  //012324 Masum Added for THEIL 413SA Signal Tower
		}
	}
	else {
		pTwr = new CTCtrlBase;
	}
}

CLTWMgr::~CLTWMgr() {
	if (pTwr) {
		delete pTwr;
		pTwr = NULL;
	}
}

void CLTWMgr::SetLight() {
	char Rd = '0', Am = '0', Gr = '0'; // Contains '0', '1' or '2'
	// Get current transfer status for each port
	// Decide on which light to activate
	// Then
	char Bl = '0';
	BOOL bUpdate = FALSE;
	CString ind = "000";
	if (Var.nLoadPort == 1) {
		switch (GetLPBState(1)) {
		case PrcsMgrServices::FOUPERROR:
			ind = DosUtil.GetLTower("RTL-OOS", "100");
			bUpdate = TRUE;
			break;
		case PrcsMgrServices::NOFOUP:
		case PrcsMgrServices::FOUPPRESENT:
		case PrcsMgrServices::CLAMPPING:
			ind = DosUtil.GetLTower("RTL-RTL", "010");
			bUpdate = TRUE;
			break;
		case PrcsMgrServices::CLAMPPED:
		case PrcsMgrServices::DOCKING:
		case PrcsMgrServices::DOCKED:
		case PrcsMgrServices::UNDOCKING:
			ind = DosUtil.GetLTower("RTL-TBK", "001");
			bUpdate = TRUE;
			break;
		case PrcsMgrServices::UNDOCKED:
		case PrcsMgrServices::UNCLAMPPING:
		case PrcsMgrServices::UNCLAMPPED:
			ind = DosUtil.GetLTower("RTL-RTU", "011");
			bUpdate = TRUE;
			break;
		}
		Bl = '1';
		//	if (GEMObjs.GetLPAccessMode(1) == CLPObj::AUTO)
		if (GetLPAccessMode(1) == CLPObj::AUTO) {
			Bl = '0';
		}
	}
	else if (Var.nLoadPort > 1) {
		//	switch(CMainFrm::m_LPBar1->CurLPBState) {
		switch (GetLPBState(1)) {
		case PrcsMgrServices::FOUPERROR:
			switch (GetLPBState(2)) {
			case PrcsMgrServices::FOUPERROR:
				ind = DosUtil.GetLTower("OOS-OOS", "100");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::NOFOUP:
			case PrcsMgrServices::FOUPPRESENT:
			case PrcsMgrServices::CLAMPPING:
				ind = DosUtil.GetLTower("OOS-RTL", "100");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::CLAMPPED:
			case PrcsMgrServices::DOCKING:
			case PrcsMgrServices::DOCKED:
			case PrcsMgrServices::UNDOCKING:
				ind = DosUtil.GetLTower("OOS-TBK", "100");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::UNDOCKED:
			case PrcsMgrServices::UNCLAMPPING:
			case PrcsMgrServices::UNCLAMPPED:
				ind = DosUtil.GetLTower("OOS-RTU", "100");
				bUpdate = TRUE;
				break;
			}
			break;
			break;
		case PrcsMgrServices::NOFOUP:
		case PrcsMgrServices::FOUPPRESENT:
		case PrcsMgrServices::CLAMPPING:
			//	switch(CMainFrm::m_LPBar2->CurLPBState) {
			switch (GetLPBState(2)) {
			case PrcsMgrServices::NOFOUP:
			case PrcsMgrServices::FOUPPRESENT:
			case PrcsMgrServices::CLAMPPING:
				ind = DosUtil.GetLTower("RTL-RTL", "010");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::CLAMPPED:
			case PrcsMgrServices::DOCKING:
			case PrcsMgrServices::DOCKED:
			case PrcsMgrServices::UNDOCKING:
				ind = DosUtil.GetLTower("RTL-TBK", "001");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::UNDOCKED:
			case PrcsMgrServices::UNCLAMPPING:
			case PrcsMgrServices::UNCLAMPPED:
				ind = DosUtil.GetLTower("RTL-RTU", "011");
				bUpdate = TRUE;
				break;
			}
			break;
		case PrcsMgrServices::CLAMPPED:
		case PrcsMgrServices::DOCKING:
		case PrcsMgrServices::DOCKED:
		case PrcsMgrServices::UNDOCKING:
			//	switch(CMainFrm::m_LPBar2->CurLPBState) {
			switch (GetLPBState(2)) {
			case PrcsMgrServices::NOFOUP:
			case PrcsMgrServices::FOUPPRESENT:
			case PrcsMgrServices::CLAMPPING:
				ind = DosUtil.GetLTower("TBK-RTL", "010");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::CLAMPPED:
			case PrcsMgrServices::DOCKING:
			case PrcsMgrServices::DOCKED:
			case PrcsMgrServices::UNDOCKING:
				ind = DosUtil.GetLTower("TBK-TBK", "001");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::UNDOCKED:
			case PrcsMgrServices::UNCLAMPPING:
			case PrcsMgrServices::UNCLAMPPED:
				ind = DosUtil.GetLTower("TBK-RTU", "011");
				bUpdate = TRUE;
				break;
			}
			break;
		case PrcsMgrServices::UNDOCKED:
		case PrcsMgrServices::UNCLAMPPING:
		case PrcsMgrServices::UNCLAMPPED:
			//	switch(CMainFrm::m_LPBar2->CurLPBState) {
			switch (GetLPBState(2)) {
			case PrcsMgrServices::NOFOUP:
			case PrcsMgrServices::FOUPPRESENT:
			case PrcsMgrServices::CLAMPPING:
				ind = DosUtil.GetLTower("RTU-RTL", "010");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::CLAMPPED:
			case PrcsMgrServices::DOCKING:
			case PrcsMgrServices::DOCKED:
			case PrcsMgrServices::UNDOCKING:
				ind = DosUtil.GetLTower("RTU-TBK", "001");
				bUpdate = TRUE;
				break;
			case PrcsMgrServices::UNDOCKED:
			case PrcsMgrServices::UNCLAMPPING:
			case PrcsMgrServices::UNCLAMPPED:
				ind = DosUtil.GetLTower("RTU-RTU", "011");
				bUpdate = TRUE;
				break;
			}
			break;
		}
		//	if (GEMObjs.GetLPAccessMode(1) == CLPObj::AUTO || GEMObjs.GetLPAccessMode(2) == CLPObj::AUTO)
		if (GetLPAccessMode(1) == CLPObj::AUTO || GetLPAccessMode(2) == CLPObj::AUTO) {
			bUpdate = TRUE;
		}
		if ((pGDev->pGEMObjs->GetLPTransferStatus(1) == CLPObj::OUTOFSERVICE) || (pGDev->pGEMObjs->GetLPTransferStatus(2) == CLPObj::OUTOFSERVICE)) {
			ind.SetAt(0, '1');
		}
		else {
			ind.SetAt(0, '0');
		}
	}
	Bl = '0';
	if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE)/* || (pGDev->pGEMObjs->GetLineMode() == EqpStatus::LOCAL)*/) {
		if (pGDev->SubModel != CGlobalDeviceContainer::TSMC) {
			Bl = '1';
		}
	}
	if ((pGDev->AramsE95.GetCurrentState() == SArams::SCHEDULEDDOWN) ||
		(pGDev->AramsE95.GetCurrentState() == SArams::UNSCHEDULEDDOWN)
		//		(GEMObjs.GetOpMode() == EqpStatus::MAINT)) {
		//		(GetOpMode() == SArams::STANDBY)
		) {
		Rd = '1';
		Am = '0';
		Gr = '0';
		if (pTwr) {
			pTwr->Set(Rd, Am, Gr, Bl);
		}
		MakeLight(Rd, Am, Gr, Bl);
	}
	else if (bUpdate) {
		Rd = ind.GetAt(0);
		Am = ind.GetAt(1);
		Gr = ind.GetAt(2);
		if (bGenAlarmSet) {
			Rd = '2';
		}
		if (pTwr) {
			pTwr->Set(Rd, Am, Gr, Bl);
		}
		MakeLight(Rd, Am, Gr, Bl);
	}
}
//012324 Masum Added for THEIL 413SA Signal Tower [[[[
void CLTWMgr::Set_Idle()
{
	if(pTwr) pTwr->Set('0', '1', '0', '0');  //Orange
}

void CLTWMgr::Set_Run()
{
	if (pTwr) pTwr->Set('0', '0', '1', '0');  //Green
}

void CLTWMgr::Set_Alarm()  
{
	if (pTwr) pTwr->Set('1', '0', '0', '0');  //Red
}
//012324 Masum Added for THEIL 413SA Signal Tower  ]]]]

void CLTWMgr::MakeLight(char Rd, char Am, char Gr, char Bl) {}

void CLTWMgr::SetLight(short Red, short Amber, short Green) {}

void CLTWMgr::SetGenAlarm(BOOL bSet) {
	bGenAlarmSet = bSet;

	char Rd, Am, Gr, Bl;
	if (pTwr) {
		pTwr->Get(Rd, Am, Gr, Bl);
		if (bGenAlarmSet) {
			Rd = '2';
		}
		else {
			Rd = '0';
		}
		Bl = '0';
		if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::REMOTE) || (pGDev->pGEMObjs->GetLineMode() == EqpStatus::LOCAL)) {
			Bl = '1';
		}
		pTwr->Set(Rd, Am, Gr, Bl);
		MakeLight(Rd, Am, Gr, Bl);
	}
}