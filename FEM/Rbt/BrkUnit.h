#pragma once

#include "RbtUnit.h"

class CBrkUnit : public CRbtUnit {
	enum DEVICETYPE { STANDARD, SMIFLOADPORT };
	DEVICETYPE DeviceType;

	short arm; // LYF arm = 1 is 128 and arm = 2 is 900 [11/8/2005]

public:
	BOOL SetPostPos(short AlgnNum, long PostPos, long OffsetPos);
	int GetDIOIN();
	BOOL IsWaferOnAlgn(short arm);
	void Log(CString msg, short lvl = 5);
	// 	BOOL AlignWafer(short arm);

	BOOL GetTinyEye();
	BOOL WaferPresentCheck();
	BOOL LockDoor(BOOL bLock);
	BOOL ReqStatus();

	BOOL Xfer(char* text);
	BOOL Stop();
	BOOL Start(short port);
	BOOL HomeRobot(short AlgnNum);
	BOOL HomeAlign(short AlgnNum);

	BOOL IsWaferOnArm();
	BOOL IsWaferOnArm(short arm);
	BOOL IsWaferOnAlgn();

	CBrkUnit();
	virtual ~CBrkUnit();
};