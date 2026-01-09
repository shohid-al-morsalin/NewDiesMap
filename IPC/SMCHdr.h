// SMCHdr.h: interface for the CSMCHdr class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
//#include "SMOL.h"
//#include "SMHdr.h"
#include "SMClt.h"
struct CSMOL;
struct CSMHdr;

class CSMClt : public CSMC {
	//public:
	//	CSMHdr SM;
public:
	static BOOL SndData(CSMHdr& SM, CString Svr, long wait = 30000);

	static BOOL ReqData(CSMHdr& SM, CString Svr, long wait = 30000);

	//short Ret();
	//BOOL _Req(long wait = 30000);
	//BOOL Snd(long wait = 30000);
	//void Set(short c, float s0 = 0, float s1 = 0, float s2 = 0, float s3 = 0, float s4 = 0, float s5 = 0, float s6 = 0, float s7 = 0);
	BOOL Send(CSMOL* p, long wait = 60000);

	BOOL Send(CSMHdr* p, long wait = 60000);

	BOOL Request(CSMOL* p, long wait = 60000);

	BOOL Request(CSMHdr* p, long wait = 60000);
	//static void SendN(CString Svr, CSMHdr *pSM, long size, long wait=INFINITE);
	CSMClt();

	virtual ~CSMClt();
};