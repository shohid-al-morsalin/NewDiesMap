// SystemProperties.cpp: implementation of the CSystemProperties class.
//
//////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
//#include "wafercal.h"
#include "SystemProperties.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CSystemProperties::Serialize(CArchive & ar) 
{
    if (ar.IsStoring()) 
	{
        magic = 1;
        ar << magic;
        ar << model;
    }
    else
	{
        ar >> magic;
        ar >> model;
    }
}
CSystemProperties::CSystemProperties()
{
	CPProfile p;

	CString sys = "System";
	CString sec = "Model";

	model = (short)p.GetIntP(sys, sec, /*10*/20);
	p.PutIntP(sys,sec,model);
}

CSystemProperties::~CSystemProperties()
{

}
