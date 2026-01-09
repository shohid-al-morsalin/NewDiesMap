// E95BEM.h: interface for the CE95BEM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E95BEM_H__293D967F_B0E6_410C_A68E_76234D02D7AE__INCLUDED_)
#define AFX_E95BEM_H__293D967F_B0E6_410C_A68E_76234D02D7AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RtnCode.h"

class CMsgItem;
class CBEME95Th;

class CBEME95  
{
public:
	CMsgItem * GetMsgItem(int ID);
	void RegisterWin(CWnd * pWnd);
	void Start900();
	void Start128();
	void StartE95To900();
	void StartE95To128();
	CRtnCode::RCODE Send(CMsgItem *pMsg);
	CBEME95Th * pCom;
	CBEME95();
	virtual ~CBEME95();

};


#endif // !defined(AFX_E95BEM_H__293D967F_B0E6_410C_A68E_76234D02D7AE__INCLUDED_)
