// SMSnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMSnd.h"
#include "SMCHdr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMSnd

IMPLEMENT_DYNCREATE(CSMSnd, CWinThread)

CSMSnd::CSMSnd() {
	pSMOL = NULL;
	pSMHdr = NULL;

	size = 0;
	Svr = "";
	wait = INFINITE;
}

CSMSnd::~CSMSnd() {}

BOOL CSMSnd::InitInstance() {
	CSMClt ipc;
	if (ipc.Start1(Svr, size)) {
		if (pSMOL) ipc.Send(pSMOL, wait);
		if (pSMHdr) ipc.Send(pSMHdr, wait);
	}
	return TRUE;
}

int CSMSnd::ExitInstance() {
	if (pSMOL) delete[] pSMOL;
	if (pSMHdr) delete[] pSMHdr;

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSMSnd, CWinThread)
	//{{AFX_MSG_MAP(CSMSnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMSnd message handlers