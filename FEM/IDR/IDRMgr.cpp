// IDRMgr.cpp : implementation file
//
#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "IDRSvr.h"
#include "IPC/SMCHdr.h"
#include "IPC/SMHdr.h"
#include "IPC/SMSvr.h"
#include "SRC/DOSUtil.h"

#include "IDRMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CIDRMgr
CIDRMgr* pCIDRMgr = NULL;

//IMPLEMENT_DYNCREATE(CIDRMgr, CSvrThread)

CIDRMgr::CIDRMgr() {
	DosUtil.SetCIDRType(DosUtil.GetCIDRType());

	type = 1;
	SvrName = "CIDRMgr";
	nCIDR = 0;
	for (int i = 0; i < 4; i++) {
		pCIDRSvr[i] = NULL;
	}
}

CIDRMgr::~CIDRMgr() {
	StopCIDRs();
}

BEGIN_MESSAGE_MAP(CIDRMgr, CSvrThread)
	//{{AFX_MSG_MAP(CIDRMgr)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP();

/////////////////////////////////////////////////////////////////////////////
// CIDRMgr message handlers
BOOL CIDRMgr::ProcessCmd(CSMOL& SM) {
	switch (SM.cmd) {
	case CSMOL::cmdPRCSMGRSERVICES:
		ExecPrcsMgrCmd(SM);
		break;
	}
	return TRUE;
}

BOOL CIDRMgr::StartCIDRs(CSMOL& SM) {
	short unit;
	short port;
	unit = SM.Pm.unit;
	port = SM.Pm.port;
	if ((unit < 1) || (unit > 4)) {
		ErrorStr.Format("Unit (%d) out of range", unit);
		Log(ErrorStr, 2);
		ASSERT(0);
		return FALSE;
	}
	int i = unit - 1;
	if (pCIDRSvr[i]) {
		ErrorStr.Format("Unit (%d) already exist", unit);
		Log(ErrorStr, 2);
		ASSERT(0);
		return FALSE;
	}
	pCIDRSvr[i] = new CIDRSvr;
	if (!pCIDRSvr[i]) {
		ErrorStr.Format("Unit (%d) alloc error", unit);
		Log(ErrorStr, 2);
		ASSERT(0);
		return FALSE;
	}
	pCIDRSvr[i]->Unit = unit;
	pCIDRSvr[i]->Port = port;
	//pCIDRSvr[i]->SM = SM;
	if (!CSMClt::StartSvrThread(pCIDRSvr[i])) {
		delete pCIDRSvr[i];
		pCIDRSvr[i] = NULL;
		ErrorStr.Format("Unit (%d) fail to start", unit);
		Log(ErrorStr, 0);
		ASSERT(0);
		return FALSE;
	}
	while (!pCIDRSvr[i]->bStarted) {
		Sleep(10);
	}
	nCIDR++;
	ErrorStr.Format("Unit (%d) created", unit);
	Log(ErrorStr, 10);

	pCIDRSvr[i]->StartUnit(SM);

	// Operational test here

	return TRUE;
}

BOOL CIDRMgr::StopCIDRs() {
	for (int i = 0; i < 4; i++) {
		if (pCIDRSvr[i]) {
			if (!CSMClt::StopSvrThread(pCIDRSvr[i]->SvrName, pCIDRSvr[i]->type)) {
				ErrorStr.Format("Unit (%d) fail to stop", i + 1);
				Log(ErrorStr, 0);
				ASSERT(0);
			}
			pCIDRSvr[i] = NULL;
			CString msg;
			msg.Format("LPSvr (%d) stopped", i + 1);
			Log(msg, 10);
		}
	}
	ErrorStr.Format("All Unit (%d) stopped", nCIDR);
	Log(ErrorStr, 10);
	return TRUE;
}

BOOL CIDRMgr::ExecPrcsMgrCmd(CSMOL& SM) {
	CString msg;
	CIDRSvr* pSvr;
	CSMClt ipc1;
	switch (SM.Pm.act) {
	case PrcsMgrServices::STARTUNIT:
		if (SM.exe == CSMHdr::REQ) {
			ASSERT(0);
		}
		ipc->Done();
		StartCIDRs(SM);
		return FALSE;
		break;
	case PrcsMgrServices::REGISTERWIN:
		msg.Format("Register CIDR Unit #%d", SM.Pm.unit);
		Log(msg, 10);
		SendMsg(pCIDRSvr[SM.Pm.unit - 1], SM);
		break;
	case PrcsMgrServices::READCARRIERID:
		if (!ValidUnit(SM)) {
			Reply(SM, FALSE);
			return FALSE;
		}
		Log("Reading carrier ID", 10);
		pSvr = pCIDRSvr[SM.Pm.unit - 1];
		if (!pSvr) {
			Reply(SM, FALSE);
			return FALSE;
		}
		if (ipc1.Start1(pSvr->SvrName, sizeof(CSMOL))) {
			if (!ipc1.Send(&SM)) {
				Reply(SM, FALSE);
			}
		}
		break;
	}
	return TRUE;
}

BOOL CIDRMgr::ValidUnit(CSMOL& SM) {
	short unit = SM.Pm.unit;
	if (unit >= 1 && unit <= 4) {
		return TRUE;
	}
	Log("Invalid unit number specified", 2);
	Reply(SM, FALSE);
	return FALSE;
}

BOOL CIDRMgr::Reply(CSMOL& SM, BOOL ret) {
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

void CIDRMgr::CleanUp() {}

void CIDRMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CIDRMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CIDRMgr::SendMsg(CIDRSvr* pSvr, CSMOL& SM) {
	if (pSvr) {
		CSMClt ipc;
		if (ipc.Start1(pSvr->SvrName, sizeof(CSMOL))) {
			if (ipc.Send(&SM)) {
				return TRUE;
			}
		}
	}
	ASSERT(0);
	Reply(SM, FALSE);
	return FALSE;
}