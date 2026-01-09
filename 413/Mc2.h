// Mc2.h: interface for the CMC2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "MotorS2.h"

class CMotorL2;

class CMC2 : public CMotorS2 {
public:
	BOOL MovePR(float dist);
	BOOL MoveP(float fP);
	BOOL ResetAll();
	BOOL ResetAllWithConfirmation();
	BOOL SetSpectroPower(BOOL bOnOff);
	BOOL SetMicroscopeLight(BOOL bOnOff);
	BOOL ResetY();
	BOOL ResetX();
	BOOL MoveRP(float dist);
	BOOL MoveRY(float dist);
	BOOL MoveRX(float dist);
	float GetFFUPressure();
	BOOL GetXYPos(float* fX, float* fY);
	BOOL ResetP();
	BOOL Reset();
	BOOL MoveY(float fY);
	BOOL MoveX(float x);
	BOOL Initialize(CListBox* pMessage);
	BOOL ResetXY();
	BOOL ResetXYSi();
	BOOL MoveAXY(float X, float Y);
	BOOL MoveAXYZ1(float X, float Y, float Z1);
	void ResetZD(CMotorL2* pZ);
	BOOL MoveRXY1(float fX, float fY);
	BOOL SetSpectroSourceLight(BOOL bOnOff);

	// [01192024 ZHIMING
	BOOL IsDoorInterlockTriggered();
	BOOL IsWaferPresent();
	// [04232024
	BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// ]

	CMC2();
	virtual ~CMC2();
};
