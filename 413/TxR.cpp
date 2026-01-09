// TxR.cpp: implementation of the CTxR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "TxR.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxR::CTxR() {
	Set(0, 0, 0);
}

CTxR::~CTxR() {}

void CTxR::Set(float X, float Y, float A) {
	Tx = X; Ty = Y;
	Tr = /*0.017453293 **/ A;
	sinA = sin(Tr);
	cosA = cos(Tr);
}

// forward transformation [10/24/2012 Yuen]
void CTxR::Tran(float& X, float& Y)		// Transform from logical space to device space [10/24/2012 Yuen]
{
	float aX = X;
	float aY = Y;
	X = aX * cosA - aY * sinA + Tx;
	Y = aY * cosA + aX * sinA + Ty;
}

// reverse transformation [10/24/2012 Yuen]
void CTxR::RTran(float& X, float& Y)	// Transform from device space to logical space [10/24/2012 Yuen]
{
	X -= Tx;
	Y -= Ty;

	float aX = X;
	float aY = Y;
	X = -aX * cosA + aY * sinA;
	Y = -aY * cosA - aX * sinA;
}