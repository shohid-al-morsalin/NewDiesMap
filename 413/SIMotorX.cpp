// SiMotor.cpp: implementation of the CSiMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiMotor.h"

#include "413App.h"

#define		SI_COMPORT			(1)
#define		DELIMITOR			('\r')
#define		MOTOR_ERROR_BASE	(0x10000)

#define		MIN_SPEED			(0.025f)
#define		MAX_SPEED			(50.0f)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char chRevisionLevel[] = "RV";
const char chRequestStatus[] = "RS";
const char chMicrostepResolution[] = "MR";
const char chAcknowledgement[] = "00";
const char chStop[] = "ST";
const char chDistance[] = "DI";
const char chFeedToLength[] = "FL";
const char chFeedToPos[] = "FP";
const char chSetAbsPos[] = "SP";
const char chVelocity[] = "VE";
const char chJogSpeed[] = "JS";
//const char chImmediatePosition[] = "IP";
//const char chImmediatePosition[] = "SP";
const char chPowerUpCurrent[] = "PC";
const char chChangeCurrent[] = "CC";
const char chSetAcceleration[] = "AC";
const char chSetDeceleration[] = "DE";
const char chSetOutput[] = "SO";
const char chInputStatus[] = "IS";
const char chDefineLimits[] = "DL";
const char chInputHigh[] = "1";
const char chJogStart[] = "CJ";
const char chJogStop[] = "SJ";
const char chAnalogInput[] = "RA";

static char* findnext(char* psz) {
	char* p = strchr(psz, DELIMITOR);
	if (p) {
		if (*(++p) == NULL) return NULL;
	}
	return p;
}

CSiMotor::CSiMotor() {
	bSimu = FALSE;
	ID = SIMOTOR;
	IDStr = "SI Motor";
	m_pif = new CSerialICC;
}

CSiMotor::~CSiMotor() {
	if (m_pif) {
		delete m_pif;
		m_pif = NULL;
	}
}

// Send the command through the communication channel [5/29/2012 Yuen]
BOOL CSiMotor::Send(const char* psz) {
	char szCommand[100];

	sprintf(szCommand, "%s%c", psz, DELIMITOR);
	if (m_pif->SendData(szCommand, strlen(szCommand))) {
		::Sleep(30);	// give a time to process a command
		return TRUE;
	}
	return FALSE;
}

// Read a string from communication channel [5/29/2012 Yuen]
UINT CSiMotor::Receive(UINT nSize, char* chBuffer) {
	ZeroMemory(chBuffer, nSize);
	UINT nRead = m_pif->ReadData(chBuffer, nSize);
	if (nRead == nSize) nRead--;
	chBuffer[nRead] = NULL;
	return nRead;
}

BOOL CSiMotor::IsValidAxis(int nAxis) {
	if ((nAxis < 1) || (nAxis >= NO_HUBPORT)) return FALSE;
	if (moAxis[nAxis - 1].bmpStatus == 0) return FALSE;
	return TRUE;
}

BOOL CSiMotor::SetSpeed(int nAxis, float fSpeed) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	if (fSpeed < MIN_SPEED) fSpeed = MIN_SPEED;
	if (fSpeed > MAX_SPEED) fSpeed = MAX_SPEED;

	sprintf(chBuffer, "%d%s%.3f", nAxis, chVelocity, fSpeed);
	return Send(chBuffer);
}

BOOL CSiMotor::SetDistance(int nAxis, int nDistance) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%d%s%i%c", nAxis, chDistance, nDistance, DELIMITOR);
	return SendLim(chBuffer);
}

BOOL CSiMotor::FeedToLength(int nAxis) {
	//	if (!bMovement) { return TRUE; }
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%d%s%c", nAxis, chFeedToLength, DELIMITOR);
	return SendLim(chBuffer);
}

BOOL CSiMotor::Stop(int nAxis) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%d%s", nAxis, chStop);
	return Send(chBuffer);
}

// BOOL CSiMotor::GetXYPosition(long *plX, long *plY)
// {
// 	if (IsValidAxis(p413App->Global.X_Axis) && IsValidAxis(p413App->Global.Y_Axis))
// 	{
// 		if (plX) *plX = moAxis[p413App->Global.X_Axis-1].nPosition;
// 		if (plY) *plY = moAxis[p413App->Global.Y_Axis-1].nPosition;
// 		return TRUE;
// 	}
// 	return FALSE;
// }

BOOL CSiMotor::SetCurrent(int nAxis, float fCurrent) {
	if (!IsValidAxis(nAxis)) return FALSE;
	char chBuffer[100];
	sprintf(chBuffer, "%d%s%.3f", nAxis, chChangeCurrent, fCurrent);
	Send(chBuffer);
	return TRUE;
}

BOOL CSiMotor::IsMotorMoving(int nAxis) {
	char chBuffer[100];
	sprintf(chBuffer, "%dRS", nAxis);
	if (!Send(chBuffer)) return TRUE;
	Sleep(10);
	int nInput = Receive(sizeof(chBuffer), chBuffer);
	if (nInput <= 0) return TRUE;
	if (strchr(chBuffer, 'R') != NULL) {
		return FALSE;
	}
	return TRUE;
}

BOOL CSiMotor::devGetCurPos(int nAxis, float* pfPos) {
	CString str;
	char chBuffer[100];
	str.Format("%dIP", nAxis);
	if (!Send(str)) return FALSE;
	Sleep(20);
	if (!Receive(sizeof(chBuffer), chBuffer)) return FALSE;

	long pos;
	sscanf(chBuffer + 4, "%x", &pos);
	moAxis[nAxis - 1].fPos = *pfPos = pos;  // Update SW tracking [6/2/2011 XPMUser]
	return TRUE;
}

