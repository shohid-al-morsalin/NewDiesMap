// GLMotor.cpp: implementation of the CGLMotor_v1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "413App.h"
#include "GLMotor_v1.h"
#include "..\SRC\Tokenizer.h"
#include "GlobalDeviceContainer.h"
#include "..\IPC\TelnetClientSocket.h"
#include "..\IPC\Lock.h"
#include "FFUThread.h"

#ifdef DEV_GCLIB
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"
#pragma comment(lib, "../413/gclibo.lib")
#endif

#define DELIMITER	':'
#define RETURN "\r"

char AxN_v1[4] = { 'A', 'B', 'C', 'D' };
char axN_v1[4] = { 'a', 'b', 'c', 'd' };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGLMotor_v1* pGLM = NULL;	// Local use only [6/3/2012 Administrator]

CGLMotor_v1::CGLMotor_v1() {
	pGLM = this;
	pLockExec = new CDMLock("GLMotorExecLock");

	ID = GLMOTOR;
	IDStr = "GL Motor";

	g1 = NULL;
	g2 = NULL;
}

CGLMotor_v1::~CGLMotor_v1() {
	if (pLockExec) {
		delete pLockExec;
		pLockExec = NULL;
	}

	ClosePort();
	pGLM = NULL;
}

// [5/27/2020]
BOOL CGLMotor_v1::IsDoorInterlockTriggered() {
	char res[2];

	CString comd = ("XQ #CIL,7\r");

	CString rspd = _SendR(comd.GetBuffer(0));

	// MG @IN2, "DOOR"
	// " #DOOR\r\n"
	while (rspd.GetLength() != 8) {
		comd = ("XQ #CIL,7\r");
		rspd = _SendR(comd.GetBuffer(0));
		SleepEx(2000);
	}

	SleepEx(1000);
	res[0] = rspd.GetAt(1);
	return atoi(res);
}

// Stop the axis from moving [5/29/2012 Yuen]
BOOL CGLMotor_v1::Stop(int nAxis) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	cmd.Format("~a=%d;ST~a", a);

	return _SendN(cmd.GetBuffer(0));
}

BOOL CGLMotor_v1::devGetXYPos(float* pfX, float* pfY, DWORD tout) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < tout) {
		cmd = "XQ #GPXY,5";
		rsp = _SendR(cmd);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[4];
			for (int j = 0; j < 4; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GPXY and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 5) || (cs[0] != "#GPXY")) {
				MsgStr.Format("devGetXYPos: receive '%s' instead of '#GPXY'", cs[0]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[3].GetLength() != 3) || (cs[3] != "@\r\n")) {
				MsgStr.Format("devGetXYPos: receive '%s' instead of '@\r\n'", cs[3]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			*pfX = atof(cs[1]);
			*pfY = atof(cs[2]);
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("devGetXYPos fail", 2);
	return FALSE;
}

BOOL CGLMotor_v1::setEncoderPos(int nAxis, int pos) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd.Format("DEA=%d", pos);
		break;
	case YAXIS:
		cmd.Format("DEB=%d", pos);
		break;
	default:
		return FALSE;
		break;
	}
	return _SendN(cmd);
}

// Servo on the motor [6/5/2012 user]
BOOL CGLMotor_v1::SetCurrent(int nAxis, float fCurrent) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd = "SHA";
		break;
	case YAXIS:
		cmd = "SHB";
		break;
	case ECHOTOPZAXIS:
		cmd = "SHD";
		break;
	case ROUGHZAXIS:
		cmd = "SHC";
		break;
	default:
		return FALSE;
		break;
	}
	return _SendN(cmd);
}

// Make sure no axis is moving before this function is called [6/15/2013 Yuen]
BOOL CGLMotor_v1::devGetCurPos(int nAxis, float* pfPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;	// Invalid axis. Force software to hang [10/1/2012 Yuen]
	}

	if (a == AXISID::ECHOTOPZAXIS) {
		a = AXISID::ROUGHZAXIS;
	}
	else if (a == AXISID::ROUGHZAXIS) {
		a = AXISID::ECHOTOPZAXIS;
	}
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 10000) {
		cmd.Format("~a=%d;XQ #GPA,5", a);
		rsp = _SendR(cmd);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[3];
			for (int j = 0; j < 3; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GPA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#GPA")) {
				SleepEx(100);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[2].GetLength() != 3) || (cs[2] != "@\r\n")) {
				SleepEx(100);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			*pfPos = atof(cs[1]);
			moAxis[nAxis - 1].fPos = *pfPos;
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("[ME] GetCurrentPos: Unexpected response from DMC", 2);
	return FALSE;
}

