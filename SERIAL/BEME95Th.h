// E95BEMTh.h: interface for the CE95BEMTh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E95BEMTH_H__4D1CA23B_15C4_465E_8D67_51247587A4F4__INCLUDED_)
#define AFX_E95BEMTH_H__4D1CA23B_15C4_465E_8D67_51247587A4F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialTh.h"
#include "RtnCode.h"

class CBEME95Th : public CSerialTh  
{
public:
	CRtnCode::RCODE Send(CMsgItem *pMsg);
	CBEME95Th();
	virtual ~CBEME95Th();

};

#endif // !defined(AFX_E95BEMTH_H__4D1CA23B_15C4_465E_8D67_51247587A4F4__INCLUDED_)
