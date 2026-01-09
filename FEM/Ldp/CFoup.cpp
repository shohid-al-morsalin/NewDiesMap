#include "stdafx.h"

#include "ComBase.h"
#include "ComThread.h"
#include "FEM/IDR/IDRUnit.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMHdr.h"
#include "IPC/SMCHdr.h"
#include "resource.h"
#include "SRC/DOSUtil.h"

#include "CFoup.h"

// Callback from derived class
BOOL CFoup::Add(CSMHdr& SM) {
	if (!pComTh) {
		return FALSE;
	}
	return pComTh->AddToSMList(SM);
}

BOOL CFoup::LedManual(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::LedClearAll() {
	return TRUE;
}

BOOL CFoup::LedReserved(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Reply(CSMHdr& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

void CFoup::SetInP(BOOL bInP) {
	if (bInP) {
		bInPlace = TRUE;
	}
	else {
		bInPlace = FALSE;
	}
}

CString CFoup::GetSM() {
	CString Slotmap;
	Slotmap = DosUtil.GetSlotmap();
	for (int i = 0; i < Var.MaxSlot; i++) {
		switch (Slotmap.GetAt(i)) {
		case '0':
			Slotmap.SetAt(i, CSubstObj::GetSMChar(CSubstObj::SM_EMPTY));
			break;
		case '1':
			Slotmap.SetAt(i, CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED));
			break;
		default:
			Slotmap.SetAt(i, CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED));
			break;
		}
	}
	return Slotmap;
}

CQueOutMgr* CFoup::GetQueOutMgr() {
	return NULL;
}

BOOL CFoup::SetE84AccessMode(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

// Note:  Yuen[2002/08/01 7:24]
BOOL CFoup::Open(CSMHdr& SM) {
	// Stage 1
	Reply(SM, TRUE);
	// Stage 2
	Sleep(100);
	CSMHdr SM1;
	SM1 = SM;
	SM1.Ack = ID_COM_ENDOPEN;
	SM1.Nak = ID_COM_ABORTOPEN;
#if 1
	bOpen = TRUE;
	Reply(SM1, bOpen);
	return TRUE;
#else
	Reply(SM1, FALSE);
	return FALSE;
#endif
}

BOOL CFoup::Close(CSMHdr& SM) {
	// Stage 1
	Reply(SM, TRUE);
	Sleep(100);
	// Stage 2
	CSMHdr SM1;
	SM1 = SM;
	SM1.Ack = ID_COM_ENDCLOSE;
	SM1.Nak = ID_COM_ABORTCLOSE;
#if 1
	Reply(SM1, TRUE);
	bOpen = FALSE;
	return TRUE;
#else
	Reply(SM1, FALSE);
	return FALSE;
#endif
}

CFoup::~CFoup() {
	if (pComTh) {
		// 		HANDLE hDone; // Server informs client mutex
		// 		hDone =::CreateEvent(NULL, TRUE, FALSE, pComTh->KillName);
		// 		pComTh->PostThreadMessage(WM_QUIT, 0, NULL);
		// 		WaitForSingleObject(hDone, 60000);
		// 		CloseHandle(hDone);
		pComTh->bRun = FALSE;
		while (pComTh->bStarted == TRUE) {
			Sleep(50);
		}
		//delete pComTh; // pComTh is autodelete [3/16/2012 Administrator]
	}
}

CFoup::CFoup() {
	m_hWnd = NULL;
	m_hWndBak = NULL;
	//	m_hWndLPBar = 0;

	bOpen = FALSE;
	bNoClose = FALSE;
	bDisabled = FALSE;
	bReady = TRUE;
	bInPlace = FALSE;
	bAlarmed = FALSE;
	ErrorStr = "";
	Caller = "";

	pComTh = NULL;

	Unit = 0;
	m_hWndLPBar = 0;
}

BOOL CFoup::Initialize() {
	return TRUE;
}

BOOL CFoup::Map(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Calib(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Register(CSMHdr& SM) {
	// Register with Com Thread
	if (pComTh) {
		pComTh->pCom->hWnd = SM.HWin;
	}
	// Register with load port
	m_hWnd = SM.HWin;
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::RegisterEx(CSMHdr& SM) {
	// Register with Com Thread
	if (pComTh) {
		if (pComTh->pCom->hWndBak) {
			Log("Already registered", 9);
			if (pComTh->pCom->hWnd == SM.HWin) {
				Log("Second time registeration ignored", 9);
				return TRUE;
			}
		}
		pComTh->pCom->hWndBak = pComTh->pCom->hWnd;
		pComTh->pCom->hWnd = SM.HWin;
	}
	// Register with load port
	if (m_hWndBak) {
		ASSERT(0);
	}
	m_hWndBak = m_hWnd;
	m_hWnd = SM.HWin;
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::UnRegisterEx(CSMHdr& SM) {
	// Register with Com Thread
	if (pComTh) {
		pComTh->pCom->hWnd = pComTh->pCom->hWndBak;
		pComTh->pCom->hWndBak = NULL;
	}
	// Register with load port
	m_hWnd = m_hWndBak;
	m_hWndBak = NULL;
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::RegisterLPBar(CSMHdr& SM) {
	m_hWndLPBar = SM.HWin;
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Stop(CSMHdr& SM) {
	BOOL ret = Disconnect();
	Reply(SM, ret);
	return ret;
}

BOOL CFoup::Status(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr) {
	bRdy = bReady;
	bInP = bInPlace;
	bAlr = bAlarmed;
	return TRUE;
}

BOOL CFoup::CloseNoRelease(CSMHdr& SM) {
	CSMHdr SM1;
	SM1 = SM;
	SM1.Ack = ID_COM_CLOSENORELEASECMPL;
	Reply(SM1, TRUE);
	bOpen = TRUE;
	return TRUE;
}

BOOL CFoup::SetDefault(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

void CFoup::PushButton2(CSMHdr& SM) {
	if (m_hWnd) {
		::PostMessage(m_hWnd, WM_COMMAND, ID_COM_BUTTON1PUSHED, NULL);
	}
}

void CFoup::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CFoup");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CFoup::EnableHOAvbl(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::EnableButtonReport(CSMHdr& SM) {
	Reply(SM, TRUE);
	return TRUE;
}

//BOOL CFoup::LEDLoadUnload(CSMHdr & SM)
//{
//	Reply(SM, TRUE);
//	return TRUE;
//}
//
BOOL CFoup::Clamp(CSMHdr& SM) {
	// Stage 1
	// Reception of message
	Reply(SM, TRUE);
	// Stage 2
	// Outcome of execution
	Sleep(100);
	CSMHdr SM1;
	SM1 = SM;
	SM1.Ack = ID_COM_LATCHCMPL;
	SM1.Nak = ID_LP_LATCHFAILED; //YFTD : this is the default message if latch failed
	// Case TRUE
#if 1
	Reply(SM1, TRUE);
	bOpen = TRUE;
	return TRUE;
#else
	// Case FALSE
	Reply(SM1, FALSE);
	return FALSE;
#endif
}

BOOL CFoup::Unclamp(CSMHdr& SM) {
	// Stage 1
	Reply(SM, TRUE);
	// Stage 2
	Sleep(100);
	CSMHdr SM1;
	SM1 = SM;
	SM1.Ack = ID_COM_RELEASECMPL;
	Reply(SM1, TRUE);
	bOpen = TRUE;
	return TRUE;
}

BOOL CFoup::Connect(RPLY& rpy, short unit, short port) {
	Unit = unit;
	CSMHdr SM;
	SM.cmd = CSMHdr::CTCONNECT;
	SM.sub[0] = port;
	SM.sub[1] = Unit;
	SM.sub[2] = bNoClose;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);

	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Disconnect() {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTDISCONNECT;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Reset(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTRESET;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::Auto(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTAUTO;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::NoVac(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTNOVAC;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::NeedVac(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTNEEDVAC;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}

BOOL CFoup::StartComThread(short unit) {
	pComTh = new CComThread;
	if (pComTh) {
		pComTh->bSimu = TRUE;
		pComTh->StartCom(unit);
		// YFL:  [1/2/2005]
		if (StartServer(pComTh)) {
			return TRUE;
		}
		pComTh = NULL; // YFL: Must set this [1/2/2005]
	}
	return FALSE;
}

BOOL CFoup::StartServer(CComThread* pComTh) {
	if (pComTh) {
		pComTh->bSimu = TRUE;
		if (CSMClt::StartSvrThread(pComTh)) {
			ULONGLONG tick = GetTickCount64();
			while (pComTh->bStarted == FALSE) {
				Sleep(10);
				if ((GetTickCount64() - tick) > 60000) {
					delete pComTh;
					pComTh = NULL;
					return FALSE;
				}
			}
			return TRUE;
		}
		if (pComTh) {
			delete pComTh;
			pComTh = NULL;	// YFL: no purpose, consistancy only [1/2/2005]
		}
	}
	return FALSE;
}

//BOOL CFoup::UpdateLightTower()
//{
//	if (pLTMgr) {
//		pLTMgr->SetLight();
//	}
//	return TRUE;
//}

//BOOL CFoup::LedAlarmed(CSMHdr &SM)
//{
//	Reply(SM,TRUE);
//	return TRUE;
//}

BOOL CFoup::Abort(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTABORT;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	Reply(SM, TRUE);
	return TRUE;
}