BOOL CGLMotor_v1::devMoveA(int nAxis, float lPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	cmd.Format("V%c=%.0f;XQ #MA%c,%d", axN_v1[a], lPos, AxN_v1[a], a + 1);
	if (_SendN(cmd)) {
		BOOL ret = CSiBase::devMoveA(nAxis, lPos);
		return ret;
	}
	return FALSE;
}

BOOL CGLMotor_v1::devMoveR(int nAxis, float lDist) {
	short a = GetAxisNo(nAxis);
	if (a == -1) {
		return FALSE;
	}
	cmd.Format("V%c=%.0f;XQ #MR%c,%d", axN_v1[a], lDist, AxN_v1[a], a + 1);
	if (_SendN(cmd)) {
		BOOL ret = CSiBase::devMoveR(nAxis, lDist);
		return ret;
	}
	return FALSE;
}

BOOL CGLMotor_v1::devSetAbsPos(int nAxis, float lPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a != NOAXIS) {
		long pos = lPos / 6.103143119f;
		cmd.Format("~a=%d;DP~a=%ld", a, pos);
		if (!_SendN(cmd)) return FALSE;
		SleepEx(100);
		cmd.Format("~a=%d;DE~a=%.0f", a, lPos);
		if (_SendN(cmd)) {
			return CSiBase::devSetAbsPos(nAxis, lPos);
		}
	}
	return FALSE;
}

BOOL CGLMotor_v1::StopAll() {
	cmd = "ST;MO";
	return _SendN(cmd);
}

BOOL CGLMotor_v1::Initialize() {
	C413Global* g = &p413App->Global;
	cmd.Format("AC %d,%d,%d,%d", moAxis[g->X_Axis - 1].Acc, moAxis[g->Y_Axis - 1].Acc, moAxis[g->EchoTop_Axis - 1].Acc, moAxis[g->RoughZ_Axis - 1].Acc);
	if (!_SendN(cmd)) {
		return FALSE;
	}

	SleepEx(100);
	cmd.Format("DC %d,%d,%d,%d", moAxis[g->X_Axis - 1].Acc, moAxis[g->Y_Axis - 1].Acc, moAxis[g->EchoTop_Axis - 1].Acc, moAxis[g->RoughZ_Axis - 1].Acc);
	if (!_SendN(cmd)) {
		return FALSE;
	}

	SleepEx(100);
	cmd.Format("SP %d,%d,%d,%d", moAxis[g->X_Axis - 1].Spd, moAxis[g->Y_Axis - 1].Spd, moAxis[g->EchoTop_Axis - 1].Spd, moAxis[g->RoughZ_Axis - 1].Spd);
	if (!_SendN(cmd)) {
		return FALSE;
	}
	return TRUE;
}

BOOL CGLMotor_v1::devGetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 10000) {
		cmd = "XQ #GP,5";
		rsp = _SendR(cmd);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[6];
			for (int j = 0; j < 6; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GP and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 3) || (cs[0] != "#GP")) {
				MsgStr.Format("devGetPosAll: Receive %s instead of '#GP'", cs[0]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 3) || (cs[5] != "@\r\n")) {
				MsgStr.Format("devGetPosAll: Receive %s instead of '@\r\n'", cs[5]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			C413Global* g = &p413App->Global;
			*pfX = atof(cs[1]);
			moAxis[g->X_Axis - 1].fPos = *pfX;
			*pfY = atof(cs[2]);
			moAxis[g->Y_Axis - 1].fPos = *pfY;
			*pfZ2 = atof(cs[3]);
			moAxis[g->RoughZ_Axis - 1].fPos = *pfZ2;
			*pfZ1 = atof(cs[4]);
			moAxis[g->EchoTop_Axis - 1].fPos = *pfZ1;
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("devGetPosAll: timeout", 2);
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CGLMotor_v1::_WaitAllStop(int tout, BOOL& bPos) {
	ULONGLONG tick = GetTickCount64();
	while (int(GetTickCount64() - tick) < tout) {
		CString command = "XQ #IMA,5";
		rsp = _SendR(command);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			cs[10] = "";
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				MsgStr.Format("_WaitAllStop: receive '%s' instead of '#IMA'", cs[0]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 3) || (cs[5] != "@\r\n")) {
				MsgStr.Format("_WaitAllStop: receive '%s' instead of '@\r\n'", cs[5]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}

			// Message format is now verified [10/1/2012 Yuen]
			if (CS2Value(cs[1])) {
				SleepEx(50);
				continue;
			}
			if (CS2Value(cs[2])) {
				SleepEx(50);
				continue;
			}
			if (CS2Value(cs[3])) {
				SleepEx(50);
				continue;
			}
			if (CS2Value(cs[4])) {
				SleepEx(50);
				continue;
			}
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				//bPos = TRUE;	// Temporary disable this feature [7/18/2013 Yuen]
			}
			return TRUE;
		}
		else {
			SleepEx(500);
		}
	}
	p413App->Log("_WaitAllStop Timeout!", 2);
	return FALSE;
}

