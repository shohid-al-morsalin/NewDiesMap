// E95BEMTh.cpp: implementation of the CE95BEMTh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BEME95Th.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBEME95Th::CBEME95Th()
{
	Protocol = PROTOCL::MSGITEMFORMAT;
}

CBEME95Th::~CBEME95Th()
{

}

CRtnCode::RCODE CBEME95Th::Send(CMsgItem *pMsg)
{
	return Que.Put(pMsg);
}
