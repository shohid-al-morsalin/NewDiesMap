#include "stdafx.h"
#include "resource.h"

// #include "ComFL300.h"
// #include "ComBase.h"
// #include "ComThread.h"
#include "IPC/SubstObj.h"
#include "SRC/Comm.h"

#include "ComTDK.h"

CComTDK::CComTDK() {}

CComTDK::~CComTDK() {}

BOOL CComTDK::SetDefault() {
	ClearAllLED();
	Reset();
	// Does force initial position
	Calib();
	return TRUE;
}

void CComTDK::Decode(CString& msg) {
	CString inst;
	if (msg.Find("s00ABS:") != -1) {
		// We have error, handle and recover
		if (msg.Find("/SAFTY") != -1) {
			Reset();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_SENSORTRIP, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_COM_SENSORTRIP, NULL);
			}
		}
		if (msg.Find("/YLMIT") != -1) {
			Reset();
			if (hWnd) {
				// Double send because this instant do not know which state the LP is in
				PostMsg(hWnd, WM_COMMAND, ID_LP_FOUPOPENFAILED, NULL);
				PostMsg(hWnd, WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
			}
			if (hWndLPBar) {
				// Double send because this instant do not know which state the LP is in
				PostMsg(hWndLPBar, WM_COMMAND, ID_LP_FOUPOPENFAILED, NULL);
				PostMsg(hWndLPBar, WM_COMMAND, ID_LP_FOUPCLOSEFAILED, NULL);
			}
		}
		if (msg.Find("/VACCS") != -1) {
			Reset();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_VACUUMLOST, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_COM_VACUUMLOST, NULL);
			}
		}
		if (msg.Find("/AIRSN") != -1) {
			Reset();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_NOCOMPRESSAIR, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_COM_NOCOMPRESSAIR, NULL);
			}
		}
		if (msg.Find("/PROTS") != -1) {
			Reset();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_WAFEROUT, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_COM_WAFEROUT, NULL);
			}
		}
		if (msg.Find("/INTCL") != -1) {
			Reset();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_LPBAR_CARRIERLOST, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_LPBAR_CARRIERLOST, NULL);
			}
		}
	}
	if (msg.Find("s00INF:") != -1) {
		if (msg.Find("SMTON;") != -1) {
			// Carrier Presence triggered
			bInPlace = TRUE;
		}
		// We have info
		if (msg.Find("ABNST;") != -1) {
			// Carrier Placement sensor
			bReady = TRUE;
		}
		if (msg.Find("PODON;") != -1) {
			// Carrier Arrived
			bOpen = TRUE;
			Status();
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_CARRIERARRIVED, NULL);
			}
			if (hWndLPBar) {
				PostMsg(hWndLPBar, WM_COMMAND, ID_LP_GETLPSTATUSOK, NULL);
			}
		}
		if (msg.Find("PODOF;") != -1) {
			bInPlace = FALSE;
			bReady = FALSE;
			// Carrier Arrived
			// Get the seme event also the presence /plcement sensor is cleared
			if (bOpen) {
				bOpen = FALSE;
				Status();
				if (hWnd) {
					PostMsg(hWnd, WM_COMMAND, ID_COM_CARRIERREMOVED, NULL);
				}
				if (hWndLPBar) {
					PostMsg(hWndLPBar, WM_COMMAND, ID_LP_GETLPSTATUSOK, NULL);
				}
			}
		}
		if (msg.Find("CLDMP;") != -1) {
			// Loading is completd
			// Need to check error status
			//SendTDK("s00FIN:CLDMP;");
			if (HasError()) {
				// Loading has error
				ShowError();
				return;
			}
			ReadMap(TRUE);
			if (hWnd) {
				// NO seperate message after map is completed
				PostMsg(hWnd, WM_COMMAND, ID_COM_COMPLETEMAP, NULL);
				PostMsg(hWnd, WM_COMMAND, ID_COM_ENDOPEN, NULL);
			}
			return;
		}
		if (msg.Find("CULFC;") != -1) {
			// From close no release
			//SendTDK("s00FIN:CULFC;");
			if (HasError()) {
				ShowError();
				return;
			}
			ReadMap(TRUE);
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_ENDCLOSE, NULL);
			}
			return;
		}
		if (msg.Find("MANSW;") != -1) {
			// We have button pressed
			//SendTDK("s00FIN:MANSW;");
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_BUTTONPUSHED, NULL);
			}
		}
		if (msg.Find("PODCL;") != -1) {
			// Clamp completed
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_LATCHCMPL, NULL);
			}
		}
		if (msg.Find("PODOP;") != -1) {
			// Clamp released
			if (hWnd) {
				PostMsg(hWnd, WM_COMMAND, ID_COM_RELEASECMPL, NULL);
			}
		}
	}
}

