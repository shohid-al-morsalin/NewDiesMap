// AppSvr.h: interface for the CAppSvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPSVR_H__BAA62D19_98A4_4A2E_A139_A0D3F32C7E23__INCLUDED_)
#define AFX_APPSVR_H__BAA62D19_98A4_4A2E_A139_A0D3F32C7E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\IPC\SvrThread.h"

class CAppSvr : public CSvrThread  
{
public:
	BOOL Reply(CSMHdr & SM, BOOL ret);
	BOOL ProcessCmd(CSMHdr & SM);
public:
	void CleanUp();
	CAppSvr();
	virtual ~CAppSvr();

};

extern CAppSvr *pAppSvr;


#endif // !defined(AFX_APPSVR_H__BAA62D19_98A4_4A2E_A139_A0D3F32C7E23__INCLUDED_)
