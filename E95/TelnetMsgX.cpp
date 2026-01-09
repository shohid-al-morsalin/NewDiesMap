// TelnetMsg.cpp: implementation of the CTelnetMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "TelnetMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTelnetMsg::CTelnetMsg()
{
	idx = 0;
	msg = "";
	ret = ":?";
	for (int i=0; i<8; i++)
	{
		Axis[i] = 0;	// 0 not moving, 1 is moving [6/4/2012 Administrator]
	}
}

CTelnetMsg::~CTelnetMsg()
{

}