BOOL CComTDK::Auto() {
	return TRUE;
}

BOOL CComTDK::Reset(BOOL bCalib) {
	char SMap[512];
	if (ReadTDK("s00SET:RESET;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:RESET;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Open() {
	char SMap[512];
	if (ReadTDK("s00MOV:CLDMP;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:CLDMP;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Close() {
	char SMap[512];
	if (ReadTDK("s00MOV:CULOD;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:CULOD;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Map() {
	char SMap[512];
	if (ReadTDK("s00MOV:CLDMP;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:CLDMP;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::NoVac() {
	// TDK does not work w/o vac
	return TRUE;
}

BOOL CComTDK::NeedVac() {
	return TRUE;
}

BOOL CComTDK::CloseNoRelease() {
	char SMap[512];
	if (ReadTDK("s00MOV:CULFC;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:CULFC;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Calib() {
	char SMap[512];
	if (ReadTDK("s00MOV:ABORG;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:ABORG;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Status() {
	char SMap[512];
	if (ReadTDK("s00GET:STATE;", (char*)SMap, sizeof(SMap))) {
		//                   ID ACK       Bits[20]
		// Response will be 's00ACK:STATE/00100000101000101010;
		if (::strstr(SMap, "ACK:STATE/")) {
			// We have correct response
			if (::strlen(SMap) >= 30) {
				switch (SMap[19]) {
				case '0':
					bInPlace = FALSE;
					bReady = TRUE;
					break;
				case '1':
					bInPlace = TRUE;
					bReady = TRUE;
					bOpen = TRUE;
					break;
				case '2':
					bInPlace = TRUE;
					bReady = FALSE;
					break;
				default:
					bInPlace = TRUE;
					bReady = TRUE;
					break;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComTDK::Unclamp() {
	char SMap[512];

	if (ReadTDK("s00MOV:PODOP;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:PODOP;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::Clamp() {
	char SMap[512];

	if (ReadTDK("s00MOV:PODCL;", (char*)SMap, sizeof(SMap))) {
		if (::strstr(SMap, "s00ACK:PODCL;")) {
			return TRUE;
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return FALSE;
}

BOOL CComTDK::AccessModeChange(CLPObj::ACCESSMODE mode) {
	//Log("Access mode change is not available on TDK",3);
	return TRUE;
}

//BOOL CComTDK::LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState)
//{
//	CString str;
//	CString inst;
//
//
//	switch(LpState)
//	{
//	case PrcsMgrServices::NOFOUP:
//		str = DosUtil.GetLPLight("NoFoup","101");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;",GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::FOUPPRESENT:
//		str = DosUtil.GetLPLight("FoupPresent","002");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::BUTTONPRESSED1:
//		str = DosUtil.GetLPLight("ButtonPressed1","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::CLAMPPED:
//		str = DosUtil.GetLPLight("Clampped","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::DOCKING:
//		str = DosUtil.GetLPLight("Docking","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::DOCKED:
//		str = DosUtil.GetLPLight("Docked","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
////		if (DOPLed) {
////			inst.Format("HCS LED %d %s",DOPLed,"ON");
////			SendAck(inst);
////		}
//		return TRUE;
//		break;
//	case PrcsMgrServices::UNDOCKING:
//		str = DosUtil.GetLPLight("Undocking","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::UNDOCKED:
//		str = DosUtil.GetLPLight("Undocked","022");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::BUTTONPRESSED2:
//		str = DosUtil.GetLPLight("ButtonPressed2","022");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//	case PrcsMgrServices::UNCLAMPPED:
//		str = DosUtil.GetLPLight("Unclamped","020");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::UNCLAMPPING:
//		str = DosUtil.GetLPLight("Unclamping","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	case PrcsMgrServices::CLAMPPING:
//		str = DosUtil.GetLPLight("Clamping","000");
//		if (RTLLed)
//		{
//			inst.Format("s00SET:%sLOD;",GetOnOff(str.GetAt(0)));
//			SendTDK(inst,2);
//		}
//		if (RTULed)
//		{
//			inst.Format("s00SET:%sULD;",GetOnOff(str.GetAt(1)));
//			SendTDK(inst,2);
//		}
//		inst.Format("s00SET:%sMSW;", GetOnOff(str.GetAt(2)));
//		SendTDK(inst,2);
//		return TRUE;
//		break;
//	}
//	return FALSE;
//}

BOOL CComTDK::LEDManual(BOOL on) {
	//Log("Manual Indicator is not available on TDK",3);
	return TRUE;
}

BOOL CComTDK::LEDReserved(BOOL on) {
	//Log("Reserved Indicator is not available on TDK",3);
	return TRUE;
}

BOOL CComTDK::LEDAlarm(BOOL on) {
	//Log("Alarm Indicator is not available on TDK",3);
	return TRUE;
}

BOOL CComTDK::ClearAllLED() {
	// LYF:Does not appears to be any valid command
//	CString Inst;
//	Inst.Format("LOLOD");
//	SendTDK(Inst);
//	Inst.Format("LOULD");
//	SendTDK(Inst);
//	Inst.Format("LOMSW");
//	SendTDK(Inst);
	return TRUE;
}

BOOL CComTDK::EnableHOAvbl() {
	//Log("HO Available is not accessible on TDK",3);
	return TRUE;
}

BOOL CComTDK::EnableButtonReport() {
	//Log("HO Available is not accessible on TDK",3);
	return TRUE;
}

BOOL CComTDK::PostMsg(HWND hWNd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	BOOL ret = ::PostMessage(hWNd, Msg, wParam, lParam);
	return ret;
}

BOOL CComTDK::ReadMap(BOOL bFirst) {
	CString sm;
	char SMap[512];
	if (ReadTDK("s00GET:MAPRD;", SMap, sizeof(SMap))) {
		// Verify the response
		// Response will be s00ACK:MAPRD/1100000000000000000000001; if everything is OK
		if (::strstr((char*)SMap, "ACK:MAPRD/")) {
			if (ExtractSlotmap(SMap + 13)) {
				sm.Format("SM%s", SlotMap);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComTDK::ExtractSlotmap(char* SMap) {
	CString txt;
	memset(SlotMap, '0', MAXSLOTMAPLEN);
	if (MaxSlot != 25) {
		txt.Format("MaxSlot is %d, reset to 25", MaxSlot);
		//		Log(txt,2);
		MaxSlot = 25;
	}
	txt = SMap;
	//	Log(txt,2);
	int i;
	for (i = 0; i < MaxSlot; i++) {
		if (txt[i] == '0') {
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
			continue;
		}
		if (txt[i] == '1') {
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
			continue;
		}
		if (txt[i] == '2') {
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
			continue;
		}
		else {
			CString str1;
			str1.Format("Slot # %d error <%c>", i + 1, txt[i]);
			//			Log(str1,2);
		}
	}
	SlotMap[i] = 0;
	if (i > MaxSlot) {
		return TRUE;
	}
	return FALSE;
}

BOOL CComTDK::HasError() {
	char SMap[512];
	if (ReadTDK("s00GET:STATE;", (char*)SMap, sizeof(SMap))) {
		//                   ID ACK       Bits[20]
		// Response will be 's00ACK:STATE/00100000101000101010;
		if (::strstr(SMap, "ACK:STATE/")) {
			// We have correct response
			if (('0' == SMap[17]) && ('0' == SMap[18])) {
				return FALSE;
			}
		}
	}
	CString TempMsg;
	TempMsg.Format("LP Has error:<%s>", SMap);
	//	Log(TempMsg,2);
	return TRUE;
}

void CComTDK::ShowError() {
	char SMap[512];
	if (ReadTDK("s00GET:STATE;", (char*)SMap, sizeof(SMap))) {
		//                   ID ACK       Bits[20]
		// Response will be 's00ACK:STATE/00100000101000101010;
		if (::strstr(SMap, "ACK:STATE/")) {
			// We have correct response
			if ('0' == SMap[17] && '0' == SMap[18]) {
				//				Log("No Error detected",3);
			}
		}
		else if (::strstr(SMap, "NAK")) {
			ReportNAK(SMap);
		}
	}
	return;
}

void CComTDK::ReportNAK(char* SMap) {
	if (::strstr(SMap, "CKSUM")) {
		//		Log("Checksum error",2);
	}
	else if (::strstr(SMap, "CMDER")) {
		//		Log("Command error",2);
	}
	else if (::strstr(SMap, "SFSER")) {
		//		Log("Obstacle detection sensor detected",2);
	}
	else if (::strstr(SMap, "INTER")) {
		//		Log("Interlock error",2);
	}
	else if (::strstr(SMap, "CBUSY")) {
		//		Log("Communication busy (wait). The command is not accepted.",2);
	}
	else if (::strstr(SMap, "FPILG")) {
		//		Log("FOUP loading error or no FOUP",2);
	}
	else if (::strstr(SMap, "LATCH")) {
		//		Log("Latch key error",2);
	}
	else if (::strstr(SMap, "FPCLP")) {
		//		Log("FOUP clamp error",2);
	}
	else if (::strstr(SMap, "YPOSI")) {
		//		Log("Y-axis (docking table) position error",2);
	}
	else if (::strstr(SMap, "DOCPO")) {
		//		Log("Door open/close position error (mapper frame position error)",2);
	}
	else if (::strstr(SMap, "DPOSI")) {
		//		Log("Door open/close position error",2);
	}
	else if (::strstr(SMap, "PROTS")) {
		//		Log("Wafer protrusion detected",2);
	}
	else if (::strstr(SMap, "MPARM")) {
		//		Log("Mapper arm strage error. Mapping starting position error",2);
	}
	else if (::strstr(SMap, "ZPOSI")) {
		//		Log("Z-axis (door top/bottom) position error",2);
	}
	else if (::strstr(SMap, "MPSTP")) {
		//		Log("Mapper stopper position error",2);
	}
	else if (::strstr(SMap, "DVACM")) {
		//		Log("Door - Being vacuumed.",2);
	}
	else if (::strstr(SMap, "ERROR")) {
		//		Log("Error",2);
	}
	else if (::strstr(SMap, "ORGYT")) {
		//		Log("Origin return - Not done.",2);
	}
	else if (::strstr(SMap, "SAFTY")) {
		//		Log("Load port interlock",2);
	}
	else {
		CString Temp;
		Temp.Format("Unknown error %s", SMap);
		//		Log(Temp,2);
	}
	return;
}

CString CComTDK::GetOnOff(char ch) {
	switch (ch) {
	case '1':
		return "LP";
		break;
	case '2':
		return "BL";
		break;
	default:
		return "LO";
		break;
	}
	return "LO";
}

BOOL CComTDK::SendMsg(CSMHdr& SM) {
	return TRUE;
}

BOOL CComTDK::Abort() {
	Reset();
	return Calib();
}
BOOL CComTDK::Connect(short port) {
	if (port == 0) {
		if (pCom) {
			pCom->Close();
			delete pCom;
		}
		pCom = NULL;
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
		SetDefault();
		return TRUE;
	}
	delete pCom;
	pCom = NULL;
	return FALSE;
}
BOOL CComTDK::Disconnect() {
	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		return TRUE;
	}
	return TRUE;
}