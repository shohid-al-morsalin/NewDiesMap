// MotorL.cpp: implementation of the CMotorL2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MotorL2.h"
#include "SIBase.h"
#include "..\SRC\DOSUtil.h"
#include "413App.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

float CMotorL2::Real2Dev(float pos) {
	return (pos * Param.Scale);
}

float CMotorL2::Dev2Real(float pos) {
	return (pos / Param.Scale);
}

BOOL CMotorL2::MoveR(float dist) {
	if (!pSiHub) return FALSE;
	m_StopRequested = FALSE;

	if (dist > Param.MaxPos1 || dist < Param.MinPos1) {
		CString msg; msg.Format("%d Limit exceeds (R)", MotorID);
		p413App->Log(msg, 2);
		return FALSE;
	}

	float fpos = dist * Param.Scale;
	return pSiHub->devMoveR(MotorID, Param.bPolarity ? fpos : -fpos);
}

BOOL CMotorL2::Move(float dist) {
	if (!pSiHub) return FALSE;
	m_StopRequested = FALSE;

	if (dist > Param.MaxPos1 || dist < Param.MinPos1) {
		CString msg; msg.Format("%d Limit exceeds (A)", MotorID);
		p413App->Log(msg, 2);
		return FALSE;
	}

	float fpos = dist * Param.Scale;
	return pSiHub->devMoveA(MotorID, Param.bPolarity ? fpos : -fpos);
}

void CMotorL2::Set(CMotorParam* par) {
	Param = *par;
}

BOOL CMotorL2::Reset(float fSteps, float speed) {
	if (pSiHub) {
		vSetSpeed(speed);
		return pSiHub->Reset(MotorID, (long)Real2Dev(Param.bResetDirection ? fSteps : -fSteps));
	}
	return FALSE;
}

BOOL CMotorL2::GetStatus(int nChan) {
	if (pSiHub) {
		return pSiHub->GetInput(MotorID, nChan);
	}
	else {
		return FALSE;
	}
}

BOOL CMotorL2::SetOutput(int nChan, BOOL bOnOff) {
	if (pSiHub) {
		return pSiHub->SetOutput(MotorID, nChan, bOnOff);
	}
	else {
		return FALSE;
	}
}

float CMotorL2::GetAnalogInput() {
	return 0;
}

CMotorL2::CMotorL2() {
	m_StopRequested = FALSE;

	MotorID = 0;
	pSiHub = NULL;
	curPos = 0;
	Res = 0.050f;

	Param.Scale = 1.0;
	Param.Speed = 1;
	Param.Acceleration = 4;
	Param.InitSpeed = 100;
	Param.bPolarity = TRUE;
}

CMotorL2::CMotorL2(int iAxis, CSiBase* SiMc) {
	m_StopRequested = FALSE;

	MotorID = iAxis;
	pSiHub = SiMc;
	curPos = 0;
	Res = 0.050f;

	Param.Scale = 1.0;
	Param.Speed = 1;
	Param.Acceleration = 4;
	Param.InitSpeed = 100;
	Param.bPolarity = TRUE;
}

CMotorL2::~CMotorL2() {}

int CMotorL2::iGetErrorCode() {
	return 0;
}

BOOL CMotorL2::fReadPos(float& fPos) {
	if (!pSiHub) return FALSE;
	float m_Pos;
	if (pSiHub->devGetCurPos(MotorID, &m_Pos)) {
		fPos = fGetPos(m_Pos);
		return TRUE;
	}
	return FALSE;
}

void CMotorL2::vSetAcceleration(float fAcc) {
	Param.Acceleration = fAcc;
	SetAcc(fAcc);
}

void CMotorL2::SetAcc(float fAcc) {
	if (pSiHub) {
		pSiHub->SetAcceleration(MotorID, fAcc);
	}
}

void CMotorL2::vSetSpeed(float fSpeed) {
	Param.Speed = fSpeed;
	if (pSiHub) {
		pSiHub->SetSpeed(MotorID, Param.Speed);
	}
}

void CMotorL2::vSetJogSpeed(float fSpeed) {
	Param.JogSpeed = fSpeed;
}

void CMotorL2::vSetInitSpeed(float InitSpeed) {
	Param.InitSpeed = InitSpeed;
}

