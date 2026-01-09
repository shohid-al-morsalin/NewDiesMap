#include "stdafx.h"
#include "resource.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "413App.h"
#include "AlrmList.h"
#include "ComBase.h"
#include "GlobalDeviceContainer.h"
#include "LdPTOut.h"
#include "LTW/LTWMgr.h"
#include "SRC/Comm.h"
#include "SRC/DOSUtil.h"
#include "SystemStatusDlg.h"

#include "ComFL300.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Not tread save [5/8/2002]
CComFL300::CComFL300() {
	// 	if (!pGDev)
	// 	{
	// 		ASSERT(0);
	// 	}
	// 	RTLLed = 1;
	// 	RTULed = 2;
	// 	MANLed = 3;
	// 	AUTLed = 6;
	// 	DOPLed = 0;
	memset(SlotMap, 0, sizeof(SlotMap));

	// 	RTLLed = DosUtil.GetLPUnit("RTLLed", 1);
	// 	RTULed = DosUtil.GetLPUnit("RTULed", 2);
	// 	MANLed = DosUtil.GetLPUnit("MANUALLed", 3);
	// 	AUTLed = DosUtil.GetLPUnit("AUTOLed", 4);
	// 	DOPLed = DosUtil.GetLPUnit("DOOROPENLed", 0);
	// 	ACCLed = DosUtil.GetLPUnit("ACCESSLed",10);
	// 	RSVLed = DosUtil.GetLPUnit("RESERVEDLed",5);
	// 	ALRLed = DosUtil.GetLPUnit("ALARMEDLed",6);
	// 	PBTPort = DosUtil.GetLPUnit("PushButton",2);

	//LPState = UNKNOWN;
	bCarrierPresent = FALSE;
	bClampResponse = TRUE;
	bUnclampResponse = TRUE;
}

CComFL300::~CComFL300() {
	Disconnect();
}

