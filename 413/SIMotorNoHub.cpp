// SiMotor.cpp: implementation of the CSiMotorNoHub class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiMotorNoHub.h"

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

CSiMotorNoHub::CSiMotorNoHub() {
	bSimu = FALSE;
	ID = SIMOTOR_NOHUB;
	IDStr = "SI Motor No Hub";
	m_pif = new CSerialICC;
}

CSiMotorNoHub::~CSiMotorNoHub() {
	if (m_pif) {
		delete m_pif;
		m_pif = NULL;
	}
}

// Send the command through the communication channel [5/29/2012 Yuen]
BOOL CSiMotorNoHub::Send(const char* psz) {
	char szCommand[100];

	sprintf(szCommand, "%s%c", psz, DELIMITOR);
	if (m_pif->SendData(szCommand, strlen(szCommand))) {
		::Sleep(30);	// give a time to process a command
		return TRUE;
	}
	return FALSE;
}

// Read a string from communication channel [5/29/2012 Yuen]
UINT CSiMotorNoHub::Receive(UINT nSize, char* chBuffer) {
	ZeroMemory(chBuffer, nSize);
	UINT nRead = m_pif->ReadData(chBuffer, nSize);
	if (nRead == nSize) nRead--;
	chBuffer[nRead] = NULL;
	return nRead;
}

BOOL CSiMotorNoHub::IsValidAxis(int nAxis) {
	if (nAxis != 0) return FALSE;
	if (moAxis[0].bmpStatus == 0) return FALSE;
	return TRUE;
}

BOOL CSiMotorNoHub::SetSpeed(int nAxis, float fSpeed) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	if (fSpeed < MIN_SPEED) fSpeed = MIN_SPEED;
	if (fSpeed > MAX_SPEED) fSpeed = MAX_SPEED;

	sprintf(chBuffer, "%s%.3f", chVelocity, fSpeed);
	return Send(chBuffer);
}

BOOL CSiMotorNoHub::SetDistance(int nAxis, int nDistance) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%s%i%c", chDistance, nDistance, DELIMITOR);
	return SendLim(chBuffer);
}

BOOL CSiMotorNoHub::FeedToLength(int nAxis) {
	//	if (!bMovement) { return TRUE; }
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%s%c", chFeedToLength, DELIMITOR);
	return SendLim(chBuffer);
}

BOOL CSiMotorNoHub::Stop(int nAxis) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%s", chStop);
	return Send(chBuffer);
}

BOOL CSiMotorNoHub::SetCurrent(int nAxis, float fCurrent) {
	if (!IsValidAxis(nAxis)) return FALSE;
	char chBuffer[100];
	sprintf(chBuffer, "%s%.3f", chChangeCurrent, fCurrent);
	Send(chBuffer);
	return TRUE;
}

BOOL CSiMotorNoHub::IsMotorMoving(int nAxis) {
	char chBuffer[100];
	sprintf(chBuffer, "RS");
	if (!Send(chBuffer)) return TRUE;
	Sleep(10);
	int nInput = Receive(sizeof(chBuffer), chBuffer);
	if (nInput <= 0) return TRUE;
	if (strchr(chBuffer, 'R') != NULL) {
		return FALSE;
	}
	return TRUE;
}

BOOL CSiMotorNoHub::devGetCurPos(int nAxis, float* pfPos) {
	CString str;
	char chBuffer[100];
	str.Format("IP");
	if (!Send(str)) return FALSE;
	Sleep(20);
	if (!Receive(sizeof(chBuffer), chBuffer)) return FALSE;

	long pos;
	sscanf(chBuffer + 3, "%x", &pos);
	moAxis[0].fPos = *pfPos = pos;  // Update SW tracking [6/2/2011 XPMUser]
	return TRUE;
}

BOOL CSiMotorNoHub::devMoveA(int nAxis, float lPos) {
	// move to absolute pos
	if (!SetDistance(nAxis, lPos)) {
		return FALSE;
	}
	if (!FeedToPosition(nAxis)) {
		return FALSE;
	}
	return CSiBase::devMoveA(nAxis, lPos);
}

BOOL CSiMotorNoHub::devMoveR(int nAxis, float lDist) {
	// move relative
	if (!SetDistance(nAxis, lDist)) {
		return FALSE;
	}
	if (!FeedToLength(nAxis)) {
		return FALSE;
	}
	return CSiBase::devMoveR(nAxis, lDist);
}

