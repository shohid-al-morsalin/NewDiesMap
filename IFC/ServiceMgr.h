// ServiceMgr.h: interface for the CServiceMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEMGR_H__5CBD972F_794F_442D_8C33_CD9C321FDC83__INCLUDED_)
#define AFX_SERVICEMGR_H__5CBD972F_794F_442D_8C33_CD9C321FDC83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAppSvr;
class CSecsSvr;

class CServiceMgr  
{
	CAppSvr *pAppSvr;
	CSecsSvr *pSecsSvr;
	
public:
	BOOL StopSecsSvr();
	int StartSecsSvr(CWnd *pWnd);
	void Log(CString msg, short lvl);
	BOOL StopAppSvr();
	int StartAppSvr();
	CServiceMgr();
	virtual ~CServiceMgr();

};

extern CServiceMgr ServiceMgr;

#endif // !defined(AFX_SERVICEMGR_H__5CBD972F_794F_442D_8C33_CD9C321FDC83__INCLUDED_)