void CComFL300::Decode(CString& msg) {
	CString str;
	CString inst;
	Log(msg, 2);
	//TRACE("<-");
	//TRACE(msg);
	//TRACE("\n");
	if (msg.Find("AERS") != -1) {
		if (msg.Find("E84") != -1) {
			if (msg.Find("TIMEOUT") != -1) {
				if (msg.Find("E84_TP1_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP1TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				if (msg.Find("E84_TP2_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP2TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				if (msg.Find("E84_TP3_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP3TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				if (msg.Find("E84_TP4_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP4TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				if (msg.Find("E84_TP5_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP5TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				if (msg.Find("E84_TP6_TIMEOUT") != -1) {
					PostMsg(hWnd, ID_COM_TP6TIMEOUT, bCarrierPresent, NULL);
					Sleep(10000); // Wait a while [10/7/2012 Yuen]
					Reset();
					return;
				}
				return;
			}
			if (msg.Find("LOST_E84_SIGNAL") != -1) {
				PostMsg(hWnd, ID_COM_LOSTE84SIGNAL, bCarrierPresent, NULL);
				Sleep(10000); // Wait a while [10/7/2012 Yuen]
				Reset();
				return;
			}
			if (msg.Find("E84_LOAD_COMPLETED") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_LOADCOMPLETED, NULL);
				return;
			}
			if (msg.Find("E84_L_REQ_SUBMITTED") != -1) {
				// This is correct
				pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::TRANSFERBLOCKED);
				PostMsg(hWnd, WM_COMMAND, ID_COM_LREQSUBMITTED, NULL);
				return;
			}
			if (msg.Find("E84_L_REQ_COMPLETED") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_LREQCOMPLETED, NULL);
				return;
			}
			if (msg.Find("E84_LOAD_STARTED") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_LOADSTARTED, NULL);
				return;
			}
			if (msg.Find("E84_U_REQ_SUBMITTED") != -1) {
				// This is correct
				pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::TRANSFERBLOCKED);
				PostMsg(hWnd, WM_COMMAND, ID_COM_UREQSUBMITTED, NULL);
				return;
			}
			if (msg.Find("E84_U_REQ_COMPLETED") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_UREQCOMPLETED, NULL);
				return;
			}
			if (msg.Find("E84_UNLOAD_STARTED") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_UNLOADSTARTED, NULL);
				return;
			}
			if (msg.Find("E84_UNLOAD_COMPLETED") != -1) {
				ProcessCarrierRemoved();  //  [2/9/2011 C2C]
				EnableHOAvbl();
				PostMsg(hWnd, WM_COMMAND, ID_COM_UNLOADCOMPLETED, NULL);
				return;
			}
			if (msg.Find("UNEXPECTED_E84_SIGNAL") != -1) {
				PostMsg(hWnd, ID_COM_UNEXPECTEDE84SIGNAL, bCarrierPresent, NULL);
				Sleep(10000); // Wait a while [10/7/2012 Yuen]
				Reset();
				return;
			}
			if (msg.Find("E84_NON_AMHS_CARRIER_ARRIVED") != -1) {
				Status();
				PostMsg(hWnd, WM_COMMAND, ID_COM_NONHSMSCARRIERARRIVED, NULL);
				return;
			}
			if (msg.Find("E84_NON_AMHS_CARRIER_REMOVED") != -1) {
				Status();
				PostMsg(hWnd, WM_COMMAND, ID_COM_NONHSMSCARRIERREMOVED, NULL);
				return;
			}
		}
		else {
			if (msg.Find("CARRIER_ARRIVED") != -1) {
				ProcessCarrierArrived();
				return;
			}
			if (msg.Find("CARRIER_REMOVED") != -1) {
				ProcessCarrierRemoved();
				return;
			}
			if (msg.Find("END_OPEN") != -1) {
				ProcessEndOpen();
				return;
			}
			if (msg.Find("END_CLOSE") != -1) {
				ProcessEndClose();
				return;
			}
			if (msg.Find("ABORT_CLOSE") != -1) {
				//SendHCA("HCS LED 6 ON",6000);
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::UNDOCKFAILED);
				pGDev->pOLnClt->GEMEventFoupCloseFailed(unit);
				PostMsg(hWnd, WM_COMMAND, ID_COM_ABORTCLOSE, NULL);
				RefreshLED();
				return;
			}
			if (msg.Find("ABORT_OPEN") != -1) {
				//SendHCA("HCS LED 6 ON",6000);
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::DOCKFAILED);
				pGDev->pOLnClt->GEMEventFoupOpenFail(unit);
				PostMsg(hWnd, WM_COMMAND, ID_COM_ABORTOPEN, NULL);
				RefreshLED();
				return;
			}
			if (msg.Find("ABORT_APHD_LATCH") != -1) {
				//SendHCA("HCS LED 6 ON",6000);
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::CLAMPFAILED);
				pGDev->pOLnClt->GEMEventFoupClampFail(unit);
				PostMsg(hWnd, WM_COMMAND, ID_COM_ABORTAPHDLATCH, NULL);
				RefreshLED();
				return;
			}
			if (msg.Find("ABORT_APHD_RELEASE") != -1) {
				//SendHCA("HCS LED 6 ON",6000);
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPFAILED);
				pGDev->pOLnClt->GEMEventFoupUnclampFail(unit);
				PostMsg(hWnd, WM_COMMAND, ID_COM_ABORTAPHDRELEASE, NULL);
				RefreshLED();
				return;
			}
			if (msg.Find("CMPL_APHD_LATCH") != -1) {
				pGDev->pOLnClt->GEMEventFoupClamped(unit);
				if (bClampResponse) {
					pGDev->SetLPState(unit, PrcsMgrServices::CLAMPPED);
					PostMsg(hWnd, WM_COMMAND, ID_COM_LATCHCMPL, NULL);
					RefreshLED();
				}
				else {
					bClampResponse = TRUE;
				}
				RefreshLED();
				return;
			}
			if (msg.Find("CMPL_APHD_RELEASE") != -1) {
				pGDev->pOLnClt->GEMEventFoupUnclamped(unit);
				if (bUnclampResponse) {
					pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPPED);
					PostMsg(hWnd, WM_COMMAND, ID_COM_RELEASECMPL, NULL);
					RefreshLED();
				}
				else {
					bUnclampResponse = TRUE;
				}
				RefreshLED();
				return;
			}
			if (msg.Find("CMPL_CLOSE_NO_RELEASE") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_CLOSENORELEASECMPL, NULL);
				int ack;
				if (ReadMap()) {
					ack = ID_COM_SLOTMAP;
				}
				else {
					ack = ID_COM_SLOTMAPFAIL;
				}
				//pGDev->SetLPState(unit,PrcsMgrServices::UNDOCKED);
				//pGDev->pOLnClt->GEMEventFoupUndocked(unit);
				pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::READYTOUNLOAD);
				if (AccMode == AMAUTO) {
					EnableHOAvbl();
				}
				PostMsg(hWnd, ack, unit, (long)SlotMap);
				RefreshLED();
				return;
			}
			if (msg.Find("BUTTON_2_PUSHED") != -1) {
				ProcessButtonPushed();
				return;
			}
			if (msg.Find("BUTTON_1_PUSHED") != -1) {
				ProcessButtonPushed();
				return;
			}
			if (msg.Find("CMPL_MAP") != -1) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_COMPLETEMAP, NULL);
				int ack;
				if (ReadMap()) {
					ack = ID_COM_SLOTMAP;
				}
				else {
					ack = ID_COM_SLOTMAPFAIL;
				}
				PostMsg(hWnd, ack, unit, (long)SlotMap);
				return;
			}
			if (msg.Find("EXIT_HOME") != -1) {
				pGDev->SetLPState(unit, PrcsMgrServices::DOCKING);
				pGDev->SetLPState(unit, PrcsMgrServices::DOCKED);
				if (AccMode == AMMANUAL) {
					//SendHCA("HCS LED 2 ON",6000);
					LEDDOP(TRUE);
				}
				// 				pGDev->pOLnClt->GEMEventFoupDocked(unit);
				PostMsg(hWnd, WM_COMMAND, ID_COM_EXITHOME, NULL);
				RefreshLED();
			}
		}
		return;
	}
	if (msg.Find("ARS") != -1) {
		if (msg.Find("FATAL") != -1) {
			if (msg.Find("Excessive wafer out") != -1) {
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::FOUPERROR);
				switch (unit) {
				case 1:
					pGDev->SetAlarm(CAlrmList::LP1EXCESSIVEWAFEROUT);
					break;
				case 2:
					pGDev->SetAlarm(CAlrmList::LP2EXCESSIVEWAFEROUT);
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->pLTMgr->SetGenAlarm(TRUE);
				PostMsg(hWnd, WM_COMMAND, ID_COM_WAFEROUT, NULL);
				RefreshLED();
				Reset();
				return;
			}
			if (msg.Find("Perimeter sensor is tripped") != -1) {
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::FOUPERROR);
				switch (unit) {
				case 1:
					pGDev->SetAlarm(CAlrmList::LP1PERISENSORTRIPED);
					break;
				case 2:
					pGDev->SetAlarm(CAlrmList::LP2PERISENSORTRIPED);
					break;
				}
				pGDev->pLTMgr->SetGenAlarm(TRUE);
				PostMsg(hWnd, WM_COMMAND, ID_COM_SENSORTRIP, NULL);
				RefreshLED();
				Reset();
				return;
			}
			if (msg.Find("Vacuum failed to be turned On") != -1) {
				LEDAlarm(TRUE);
				pGDev->SetLPState(unit, PrcsMgrServices::FOUPERROR);
				switch (unit) {
				case 1:
					pGDev->SetAlarm(CAlrmList::LP1NOVACUUM);
					break;
				case 2:
					pGDev->SetAlarm(CAlrmList::LP2NOVACUUM);
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->pLTMgr->SetGenAlarm(TRUE);
				PostMsg(hWnd, WM_COMMAND, ID_COM_NOVACUUM, NULL);
				RefreshLED();
				Reset();
				return;
			}
			if (msg.Find("E84 failed. Need a RESET") != -1) {
				Reset();
				return;
			}
			return;
		}
		if (msg.Find("WARNING") != -1) {
			if (msg.Find("Vacuum is lost during motion") != -1) {
				switch (unit) {
				case 1:
					pGDev->SetAlarm(CAlrmList::LP1VACUUMLOST);
					break;
				case 2:
					pGDev->SetAlarm(CAlrmList::LP2VACUUMLOST);
					break;
				default:
					ASSERT(0);
					break;
				}
				pGDev->pLTMgr->SetGenAlarm(TRUE);
				PostMsg(hWnd, WM_COMMAND, ID_COM_VACUUMLOST, NULL);
				Reset();
				return;
			}
		}
		return;
	}
	if (msg.Find("HCA") != -1) {
		if (msg.Find("ALARM") != -1) {
			LEDAlarm(TRUE);
			pGDev->SetLPState(unit, PrcsMgrServices::FOUPERROR);
			pGDev->pLTMgr->SetGenAlarm(TRUE);
			PostMsg(hWnd, WM_COMMAND, ID_COM_ALARM, NULL);
			RefreshLED();
			return;
		}
		else if (msg.Find("RMIDA NO") != -1) {
			PostMsg(hWnd, WM_COMMAND, ID_COM_RMIDANO, NULL);
			return;
		}
		else if (msg.Find("NO_POD") != -1) {
			PostMsg(hWnd, WM_COMMAND, ID_COM_NOPOD, NULL);
			return;
		}
		return;
	}
}

