// IDRUnitOmron.h: interface for the CIDRUnitOmron class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IDRUnit.h"

class CIDRUnitOmron : public CIDRUnit {
public:
	CString ReadCID();
	BOOL WriteID(char* id);
	void RemoveTrailingSpaces(CString& str);
	char ID[32];

	BOOL ReadID();
	char* GetCID(char* cid, short len);

	CString cmdRead, cmdWrite, cmdTest;
	BOOL GetDevStatus();
	CIDRUnitOmron();
	virtual ~CIDRUnitOmron();
};