void CMotorL2::vSetPos(float fPos) {
	float m_Pos = Real2Dev(fPos);
	if (pSiHub) {
		pSiHub->devSetAbsPos(MotorID, m_Pos);
	}
}

void CMotorL2::vSetScale(double fScale) {
	Param.Scale = fScale;
}

void CMotorL2::setEncoderPos(int pos) {
	if (pSiHub) {
		pSiHub->setEncoderPos(MotorID, pos);
	}
}

// Wait for motor retrieve final position [6/15/2013 Yuen]
BOOL CMotorL2::WaitForMotor(int tout) {
	BOOL bPos = FALSE;
	if (!pSiHub) return TRUE;
	if (pSiHub->_WaitStop(MotorID, tout, bPos)) {
		if (!bPos) {
			bGetPos(&curPos);
		}
		else {
			curPos = fGetPos(long(pSiHub->GetCurPos(MotorID)));
		}
		return TRUE;
	}
	return FALSE;
}

// BOOL CMotorL2::WaitForAllMotor(int tout, int MIDX, int MIDY, int MIDZ1, int MIDZ2)
// {
// 	if (!pSiHub) return TRUE;
// 	Sleep(50);  // Wait till Hub ready to accept next command [9/17/2010 C2C]
// 	int tick = GetTickCount();
// 	while (pSiHub->IsMotorMoving(MIDX,MIDY,MIDZ1,MIDZ2))
// 	{
// 		MSG message;
// 		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
// 		{
// 			TranslateMessage (&message);  // yield to other process while waiting for the call back function
// 			DispatchMessage (&message);
// 		}
// 		Sleep(10);  //  [5/19/2010 Valued Customer]
// 		if ((GetTickCount()-tick) > tout)
// 		{
// 			return FALSE;
// 		}
// 	}
// 	return TRUE;
// }

void CMotorL2::EnableMotor(BOOL bOnOff) {}

void CMotorL2::vSetMotorCurrent(float Current) {
	Param.MotorCurrent = Current;
	if (pSiHub) {
		pSiHub->SetCurrent(MotorID, Param.MotorCurrent);
	}
}

void CMotorL2::vSetMicroRes(short val) {
	Param.MicroReso = val;
	if (pSiHub) {
		pSiHub->SetMicroRes(MotorID, Param.MicroReso);
	}
}

BOOL CMotorL2::Init(CListBox* pMessage) {
	if (pMessage) {
		CString str;
		str.Format("Axis %d initialized (simu)", MotorID);
		pMessage->InsertString(pMessage->GetCount(), str);
	}
	return TRUE;
}

void CMotorL2::SetNormalSpeed() {
	vSetSpeed(Param.Speed);
}

void CMotorL2::SetProperties() {
	float m_Pos;  // Make this variable local [7/7/2010 128 NT]
	switch (Param.ResetPos) {
	case CMotorParam::poZERO:
		m_Pos = 0;
		break;	// use 0 as the reset pos
	case CMotorParam::poMAXPOS:
		m_Pos = Param.MaxPos1;
		break;	// use max pos as the reset pos
	case CMotorParam::poHOMEPOS:
		m_Pos = Param.HomePos1;    // For MOT [8/12/2011 FSMT]
		break;	// use home pos as the reset pos
	case CMotorParam::poMINPOS:
		m_Pos = Param.MinPos1;
		break;	// use min as the reset pos
	case CMotorParam::poNEGHOMEPOS:
		m_Pos = -Param.HomePos1;  // -ve = C2C, +ve = MOT
		//homePos = 0;
		break;	// use home pos as reset pos and having a neg offset
	default:
		m_Pos = -Param.HomePos1;
		ASSERT(0);
		break;
	}
	vSetPos(m_Pos + Param.Offset);
	pSiHub->moAxis[MotorID].fPos = Real2Dev(m_Pos);
}

BOOL CMotorL2::SetSpeed(float fSpeed) {
	if (pSiHub) {
		return pSiHub->SetSpeed(MotorID, fSpeed);
	}
	else {
		return FALSE;
	}
}

BOOL CMotorL2::bSetCurrent(float fCurrent) {
	return TRUE;
}

void CMotorL2::vSetParameter(char* szStr) {
	vSetAcceleration(Param.Acceleration);

	vSetSpeed(Param.Speed);

	vSetJogSpeed(Param.JogSpeed);

	vSetScale(Param.Scale);

	vSetMotorCurrent(Param.MotorCurrent);
}