BOOL CComFL300::Status() {
	// LYF Obsolete [11/5/2005]
	return TRUE;

	char map[512];
	short cnt = 5;
	while (cnt--) {
		if (Read("FSR FC=0", (char*)map, sizeof(map), 6000)) {
			if (strstr(map, "CIP=")) {
				if (strstr(map, "CIP=TRUE")) {
					bInPlace = TRUE;
				}
				else {
					bInPlace = FALSE;
				}
				if (strstr(map, "READY=TRUE")) {
					bReady = TRUE;
				}
				else {
					bReady = FALSE;
				}
				return TRUE;
			}
		}
		Sleep(1000);
	}
	return FALSE;
}

BOOL CComFL300::ExtractSlotmap(char* SMap) {
	BOOL bNoErr = TRUE;
	CString txt;
	txt = SMap;

	SlotMap[0] = '0';
	int i;
	//[20243008/Masum
	int nSlot = p413App->Global.SlotsPerFOUP;
	if (nSlot == 13)
	{
		int ind = 0;
		for (i = 0; i < 25; i++) {
			CString str;
			if (i != 0)
			{
				if (i % 2 != 0) continue;
			}
			str.Format("S%d=E", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[ind] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
				ind++;
				continue;
			}
			str.Format("S%d=F", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[ind] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
				ind++;
				continue;
			}
			str.Format("S%d=C", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[ind] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
				ind++;
				continue;
			}
			str.Format("S%d=2", i + 1);
			if (txt.Find(str, 0) != -1) {
				if (bIgnorDoubleSlotted) {
					SlotMap[ind] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
					ind++;
				}
				else {
					SlotMap[ind] = CSubstObj::GetSMChar(CSubstObj::SM_DOUBLE_SLOTTED);
					ind++;
				}
				continue;
			}
			CString str1;
			str1.Format("Slot # %d error", i + 1);
			bNoErr = FALSE;
		}
	}
	//]
	else
	{
		for (i = 0; i < 25; i++) {
			CString str;
			str.Format("S%d=E", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
				continue;
			}
			str.Format("S%d=F", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
				continue;
			}
			str.Format("S%d=C", i + 1);
			if (txt.Find(str, 0) != -1) {
				SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
				continue;
			}
			str.Format("S%d=2", i + 1);
			if (txt.Find(str, 0) != -1) {
				if (bIgnorDoubleSlotted) {
					SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
				}
				else {
					SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_DOUBLE_SLOTTED);
				}
				continue;
			}
			CString str1;
			str1.Format("Slot # %d error", i + 1);
			bNoErr = FALSE;
		}
	}
	SlotMap[i] = 0;
	return bNoErr;

	//for (i = 0; i < 25; i++) {
	//	CString str;
	//	str.Format("S%d=E", i + 1);
	//	if (txt.Find(str, 0) != -1) {
	//		SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
	//		continue;
	//	}
	//	str.Format("S%d=F", i + 1);
	//	if (txt.Find(str, 0) != -1) {
	//		SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
	//		continue;
	//	}
	//	str.Format("S%d=C", i + 1);
	//	if (txt.Find(str, 0) != -1) {
	//		SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
	//		continue;
	//	}
	//	str.Format("S%d=2", i + 1);
	//	if (txt.Find(str, 0) != -1) {
	//		if (bIgnorDoubleSlotted) {
	//			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
	//		}
	//		else {
	//			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_DOUBLE_SLOTTED);
	//		}
	//		continue;
	//	}
	//	CString str1;
	//	str1.Format("Slot # %d error", i + 1);
	//	bNoErr = FALSE;
	//}

}

