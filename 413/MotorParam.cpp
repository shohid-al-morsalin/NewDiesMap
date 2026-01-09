// MotorParam.cpp: implementation of the CMotorParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MotorParam.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// IMPLEMENT_SERIAL (CMotorParam, CObject, 1)

CMotorParam::CMotorParam() {
	bPolarity = 0;
	Scale = scaledmy = 2100;
	MotorCurrent = 0.8f;
	Speed = InitSpeed = 5;
	Acceleration = 25;
	JogSpeed = 5;
	MaxPos1 = 150, MinPos1 = -150;
	HomePos1 = 0;
	MaxPos2 = 150, MinPos2 = -150;
	HomePos2 = 0;
	MicroReso = 8;
	Offset = 0;
	bResetDirection = 0;
	ResetPos = 0;
	TxR.Set(0, 0, 0);
}

CMotorParam::~CMotorParam() {}

CMotorParam& CMotorParam::operator =(CMotorParam& si) {
	HomePos1 = si.HomePos1;
	MaxPos1 = si.MaxPos1;
	MinPos1 = si.MinPos1;
	HomePos2 = si.HomePos2;
	MaxPos2 = si.MaxPos2;
	MinPos2 = si.MinPos2;
	Scale = si.Scale;
	Acceleration = si.Acceleration;
	InitSpeed = si.InitSpeed;
	Speed = si.Speed;
	MotorCurrent = si.MotorCurrent;
	bPolarity = si.bPolarity;
	JogSpeed = si.JogSpeed;
	MicroReso = si.MicroReso;
	Offset = si.Offset;
	bResetDirection = si.bResetDirection;
	ResetPos = si.ResetPos;
	TxR.Tr = si.TxR.Tr;
	TxR.Tx = si.TxR.Tx;
	TxR.Ty = si.TxR.Ty;
	return *this;
}

void CMotorParam::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 7;
		ar << magic;
		ar << HomePos1;
		ar << MaxPos1;
		ar << MinPos1;
		ar << scaledmy;
		ar << Acceleration;
		ar << InitSpeed;
		ar << Speed;
		ar << MotorCurrent;
		ar << MicroReso;
		ar << Offset;
		ar << bResetDirection;
		ar << bPolarity;
		ar << ResetPos;
		ar << HomePos2;
		ar << MaxPos2;
		ar << MinPos2;
		ar << TxR.Tr;
		ar << TxR.Tx;
		ar << TxR.Ty;
		ar << Scale;
	}
	else {
		ar >> magic;
		ar >> HomePos1;
		ar >> MaxPos1;
		ar >> MinPos1;
		ar >> scaledmy;
		if (magic < 7) {
			Scale = scaledmy;
		}
		ar >> Acceleration;
		ar >> InitSpeed;
		ar >> Speed;
		ar >> MotorCurrent;
		ar >> MicroReso;
		ar >> Offset;
		if (magic > 1) {
			ar >> bResetDirection;
		}
		if (magic > 2) {
			ar >> bPolarity;
		}
		if (magic > 3) {
			ar >> ResetPos;
		}
		if (magic > 4) {
			ar >> HomePos2;
			ar >> MaxPos2;
			ar >> MinPos2;
		}
		if (magic > 5) {
			ar >> TxR.Tr;
			ar >> TxR.Tx;
			ar >> TxR.Ty;
		}
		if (magic > 6) {
			ar >> Scale;
		}
	}
}