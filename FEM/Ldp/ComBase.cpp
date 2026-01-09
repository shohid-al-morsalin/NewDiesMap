#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "IPC/SMHdr.h"
#include "SRC/Comm.h"
#include "SRC/DOSUtil.h"
#include "SYS/Var.h"

#include "ComBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CComBase::CComBase() {
	pCom = NULL;
	hWnd = NULL;
	hWndBak = NULL;
	hWndLPBar = NULL;

	unit = 0;
	AccMode = AMMANUAL;

	bIgnorDoubleSlotted = TRUE;

	bReady = FALSE;
	bInPlace = FALSE;
	bAlarmed = FALSE;
	bStarted = FALSE;
	bOpen = FALSE;
	bPresent = FALSE;
	MaxSlot = Var.MaxSlot;
	memset(SlotMap, 0, sizeof(SlotMap));

	RTLLed = 1;
	RTULed = 2;
	MANLed = 3;
	AUTLed = 4;
	DOPLed = 0;
	LODLed = 9;
	ALRLed = 6;

	RTLLed = DosUtil.GetLPUnit("RTLLed", RTLLed);
	DosUtil.SetLPUnit("RTLLed", RTLLed);
	RTULed = DosUtil.GetLPUnit("RTULed", RTULed);
	DosUtil.SetLPUnit("RTULed", RTULed);
	MANLed = DosUtil.GetLPUnit("MANUALLed", MANLed);
	DosUtil.SetLPUnit("MANUALLed", MANLed);
	AUTLed = DosUtil.GetLPUnit("AUTOLed", AUTLed);
	DosUtil.SetLPUnit("AUTOLed", AUTLed);
	DOPLed = DosUtil.GetLPUnit("DOOROPENLed", DOPLed);
	DosUtil.SetLPUnit("DOOROPENLed", DOPLed);
	ACCLed = DosUtil.GetLPUnit("ACCESSLed", ACCLed);
	DosUtil.SetLPUnit("ACCESSLed", ACCLed);
	RSVLed = DosUtil.GetLPUnit("RESERVEDLed", RSVLed);
	DosUtil.SetLPUnit("RESERVEDLed", RSVLed);
	ALRLed = DosUtil.GetLPUnit("ALARMEDLed", ALRLed);
	DosUtil.SetLPUnit("ALARMEDLed", ALRLed);
	LODLed = DosUtil.GetLPUnit("LOADLed", LODLed);
	DosUtil.SetLPUnit("LOADLed", LODLed);
	PBTPort = DosUtil.GetLPUnit("PushButton", PBTPort);
	DosUtil.SetLPUnit("PushButton", PBTPort);

	P68Auto = 1;
	P68Manual = 2;

	P68Auto = DosUtil.GetLPUnit("P68Auto", P68Auto);
	DosUtil.SetLPUnit("P68Auto", P68Auto);

	P68Manual = DosUtil.GetLPUnit("P68Manual", P68Manual);
	DosUtil.SetLPUnit("P68Manual", P68Manual);
}

CComBase::~CComBase() {
	Disconnect();
}

// Called from other thread only. Will stall if call from same thread [5/20/2002]
void CComBase::WaitTillStart() {
	while (!bStarted) {
		Sleep(100);
	}
}

BOOL CComBase::Disconnect() {
	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		return TRUE;
	}
	return TRUE;
}

BOOL CComBase::Connect(short port) {
	ClearAllLED();
	return TRUE;
}

void CComBase::CheckComIn() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCom) {
		int cnt = 0;
		char ch[2];
		CString tmp = "";
		if (pCom->ReadChr(ch) > 0) {
			if (ch[0] == '\r') {
				return;
			}
			if (ch[0] == '\n') {
				return;
			}
			if (ch[0] == 7) {
				Log("Bell recieved", 2);
				return;
			}
			tmp += ch;
			cnt++;
			ULONGLONG tick = GetTickCount64();
			while ((GetTickCount64() - tick) < 6000) {
				if (pCom->ReadChr(ch) > 0) {
					cnt++;
					if (ch[0] == '\r') {
						Decode(tmp);
						return;
					}
					else if (ch[0] == '\n') {
						Decode(tmp);
						return;
					}
					else {
						tmp += ch;
					}
					if (ch[0] == 7) {
						ch[0] = 0;
						tmp += ch;
						//							Log(tmp,3);
						Log("Bell recieved", 2);
					}
					tick = GetTickCount64();
				}
				Sleep(1);
			}
			// Char timeout occured [11/2/2002]
			Log(tmp, 2);
			Log("Inter-charecter timeout", 2);
			return;
		}
	}
}

