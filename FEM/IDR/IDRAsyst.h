// IDRAsyst.h: interface for the CIDRAsyst class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "IDRUnit.h"

class CIDRAsyst : public CIDRUnit {
public:
	CString ReadCID();
	char* GetCID(char* cid, short len);
	BOOL WriteID(char* id);
	BOOL WriteData(short Seg, short len, char* data);
	BOOL ReadData(short Seg, short len);

	BOOL GetDevStatus();
	BOOL ReadID();

	CIDRAsyst();
	virtual ~CIDRAsyst();
};