BOOL CComFL300::Map() {
	// Slotmap info will not come immediately
	// It takes some time to map
	return SendHCA("HCS MAP", 240000);
}

BOOL CComFL300::ReadMap() {
	char map1[512];

	Log("SM read");
	if (Read("FSR FC=2", map1, sizeof(map1), SLOTMAPREADTIMEOUT)) {
		std::string str(map1, 512);//123check
		if (ExtractSlotmap(map1)) {
			return TRUE;
		}
	}
	if (Read("FSR FC=2", map1, sizeof(map1), SLOTMAPREADTIMEOUT)) {
		if (ExtractSlotmap(map1)) {
			return TRUE;
		}
	}
	if (Read("FSR FC=2", map1, sizeof(map1), SLOTMAPREADTIMEOUT)) {
		if (ExtractSlotmap(map1)) {
			return TRUE;
		}
	}
	if (Read("FSR FC=2", map1, sizeof(map1), SLOTMAPREADTIMEOUT)) {
		if (ExtractSlotmap(map1)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CComFL300::AccessModeChange(CLPObj::ACCESSMODE mode) {
	BOOL bRet = FALSE;
	CString inst;
	if (mode == CLPObj::MANUAL) {
		inst = "ECS P68 0";	// Alway perform this to reset LP [10/3/2012 Yuen]
		if (SendAck(inst, 8000)) {
			inst.Format("ECS P68 %d", P68Manual);
			//inst = "ECS P68 34";	// TSMC#1 [10/3/2012 Yuen]
			//inst = "ECS P68 2";	// Others [10/3/2012 Yuen]

			//  [9/20/2020 zhimi]
			// added since LP2 EXIT_HOME report is not activated
			{
				SendAck(inst, 8000);
				inst.Format("EDER SELECT ON 21");
				SendAck(inst, 8000);
				inst.Format("EDER SELECT ON 21");
			}
			AccMode = AMMANUAL;
			if (SendAck(inst, 8000)) {
				bRet = TRUE;
				LEDManual(TRUE);
			}
		}
	}
	else {
		inst = "ECS P68 0";	// Alway perform this to reset LP [10/3/2012 Yuen]
		if (SendAck(inst, 8000)) {
			inst.Format("ECS P68 %d", P68Auto);
			//inst = "ECS P68 33"; // TSMC#1 [10/3/2012 Yuen]
			//inst = "ECS P68 19"; // UMC [10/3/2012 Yuen]
			//inst = "ECS P68 1";  // Others [10/3/2012 Yuen]

			//  [9/20/2020 zhimi]
			// added since LP2 EXIT_HOME report is not activated
			{
				SendAck(inst, 8000);
				inst.Format("EDER SELECT ON 21");
				SendAck(inst, 8000);
				inst.Format("EDER SELECT ON 21");
			}
			AccMode = AMAUTO;
			if (SendAck(inst, 8000)) {
				bRet = TRUE;
				LEDManual(FALSE);
				EnableHOAvbl();
			}
		}
	}
	if (bRet) {
		if (mode == CLPObj::AUTO) {
			if (SendHCA("FSR FC=0", 6000)) {
				CString str = GetResp();
				if (str.Find("CIP=TRUE") != -1) {
					ClampEx();
				}
				if (str.Find("HOME=TRUE") != -1) {
					if ((pGDev->pGEMObjs->GetLPTransferStatus(unit) == CLPObj::READYTOUNLOAD) ||
						(pGDev->pGEMObjs->GetLPTransferStatus(unit) == CLPObj::READYTOUNLOAD)) {
						EnableHOAvbl();
					}
				}
			}
		}
		else {
			if (pGDev->pGEMObjs->GetLPTransferStatus(unit) == CLPObj::READYTOUNLOAD) {
				UnclampEx();
			}
		}
		// 		else
		// 		{
		// 			if (SendHCA("FSR FC=0",6000)) {
		// 				CString str = GetResp();
		// 				if (str.Find("CIP=TRUE") != -1) {
		// 					if (str.Find("HOME=TRUE") != -1) {
		// 						PrcsMgrServices::LPBSTATE lps = pGDev->GetLPState(unit);
		// 						UnclampEx();
		// 					}
		// 				}
		// 			}
		// 		}
	}
	RefreshLED();

	return bRet;
}

BOOL CComFL300::Clamp() {
	CString inst;
	inst = "HCS APHD_LATCH";
	if (SendAck(inst, LATCHFOUPTIMEOUT)) {
		pGDev->SetLPState(unit, PrcsMgrServices::CLAMPPING);
		RefreshLED();
		return TRUE;
	}
	return FALSE;
}

BOOL CComFL300::Unclamp() {
	CString inst;
	inst = "HCS APHD_RELEASE";
	if (SendAck(inst, UNLATCHFOUPTIMEOUT)) {
		pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPPING);
		RefreshLED();
		return TRUE;
	}
	return FALSE;
}

BOOL CComFL300::CloseNoRelease() {
	CString inst;
	inst = "HCS CLOSE_NO_RELEASE";
	if (SendAck(inst, CLOSEFOUPTIMEOUT)) {
		pGDev->SetLPState(unit, PrcsMgrServices::UNDOCKING);
		RefreshLED();
		return TRUE;
	}
	return FALSE;
}

BOOL CComFL300::ClearAllLED() {
	BOOL bRet = TRUE;
	// Need exact command to clamp Pod
	//CString inst;
	//inst = "HCS LED 1 OFF";
	if (!SendHCA("HCS LED 1 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 2 OFF";
	if (!SendHCA("HCS LED 2 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 3 OFF";
	if (!SendHCA("HCS LED 3 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 4 OFF";
	if (!SendHCA("HCS LED 4 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 5 OFF";
	if (!SendHCA("HCS LED 5 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 6 OFF";
	if (!SendHCA("HCS LED 6 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 9 OFF";
	if (!SendHCA("HCS LED 9 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	//inst = "HCS LED 10 OFF";
	if (!SendHCA("HCS LED 10 OFF", 6000, FALSE)) {
		bRet = FALSE;
	}
	return TRUE;
}

BOOL CComFL300::LEDReserved(BOOL on) {
	CString inst;
	if (RSVLed) {
		if (on) {
			inst.Format("HCS LED %d ON", RSVLed);
		}
		else {
			inst.Format("HCS LED %d OFF", RSVLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDAlarm(BOOL on) {
	CString inst;
	if (ALRLed) {
		if (on) {
			inst.Format("HCS LED %d ON", ALRLed);
		}
		else {
			inst.Format("HCS LED %d OFF", ALRLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDDOP(BOOL on) {
	CString inst;
	if (DOPLed) {
		if (on) {
			inst.Format("HCS LED %d ON", DOPLed);
		}
		else {
			inst.Format("HCS LED %d OFF", DOPLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDManual(BOOL on) {
	// Need exact command to clamp Pod
	CString inst;
	if (MANLed) {
		if (on) {
			inst.Format("HCS LED %d ON", MANLed);;
		}
		else {
			inst.Format("HCS LED %d OFF", MANLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	if (AUTLed) {
		if (on) {
			inst.Format("HCS LED %d OFF", AUTLed);
		}
		else {
			inst.Format("HCS LED %d ON", AUTLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDRTL(BOOL on) {
	CString inst;
	if (RTLLed) {
		if (on) {
			inst.Format("HCS LED %d ON", RTLLed);
		}
		else {
			inst.Format("HCS LED %d OFF", RTLLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDRTU(BOOL on) {
	CString inst;
	if (RTULed) {
		if (on) {
			inst.Format("HCS LED %d ON", RTULed);
		}
		else {
			inst.Format("HCS LED %d OFF", RTULed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::LEDLOD(BOOL on) {
	CString inst;
	if (LODLed) {
		if (on) {
			inst.Format("HCS LED %d ON", LODLed);
		}
		else {
			inst.Format("HCS LED %d OFF", LODLed);
		}
		if (!SendAck(inst, 6000, FALSE)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CComFL300::EnableHOAvbl() {
	return SendHCA("HCS TURN_ON_HO_AVBL", 6000);
}

BOOL CComFL300::Calib() {
	return SendHCA("HCS CALIB", CALIBLOADPORTTIMEOUT);
}

BOOL CComFL300::NoVac() {
	return SendHCA("HCS NO_VACUUM", 6000);
}

BOOL CComFL300::NeedVac() {
	return SendHCA("HCS NEED_VACUUM", 6000);
}

BOOL CComFL300::Open() {
	pGDev->SetLPState(unit, PrcsMgrServices::DOCKING);
	RefreshLED();
	return SendHCA("HCS OPEN", OPENFOUPTIMEOUT);
}

BOOL CComFL300::_SetDefault() {
	if (!SendHCA("ECS P38=0", 3000)) return FALSE;
	if (!SendHCA("ECS P68=0", 3000)) return FALSE;
	if (!SendHCA("HCS AUTO", 3000)) return FALSE;
	if (!SendHCA("EDER ON", 3000)) return FALSE;
	if (!SendHCA("HCS BUTTON_1_REPORT OFF", 3000)) return FALSE;
	if (!SendHCA("HCS BUTTON_2_REPORT OFF", 3000)) return FALSE;
	ClearAllLED();
	CLPObj::ACCESSMODE am = pGDev->pGEMObjs->GetLPAccessMode(unit);
	AccessModeChange(am);
	return TRUE;
}

BOOL CComFL300::SetDefault() {
	if (!_SetDefault()) {
		if (pGDev) {
			CString msg;
			msg.Format("LP%d fail to response", unit);
			switch (unit) {
			case 1:
				pGDev->SetAlarm(CAlrmList::LP1COMMFAILURE, msg);
				break;
			case 2:
				pGDev->SetAlarm(CAlrmList::LP2COMMFAILURE, msg);
				break;
			}
			pGDev->pLTMgr->SetGenAlarm(TRUE);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CComFL300::Close() {
	switch (AccMode) {
	case AMMANUAL:
		if (SendHCA("HCS CLOSE", CLOSEFOUPTIMEOUT)) {
			pGDev->SetLPState(unit, PrcsMgrServices::UNDOCKING);
			RefreshLED();
			return TRUE;
		}
		break;
	case AMAUTO:
		if (SendAck("HCS CLOSE_NO_RELEASE", CLOSEFOUPTIMEOUT)) {
			pGDev->SetLPState(unit, PrcsMgrServices::UNDOCKING);
			RefreshLED();
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CComFL300::Reset(BOOL bCalib) {
	int cnt = 1;

	if (!SendHCA("HCS RESET", 6000)) return FALSE;
	Sleep(4000);
	while (!SendHCA("HCS AUTO", 6000)) {
		Sleep(1000);
	}

	bCarrierPresent = FALSE;
	CString str, str1;
	if (!bCalib) {
		if (SendHCA("FSR FC=0", 6000)) {
			Sleep(200);	// Give some time for load port to response [3/16/2012 Administrator]
			str = GetResp();
			if (str.Find("CIP=TRUE") != -1) {
				bCarrierPresent = TRUE;
				// Remove old method [3/16/2012 Administrator]
				//if (str.Find("PODPOS=HOME") == -1)
				//{
				//	// FOUP is possibly docked
				//	pGDev->SetLPState(unit,PrcsMgrServices::OPENED);
				//	RefreshLED();
				//}
				//else {
				//	// FOUP is possibly un-docked
				//	SendHCA("HCS APHD_RELEASE",6000);
				//	pGDev->SetLPState(unit,PrcsMgrServices::UNCLAMPPED);
				//	RefreshLED();
				//}

				// New method [3/16/2012 Administrator]
				if (str.Find("DOORST=OPEN") != -1) {
					// Door is open [3/16/2012 Administrator]
					pGDev->SetLPState(unit, PrcsMgrServices::OPENED);
					RefreshLED();
				}
				else {
					// Door is close [3/16/2012 Administrator]
					SendHCA("HCS APHD_RELEASE", 6000);
					CString str2;
					// Look for HCA OK [3/16/2012 Administrator]
					while (1) {
						str2 = GetResp();
						if (str2.Find("HCA") != -1) {
							break;
						}
						Sleep(100);
					}
					pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPPED);
					RefreshLED();
				}
			}
			else {
				pGDev->SetLPState(unit, PrcsMgrServices::NOFOUP);
				RefreshLED();
			}
		}
		return TRUE;
	}
	char text[256];
	if (SendHCA("FSR FC=0", 6000)) {
		str = GetResp();
		str1 = str;
		if (str.Find("HOME=TRUE") == -1) {
			if (SendHCA("HCS MANUAL", 6000)) {	// 2 reply  from load port
				if (GetRespLFFoup(&text[0], 255, 6000)) {
					SendHCA("HCS VACUUM_OFF", 6000);
					Sleep(1000);
					// 					SendHCA("HCS CALIB",6000);
					// 					Sleep(45000);
					// 					// block until load port reply
					// 					if (GetRespLFFoup(&text[0],255,480000))
					// 					{
					// 						str = text;
					// 						if (str.Find("CMPL_CAL") != -1)
					// 						{
					// 						}
					// 						else
					// 						{
					// 							// Something else wrong
					// 							SendHCA("HCS AUTO",6000);
					// 							//ASSERT(0);
					// 							//return FALSE;
					// 						}
					// 					}
					// 					else
					// 					{
					// 						// HCS CALIB timeout, error
					// 						SendHCA("HCS AUTO",6000);
					// 						ASSERT(0);
					// 						return FALSE;
					// 					}
				}
				else {
					// HCS MANUAL timeout
					SendHCA("HCS AUTO", 6000);
					ASSERT(0);
					return FALSE;
				}
			}
		}
		SendHCA("HCS AUTO", 6000);
		Sleep(200); // Wait till LP is ready
		if (SendHCA("FSR FC=0", 6000)) {
			str = GetResp();
			if (str.Find("CIP=TRUE") != -1) {
				bCarrierPresent = TRUE;
				if (str.Find("DOORST=OPEN") != -1) {
					// Door is open [3/16/2012 Administrator]
					pGDev->SetLPState(unit, PrcsMgrServices::OPENED);
					RefreshLED();
				}
				else {
					if (pGDev->pGEMObjs->GetLPAccessMode(unit) == CLPObj::MANUAL) {
						SendHCA("HCS APHD_RELEASE", 6000);
						// 						CString str2;
						// 						// Look for HCA OK [3/16/2012 Administrator]
						// 						while (1)
						// 						{
						// 							str2 = GetResp();
						// 							if (str2.Find("HCA") != -1)
						// 							{
						// 								break;
						// 							}
						// 							Sleep(100);
						// 						}
						pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPPED);
						RefreshLED();
						//pGDev->pGEMObjs->SetLPTransferStatus(unit,CLPObj::READYTOUNLOAD);
					}
					else {
						SendHCA("HCS APHD_LATCH", 6000);
						// 						CString str2;
						// 						// Look for HCA OK [3/16/2012 Administrator]
						// 						while (1)
						// 						{
						// 							str2 = GetResp();
						// 							if (str2.Find("HCA") != -1)
						// 							{
						// 								break;
						// 							}
						// 							Sleep(100);
						// 						}
						pGDev->SetLPState(unit, PrcsMgrServices::CLAMPPED);
						RefreshLED();
						// If carrier is detected, scenario will proceed to load complete [4/20/2012 Yuen]
					}
					// 					pGDev->pGEMObjs->SetLPTransferStatus(unit,CLPObj::READYTOUNLOAD);
				}
			}
			else {
				pGDev->SetLPState(unit, PrcsMgrServices::NOFOUP);
				RefreshLED();
				pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::READYTOLOAD);
			}
		}
		//RefreshLED();
		return TRUE;
	}
	SendHCA("HCS AUTO", 6000);
	return FALSE;
}

BOOL CComFL300::Auto() {
	return SendHCA("HCS AUTO", 6000);
}

BOOL CComFL300::PostMsg(HWND hWNd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (hWNd) {
		return ::PostMessage(hWNd, Msg, wParam, lParam);
	}
	return FALSE;
}

CString CComFL300::GetOnOff(char ch) {
	switch (ch) {
	case '1':
		return "ON";
		break;
	case '2':
		return "BLINK";
		break;
	default:
		return "OFF";
		break;
	}
	return "OFF";
}

BOOL CComFL300::SendMsg(CSMHdr& SM) {
	return TRUE;
}

BOOL CComFL300::Connect(short port) {
	if (port == 0) {
		if (pCom) {
			pCom->Close();
			delete pCom;
			pCom = NULL;
		}
		return TRUE;
	}

	CSingleLock Lock(&CS);
	Lock.Lock();
	if (!pCom) {
		pCom = new CComm;
		if (!pCom) {
			ASSERT(0);
			return FALSE;
		}
	}
	pCom->SetParam(char(port), 9600, 8, 0, 1, 0);
	if (pCom->Open()) {
		return CComBase::Connect(port);
	}
	delete pCom;
	pCom = NULL;
	return FALSE;
}
BOOL CComFL300::Disconnect() {
	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		return TRUE;
	}
	return TRUE;
}

BOOL CComFL300::EnableButtonReport() {
	if (AccMode == AMAUTO) {
		return TRUE;
	}
	SendHCA("HCS BUTTON_1_REPORT ON", 2000);
	SendHCA("HCS BUTTON_2_REPORT ON", 2000);
	//	SendHCA("HCS LED 9 ON",2000);
	return TRUE;
}

void CComFL300::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CComFL300");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CComFL300::ButtonPushSensed() {
	CLPObj::TRANSFERSTATUS st = pGDev->pGEMObjs->GetLPTransferStatus(unit);

	if (st == CLPObj::READYTOUNLOAD) {
		pGDev->SetLPState(unit, PrcsMgrServices::BUTTONPRESSED);
	}
	else {
		pGDev->SetLPState(unit, PrcsMgrServices::BUTTONPRESSED);
	}
	PostMsg(hWnd, WM_COMMAND, ID_COM_BUTTONPUSHED, NULL);
	RefreshLED();
}

BOOL CComFL300::IsOperatable() {
	if (!pGDev->AramsE95.IsOperatable()) {
		PostMsg(hWnd, WM_COMMAND, ID_COM_EQPOUTOFSERVICE, NULL);
		return FALSE;
	}
	if (pGDev->pGEMObjs->GetLPTransferStatus(unit) == CLPObj::OUTOFSERVICE) {
		PostMsg(hWnd, WM_COMMAND, ID_COM_LPOUTOFSERVICE, NULL);
		return FALSE;
	}
	return TRUE;
}

void CComFL300::RefreshLED() {
	CString state = "";
	switch (pGDev->GetLPState(unit)) {
	case PrcsMgrServices::NOFOUP:
		state = "NoFoup";
		break;
	case PrcsMgrServices::FOUPPRESENT:
		state = "FoupPresent";
		break;
	case PrcsMgrServices::BUTTONPRESSED:
		state = "ButtonPressed2";
		break;
	case PrcsMgrServices::CLAMPPING:
		state = "Clamping";
		break;
	case PrcsMgrServices::CLAMPPED:
		state = "Clampped";
		break;
	case PrcsMgrServices::DOCKING:
		state = "Docking";
		break;
	case PrcsMgrServices::DOCKED:
		state = "Docked";
		break;
	case PrcsMgrServices::OPENED:
		state = "Opened";
		if (AccMode == AMMANUAL) {
			//SendHCA("HCS LED 2 ON",6000);
			LEDLOD(TRUE);
		}
		else {
			//SendHCA("HCS LED 2 OFF",6000);
			LEDLOD(FALSE);
		}
		break;
	case PrcsMgrServices::UNDOCKING:
		state = "Undocking";
		break;
	case PrcsMgrServices::UNDOCKED:
		state = "Undocked";
		break;
	case PrcsMgrServices::UNCLAMPPING:
		state = "Unclamping";
		break;
	case PrcsMgrServices::UNCLAMPPED:
		state = "Unclamped";
		break;
	}
	CString str;
	str = GetLPLight(state);
	CString inst;
	inst.Format("HCS LED %d %s", RTLLed, GetOnOff(str.GetAt(0)));
	SendHCA(inst, 6000, FALSE);
	inst.Format("HCS LED %d %s", RTULed, GetOnOff(str.GetAt(1)));
	SendHCA(inst, 6000, FALSE);
	inst.Format("HCS LED %d %s", LODLed, GetOnOff(str.GetAt(2)));
	SendHCA(inst, 6000, FALSE);
}

CString CComFL300::GetLPLight(CString state) {
	switch (AccMode) {
	case AMMANUAL:
		return DosUtil.GetLPLight(state, "000");
		break;
	case AMAUTO:
		return DosUtil.GetLPLightAuto(state, "000");
		break;
	}
	return "";
}

void CComFL300::ProcessCarrierArrived() {
	if (!IsOperatable()) {
		SendHCA("HCS LED 6 BLINK", 6000, FALSE);
		return;  // no response if system is down
	}
	// 	pGDev->pGEMObjs->Cms[unit].CaObj.Preset();  // Added [11/25/2010 XPMUser]
	// 	pGDev->pOLnClt->GEMEventMaterialRecieved(unit);
	pGDev->SetLPState(unit, PrcsMgrServices::FOUPPRESENT);
	EnableButtonReport();
	PostMsg(hWnd, WM_COMMAND, ID_COM_CARRIERARRIVED, NULL);
	RefreshLED();
	Status();
}

void CComFL300::ProcessCarrierRemoved() {
	if (pGDev->pGEMObjs->GetLPTransferStatus(unit) != CLPObj::TRANSFERBLOCKED) {
		pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::TRANSFERBLOCKED);
	}
	//SendHCA("HCS LED 6 OFF",6000);  //switch off alarm no matter what
	LEDAlarm(FALSE);
	//SendHCA("HCS LED 9 OFF", 6000); // Just in case if button is not pressed
	LEDLOD(FALSE);
	// 	pGDev->pOLnClt->GEMEventMaterialRemoved(unit);
	pGDev->SetLPState(unit, PrcsMgrServices::NOFOUP);
	PostMsg(hWnd, WM_COMMAND, ID_COM_CARRIERREMOVED, NULL);
	RefreshLED();
	Status();
}

void CComFL300::ProcessEndOpen() {
	pGDev->pOLnClt->GEMEventFoupOpened(unit);
	pGDev->SetLPState(unit, PrcsMgrServices::OPENED);
	if (AccMode == AMMANUAL) {
		//SendHCA("HCS LED 2 ON",6000);
		LEDDOP(TRUE);
	}
	PostMsg(hWnd, WM_COMMAND, ID_COM_ENDOPEN, NULL);
	RefreshLED();
	int ack;
	if (ReadMap()) {
		ack = ID_COM_SLOTMAP;
	}
	else {
		ack = ID_COM_SLOTMAPFAIL;
	}
	PostMsg(hWnd, ack, unit, (long)SlotMap);
}

void CComFL300::ProcessEndClose() {
	pGDev->pOLnClt->GEMEventFoupClosed(unit);
	pGDev->SetLPState(unit, PrcsMgrServices::UNDOCKED);
	// 	RefreshLED();q
	pGDev->pOLnClt->GEMEventFoupUndocked(unit);
	pGDev->SetLPState(unit, PrcsMgrServices::UNCLAMPPED);
	pGDev->pOLnClt->GEMEventFoupUnclamped(unit);
	//SendHCA("HCS LED 2 OFF",6000);
	LEDLOD(FALSE);
	pGDev->pGEMObjs->SetLPTransferStatus(unit, CLPObj::READYTOUNLOAD);
	PostMsg(hWnd, WM_COMMAND, ID_COM_ENDCLOSE, NULL);
	RefreshLED();
	int ack;
	if (ReadMap()) {
		ack = ID_COM_SLOTMAP;
	}
	else {
		ack = ID_COM_SLOTMAPFAIL;
	}
	PostMsg(hWnd, ack, unit, (long)SlotMap);
}

void CComFL300::ProcessButtonPushed() {
	if (AccMode == AMAUTO) {
		pGDev->pOLnClt->GEMEventAccessModeViolation(unit);
		return;
	}
	pGDev->SetLPState(unit, PrcsMgrServices::BUTTONPRESSED);
	RefreshLED();
	SendAck("HCS BUTTON_1_REPORT OFF", 8000);
	SendHCA("HCS BUTTON_2_REPORT OFF", 8000);
	//SendHCA("HCS LED 9 OFF",6000);
	LEDLOD(FALSE);
	ButtonPushSensed();
}

BOOL CComFL300::ClampEx() {
	bClampResponse = FALSE;
	CString inst;
	inst = "HCS APHD_LATCH";
	if (SendAck(inst, LATCHFOUPTIMEOUT)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CComFL300::UnclampEx() {
	bUnclampResponse = FALSE;
	CString inst;
	inst = "HCS APHD_RELEASE";
	if (SendAck(inst, LATCHFOUPTIMEOUT)) {
		return TRUE;
	}
	return FALSE;
}