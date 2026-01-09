// MotorParam.h: interface for the CMotorParam class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "TxR.h"

class CMotorParam {
public:
	enum PORESET {
		poZERO, poMAXPOS, poHOMEPOS, poMINPOS, poNEGHOMEPOS
	};

	BOOL bPolarity;
	float scaledmy;
	double Scale;
	float MotorCurrent;
	float Speed, InitSpeed;
	float Acceleration;
	float JogSpeed;
	short MicroReso;					// micro step resolution [7/8/2010 128 NT]
	// Echo probe position [6/14/2012 FSM 413 C2C]
	float MaxPos1, MinPos1, HomePos1;	// in real coordinates, HomePos1 for Echo [8/19/2011 XPMUser]
	CTxR TxR;							// Coordinate transformation [6/15/2012 FSM 413 C2C]
	float Offset;						// Offset from reset position in real coordinate, only use in reset, gotozero alway goto home position [8/19/2011 XPMUser]
	// Thin Film probe position [6/14/2012 FSM 413 C2C]
	float MaxPos2, MinPos2;				// in real coordinates [8/19/2011 XPMUser]
	float HomePos2;						// Offset from reset position in real coordinate, HomePos2 for Thin Film, only use in reset, GotoZero alway goto home position [8/19/2011 XPMUser]
	BOOL bResetDirection;				// Either 1 or -1 [8/19/2011 XPMUser]
	short ResetPos;						// Reset position [9/16/2011 C2C]

	CMotorParam();
	virtual ~CMotorParam();
	virtual void Serialize(CArchive& ar);
	CMotorParam& operator =(CMotorParam& si);
};