BOOL CGLMotor_v1::SetSpeed(int nAxis, float fSpeed) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd.Format("SPA=%f.0", fSpeed);
		break;
	case YAXIS:
		cmd.Format("SPB=%f.0", fSpeed);
		break;
	case ECHOTOPZAXIS:
		cmd.Format("SPD=%f.0", fSpeed);
		break;
	case ROUGHZAXIS:
		cmd.Format("SPC=%f.0", fSpeed);
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return _SendN(cmd);
}

// Relative move [6/22/2012 FSM413]
BOOL CGLMotor_v1::devMoveRXY(float iX, float iY) {
	if ((!iX) && (!iY)) {
		// all zero, no move need [6/25/2012 FSM413]
		return TRUE;
	}

	cmd.Format("Va=%.0f;Vb=%.0f;XQ #MRXY,1", iX, iY);
	if (_SendN(cmd)) {
		return CSiBase::devMoveRXY(iX, iY);
	}
	MsgStr.Format("MoveAXY: devMoveRXY(%.3f,%.3f) fail", iX, iY);
	p413App->Log(MsgStr, 2);
	return FALSE;
}

// This function must return in split seconds or the rest of the motion system will stall [6/14/2013 Yuen]
// Return -1 to indicate no reading and previous reading should be used [6/14/2013 Yuen]
float CGLMotor_v1::fGetFFUPressNow() {
	static float curValue = 0;

	cmd.Format("XQ #AN3,7");
	rsp = _SendR(cmd);
	if (rsp.GetLength() == 0) {
		return curValue;
	}
	CTokenizer tok(rsp, " ");
	CString cs[3];
	for (int j = 0; j < 3; j++) {
		tok.Next(cs[j]);
	}
	// First token must be #AN and one character only [10/1/2012 Yuen]
	if ((cs[0].GetLength() != 3) || (cs[0] != "#AN")) {
		return curValue;
	}
	// Last token must be @ and one character only [10/1/2012 Yuen]
	if ((cs[2].GetLength() != 3) || (cs[2] != "@\r\n")) {
		return curValue;
	}
	// Message format is now verified [10/1/2012 Yuen]
	FFUPressNow = atof(cs[1]);
	return FFUPressNow;
}

BOOL CGLMotor_v1::ResetXY() {
	BOOL bPos = FALSE;
	cmd = "XQ #RSX,1";
	if (_SendN(cmd)) {
		cmd = "XQ #RSY,2";
		if (_SendN(cmd)) {
			SleepEx(50);
			if (_WaitForXY(120000, bPos)) {
				if (bPos) {
					return TRUE;
				}
				return CSiBase::ResetXY();
			}
		}
	}
	_WaitForXY(120000, bPos);
	return FALSE;
}

