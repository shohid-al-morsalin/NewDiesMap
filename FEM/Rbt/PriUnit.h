// PriUnit.h: interface for the CPriUnit class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "RbtUnit.h"

class CPriUnit : public CRbtUnit {
	unsigned long Status;

public:
	CString GetSM();
	BOOL Map();
	BOOL Send(char* text);
	BOOL BusyCheck();

	void SetSpeed();

	void SetStation();

	void SetRetractPos();

	BOOL StatusCheck();
	BOOL InitRobot();
	long ExtractHex(CString& str);

	BOOL IsHomed();

	BOOL Stat();

	BOOL ReqStatus();

	BOOL Xfer(char* text);

	BOOL HomeRobot(short AlgnNum);

	BOOL HomeAlign(short AlgnNum);

	//	BOOL Stop();

	BOOL Start(short port);

	BOOL IsWaferOnArm();
	BOOL IsWaferOnAlgn();

	CPriUnit();
	virtual ~CPriUnit();
};