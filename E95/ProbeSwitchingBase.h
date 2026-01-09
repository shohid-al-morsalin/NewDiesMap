#pragma once
//02082024/MORSALIN/CProbeSwitchingBase Class
#include "stdafx.h"
using namespace std;

class CProbeSwitchingBase {
public:
	CProbeSwitchingBase();
	virtual BOOL Connect(int comPort, int transRate, char parity, int stopBit);
	virtual void Disconnect();
	virtual CString Cal_CRC(std::string stream);
	virtual void Cal_OneCRC(vector<long long> dataArray);
	virtual BOOL ExecuteCommand(CString cmd);
	virtual BOOL SendCommand(CString SendStream);
	virtual BOOL InitMovingParameters(int steps);
	virtual BOOL MoveR(int steps);
	virtual BOOL MoveA(int steps);
	virtual BOOL MoveToECHOprb();
	virtual BOOL MoveToCWL008prb(); // 10172024 Morsalin
	virtual BOOL MoveToCWL030prb(); // 10172024 Morsalin
	virtual BOOL MoveToROUGHNESSprb();
	virtual BOOL CurrentPosition();
	virtual BOOL ResetAlarm();
	virtual BOOL toEchoOffset();
	virtual BOOL toCWLOffset();
};