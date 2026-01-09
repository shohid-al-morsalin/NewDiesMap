// SiMotor.h: interface for the CSiMotor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "SiBase.h"
#include "..\serial\SerialICC.h"

class CSerialPort;

class CSiMotor : public CSiBase {
public:
	CSiMotor();
	virtual ~CSiMotor();

protected:
	CSerialICC* m_pif;

	BOOL Send(const char* psz);
	BOOL SendLim(char* buf);
	virtual UINT Receive(UINT nSize, char* chBuffer);

public:
	BOOL _WaitStop(int nAxis, long tout, BOOL& bPos);
	BOOL Reset(short MotorID, float fSteps);
	BOOL ClosePort();
	BOOL OpenPort(short PortNo);
	BOOL SetMicroRes(int Axis, short MicroRes);
	BOOL GetInput(int nAxis, int nChan);
	BOOL SetOutput(int nAxis, int nChan, BOOL bOnOff);
	BOOL StopAll();
	BOOL SetAcceleration(int nAxis, float fAcc);
	BOOL devSetAbsPos(int nAxis, float lPos);
	BOOL FeedToPosition(int nAxis);
	BOOL devGetCurPos(int nAxis, float* pfPos);
	BOOL IsMotorMoving(int nAxis);
	// 	BOOL CSiMotor::IsMotorMoving(int nAxis1, int nAxis2, int nAxis3, int nAxis4);
	BOOL SetCurrent(int nAxis, float fCurrent);
	BOOL devMoveR(int nAxis, float lDist);
	BOOL devMoveA(int nAxis, float lPos);
	// 	BOOL GetXYPosition(long *plX, long *plY);
	BOOL SetSpeed(int nAxis, float fSpeed);
	BOOL IsValidAxis(int nAxis);
	BOOL SetDistance(int nAxis, int nDistance);
	BOOL FeedToLength(int nAxis);
	BOOL Stop(int nAxis);
	BOOL DefineLimits(int nAxis, int nData);
	float GetAnalogInput(int nAxis);
};