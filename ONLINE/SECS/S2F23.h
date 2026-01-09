// S2F24.h: interface for the CS2F23 class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "S2Msg.h"

class CS2F23 : public CS2Msg {
public:
	BOOL Validate();
	CS2F23();
	virtual ~CS2F23();
};