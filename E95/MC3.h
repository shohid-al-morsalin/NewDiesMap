// MC3.h: interface for the CMC3 class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\413\MotorS2.h"

class CMotorL2;

class CMC3 : public CMotorS2 {
	short Limit;

public:
	long devGetTOutDelay(float dX, float dY);
	void ReconnectTCP();
	BOOL IsWaferPresent();
	BOOL ResetAll();
	BOOL SetSpectroPower(BOOL bOnOff);
	BOOL SetMicroscopeLight(BOOL bOnOff);
	BOOL ResetY();
	BOOL ResetX();
	BOOL MoveRY(float dist);
	BOOL MoveRX(float dist);
	float GetFFUPressure();
	BOOL GetXYPos(float* fX, float* fY);
	BOOL MoveY(float fY);
	BOOL MoveX(float fX);
	//BOOL GetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2); // 06112024/MORSALIN --> commented
	BOOL GetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2, float* pfZ3); // 06112024/MORSALIN
	BOOL Initialize(CListBox* pMessage);
	BOOL ResetXY();
	BOOL SetSpectroIllumLight(BOOL bOnOff);
	// 	BOOL WaitStopAll(int tout);
	// 	void ResetAllGl();
	BOOL MoveAXY(float X, float Y);
	BOOL MoveAXYZ1(float fX, float fY, float fZ1);
	void ResetZD(CMotorL2* pZ);
	BOOL MoveRXY1(float fX, float fY);
	BOOL SetSpectroSourceLight(BOOL bOnOff);
	CMC3();
	virtual ~CMC3();

	// [5/27/2020]
	BOOL IsDoorInterlockTriggered();
	// [04232024
	BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// 20240814/Masum
	BOOL SetFFU(BOOL bOnOff);
	// ]
};
