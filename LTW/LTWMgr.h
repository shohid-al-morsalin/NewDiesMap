// LTWMgr.h: interface for the CLTWMgr class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IPC/SArams.h"
#include "IPC/Smol.h"
#include "LPObj.h"

class CTCtrlBase;

class CLTWMgr {
public:
	CTCtrlBase* pTwr;
	BOOL bGenAlarmSet;
public:
	void SetGenAlarm(BOOL bSet);
	char LTstate[5];
	virtual void SetLight(short Red, short Amber, short Green);
	virtual SArams::ARAMSSTATE GetOpMode() = 0;
	virtual CLPObj::ACCESSMODE GetLPAccessMode(short port) = 0;
	virtual PrcsMgrServices::LPBSTATE GetLPBState(short port) = 0;
	void SetLight();
	void Set_Idle();
	void Set_Run();
	void Set_Alarm();

	virtual void MakeLight(char Rd, char Am, char Gr, char Bl);
	CLTWMgr();
	virtual ~CLTWMgr();
};

extern CLTWMgr* pLTMgr;