void CMotorL2::vGetParameter(char* szStr) {
	char szVal[40];
	float fData;

	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	GetPrivateProfileString(szStr, "Acceleration", "25", szVal, sizeof(szVal), inifile);
	fData = (float)atof(szVal);
	Param.Acceleration = fData;
	vSetAcceleration(Param.Acceleration);

	GetPrivateProfileString(szStr, "Speed", "5", szVal, sizeof(szVal), inifile);
	fData = (float)atof(szVal);
	Param.Speed = fData;
	vSetSpeed(Param.Speed);

	GetPrivateProfileString(szStr, "JogSpeed", "5", szVal, sizeof(szVal), inifile);
	fData = (float)atof(szVal);
	Param.JogSpeed = fData;
	vSetJogSpeed(Param.JogSpeed);

	GetPrivateProfileString(szStr, "Scale", "2100.0", szVal, sizeof(szVal), inifile);
	double v = atof(szVal);
	Param.Scale = v/*fData*/;
	vSetScale(Param.Scale);

	GetPrivateProfileString(szStr, "MicroRes", "8", szVal, sizeof(szVal), inifile);
	Param.MicroReso = atoi(szVal);
	vSetMicroRes(Param.MicroReso);

	GetPrivateProfileString(szStr, "Current", "1.0", szVal, sizeof(szVal), inifile);
	fData = (float)atof(szVal);
	Param.MotorCurrent = fData;
	vSetMotorCurrent(Param.MotorCurrent);

	Param.bPolarity = (1 == GetPrivateProfileInt(szStr, "Polarity", 1, inifile));

	// what recorded in the ini were motor steps, to make it easer to set up, use mm then convert it here
	char  szDefault[40];
	sprintf(szDefault, "%.4f", Param.MinPos1);
	GetPrivateProfileString(szStr, "MinPos", szDefault, szVal, sizeof(szVal), inifile);
	Param.MinPos1 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.MaxPos1);
	GetPrivateProfileString(szStr, "MaxPos", szDefault, szVal, sizeof(szVal), inifile);
	Param.MaxPos1 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.HomePos1);
	GetPrivateProfileString(szStr, "HomePos", szDefault, szVal, sizeof(szVal), inifile);
	Param.HomePos1 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.MinPos2);
	GetPrivateProfileString(szStr, "MinPos2", szDefault, szVal, sizeof(szVal), inifile);
	Param.MinPos2 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.MaxPos2);
	GetPrivateProfileString(szStr, "MaxPos2", szDefault, szVal, sizeof(szVal), inifile);
	Param.MaxPos2 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.HomePos2);
	GetPrivateProfileString(szStr, "HomePos2", szDefault, szVal, sizeof(szVal), inifile);
	Param.HomePos2 = atof(szVal);

	sprintf(szDefault, "%.4f", Param.TxR.Tr);
	GetPrivateProfileString(szStr, "Angle", szDefault, szVal, sizeof(szVal), inifile);
	Param.TxR.Tr = atof(szVal);

	sprintf(szDefault, "%.4f", Param.TxR.Tx);
	GetPrivateProfileString(szStr, "Tx", szDefault, szVal, sizeof(szVal), inifile);
	Param.TxR.Tx = atof(szVal);

	sprintf(szDefault, "%.4f", Param.TxR.Ty);
	GetPrivateProfileString(szStr, "Ty", szDefault, szVal, sizeof(szVal), inifile);
	Param.TxR.Ty = atof(szVal);

	sprintf(szDefault, "%.4f", Param.Offset);
	GetPrivateProfileString(szStr, "Offset", szDefault, szVal, sizeof(szVal), inifile);
	Param.Offset = atof(szVal);

	sprintf(szDefault, "%d", Param.bResetDirection);
	GetPrivateProfileString(szStr, "ResetDirection", szDefault, szVal, sizeof(szVal), inifile);
	Param.bResetDirection = atoi(szVal);

	sprintf(szDefault, "%d", Param.ResetPos);
	GetPrivateProfileString(szStr, "ResetPosition", szDefault, szVal, sizeof(szVal), inifile);
	Param.ResetPos = atoi(szVal);

	sprintf(szDefault, "%.3f", Res);
	GetPrivateProfileString(szStr, "Accuracy", szDefault, szVal, sizeof(szVal), inifile);
	Res = atoi(szVal);
	if (Res < 0.001f) {
		Res = 0.001f;
	}
}

