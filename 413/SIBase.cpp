// SiBase.cpp: implementation of the CSiBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiBase.h"
#include "413App.h"
#include "..\SRC\Tokenizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiBase::CSiBase() {
	bSimu = TRUE;
	ID = SIMUMOTOR;
	NTrial = 16;
	IDStr = "Simulation";

	// moAxis are not initialize on purpose [6/14/2013 Yuen]
}

CSiBase::~CSiBase() {}

BOOL CSiBase::SetSpeed(int nAxis, float fSpeed) {
	return TRUE;
}

BOOL CSiBase::Stop(int nAxis) {
	return TRUE;
}

BOOL CSiBase::devGetXYPos(float* pfX, float* pfY, DWORD tout) {
	if (ID == SIMOTOR_NOHUB) {
		//ASSERT(0);
		return TRUE;
	}
	// Regular implementation [10/1/2012 Yuen]
	C413Global* g = &p413App->Global;
	if (IsValidAxis(g->X_Axis) && IsValidAxis(g->Y_Axis)) {
		if (pfX) *pfX = moAxis[g->X_Axis - 1].fPos;
		if (pfY) *pfY = moAxis[g->Y_Axis - 1].fPos;
		return TRUE;
	}
	return FALSE;
}

BOOL CSiBase::SetCurrent(int nAxis, float fCurrent) {
	return TRUE;
}

// BOOL CSiBase::IsMotorMoving(int nAxis)
// {
// 	// Regular implementation [10/1/2012 Yuen]
// 	return FALSE;  // return not moving so that simulator could continue
// }

// BOOL CSiBase::IsMotorMoving(int nAxis1, int nAxis2, int nAxis3, int nAxis4)
// {
// 	return FALSE;
// }

BOOL CSiBase::devGetCurPos(int nAxis, float* plPos) {
	if (ID == SIMOTOR_NOHUB) {
		*plPos = moAxis[0].fPos;
		return TRUE;
	}
	*plPos = moAxis[nAxis - 1].fPos;
	return TRUE;
}

BOOL CSiBase::devMoveA(int nAxis, float lPos) {
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = lPos;
		if (p413App->isFastScanning == true) return TRUE; // For moving fast / Mahedi Kamla 10152024
#ifdef PAUSEPGM
		SleepEx(300);
#endif
		return TRUE;
	}
	moAxis[nAxis - 1].fPos = lPos;	// Unit: Motor steps [6/17/2014 Yuen]
	if (p413App->isFastScanning == true) return TRUE; // For moving fast / Mahedi Kamla 10152024
#ifdef PAUSEPGM
	SleepEx(300);
#endif
	return TRUE;
}

BOOL CSiBase::devMoveR(int nAxis, float lDist) {
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos += lDist;
#ifdef PAUSEPGM
		SleepEx(300);
#endif
		return TRUE;
	}
	moAxis[nAxis - 1].fPos += lDist;	// Unit: Motor steps [6/17/2014 Yuen]

#ifdef PAUSEPGM
	SleepEx(300);
#endif
	return TRUE;
}

BOOL CSiBase::devSetAbsPos(int nAxis, float lPos) {
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = lPos;
		return TRUE;
	}
	moAxis[nAxis - 1].fPos = lPos;
	return TRUE;
}

BOOL CSiBase::SetAcceleration(int nAxis, float fAcc) {
	return TRUE;
}

BOOL CSiBase::StopAll() {
	return TRUE;
}

BOOL CSiBase::SetOutput(int nAxis, int nChan, BOOL bOnOff) {
	return TRUE;
}

BOOL CSiBase::GetInput(int nAxis, int nChan) {
	return FALSE;
}

float CSiBase::GetAnalogInput(int nAxis) {
	return 2.5;
}

BOOL CSiBase::IsDoorInterlockTriggered() {
	return 0;
}

BOOL CSiBase::setEncoderPos(int nAxis, int pos) {
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = pos;
		return TRUE;
	}
	moAxis[nAxis - 1].fPos = pos;
	return TRUE;
}

BOOL CSiBase::DefineLimits(int nAxis, int nData) {
	return TRUE;
}

BOOL CSiBase::OpenPort(short PortNo) {
	return TRUE;
}

BOOL CSiBase::ClosePort() {
	return TRUE;
}

BOOL CSiBase::SetMicroRes(int nAxis, short MicroRes) {
	return TRUE;
}

