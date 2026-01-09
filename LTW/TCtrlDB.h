// TCtrlDB.h: interface for the CTCtrlDB class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "TCtrlBase.h"

class CTCtrlDB : public CTCtrlBase {
public:
	unsigned char PortValue;
	void Update();
	CTCtrlDB();
	virtual ~CTCtrlDB();
};