void CMotorL2::vSaveParameter(char* szStr) {
	CString Val;

	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	Val.Format("%.2f", Param.Acceleration);
	WritePrivateProfileString(szStr, "Acceleration", Val, inifile);

	Val.Format("%.2f", Param.Speed);
	WritePrivateProfileString(szStr, "Speed", Val, inifile);

	Val.Format("%.2f", Param.JogSpeed);
	WritePrivateProfileString(szStr, "JogSpeed", Val, inifile);

	Val.Format("%.4f", Param.Scale);
	WritePrivateProfileString(szStr, "Scale", Val, inifile);

	Val.Format("%.2f", Param.MotorCurrent);
	WritePrivateProfileString(szStr, "Current", Val, inifile);

	Val.Format("%d", Param.MicroReso);
	WritePrivateProfileString(szStr, "MicroRes", Val, inifile);

	Val.Format("%d", Param.bPolarity);
	WritePrivateProfileString(szStr, "Polarity", Val, inifile);

	Val.Format("%.4f", Param.HomePos1);
	WritePrivateProfileString(szStr, "HomePos", Val, inifile);

	Val.Format("%.4f", Param.MaxPos1);
	WritePrivateProfileString(szStr, "MaxPos", Val, inifile);

	Val.Format("%.4f", Param.MinPos1);
	WritePrivateProfileString(szStr, "MinPos", Val, inifile);

	Val.Format("%.4f", Param.HomePos2);
	WritePrivateProfileString(szStr, "HomePos2", Val, inifile);

	Val.Format("%.4f", Param.MaxPos2);
	WritePrivateProfileString(szStr, "MaxPos2", Val, inifile);

	Val.Format("%.4f", Param.MinPos2);
	WritePrivateProfileString(szStr, "MinPos2", Val, inifile);

	Val.Format("%.4f", Param.TxR.Tr);
	WritePrivateProfileString(szStr, "Angle", Val, inifile);

	Val.Format("%.4f", Param.TxR.Tx);
	WritePrivateProfileString(szStr, "Tx", Val, inifile);

	Val.Format("%.4f", Param.TxR.Ty);
	WritePrivateProfileString(szStr, "Ty", Val, inifile);

	Val.Format("%.4f", Param.Offset);
	WritePrivateProfileString(szStr, "Offset", Val, inifile);

	Val.Format("%d", Param.bResetDirection);
	WritePrivateProfileString(szStr, "ResetDirection", Val, inifile);

	Val.Format("%d", Param.ResetPos);
	WritePrivateProfileString(szStr, "ResetPosition", Val, inifile);

	Val.Format("%.3f", Res);
	WritePrivateProfileString(szStr, "Accuracy", Val, inifile);
}

BOOL CMotorL2::bGetPos(float* pfPos) {
	if (pSiHub) {
		float lPos;
		if (pSiHub->devGetCurPos(MotorID, &lPos)) {
			*pfPos = fGetPos(long(lPos));
			return TRUE;
		}
	}
	return FALSE;
}

// Merely convert dev to real [6/14/2013 Yuen]
float CMotorL2::fGetPos(long p) {
	float pos = Dev2Real(p);
	return Param.bPolarity ? pos : -pos;
}

short CMotorL2::GetCtrlType() {
	if (pSiHub) {
		return pSiHub->ID;
	}
	return CSiBase::SIMUMOTOR;
}

// BOOL CMotorL2::GetXYPos(float *X, float *Y)
// {
// 	if (pSiHub)
// 	{
// 		float lX,lY;
// 		if (pSiHub->devGetXYPos(&lX,&lY,10000))
// 		{
// 			*X = Dev2Real(lX);
// 			*Y = Dev2Real(lY);
// 			// Assume both X and Y axis has the same polarity [6/5/2012 Administrator]
// 			if (Param.bPolarity)
// 			{
// 				*X = -*X;
// 				*Y = -*Y;
// 			}
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

BOOL CMotorL2::IsSimu() {
	return pSiHub->bSimu;
}