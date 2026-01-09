// SiBase.h: interface for the CSiBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
// FLAG for status of axis
// #define		AS_ACTIVE			(0x0001)
// #define		AS_MOVING			(0x0002)
#define		NO_HUBPORT			(8)

typedef void (CALLBACK* lpfnMessageProc)(int, LPVOID);

class CTelnetClientSocket;

// Properties and status of an axis [6/3/2012 Administrator]
class CAxis {
public:
	enum AXSTATUS {
		AS_NOTCONNECTED, AS_ACTIVE, AS_MOVING
	};
	AXSTATUS bmpStatus;
	float fPos;			// Current position, in motor steps [10/28/2012 Yuen]
	int Acc, Spd, MStp;	// Acceleration, Deceleration, Speed and micro-step setting [6/3/2012 Administrator]
	float P, I, D;		// PID of servo motor [6/3/2012 Administrator]

	CAxis() {
		bmpStatus = AXSTATUS::AS_NOTCONNECTED;
		fPos = 0;
		Acc = 250000;
		Spd = 124000;
		MStp = 4;
		P = 8;
		I = 0;
		D = 64;
	}
	void Set(int Ac, int Sp, int MS) {
		Acc = Ac;
		Spd = Sp;
		MStp = MS;
	}
	void SetPID(float fP, float fI, float fD) {
		P = fP;
		I = fI;
		D = fD;
	}
};

class CSiBase {
public:
	BOOL bSimu;
	short NTrial;

	enum CTRLID {
		SIMUMOTOR, SIMOTOR, GLMOTOR, SIMOTOR_NOHUB
	};
	enum AXISID {
		// NOAXIS = -1, XAXIS, YAXIS, ROUGHZAXIS, ECHOTOPZAXIS, PAXIS // Note: do not change sequence, values are used as indexes [6/22/2012 FSM413]
		NOAXIS = -1, XAXIS, YAXIS, ECHOTOPZAXIS, ROUGHZAXIS, PAXIS
	};
	CTRLID ID;
	CString IDStr, MsgStr;
	CAxis moAxis[NO_HUBPORT];  // cache of live status of each motor, unit in encder counts [5/31/2012 Yuen]

public:
	virtual BOOL _WaitStop(int nAxis, long tout, BOOL& bPos);
	virtual BOOL _WaitAllStop(int tout, BOOL& bPos);
	virtual BOOL _WaitForXY(int tout, BOOL& bPos);

	CSiBase();
	virtual ~CSiBase();

public:
	virtual void WaitConnect();
	virtual void ReconnectTCP();
	float GetCurPos(short MotorID);
	long CS2Value(CString cs);
	void SleepEx(DWORD dly);
	short GetValue(CString& cs);
	BOOL IsSimu();
	AXISID GetAxisNo(int nAxis);
	short GetMotorID(CSiBase::AXISID a);

	virtual BOOL IsWaferPresent();
	virtual BOOL devMoveAXY(float iX, float iY);
	virtual BOOL devMoveAXYZ1(float iX, float iY, float iZ1);
	virtual BOOL SetSpectrometerPower(BOOL bOnOff);
	virtual BOOL SetSpectroIllumLight(BOOL bOnOff);
	virtual BOOL SetSpectroSourceLight(BOOL bOnOff);
	virtual BOOL SetMicroscopeLight(BOOL bOnOff);
	virtual BOOL ResetAll();
	virtual BOOL ResetY();
	virtual BOOL ResetX();
	virtual BOOL ResetRoughZ();
	virtual BOOL ResetEchoTopZ();
	virtual BOOL ResetXY();
	virtual BOOL Reset(short MotorID, float fSteps);
	virtual float fGetFFUPressNow();
	virtual BOOL devMoveRXY(float iX, float iY);
	//virtual BOOL devGetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2);  // 06112024/MORSALIN  --> commented
	virtual BOOL devGetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2, float* pfZ3);  // 06112024/MORSALIN
	virtual BOOL IsValidAxis(int nAxis);
	virtual void SetCallBack(lpfnMessageProc pFunc = NULL);
	virtual BOOL ClosePort();
	virtual BOOL OpenPort(short PortNo);
	virtual BOOL SetMicroRes(int Axis, short MicroRes);
	virtual	BOOL GetInput(int nAxis, int nChan);
	virtual BOOL SetOutput(int nAxis, int nChan, BOOL bOnOff);
	virtual BOOL StopAll();
	virtual BOOL SetAcceleration(int nAxis, float fAcc);
	virtual BOOL devSetAbsPos(int nAxis, float lPos);
	virtual BOOL devGetCurPos(int nAxis, float* pfPos);
	virtual BOOL SetCurrent(int nAxis, float fCurrent);
	virtual BOOL devMoveR(int nAxis, float lDist);
	virtual BOOL devMoveA(int nAxis, float lPos);
	virtual BOOL devGetXYPos(float* plX, float* plY, DWORD tout);
	virtual BOOL SetSpeed(int nAxis, float fSpeed);
	virtual BOOL Stop(int nAxis);
	virtual BOOL DefineLimits(int nAxis, int nData);
	virtual float GetAnalogInput(int nAxis);

	// [5/27/2020 Zhiming]
	virtual BOOL IsDoorInterlockTriggered();
	virtual BOOL setEncoderPos(int nAxis, int pos);
	// [04232024
	virtual BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// [05152024
	CAxis moAxisNoHub;
	// 20240814/Masum
	virtual BOOL SetFFU(BOOL bOnOff);
	// ]
};
