// IDRUnitKey.h: interface for the CIDRUnitKey class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IDRUnit.h"

class CIDRUnitKey : public CIDRUnit {
public:
	CString ReadCID();
	BOOL WriteID(char* id);
	BOOL GetDevStatus();
	char* GetCID(char* cid, short len);

	BOOL ReadID();

	CIDRUnitKey();
	virtual ~CIDRUnitKey();
};