// AlignerSwitcher.h: interface for the CAlignerSwitcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALIGNERSWITCHER_H__BFA2C167_B1BC_415B_9361_5783BF93A1C4__INCLUDED_)
#define AFX_ALIGNERSWITCHER_H__BFA2C167_B1BC_415B_9361_5783BF93A1C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAlignerSwitcher  
{
public:
	static int GetWaferPresent();
	static BOOL Switch900();
	static BOOL Swith128();
	CAlignerSwitcher();
	virtual ~CAlignerSwitcher();

};

#endif // !defined(AFX_ALIGNERSWITCHER_H__BFA2C167_B1BC_415B_9361_5783BF93A1C4__INCLUDED_)
