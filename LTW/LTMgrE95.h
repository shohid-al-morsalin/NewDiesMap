// LTMgrE95.h: interface for the CLTMgrE95 class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "LTWMgr.h"
class CLTMgrE95 : public CLTWMgr {
public:
	void MakeLight(char Rd, char Am, char Gr, char Bl);

	CLPObj::ACCESSMODE GetLPAccessMode(short port);
	PrcsMgrServices::LPBSTATE GetLPBState(short port);
	SArams::ARAMSSTATE GetOpMode();
	CLTMgrE95();
	virtual ~CLTMgrE95();
};