BOOL CSiBase::IsSimu() {
	return bSimu;
}

void CSiBase::SetCallBack(lpfnMessageProc pFunc) {}

BOOL CSiBase::IsValidAxis(int nAxis) {
	return TRUE;
}

// [ 06112024/MORSALIN  --> commented
//BOOL CSiBase::devGetPosAll(float* pX, float* pY, float* pZ1, float* pZ2) {
//	if (ID == SIMOTOR_NOHUB) {
//		ASSERT(0);
//		return TRUE;
//	}
//	// Regular implementation [10/1/2012 Yuen]
//	C413Global* g = &p413App->Global;
//	if (pX) *pX = moAxis[g->X_Axis - 1].fPos;
//	if (pY) *pY = moAxis[g->Y_Axis - 1].fPos;
//	if (pZ1) *pZ1 = moAxis[g->EchoTop_Axis - 1].fPos;
//	if (pZ2) *pZ2 = moAxis[g->RoughZ_Axis - 1].fPos;
//
//	// 	CString msg;
//	// 	msg.Format("SiB:devGetPosAll: X=%.4f  Y=%.4f  Z1=%.4f  Z2=%.4f",*pX,*pY,*pZ1,*pZ2);
//	// 	p413App->Log(msg);
//
//	return TRUE;
//}

// ]

// [ 06112024/MORSALIN 
BOOL CSiBase::devGetPosAll(float* pX, float* pY, float* pZ1, float* pZ2, float* pZ3) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	// Regular implementation [10/1/2012 Yuen]
	C413Global* g = &p413App->Global;
	if (pX) *pX = moAxis[g->X_Axis - 1].fPos;
	if (pY) *pY = moAxis[g->Y_Axis - 1].fPos;
	if (pZ1) *pZ1 = moAxis[g->EchoTop_Axis - 1].fPos;
	if (pZ2) *pZ2 = moAxis[g->RoughZ_Axis - 1].fPos;
	if (pZ3) *pZ3 = moAxis[g->TopCWLZ_Axis - 1].fPos;  //06112024/MORSALIN

	// 	CString msg;
	// 	msg.Format("SiB:devGetPosAll: X=%.4f  Y=%.4f  Z1=%.4f  Z2=%.4f",*pX,*pY,*pZ1,*pZ2);
	// 	p413App->Log(msg);

	return TRUE;
}
// ]

BOOL CSiBase::_WaitStop(int nAxis, long tout, BOOL& bPos) {
	return TRUE;
}

// Return FALSE if function time out or failed to detect positively that all axis had stopped  [10/1/2012 Yuen]
BOOL CSiBase::_WaitAllStop(int tout, BOOL& bPos) {
	return TRUE;
}

BOOL CSiBase::devMoveRXY(float iX, float iY) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	C413Global* g = &p413App->Global;
	moAxis[g->X_Axis - 1].fPos += iX;
	moAxis[g->Y_Axis - 1].fPos += iY;

#ifdef PAUSEPGM
	//SleepEx(300);
#endif

	// Do not need to wait stop [8/5/2013 Yuen]
	return TRUE;
}

float CSiBase::fGetFFUPressNow() {
	// simulation [4/28/2013 Administrator]
	return rand() / float(RAND_MAX);
}

BOOL CSiBase::Reset(short MotorID, float fSteps) {
	return TRUE;
}

