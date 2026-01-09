// E95BEM.cpp: implementation of the CE95BEM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BEME95.h"
#include "BEME95Th.h"
#include "..\SYS\CUMMsg.h"
//#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// This is the communication channel between E95 and BEM module

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// LYF RS232 version [4/23/2006]

CBEME95::CBEME95()
{
	pCom = NULL;
}

CBEME95::~CBEME95()
{
	if (pCom) {
		pCom->StopThread();
		delete pCom;
	}
}

CRtnCode::RCODE CBEME95::Send(CMsgItem *pMsg)
{
	if (pCom) {
		return pCom->Send(pMsg);
	}
	return CRtnCode::RCODE::ERR;
}

// Call by BEM only
void CBEME95::Start128()
{
	pCom = new CBEME95Th;
	if (pCom) {
		pCom->IDSrc = 128;
		pCom->SetMessageEventHandler(UM_GENMESSAGEINQUE);
		CSerialTh::Start(pCom, DosUtil.Get128Port());
		Sleep(1000);
		if (pCom->bSimu) {
		}
		else {
		}
	}
}

// Call by BEM only
void CBEME95::Start900()
{
	pCom = new CBEME95Th;
	if (pCom) {
		pCom->IDSrc = 900;
		pCom->SetMessageEventHandler(UM_GENMESSAGEINQUE);
		CSerialTh::Start(pCom, DosUtil.Get900Port());
		Sleep(1000);
		if (pCom->bSimu) {
		}
		else {
		}
	}
}

// Call by E95 only
void CBEME95::StartE95To900()
{
	pCom = new CBEME95Th;
	if (pCom) {
		pCom->IDSrc = 1;
		pCom->SetMessageEventHandler(UM_DSPCHRMESSAGEINQUE);
		CSerialTh::Start(pCom, DosUtil.GetE95900Port());
		Sleep(1000);
		if (pCom->bSimu) {
		}
		else {
		}
	}
}

// Call by E95 only
void CBEME95::StartE95To128()
{
	pCom = new CBEME95Th;
	if (pCom) {
		pCom->IDSrc = 2;
		pCom->SetMessageEventHandler(UM_DSPCHRMESSAGEINQUE);
		CSerialTh::Start(pCom, DosUtil.GetE95128Port());
		Sleep(1000);
		if (pCom->bSimu) {
		}
		else {
		}
	}
}

void CBEME95::RegisterWin(CWnd * pWnd)
{
	if (pCom) {
		pCom->pWnd = pWnd;
	}
}

CMsgItem * CBEME95::GetMsgItem(int ID)
{
	if (pCom) {
		return pCom->Que.InQue.Remove(ID);
	}
	return NULL;
}
