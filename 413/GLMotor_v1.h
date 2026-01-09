// GLMotor.h: interface for the CGLMotor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "SiBase.h"

#define CIRNORMALTEXTLEN1	16

#ifdef DEV_GCLIB
#include "gclib.h"
#pragma comment(lib, "../413/gclib.lib")
#endif

class CDMLock;

class CGLMotor_v1 : public CSiBase {
	CString cmd;
	CString rsp;

	// No need response (g1)
	BOOL _SendN(const char* command);

	// Receive reponse (g2)
	CString _SendR(const char* command);

	BOOL Initialize();

	GCon g1, g2;

public:
	CDMLock* pLockExec;

public:

	void SleepEx(DWORD tout);
	BOOL IsWaferPresent();
	BOOL devMoveAXY(float iX, float iY);
	BOOL devMoveAXYZ1(float iX, float iY, float iZ1);
	BOOL SetSpectrometerPower(BOOL bOnOff);
	BOOL SetSpectroIllumLight(BOOL bOnOff);
	BOOL SetSpectroSourceLight(BOOL bOnOff);
	BOOL SetMicroscopeLight(BOOL bOnOff);
	BOOL ResetAll();
	BOOL ResetY();
	BOOL ResetX();
	BOOL ResetRoughZ();
	BOOL ResetEchoTopZ();
	BOOL ResetXY();
	float fGetFFUPressNow();
	BOOL devMoveRXY(float iX, float iY);
	BOOL SetSpeed(int nAxis, float fSpeed);
	BOOL devGetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2);
	BOOL ClosePort();
	BOOL OpenPort(short PortNo);
	BOOL StopAll();
	BOOL devSetAbsPos(int nAxis, float lPos);
	BOOL devMoveR(int nAxis, float lDist);
	BOOL devMoveA(int nAxis, float lPos);
	BOOL devGetCurPos(int nAxis, float* pfPos);
	BOOL SetCurrent(int nAxis, float fCurrent);
	BOOL devGetXYPos(float* pfX, float* pfY, DWORD tout);
	BOOL Stop(int nAxis);

	CGLMotor_v1();
	virtual ~CGLMotor_v1();

	// [5/27/2020]
	BOOL IsDoorInterlockTriggered();
	BOOL setEncoderPos(int nAxis, int pos);

	// [6/16/2020]
	inline void e(GReturn error);
	int find_avilable_handle_number(GCon g);
	BOOL clear_handle();
	BOOL check_handle();
	BOOL reissue_handle1();
	BOOL reissue_handle2();

	// [20211222]
	BOOL _WaitForXY(int tout, BOOL& bPos);
	BOOL _WaitAllStop(int tout, BOOL& bPos);
	BOOL _WaitStop(int nAxis, long tout, BOOL& bPos);
	// [04232024
	BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// ]
};
