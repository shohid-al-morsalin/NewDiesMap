// GLMotor.h: interface for the CGLMotor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "SiBase.h"

#define CIRNORMALTEXTLEN1	16

class CDMLock;
class CTelnetClientSocket;

class CGLMotor : public CSiBase {
	CString cmd;
	CString rsp;

	BOOL _Send(const char* psz, int len);
	BOOL _Send2(const char* psz);
	BOOL _SendN2(CString cmd);
	BOOL _SendN11(CString cmd);
	CString _GetResp(DWORD tout);
	CString _SendR(CString cmd, long tout);

	BOOL Initialize1();
	BOOL Initialize2();

	lpfnMessageProc m_lpfnMessageProc;
	CTelnetClientSocket* pSock1, * pSock2;

	static void CALLBACK OnMessageProc1(int, LPVOID);
	static void CALLBACK OnMessageProc2(int, LPVOID);
	static void CrackMessage(char* msg);

public:
	CDMLock* pLockExec;
	// 	CCriticalSection CSRecv;

	int idx;
	CString CirNormalText[CIRNORMALTEXTLEN1 + 3];	// Circular buffer string [5/26/2012 Administrator]

	short hd, tl;	// Head and tail of CirReturnText [7/14/2013 Yuen]
	CString CirReturnText[CIRNORMALTEXTLEN1 + 3];	// Circular buffer string [5/26/2012 Administrator]

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

	void WaitConnect();
	void CloseSock2();
	void CloseSock1();
	BOOL ReconnectSoc2();
	BOOL ReconnetSoc1();
	void ReconnectTCP();

	void Flush();
	void SendReply(CString& msg);
	void SetCallBack(lpfnMessageProc pFunc = NULL);
	BOOL _ExecPrgm(CString& msg, short nAxis);
	BOOL GetInput(int nAxis, int nChan);
	BOOL SetOutput(int nAxis, int nChan, BOOL bOnOff);
	float GetAnalogInput(int nAxis);

	CGLMotor();
	virtual ~CGLMotor();

	// [5/27/2020 ZHIMING
	BOOL IsDoorInterlockTriggered();
	BOOL setEncoderPos(int nAxis, int pos);

	// [6/17/2020
	int find_avilable_handle_number();
	BOOL clear_handle();
	BOOL check_handle();

	// [12222021
	BOOL _WaitForXY(int tout, BOOL& bPos);
	BOOL _WaitAllStop(int tout, BOOL& bPos);
	BOOL _WaitStop(int nAxis, long tout, BOOL& bPos);

	// [01102022
	BOOL ResetMotionMemory(int nAxis);
	BOOL ResetAllMotionMemory();
	// [04232024
	BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// [06172024
	BOOL SetAcceleration(int nAxis, float fSpeed);
	// [08142024 Masum
	BOOL SetFFU(BOOL bOnOff);
	// ]
};
