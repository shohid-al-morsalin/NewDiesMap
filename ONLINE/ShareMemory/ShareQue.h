// ShareQue.h: interface for the CShareQue class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

//#include "Evt.h"
#include "PtrListEx.h"

class CEvt;

class CShareQue : public CPtrListEx {
public:
	CEvt* Remove();
	BOOL Add(CEvt* Evt);
	void Dealloc(void* p);
	short MaxQCnt;

	CShareQue();
	virtual ~CShareQue();
};