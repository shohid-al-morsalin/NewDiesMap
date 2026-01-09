// LogFEM.cpp: implementation of the CLogFEM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LOG/LogSvr.h"
#include "IPC/SMHdr.h"
#include "IPC/SMCHdr.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "LogFEM.h"
// CLogFEM Log;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFEM::CLogFEM() {
	// 	pLog = this;
}

CLogFEM::~CLogFEM() {
	if (pIPC) {
		delete pIPC;
		pIPC = NULL;
	}
}

void CLogFEM::Log(CString& msg, short lvl, CString src, short id) {
	if (pIPC) {
		pIPC->LogMsg(src, "", msg, lvl, id);
		return;
	}
	if (pGDev && pGDev->pLogSvr) {
		pIPC = new CSMClt;
		if (pIPC) {
			if (pIPC->Start1(pGDev->pLogSvr->SvrName, sizeof(CSMHdr))) {
				pIPC->LogMsg(src, "", msg, lvl, id);
			}
		}
	}
}

void CLogFEM::Log(CString& msg, short lvl) {
	Log(msg, lvl, "", 0);
}

void CLogFEM::Log(CSMHdr& SM) {
	if (pGDev && pGDev->pLogSvr) {
		if (pIPC) {
			pIPC->Send(&SM);
			return;
		}
		pIPC = new CSMClt;
		if (pIPC) {
			if (pIPC->Start1(pGDev->pLogSvr->SvrName, sizeof(CSMHdr))) {
				pIPC->Send(&SM);
			}
		}
	}
}

void CLogFEM::LogLocal(CString& msg, short level) {
	if (pUDev) {
		pUDev->EqpMessage(msg, level);
	}
}