BOOL CGLMotor_v1::ResetEchoTopZ() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSZ1,3";
	if (_SendN(cmd)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(ECHOTOPZAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetEchoTopZ();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor_v1::ResetRoughZ() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSZ2,3";
	if (_SendN(cmd)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(ROUGHZAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetRoughZ();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor_v1::ResetX() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSX,1";
	if (_SendN(cmd)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(XAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetX();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor_v1::ResetY() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSY,2";
	if (_SendN(cmd)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(YAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetY();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor_v1::ResetAll() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSX,1";
	if (_SendN(cmd)) {
		cmd = "XQ #RSY,2";
		if (_SendN(cmd)) {
			cmd = "XQ #RSZ1,3";
			if (_SendN(cmd)) {
				cmd = "XQ #RSZ2,4";
				if (_SendN(cmd)) {
					SleepEx(50);
					if (_WaitAllStop(180000, bPos)) {
						pLockExec->Unlock();
						if (bPos) return TRUE;
						return CSiBase::ResetAll();
					}
				}
			}
		}
	}
	_WaitAllStop(120000, bPos);
	pLockExec->Unlock();
	p413App->Log("Reset timeout", 2);
	return FALSE;
}

BOOL CGLMotor_v1::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	p413App->Global.bChuckVac = bOnOff;
	if (bOnOff) {
		if (nWaferDiameter == 200) {
			cmd = "SB 3";
		}
		else {
			cmd = "SB 3; SB 2";
		}
		return _SendN(cmd);
	}
	else {
		cmd = "CB 3;CB 2";
		return _SendN(cmd);
	}
	return FALSE;
}

BOOL CGLMotor_v1::SetMicroscopeLight(BOOL bOnOff) {
	if (bOnOff) {
		cmd = "SB 1";
	}
	else {
		cmd = "CB 1";
	}
	if (_SendN(cmd)) {
		p413App->Global.bEchoLight = bOnOff;	// For simulation only [6/16/2013 Yuen]
		return TRUE;
	}
	return FALSE;
}

BOOL CGLMotor_v1::SetSpectroSourceLight(BOOL bOnOff) {
	p413App->Global.bSpecSource = bOnOff;
	if (bOnOff) {
		cmd = "SB 8";
	}
	else {
		cmd = "CB 8";
	}
	return _SendN(cmd);
}

BOOL CGLMotor_v1::SetSpectroIllumLight(BOOL bOnOff) {
	if (bOnOff) {
		cmd = "SB 7";
	}
	else {
		cmd = "CB 7";
	}
	if (_SendN(cmd)) {
		p413App->Global.bSpectLight = bOnOff;	// For simulation only [6/16/2013 Yuen]
		return TRUE;
	}
	return FALSE;
}

BOOL CGLMotor_v1::SetSpectrometerPower(BOOL bOnOff) {
	p413App->Global.bSpecPower = bOnOff;
	if (bOnOff) {
		cmd = "SB 6";
	}
	else {
		cmd = "CB 6";
	}
	return _SendN(cmd);
}

// Non blocking [3/30/2013 Administrator]
BOOL CGLMotor_v1::devMoveAXY(float iX, float iY) {
	cmd.Format("Va=%.0f;Vb=%.0f;XQ #MAXY,1", iX, iY);
	if (_SendN(cmd)) {
		return CSiBase::devMoveAXY(iX, iY);
	}
	MsgStr.Format("devMoveAXY(%.3f,%.3f) fail", iX, iY);
	p413App->Log(MsgStr, 2);
	return FALSE;
}

// Non blocking [3/30/2013 Administrator]
BOOL CGLMotor_v1::devMoveAXYZ1(float iX, float iY, float iZ1) {
	ASSERT(0); // to be implemented [6/15/2013 Yuen]
	cmd.Format("Va=%.0f;Vb=%.0f;Vc=%.0f;XQ #MAXYZ1,1", iX, iY, iZ1);
	if (_SendN(cmd)) {
		return CSiBase::devMoveAXYZ1(iX, iY, iZ1);
	}
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CGLMotor_v1::_WaitForXY(int tout, BOOL& bPos) {
	ULONGLONG tick = GetTickCount64();
	while (int(GetTickCount64() - tick) < tout) {
		CString command = "XQ #IMA,5";
		rsp = _SendR(command);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and 4 characters only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				SleepEx(100);
				continue;
			}
			// Last token must be @ and 3 characters only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 3) || (cs[5] != "@\r\n")) {
				SleepEx(100);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			if (CS2Value(cs[1])) {
				SleepEx(100);
				continue;
			}
			if (CS2Value(cs[2])) {
				SleepEx(100);
				continue;
			}
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				bPos = TRUE;
			}
			return TRUE;
		}
	}
	p413App->Log("WaitForXY: Timeout", 2);
	return FALSE;
}

BOOL CGLMotor_v1::IsWaferPresent() {
	cmd.Format("XQ #IN1,7");
	rsp = _SendR(cmd);
	if (rsp.GetLength() > 0) {
		CTokenizer tok(rsp, " ");
		CString cs[3];
		for (int j = 0; j < 3; j++) {
			tok.Next(cs[j]);
		}
		// First token must be #IN and 3 characters only [10/1/2012 Yuen]
		if ((cs[0].GetLength() != 3) || (cs[0] != "#IN")) {
			return FALSE;
		}
		// Last token must be @ and 3 characters only [10/1/2012 Yuen]
		if ((cs[2].GetLength() != 3) || (cs[2] != "@\r\n")) {
			return FALSE;
		}
		// Message format is now verified [10/1/2012 Yuen]
		return (BOOL)atoi(cs[1]);
	}
	p413App->Log("[ME] IsWaferPresent: Unexpected response from DMC", 2);
	return TRUE; // False positive intentionally [10/2/2012 FSM413]
}

// do not lock [7/16/2013 Yuen]
BOOL CGLMotor_v1::_WaitStop(int nAxis, long tout, BOOL& bPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	ULONGLONG tick = GetTickCount64();
	while (int(GetTickCount64() - tick) < tout) {
		CString command = "XQ #IMA,5";
		rsp = _SendR(command);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and 3 characters only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				SleepEx(100);
				continue;
			}
			// Last token must be @ and 3 characters only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 3) || (cs[5] != "@\r\n")) {
				SleepEx(100);
				continue;
			}

			// Message format is now verified [10/1/2012 Yuen]
			switch (a) {
			case XAXIS:
				if (CS2Value(cs[1])) {
					SleepEx(100);
					continue;
				}
				break;
			case YAXIS:
				if (CS2Value(cs[2])) {
					SleepEx(100);
					continue;
				}
				break;
			case ECHOTOPZAXIS:
				if (CS2Value(cs[4])) {
					SleepEx(100);
					continue;
				}
				break;
			case ROUGHZAXIS:
				if (CS2Value(cs[3])) {
					SleepEx(100);
					continue;
				}
				break;
			}
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				bPos = TRUE;
			}
			return TRUE;
		}
		else SleepEx(500);
	}
	p413App->Log("WaitStop Timeout!", 2);

	return FALSE;
}

