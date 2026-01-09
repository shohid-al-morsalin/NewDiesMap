// JobsMgrBase.h: interface for the CJobsMgrBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOBSMGRBASE_H__697A162C_17F6_45E0_B984_B03692B86374__INCLUDED_)
#define AFX_JOBSMGRBASE_H__697A162C_17F6_45E0_B984_B03692B86374__INCLUDED_
#include "CtrlJobMgr.h"
class CPRJob;
class CCtrlJob;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CJobsMgrBase : public CCtrlJobMgr  
{
public:	



	CJobsMgrBase();
	virtual ~CJobsMgrBase();

protected:
	CPRJob * FindPJ(CString PJ);
	CCtrlJob * FindCJ(CString CJ);
};

#endif // !defined(AFX_JOBSMGRBASE_H__697A162C_17F6_45E0_B984_B03692B86374__INCLUDED_)
