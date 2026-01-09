// TCtrlDBNet.h: interface for the CTCtrlDBNet class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "TCtrlBase.h"
#include "..\IPC\LTwrVar.h"

class CTCtrlDBNet : public CTCtrlBase {
public:
	LTwrVar::LTLED TranslateState(LIGHTSIGNAL state);
	void Update();
	CTCtrlDBNet();
	virtual ~CTCtrlDBNet();
};
