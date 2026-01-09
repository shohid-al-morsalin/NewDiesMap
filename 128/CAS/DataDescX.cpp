// DataDesc.cpp: implementation of the CDataDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataDesc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataDesc::CDataDesc()
{
	for (int i = 0; i < DESCENTRY; i++) 
	{
		desc[i] = "";
		dsp[i] = TRUE;
	}
}

CDataDesc::~CDataDesc()
{

}

short CDataDesc::GetColumnCount()
{
	int cnt = 0;
	for (int i = 0; i < DESCENTRY; i++)
	{
		if (dsp[i]) cnt++;
	}
	return cnt;
}

short CDataDesc::GetColumnIndex(short col)
{
	int cnt = 0;
	for (int i = 0; i < DESCENTRY; i++)
	{
		if (col == cnt) {
			if (dsp[i]) return i;
		}
		if (dsp[i]) cnt++;
	}
	return -1;
}