void CGLMotor_v1::SleepEx(DWORD tout) {
	Sleep(tout);
	return;

	ULONGLONG tm = GetTickCount64();
	while ((GetTickCount64() - tm) < tout) {
		MSG msg;
		while (PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

// [6/16/2020]
inline void CGLMotor_v1::e(GReturn error) {
	if (error != G_NO_ERROR)
		throw error;
}

// g can be either g1 or g2
int CGLMotor_v1::find_avilable_handle_number(GCon g) {
	char buf[G_SMALL_BUFFER];
	int number_of_available = 0;

	GCommand(g, "TH", buf, G_SMALL_BUFFER, NULL);

	CTokenizer t((CString)buf, RETURN);

	CString str;
	t.Next(str);
	while (t.Next(str)) {
		if (str.Find("AVAILABLE") != -1)
			number_of_available += 1;
	}

	return number_of_available;
}

BOOL CGLMotor_v1::clear_handle() {
	_SendN("IHT=>-3");
	SleepEx(500);
	_SendN("IHS=>-3");
	ClosePort();
	p413App->Log("Closed all handle");
	return TRUE;
}

BOOL CGLMotor_v1::check_handle() {
	int number_of_available = find_avilable_handle_number(g1);
	if (number_of_available < 4) {
		return TRUE;
	}
	return FALSE;
}

GReturn CGLMotor_v1::reissue_handle1() {
	char buf[G_SMALL_BUFFER];
	CString str;

	try {
		str.Format("%s -d", p413App->IPAddress);
		e(GOpen(str.GetBuffer(0), &g1));
		TRACE("New Galil 1 Connected");
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		str.Format("New Galil 1 Connect failed: %s", buf);
		TRACE(str);
		return error;
	}

	return G_NO_ERROR;
}

BOOL CGLMotor_v1::reissue_handle2() {
	char buf[G_SMALL_BUFFER];
	CString str;

	try {
		str.Format("%s -d -s ALL", p413App->IPAddress);
		e(GOpen(str.GetBuffer(0), &g2));
		TRACE("New Galil 2 Connected");
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		str.Format("New Galil 2 Connect failed: %s", buf);
		TRACE(str);
		return error;
	}

	return G_NO_ERROR;
}

// Method 2 uses gclib
BOOL CGLMotor_v1::OpenPort(short portNo) {
	char buf[G_SMALL_BUFFER];
	BOOL bSucceed = TRUE;
	CString str;

	// Connect first galil handle
	try {
		str.Format("%s -d", p413App->IPAddress);
		e(GOpen(str.GetBuffer(0), &g1));
		p413App->Log("Galil 1 Connected");

		Initialize();
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		str.Format("Galil 1 Connect failed: %s", buf);
		p413App->Log(str);
		bSucceed = FALSE;
	}

	// Connect second galil handle
	try {
		str.Format("%s -d -s ALL", p413App->IPAddress);
		e(GOpen(str.GetBuffer(0), &g2));
		p413App->Log("Galil 2 Connected");
		_SendR("CF I");
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		str.Format("Galil 2 Connect failed: %s", buf);
		p413App->Log(str);
		bSucceed = FALSE;
	}

	return bSucceed;
}

BOOL CGLMotor_v1::ClosePort() {
	if (g1) GClose(g1);
	if (g2) GClose(g2);
	return TRUE;
}

BOOL CGLMotor_v1::_SendN(const char* command) {
	pLockExec->Lock();
	BOOL bSucceed = TRUE;
	char buf[G_SMALL_BUFFER];

	if (!g1) return FALSE;

	try {
		cmd.Format("%s", command);
		GCommand(g1, cmd.GetBuffer(0), buf, G_SMALL_BUFFER, NULL);
		pLockExec->Unlock();
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		CString str;
		str.Format("SendN (error): %s", buf);
		p413App->Log(str);
		if (!check_handle()) {
			char buf[G_SMALL_BUFFER];
			try {
				e(reissue_handle1());
				SleepEx(500);
				p413App->Log("Handle 1 issued successfully");
				e(reissue_handle2());
				SleepEx(500);
				p413App->Log("Handle 2 issued successfully");
			}
			catch (GReturn error) {
				GError(error, buf, G_SMALL_BUFFER);
				str.Format("Reissue handle error: %s", buf);
				p413App->Log(str);
			}
		}
		else return FALSE;

		if (check_handle()) {
			p413App->Log("Send command again");
			_SendN(command);
		}

		TRACE(str);
		pLockExec->Unlock();
		bSucceed = FALSE;
	}
	return bSucceed;
}

CString CGLMotor_v1::_SendR(const char* command) {
	pLockExec->Lock();

	char buf[G_SMALL_BUFFER];

	if (!g2) return "";

	try {
		cmd.Format("%s", command);
		GTimeout(g2, 5000);
		GCommand(g2, cmd.GetBuffer(0), buf, G_SMALL_BUFFER, NULL);
		TRACE("S %s\n", command);
		SleepEx(10);
		GTimeout(g2, 100);
		e(GMessage(g2, buf, G_SMALL_BUFFER));
		TRACE("S %s\n", buf);
		pLockExec->Unlock();
		return _T(buf);
	}
	catch (GReturn error) {
		GError(error, buf, G_SMALL_BUFFER);
		CString str;
		str.Format("SendR (error): %s", buf);
		p413App->Log(str);
		if (!check_handle()) {
			char buf[G_SMALL_BUFFER];
			CString str;
			try {
				e(reissue_handle1());
				SleepEx(500);
				p413App->Log("Handle 1 issued successfully");
				e(reissue_handle2());
				SleepEx(500);
				p413App->Log("Handle 2 issued successfully");
			}
			catch (GReturn error) {
				GError(error, buf, G_SMALL_BUFFER);
				str.Format("Reissue handle error: %s", buf);
				p413App->Log(str);
			}
		}
		else return "";

		if (check_handle()) {
			p413App->Log("Send command again");
			return _SendR(command);
		}
		TRACE(str);
		pLockExec->Unlock();
		return "";
	}
}