BOOL CSiBase::ResetXY() {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	C413Global* g = &p413App->Global;
	moAxis[g->X_Axis - 1].fPos = 0;
	moAxis[g->Y_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::ResetEchoTopZ() {
	C413Global* g = &p413App->Global;
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = 0;
		return TRUE;
	}
	moAxis[g->EchoTop_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::ResetRoughZ() {
	C413Global* g = &p413App->Global;
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = 0;
		return TRUE;
	}
	moAxis[g->RoughZ_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::ResetX() {
	C413Global* g = &p413App->Global;
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = 0;
		return TRUE;
	}
	moAxis[g->X_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::ResetY() {
	C413Global* g = &p413App->Global;
	if (ID == SIMOTOR_NOHUB) {
		moAxis[0].fPos = 0;
		return TRUE;
	}
	moAxis[g->Y_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::ResetAll() {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	C413Global* g = &p413App->Global;
	moAxis[g->X_Axis - 1].fPos = 0;
	moAxis[g->Y_Axis - 1].fPos = 0;
	moAxis[g->EchoTop_Axis - 1].fPos = 0;
	moAxis[g->RoughZ_Axis - 1].fPos = 0;
	return TRUE;
}

BOOL CSiBase::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	p413App->Global.bChuckVac = bOnOff;
	return TRUE;
}

// 20240814/Masum

BOOL CSiBase::SetFFU(BOOL bOnOff)
{
	p413App->Global.bSetFFU = bOnOff;
	return TRUE;
}

BOOL CSiBase::SetMicroscopeLight(BOOL bOnOff) {
	p413App->Global.bEchoLight = bOnOff;
	return TRUE;
}

BOOL CSiBase::SetSpectroSourceLight(BOOL bOnOff) {
	p413App->Global.bSpecSource = bOnOff;
	return TRUE;
}

BOOL CSiBase::SetSpectroIllumLight(BOOL bOnOff) {
	p413App->Global.bSpectLight = bOnOff;
	return TRUE;
}

BOOL CSiBase::SetSpectrometerPower(BOOL bOnOff) {
	p413App->Global.bSpecSource = bOnOff;
	return TRUE;
}

BOOL CSiBase::devMoveAXY(float iX, float iY) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	C413Global* g = &p413App->Global;
	moAxis[g->X_Axis - 1].fPos = iX;
	moAxis[g->Y_Axis - 1].fPos = iY;

#ifdef PAUSEPGM
	//SleepEx(300);
#endif
	return TRUE;
}

BOOL CSiBase::devMoveAXYZ1(float iX, float iY, float iZ1) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return TRUE;
	}
	C413Global* g = &p413App->Global;
	moAxis[g->X_Axis - 1].fPos = iX;
	moAxis[g->Y_Axis - 1].fPos = iY;
	moAxis[g->EchoTop_Axis - 1].fPos = iZ1;

#ifdef PAUSEPGM
	SleepEx(300);
#endif
	return TRUE;
}

CSiBase::AXISID CSiBase::GetAxisNo(int nAxis) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return XAXIS;
	}
	// emulating galil controller [10/1/2012 Yuen]
	C413Global* g = &p413App->Global;
	AXISID a;
	if (nAxis == g->X_Axis) {
		a = XAXIS;
	}
	else if (nAxis == g->Y_Axis) {
		a = YAXIS;
	}
	else if (nAxis == g->EchoTop_Axis) {
		a = ECHOTOPZAXIS;
	}
	else if (nAxis == g->RoughZ_Axis) {
		a = ROUGHZAXIS;
	}
	else {
		a = NOAXIS;
	}
	return a;
}

short CSiBase::GetMotorID(CSiBase::AXISID a) {
	if (ID == SIMOTOR_NOHUB) {
		ASSERT(0);
		return 0;
	}
	C413Global* g = &p413App->Global;
	if (a == XAXIS) {
		return g->X_Axis;
	}
	if (a == YAXIS) {
		return g->Y_Axis;
	}
	if (a == ECHOTOPZAXIS) {
		return g->EchoTop_Axis;
	}
	if (a == ROUGHZAXIS) {
		return g->RoughZ_Axis;
	}
	return NOAXIS;
}

BOOL CSiBase::_WaitForXY(int tout, BOOL& bPos) {
	return TRUE;
}

BOOL CSiBase::IsWaferPresent() {
	return FALSE;
}

// return 0,1 or 2 [10/1/2012 Yuen]
short CSiBase::GetValue(CString& cs) {
	if (cs.GetLength() != 1) {
		return 2;
	}
	char ch = cs.GetAt(0);
	if (ch == '0') {
		return 0;
	}
	else if (ch == '1') {
		return 1;
	}
	return 2;
}

// return 0 or 1 only [6/23/2013 Yuen]
long CSiBase::CS2Value(CString cs) {
	CString str = cs.Mid(cs.Find('=') + 1);
	return atol(str);
}

void CSiBase::SleepEx(DWORD dly) {
	Sleep(dly);
	return;
	ULONGLONG now = GetTickCount64();
	while ((GetTickCount64() - now) < dly) {
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			AfxGetThread()->PumpMessage();
		}
	}
}

float CSiBase::GetCurPos(short MotorID) {
	if (ID == SIMOTOR_NOHUB) {
		return moAxis[0].fPos;;
	}
	return moAxis[MotorID - 1].fPos;
}

void CSiBase::ReconnectTCP() {}

void CSiBase::WaitConnect() {}