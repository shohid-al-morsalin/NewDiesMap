// FSMLightTower.h: interface for the CFSMLightTower class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CComm;
#include "TCtrlBase.h"

class CFSMLightTower :public CTCtrlBase {
public:
	BOOL GreenFlOff();
	BOOL GreenFlOn();
	BOOL BlueFlOff();
	BOOL BlueFlOn();
	BOOL BlueOff();
	BOOL BlueOn();
	virtual void Update();
	BOOL GreenOff();
	BOOL GreenOn();
	BOOL RedOff();
	BOOL RedOn();
	BOOL AmbOff();
	BOOL AmbOn();
	BOOL RedFlOff();
	BOOL RedFlOn();
	BOOL AmbFlOff();
	BOOL AmbFlOn();
	BOOL Buzzer1Off();
	BOOL Buzzer1On();
	BOOL Buzzer2Off();
	BOOL Buzzer2On();
	BOOL Init();
	BOOL Reset();
	CFSMLightTower();
	virtual ~CFSMLightTower();
	BOOL SetConditionAllOff();
	//BOOL SetConditionDownOrManual();
	//BOOL SetConditionLoadUnloadRequest();
	//BOOL SetConditionStandby();
	//BOOL SetConditionProcessing();
	//BOOL SetConditionAlarmed();
	//BOOL SetConditionAirAlarmed();
private:
	BOOL bInit;
	CComm* pCom;
	int Port;
	BOOL bSound;
};
extern CFSMLightTower* pFSMTower;
