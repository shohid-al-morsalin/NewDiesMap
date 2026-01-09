// MotorS2.h: interface for the CMotorS2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Global413.h"
#include "SIBase.h"

class CDMLock;
class CMotorL2;

class CMotorS2 {
public:
	//	CCriticalSection CS1;

	// 	CDMLock *pLock;

	enum STAGECOORSPACE {
		ECHOSPACE, THINFSPACE, CWLSPACE, ROUGHSPACE
	};

	BOOL bXYStageBusy;

	CSiBase* pSiMc;
	CSiBase* pSiMcSimu; // Simulator class [6/14/2013 Yuen]

	CMotorL2* pMX;	// Stage X [7/5/2010 Yuen]
	CMotorL2* pMY;	// Stage Y [7/5/2010 Yuen]
	CMotorL2* pMEchoTopZ;	// Echo motor [7/5/2010 Yuen]
	CMotorL2* pMThinFilmZ; // Thin Film motor [7/5/2010 Yuen]
	CMotorL2* pMP;  // Probe motor [7/5/2010 Yuen]

public:
	CString ErrStr;
	virtual BOOL MovePR(float dist);
	virtual BOOL MoveP(float fP);
	virtual void ReconnectTCP();
	void CalcDPZOff(float pos);
	BOOL WaitP();
	virtual BOOL IsWaferPresent();
	CMotorS2::STAGECOORSPACE ActvCam2CoorSpace(CCamBase::CAMERAID cspc);
	BOOL Transform(float& fX, float& fY, STAGECOORSPACE cspc);
	BOOL ReverseTransform(float& fX, float& fY, STAGECOORSPACE cspc);
	BOOL MoveRZ(float x, CMotorL2* pZ, BOOL bBlock);
	BOOL MoveZ(float x, CMotorL2* pZ, BOOL bBlock);
	virtual void ResetZD(CMotorL2* pZ);
	virtual BOOL ResetAll();
	BOOL ResetAllWithConfirmation();
	virtual BOOL MoveAXY(float X, float Y);
	virtual BOOL MoveAXYZ1(float X, float Y, float Z1);
	virtual BOOL MoveRXY1(float fX, float fY);
	BOOL GetZPosDev(float& z1Pos, float& z2Pos);
	virtual BOOL SetSpectroIllumLight(BOOL bOnOff);
	virtual BOOL GetPosAll(float* pfX, float* pfY, float * pfEchoTopZ, float * pfRoughZ, float* pfCWLZ, float* pfEchoBottomZ);
	virtual BOOL SetSpectroPower(BOOL bOnOff);
	virtual BOOL SetMicroscopeLight(BOOL bOnOff);
	virtual BOOL SetSpectroSourceLight(BOOL bOnOff);
	virtual BOOL ResetY();
	virtual BOOL ResetX();
	virtual BOOL MoveRP(float dist);
	virtual BOOL MoveRY(float dist);
	virtual BOOL MoveRX(float dist);
	virtual BOOL ResetXY();
	virtual float GetFFUPressure();

	void SaveParam();
	void LoadParam();
	void SetHome();
	virtual BOOL Initialize(CListBox* pMessage);
	BOOL AllStop();
	virtual BOOL Reset();
	virtual BOOL ResetP();
	void ResetEchoTopZ();
	void ResetRoughZ();
	void ResetCWLTopZ(); // 06112024/MORSALIN
	double GetSPMMY();
	double GetSPMMX();
	BOOL MoveEchoTopZ(float x, BOOL bBlock = TRUE);
	BOOL MoveREchoTopZ(float x, BOOL bBlock = TRUE);
	BOOL MoveRThinFilmZ(float z, BOOL bBlock = TRUE);
	BOOL MoveThinFilmZ(float x, BOOL bBlock = TRUE);
	BOOL MoveRRoughZ(float x, BOOL bBlock = TRUE);
	BOOL MoveRCWLTopZ(float x, BOOL bBlock = TRUE);
	BOOL MoveRXY(float fX, float fY);

	BOOL GotoXY(float x, float y);
	BOOL GotoXYZ1(float x, float y, float z);

	BOOL WaitEchoTopZ();
	BOOL WaitThinFilmZ();
	// 	BOOL WaitForXY();
	BOOL WaitForAll(long tout);

	virtual BOOL MoveX(float fX);
	virtual BOOL MoveY(float fY);
	virtual BOOL GetXYPos(float* xpos, float* ypos);

	BOOL GetEchoTopZPosDev(float& zPos);
	BOOL GetThinFilmZPosDev(float& zPos);
	BOOL SetEchoTopZSpeed(float fSpeed);

	BOOL IsSimu();

	// [5/27/2020 Zhiming]
	virtual BOOL IsDoorInterlockTriggered();
	virtual void SetEncoderPosition(int a, int b);
	// [02/10/2022 ZHIMING]
	BOOL XPolarity();
	BOOL YPolarity();
	// [04232024
	virtual BOOL SetChuckVacuum(BOOL bOnOff, int nWaferDiameter = 300);
	// [20240426 Tushar
	CSiBase* pSiMc2;
	CMotorL2* pMEchoBottomZ;
	//BOOL MoveREchoBottomZ(float x, BOOL bBlock = TRUE);
	// [05012024 ZHIMING
	void ResetEchoBottomZ();
	// [05072024
	void GetActiveCameraOffset(float& xoff, float& yoff);
	CMotorL2* pMCWLTopZ, *pMRoughZ;
	BOOL GetCWLTopZPosDev(float& zPos);
	BOOL MoveCWLTopZ(float z, BOOL bBlock = TRUE);
	BOOL WaitCWLTopZ();
	BOOL GetRoughZPosDev(float& zPos);
	BOOL MoveRoughZ(float z, BOOL bBlock = TRUE);
	BOOL WaitRoughZ();
	// [05142024
	BOOL MoveEchoBottomZ(float z, BOOL bBlock = TRUE);
	BOOL GetEchoBottomZPosDev(float& zPos);
	BOOL WaitEchoBottomZ();
	// 20240814/Masum
	virtual BOOL SetFFU(BOOL bOnOff);
	// ]

	CMotorS2();
	virtual ~CMotorS2();
};

// extern DWORD ResetLapTime;
