// ECID.cpp: implementation of the CECID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ECID.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CECID::CECID()
{
	No = 0;
	Type = eSTRING;
	memset(Name,0,ECIDNAMELEN+1);
	memset(strValue,0,ECIDSTRLEN+1);
	memset(defValue,0,ECIDSTRLEN+1);
	memset(minValue,0,ECIDSTRLEN+1);
	memset(maxValue,0,ECIDSTRLEN+1);
	memset(unit,0,ECIDSTRLEN+1);
}

CECID::~CECID()
{

}

void CECID::Set(short ID, char *Nam, double val)
{
	No = ID;
	strncpy(Name,Nam,ECIDNAMELEN);
	Type = eVALUE;
	sprintf(strValue, "%.4f", val);
}

void CECID::Set(short ID, char *Nam, char *Val)
{
	No = ID;
	strncpy(Name,Nam,ECIDNAMELEN);
	Type = eSTRING;
	strncpy(strValue, Val, ECIDSTRLEN);
}
