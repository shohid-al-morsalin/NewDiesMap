// SMCHdr.cpp: implementation of the CSMCHdr class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SMCHdr.h"
#include "SMSnd.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
void CSMClt::SendN(CString Svr, CSMHdr *pSM, long size, long wait)
{
	CSMSnd *p = new CSMSnd;
	if(p->CreateThread( CREATE_SUSPENDED ) ) {
		p->SetThreadPriority( THREAD_PRIORITY_NORMAL );
		p->Svr = Svr;
		p->pSMHdr = new CSMHdr;
		p->size = size;
		p->wait = wait;
		memcpy(p->pSMHdr, pSM, size);
		VERIFY( p->ResumeThread() );
	}
}
*/

/*
short CSMClt::Ret()
{
	return short(SM.ret[0]);
}
*/

/*
BOOL CSMClt::_Req(long wait)
{
	return _Request(&SM,wait);
}
*/

/*
BOOL CSMClt::Snd(long wait)
{
	return Send(&SM,wait);
}
*/

/*
void CSMClt::Set(short c, float s0, float s1, float s2, float s3, float s4, float s5, float s6, float s7)
{
	SM.cmd = c;
	SM.sub[0] = s0;
	SM.sub[1] = s1;
	SM.sub[2] = s2;
	SM.sub[3] = s3;
	SM.sub[4] = s4;
	SM.sub[5] = s5;
	SM.sub[6] = s6;
	SM.sub[7] = s7;
}
*/

BOOL CSMClt::Send(CSMOL* p, long wait) {
	ASSERT(bStarted);
	p->exe = CSMOL::SND;
	return CSMIpc::Send(p, sizeof(CSMOL), wait);
}

BOOL CSMClt::Send(CSMHdr* p, long wait) {
	ASSERT(bStarted);
	p->exe = CSMHdr::SND;
	return CSMIpc::Send(p, sizeof(CSMHdr), wait);
}

BOOL CSMClt::Request(CSMOL* p, long wait) {
	ASSERT(bStarted);
	p->exe = CSMOL::REQ;
	return CSMIpc::Request(p, sizeof(CSMOL), wait);
}

BOOL CSMClt::Request(CSMHdr* p, long wait) {
	ASSERT(bStarted);
	p->exe = CSMHdr::REQ;
	return CSMIpc::Request(p, sizeof(CSMHdr), wait);
}

CSMClt::CSMClt() {}

CSMClt::~CSMClt() {}

BOOL CSMClt::ReqData(CSMHdr& SM, CString Svr, long wait) {
	CSMClt ipc;
	BOOL ret = FALSE;
	if (ipc.Start1(Svr, sizeof(CSMHdr))) {
		if (ipc.Request(&SM, wait)) {
			ret = TRUE;
		}
	}
	ipc.Destroy();
	return ret;
}

BOOL CSMClt::SndData(CSMHdr& SM, CString Svr, long wait) {
	CSMClt ipc;
	BOOL ret = FALSE;
	if (ipc.Start1(Svr, sizeof(CSMHdr))) {
		if (ipc.Send(&SM, wait)) {
			ret = TRUE;
		}
	}
	ipc.Destroy();
	return ret;
}