BOOL CSiMotor::devMoveA(int nAxis, float lPos) {
	// move to absolute pos
	if (!SetDistance(nAxis, lPos)) {
		return FALSE;
	}
	if (!FeedToPosition(nAxis)) {
		return FALSE;
	}
	return CSiBase::devMoveA(nAxis, lPos);
}

BOOL CSiMotor::devMoveR(int nAxis, float lDist) {
	// move relative
	if (!SetDistance(nAxis, lDist)) {
		return FALSE;
	}
	if (!FeedToLength(nAxis)) {
		return FALSE;
	}
	return CSiBase::devMoveR(nAxis, lDist);
}

BOOL CSiMotor::FeedToPosition(int nAxis) {
	//	if (!bMovement) { return TRUE; }
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}

	char chBuffer[100];

	sprintf(chBuffer, "%d%s", nAxis, chFeedToPos);
	return Send(chBuffer);
}

BOOL CSiMotor::devSetAbsPos(int nAxis, float lPos) {
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}
	char chBuffer[100];
	sprintf(chBuffer, "%d%s%.0f", nAxis, chSetAbsPos, lPos);
	return Send(chBuffer);
}

BOOL CSiMotor::SetAcceleration(int nAxis, float fAcc) {
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}

	char chBuffer[100];

	sprintf(chBuffer, "%d%s%.2f", nAxis, chSetAcceleration, fAcc);
	if (!Send(chBuffer)) {
		return FALSE;
	}
	sprintf(chBuffer, "%d%s%.2f", nAxis, chSetDeceleration, fAcc);

	if (!Send(chBuffer)) {
		return FALSE;
	}
	sprintf(chBuffer, "%dJA%.3f", nAxis, fAcc);

	return Send(chBuffer);
}

BOOL CSiMotor::StopAll() {
	SendLim("ST\r");
	return TRUE;
}

BOOL CSiMotor::SetOutput(int nAxis, int nChan, BOOL bOnOff) {
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}
	if (nChan > 3) {
		return FALSE;
	}
	if (nChan < 1) {
		return FALSE;
	}

	char chBuffer[100];

	sprintf(chBuffer, "%d%s%d%s", nAxis, chSetOutput, nChan, bOnOff ? "H" : "L");
	return Send(chBuffer);
}

BOOL CSiMotor::GetInput(int nAxis, int nChan) {
	if (!IsValidAxis(nAxis)) return FALSE;
	char chBuffer[100];
	if (nChan < 1) return FALSE;
	if (nChan > 8) return FALSE;
	sprintf(chBuffer, "%d%s", nAxis, chInputStatus);
	if (!Send(chBuffer)) return 0;
	Sleep(250);
	if (Receive(sizeof(chBuffer), chBuffer) == 12) {
		return (*(chBuffer + 12 - nChan) == '1');
	}
	return FALSE;
}

float CSiMotor::GetAnalogInput(int nAxis) {
	if (!IsValidAxis(nAxis)) return 0;
	char chBuffer[100];
	sprintf(chBuffer, "%d%s", nAxis, chAnalogInput);
	if (!Send(chBuffer)) return 0;
	int n = Receive(sizeof(chBuffer), chBuffer);
	if ((n >= 8) && (n <= 9)) {
		for (int i = 0; i < 4; i++) {
			chBuffer[i] = chBuffer[i + 4];
		}
		chBuffer[4] = 0;
		float fP = atof(chBuffer);
		return fP;
	}
	else {
		sprintf(chBuffer, "%d%s", nAxis, chAnalogInput);
		if (!Send(chBuffer)) return 0;
		n = Receive(sizeof(chBuffer), chBuffer);
		if ((n >= 8) && (n <= 9)) {
			for (int i = 0; i < 4; i++) {
				chBuffer[i] = chBuffer[i + 4];
			}
			chBuffer[4] = 0;
			float fP = atof(chBuffer);
			return fP;
		}
		else
			return 0;
	}
}

BOOL CSiMotor::DefineLimits(int nAxis, int nData) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%d%s%d", nAxis, chDefineLimits, nData);
	return Send(chBuffer);
}

BOOL CSiMotor::SendLim(char* buf) {
	if (m_pif->SendData(buf, strlen(buf))) {
		::Sleep(5);	// give it some time to process the command
		return TRUE;
	}
	return FALSE;
}

BOOL CSiMotor::OpenPort(short PortNo) {
	return m_pif->Open(PortNo);
}

BOOL CSiMotor::ClosePort() {
	return m_pif->Close();
}

BOOL CSiMotor::SetMicroRes(int nAxis, short MicroRes) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%dMR%d", nAxis, MicroRes);
	return Send(chBuffer);
}

BOOL CSiMotor::Reset(short MotorID, float fSteps) {
	// fSteps must be negative at start [8/19/2011 XPMUser]
	//if (fSteps > 0)
	//{
	//	fSteps *= -1;
	//}
	return devMoveR(MotorID, fSteps);
}

// do not lock [7/16/2013 Yuen]
BOOL CSiMotor::_WaitStop(int nAxis, long tout, int& bPos) {
	if (!IsValidAxis(nAxis)) return FALSE;

	ULONGLONG tick = GetTickCount64();

	while ((GetTickCount64() - tick) < 90000) {
		if (!IsMotorMoving(nAxis)) {
			return TRUE;
		}
		Sleep(100);
	}

	p413App->Log("WaitStop Timeout!", 2);
	return FALSE;
}