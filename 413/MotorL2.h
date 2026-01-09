// MotorL.h: interface for the CMotorL2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SIBase.h"
#include "MotorParam.h"// Added by ClassView

// MotorID identify each axis [10/28/2012 Yuen]
// Each object refers to one axis only [10/28/2012 Yuen]
class CMotorL2 {
public:
	CSiBase* pSiHub;

protected:
	long _homPos; // physical hardware position
	BOOL bReset;
	char m_Name[20];
	BOOL  m_StopRequested;

public:
	int MotorID;

	float Res;
	float curPos;
	CMotorParam Param;

public:
	void SetAcc(float fAcc);
	BOOL IsSimu();
	float fGetPos(long p);	// Convert Position from device to real [10/28/2012 Yuen]
// 	BOOL GetXYPos(float *X, float *Y);
	short GetCtrlType();
	BOOL bGetPos(float* pfPos);
	void vSaveParameter(char* szStr);
	void vSetParameter(char* szStr);
	virtual void SetProperties();
	virtual void SetNormalSpeed();
	virtual void vGetParameter(char* szStr);
	virtual BOOL Init(CListBox* pMessage);
	void vSetMotorCurrent(float Current);
	void vSetMicroRes(short val);
	virtual void EnableMotor(BOOL bOnOff);
	BOOL WaitForMotor(int tout);
	// 	BOOL WaitForAllMotor(int tout, int MIDX, int MIDY, int MIDZ1, int MIDZ2);
	void vSetInitSpeed(float InitSpeed);
	virtual int iGetErrorCode(void);
	BOOL GetStatus(int nChan);
	BOOL SetOutput(int nChan, BOOL bOnOff);
	BOOL SetSpeed(float fSpeed);
	virtual BOOL bSetCurrent(float fCurrent);
	virtual float GetAnalogInput();
	BOOL MoveR(float dist);

	float Real2Dev(float pos);
	float Dev2Real(float pos);
	BOOL Move(float pos);

	//    BOOL ResetGL();
	BOOL Reset(float fSteps, float speed);
	//	virtual	BOOL Park();

	void Set(CMotorParam* param);
	BOOL fReadPos(float& fPos);		// read from counter and convert to user unit
	void  vSetSpeed(float fSpeed);
	virtual void  vSetJogSpeed(float fSpeed);
	void  vSetAcceleration(float fAcc);
	void StopMotorRequest();
	void vSetPos(float fPos);
	void vSetScale(double fScale);

	CMotorL2();
	CMotorL2(int iAxis, CSiBase* SiMc);
	virtual ~CMotorL2();

	virtual void setEncoderPos(int pos);
};
