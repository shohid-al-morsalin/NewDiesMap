#pragma once
//02082024/MORSALIN/CProbeSwitching Class
#include <stdafx.h>
#include "Global413.h"
#include <ProbeSwitchingBase.h>
#include <string>
using namespace std;
class CSerial;

class CProbeSwitching : public CProbeSwitchingBase {
protected:
	CSerial* com;

public:

	CProbeSwitching();
	virtual ~CProbeSwitching();
	BOOL Connect(int comPort, int transRate, char parity, int stopBit);
	void Disconnect();
	CString Cal_CRC(std::string stream);
	void Cal_OneCRC(vector<long long> dataArray);
	BOOL isMotorMoving();
	BOOL ExecuteCommand(CString cmd);
	BOOL SendCommand(CString SendStream);
	BOOL InitMovingParameters(int steps);
	BOOL MoveR(int steps);
	BOOL MoveA(int steps);
	BOOL MoveToECHOprb();
	BOOL MoveToCWL008prb(); // 10172024 Morsalin
	BOOL MoveToCWL030prb(); // 10172024 Morsalin
	BOOL MoveToROUGHNESSprb();
	BOOL CurrentPosition();
	BOOL ResetAlarm();
	BOOL SetSoftLimit();
	BOOL toEchoOffset();
	BOOL toCWLOffset();

	BOOL isOpen = false;

	bool IsConnect, positive;

	long long CRC;
	CString finalCRC, ReceiveStream;
	string CRCString;
	int fullRotationStep = 1000;
	int prevPos;

	CString CurPos_cmd = "01 03 00 CC 00 02";
	CString Reset_cmd = "01 06 00 7D 00 00";//
	CString Initialize_cmd = "01 10 18 00 00 0A 14 00 00 00 02 ";
	CString Moving_cmd = "01 06 00 7D 08 00";//
	CString PosLimitSet_cmd = "01 10 03 88 00 02 04 ";
	CString NegLimitSet_cmd = "01 10 03 8A 00 02 04 ";
};