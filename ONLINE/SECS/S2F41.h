// S2F41.h: interface for the CS2F41 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "S2Msg.h"

class CS2F41 : public CS2Msg {
public:
	BOOL Validate();
	CS2F41();
	virtual ~CS2F41();
};
