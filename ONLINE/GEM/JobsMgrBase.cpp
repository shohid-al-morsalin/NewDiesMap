// JobsMgrBase.cpp: implementation of the CJobsMgrBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JobsMgrBase.h"
class CPRJob;
class CCtrlJob;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJobsMgrBase::CJobsMgrBase()
{

}

CJobsMgrBase::~CJobsMgrBase()
{

}

CPRJob * CJobsMgrBase::FindPJ(CString PJ){return NULL;}
CCtrlJob * CJobsMgrBase::FindCJ(CString CJ){return NULL;}
