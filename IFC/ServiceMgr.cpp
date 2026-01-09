// ServiceMgr.cpp: implementation of the CServiceMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServiceMgr.h"
#include "AppSvr.h"
#include "SecsSvr.h"	// Added by ClassView
#include "..\Sys\Var.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CServiceMgr ServiceMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceMgr::CServiceMgr()
{
	pAppSvr = NULL;
	pSecsSvr = NULL;
}

CServiceMgr::~CServiceMgr()
{
	StopSecsSvr();
	StopAppSvr();
}

int CServiceMgr::StartAppSvr()
{
	if (pAppSvr)
	{
		Log("AppSvr already exist", 0);
		ASSERT(0);
		return -91;
	}
	else 
	{
		pAppSvr = new CAppSvr;
		if (!pAppSvr)
		{
			Log("AppSvr alloc error", 0);
			ASSERT(0);
			return -92;
		}
		if (!CSMClt::StartSvrThread(pAppSvr))
		{
			Log("Fail to start AppSvr", 0);
			ASSERT(0);
			return -93;
		}
		Log("AppSvr started", 10);
		while (!pAppSvr->bStarted) { Sleep(10); }
		
		return 0;
	}
	// Leaked
	ASSERT(0);
	return -94;
}

BOOL CServiceMgr::StopAppSvr()
{
	if (pAppSvr)
	{
		if (CSMClt::StopSvrThread(pAppSvr->SvrName, pAppSvr->type))
		{
			pAppSvr = NULL;
			Log("AppSvr stopped", 10);
			return TRUE;
		}
	}
	return FALSE;	
}

void CServiceMgr::Log(CString msg, short lvl)
{
	CSMClt ipc;
	if (ipc.Start1(V.LogSvr,sizeof(CSMHdr))) {
		ipc.LogMsg("SvcMgr","",msg,lvl);
	}
}

int CServiceMgr::StartSecsSvr(CWnd *pWnd)
{
	if (pSecsSvr)
	{
		Log("SecsSvr already exist", 0);
		ASSERT(0);
		return -4;
	}
	else 
	{
		pSecsSvr = new CSecsSvr;
		if (!pSecsSvr)
		{
			Log("CSecsSvr alloc error", 0);
			ASSERT(0);
			return -5;
		}
		pSecsSvr->pWndMainFrm = pWnd;
		if (!CSMClt::StartSvrThread(pSecsSvr))
		{
			Log("Fail to start SecsSvr Thread",0);
			ASSERT(0);
			return -6;
		}
		Log("SecsSvr started", 10);
		while (!pSecsSvr->bStarted) { Sleep(10); }
		return 0;
	}
	return -7;
}

BOOL CServiceMgr::StopSecsSvr()
{
	if (pSecsSvr) {
		if (CSMClt::StopSvrThread(pSecsSvr->SvrName, pSecsSvr->type))
		{
			pSecsSvr = NULL;
			Log("SecsSvr stopped", 10);
			return TRUE;
		}
	}
	return FALSE;
}
