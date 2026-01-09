// S2F31.h: interface for the CS2F31 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "S2Msg.h"

class CS2F31 : public CS2Msg {
public:
	BOOL Validate();
	CS2F31();
	virtual ~CS2F31();
};