BOOL CSiMotorNoHub::FeedToPosition(int nAxis) {
	//	if (!bMovement) { return TRUE; }
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}

	char chBuffer[100];

	sprintf(chBuffer, "%s", chFeedToPos);
	return Send(chBuffer);
}

BOOL CSiMotorNoHub::devSetAbsPos(int nAxis, float lPos) {
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}
	char chBuffer[100];
	sprintf(chBuffer, "%s%.0f", chSetAbsPos, lPos);
	return Send(chBuffer);
}

BOOL CSiMotorNoHub::SetAcceleration(int nAxis, float fAcc) {
	if (!IsValidAxis(nAxis)) {
		return FALSE;
	}

	char chBuffer[100];

	sprintf(chBuffer, "%s%.2f", chSetAcceleration, fAcc);

	if (!Send(chBuffer)) {
		return FALSE;
	}

	sprintf(chBuffer, "%s%.2f", chSetDeceleration, fAcc);

	if (!Send(chBuffer)) {
		return FALSE;
	}

	sprintf(chBuffer, "JA%.3f", fAcc);

	return Send(chBuffer);
}

BOOL CSiMotorNoHub::StopAll() {
	SendLim("ST\r");
	return TRUE;
}

BOOL CSiMotorNoHub::SetOutput(int nAxis, int nChan, BOOL bOnOff) {
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

	sprintf(chBuffer, "%s%d%s", chSetOutput, nChan, bOnOff ? "H" : "L");

	return Send(chBuffer);
}

BOOL CSiMotorNoHub::GetInput(int nAxis, int nChan) {
	if (!IsValidAxis(nAxis)) return FALSE;
	char chBuffer[100];
	if (nChan < 1) return FALSE;
	if (nChan > 8) return FALSE;
	sprintf(chBuffer, "%s", chInputStatus);
	if (!Send(chBuffer)) return 0;
	Sleep(250);
	if (Receive(sizeof(chBuffer), chBuffer) == 11) {
		return (*(chBuffer + 11 - nChan) == '1');
	}
	return FALSE;
}

float CSiMotorNoHub::GetAnalogInput(int nAxis) {
	if (!IsValidAxis(nAxis)) return 0;
	char chBuffer[100];
	sprintf(chBuffer, "%s", chAnalogInput);

	if (!Send(chBuffer)) return 0;
	int n = Receive(sizeof(chBuffer), chBuffer);
	if ((n >= 7) && (n <= 8)) {
		for (int i = 0; i < 4; i++) {
			chBuffer[i] = chBuffer[i + 4];
		}
		chBuffer[4] = 0;
		float fP = atof(chBuffer);
		return fP;
	}
	else {
		sprintf(chBuffer, "%s", chAnalogInput);
		if (!Send(chBuffer)) return 0;
		n = Receive(sizeof(chBuffer), chBuffer);
		if ((n >= 7) && (n <= 8)) {
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

BOOL CSiMotorNoHub::DefineLimits(int nAxis, int nData) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "%s%d", chDefineLimits, nData);

	return Send(chBuffer);
}

BOOL CSiMotorNoHub::SendLim(char* buf) {
	if (m_pif->SendData(buf, strlen(buf))) {
		::Sleep(5);	// give it some time to process the command
		return TRUE;
	}
	return FALSE;
}

BOOL CSiMotorNoHub::OpenPort(short PortNo) {
	return m_pif->Open(PortNo);
}

BOOL CSiMotorNoHub::ClosePort() {
	return m_pif->Close();
}

BOOL CSiMotorNoHub::SetMicroRes(int nAxis, short MicroRes) {
	if (!IsValidAxis(nAxis)) return FALSE;

	char chBuffer[100];

	sprintf(chBuffer, "MR%d", MicroRes);

	return Send(chBuffer);
}

BOOL CSiMotorNoHub::Reset(short MotorID, float fSteps) {
	// fSteps must be negative at start [8/19/2011 XPMUser]
	//if (fSteps > 0)
	//{
	//	fSteps *= -1;
	//}
	return devMoveR(MotorID, fSteps);
}

// do not lock [7/16/2013 Yuen]
BOOL CSiMotorNoHub::_WaitStop(int nAxis, long tout, BOOL& bPos) {
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