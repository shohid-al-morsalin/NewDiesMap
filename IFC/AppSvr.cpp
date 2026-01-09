// AppSvr.cpp: implementation of the CAppSvr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\..\Src\Arams.h"
#include "AppSvr.h"
#include "..\Sys\Var.h"
#include "..\..\IPC\SMHdr.h"
#include "..\..\IPC\SMCHdr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CAppSvr *pAppSvr = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppSvr::CAppSvr()
{
	type = 2;
	SvrName = "AppSvr";

//	if (pArams) {
//		ASSERT(0);
//	}
//	pArams = new CARAMS;
//	if (!pArams) {
//		ASSERT(0);
//	}
	
}

CAppSvr::~CAppSvr()
{
//	if (pArams) {
//		delete pArams;
//		pArams = NULL;
//	}
}

BOOL CAppSvr::ProcessCmd(CSMHdr &SM)
{
	switch(SM.cmd) {
	case CSMHdr::CTSYNCARAMS:
//		if (!pArams) {
//			ASSERT(0);
//			break;
//		}
		Arams.SetAttr(SM.Ar);
		Reply(SM,TRUE);
		break;
	}
	return TRUE;
}

BOOL CAppSvr::Reply(CSMHdr & SM, BOOL ret)
{
	if (SM.HWin)
	{
		if (ret)
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
//			if (Var.UIMode > 4) {
//				CString str;
//				str.Format("WM_COMMAND %d %d",SM.HWin, SM.Ack);
//				Log(str,10);
//			}
		}
		else
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
//			if (Var.UIMode > 4) {
//				CString str;
//				str.Format("WM_COMMAND %d %d",SM.HWin, SM.Nak);
//				Log(str,10);
//			}
		}
		return TRUE;
	}
	return FALSE;
}


void CAppSvr::CleanUp()
{
}
