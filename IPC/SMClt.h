#ifndef CSMCLT_H
#define CSMCLT_H
#include "SMIpc.h"
class CSvrThread;

class CSMC : public CSMIpc {
public:
	BOOL bStarted;

	BOOL EndHdr();

	BOOL EndOL();

	static BOOL StopSvrThread(CString svr, short type);

	static BOOL StartSvrThread(CWinThread* pThread, int priority = THREAD_PRIORITY_NORMAL);

	BOOL Start(CSvrThread* pSvr, long size);

	BOOL Start1(CString app, long size, BOOL bAssert = TRUE);

	BOOL Start2(CString app, long size, BOOL bAssert = TRUE);
	CSMC();

	virtual ~CSMC();
};
#endif // CSMCLT_H