BOOL CComBase::Read(CString msg, char* text, short max, int tout) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCom) {
		if (pCom->bConnected) {
			if (pCom->RequestFoup(msg, text, max, tout)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComBase::ReadTDK(const CString& msg, char* text, short max) {
	if (pCom) {
		if (pCom->bConnected) {
			if (pCom->RequestTDK(msg, text, max)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComBase::SendAck(CString msg, int tout, BOOL bLog) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCom) {
		if (pCom->bConnected) {
			if (pCom->SendAck(msg, tout)) {
				if (bLog) Log(msg, 5);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CComBase::SendHCA(CString msg, int tout, BOOL bLog) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCom) {
		if (pCom->bConnected) {
			if (pCom->SendHCA(msg, tout)) {
				if (bLog) Log(msg);
				return TRUE;
			}
			Log(msg);
		}
	}
	return FALSE;
}

BOOL CComBase::SendTDK(CString msg, int Rpy) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCom) {
		pCom->SendTDK(msg, Rpy);
		return TRUE;
	}
	return FALSE;
}

BOOL CComBase::Reply(CSMHdr& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			//			::PostMessage(SM.HWin, SM.Ack, 0, NULL);  // LYF  [10/23/2005]
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			//			::PostMessage(SM.HWin, SM.Nak, 0, NULL);	// LYF  [10/23/2005]
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CComBase::Reply(CSMOL& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			//			::PostMessage(SM.HWin, SM.Ack, 0, NULL);	// LYF  [10/23/2005]
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			//			::PostMessage(SM.HWin, SM.Nak, 0, NULL);	// LYF  [10/23/2005]
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

//void CComBase::Log(CString msg, short lvl)
//{
//	GDev.Log("ComBase",msg,lvl);
//}

BOOL CComBase::GetRespLFFoup(char* msg, short max, int tout) {
	if (pCom) {
		pCom->GetRespLFFoup(msg, max, tout);
		return TRUE;
	}
	return FALSE;
}

char* CComBase::GetResp() {
	if (pCom) {
		return &pCom->resp[0];
	}
	return NULL;
}

BOOL CComBase::NeedVac() {
	return TRUE;
}

BOOL CComBase::Auto() {
	return TRUE;
}

BOOL CComBase::Reset(BOOL bCalib) {
	return TRUE;
}

BOOL CComBase::Calib() {
	return TRUE;
}

BOOL CComBase::NoVac() {
	return TRUE;
}

BOOL CComBase::Open() {
	return TRUE;
}

BOOL CComBase::Close() {
	return TRUE;
}

BOOL CComBase::CloseNoRelease() {
	return TRUE;
}

BOOL CComBase::Map() {
	return TRUE;
}

BOOL CComBase::Unclamp() {
	return TRUE;
}

BOOL CComBase::Clamp() {
	return TRUE;
}

BOOL CComBase::Status() {
	return TRUE;
}

BOOL CComBase::AccessModeChange(CLPObj::ACCESSMODE mode) {
	return TRUE;
}

//BOOL CComBase::LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState)
//{
//	return TRUE;
//}

BOOL CComBase::ClearAllLED() {
	return TRUE;
}

BOOL CComBase::LEDReserved(BOOL on) {
	return TRUE;
}
BOOL CComBase::LEDManual(BOOL on) {
	return TRUE;
}

BOOL CComBase::EnableHOAvbl() {
	return TRUE;
}

BOOL CComBase::EnableButtonReport() {
	return TRUE;
}

BOOL CComBase::LEDAlarm(BOOL on) {
	return TRUE;
}
BOOL CComBase::Abort() {
	return TRUE;
}

void CComBase::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CComBase");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}