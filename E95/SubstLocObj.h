// SubstLocObj.h: interface for the CSubstLocObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBSTLOCOBJ_H__8917368E_EB9F_4C61_9C32_E357DC0A2F95__INCLUDED_)
#define AFX_SUBSTLOCOBJ_H__8917368E_EB9F_4C61_9C32_E357DC0A2F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\IPC\OSSMemS.h"

class CSubstObj;

class CSubstLocObj : public OSSMemS  
{
public:
	enum ACTION
	{
		READ, SUBSTLOCSTATECHANGE
	};
	
	enum SUBSTLOCSTATE
	{
		OCCUPIED, UNOCCUPIED
	};
	
	ACTION act;
	short LocNo;
	CSubstObj *pSubstrate;
	SUBSTLOCSTATE SubstLocState;

};

#endif // !defined(AFX_SUBSTLOCOBJ_H__8917368E_EB9F_4C61_9C32_E357DC0A2F95__INCLUDED_)
