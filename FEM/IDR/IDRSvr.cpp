// IDRSvr.cpp : implementation file
//

#include "stdafx.h"

#include "IPC/SMCHdr.h"
#include "IPC/SMHdr.h"
#include "IPC/SMSvr.h"
#include "SRC/DOSUtil.h"
#include "IDRAsyst.h"
#include "IDRUnitKey.h"	// Added by ClassView
#include "IDRUnitOmron.h"	// Added by ClassView

#include "IDRSvr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIDRSvr

//IMPLEMENT_DYNCREATE(CIDRSvr, CSvrThread)

CIDRSvr::CIDRSvr() {
	type = 1;
	IDRUnit = 0;
	SvrName = "CIDRSvr";

	Unit = 0;
	Port = 0;
}

CIDRSvr::~CIDRSvr() {
	if (IDRUnit) {
		delete IDRUnit;
	}
}

BOOL CIDRSvr::InitInstance() {
	ASSERT(Unit);

	SvrName.Format("CIDRSvr%d", Unit);

	CSvrThread::InitInstance();

	return TRUE;
}

int CIDRSvr::ExitInstance() {
	if (IDRUnit) {
		IDRUnit->Stop();
	}

	return CSvrThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CIDRSvr, CSvrThread)
	//{{AFX_MSG_MAP(CIDRSvr)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIDRSvr message handlers

BOOL CIDRSvr::ProcessCmd(CSMOL& SM) {
	switch (SM.cmd) {
	case CSMOL::cmdPRCSMGRSERVICES:
		return ExecPrcsMgrCmd(SM);
		break;
	}
	return TRUE;
}

BOOL CIDRSvr::ExecPrcsMgrCmd(CSMOL& SM) {
	BOOL ret;
	CSMHdr SM1;
	switch (SM.Pm.act) {
	case PrcsMgrServices::READCARRIERID:
		ipc->Done();	// To prevent main thread from lockup due to dailog instantiation down stream
		if (IDRUnit) {
			Reply(SM, IDRUnit->ReadCarrierID());
		}
		else Reply(SM, FALSE);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		return FALSE;
		break;
	case PrcsMgrServices::REGISTERWIN:
		SM1.cmd = CSMHdr::CTREGISTER;
		SetReply(SM1, SM);
		IDRUnit->Register(SM1);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		break;
	case PrcsMgrServices::WRITECARRIERID:
		ipc->Done();
		if (IDRUnit) {
			ret = IDRUnit->WriteCarrierID(SM.Pm.text);
			Reply(SM, ret);
		}
		else Reply(SM, FALSE);
		if (SM.exe == CSMHdr::REQ) ASSERT(0);
		return FALSE;
		break;
	case PrcsMgrServices::WRITETAGDATA:
		break;
	case PrcsMgrServices::READTAGDATA:
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CIDRSvr::Reply(CSMOL& SM, BOOL ret) {
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

void CIDRSvr::CleanUp() {}

//void CIDRSvr::Log(CString msg, short lvl)
//{
//	CSMClt ipc;
//	if (ipc.Start1("LogSvr",sizeof(CSMHdr))) {
//		ipc.LogMsg("CIDRSvr","",msg,lvl);
//	}
//}

BOOL CIDRSvr::StartUnit(CSMOL& SM) {
	CString type;

	type = DosUtil.GetCIDRType();

	if (type == "Asyst") {
		IDRUnit = new CIDRAsyst;
	}
	else if (type == "Omron") {
		IDRUnit = new CIDRUnitOmron;
	}
	else if (type == "Keyence") {
		IDRUnit = new CIDRUnitKey;
	}
	else {
		IDRUnit = NULL;
	}

	if (IDRUnit) {
		BOOL ret = IDRUnit->Start(Unit, Port);
		ErrorStr = IDRUnit->ErrorStr;
		Reply(SM, ret);
		return ret;
	}
	return FALSE;
}

void CIDRSvr::SetReply(CSMHdr& SM1, CSMOL& SM) {
	SM1.Ack = SM.Ack;
	SM1.Nak = SM.Nak;
	SM1.HWin = SM.HWin;
}